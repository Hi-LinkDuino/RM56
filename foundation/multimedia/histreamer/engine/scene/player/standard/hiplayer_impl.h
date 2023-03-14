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

#ifndef HISTREAMER_SCENE_STD_HIPLAYER_IMPL_H
#define HISTREAMER_SCENE_STD_HIPLAYER_IMPL_H

#include <memory>
#include <unordered_map>

#include "hiplayer_callback_looper.h"
#include "i_player_engine.h"
#include "filters/demux/demuxer_filter.h"
#include "filters/source/media_source/media_source_filter.h"
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"
#include "hiplayer_callback_looper.h"
#include "internal/state_machine.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/filter_callback.h"
#include "pipeline/core/media_sync_manager.h"
#include "pipeline/core/pipeline.h"
#include "pipeline/core/pipeline_core.h"
#include "pipeline/filters/codec/audio_decoder/audio_decoder_filter.h"
#ifdef VIDEO_SUPPORT
#include "pipeline/filters/codec/video_decoder/video_decoder_filter.h"
#include "pipeline/filters/sink/video_sink/video_sink_filter.h"
#endif
#include "pipeline/filters/sink/audio_sink/audio_sink_filter.h"
#include "scene/common/media_stat_stub.h"
#include "play_executor.h"

namespace OHOS {
namespace Media {
class HiPlayerImpl : public Pipeline::EventReceiver,
                     public PlayExecutor,
                     public StateChangeCallback,
                     public Pipeline::FilterCallback,
                     public IPlayerEngine {
    friend class StateMachine;

public:
    HiPlayerImpl();
    ~HiPlayerImpl() override;
    HiPlayerImpl(const HiPlayerImpl& other) = delete;
    HiPlayerImpl& operator=(const HiPlayerImpl& other) = delete;
    ErrorCode Init();
    // interface from PlayerInterface
    int32_t SetSource(const std::string& uri) override;
    int32_t SetSource(const std::shared_ptr<IMediaDataSource>& dataSrc) override;
    int32_t Prepare() override;
    int32_t PrepareAsync() override;
    int32_t Play() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Reset() override;
    int32_t Seek(int32_t mSeconds, PlayerSeekMode mode) override;
    int32_t SetVolume(float leftVolume, float rightVolume) override;
    int32_t SetVideoSurface(sptr<Surface> surface) override;
    int32_t SetLooping(bool loop) override;
    int32_t SetParameter(const Format &param) override;
    int32_t SetObs(const std::weak_ptr<IPlayerEngineObs>& obs) override;
    int32_t GetCurrentTime(int32_t& currentPositionMs) override;
    int32_t GetDuration(int32_t& durationMs) override;
    int32_t SetPlaybackSpeed(PlaybackRateMode mode) override;
    int32_t GetPlaybackSpeed(PlaybackRateMode& mode) override;

    int32_t GetVideoTrackInfo(std::vector<Format>& videoTrack) override;
    int32_t GetAudioTrackInfo(std::vector<Format>& audioTrack) override;
    int32_t GetVideoWidth() override;
    int32_t GetVideoHeight() override;

    // internal interfaces
    void OnEvent(const Event& event) override;
    void OnStateChanged(StateId state) override;
    ErrorCode OnCallback(const Pipeline::FilterCallbackType& type, Pipeline::Filter* filter,
                         const Plugin::Any& parameter) override;

    // interface from PlayExecutor
    bool IsSingleLoop() override;
    ErrorCode DoSetSource(const std::shared_ptr<MediaSource>& source) override;
    ErrorCode PrepareFilters() override;
    ErrorCode DoPlay() override;
    ErrorCode DoPause() override;
    ErrorCode DoResume() override;
    ErrorCode DoStop() override;
    ErrorCode DoReset() override;
    ErrorCode DoSeek(bool allowed, int64_t hstTime, Plugin::SeekMode mode, bool appTriggered) override;
    ErrorCode DoOnReady() override;
    ErrorCode DoOnComplete() override;
    ErrorCode DoOnError(ErrorCode) override;

private:
    ErrorCode StopAsync();
    ErrorCode SetVolumeToSink(float volume, bool reportUpward = true);
    Pipeline::PFilter CreateAudioDecoder(const std::string& desc);
    ErrorCode NewAudioPortFound(Pipeline::Filter* filter, const Plugin::Any& parameter);
#ifdef VIDEO_SUPPORT
    ErrorCode NewVideoPortFound(Pipeline::Filter* filter, const Plugin::Any& parameter);
#endif
    ErrorCode RemoveFilterChains(Pipeline::Filter* filter, const Plugin::Any& parameter);
    void ActiveFilters(const std::vector<Pipeline::Filter*>& filters);
    void HandlePluginErrorEvent(const Event& event);
    void UpdateStateNoLock(PlayerStates newState, bool notifyUpward = true);

    OSAL::Mutex stateMutex_ {};
    OSAL::ConditionVariable cond_ {};
    StateMachine fsm_;
    std::atomic<StateId> curFsmState_;
    std::shared_ptr<Pipeline::PipelineCore> pipeline_;
    std::atomic<PlayerStates> pipelineStates_ {PlayerStates::PLAYER_IDLE}; // only update in UpdateStateNoLock()
    std::queue<PlayerStates> pendingStates_ {};
    std::atomic<bool> initialized_ {false};

    std::weak_ptr<Plugin::Meta> sourceMeta_ {};
    std::vector<std::weak_ptr<Plugin::Meta>> streamMeta_ {};
    int64_t duration_ {-1};
    std::atomic<bool> singleLoop_ {false};
    float volume_;
    MediaStatStub mediaStats_;
    std::atomic<int64_t> lastSeekPosition_ {-1};
    std::atomic<Plugin::SeekMode> lastSeekMode_ {Plugin::SeekMode::SEEK_PREVIOUS_SYNC};
    std::atomic<bool> seekInProgress_ {false};

    std::shared_ptr<Pipeline::MediaSourceFilter> audioSource_;
    std::shared_ptr<Pipeline::DemuxerFilter> demuxer_;
    std::shared_ptr<Pipeline::AudioDecoderFilter> audioDecoder_;
    std::shared_ptr<Pipeline::AudioSinkFilter> audioSink_;
#ifdef VIDEO_SUPPORT
    std::shared_ptr<Pipeline::VideoDecoderFilter> videoDecoder_;
    std::shared_ptr<Pipeline::VideoSinkFilter> videoSink_;
#endif
    std::unordered_map<std::string, std::shared_ptr<Pipeline::AudioDecoderFilter>> audioDecoderMap_;
    std::shared_ptr<Pipeline::MediaSyncManager> syncManager_;
    HiPlayerCallbackLooper callbackLooper_ {};
};
}  // namespace Media
}  // namespace OHOS
#endif // HISTREAMER_SCENE_STD_HIPLAYER_IMPL_H
