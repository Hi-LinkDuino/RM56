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

#ifdef VIDEO_SUPPORT

#define HST_LOG_TAG "FfmpegVideoDecoderPlugin"

#include "video_ffmpeg_decoder_plugin.h"
#include <cstring>
#include <map>
#include <set>
#include "plugin/common/plugin_caps_builder.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"
#include "plugin/common/surface_memory.h"

namespace {
// register plugins
using namespace OHOS::Media::Plugin;
using namespace Ffmpeg;
void UpdatePluginDefinition(const AVCodec* codec, CodecPluginDef& definition);

std::map<std::string, std::shared_ptr<const AVCodec>> codecMap;

constexpr size_t BUFFER_QUEUE_SIZE = 8;
constexpr int32_t STRIDE_ALIGN = 16;

std::set<AVCodecID> supportedCodec = {AV_CODEC_ID_H264};

std::shared_ptr<CodecPlugin> VideoFfmpegDecoderCreator(const std::string& name)
{
    return std::make_shared<VideoFfmpegDecoderPlugin>(name);
}

Status RegisterVideoDecoderPlugins(const std::shared_ptr<Register>& reg)
{
    const AVCodec* codec = nullptr;
    void* iter = nullptr;
    MEDIA_LOG_I("registering video decoders");
    while ((codec = av_codec_iterate(&iter))) {
        if (!av_codec_is_decoder(codec) || codec->type != AVMEDIA_TYPE_VIDEO) {
            continue;
        }
        if (supportedCodec.find(codec->id) == supportedCodec.end()) {
            MEDIA_LOG_DD("codec " PUBLIC_LOG_S "(" PUBLIC_LOG_S ") is not supported right now",
                         codec->name, codec->long_name);
            continue;
        }
        CodecPluginDef definition;
        definition.name = "videodecoder_" + std::string(codec->name);
        definition.codecType = CodecType::VIDEO_DECODER;
        definition.rank = 100; // 100
        definition.creator = VideoFfmpegDecoderCreator;
        UpdatePluginDefinition(codec, definition);
        // do not delete the codec in the deleter
        codecMap[definition.name] = std::shared_ptr<AVCodec>(const_cast<AVCodec*>(codec), [](void* ptr) {});
        if (reg->AddPlugin(definition) != Status::OK) {
            MEDIA_LOG_W("register plugin " PUBLIC_LOG_S "(" PUBLIC_LOG_S ") failed",
                        codec->name, codec->long_name);
        }
    }
    return Status::OK;
}

void UnRegisterVideoDecoderPlugins()
{
    codecMap.clear();
}

void UpdateInCaps(const AVCodec* codec, CodecPluginDef& definition)
{
    CapabilityBuilder incapBuilder;
    switch (codec->id) {
        case AV_CODEC_ID_H264:
            incapBuilder.SetMime(OHOS::Media::MEDIA_MIME_VIDEO_H264);
            break;
        default:
            incapBuilder.SetMime("video/unknown");
            MEDIA_LOG_I("codec is not supported right now");
            break;
    }
    definition.inCaps.push_back(incapBuilder.Build());
}

void UpdateOutCaps(const AVCodec* codec, CodecPluginDef& definition)
{
    CapabilityBuilder outcapBuilder;
    outcapBuilder.SetMime(OHOS::Media::MEDIA_MIME_VIDEO_RAW);
    if (codec->pix_fmts != nullptr) {
        DiscreteCapability<VideoPixelFormat> values;
        size_t index = 0;
        for (index = 0; codec->pix_fmts[index] != 0; ++index) {
            auto supportFormat = ConvertPixelFormatFromFFmpeg(codec->pix_fmts[index]);
            if (supportFormat != VideoPixelFormat::UNKNOWN) {
                values.push_back(supportFormat);
            }
        }
        if (index) {
            outcapBuilder.SetVideoPixelFormatList(values);
        }
    }
    definition.outCaps.push_back(outcapBuilder.Build());
}

void UpdatePluginDefinition(const AVCodec* codec, CodecPluginDef& definition)
{
    UpdateInCaps(codec, definition);
    UpdateOutCaps(codec, definition);
}
} // namespace

PLUGIN_DEFINITION(FFmpegVideoDecoders, LicenseType::LGPL, RegisterVideoDecoderPlugins, UnRegisterVideoDecoderPlugins);

