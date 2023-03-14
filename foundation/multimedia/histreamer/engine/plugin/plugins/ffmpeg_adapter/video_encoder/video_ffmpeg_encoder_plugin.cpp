/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(RECORDER_SUPPORT) && defined(VIDEO_SUPPORT)

#define HST_LOG_TAG "FfmpegVideoEncoderPlugin"

#include "video_ffmpeg_encoder_plugin.h"
#include <cstring>
#include <map>
#include <set>
#include "plugin/common/plugin_caps_builder.h"
#include "plugin/common/plugin_time.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"
#include "ffmpeg_vid_enc_config.h"
#include "pipeline/core/plugin_attr_desc.h"

namespace {
// register plugins
using namespace OHOS::Media::Plugin;
using namespace Ffmpeg;
void UpdatePluginDefinition(const AVCodec* codec, CodecPluginDef& definition);

std::map<std::string, std::shared_ptr<const AVCodec>> codecMap;

const size_t BUFFER_QUEUE_SIZE = 6;
const size_t DEFAULT_ALIGN = 16;

std::set<AVCodecID> supportedCodec = {AV_CODEC_ID_H264};

Status RegisterVideoEncoderPlugins(const std::shared_ptr<Register>& reg)
{
    const AVCodec* codec = nullptr;
    void* iter = nullptr;
    MEDIA_LOG_I("registering video encoders");
    while ((codec = av_codec_iterate(&iter))) {
        if (!av_codec_is_encoder(codec) || codec->type != AVMEDIA_TYPE_VIDEO) {
            continue;
        }
        std::string iterCodec(codec->name);
        if (iterCodec.find("264") != std::string::npos && iterCodec != "libx264") {
            continue;
        }
        if (supportedCodec.find(codec->id) == supportedCodec.end()) {
            MEDIA_LOG_DD("codec %s(%s) is not supported right now", codec->name, codec->long_name);
            continue;
        }
        CodecPluginDef definition;
        definition.name = "video_encoder_" + std::string(codec->name);
        definition.codecType = CodecType::VIDEO_ENCODER;
        definition.rank = 100; // 100
        definition.creator = [](const std::string& name) -> std::shared_ptr<CodecPlugin> {
            return std::make_shared<VideoFfmpegEncoderPlugin>(name);
        };
        UpdatePluginDefinition(codec, definition);
        // do not delete the codec in the deleter
        codecMap[definition.name] = std::shared_ptr<AVCodec>(const_cast<AVCodec*>(codec), [](void* ptr) {});
        if (reg->AddPlugin(definition) != Status::OK) {
            MEDIA_LOG_W("register plugin %s(%s) failed", codec->name, codec->long_name);
        }
    }
    return Status::OK;
}

void UnRegisterVideoEncoderPlugins()
{
    codecMap.clear();
}

void UpdateInCaps(const AVCodec* codec, CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime(OHOS::Media::MEDIA_MIME_VIDEO_RAW);
    if (codec->pix_fmts != nullptr) {
        DiscreteCapability<VideoPixelFormat> values;
        for (uint32_t index = 0; codec->pix_fmts[index] != -1; ++index) {
            values.push_back(ConvertPixelFormatFromFFmpeg(codec->pix_fmts[index]));
        }
        if (!values.empty()) {
            capBuilder.SetVideoPixelFormatList(values);
        }
    } else {
        capBuilder.SetVideoPixelFormatList({VideoPixelFormat::YUV420P});
    }
    definition.inCaps.push_back(capBuilder.Build());
}

void UpdateOutCaps(const AVCodec* codec, CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime("video/unknown");
    switch (codec->id) {
        case AV_CODEC_ID_H264:
            capBuilder.SetMime(OHOS::Media::MEDIA_MIME_VIDEO_H264);
            break;
        default:
            MEDIA_LOG_I("codec is not supported right now");
            break;
    }
    definition.outCaps.push_back(capBuilder.Build());
}

void UpdatePluginDefinition(const AVCodec* codec, CodecPluginDef& definition)
{
    UpdateInCaps(codec, definition);
    UpdateOutCaps(codec, definition);
}
} // namespace

PLUGIN_DEFINITION(FFmpegVideoEncoders, LicenseType::LGPL, RegisterVideoEncoderPlugins, UnRegisterVideoEncoderPlugins);

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
VideoFfmpegEncoderPlugin::VideoFfmpegEncoderPlugin(std::string name)
    : CodecPlugin(std::move(name)), outBufferQ_("vencPluginQueue", BUFFER_QUEUE_SIZE)
{
}

