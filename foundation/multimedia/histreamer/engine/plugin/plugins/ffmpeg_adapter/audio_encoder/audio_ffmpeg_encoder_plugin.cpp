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

#define HST_LOG_TAG "Ffmpeg_Au_Encoder"

#include "audio_ffmpeg_encoder_plugin.h"
#include <cstring>
#include <map>
#include <set>
#include "ffmpeg_au_enc_config.h"
#include "plugin/common/plugin_caps_builder.h"
#include "plugins/ffmpeg_adapter/utils/ffmpeg_utils.h"

namespace {
// register plugins
using namespace OHOS::Media::Plugin;
using namespace Ffmpeg;
void UpdatePluginDefinition(const AVCodec* codec, CodecPluginDef& definition);

std::map<std::string, std::shared_ptr<const AVCodec>> codecMap;
const size_t BUFFER_QUEUE_SIZE = 6;
std::set<AVCodecID> g_supportedCodec = {AV_CODEC_ID_AAC, AV_CODEC_ID_AAC_LATM};

std::shared_ptr<CodecPlugin> AuFfmpegEncoderCreator(const std::string& name)
{
    return std::make_shared<AudioFfmpegEncoderPlugin>(name);
}

Status RegisterAudioEncoderPlugins(const std::shared_ptr<Register>& reg)
{
    const AVCodec* codec = nullptr;
    void* ite = nullptr;
    MEDIA_LOG_I("registering audio encoders");
    while ((codec = av_codec_iterate(&ite))) {
        if (!av_codec_is_encoder(codec) || codec->type != AVMEDIA_TYPE_AUDIO) {
            continue;
        }
        if (g_supportedCodec.find(codec->id) == g_supportedCodec.end()) {
            MEDIA_LOG_DD("codec " PUBLIC_LOG_S "(" PUBLIC_LOG_S ") is not supported right now",
                         codec->name, codec->long_name);
            continue;
        }
        CodecPluginDef definition;
        definition.name = "ffmpegAuEnc_" + std::string(codec->name);
        definition.codecType = CodecType::AUDIO_ENCODER;
        definition.rank = 100; // 100
        definition.creator = AuFfmpegEncoderCreator;
        UpdatePluginDefinition(codec, definition);
        // do not delete the codec in the deleter
        codecMap[definition.name] = std::shared_ptr<AVCodec>(const_cast<AVCodec*>(codec), [](void* ptr) {});
        if (reg->AddPlugin(definition) != Status::OK) {
            MEDIA_LOG_W("register plugin " PUBLIC_LOG_S "(" PUBLIC_LOG_S ") failed", codec->name, codec->long_name);
        }
    }
    return Status::OK;
}

void UnRegisterAudioEncoderPlugin()
{
    codecMap.clear();
}

void UpdateInCaps(const AVCodec* codec, CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_RAW);
    size_t index = 0;
    if (codec->supported_samplerates != nullptr) {
        DiscreteCapability<uint32_t> values;
        for (index = 0; codec->supported_samplerates[index] != 0; ++index) {
            values.push_back(codec->supported_samplerates[index]);
        }
        if (index) {
            capBuilder.SetAudioSampleRateList(values);
        }
    }
    definition.inCaps.push_back(capBuilder.Build());
}

void UpdateOutCaps(const AVCodec* codec, CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    switch (codec->id) {
        case AV_CODEC_ID_AAC:
            capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_AAC)
                .SetAudioMpegVersion(4) // 4
                .SetAudioAacProfile(AudioAacProfile::LC)
                .SetAudioAacStreamFormat(AudioAacStreamFormat::MP4ADTS);
            break;
        case AV_CODEC_ID_AAC_LATM:
            capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_AAC_LATM)
                .SetAudioMpegVersion(4)  // 4
                .SetAudioAacStreamFormat(AudioAacStreamFormat::MP4LOAS);
            break;
        default:
            MEDIA_LOG_I("codec is not supported right now");
    }
    definition.outCaps.push_back(capBuilder.Build());
}

