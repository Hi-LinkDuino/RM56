
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
#include "scene/player/standard/media_utils.h"
#include "plugin/common/media_source.h"
#include "plugin/common/plugin_time.h"
#include "plugin/core/plugin_meta.h"
#include "utils/steady_clock.h"
#include "media_errors.h"

namespace {
const float MAX_MEDIA_VOLUME = 1.0f; // standard interface volume is between 0 to 1.
}

namespace OHOS {
namespace Media {
using namespace Pipeline;

HiPlayerImpl::HiPlayerImpl()
    : fsm_(*this),
      curFsmState_(StateId::IDLE),
      volume_(-1.0f), // default negative, if app not set, will not set it.
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
    videoSink_ =
        FilterFactory::Instance().CreateFilterWithType<VideoSinkFilter>("builtin.player.videosink", "videoSink");
    FALSE_RETURN(videoSink_ != nullptr);
    videoSink_->SetSyncCenter(syncManager_);
#endif
#endif
    FALSE_RETURN(audioSource_ != nullptr);
    FALSE_RETURN(demuxer_ != nullptr);
    FALSE_RETURN(audioSink_ != nullptr);
    audioSink_->SetSyncCenter(syncManager_);
    pipeline_ = std::make_shared<PipelineCore>();
    callbackLooper_.SetPlayEngine(this);
}

HiPlayerImpl::~HiPlayerImpl()
{
    MEDIA_LOG_I("dtor called.");
    fsm_.SendEventAsync(Intent::STOP);
    fsm_.Stop();
    callbackLooper_.Stop();
    audioSink_.reset();
#ifdef VIDEO_SUPPORT
    videoSink_.reset();
#endif
    syncManager_.reset();
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
        if (callbackLooper_.IsStarted()) {
            Format format;
            while (!pendingStates_.empty()) {
                auto pendingState = pendingStates_.front();
                pendingStates_.pop();
                MEDIA_LOG_I("sending pending state change: " PUBLIC_LOG_S, StringnessPlayerState(pendingState).c_str());
                callbackLooper_.OnInfo(INFO_TYPE_STATE_CHANGE, pendingState, format);
            }
            MEDIA_LOG_I("sending newest state change: " PUBLIC_LOG_S,
                        StringnessPlayerState(pipelineStates_.load()).c_str());
            callbackLooper_.OnInfo(INFO_TYPE_STATE_CHANGE, pipelineStates_, format);
        } else {
            pendingStates_.push(newState);
        }
    }
}

ErrorCode HiPlayerImpl::Init()
{
    MEDIA_LOG_I("Init entered.");
    mediaStats_.Reset();
    if (initialized_.load()) {
        return ErrorCode::SUCCESS;
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
    return ret;
}

int32_t HiPlayerImpl::SetSource(const std::string& uri)
{
    MEDIA_LOG_I("SetSource entered source uri: " PUBLIC_LOG_S, uri.c_str());
    PROFILE_BEGIN("SetSource begin");
    auto ret = Init();
    if (ret == ErrorCode::SUCCESS) {
        ret = fsm_.SendEvent(Intent::SET_SOURCE, std::make_shared<MediaSource>(uri));
    }
    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("SetSource error: " PUBLIC_LOG_S, GetErrorName(ret));
    }
    PROFILE_END("SetSource end.");
    return TransErrorCode(ret);
}

int32_t HiPlayerImpl::SetSource(const std::shared_ptr<IMediaDataSource>& dataSrc)
{
    MEDIA_LOG_W("SetSource only support url format source!");
    return TransErrorCode(ErrorCode::ERROR_UNIMPLEMENTED);
}

int32_t HiPlayerImpl::Prepare()
{
    MEDIA_LOG_I("Prepare entered, current fsm state: " PUBLIC_LOG_S ".", fsm_.GetCurrentState().c_str());
    PROFILE_BEGIN();
    auto ret = fsm_.SendEvent(Intent::PREPARE);
    if (ret != ErrorCode::SUCCESS) {
        PROFILE_END("Prepare failed,");
        MEDIA_LOG_E("Prepare failed with error " PUBLIC_LOG_D32, ret);
        return TransErrorCode(ret);
    }
    OSAL::ScopedLock lock(stateMutex_);
    if (curFsmState_ == StateId::PREPARING) { // Wait state change to ready
        cond_.Wait(lock, [this] { return curFsmState_ != StateId::PREPARING; });
    }
    MEDIA_LOG_D("Prepare finished, current fsm state: " PUBLIC_LOG "s.", fsm_.GetCurrentState().c_str());
    PROFILE_END("Prepare finished, current fsm state: " PUBLIC_LOG "s.", fsm_.GetCurrentState().c_str());
    if (curFsmState_ == StateId::READY) {
        return TransErrorCode(ErrorCode::SUCCESS);
    }
    return TransErrorCode(ErrorCode::ERROR_UNKNOWN);
}

int HiPlayerImpl::PrepareAsync()
{
    MEDIA_LOG_I("Prepare async entered, current fsm state: " PUBLIC_LOG_S, fsm_.GetCurrentState().c_str());
    PROFILE_BEGIN();
    auto ret = fsm_.SendEventAsync(Intent::PREPARE);
    if (ret != ErrorCode::SUCCESS) {
        PROFILE_END("Prepare async failed,");
        MEDIA_LOG_E("Prepare async failed with error " PUBLIC_LOG_D32, ret);
    } else {
        PROFILE_END("Prepare async successfully,");
    }
    return TransErrorCode(ret);
}

int32_t HiPlayerImpl::Play()
{
    MEDIA_LOG_I("Play entered.");
    callbackLooper_.StartReportMediaProgress();
    PROFILE_BEGIN();
    seekInProgress_.store(false);
    ErrorCode ret;
    if (pipelineStates_ == PlayerStates::PLAYER_PAUSED) {
        ret = fsm_.SendEvent(Intent::RESUME);
    } else {
        ret = fsm_.SendEvent(Intent::PLAY);
    }
    PROFILE_END("Play ret = " PUBLIC_LOG_D32, TransErrorCode(ret));
    return TransErrorCode(ret);
}

int32_t HiPlayerImpl::Pause()
{
    MEDIA_LOG_I("Pause entered.");
    PROFILE_BEGIN();
    auto ret = TransErrorCode(fsm_.SendEvent(Intent::PAUSE));
    callbackLooper_.StopReportMediaProgress();
    callbackLooper_.ManualReportMediaProgressOnce();
    PROFILE_END("Pause ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

int32_t HiPlayerImpl::Stop()
{
    MEDIA_LOG_I("Stop entered.");
    PROFILE_BEGIN();
    auto ret = TransErrorCode(fsm_.SendEvent(Intent::STOP));
    callbackLooper_.StopReportMediaProgress();
    callbackLooper_.ManualReportMediaProgressOnce();
    PROFILE_END("Stop ret = " PUBLIC_LOG_D32, ret);
    return ret;
}

ErrorCode HiPlayerImpl::StopAsync()
{
    MEDIA_LOG_I("StopAsync entered.");
    return fsm_.SendEventAsync(Intent::STOP);
}

int32_t HiPlayerImpl::Seek(int32_t mSeconds, PlayerSeekMode mode)
{
    MEDIA_LOG_I("Seek entered. mSeconds : " PUBLIC_LOG_D32 ", seekMode : " PUBLIC_LOG_D32,
                mSeconds, static_cast<int32_t>(mode));
    int64_t hstTime = 0;
    int32_t durationMs = 0;
    NZERO_RETURN(GetDuration(durationMs));
    MEDIA_LOG_D("Seek durationMs : " PUBLIC_LOG_D32, durationMs);
    if (mSeconds >= durationMs) { // if exceeds change to duration
        mSeconds = durationMs;
    }
    if (audioSource_->GetSeekable() != Plugin::Seekable::SEEKABLE) {
        MEDIA_LOG_E("Seek, invalid operation, audio source is unseekable or invalid");
        return MSERR_INVALID_OPERATION;
    }
    if (!Plugin::Ms2HstTime(mSeconds, hstTime)) {
        return TransErrorCode(ErrorCode::ERROR_INVALID_PARAMETER_VALUE);
    }
    auto smode = Transform2SeekMode(mode);
    lastSeekPosition_.store(hstTime);
    lastSeekMode_.store(smode);
    if (!seekInProgress_.load()) {
        seekInProgress_.store(true);
        return TransErrorCode(fsm_.SendEventAsync(Intent::SEEK, SeekInfo{hstTime, smode}));
    } else {
        MEDIA_LOG_D("Seek in progress. Record the seek request [" PUBLIC_LOG_D64 "," PUBLIC_LOG_D32 "]",
                    hstTime, static_cast<int32_t>(smode));
        return MSERR_OK;
    }
}

int32_t HiPlayerImpl::SetVolume(float leftVolume, float rightVolume)
{
    MEDIA_LOG_I("SetVolume entered.");
    if (leftVolume < 0 || leftVolume > MAX_MEDIA_VOLUME || rightVolume < 0 || rightVolume > MAX_MEDIA_VOLUME) {
        MEDIA_LOG_E("volume not valid, should be in range [0,100]");
        return TransErrorCode(ErrorCode::ERROR_INVALID_PARAMETER_VALUE);
    }
    float volume = 0.0f;
    if (leftVolume < 1e-6 && rightVolume >= 1e-6) {  // 1e-6
        volume = rightVolume;
    } else if (rightVolume < 1e-6 && leftVolume >= 1e-6) {  // 1e-6
        volume = leftVolume;
    } else {
        volume = (leftVolume + rightVolume) / 2;  // 2
    }
    volume /= MAX_MEDIA_VOLUME;  // normalize to 0~1
    if (pipelineStates_ == PlayerStates::PLAYER_STOPPED) {
        return TransErrorCode(ErrorCode::ERROR_INVALID_OPERATION);
    }
    volume_ = volume;
    if (pipelineStates_ == PlayerStates::PLAYER_IDLE || pipelineStates_ == PlayerStates::PLAYER_INITIALIZED ||
        pipelineStates_ == PlayerStates::PLAYER_PREPARING || audioSink_ == nullptr) {
        MEDIA_LOG_W("cannot set volume, will do this onReady");
        return TransErrorCode(ErrorCode::SUCCESS);
    }
    return TransErrorCode(SetVolumeToSink(volume));
}

int32_t HiPlayerImpl::SetVideoSurface(sptr<Surface> surface)
{
    MEDIA_LOG_D("SetVideoSurface entered.");
#ifdef VIDEO_SUPPORT
    return TransErrorCode(videoSink_->SetVideoSurface(surface));
#else
    return TransErrorCode(ErrorCode::SUCCESS);
#endif
}

int32_t HiPlayerImpl::GetVideoTrackInfo(std::vector<Format>& videoTrack)
{
    MEDIA_LOG_I("GetVideoTrackInfo entered.");
    std::string mime;
    uint32_t height;
    uint32_t width;
    std::vector<std::shared_ptr<Plugin::Meta>> metaInfo = demuxer_->GetStreamMetaInfo();
    for (const auto& trackInfo : metaInfo) {
        if (trackInfo->GetString(Plugin::MetaID::MIME, mime)) {
            if (IsVideoMime(mime)) {
                Format videoTrackInfo {};
                (void)videoTrackInfo.PutStringValue("codec_mime", "video/mpeg");
                if (trackInfo->GetUint32(Plugin::MetaID::VIDEO_HEIGHT, height)) {
                    (void)videoTrackInfo.PutIntValue("height", static_cast<int32_t>(height));
                }
                if (trackInfo->GetUint32(Plugin::MetaID::VIDEO_WIDTH, width)) {
                    (void)videoTrackInfo.PutIntValue("width", static_cast<int32_t>(width));
                }
                videoTrack.push_back(videoTrackInfo);
            }
        }
    }
    return TransErrorCode(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::GetAudioTrackInfo(std::vector<Format>& audioTrack)
{
    MEDIA_LOG_I("GetAudioTrackInfo entered.");
    uint32_t audioSampleRate;
    int64_t audioBitRate;
    std::string mime;
    uint32_t audioChannels;
    std::vector<std::shared_ptr<Plugin::Meta>> metaInfo = demuxer_->GetStreamMetaInfo();
    for (const auto& trackInfo : metaInfo) {
        if (trackInfo->GetString(Plugin::MetaID::MIME, mime)) {
            if (IsAudioMime(mime)) {
                Format audioTrackInfo {};
                if (trackInfo->GetInt64(Plugin::MetaID::MEDIA_BITRATE, audioBitRate)) {
                    (void)audioTrackInfo.PutIntValue("bitrate", static_cast<int32_t>(audioBitRate));
                }
                if (trackInfo->GetUint32(Plugin::MetaID::AUDIO_CHANNELS, audioChannels)) {
                    (void)audioTrackInfo.PutIntValue("channel_count", static_cast<int32_t>(audioChannels));
                }
                (void)audioTrackInfo.PutStringValue("codec_mime", "audio/mpeg");
                if (trackInfo->GetUint32(Plugin::MetaID::AUDIO_SAMPLE_RATE, audioSampleRate)) {
                    (void)audioTrackInfo.PutIntValue("sample_rate", static_cast<int32_t>(audioSampleRate));
                }
                audioTrack.push_back(audioTrackInfo);
            }
        }
    }
    return TransErrorCode(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::GetVideoWidth()
{
    MEDIA_LOG_I("GetVideoWidth entered.");
    return TransErrorCode(ErrorCode::ERROR_UNIMPLEMENTED);
}

int32_t HiPlayerImpl::GetVideoHeight()
{
    MEDIA_LOG_I("GetVideoHeight entered.");
    return TransErrorCode(ErrorCode::ERROR_UNIMPLEMENTED);
}

void HiPlayerImpl::HandlePluginErrorEvent(const Event& event)
{
    Plugin::PluginEvent pluginEvent = Plugin::AnyCast<Plugin::PluginEvent>(event.param);
    MEDIA_LOG_I("Receive PLUGIN_ERROR, type:  " PUBLIC_LOG_D32, CppExt::to_underlying(pluginEvent.type));
    if (pluginEvent.type == Plugin::PluginEventType::CLIENT_ERROR &&
        pluginEvent.param.SameTypeWith(typeid(Plugin::NetworkClientErrorCode))) {
        auto netClientErrorCode = Plugin::AnyCast<Plugin::NetworkClientErrorCode>(pluginEvent.param);
        auto errorType {PlayerErrorType::PLAYER_ERROR_UNKNOWN};
        auto serviceErrCode { MSERR_UNKNOWN };
        if (netClientErrorCode == Plugin::NetworkClientErrorCode::ERROR_TIME_OUT) {
            errorType = PlayerErrorType::PLAYER_ERROR;
            serviceErrCode = MSERR_NETWORK_TIMEOUT;
        }
        callbackLooper_.OnError(errorType, serviceErrCode);
    }
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
    PROFILE_BEGIN();
    int64_t seekPos = hstTime;
    Plugin::SeekMode seekMode = mode;
    if (appTriggered && (hstTime != lastSeekPosition_.load() || mode != lastSeekMode_.load())) { // Newer seek happened
        seekPos = lastSeekPosition_.load();
        seekMode = lastSeekMode_.load();
    }
    auto rtv = allowed && seekPos >= 0 ? ErrorCode::SUCCESS : ErrorCode::ERROR_INVALID_OPERATION;
    if (rtv == ErrorCode::SUCCESS) {
        pipeline_->FlushStart();
        PROFILE_END("Flush start");
        PROFILE_RESET();
        pipeline_->FlushEnd();
        PROFILE_END("Flush end");
        PROFILE_RESET();
        syncManager_->Seek(seekPos);
        rtv = demuxer_->SeekTo(seekPos, seekMode);
        PROFILE_END("SeekTo");
    }
    if (rtv != ErrorCode::SUCCESS) {
        callbackLooper_.OnError(PLAYER_ERROR, TransErrorCode(rtv));
    } else {
        Format format;
        int64_t currentPos = Plugin::HstTime2Ms(seekPos);
        MEDIA_LOG_I("Seek done, currentPos : " PUBLIC_LOG_D64 ", appTriggered: " PUBLIC_LOG_D32,
                    currentPos, appTriggered);
        callbackLooper_.OnInfo(INFO_TYPE_SEEKDONE, static_cast<int32_t>(currentPos), format);
    }
    if (appTriggered && seekInProgress_.load()) {
        OSAL::SleepFor(10); // 10 wait seek real complete
        seekInProgress_.store(false);
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
    MEDIA_LOG_I("OnComplete looping: " PUBLIC_LOG_D32 ".", singleLoop_.load());
    Format format;
    callbackLooper_.OnInfo(INFO_TYPE_EOS, static_cast<int32_t>(singleLoop_.load()), format);
    if (!singleLoop_.load()) {
        callbackLooper_.StopReportMediaProgress();
        callbackLooper_.ManualReportMediaProgressOnce();
    }
    return ErrorCode::SUCCESS;
}

ErrorCode HiPlayerImpl::DoOnError(ErrorCode errorCode)
{
    UpdateStateNoLock(PlayerStates::PLAYER_STATE_ERROR);
    callbackLooper_.OnError(PLAYER_ERROR, TransErrorCode(errorCode));
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

    if (ret != ErrorCode::SUCCESS) {
        MEDIA_LOG_E("SetVolume failed with error " PUBLIC_LOG_D32, static_cast<int>(ret));
        callbackLooper_.OnError(PLAYER_ERROR, TransErrorCode(ret));
    } else if (reportUpward) {
        Format format;
        callbackLooper_.OnInfo(INFO_TYPE_VOLUME_CHANGE, volume, format);
    }
    return ret;
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

int32_t HiPlayerImpl::SetLooping(bool loop)
{
    MEDIA_LOG_I("SetLooping entered, loop: " PUBLIC_LOG_D32, loop);
    singleLoop_ = loop;
    return TransErrorCode(ErrorCode::SUCCESS);
}

bool HiPlayerImpl::IsSingleLoop()
{
    // note that we should also consider the live source, which cannot be singleLoop!
    return singleLoop_;
}

int32_t HiPlayerImpl::SetParameter(const Format& params)
{
    MEDIA_LOG_I("SetParameter entered.");
    return CppExt::to_underlying(ErrorCode::ERROR_UNIMPLEMENTED);
}

int32_t HiPlayerImpl::SetObs(const std::weak_ptr<IPlayerEngineObs>& obs)
{
    MEDIA_LOG_I("SetObs entered.");
    callbackLooper_.StartWithPlayerEngineObs(obs);
    return TransErrorCode(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::Reset()
{
    MEDIA_LOG_I("Reset entered.");
    singleLoop_ = false;
    mediaStats_.Reset();
    return TransErrorCode(fsm_.SendEvent(Intent::RESET));
}

int32_t HiPlayerImpl::GetCurrentTime(int32_t& currentPositionMs)
{
    currentPositionMs = Plugin::HstTime2Ms(syncManager_->GetMediaTimeNow());
    return TransErrorCode(ErrorCode::SUCCESS);
}

int32_t HiPlayerImpl::GetDuration(int32_t& durationMs)
{
    durationMs = 0;
    if (pipelineStates_ == PlayerStates::PLAYER_IDLE || pipelineStates_ == PlayerStates::PLAYER_PREPARING ||
        audioSource_ == nullptr) {
        MEDIA_LOG_E("GetDuration, invalid state or audioSource_ is null. state: " PUBLIC_LOG_S,
                    StringnessPlayerState(pipelineStates_).c_str());
        return MSERR_INVALID_STATE;
    }
    if (duration_ < 0) {
        durationMs = -1;
        MEDIA_LOG_W("no valid duration");
        return MSERR_UNKNOWN;
    }
    durationMs = Plugin::HstTime2Ms(duration_);
    MEDIA_LOG_DD("GetDuration returned " PUBLIC_LOG_D32, durationMs);
    return MSERR_OK;
}

int32_t HiPlayerImpl::SetPlaybackSpeed(PlaybackRateMode mode)
{
    MEDIA_LOG_I("SetPlaybackSpeed entered.");
    (void)mode;
    return TransErrorCode(ErrorCode::ERROR_UNIMPLEMENTED);
}
int32_t HiPlayerImpl::GetPlaybackSpeed(PlaybackRateMode& mode)
{
    MEDIA_LOG_I("GetPlaybackSpeed entered.");
    (void)mode;
    return TransErrorCode(ErrorCode::ERROR_UNIMPLEMENTED);
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
            videoDecoder_ = FilterFactory::Instance().CreateFilterWithType<VideoDecoderFilter>(
                "builtin.player.videodecoder", "videodecoder-" + portDesc.name);
            if (pipeline_->AddFilters({videoDecoder_.get()}) == ErrorCode::SUCCESS) {
                // link demuxer and video decoder
                auto fromPort = filter->GetOutPort(portDesc.name);
                auto toPort = videoDecoder_->GetInPort(PORT_NAME_DEFAULT);
                FAIL_LOG(pipeline_->LinkPorts(fromPort, toPort));  // link ports
                newFilters.emplace_back(videoDecoder_.get());

                // link video decoder and video sink
                if (pipeline_->AddFilters({videoSink_.get()}) == ErrorCode::SUCCESS) {
                    fromPort = videoDecoder_->GetOutPort(PORT_NAME_DEFAULT);
                    toPort = videoSink_->GetInPort(PORT_NAME_DEFAULT);
                    FAIL_LOG(pipeline_->LinkPorts(fromPort, toPort));  // link ports
                    newFilters.push_back(videoSink_.get());
                    mediaStats_.Append(videoSink_->GetName());
                }
            }
        }
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
}  // namespace Media
}  // namespace OHOS
