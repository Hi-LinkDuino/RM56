/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "AudioDecoderFilter"

#include "audio_decoder_filter.h"
#include "factory/filter_factory.h"
#include "filters/common/dump_buffer.h"
#include "osal/utils/util.h"
#include "pipeline/core/filter_codec_mode.h"
#include "pipeline/filters/codec/codec_filter_base.h"
#include "pipeline/filters/codec/codec_filter_factory.h"
#include "pipeline/filters/codec/sync_mode.h"
#include "pipeline/filters/codec/async_mode.h"
#include "utils/steady_clock.h"

namespace {
constexpr uint32_t DEFAULT_IN_BUFFER_POOL_SIZE = 5;
constexpr uint32_t DEFAULT_OUT_BUFFER_POOL_SIZE = 5;
constexpr int32_t MAX_SAMPLE_PER_FRAME = 10240; // 10240 set max samples per frame
};

namespace OHOS {
namespace Media {
namespace Pipeline {
AudioDecoderFilter::AudioDecoderFilter(const std::string& name, std::shared_ptr<CodecMode>& codecMode)
    : CodecFilterBase(name)
{
    MEDIA_LOG_D("audio decoder ctor called");
    filterType_ = FilterType::AUDIO_DECODER;
    bufferMetaType_ = Plugin::BufferMetaType::AUDIO;
    codecMode_ = codecMode;
}

AudioDecoderFilter::~AudioDecoderFilter()
{
    MEDIA_LOG_D("audio decoder dtor called");
    if (plugin_) {
        plugin_->Stop();
        plugin_->Deinit();
    }
    (void)codecMode_->Release();
}

ErrorCode AudioDecoderFilter::Prepare()
{
    MEDIA_LOG_I("audio decoder prepare called.");
#ifndef OHOH_LITE
    codecMode_->SetBufferPoolSize(static_cast<uint32_t>(DEFAULT_IN_BUFFER_POOL_SIZE),
                                  static_cast<uint32_t>(DEFAULT_OUT_BUFFER_POOL_SIZE));
#endif
    (void)codecMode_->Prepare();
    return CodecFilterBase::Prepare();
}

ErrorCode AudioDecoderFilter::Stop()
{
    MEDIA_LOG_D("audio decoder stop start.");
    FAIL_RETURN(CodecFilterBase::Stop());
    MEDIA_LOG_D("audio decoder stop end.");
    return ErrorCode::SUCCESS;
}

bool AudioDecoderFilter::Negotiate(const std::string& inPort,
                                   const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                                   Plugin::Capability& negotiatedCap,
                                   const Plugin::TagMap& upstreamParams,
                                   Plugin::TagMap& downstreamParams)
{
    FALSE_RETURN_V(CodecFilterBase::Negotiate(inPort, upstreamCap, negotiatedCap, upstreamParams, downstreamParams),
                   false);
    MEDIA_LOG_D("audio decoder negotiate end");
    return true;
}

bool AudioDecoderFilter::Configure(const std::string &inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    PROFILE_BEGIN("audio decoder configure begin");
    FALSE_RETURN_V(CodecFilterBase::Configure(inPort, upstreamMeta), false);
    PROFILE_END("audio decoder configure end");
    return true;
}

ErrorCode AudioDecoderFilter::PushData(const std::string &inPort, const AVBufferPtr& buffer, int64_t offset)
{
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED && state_ != FilterState::RUNNING) {
        MEDIA_LOG_W("pushing data to decoder when state is " PUBLIC_LOG_D32, static_cast<int>(state_.load()));
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    if (isFlushing_) {
        MEDIA_LOG_I("audio decoder is flushing, discarding this data from port " PUBLIC_LOG_S, inPort.c_str());
        return ErrorCode::SUCCESS;
    }
    DUMP_BUFFER2FILE("decoder_input.data", buffer);
    return codecMode_->PushData(inPort, buffer, offset);
}

void AudioDecoderFilter::FlushStart()
{
    MEDIA_LOG_D("audio decoder FlushStart entered.");
    codecMode_->FlushStart();
    CodecFilterBase::FlushStart();
    MEDIA_LOG_D("audio decoder FlushStart exit.");
}

void AudioDecoderFilter::FlushEnd()
{
    MEDIA_LOG_I("audio decoder FlushEnd entered");
    isFlushing_ = false;
    codecMode_->FlushEnd();
}

void AudioDecoderFilter::OnInputBufferDone(const std::shared_ptr<Plugin::Buffer>& input)
{
    MEDIA_LOG_D("AudioDecoderFilter::OnInputBufferDone");
}

void AudioDecoderFilter::OnOutputBufferDone(const std::shared_ptr<Plugin::Buffer>& output)
{
    codecMode_->OnOutputBufferDone(output);
}

uint32_t AudioDecoderFilter::CalculateBufferSize(const std::shared_ptr<const OHOS::Media::Plugin::Meta>& meta)
{
    using namespace OHOS::Media;
    uint32_t samplesPerFrame;
    if (!meta->GetUint32(Plugin::MetaID::AUDIO_SAMPLE_PER_FRAME, samplesPerFrame)) {
        return 0;
    }
    uint32_t channels;
    if (!meta->GetUint32(Plugin::MetaID::AUDIO_CHANNELS, channels)) {
        return 0;
    }
    Plugin::AudioSampleFormat format;
    if (!meta->GetData<Plugin::AudioSampleFormat>(Plugin::MetaID::AUDIO_SAMPLE_FORMAT, format)) {
        return 0;
    }
    return Pipeline::GetBytesPerSample(format) * samplesPerFrame * channels;
}

std::vector<Capability::Key> AudioDecoderFilter::GetRequiredOutCapKeys()
{
    std::vector<Capability::Key> capKey;
    capKey.push_back(Capability::Key::AUDIO_SAMPLE_FORMAT);
    return capKey;
}

void AudioDecoderFilter::UpdateParams(const std::shared_ptr<const Plugin::Meta>& upMeta,
                                      std::shared_ptr<Plugin::Meta>& meta)
{
    uint32_t samplesPerFrame = 0;
    if (GetPluginParameterLocked(Tag::AUDIO_SAMPLE_PER_FRAME, samplesPerFrame) != ErrorCode::SUCCESS) {
        MEDIA_LOG_W("Can't acquire samples per frame from decoder plugin: " PUBLIC_LOG_S, pluginInfo_->name.c_str());
        samplesPerFrame = MAX_SAMPLE_PER_FRAME;
    }
    (void)meta->SetUint32(Plugin::MetaID::AUDIO_SAMPLE_PER_FRAME, samplesPerFrame);
    bool useStreamChannelParams {false};
    auto iter = sinkParams_.Find(Plugin::Tag::AUDIO_OUTPUT_CHANNELS);
    if (iter != std::end(sinkParams_) && iter->second.SameTypeWith(typeid(uint32_t))) {
        auto outputChannels = Plugin::AnyCast<uint32_t>(iter->second);
        uint32_t upChannels {0};
        if (upMeta->GetUint32(Plugin::MetaID::AUDIO_CHANNELS, upChannels) && upChannels < outputChannels) {
            outputChannels = upChannels;
            useStreamChannelParams = true;
        }
        if (plugin_ != nullptr &&
            plugin_->SetParameter(Plugin::Tag::AUDIO_OUTPUT_CHANNELS, outputChannels) != Plugin::Status::OK) {
            MEDIA_LOG_W("Set outputChannels to plugin " PUBLIC_LOG_S " failed", plugin_->GetName().c_str());
        }
        (void)meta->SetUint32(Plugin::MetaID::AUDIO_OUTPUT_CHANNELS, outputChannels);
    }
    iter = sinkParams_.Find(Plugin::Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT);
    if (iter != std::end(sinkParams_) && iter->second.SameTypeWith(typeid(Plugin::AudioChannelLayout))) {
        auto outputChanLayout = Plugin::AnyCast<Plugin::AudioChannelLayout>(iter->second);
        Plugin::AudioChannelLayout upAudioChannelLayout;
        if (useStreamChannelParams && upMeta->GetData(Plugin::MetaID::AUDIO_CHANNEL_LAYOUT, upAudioChannelLayout)) {
            outputChanLayout = upAudioChannelLayout;
        }
        if (plugin_ != nullptr &&
            plugin_->SetParameter(Plugin::Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT, outputChanLayout) != Plugin::Status::OK) {
            MEDIA_LOG_W("Set outputChannelLayout to plugin " PUBLIC_LOG_S " failed", plugin_->GetName().c_str());
        }
        (void)meta->SetData(Plugin::MetaID::AUDIO_OUTPUT_CHANNEL_LAYOUT, outputChanLayout);
    }
}
} // Pipeline
} // Media
} // OHOS