Status VideoFfmpegEncoderPlugin::Init()
{
    OSAL::ScopedLock lock(avMutex_);
    auto iter = codecMap.find(pluginName_);
    FALSE_RETURN_V_MSG_E(iter != codecMap.end(), Status::ERROR_UNSUPPORTED_FORMAT,
                      "cannot find codec with name " PUBLIC_LOG_S, pluginName_.c_str());
    avCodec_ = iter->second;
    cachedFrame_ = std::shared_ptr<AVFrame>(av_frame_alloc(), [](AVFrame* fp) { av_frame_free(&fp); });
    cachedPacket_ = std::make_shared<AVPacket>();
    vencParams_[Tag::REQUIRED_OUT_BUFFER_CNT] = (uint32_t)BUFFER_QUEUE_SIZE;
    if (!encodeTask_) {
        encodeTask_ = std::make_shared<OHOS::Media::OSAL::Task>("videoFfmpegEncThread");
        encodeTask_->RegisterHandler([this] { ReceiveBuffer(); });
    }
    state_ = State::INITIALIZED;
    MEDIA_LOG_I("Init success");
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::Deinit()
{
    OSAL::ScopedLock l(avMutex_);
    avCodec_.reset();
    cachedFrame_.reset();
    cachedPacket_.reset();
    ResetLocked();
    if (encodeTask_) {
        encodeTask_->Stop();
        encodeTask_.reset();
    }
    state_ = State::DESTROYED;
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::SetParameter(Tag tag, const ValueType& value)
{
    OSAL::ScopedLock l(parameterMutex_);
    vencParams_.insert(std::make_pair(tag, value));
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::GetParameter(Tag tag, ValueType& value)
{
    {
        OSAL::ScopedLock l(parameterMutex_);
        auto res = vencParams_.find(tag);
        if (res != vencParams_.end()) {
            value = res->second;
            return Status::OK;
        }
    }
    OSAL::ScopedLock lock(avMutex_);
    FALSE_RETURN_V_MSG_E(avCodecContext_ != nullptr, Status::ERROR_WRONG_STATE, "codec context is null");
    return GetVideoEncoderParameters(*avCodecContext_, tag, value);
}

template <typename T>
void VideoFfmpegEncoderPlugin::FindInParameterMapThenAssignLocked(Tag tag, T& assign)
{
    auto iter = vencParams_.find(tag);
    if (iter != vencParams_.end() && typeid(T) == iter->second.Type()) {
        assign = Plugin::AnyCast<T>(iter->second);
    } else {
        MEDIA_LOG_W("parameter %d is not found or type mismatch", static_cast<int32_t>(tag));
    }
}

Status VideoFfmpegEncoderPlugin::CreateCodecContext()
{
    auto context = avcodec_alloc_context3(avCodec_.get());
    FALSE_RETURN_V_MSG_E(context != nullptr, Status::ERROR_UNKNOWN, "cannot allocate codec context");

    avCodecContext_ = std::shared_ptr<AVCodecContext>(context, [](AVCodecContext* ptr) {
        if (ptr != nullptr) {
            if (ptr->extradata) {
                av_free(ptr->extradata);
                ptr->extradata = nullptr;
            }
            avcodec_free_context(&ptr);
        }
    });
    MEDIA_LOG_I("Create ffmpeg codec context success");
    return Status::OK;
}

void VideoFfmpegEncoderPlugin::InitCodecContext()
{
    avCodecContext_->codec_type = AVMEDIA_TYPE_VIDEO;
    FindInParameterMapThenAssignLocked<std::uint32_t>(Tag::VIDEO_WIDTH, width_);
    FindInParameterMapThenAssignLocked<std::uint32_t>(Tag::VIDEO_HEIGHT, height_);
    FindInParameterMapThenAssignLocked<uint32_t>(Tag::VIDEO_FRAME_RATE, frameRate_);
    FindInParameterMapThenAssignLocked<Plugin::VideoPixelFormat>(Tag::VIDEO_PIXEL_FORMAT, pixelFormat_);
    MEDIA_LOG_D("width: " PUBLIC_LOG_U32 ", height: " PUBLIC_LOG_U32 ", pixelFormat: " PUBLIC_LOG_S ", frameRate_: "
                PUBLIC_LOG_U32, width_, height_, Pipeline::GetVideoPixelFormatNameStr(pixelFormat_), frameRate_);
    ConfigVideoEncoder(*avCodecContext_, vencParams_);
}

void VideoFfmpegEncoderPlugin::DeinitCodecContext()
{
    if (avCodecContext_ == nullptr) {
        return;
    }
    if (avCodecContext_->extradata) {
        av_free(avCodecContext_->extradata);
        avCodecContext_->extradata = nullptr;
    }
    avCodecContext_->extradata_size = 0;
    avCodecContext_->opaque = nullptr;
    avCodecContext_->width = 0;
    avCodecContext_->height = 0;
    avCodecContext_->time_base.den = 0;
    avCodecContext_->time_base.num = 0;
    avCodecContext_->ticks_per_frame = 0;
    avCodecContext_->sample_aspect_ratio.num = 0;
    avCodecContext_->sample_aspect_ratio.den = 0;
    avCodecContext_->get_buffer2 = nullptr;
}

Status VideoFfmpegEncoderPlugin::OpenCodecContext()
{
    AVCodec* venc = avcodec_find_encoder(avCodecContext_->codec_id);
    if (venc == nullptr) {
        MEDIA_LOG_E("Codec: " PUBLIC_LOG_D32 " is not found", static_cast<int32_t>(avCodecContext_->codec_id));
        DeinitCodecContext();
        return Status::ERROR_INVALID_PARAMETER;
    }
    auto res = avcodec_open2(avCodecContext_.get(), avCodec_.get(), nullptr);
    if (res != 0) {
        MEDIA_LOG_E("avcodec open error " PUBLIC_LOG_S " when start encoder ", AVStrError(res).c_str());
        DeinitCodecContext();
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_I("Open ffmpeg codec context success");
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::CloseCodecContext()
{
    Status ret = Status::OK;
    if (avCodecContext_ != nullptr) {
        auto res = avcodec_close(avCodecContext_.get());
        if (res != 0) {
            DeinitCodecContext();
            MEDIA_LOG_E("avcodec close error " PUBLIC_LOG_S " when stop encoder", AVStrError(res).c_str());
            ret = Status::ERROR_UNKNOWN;
        }
        avCodecContext_.reset();
    }
    return ret;
}

Status VideoFfmpegEncoderPlugin::Prepare()
{
    {
        OSAL::ScopedLock l(avMutex_);
        FALSE_RETURN_V(state_ == State::INITIALIZED || state_ == State::PREPARED, Status::ERROR_WRONG_STATE);
        FALSE_RETURN_V_MSG_E(CreateCodecContext() == Status::OK, Status::ERROR_UNKNOWN, "Create codec context fail");
        {
            OSAL::ScopedLock lock(parameterMutex_);
            InitCodecContext();
        }
#ifdef DUMP_RAW_DATA
        dumpFd_ = fopen("./enc_out.es", "wb");
#endif
        state_ = State::PREPARED;
    }
    outBufferQ_.SetActive(true);
    MEDIA_LOG_I("Prepare success");
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::ResetLocked()
{
    {
        OSAL::ScopedLock lock(parameterMutex_);
        vencParams_.clear();
    }
    avCodecContext_.reset();
    outBufferQ_.Clear();
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fclose(dumpFd_);
        dumpFd_ = nullptr;
    }
#endif
    state_ = State::INITIALIZED;
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::Reset()
{
    OSAL::ScopedLock l(avMutex_);
    return ResetLocked();
}

Status VideoFfmpegEncoderPlugin::Start()
{
    {
        OSAL::ScopedLock lock(avMutex_);
        FALSE_RETURN_V(state_ == State::PREPARED, Status::ERROR_WRONG_STATE);
        FALSE_RETURN_V_MSG_E(OpenCodecContext() == Status::OK, Status::ERROR_UNKNOWN, "Open codec context fail");
        state_ = State::RUNNING;
    }
    outBufferQ_.SetActive(true);
    encodeTask_->Start();
    MEDIA_LOG_I("Start success");
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::Stop()
{
    Status ret = Status::OK;
    {
        OSAL::ScopedLock lock(avMutex_);
        ret = CloseCodecContext();
#ifdef DUMP_RAW_DATA
        if (dumpFd_) {
            std::fclose(dumpFd_);
            dumpFd_ = nullptr;
        }
#endif
        state_ = State::INITIALIZED;
    }
    outBufferQ_.SetActive(false);
    encodeTask_->Stop();
    MEDIA_LOG_I("Stop success");
    return ret;
}

Status VideoFfmpegEncoderPlugin::QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue output buffer");
    if (outputBuffer) {
        outBufferQ_.Push(outputBuffer);
        return Status::OK;
    }
    return Status::ERROR_INVALID_PARAMETER;
}

Status VideoFfmpegEncoderPlugin::Flush()
{
    OSAL::ScopedLock l(avMutex_);
    if (avCodecContext_ != nullptr) {
        // flush avcodec buffers
    }
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue input buffer");
    FALSE_RETURN_V_MSG_E(!inputBuffer->IsEmpty() || (inputBuffer->flag & BUFFER_FLAG_EOS),
        Status::ERROR_INVALID_DATA, "encoder does not support fd buffer");
    Status ret = Status::OK;
    {
        OSAL::ScopedLock lock(avMutex_);
        ret = SendBufferLocked(inputBuffer);
    }
    NotifyInputBufferDone(inputBuffer);
    return ret;
}

Status VideoFfmpegEncoderPlugin::FillAvFrame(const std::shared_ptr<Buffer>& inputBuffer)
{
    const uint8_t *data = inputBuffer->GetMemory()->GetReadOnlyData();
    auto bufferMeta = inputBuffer->GetBufferMeta();
    FALSE_RETURN_V_MSG_W(bufferMeta != nullptr && bufferMeta->GetType() == BufferMetaType::VIDEO,
        Status::ERROR_INVALID_PARAMETER, "invalid buffer meta");
    std::shared_ptr<VideoBufferMeta> videoMeta = std::dynamic_pointer_cast<VideoBufferMeta>(bufferMeta);
    FALSE_RETURN_V_MSG_W(pixelFormat_ == videoMeta->videoPixelFormat, Status::ERROR_INVALID_PARAMETER,
        "pixel format change");
    cachedFrame_->format = ConvertPixelFormatToFFmpeg(videoMeta->videoPixelFormat);
    cachedFrame_->width = videoMeta->width;
    cachedFrame_->height = videoMeta->height;
    if (!videoMeta->stride.empty()) {
        for (uint32_t i = 0; i < videoMeta->planes; i++) {
            cachedFrame_->linesize[i] = static_cast<int32_t>(videoMeta->stride[i]);
        }
    }
    int32_t ySize = cachedFrame_->linesize[0] * AlignUp(cachedFrame_->height, DEFAULT_ALIGN);
    // AV_PIX_FMT_YUV420P: linesize[0] = linesize[1] * 2, AV_PIX_FMT_NV12: linesize[0] = linesize[1]
    int32_t uvSize = cachedFrame_->linesize[1] * AlignUp(cachedFrame_->height, DEFAULT_ALIGN) / 2; // 2
    if (cachedFrame_->format == AV_PIX_FMT_YUV420P) {
        cachedFrame_->data[0] = const_cast<uint8_t *>(data);
        cachedFrame_->data[1] = cachedFrame_->data[0] + ySize;
        cachedFrame_->data[2] = cachedFrame_->data[1] + uvSize; // 2: plane 2
    } else if ((cachedFrame_->format == AV_PIX_FMT_NV12) || (cachedFrame_->format == AV_PIX_FMT_NV21)) {
        cachedFrame_->data[0] = const_cast<uint8_t *>(data);
        cachedFrame_->data[1] = cachedFrame_->data[0] + ySize;
    } else {
        MEDIA_LOG_E("Unsupported pixel format: " PUBLIC_LOG_D32, cachedFrame_->format);
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    cachedFrame_->pts = ConvertTimeToFFmpeg(
        static_cast<uint64_t>(HstTime2Us(inputBuffer->pts)) / avCodecContext_->ticks_per_frame,
        avCodecContext_->time_base);
    MEDIA_LOG_D("hst pts: " PUBLIC_LOG_U64 " ns, ffmpeg pts: " PUBLIC_LOG_D64
                " us, den: " PUBLIC_LOG_D32 ", num: " PUBLIC_LOG_D32,
                inputBuffer->pts, cachedFrame_->pts, avCodecContext_->time_base.den, avCodecContext_->time_base.num);
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::SendBufferLocked(const std::shared_ptr<Buffer>& inputBuffer)
{
    FALSE_RETURN_V_MSG_E(state_ == State::RUNNING,
        Status::ERROR_WRONG_STATE, "queue input buffer in wrong state");
    bool isEos = false;
    if (inputBuffer == nullptr || (inputBuffer->flag & BUFFER_FLAG_EOS) != 0) {
        isEos = true;
    } else {
        auto res = FillAvFrame(inputBuffer);
        FALSE_RETURN_V(res == Status::OK, res);
    }
    AVFrame *frame = nullptr;
    if (!isEos) {
        frame = cachedFrame_.get();
    }
    auto ret = avcodec_send_frame(avCodecContext_.get(), frame);
    if (ret < 0) {
        MEDIA_LOG_D("send buffer error " PUBLIC_LOG_S, AVStrError(ret).c_str());
        return (ret == AVERROR_EOF) ? Status::END_OF_STREAM : Status::ERROR_NO_MEMORY;
    }
    if (frame) {
        av_frame_unref(cachedFrame_.get());
    }
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::FillFrameBuffer(const std::shared_ptr<Buffer>& packetBuffer)
{
    FALSE_RETURN_V_MSG_E(cachedPacket_->data != nullptr, Status::ERROR_UNKNOWN,
                         "avcodec_receive_packet() packet data is empty");
    auto frameBufferMem = packetBuffer->GetMemory();
    FALSE_RETURN_V_MSG_E(frameBufferMem->Write(cachedPacket_->data, cachedPacket_->size, 0) ==
                         static_cast<size_t>(cachedPacket_->size), Status::ERROR_UNKNOWN,
                         "copy packet data to buffer fail");
    if (cachedPacket_->flags & AV_PKT_FLAG_KEY) {
        MEDIA_LOG_D("It is key frame");
        packetBuffer->flag |= BUFFER_FLAG_KEY_FRAME;
    }
    packetBuffer->pts =
            static_cast<uint64_t>(ConvertTimeFromFFmpeg(cachedPacket_->pts, avCodecContext_->time_base));
    packetBuffer->dts =
            static_cast<uint64_t>(ConvertTimeFromFFmpeg(cachedPacket_->dts, avCodecContext_->time_base));
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fwrite(reinterpret_cast<const char *>(cachedPacket_->data), 1, cachedPacket_->size, dumpFd_);
    }
#endif
    MEDIA_LOG_D("receive one pkt, hst pts: " PUBLIC_LOG_U64 " ns, ffmpeg pts: " PUBLIC_LOG_D64
                " us, duration: " PUBLIC_LOG_D64 ", pos: " PUBLIC_LOG_D64,
                packetBuffer->pts, cachedPacket_->pts, cachedPacket_->duration, cachedPacket_->pos);
    return Status::OK;
}

Status VideoFfmpegEncoderPlugin::ReceiveBufferLocked(const std::shared_ptr<Buffer>& packetBuffer)
{
    FALSE_RETURN_V_MSG_E(state_ == State::RUNNING, Status::ERROR_WRONG_STATE,
        "encode task in wrong state");
    Status status;
    auto ret = avcodec_receive_packet(avCodecContext_.get(), cachedPacket_.get());
    if (ret >= 0) {
        status = FillFrameBuffer(packetBuffer);
    } else if (ret == AVERROR_EOF) {
        MEDIA_LOG_I("eos received");
        packetBuffer->GetMemory()->Reset();
        packetBuffer->flag |= BUFFER_FLAG_EOS;
        avcodec_flush_buffers(avCodecContext_.get());
        status = Status::END_OF_STREAM;
    } else {
        MEDIA_LOG_D("video encoder receive error: " PUBLIC_LOG_S, AVStrError(ret).c_str());
        status = Status::ERROR_TIMED_OUT;
    }
    av_frame_unref(cachedFrame_.get());
    return status;
}

void VideoFfmpegEncoderPlugin::ReceiveBuffer()
{
    std::shared_ptr<Buffer> packetBuffer = outBufferQ_.Pop();
    FALSE_RETURN_MSG(packetBuffer != nullptr && !packetBuffer->IsEmpty() &&
        packetBuffer->GetBufferMeta()->GetType() == BufferMetaType::VIDEO,
        "cannot fetch valid buffer to output");
    Status status;
    {
        OSAL::ScopedLock lock(avMutex_);
        status = ReceiveBufferLocked(packetBuffer);
    }
    if (status == Status::OK || status == Status::END_OF_STREAM) {
        NotifyOutputBufferDone(packetBuffer);
    } else {
        outBufferQ_.Push(packetBuffer);
    }
}

void VideoFfmpegEncoderPlugin::NotifyInputBufferDone(const std::shared_ptr<Buffer>& input)
{
    if (dataCb_ != nullptr) {
        dataCb_->OnInputBufferDone(const_cast<std::shared_ptr<Buffer>&>(input));
    }
}

void VideoFfmpegEncoderPlugin::NotifyOutputBufferDone(const std::shared_ptr<Buffer>& output)
{
    if (dataCb_ != nullptr) {
        dataCb_->OnOutputBufferDone(const_cast<std::shared_ptr<Buffer>&>(output));
    }
}

std::shared_ptr<Allocator> VideoFfmpegEncoderPlugin::GetAllocator()
{
    return nullptr;
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