namespace OHOS {
namespace Media {
namespace Plugin {
VideoFfmpegDecoderPlugin::VideoFfmpegDecoderPlugin(std::string name)
    : CodecPlugin(std::move(name)), outBufferQ_("vdecPluginQueue", BUFFER_QUEUE_SIZE)
{
    for (int32_t i = 0; i < AV_NUM_DATA_POINTERS; i++) {
        scaleData_[i] = nullptr;
        scaleLineSize_[i] = 0;
    }
    isAllocScaleData_ = false;
}

Status VideoFfmpegDecoderPlugin::Init()
{
    OSAL::ScopedLock l(avMutex_);
    auto iter = codecMap.find(pluginName_);
    if (iter == codecMap.end()) {
        MEDIA_LOG_W("cannot find codec with name " PUBLIC_LOG_S, pluginName_.c_str());
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    avCodec_ = iter->second;
    cachedFrame_ = std::shared_ptr<AVFrame>(av_frame_alloc(), [](AVFrame* fp) { av_frame_free(&fp); });
    videoDecParams_[Tag::REQUIRED_OUT_BUFFER_CNT] = (uint32_t)BUFFER_QUEUE_SIZE;
    if (!decodeTask_) {
        decodeTask_ = std::make_shared<OHOS::Media::OSAL::Task>("videoFfmpegDecThread");
        decodeTask_->RegisterHandler([this] { ReceiveFrameBuffer(); });
    }
    state_ = State::INITIALIZED;
    MEDIA_LOG_I("Init success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::Deinit()
{
    OSAL::ScopedLock l(avMutex_);
    avCodec_.reset();
    cachedFrame_.reset();
    ResetLocked();
    if (decodeTask_) {
        decodeTask_->Stop();
        decodeTask_.reset();
    }
    state_ = State::DESTROYED;
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::SetParameter(Tag tag, const ValueType& value)
{
    OSAL::ScopedLock l(avMutex_);
    videoDecParams_.insert(std::make_pair(tag, value));
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::GetParameter(Tag tag, ValueType& value)
{
    OSAL::ScopedLock l(avMutex_);
    auto res = videoDecParams_.find(tag);
    if (res != videoDecParams_.end()) {
        value = res->second;
        return Status::OK;
    }
    return Status::ERROR_INVALID_PARAMETER;
}

template <typename T>
void VideoFfmpegDecoderPlugin::FindInParameterMapThenAssignLocked(Tag tag, T& assign)
{
    auto iter = videoDecParams_.find(tag);
    if (iter != videoDecParams_.end() && iter->second.SameTypeWith(typeid(T))) {
        assign = Plugin::AnyCast<T>(iter->second);
    } else {
        MEDIA_LOG_W("parameter " PUBLIC_LOG_D32 " is not found or type mismatch", static_cast<int32_t>(tag));
    }
}

Status VideoFfmpegDecoderPlugin::CreateCodecContext()
{
    auto context = avcodec_alloc_context3(avCodec_.get());
    if (context == nullptr) {
        MEDIA_LOG_E("cannot allocate codec context");
        return Status::ERROR_UNKNOWN;
    }
    avCodecContext_ = std::shared_ptr<AVCodecContext>(context, [](AVCodecContext* ptr) {
        if (ptr != nullptr) {
            if (ptr->extradata) {
                av_free(ptr->extradata);
                ptr->extradata = nullptr;
            }
            avcodec_free_context(&ptr);
        }
    });
    MEDIA_LOG_I("CreateCodecContext success");
    return Status::OK;
}

void VideoFfmpegDecoderPlugin::InitCodecContext()
{
    avCodecContext_->codec_type = AVMEDIA_TYPE_VIDEO;
    FindInParameterMapThenAssignLocked<int64_t>(Tag::MEDIA_BITRATE, avCodecContext_->bit_rate);
    FindInParameterMapThenAssignLocked<std::uint32_t>(Tag::VIDEO_WIDTH, width_);
    FindInParameterMapThenAssignLocked<std::uint32_t>(Tag::VIDEO_HEIGHT, height_);
    FindInParameterMapThenAssignLocked<Plugin::VideoPixelFormat>(Tag::VIDEO_PIXEL_FORMAT, pixelFormat_);
    MEDIA_LOG_D("bitRate: " PUBLIC_LOG_D64 ", width: " PUBLIC_LOG_U32 ", height: " PUBLIC_LOG_U32
                ", pixelFormat: " PUBLIC_LOG_U32, avCodecContext_->bit_rate, width_, height_, pixelFormat_);
    SetCodecExtraData();
    // Reset coded_width/_height to prevent it being reused from last time when
    // the codec is opened again, causing a mismatch and possible segfault/corruption.
    avCodecContext_->coded_width = 0;
    avCodecContext_->coded_height = 0;
    avCodecContext_->workaround_bugs |= FF_BUG_AUTODETECT;
    avCodecContext_->err_recognition = 1;
}

void VideoFfmpegDecoderPlugin::DeinitCodecContext()
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
    avCodecContext_->coded_width = 0;
    avCodecContext_->coded_height = 0;
    avCodecContext_->time_base.den = 0;
    avCodecContext_->time_base.num = 0;
    avCodecContext_->ticks_per_frame = 0;
    avCodecContext_->sample_aspect_ratio.num = 0;
    avCodecContext_->sample_aspect_ratio.den = 0;
    avCodecContext_->get_buffer2 = nullptr;
}

void VideoFfmpegDecoderPlugin::SetCodecExtraData()
{
    auto iter = videoDecParams_.find(Tag::MEDIA_CODEC_CONFIG);
    if (iter == videoDecParams_.end() || !iter->second.SameTypeWith(typeid(std::vector<uint8_t>))) {
        return;
    }
    auto codecConfig = Plugin::AnyCast<std::vector<uint8_t>>(iter->second);
    int configSize = codecConfig.size();
    if (configSize > 0) {
        auto allocSize = AlignUp(configSize + AV_INPUT_BUFFER_PADDING_SIZE, STRIDE_ALIGN);
        avCodecContext_->extradata = static_cast<uint8_t*>(av_mallocz(allocSize));
        (void)memcpy_s(avCodecContext_->extradata, configSize, codecConfig.data(), configSize);
        avCodecContext_->extradata_size = configSize;
        MEDIA_LOG_I("SetCodecExtraData success");
    }
}

Status VideoFfmpegDecoderPlugin::OpenCodecContext()
{
    AVCodec* vdec = avcodec_find_decoder(avCodecContext_->codec_id);
    if (vdec == nullptr) {
        MEDIA_LOG_E("Codec: " PUBLIC_LOG_D32 " is not found", static_cast<int32_t>(avCodecContext_->codec_id));
        DeinitCodecContext();
        return Status::ERROR_INVALID_PARAMETER;
    }
    auto res = avcodec_open2(avCodecContext_.get(), avCodec_.get(), nullptr);
    if (res != 0) {
        MEDIA_LOG_E("avcodec open error " PUBLIC_LOG_S " when start decoder ", AVStrError(res).c_str());
        DeinitCodecContext();
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_I("OpenCodecContext success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::CloseCodecContext()
{
    Status ret = Status::OK;
    if (avCodecContext_ != nullptr) {
        auto res = avcodec_close(avCodecContext_.get());
        if (res != 0) {
            DeinitCodecContext();
            MEDIA_LOG_E("avcodec close error " PUBLIC_LOG_S " when stop decoder", AVStrError(res).c_str());
            ret = Status::ERROR_UNKNOWN;
        }
        avCodecContext_.reset();
    }
    return ret;
}

Status VideoFfmpegDecoderPlugin::Prepare()
{
    {
        OSAL::ScopedLock l(avMutex_);
        if (state_ != State::INITIALIZED && state_ != State::PREPARED) {
            return Status::ERROR_WRONG_STATE;
        }
        if (CreateCodecContext() != Status::OK) {
            MEDIA_LOG_E("Create codec context fail");
            return Status::ERROR_UNKNOWN;
        }
        InitCodecContext();
#ifdef DUMP_RAW_DATA
        dumpFd_ = std::fopen("./vdec_out.yuv", "wb");
#endif
        state_ = State::PREPARED;
    }
    avPacket_ = std::shared_ptr<AVPacket>(av_packet_alloc(), [](AVPacket* ptr) {
        av_packet_free(&ptr);
    });
    outBufferQ_.SetActive(true);
    MEDIA_LOG_I("Prepare success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::ResetLocked()
{
    videoDecParams_.clear();
    avCodecContext_.reset();
    outBufferQ_.Clear();
    if (scaleData_[0] != nullptr) {
        if (isAllocScaleData_) {
            av_free(scaleData_[0]);
            isAllocScaleData_ = false;
        }
        for (int32_t i = 0; i < AV_NUM_DATA_POINTERS; i++) {
            scaleData_[i] = nullptr;
            scaleLineSize_[i] = 0;
        }
    }
#ifdef DUMP_RAW_DATA
    if (dumpFd_) {
        std::fclose(dumpFd_);
        dumpFd_ = nullptr;
    }
#endif
    state_ = State::INITIALIZED;
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::Reset()
{
    OSAL::ScopedLock l(avMutex_);
    return ResetLocked();
}

Status VideoFfmpegDecoderPlugin::Start()
{
    {
        OSAL::ScopedLock l(avMutex_);
        if (state_ != State::PREPARED) {
            return Status::ERROR_WRONG_STATE;
        }
        if (OpenCodecContext() != Status::OK) {
            MEDIA_LOG_E("Open codec context fail");
            return Status::ERROR_UNKNOWN;
        }
        state_ = State::RUNNING;
    }
    outBufferQ_.SetActive(true);
    decodeTask_->Start();
    MEDIA_LOG_I("Start success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::Stop()
{
    Status ret = Status::OK;
    {
        OSAL::ScopedLock l(avMutex_);
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
    decodeTask_->Stop();
    MEDIA_LOG_I("Stop success");
    return ret;
}

Status VideoFfmpegDecoderPlugin::QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffer, int32_t timeoutMs)
{
    outBufferQ_.Push(outputBuffer);
    MEDIA_LOG_DD("QueueOutputBuffer success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::Flush()
{
    OSAL::ScopedLock l(avMutex_);
    if (avCodecContext_ != nullptr) {
        // flush avcodec buffers
    }
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs)
{
    if (inputBuffer->IsEmpty() && !(inputBuffer->flag & BUFFER_FLAG_EOS)) {
        MEDIA_LOG_E("decoder does not support fd buffer");
        return Status::ERROR_INVALID_DATA;
    }
    Status ret = Status::OK;
    {
        OSAL::ScopedLock l(avMutex_);
        ret = SendBufferLocked(inputBuffer);
    }
    NotifyInputBufferDone(inputBuffer);
    MEDIA_LOG_DD("QueueInputBuffer ret: " PUBLIC_LOG_U32, ret);
    return ret;
}

Status VideoFfmpegDecoderPlugin::SendBufferLocked(const std::shared_ptr<Buffer>& inputBuffer)
{
    if (state_ != State::RUNNING) {
        MEDIA_LOG_W("SendBufferLocked in wrong state: " PUBLIC_LOG_D32, state_);
        return Status::ERROR_WRONG_STATE;
    }
    if (inputBuffer && !(inputBuffer->flag & BUFFER_FLAG_EOS)) {
        auto inputMemory = inputBuffer->GetMemory();
        const uint8_t* ptr = inputMemory->GetReadOnlyData();
        auto bufferLength = inputMemory->GetSize();
        size_t bufferEnd = bufferLength;
        // pad to data if needed
        if ((bufferLength % AV_INPUT_BUFFER_PADDING_SIZE != 0) &&
            (bufferLength - bufferEnd + bufferLength % AV_INPUT_BUFFER_PADDING_SIZE < AV_INPUT_BUFFER_PADDING_SIZE)) {
            if (paddedBufferSize_ < bufferLength + AV_INPUT_BUFFER_PADDING_SIZE) {
                paddedBufferSize_ = bufferLength + AV_INPUT_BUFFER_PADDING_SIZE;
                paddedBuffer_.reserve(paddedBufferSize_);
                MEDIA_LOG_I("increase padded buffer size to " PUBLIC_LOG_ZU, paddedBufferSize_);
            }
            paddedBuffer_.assign(ptr, ptr + bufferLength);
            paddedBuffer_.insert(paddedBuffer_.end(), AV_INPUT_BUFFER_PADDING_SIZE, 0);
            ptr = paddedBuffer_.data();
        }
        avPacket_->data = const_cast<uint8_t*>(ptr);
        avPacket_->size = static_cast<int32_t>(bufferLength);
        avPacket_->pts = static_cast<int64_t>(inputBuffer->pts);
    }
    auto ret = avcodec_send_packet(avCodecContext_.get(), avPacket_.get());
    av_packet_unref(avPacket_.get());
    if (ret < 0) {
        MEDIA_LOG_DD("send buffer error " PUBLIC_LOG_S, AVStrError(ret).c_str());
        return Status::ERROR_NO_MEMORY;
    }
    return Status::OK;
}

#ifdef DUMP_RAW_DATA
void VideoFfmpegDecoderPlugin::DumpVideoRawOutData()
{
    if (dumpFd_ == nullptr) {
        return;
    }
    if (pixelFormat_ == VideoPixelFormat::YUV420P) {
        if (scaleData_[0] != nullptr && scaleLineSize_[0] != 0) {
            std::fwrite(reinterpret_cast<const char*>(scaleData_[0]),
                        scaleLineSize_[0] * height_, 1, dumpFd_);
        }
        if (scaleData_[1] != nullptr && scaleLineSize_[1] != 0) {
            std::fwrite(reinterpret_cast<const char*>(scaleData_[1]),
                        scaleLineSize_[1] * height_ / 2, 1, dumpFd_); // 2
        }
        if (scaleData_[2] != nullptr && scaleLineSize_[2] != 0) { // 2
            std::fwrite(reinterpret_cast<const char*>(scaleData_[2]),
                        scaleLineSize_[2] * height_ / 2, 1, dumpFd_); // 2
        }
    } else if (pixelFormat_ == VideoPixelFormat::NV21 || pixelFormat_ == VideoPixelFormat::NV12) {
        if (scaleData_[0] != nullptr && scaleLineSize_[0] != 0) {
            std::fwrite(reinterpret_cast<const char*>(scaleData_[0]),
                        scaleLineSize_[0] * height_, 1, dumpFd_);
        }
        if (scaleData_[1] != nullptr && scaleLineSize_[1] != 0) {
            std::fwrite(reinterpret_cast<const char*>(scaleData_[1]),
                        scaleLineSize_[1] * height_ / 2, 1, dumpFd_); // 2
        }
    } else if (pixelFormat_ == VideoPixelFormat::RGBA || pixelFormat_ == VideoPixelFormat::ARGB ||
               pixelFormat_ == VideoPixelFormat::ABGR || pixelFormat_ == VideoPixelFormat::BGRA) {
        if (scaleData_[0] != nullptr && scaleLineSize_[0] != 0) {
            std::fwrite(reinterpret_cast<const char*>(scaleData_[0]),
                        scaleLineSize_[0] * height_, 1, dumpFd_);
        }
    }
}
#endif

Status VideoFfmpegDecoderPlugin::CreateSwsContext()
{
    if (swsCtx_ != nullptr) {
        return Status::OK;
    }
    auto swsContext = sws_getContext(cachedFrame_->width, cachedFrame_->height,
                                     static_cast<enum AVPixelFormat>(cachedFrame_->format),
                                     static_cast<int32_t>(width_), static_cast<int32_t>(height_),
                                     ConvertPixelFormatToFFmpeg(pixelFormat_), SWS_BILINEAR, NULL, NULL, NULL);
    FALSE_RETURN_V_MSG_E(swsContext != nullptr, Status::ERROR_UNKNOWN, "sws_getContext fail");
    swsCtx_ = std::shared_ptr<struct SwsContext>(swsContext, [](struct SwsContext *ptr) {
        if (ptr != nullptr) {
            sws_freeContext(ptr);
        }
    });
    FALSE_RETURN_V_MSG_E(swsCtx_ != nullptr, Status::ERROR_NO_MEMORY, "create swsCtx fail");
    if (scaleData_[0] == nullptr && !isAllocScaleData_) {
        auto ret = av_image_alloc(scaleData_, scaleLineSize_, width_, height_,
                                  ConvertPixelFormatToFFmpeg(pixelFormat_), STRIDE_ALIGN);
        FALSE_RETURN_V_MSG_E(ret >= 0, Status::ERROR_UNKNOWN, "av_image_fill_linesizes fail: " PUBLIC_LOG_D32, ret);
        MEDIA_LOG_D("pixelFormat_: " PUBLIC_LOG_U32 ", pix_fmt: " PUBLIC_LOG_D32,
                    pixelFormat_, ConvertPixelFormatToFFmpeg(pixelFormat_));
        for (int32_t i = 0; i < AV_NUM_DATA_POINTERS; i++) {
            MEDIA_LOG_D("scaleData[" PUBLIC_LOG_D32 "]: " PUBLIC_LOG_P ", scaleLineSize[" PUBLIC_LOG_D32 "]: "
                        PUBLIC_LOG_D32, i, scaleData_[i], i, scaleLineSize_[i]);
            if (scaleData_[i] && !scaleLineSize_[i]) {
                MEDIA_LOG_E("scaleFrame is broken, i: " PUBLIC_LOG_D32, i);
                return Status::ERROR_UNKNOWN;
            }
        }
        isAllocScaleData_ = true;
    }
    MEDIA_LOG_D("CreateSwsContext success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::ScaleVideoFrame()
{
    if (ConvertPixelFormatFromFFmpeg(static_cast<AVPixelFormat>(cachedFrame_->format)) == pixelFormat_ &&
        static_cast<uint32_t>(cachedFrame_->width) == width_ &&
        static_cast<uint32_t>(cachedFrame_->height) == height_) {
        for (int32_t i = 0; cachedFrame_->linesize[i] > 0; i++) {
            scaleData_[i] = cachedFrame_->data[i];
            scaleLineSize_[i] = cachedFrame_->linesize[i];
        }
        return Status::OK;
    }
    auto ret = CreateSwsContext();
    FALSE_RETURN_V_MSG_E(ret == Status::OK, ret, "CreateSwsContext fail: " PUBLIC_LOG_D32, ret);
    int32_t res = sws_scale(swsCtx_.get(), cachedFrame_->data, cachedFrame_->linesize, 0, cachedFrame_->height,
                            scaleData_, scaleLineSize_);
    FALSE_RETURN_V_MSG_E(res >= 0, Status::ERROR_UNKNOWN, "sws_scale fail: " PUBLIC_LOG_D32, ret);
    MEDIA_LOG_D("ScaleVideoFrame success");
    return Status::OK;
}

#ifndef OHOS_LITE
Status VideoFfmpegDecoderPlugin::WriteYuvDataStride(const std::shared_ptr<Buffer>& frameBuffer, int32_t stride)
{
    auto frameBufferMem = frameBuffer->GetMemory();
    size_t srcPos = 0;
    size_t dstPos = 0;
    if (pixelFormat_ == VideoPixelFormat::YUV420P) {
        auto writeSize = scaleLineSize_[0];
        for (uint32_t colNum = 0; colNum < height_; colNum++) {
            frameBufferMem->Write(scaleData_[0] + srcPos, writeSize, dstPos);
            dstPos += stride;
        }
        srcPos = 0;
        writeSize = scaleLineSize_[1];
        for (uint32_t colNum = 0; colNum < height_; colNum++) {
            frameBufferMem->Write(scaleData_[1] + srcPos, writeSize, dstPos);
            dstPos += stride;
        }
        srcPos = 0;
        writeSize = scaleLineSize_[2]; // 2
        for (uint32_t colNum = 0; colNum < height_; colNum++) {
            frameBufferMem->Write(scaleData_[2] + srcPos, writeSize, dstPos); // 2
            dstPos += stride;
        }
    } else if ((pixelFormat_ == VideoPixelFormat::NV12) || (pixelFormat_ == VideoPixelFormat::NV21)) {
        auto writeSize = scaleLineSize_[0];
        for (uint32_t colNum = 0; colNum < height_; colNum++) {
            frameBufferMem->Write(scaleData_[0] + srcPos, writeSize, dstPos);
            dstPos += stride;
        }
        srcPos = 0;
        writeSize = scaleLineSize_[1];
        for (uint32_t colNum = 0; colNum < height_; colNum++) {
            frameBufferMem->Write(scaleData_[1] + srcPos, writeSize, dstPos);
            dstPos += stride;
        }
    } else {
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    MEDIA_LOG_D("WriteYuvDataStride success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::WriteRgbDataStride(const std::shared_ptr<Buffer>& frameBuffer, int32_t stride)
{
    auto frameBufferMem = frameBuffer->GetMemory();
    if (pixelFormat_ == VideoPixelFormat::RGBA || pixelFormat_ == VideoPixelFormat::ARGB ||
        pixelFormat_ == VideoPixelFormat::ABGR || pixelFormat_ == VideoPixelFormat::BGRA) {
        size_t srcPos = 0;
        size_t dstPos = 0;
        auto writeSize = scaleLineSize_[0];
        for (uint32_t colNum = 0; colNum < height_; colNum++) {
            frameBufferMem->Write(scaleData_[0] + srcPos, writeSize, dstPos);
            dstPos += stride;
        }
    } else {
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    MEDIA_LOG_D("WriteRgbDataStride success");
    return Status::OK;
}
#endif

Status VideoFfmpegDecoderPlugin::WriteYuvData(const std::shared_ptr<Buffer>& frameBuffer)
{
    auto frameBufferMem = frameBuffer->GetMemory();
#ifndef OHOS_LITE
    if (frameBufferMem->GetMemoryType() == Plugin::MemoryType::SURFACE_BUFFER) {
        std::shared_ptr<Plugin::SurfaceMemory> surfaceMemory =
                Plugin::ReinterpretPointerCast<Plugin::SurfaceMemory>(frameBufferMem);
        auto stride = surfaceMemory->GetSurfaceBufferStride();
        if (stride % width_) {
            return WriteYuvDataStride(frameBuffer, stride);
        }
    }
#endif
    size_t ySize = static_cast<size_t>(scaleLineSize_[0] * height_);
    // AV_PIX_FMT_YUV420P: scaleLineSize_[0] = scaleLineSize_[1] * 2 = scaleLineSize_[2] * 2
    // AV_PIX_FMT_NV12: scaleLineSize_[0] = scaleLineSize_[1]
    size_t uvSize = static_cast<size_t>(scaleLineSize_[1] * height_ / 2); // 2
    size_t frameSize = 0;
    if (pixelFormat_ == VideoPixelFormat::YUV420P) {
        frameSize = ySize + (uvSize * 2); // 2
    } else if (pixelFormat_ == VideoPixelFormat::NV21 || pixelFormat_ == VideoPixelFormat::NV12) {
        frameSize = ySize + uvSize;
    }
    FALSE_RETURN_V_MSG_E(frameBufferMem->GetCapacity() >= frameSize, Status::ERROR_NO_MEMORY,
                         "output buffer size is not enough: real[" PUBLIC_LOG "zu], need[" PUBLIC_LOG "zu]",
                         frameBufferMem->GetCapacity(), frameSize);
    if (pixelFormat_ == VideoPixelFormat::YUV420P) {
        frameBufferMem->Write(scaleData_[0], ySize);
        frameBufferMem->Write(scaleData_[1], uvSize);
        frameBufferMem->Write(scaleData_[2], uvSize); // 2
    } else if ((pixelFormat_ == VideoPixelFormat::NV12) || (pixelFormat_ == VideoPixelFormat::NV21)) {
        frameBufferMem->Write(scaleData_[0], ySize);
        frameBufferMem->Write(scaleData_[1], uvSize);
    } else {
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    MEDIA_LOG_DD("WriteYuvData success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::WriteRgbData(const std::shared_ptr<Buffer>& frameBuffer)
{
    auto frameBufferMem = frameBuffer->GetMemory();
#ifndef OHOS_LITE
    if (frameBufferMem->GetMemoryType() == Plugin::MemoryType::SURFACE_BUFFER) {
        std::shared_ptr<Plugin::SurfaceMemory> surfaceMemory =
                Plugin::ReinterpretPointerCast<Plugin::SurfaceMemory>(frameBufferMem);
        auto stride = surfaceMemory->GetSurfaceBufferStride();
        if (stride % width_) {
            return WriteRgbDataStride(frameBuffer, stride);
        }
    }
#endif
    size_t frameSize = static_cast<size_t>(scaleLineSize_[0] * height_);
    FALSE_RETURN_V_MSG_E(frameBufferMem->GetCapacity() >= frameSize, Status::ERROR_NO_MEMORY,
                         "output buffer size is not enough: real[" PUBLIC_LOG "zu], need[" PUBLIC_LOG "zu]",
                         frameBufferMem->GetCapacity(), frameSize);
    if (pixelFormat_ == VideoPixelFormat::RGBA || pixelFormat_ == VideoPixelFormat::ARGB ||
        pixelFormat_ == VideoPixelFormat::ABGR || pixelFormat_ == VideoPixelFormat::BGRA) {
        frameBufferMem->Write(scaleData_[0], frameSize);
    } else {
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    MEDIA_LOG_D("WriteRgbData success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::FillFrameBuffer(const std::shared_ptr<Buffer>& frameBuffer)
{
    MEDIA_LOG_DD("receive one frame: " PUBLIC_LOG_D32 ", picture type: " PUBLIC_LOG_D32 ", pixel format: "
                 PUBLIC_LOG_D32 ", packet size: " PUBLIC_LOG_D32, cachedFrame_->key_frame,
                 static_cast<int32_t>(cachedFrame_->pict_type), cachedFrame_->format, cachedFrame_->pkt_size);
    FALSE_RETURN_V_MSG_E((cachedFrame_->flags & AV_FRAME_FLAG_CORRUPT) == 0, Status::ERROR_INVALID_DATA,
                         "decoded frame is corrupt");
    auto ret = ScaleVideoFrame();
    FALSE_RETURN_V_MSG_E(ret == Status::OK, ret, "ScaleVideoFrame fail: " PUBLIC_LOG_D32, ret);
    auto bufferMeta = frameBuffer->GetBufferMeta();
    if (bufferMeta != nullptr && bufferMeta->GetType() == BufferMetaType::VIDEO) {
        std::shared_ptr<VideoBufferMeta> videoMeta = ReinterpretPointerCast<VideoBufferMeta>(bufferMeta);
        videoMeta->videoPixelFormat = pixelFormat_;
        videoMeta->height = height_;
        videoMeta->width = width_;
        for (int i = 0; scaleLineSize_[i] > 0; ++i) {
            videoMeta->stride.emplace_back(scaleLineSize_[i]);
        }
        videoMeta->planes = videoMeta->stride.size();
    }
#ifdef DUMP_RAW_DATA
    DumpVideoRawOutData();
#endif
    auto newFormat = ConvertPixelFormatToFFmpeg(pixelFormat_);
    if (IsYuvFormat(newFormat)) {
        FALSE_RETURN_V_MSG_E(WriteYuvData(frameBuffer) == Status::OK, Status::ERROR_UNSUPPORTED_FORMAT,
                             "Unsupported pixel format: " PUBLIC_LOG_U32, pixelFormat_);
    } else if (IsRgbFormat(newFormat)) {
        FALSE_RETURN_V_MSG_E(WriteRgbData(frameBuffer) == Status::OK, Status::ERROR_UNSUPPORTED_FORMAT,
                             "Unsupported pixel format: " PUBLIC_LOG_U32, pixelFormat_);
    } else {
        MEDIA_LOG_E("Unsupported pixel format: " PUBLIC_LOG_U32, pixelFormat_);
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    frameBuffer->pts = static_cast<uint64_t>(cachedFrame_->pts);
    MEDIA_LOG_DD("FillFrameBuffer success");
    return Status::OK;
}

Status VideoFfmpegDecoderPlugin::ReceiveBufferLocked(const std::shared_ptr<Buffer>& frameBuffer)
{
    if (state_ != State::RUNNING) {
        MEDIA_LOG_W("ReceiveBufferLocked in wrong state: " PUBLIC_LOG_D32, state_);
        return Status::ERROR_WRONG_STATE;
    }
    Status status;
    auto ret = avcodec_receive_frame(avCodecContext_.get(), cachedFrame_.get());
    if (ret >= 0) {
        status = FillFrameBuffer(frameBuffer);
    } else if (ret == AVERROR_EOF) {
        MEDIA_LOG_I("eos received");
        frameBuffer->GetMemory()->Reset();
        frameBuffer->flag |= BUFFER_FLAG_EOS;
        avcodec_flush_buffers(avCodecContext_.get());
        status = Status::END_OF_STREAM;
    } else {
        MEDIA_LOG_DD("video decoder receive error: " PUBLIC_LOG_S, AVStrError(ret).c_str());
        status = Status::ERROR_TIMED_OUT;
    }
    av_frame_unref(cachedFrame_.get());
    MEDIA_LOG_DD("ReceiveBufferLocked status: " PUBLIC_LOG_U32, status);
    return status;
}

void VideoFfmpegDecoderPlugin::ReceiveFrameBuffer()
{
    std::shared_ptr<Buffer> frameBuffer = outBufferQ_.Pop();
    if (frameBuffer == nullptr || frameBuffer->IsEmpty()) {
        MEDIA_LOG_W("cannot fetch valid buffer to output");
        return;
    }
    auto frameMeta = frameBuffer->GetBufferMeta();
    if (frameMeta == nullptr || frameMeta->GetType() != BufferMetaType::VIDEO) {
        MEDIA_LOG_W("output buffer is not video buffer");
        return;
    }
    Status status;
    {
        OSAL::ScopedLock l(avMutex_);
        status = ReceiveBufferLocked(frameBuffer);
    }
    if (status == Status::OK || status == Status::END_OF_STREAM) {
        NotifyOutputBufferDone(frameBuffer);
    } else {
        outBufferQ_.Push(frameBuffer);
    }
}

void VideoFfmpegDecoderPlugin::NotifyInputBufferDone(const std::shared_ptr<Buffer>& input)
{
    if (dataCb_ != nullptr) {
        dataCb_->OnInputBufferDone(input);
    }
}

void VideoFfmpegDecoderPlugin::NotifyOutputBufferDone(const std::shared_ptr<Buffer>& output)
{
    if (dataCb_ != nullptr) {
        dataCb_->OnOutputBufferDone(output);
    }
}

std::shared_ptr<Allocator> VideoFfmpegDecoderPlugin::GetAllocator()
{
    return nullptr;
}
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif
