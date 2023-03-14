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

#define HST_LOG_TAG "HiPlayerImpl"

#include "hiplayer_impl.h"
#include "foundation/log.h"
#include "pipeline/factory/filter_factory.h"
#include "plugin/common/plugin_time.h"
#include "plugin/core/plugin_meta.h"
#include "utils/steady_clock.h"

namespace {
const float MAX_MEDIA_VOLUME = 100.0f;
}

namespace OHOS {
namespace Media {
using namespace Pipeline;

HiPlayerImpl::HiPlayerImpl()
    : fsm_(*this),
      curFsmState_(StateId::IDLE),
      volume_(-1.0f),
      mediaStats_()
{
    MEDIA_LOG_I("hiPlayerImpl ctor");
    FilterFactory::Instance().Init();
    syncManager_ = std::make_shared<MediaSyncManager>();

    audioSource_ =
        FilterFactory::Instance().CreateFilterWithType<MediaSourceFilter>("builtin.player.mediasource", "mediaSource");

#ifdef UNIT_TEST
    demuxer_ = FilterFactory::Instance().CreateFilterWithType<DemuxerFilter>("builtin.player.demuxer", "demuxer");
    audioDecoder_ = FilterFactory::Instance().CreateFilterWithType<AudioDecoderFilter>("builtin.player.audiodecoder",
                                                                                       "audiodecoder");
    audioSink_ =
        FilterFactory::Instance().CreateFilterWithType<AudioSinkFilter>("builtin.player.audiosink", "audiosink");
#else
    demuxer_ = FilterFactory::Instance().CreateFilterWithType<DemuxerFilter>("builtin.player.demuxer", "demuxer");
    audioSink_ =
        FilterFactory::Instance().CreateFilterWithType<AudioSinkFilter>("builtin.player.audiosink", "audioSink");
#ifdef VIDEO_SUPPORT
    videoSink =
        FilterFactory::Instance().CreateFilterWithType<VideoSinkFilter>("builtin.player.videosink", "videoSink");
    FALSE_RETURN(videoSink != nullptr);
    videoSink->SetSyncCenter(syncManager_);
#endif
#endif
    FALSE_RETURN(audioSource_ != nullptr);
    FALSE_RETURN(demuxer_ != nullptr);
    FALSE_RETURN(audioSink_ != nullptr);
    audioSink_->SetSyncCenter(syncManager_);
    pipeline_ = std::make_shared<PipelineCore>();
}

void HiPlayerImpl::UpdateStateNoLock(PlayerStates newState, bool notifyUpward)
{
    if (pipelineStates_ == newState) {
        return;
    }
    pipelineStates_ = newState;
    if (pipelineStates_ == PlayerStates::PLAYER_IDLE) {
        MEDIA_LOG_W("do not report idle since audio player will report idle");
        return;
    }
    if (notifyUpward) {
        auto ptr = callback_.lock();
        if (ptr != nullptr) {
            while (!pendingStates_.empty()) {
                auto pendingState = pendingStates_.front();
                pendingStates_.pop();
                MEDIA_LOG_I("sending pending state change: " PUBLIC_LOG_S, StringnessPlayerState(pendingState).c_str());
            }
            MEDIA_LOG_I("State change to : " PUBLIC_LOG_S, StringnessPlayerState(pipelineStates_.load()).c_str());
        }
    }
}

HiPlayerImpl::~HiPlayerImpl()
{
    MEDIA_LOG_D("dtor called.");
    fsm_.SendEventAsync(Intent::STOP);
    fsm_.Stop();
#ifdef VIDEO_SUPPORT
    videoSink.reset();
#endif
    audioSink_.reset();
    syncManager_.reset();
}

std::shared_ptr<HiPlayerImpl> HiPlayerImpl::CreateHiPlayerImpl()
{
    return std::shared_ptr<HiPlayerImpl>(new (std::nothrow) HiPlayerImpl());
}

int32_t HiPlayerImpl::Init()
{
    mediaStats_.Reset();
    if (initialized_.load()) {
        return CppExt::to_underlying(ErrorCode::SUCCESS);
    }
    pipeline_->Init(this, this);
    ErrorCode ret = pipeline_->AddFilters({audioSource_.get(), demuxer_.get()});
    if (ret == ErrorCode::SUCCESS) {
        ret = pipeline_->LinkFilters({audioSource_.get(), demuxer_.get()});
    }
    if (ret == ErrorCode::SUCCESS) {
        fsm_.SetStateCallback(this);
        fsm_.Start();
        initialized_ = true;
    } else {
        pipeline_->UnlinkPrevFilters();
        pipeline_->RemoveFilterChain(audioSource_.get());
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return CppExt::to_underlying(ret);
}
int32_t HiPlayerImpl::SetSource(const Source& source)
{
    PROFILE_BEGIN("SetSource begin");
    auto ret = Init();
    if (ret == CppExt::to_underlying(ErrorCode::SUCCESS)) {
        std::shared_ptr<MediaSource> mediaSource = std::make_shared<MediaSource>(source.GetSourceUri());
        if (source.GetSourceType() == OHOS::Media::SourceType::SOURCE_TYPE_STREAM) {
            mediaSource = std::make_shared<MediaSource>(source.GetDataConsumer());
        }
        ret = CppExt::to_underlying(fsm_.SendEvent(Intent::SET_SOURCE, mediaSource));
    }
    if (ret != CppExt::to_underlying(ErrorCode::SUCCESS)) {
        MEDIA_LOG_E("SetSource error: " PUBLIC_LOG_D32, ret);
    }
    PROFILE_END("SetSource end.");
    return ret;
}

int32_t HiPlayerImpl::Prepare()
{
    MEDIA_LOG_I("Prepare entered, current fsm state: " PUBLIC_LOG_S ".", fsm_.GetCurrentState().c_str());
    PROFILE_BEGIN();
    auto ret = fsm_.SendEvent(Intent::PREPARE);
    if (ret != ErrorCode::SUCCESS) {
        PROFILE_END("Prepare failed,");
        MEDIA_LOG_E("prepare failed with error " PUBLIC_LOG_D32, ret);
        return CppExt::to_underlying(ret);
    }
    OSAL::ScopedLock lock(stateMutex_);
    if (curFsmState_ == StateId::PREPARING) { // Wait state change to ready
        cond_.Wait(lock, [this] { return curFsmState_ != StateId::PREPARING; });
    }
    MEDIA_LOG_D("Prepare finished, current fsm state: " PUBLIC_LOG "s.", fsm_.GetCurrentState().c_str());
    PROFILE_END("Prepare finished, current fsm state: " PUBLIC_LOG "s.", fsm_.GetCurrentState().c_str());
    if (curFsmState_ == StateId::READY) {
        return CppExt::to_underlying(ErrorCode::SUCCESS);
    }
    return CppExt::to_underlying(ErrorCode::ERROR_UNKNOWN);
}

PFilter HiPlayerImpl::CreateAudioDecoder(const std::string& desc)
{
    if (!audioDecoderMap_[desc]) {
        audioDecoderMap_[desc] = FilterFactory::Instance().CreateFilterWithType<AudioDecoderFilter>(
            "builtin.player.audiodecoder", "audiodecoder-" + desc);
        // set parameters to decoder.
    }
    return audioDecoderMap_[desc];
}

int32_t HiPlayerImpl::Play()
{
    PROFILE_BEGIN();
    ErrorCode ret;
    MEDIA_LOG_I("Play entered.");
    if (pipelineStates_ == PlayerStates::PLAYER_PAUSED) {
        ret = fsm_.SendEvent(Intent::RESUME);
    } else {
        ret = fsm_.SendEvent(Intent::PLAY);
    }
    PROFILE_END("Play ret = " PUBLIC_LOG_D32, CppExt::to_underlying(ret));
    return CppExt::to_underlying(ret);
}

bool HiPlayerImpl::IsPlaying()
{
    return pipelineStates_ == PlayerStates::PLAYER_STARTED;
}

int32_t HiPlayerImpl::Pause()
{
    PROFILE_BEGIN();
    MEDIA_LOG_I("Pause entered.");
    auto ret = CppExt::to_underlying(fsm_.SendEvent(Intent::PAUSE));
    PROFILE_END("Pause ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

int32_t HiPlayerImpl::Stop()
{
    PROFILE_BEGIN();
    MEDIA_LOG_I("Stop entered.");
    auto ret = CppExt::to_underlying(fsm_.SendEvent(Intent::STOP));
    PROFILE_END("Stop ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

ErrorCode HiPlayerImpl::StopAsync()
{
    MEDIA_LOG_I("StopAsync entered.");
    return fsm_.SendEventAsync(Intent::STOP);
}

int32_t HiPlayerImpl::Rewind(int64_t mSeconds, int32_t mode)
{
    MEDIA_LOG_I("Rewind entered.");
    int64_t hstTime = 0;
    int64_t durationMs = 0;
    NZERO_RETURN(GetDuration(durationMs));
    MEDIA_LOG_D("Rewind durationMs : " PUBLIC_LOG_D64, durationMs);
    if (mSeconds >= durationMs) { // if exceeds change to duration
        mSeconds = durationMs;
    }
    if (audioSource_->GetSeekable() != Plugin::Seekable::SEEKABLE) {
        MEDIA_LOG_E("Seek, invalid operation, audio source is unseekable or invalid");
        return CppExt::to_underlying(ErrorCode::ERROR_INVALID_OPERATION);
    }
    if (!Plugin::Ms2HstTime(mSeconds, hstTime)) {
        return CppExt::to_underlying(ErrorCode::ERROR_INVALID_PARAMETER_VALUE);
    }
    auto smode = Transform2SeekMode(static_cast<PlayerSeekMode>(mode));
    return CppExt::to_underlying(fsm_.SendEventAsync(Intent::SEEK, SeekInfo{hstTime, smode}));
}

int32_t HiPlayerImpl::SetVolume(float leftVolume, float rightVolume)
{
    if (leftVolume < 0 || leftVolume > MAX_MEDIA_VOLUME || rightVolume < 0 || rightVolume > MAX_MEDIA_VOLUME) {
        MEDIA_LOG_E("volume not valid, should be in range [0,100]");
        return CppExt::to_underlying(ErrorCode::ERROR_INVALID_PARAMETER_VALUE);
    }
    float volume = 0.0f;
    if (leftVolume < 1e-6 && rightVolume >= 1e-6) { // 1e-6
        volume = rightVolume;
    } else if (rightVolume < 1e-6 && leftVolume >= 1e-6) { // 1e-6
        volume = leftVolume;
    } else {
        volume = (leftVolume + rightVolume) / 2; // 2
    }
    volume /= MAX_MEDIA_VOLUME; // normalize to 0~1
    if (pipelineStates_ == PlayerStates::PLAYER_STOPPED) {
        return CppExt::to_underlying(ErrorCode::ERROR_INVALID_OPERATION);
    }
    volume_ = volume;
    if (pipelineStates_ == PlayerStates::PLAYER_IDLE || pipelineStates_ == PlayerStates::PLAYER_INITIALIZED ||
        pipelineStates_ == PlayerStates::PLAYER_PREPARING || audioSink_ == nullptr) {
        MEDIA_LOG_W("cannot set volume, will do this onReady");
        return CppExt::to_underlying(ErrorCode::SUCCESS);
    }
    return CppExt::to_underlying(SetVolumeToSink(volume_));
}

#ifndef SURFACE_DISABLED
int32_t HiPlayerImpl::SetSurface(Surface* surface)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}
#endif
ErrorCode HiPlayerImpl::SetBufferSize(size_t size)
{
    return audioSource_->SetBufferSize(size);
}

void HiPlayerImpl::OnEvent(const Event& event)
{
    if (event.type != EventType::EVENT_AUDIO_PROGRESS) {
        MEDIA_LOG_I("[HiStreamer] OnEvent (" PUBLIC_LOG_S ")", GetEventName(event.type));
    }
    switch (event.type) {
        case EventType::EVENT_ERROR: {
            fsm_.SendEventAsync(Intent::NOTIFY_ERROR, event.param);
            break;
        }
        case EventType::EVENT_READY:
            fsm_.SendEventAsync(Intent::NOTIFY_READY);
            break;
        case EventType::EVENT_COMPLETE:
            mediaStats_.ReceiveEvent(event);
            if (mediaStats_.IsEventCompleteAllReceived()) {
                fsm_.SendEventAsync(Intent::NOTIFY_COMPLETE);
            }
            break;
        case EventType::EVENT_PLUGIN_ERROR: {
            HandlePluginErrorEvent(event);
            break;
        }
        case EventType::EVENT_PLUGIN_EVENT: {
            Plugin::PluginEvent pluginEvent = Plugin::AnyCast<Plugin::PluginEvent>(event.param);
            if (pluginEvent.type == Plugin::PluginEventType::BELOW_LOW_WATERLINE ||
                pluginEvent.type == Plugin::PluginEventType::ABOVE_LOW_WATERLINE) {
                MEDIA_LOG_I("Receive PLUGIN_EVENT, type:  " PUBLIC_LOG_D32, CppExt::to_underlying(pluginEvent.type));
            }
            break;
        }
        default:
            MEDIA_LOG_E("Unknown event(" PUBLIC_LOG_U32 ")", event.type);
    }
}

ErrorCode HiPlayerImpl::DoSetSource(const std::shared_ptr<MediaSource>& source)
{
    auto ret = audioSource_->SetSource(source);
    if (ret != ErrorCode::SUCCESS) {
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return ret;
}

ErrorCode HiPlayerImpl::PrepareFilters()
{
    auto ret = pipeline_->Prepare();
    if (ret != ErrorCode::SUCCESS) {
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return ret;
}

ErrorCode HiPlayerImpl::DoPlay()
{
    syncManager_->Resume();
    auto ret = pipeline_->Start();
    if (ret != ErrorCode::SUCCESS) {
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return ret;
}

ErrorCode HiPlayerImpl::DoPause()
{
    auto ret = pipeline_->Pause();
    syncManager_->Pause();
    if (ret != ErrorCode::SUCCESS) {
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return ret;
}

ErrorCode HiPlayerImpl::DoResume()
{
    syncManager_->Resume();
    auto ret = pipeline_->Resume();
    if (ret != ErrorCode::SUCCESS) {
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return ret;
}

ErrorCode HiPlayerImpl::DoStop()
{
    MEDIA_LOG_I("HiPlayerImpl DoStop called, stop pipeline.");
    mediaStats_.Reset();
    auto ret = pipeline_->Stop();
    syncManager_->Reset();
    if (ret != ErrorCode::SUCCESS) {
        UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    }
    return ret;
}

ErrorCode HiPlayerImpl::DoReset()
{
    return DoStop();
}

ErrorCode HiPlayerImpl::DoSeek(bool allowed, int64_t hstTime, Plugin::SeekMode mode, bool appTriggered)
{
    (void)appTriggered;
    PROFILE_BEGIN();
    auto rtv = allowed && hstTime >= 0 ? ErrorCode::SUCCESS : ErrorCode::ERROR_INVALID_OPERATION;
    if (rtv == ErrorCode::SUCCESS) {
        pipeline_->FlushStart();
        PROFILE_END("Flush start");
        PROFILE_RESET();
        pipeline_->FlushEnd();
        PROFILE_END("Flush end");
        PROFILE_RESET();
        syncManager_->Seek(hstTime);
        rtv = demuxer_->SeekTo(hstTime, mode);
        PROFILE_END("SeekTo");
    }
    auto ptr = callback_.lock();
    if (ptr != nullptr) {
        if (rtv != ErrorCode::SUCCESS) {
            ptr->OnError(CppExt::to_underlying(PlayerErrorTypeExt::SEEK_ERROR), CppExt::to_underlying(rtv));
        } else {
            ptr->OnRewindToComplete();
        }
    }
    return rtv;
}

ErrorCode HiPlayerImpl::DoOnReady()
{
    SetVolumeToSink(volume_, false); // do not report
    auto tmpMeta = demuxer_->GetGlobalMetaInfo();
    sourceMeta_ = tmpMeta;
    int64_t duration = 0;
    bool found = false;
    if (tmpMeta->GetInt64(Media::Plugin::MetaID::MEDIA_DURATION, duration)) {
        found = true;
    }
    streamMeta_.clear();
    int64_t tmp = 0;
    for (auto& streamMeta : demuxer_->GetStreamMetaInfo()) {
        streamMeta_.push_back(streamMeta);
        if (streamMeta->GetInt64(Media::Plugin::MetaID::MEDIA_DURATION, tmp)) {
            duration = std::max(duration, tmp);
            found = true;
        }
    }
    if (found) {
        duration_ = duration;
    }
    return ErrorCode::SUCCESS;
}

ErrorCode HiPlayerImpl::DoOnComplete()
{
    MEDIA_LOG_W("OnComplete looping: " PUBLIC_LOG_D32 ".", singleLoop_.load());
    auto ptr = callback_.lock();
    FALSE_RETURN_V_MSG(ptr != nullptr, ErrorCode::SUCCESS, "Player callback not exist.");
    ptr->OnPlaybackComplete();
    return ErrorCode::SUCCESS;
}

ErrorCode HiPlayerImpl::DoOnError(ErrorCode errorCode)
{
    UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    auto ptr = callback_.lock();
    FALSE_RETURN_V_MSG(ptr != nullptr, ErrorCode::SUCCESS, "Player callback not exist.");
    ptr->OnError(PlayerCallback::PlayerCallback::PLAYER_ERROR_UNKNOWN, static_cast<int32_t>(errorCode));
    return ErrorCode::SUCCESS;
}

ErrorCode HiPlayerImpl::SetVolumeToSink(float volume, bool reportUpward)
{
    MEDIA_LOG_I("SetVolumeToSink entered.");
    ErrorCode ret = ErrorCode::SUCCESS;
    if (volume_ > 0) {
        MEDIA_LOG_I("set volume " PUBLIC_LOG_F, volume);
        ret = audioSink_->SetVolume(volume);
    }
    auto ptr = callback_.lock();
    if (ptr != nullptr) {
        if (ret != ErrorCode::SUCCESS) {
            MEDIA_LOG_E("SetVolume failed with error " PUBLIC_LOG_D32, static_cast<int>(ret));
            ptr->OnError(PlayerCallback::PlayerCallback::PLAYER_ERROR_UNKNOWN, CppExt::to_underlying(ret));
        }
    }
    return ret;
}

bool HiPlayerImpl::IsSingleLooping()
{
    return singleLoop_.load();
}

int32_t HiPlayerImpl::SetLoop(bool loop)
{
    MEDIA_LOG_I("SetLoop entered, loop: " PUBLIC_LOG_D32, loop);
    singleLoop_ = loop;
    return CppExt::to_underlying(ErrorCode::SUCCESS);
}

bool HiPlayerImpl::IsSingleLoop()
{
    // note that we should also consider the live source, which cannot be singleLoop!
    return singleLoop_;
}

void HiPlayerImpl::SetPlayerCallback(const std::shared_ptr<PlayerCallback>& cb)
{
    callback_ = cb;
}

int32_t HiPlayerImpl::Reset()
{
    MEDIA_LOG_I("Reset entered.");
    singleLoop_ = false;
    mediaStats_.Reset();
    return CppExt::to_underlying(fsm_.SendEvent(Intent::RESET));
}

int32_t HiPlayerImpl::Release()
{
    PROFILE_BEGIN();
    auto ret = Reset();
    fsm_.Stop();
    pipeline_.reset();
    audioSource_.reset();
    demuxer_.reset();
    audioDecoderMap_.clear();
    audioSink_.reset();
    PROFILE_END("Release ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

int32_t HiPlayerImpl::DeInit()
{
    return Reset();
}

int32_t HiPlayerImpl::GetPlayerState(int32_t& state)
{
    state = static_cast<int32_t>(pipelineStates_.load());
    return CppExt::to_underlying(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::GetCurrentPosition(int64_t& currentPositionMs)
{
    currentPositionMs = Plugin::HstTime2Ms(syncManager_->GetMediaTimeNow());
    return CppExt::to_underlying(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::GetDuration(int64_t& outDurationMs)
{
    MEDIA_LOG_I("GetDuration entered.");
    outDurationMs = 0;
    if (pipelineStates_ == PlayerStates::PLAYER_IDLE || pipelineStates_ == PlayerStates::PLAYER_PREPARING
        || audioSource_ == nullptr) {
        return CppExt::to_underlying(ErrorCode::ERROR_INVALID_STATE);
    }
    if (duration_ < 0) {
        outDurationMs = -1;
        MEDIA_LOG_W("no valid duration");
        return CppExt::to_underlying(ErrorCode::ERROR_UNKNOWN);
    }
    outDurationMs = Plugin::HstTime2Ms(duration_);
    MEDIA_LOG_I("GetDuration returned " PUBLIC_LOG_D32, outDurationMs);
    return CppExt::to_underlying(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::GetVideoWidth(int32_t& videoWidth)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

int32_t HiPlayerImpl::GetVideoHeight(int32_t& videoHeight)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

void HiPlayerImpl::HandlePluginErrorEvent(const Event& event)
{
    Plugin::PluginEvent pluginEvent = Plugin::AnyCast<Plugin::PluginEvent>(event.param);
    MEDIA_LOG_I("Receive PLUGIN_ERROR, type:  " PUBLIC_LOG_D32, CppExt::to_underlying(pluginEvent.type));
    if (pluginEvent.type == Plugin::PluginEventType::CLIENT_ERROR &&
        pluginEvent.param.SameTypeWith(typeid(Plugin::NetworkClientErrorCode))) {
        auto netClientErrorCode = Plugin::AnyCast<Plugin::NetworkClientErrorCode>(pluginEvent.param);
        auto errorCode {-1};
        if (pluginEvent.type == Plugin::PluginEventType::CLIENT_ERROR &&
            pluginEvent.param.SameTypeWith(typeid(Plugin::NetworkClientErrorCode))&&
            Plugin::AnyCast<Plugin::NetworkClientErrorCode>(pluginEvent.param)
            == Plugin::NetworkClientErrorCode::ERROR_TIME_OUT) {
            errorCode = CppExt::to_underlying(Plugin::NetworkClientErrorCode::ERROR_TIME_OUT);
        }
        auto ptr = callback_.lock();
        if (ptr != nullptr) {
            ptr->OnError(PlayerCallback::PlayerErrorType::PLAYER_ERROR_UNKNOWN, errorCode);
        }
    }
}
int32_t HiPlayerImpl::SetPlaybackSpeed(float speed)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

int32_t HiPlayerImpl::GetPlaybackSpeed(float& speed)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

int32_t HiPlayerImpl::SetAudioStreamType(int32_t type)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

void HiPlayerImpl::GetAudioStreamType(int32_t& type)
{
    type = -1;
}

int32_t HiPlayerImpl::SetParameter(const Format& params)
{
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

void HiPlayerImpl::OnStateChanged(StateId state)
{
    MEDIA_LOG_I("OnStateChanged from " PUBLIC_LOG_D32 " to " PUBLIC_LOG_D32, curFsmState_.load(), state);
    UpdateStateNoLock(TransStateId2PlayerState(state));
    {
        OSAL::ScopedLock lock(stateMutex_);
        curFsmState_ = state;
        cond_.NotifyOne();
    }
}

ErrorCode HiPlayerImpl::OnCallback(const FilterCallbackType& type, Filter* filter, const Plugin::Any& parameter)
{
    ErrorCode ret = ErrorCode::SUCCESS;
    switch (type) {
        case FilterCallbackType::PORT_ADDED:
            ret = NewAudioPortFound(filter, parameter);
#ifdef VIDEO_SUPPORT
            ret = NewVideoPortFound(filter, parameter);
#endif
            break;
        case FilterCallbackType::PORT_REMOVE:
            ret = RemoveFilterChains(filter, parameter);
            break;
        default:
            break;
    }
    return ret;
}

ErrorCode HiPlayerImpl::GetTrackCnt(size_t& cnt) const
{
    cnt = streamMeta_.size();
    return ErrorCode::SUCCESS;
}

ErrorCode HiPlayerImpl::GetSourceMeta(shared_ptr<const Plugin::Meta>& meta) const
{
    meta = sourceMeta_.lock();
    return meta ? ErrorCode::SUCCESS : ErrorCode::ERROR_AGAIN;
}

ErrorCode HiPlayerImpl::GetTrackMeta(size_t id, shared_ptr<const Plugin::Meta>& meta) const
{
    if (id > streamMeta_.size() || id < 0) {
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    meta = streamMeta_[id].lock();
    if (meta == nullptr) {
        return ErrorCode::ERROR_AGAIN;
    }
    return ErrorCode::SUCCESS;
}

ErrorCode HiPlayerImpl::NewAudioPortFound(Filter* filter, const Plugin::Any& parameter)
{
    if (!parameter.SameTypeWith(typeid(PortInfo))) {
        return ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
    }
    ErrorCode rtv = ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    auto param = Plugin::AnyCast<PortInfo>(parameter);
    if (filter == demuxer_.get() && param.type == PortType::OUT) {
        MEDIA_LOG_I("new port found on demuxer " PUBLIC_LOG_ZU, param.ports.size());
        for (const auto& portDesc : param.ports) {
            if (portDesc.name.compare(0, 5, "audio") != 0) { // 5 is length of "audio"
                continue;
            }
            MEDIA_LOG_I("port name " PUBLIC_LOG_S, portDesc.name.c_str());
            auto fromPort = filter->GetOutPort(portDesc.name);
            if (portDesc.isPcm) {
                pipeline_->AddFilters({audioSink_.get()});
                FAIL_LOG(pipeline_->LinkPorts(fromPort, audioSink_->GetInPort(PORT_NAME_DEFAULT)));
                ActiveFilters({audioSink_.get()});
            } else {
                auto newAudioDecoder = CreateAudioDecoder(portDesc.name);
                pipeline_->AddFilters({newAudioDecoder.get(), audioSink_.get()});
                FAIL_LOG(pipeline_->LinkPorts(fromPort, newAudioDecoder->GetInPort(PORT_NAME_DEFAULT)));
                FAIL_LOG(pipeline_->LinkPorts(newAudioDecoder->GetOutPort(PORT_NAME_DEFAULT),
                                              audioSink_->GetInPort(PORT_NAME_DEFAULT)));
                ActiveFilters({newAudioDecoder.get(), audioSink_.get()});
            }
            mediaStats_.Append(audioSink_->GetName());
            rtv = ErrorCode::SUCCESS;
            break;
        }
    }
    return rtv;
}

#ifdef VIDEO_SUPPORT
ErrorCode HiPlayerImpl::NewVideoPortFound(Filter* filter, const Plugin::Any& parameter)
{
    if (!parameter.SameTypeWith(typeid(PortInfo))) {
        return ErrorCode::ERROR_INVALID_PARAMETER_TYPE;
    }
    auto param = Plugin::AnyCast<PortInfo>(parameter);
    if (filter != demuxer_.get() || param.type != PortType::OUT) {
        return ErrorCode::ERROR_INVALID_PARAMETER_VALUE;
    }
    std::vector<Filter*> newFilters;
    for (const auto& portDesc : param.ports) {
        if (portDesc.name.compare(0, 5, "video") == 0) { // 5 is length of "video"
            MEDIA_LOG_I("port name " PUBLIC_LOG_S, portDesc.name.c_str());
            videoDecoder = FilterFactory::Instance().CreateFilterWithType<VideoDecoderFilter>(
                "builtin.player.videodecoder", "videodecoder-" + portDesc.name);
            if (pipeline_->AddFilters({videoDecoder.get()}) == ErrorCode::SUCCESS) {
                // link demuxer and video decoder
                auto fromPort = filter->GetOutPort(portDesc.name);
                auto toPort = videoDecoder->GetInPort(PORT_NAME_DEFAULT);
                FAIL_LOG(pipeline_->LinkPorts(fromPort, toPort)); // link ports
                newFilters.emplace_back(videoDecoder.get());

                // link video decoder and video sink
                if (pipeline_->AddFilters({videoSink.get()}) == ErrorCode::SUCCESS) {
                    fromPort = videoDecoder->GetOutPort(PORT_NAME_DEFAULT);
                    toPort = videoSink->GetInPort(PORT_NAME_DEFAULT);
                    FAIL_LOG(pipeline_->LinkPorts(fromPort, toPort)); // link ports
                    newFilters.push_back(videoSink.get());
                    mediaStats_.Append(videoSink->GetName());
                }
            }
        }
        break;
    }
    if (!newFilters.empty()) {
        ActiveFilters(newFilters);
    }
    return ErrorCode::SUCCESS;
}
#endif

ErrorCode HiPlayerImpl::RemoveFilterChains(Filter* filter, const Plugin::Any& parameter)
{
    ErrorCode ret = ErrorCode::SUCCESS;
    auto param = Plugin::AnyCast<PortInfo>(parameter);
    if (filter != demuxer_.get() || param.type != PortType::OUT) {
        return ret;
    }
    for (const auto& portDesc : param.ports) {
        MEDIA_LOG_I("remove filter chain for port: " PUBLIC_LOG_S, portDesc.name.c_str());
        auto peerPort = filter->GetOutPort(portDesc.name)->GetPeerPort();
        if (peerPort) {
            auto nextFilter = const_cast<Filter*>(dynamic_cast<const Filter*>(peerPort->GetOwnerFilter()));
            if (nextFilter) {
                pipeline_->RemoveFilterChain(nextFilter);
            }
        }
    }
    return ret;
}

void HiPlayerImpl::ActiveFilters(const std::vector<Filter*>& filters)
{
    for (auto it = filters.rbegin(); it != filters.rend(); ++it) {
        (*it)->Prepare();
    }
}

PlayerStates HiPlayerImpl::TransStateId2PlayerState(StateId state)
{
    PlayerStates playerState = PLAYER_STATE_ERROR;
    switch (state) {
        case StateId::IDLE:
            playerState = PLAYER_IDLE;
            break;
        case StateId::INIT:
            playerState = PLAYER_INITIALIZED;
            break;
        case StateId::PREPARING:
            playerState = PLAYER_PREPARING;
            break;
        case StateId::READY:
            playerState = PLAYER_PREPARED;
            break;
        case StateId::PAUSE:
            playerState = PLAYER_PAUSED;
            break;
        case StateId::PLAYING:
            playerState = PLAYER_STARTED;
            break;
        case StateId::STOPPED:
            playerState = PLAYER_STOPPED;
            break;
        case StateId::EOS:
            playerState = PLAYER_PLAYBACK_COMPLETE;
            break;
        default:
            break;
    }
    return playerState;
}

Plugin::SeekMode HiPlayerImpl::Transform2SeekMode(PlayerSeekMode mode)
{
    switch (mode) {
        case PlayerSeekMode::PLAYER_SEEK_NEXT_SYNC:
            return Plugin::SeekMode::SEEK_NEXT_SYNC;
        case PlayerSeekMode::PLAYER_SEEK_PREVIOUS_SYNC:
            return Plugin::SeekMode::SEEK_PREVIOUS_SYNC;
        case PlayerSeekMode::PLAYER_SEEK_CLOSEST_SYNC:
            return Plugin::SeekMode::SEEK_CLOSEST_SYNC;
        case PlayerSeekMode::PLAYER_SEEK_CLOSEST:
            return Plugin::SeekMode::SEEK_CLOSEST;
        default:
            return Plugin::SeekMode::SEEK_CLOSEST;
    }
}

const std::string& HiPlayerImpl::StringnessPlayerState(PlayerStates state)
{
    using StateString = std::pair<PlayerStates, std::string>;
    const static std::array<StateString, 9> maps = { // array size
        std::make_pair(PlayerStates::PLAYER_STATE_ERROR, "state error"),
        std::make_pair(PlayerStates::PLAYER_IDLE, "idle"),
        std::make_pair(PlayerStates::PLAYER_INITIALIZED, "init"),
        std::make_pair(PlayerStates::PLAYER_PREPARING, "preparing"),
        std::make_pair(PlayerStates::PLAYER_PREPARED, "prepared"),
        std::make_pair(PlayerStates::PLAYER_STARTED, "started"),
        std::make_pair(PlayerStates::PLAYER_PAUSED, "paused"),
        std::make_pair(PlayerStates::PLAYER_STOPPED, "stopped"),
        std::make_pair(PlayerStates::PLAYER_PLAYBACK_COMPLETE, "completed"),
    };
    const static std::string unknown = "unknown";
    auto ite = std::find_if(maps.begin(), maps.end(), [&] (const StateString& item) -> bool {
        return item.first == state;
    });
    if (ite == maps.end()) {
        return unknown;
    }
    return ite->second;
}
} // namespace Media
} // namespace OHOS
