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

#define HST_LOG_TAG "AudioSinkFilter"
#include "audio_sink_filter.h"
#include "common/plugin_utils.h"
#include "factory/filter_factory.h"
#include "foundation/log.h"
#include "foundation/osal/utils/util.h"
#include "pipeline/filters/common/dump_buffer.h"
#include "pipeline/filters/common/plugin_settings.h"
#include "pipeline/core/plugin_attr_desc.h"
#include "plugin/common/plugin_time.h"
#include "plugin/common/plugin_types.h"
#include "plugin/core/plugin_meta.h"
#include "utils/steady_clock.h"

namespace OHOS {
namespace Media {
namespace Pipeline {
namespace {
constexpr int REPORT_DURATION = 20 * HST_MSECOND; // 20 ms
constexpr int WAIT_PREROLLED_TIMEOUT = 80 * HST_MSECOND; // 80ms
}
static AutoRegisterFilter<AudioSinkFilter> g_registerFilterHelper("builtin.player.audiosink");

AudioSinkFilter::AudioSinkFilter(const std::string& name) : MediaSynchronousSink(name)
{
    filterType_ = FilterType::AUDIO_SINK;
    syncerPriority_ = IMediaSynchronizer::AUDIO_SINK;
    reportAnchorDuration_ = REPORT_DURATION;
    waitPrerolledTimeout_ = WAIT_PREROLLED_TIMEOUT;
    MEDIA_LOG_I("audio sink ctor called");
}
AudioSinkFilter::~AudioSinkFilter()
{
    MEDIA_LOG_D("audio sink dtor called");
    if (plugin_) {
        plugin_->Stop();
        plugin_->Deinit();
    }
}

void AudioSinkFilter::Init(EventReceiver* receiver, FilterCallback* callback)
{
    MediaSynchronousSink::Init(receiver, callback);
    outPorts_.clear();
}

ErrorCode AudioSinkFilter::SetPluginParameter(Tag tag, const Plugin::ValueType& value)
{
    return TranslatePluginStatus(plugin_->SetParameter(tag, value));
}

ErrorCode AudioSinkFilter::SetParameter(int32_t key, const Plugin::Any& value)
{
    if (state_.load() == FilterState::CREATED) {
        return ErrorCode::ERROR_AGAIN;
    }
    Tag tag = Tag::INVALID;
    if (!TranslateIntoParameter(key, tag)) {
        MEDIA_LOG_I("SetParameter key " PUBLIC_LOG_D32 " is out of boundary", key);
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    RETURN_AGAIN_IF_NULL(plugin_);
    return SetPluginParameter(tag, value);
}

ErrorCode AudioSinkFilter::GetParameter(int32_t key, Plugin::Any& value)
{
    if (state_.load() == FilterState::CREATED) {
        return ErrorCode::ERROR_AGAIN;
    }
    Tag tag = Tag::INVALID;
    if (!TranslateIntoParameter(key, tag)) {
        MEDIA_LOG_I("GetParameter key " PUBLIC_LOG_D32 " is out of boundary", key);
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    RETURN_AGAIN_IF_NULL(plugin_);
    return TranslatePluginStatus(plugin_->GetParameter(tag, value));
}

bool AudioSinkFilter::Negotiate(const std::string& inPort,
                                const std::shared_ptr<const Plugin::Capability>& upstreamCap,
                                Plugin::Capability& negotiatedCap,
                                const Plugin::TagMap& upstreamParams,
                                Plugin::TagMap& downstreamParams)
{
    MEDIA_LOG_I("audio sink negotiate started");
    PROFILE_BEGIN("Audio Sink Negotiate begin");
    auto candidatePlugins = FindAvailablePlugins(*upstreamCap, Plugin::PluginType::AUDIO_SINK);
    if (candidatePlugins.empty()) {
        MEDIA_LOG_E("no available audio sink plugin");
        return false;
    }
    // always use first one
    std::shared_ptr<Plugin::PluginInfo> selectedPluginInfo = candidatePlugins[0].first;
    for (const auto& onCap : selectedPluginInfo->inCaps) {
        if (onCap.keys.count(CapabilityID::AUDIO_SAMPLE_FORMAT) == 0) {
            MEDIA_LOG_E("each in caps of sink must contains valid audio sample format");
            return false;
        }
    }
    negotiatedCap = candidatePlugins[0].second;
    MEDIA_LOG_I("use plugin " PUBLIC_LOG_S " with negotiated " PUBLIC_LOG_S, selectedPluginInfo->name.c_str(),
                Capability2String(negotiatedCap).c_str());
    auto res = UpdateAndInitPluginByInfo<Plugin::AudioSink>(plugin_, pluginInfo_, selectedPluginInfo,
        [](const std::string& name) -> std::shared_ptr<Plugin::AudioSink> {
        return Plugin::PluginManager::Instance().CreateAudioSinkPlugin(name);
    });
    Plugin::ValueType pluginValue;
    if (plugin_->GetParameter(Tag::AUDIO_OUTPUT_CHANNELS, pluginValue) == Plugin::Status::OK) {
        auto outputChannels = Plugin::AnyCast<uint32_t>(pluginValue);
        downstreamParams.Insert<Tag::AUDIO_OUTPUT_CHANNELS>(outputChannels);
        MEDIA_LOG_D("Get support outputChannels: " PUBLIC_LOG_U32, outputChannels);
    }
    if (plugin_->GetParameter(Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT, pluginValue) == Plugin::Status::OK) {
        auto outputChanLayout = Plugin::AnyCast<Plugin::AudioChannelLayout>(pluginValue);
        downstreamParams.Insert<Tag::AUDIO_OUTPUT_CHANNEL_LAYOUT>(outputChanLayout);
        MEDIA_LOG_D("Get support outputChannelLayout: " PUBLIC_LOG_U64, outputChanLayout);
    }
    PROFILE_END("Audio Sink Negotiate end");
    return res;
}

bool AudioSinkFilter::Configure(const std::string& inPort, const std::shared_ptr<const Plugin::Meta>& upstreamMeta)
{
    PROFILE_BEGIN("Audio sink configure begin");
    MEDIA_LOG_I("receive upstream meta " PUBLIC_LOG_S, Meta2String(*upstreamMeta).c_str());
    if (plugin_ == nullptr || pluginInfo_ == nullptr) {
        MEDIA_LOG_E("cannot configure decoder when no plugin available");
        return false;
    }
    SetVolumeToPlugin();
    auto err = ConfigureToPreparePlugin(upstreamMeta);
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("sink configure error");
        OnEvent({name_, EventType::EVENT_ERROR, err});
        return false;
    }
    UpdateMediaTimeRange(*upstreamMeta);
    state_ = FilterState::READY;
    OnEvent({name_, EventType::EVENT_READY});
    MEDIA_LOG_I("audio sink send EVENT_READY");
    PROFILE_END("Audio sink configure end");
    return true;
}

ErrorCode AudioSinkFilter::ConfigureToPreparePlugin(const std::shared_ptr<const Plugin::Meta>& meta)
{
    FAIL_RETURN_MSG(ConfigPluginWithMeta(*plugin_, *meta), "sink configuration failed.");
    FAIL_RETURN_MSG(TranslatePluginStatus(plugin_->Prepare()), "sink prepare failed");
    return ErrorCode::SUCCESS;
}

ErrorCode AudioSinkFilter::PushData(const std::string& inPort, const AVBufferPtr& buffer, int64_t offset)
{
    MEDIA_LOG_DD("audio sink push data started, state: " PUBLIC_LOG_D32, state_.load());
    if (isFlushing || state_.load() == FilterState::INITIALIZED) {
        MEDIA_LOG_DD("audio sink is flushing ignore this buffer");
        return ErrorCode::SUCCESS;
    }
    if (state_.load() != FilterState::RUNNING) {
        pushThreadIsBlocking = true;
        MEDIA_LOG_I("audio sink push data wait.");
        OSAL::ScopedLock lock(mutex_);
        startWorkingCondition_.Wait(lock, [this] {
            return state_ == FilterState::RUNNING || state_ == FilterState::INITIALIZED || isFlushing;
        });
        pushThreadIsBlocking = false;
    }
    if (isFlushing || state_.load() == FilterState::INITIALIZED) {
        MEDIA_LOG_I("PushData return due to: isFlushing = " PUBLIC_LOG_D32 ", state = " PUBLIC_LOG_D32,
                    isFlushing, static_cast<int>(state_.load()));
        return ErrorCode::SUCCESS;
    }
    DUMP_BUFFER2LOG("AudioSink Write", buffer, offset);
    FAIL_RETURN_MSG(WriteToPluginRefTimeSync(buffer), "audio sink write failed");
    if ((buffer->flag & BUFFER_FLAG_EOS) != 0) {
        plugin_->Drain();
        Event event{
            .srcFilter = name_,
            .type = EventType::EVENT_COMPLETE,
        };
        MEDIA_LOG_D("audio sink push data send event_complete");
        OnEvent(event);
    }
    MEDIA_LOG_DD("audio sink push data end");
    return ErrorCode::SUCCESS;
}

ErrorCode AudioSinkFilter::Start()
{
    MEDIA_LOG_I("start called");
    if (state_ != FilterState::READY && state_ != FilterState::PAUSED) {
        MEDIA_LOG_W("sink is not ready when start, state: " PUBLIC_LOG_D32, static_cast<int32_t>(state_.load()));
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    forceUpdateTimeAnchorNextTime_ = true;
    auto err = FilterBase::Start();
    if (err != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("audio sink filter start error");
        return err;
    }
    err = TranslatePluginStatus(plugin_->Start());
    FAIL_RETURN_MSG(err, "audio sink plugin start failed");
    if (pushThreadIsBlocking.load()) {
        startWorkingCondition_.NotifyOne();
    }
    frameCnt_ = 0;
    return ErrorCode::SUCCESS;
}

ErrorCode AudioSinkFilter::Stop()
{
    MEDIA_LOG_I("audio sink stop start");
    FilterBase::Stop();
    if (plugin_ != nullptr) {
        plugin_->Stop();
    }
    if (pushThreadIsBlocking.load()) {
        startWorkingCondition_.NotifyOne();
    }
    MEDIA_LOG_I("audio sink stop finish");
    return ErrorCode::SUCCESS;
}

ErrorCode AudioSinkFilter::Pause()
{
    MEDIA_LOG_I("audio sink filter pause start");
    // only worked when state is working
    if (state_ != FilterState::READY && state_ != FilterState::RUNNING) {
        MEDIA_LOG_W("audio sink cannot pause when not working");
        return ErrorCode::ERROR_INVALID_OPERATION;
    }
    auto err = FilterBase::Pause();
    FAIL_RETURN_MSG(err, "audio sink pause failed");
    err = TranslatePluginStatus(plugin_->Pause());
    MEDIA_LOG_D("audio sink filter pause end");
    return err;
}
ErrorCode AudioSinkFilter::Resume()
{
    MEDIA_LOG_I("audio sink filter resume");
    // only worked when state is paused
    if (state_ == FilterState::PAUSED) {
        forceUpdateTimeAnchorNextTime_ = true;
        state_ = FilterState::RUNNING;
        if (pushThreadIsBlocking) {
            startWorkingCondition_.NotifyOne();
        }
        if (frameCnt_ > 0) {
            frameCnt_ = 0;
        }
        return TranslatePluginStatus(plugin_->Resume());
    }
    return ErrorCode::SUCCESS;
}

void AudioSinkFilter::FlushStart()
{
    MEDIA_LOG_I("audio sink flush start entered");
    isFlushing = true;
    if (pushThreadIsBlocking) {
        startWorkingCondition_.NotifyOne();
    }
    plugin_->Pause();
    plugin_->Flush();
}

void AudioSinkFilter::FlushEnd()
{
    MEDIA_LOG_I("audio sink flush end entered");
    plugin_->Resume();
    isFlushing = false;
    ResetSyncInfo();
}
ErrorCode AudioSinkFilter::SetVolumeToPlugin()
{
    if (plugin_ == nullptr) {
        return ErrorCode::SUCCESS;
    }
    if (volume_ < 0) {
        MEDIA_LOG_D("No need to set volume because upper layer hasn't set it.");
        return ErrorCode::SUCCESS;
    }
    return TranslatePluginStatus(plugin_->SetVolume(volume_));
}
ErrorCode AudioSinkFilter::SetVolume(float volume)
{
    volume_ = volume;
    MEDIA_LOG_I("set volume " PUBLIC_LOG ".3f", volume);
    return SetVolumeToPlugin();
}

ErrorCode AudioSinkFilter::DoSyncWrite(const AVBufferPtr &buffer)
{
    if ((buffer->flag & BUFFER_FLAG_EOS) == 0) { // only need to update when not eos
        // audio sink always report time anchor and do not drop
        int64_t nowCt = 0;
        auto syncCenter = syncCenter_.lock();
        if (syncCenter) {
            nowCt = syncCenter->GetClockTimeNow();
        }
        if (lastReportedClockTime_ == HST_TIME_NONE || lastReportedClockTime_ + reportAnchorDuration_ <= nowCt ||
            forceUpdateTimeAnchorNextTime_) {
            uint64_t latency = 0;
            if (plugin_->GetLatency(latency) != Plugin::Status::OK) {
                MEDIA_LOG_W("failed to get latency");
            }
            if (syncCenter) {
                syncCenter->UpdateTimeAnchor(nowCt + latency, buffer->pts, this);
            }
            lastReportedClockTime_ = nowCt;
            forceUpdateTimeAnchorNextTime_ = true;
        }
        latestBufferPts_ = buffer->pts;
        latestBufferDuration_ = buffer->duration;
    }
    return TranslatePluginStatus(plugin_->Write(buffer));
}

void AudioSinkFilter::ResetSyncInfo()
{
    ResetPrerollReported();
    lastReportedClockTime_ = HST_TIME_NONE;
    latestBufferPts_ = HST_TIME_NONE;
    latestBufferDuration_ = HST_TIME_NONE;
}
} // namespace Pipeline
} // namespace Media
} // namespace OHOS