void UpdatePluginDefinition(const AVCodec* codec, CodecPluginDef& definition)
{
    UpdateInCaps(codec, definition);
    UpdateOutCaps(codec, definition);
}
} // namespace
PLUGIN_DEFINITION(FFmpegAudioEncoders, LicenseType::LGPL, RegisterAudioEncoderPlugins, UnRegisterAudioEncoderPlugin);

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Ffmpeg {
AudioFfmpegEncoderPlugin::AudioFfmpegEncoderPlugin(std::string name) : CodecPlugin(std::move(name)), prev_pts_(0)
{
}

AudioFfmpegEncoderPlugin::~AudioFfmpegEncoderPlugin()
{
    OSAL::ScopedLock lock(avMutex_);
    OSAL::ScopedLock lock1(parameterMutex_);
    DeInitLocked();
}

Status AudioFfmpegEncoderPlugin::Init()
{
    auto ite = codecMap.find(pluginName_);
    if (ite == codecMap.end()) {
        MEDIA_LOG_W("cannot find codec with name " PUBLIC_LOG_S, pluginName_.c_str());
        return Status::ERROR_UNSUPPORTED_FORMAT;
    }
    OSAL::ScopedLock lock(avMutex_);
    avCodec_ = ite->second;
    cachedFrame_ = std::shared_ptr<AVFrame>(av_frame_alloc(), [](AVFrame* frame) { av_frame_free(&frame);});
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::Deinit()
{
    OSAL::ScopedLock lock(avMutex_);
    OSAL::ScopedLock lock1(parameterMutex_);
    return DeInitLocked();
}

Status AudioFfmpegEncoderPlugin::DeInitLocked()
{
    ResetLocked();
    avCodec_.reset();
    cachedFrame_.reset();
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::SetParameter(Tag tag, const ValueType& value)
{
    OSAL::ScopedLock lock(parameterMutex_);
    audioParameter_.insert(std::make_pair(tag, value));
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::GetParameter(Tag tag, ValueType& value)
{
    if (tag == Tag::REQUIRED_OUT_BUFFER_CNT) {
        value = static_cast<uint32_t>(BUFFER_QUEUE_SIZE);
        return Status::OK;
    }
    OSAL::ScopedLock lock(avMutex_);
    if (avCodecContext_ == nullptr) {
        return Status::ERROR_WRONG_STATE;
    }
    return GetAudioEncoderParameters(*avCodecContext_, tag, value);
}

Status AudioFfmpegEncoderPlugin::Prepare()
{
    {
        OSAL::ScopedLock lock(avMutex_);
        if (avCodec_ == nullptr) {
            return Status::ERROR_WRONG_STATE;
        }
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
        {
            OSAL::ScopedLock lock1(parameterMutex_);
            ConfigAudioEncoder(*avCodecContext_, audioParameter_);
        }

        if (!avCodecContext_->time_base.den) {
            avCodecContext_->time_base.den = avCodecContext_->sample_rate;
            avCodecContext_->time_base.num = 1;
            avCodecContext_->ticks_per_frame = 1;
        }

        avCodecContext_->workaround_bugs =
            static_cast<uint32_t>(avCodecContext_->workaround_bugs) | static_cast<uint32_t>(FF_BUG_AUTODETECT);
    }
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::ResetLocked()
{
    audioParameter_.clear();
    avCodecContext_.reset();
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::Reset()
{
    OSAL::ScopedLock lock(avMutex_);
    OSAL::ScopedLock lock1(parameterMutex_);
    return ResetLocked();
}

bool AudioFfmpegEncoderPlugin::CheckReformat()
{
    if (avCodec_ == nullptr || avCodecContext_ == nullptr) {
        return false;
    }
    for (size_t index = 0; avCodec_->sample_fmts[index] != AV_SAMPLE_FMT_NONE; ++index) {
        if (avCodec_->sample_fmts[index] == avCodecContext_->sample_fmt) {
            return false;
        }
    }
    return true;
}

Status AudioFfmpegEncoderPlugin::Start()
{
    {
        OSAL::ScopedLock lock(avMutex_);
        if (avCodecContext_ == nullptr) {
            return Status::ERROR_WRONG_STATE;
        }
        needReformat_ = CheckReformat();
        if (needReformat_) {
            srcFmt_ = avCodecContext_->sample_fmt;
            // always use the first fmt
            avCodecContext_->sample_fmt = avCodec_->sample_fmts[0];
            SwrContext* swrContext = swr_alloc();
            FALSE_RETURN_V_MSG_E(swrContext != nullptr, Status::ERROR_NO_MEMORY, "cannot allocate swr context");
            swrContext = swr_alloc_set_opts(swrContext, avCodecContext_->channel_layout, avCodecContext_->sample_fmt,
                avCodecContext_->sample_rate, avCodecContext_->channel_layout, srcFmt_, avCodecContext_->sample_rate,
                0, nullptr);
            FALSE_RETURN_V_MSG_E(swr_init(swrContext) == 0, Status::ERROR_UNKNOWN, "swr init error");
            swrCtx_ = std::shared_ptr<SwrContext>(swrContext, [](SwrContext* ptr) {
                if (ptr) {
                    swr_free(&ptr);
                }
            });
        }
        auto res = avcodec_open2(avCodecContext_.get(), avCodec_.get(), nullptr);
        FALSE_RETURN_V_MSG_E(res == 0, Status::ERROR_UNKNOWN, "avcodec open error " PUBLIC_LOG_S " when start encoder",
                          AVStrError(res).c_str());
        FALSE_RETURN_V_MSG_E(avCodecContext_->frame_size > 0, Status::ERROR_UNKNOWN, "frame_size unknown");
        fullInputFrameSize_ = (uint32_t)av_samples_get_buffer_size(nullptr, avCodecContext_->channels,
            avCodecContext_->frame_size, srcFmt_, 1);
        srcBytesPerSample_ = av_get_bytes_per_sample(srcFmt_) * avCodecContext_->channels;
        if (needReformat_) {
            auto resampleSize = av_samples_get_buffer_size(nullptr, avCodecContext_->channels,
                                                           avCodecContext_->frame_size, avCodecContext_->sample_fmt, 0);
            resampleCache_ .reserve(resampleSize);
            resampleChannelAddr_.reserve(avCodecContext_->channels);
            auto tmp = resampleChannelAddr_.data();
            av_samples_fill_arrays(tmp, nullptr, resampleCache_.data(), avCodecContext_->channels,
                                   avCodecContext_->frame_size, avCodecContext_->sample_fmt, 0);
        }
        SetParameter(Tag::AUDIO_SAMPLE_PER_FRAME, static_cast<uint32_t>(avCodecContext_->frame_size));
    }
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::Stop()
{
    Status ret = Status::OK;
    {
        OSAL::ScopedLock lock(avMutex_);
        if (avCodecContext_ != nullptr) {
            auto res = avcodec_close(avCodecContext_.get());
            FALSE_RETURN_V_MSG_E(res == 0, Status::ERROR_UNKNOWN,
                "avcodec close error " PUBLIC_LOG_S " when stop encoder", AVStrError(res).c_str());
            avCodecContext_.reset();
        }
        if (outBuffer_) {
            outBuffer_.reset();
        }
    }
    return ret;
}

Status AudioFfmpegEncoderPlugin::Flush()
{
    MEDIA_LOG_I("Flush entered.");
    OSAL::ScopedLock lock(avMutex_);
    if (avCodecContext_ != nullptr) {
        avcodec_flush_buffers(avCodecContext_.get());
    }
    MEDIA_LOG_I("Flush exit.");
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue input buffer");
    (void)timeoutMs;
    if (inputBuffer->IsEmpty() && !(inputBuffer->flag & BUFFER_FLAG_EOS)) {
        MEDIA_LOG_E("encoder does not support fd buffer");
        return Status::ERROR_INVALID_DATA;
    }
    Status ret = Status::OK;
    {
        OSAL::ScopedLock lock(avMutex_);
        if (avCodecContext_ == nullptr) {
            return Status::ERROR_WRONG_STATE;
        }
        ret = SendBufferLocked(inputBuffer);
    }
    return ret;
}

Status AudioFfmpegEncoderPlugin::QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_DD("queue output buffer");
    (void)timeoutMs;
    if (!outputBuffer) {
        return Status::ERROR_INVALID_PARAMETER;
    }
    outBuffer_ = outputBuffer;
    return SendOutputBuffer();
}

Status AudioFfmpegEncoderPlugin::SendOutputBuffer()
{
    MEDIA_LOG_DD("send output buffer");
    Status status = ReceiveBuffer();
    if (status == Status::OK || status == Status::END_OF_STREAM) {
        dataCallback_->OnOutputBufferDone(outBuffer_);
    }
    outBuffer_.reset();
    return status;
}

void AudioFfmpegEncoderPlugin::FillInFrameCache(const std::shared_ptr<Memory>& mem)
{
    uint8_t* sampleData = nullptr;
    int32_t nbSamples = 0;
    if (needReformat_) {
        std::vector<const uint8_t*> input(avCodecContext_->channels);
        input[0] = mem->GetReadOnlyData();
        if (av_sample_fmt_is_planar(srcFmt_)) {
            size_t lineSize = mem->GetSize() / avCodecContext_->channels;
            for (int i = 1; i < avCodecContext_->channels; ++i) {
                input[i] = input[i-1] + lineSize;
            }
        }
        auto res = swr_convert(swrCtx_.get(), resampleChannelAddr_.data(), avCodecContext_->frame_size,
                               input.data(), avCodecContext_->frame_size);
        if (res < 0) {
            MEDIA_LOG_E("resample input failed");
            nbSamples = 0;
        } else {
            sampleData = resampleCache_.data();
            nbSamples = res;
        }
    } else {
        sampleData = const_cast<uint8_t*>(mem->GetReadOnlyData());
        nbSamples = mem->GetSize() / srcBytesPerSample_;
    }
    cachedFrame_->format = avCodecContext_->sample_fmt;
    cachedFrame_->sample_rate = avCodecContext_->sample_rate;
    cachedFrame_->channels = avCodecContext_->channels;
    cachedFrame_->channel_layout = avCodecContext_->channel_layout;
    cachedFrame_->nb_samples = nbSamples;
    if (av_sample_fmt_is_planar(avCodecContext_->sample_fmt) && avCodecContext_->channels > 1) {
        if (avCodecContext_->channels > AV_NUM_DATA_POINTERS) {
            av_freep(cachedFrame_->extended_data);
            cachedFrame_->extended_data = static_cast<uint8_t**>(av_malloc_array(avCodecContext_->channels,
                sizeof(uint8_t *)));
        } else {
            cachedFrame_->extended_data = cachedFrame_->data;
        }
        cachedFrame_->extended_data[0] = sampleData;
        cachedFrame_->linesize[0] = nbSamples * av_get_bytes_per_sample(avCodecContext_->sample_fmt);
        for (int i = 1; i < avCodecContext_->channels; i++) {
            cachedFrame_->extended_data[i] = cachedFrame_->extended_data[i-1] + cachedFrame_->linesize[0];
        }
    } else {
        cachedFrame_->data[0] = sampleData;
        cachedFrame_->extended_data = cachedFrame_->data;
        cachedFrame_->linesize[0] = nbSamples * av_get_bytes_per_sample(avCodecContext_->sample_fmt) *
            avCodecContext_->channels;
    }
}


Status AudioFfmpegEncoderPlugin::SendBufferLocked(const std::shared_ptr<Buffer>& inputBuffer)
{
    bool eos = false;
    if (inputBuffer == nullptr || (inputBuffer->flag & BUFFER_FLAG_EOS) != 0) {
        // eos buffer
        eos = true;
    } else {
        auto inputMemory = inputBuffer->GetMemory();
        FALSE_RETURN_V_MSG_W(inputMemory->GetSize() == fullInputFrameSize_, Status::ERROR_NOT_ENOUGH_DATA,
            "Not enough data, input: " PUBLIC_LOG_ZU ", fullInputFrameSize: " PUBLIC_LOG_U32,
            inputMemory->GetSize(), fullInputFrameSize_);
        FillInFrameCache(inputMemory);
    }
    AVFrame* inputFrame = nullptr;
    if (!eos) {
        inputFrame = cachedFrame_.get();
    }
    auto ret = avcodec_send_frame(avCodecContext_.get(), inputFrame);
    if (!eos && inputFrame) {
        av_frame_unref(inputFrame);
    }
    if (ret == 0) {
        return Status::OK;
    } else if (ret == AVERROR_EOF) {
        return Status::END_OF_STREAM;
    } else if (ret == AVERROR(EAGAIN)) {
        return Status::ERROR_AGAIN;
    } else {
        MEDIA_LOG_E("send buffer error " PUBLIC_LOG_S, AVStrError(ret).c_str());
        return Status::ERROR_UNKNOWN;
    }
}

Status AudioFfmpegEncoderPlugin::ReceiveFrameSucc(const std::shared_ptr<Buffer>& ioInfo,
                                                  const std::shared_ptr<AVPacket>& packet)
{
    auto ioInfoMem = ioInfo->GetMemory();
    FALSE_RETURN_V_MSG_W(ioInfoMem->GetCapacity() >= static_cast<size_t>(packet->size),
                         Status::ERROR_NO_MEMORY, "buffer size is not enough");
    ioInfoMem->Write(packet->data, packet->size);
    // how get perfect pts with upstream pts ?
    ioInfo->duration = ConvertTimeFromFFmpeg(packet->duration, avCodecContext_->time_base);
    ioInfo->pts = (UINT64_MAX - prev_pts_ < packet->duration) ?
                  (ioInfo->duration - (UINT64_MAX - prev_pts_)) :
                  (prev_pts_ + ioInfo->duration);
    prev_pts_ = ioInfo->pts;
    return Status::OK;
}

Status AudioFfmpegEncoderPlugin::ReceiveBufferLocked(const std::shared_ptr<Buffer>& ioInfo)
{
    Status status;
    std::shared_ptr<AVPacket> packet = std::make_shared<AVPacket>();
    auto ret = avcodec_receive_packet(avCodecContext_.get(), packet.get());
    if (ret >= 0) {
        MEDIA_LOG_DD("receive one frame");
        status = ReceiveFrameSucc(ioInfo, packet);
    } else if (ret == AVERROR_EOF) {
        MEDIA_LOG_I("eos received");
        ioInfo->GetMemory()->Reset();
        ioInfo->flag = BUFFER_FLAG_EOS;
        status = Status::END_OF_STREAM;
    } else if (ret == AVERROR(EAGAIN)) {
        status = Status::ERROR_NOT_ENOUGH_DATA;
    } else {
        MEDIA_LOG_E("audio encoder receive error: " PUBLIC_LOG_S, AVStrError(ret).c_str());
        status = Status::ERROR_UNKNOWN;
    }
    av_frame_unref(cachedFrame_.get());
    return status;
}

Status AudioFfmpegEncoderPlugin::ReceiveBuffer()
{
    std::shared_ptr<Buffer> ioInfo = outBuffer_;
    if ((ioInfo == nullptr) || ioInfo->IsEmpty() ||
        (ioInfo->GetBufferMeta()->GetType() != BufferMetaType::AUDIO)) {
        MEDIA_LOG_W("cannot fetch valid buffer to output");
        return Status::ERROR_NO_MEMORY;
    }
    Status status = Status::OK;
    {
        OSAL::ScopedLock l(avMutex_);
        if (avCodecContext_ == nullptr) {
            return Status::ERROR_WRONG_STATE;
        }
        status = ReceiveBufferLocked(ioInfo);
    }
    return status;
}

std::shared_ptr<Allocator> AudioFfmpegEncoderPlugin::GetAllocator()
{
    return nullptr;
}
} // namespace Ffmpeg
} // namespace Plugin
} // namespace Media
} // namespace OHOS