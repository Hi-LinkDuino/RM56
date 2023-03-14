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

#ifndef HISTREAMER_SCENE_LITE_HIPLAYER_IMPL_H
#define HISTREAMER_SCENE_LITE_HIPLAYER_IMPL_H

#include <memory>
#include <unordered_map>

#include "common/any.h"
#ifdef VIDEO_SUPPORT
#include "filters/codec/video_decoder/video_decoder_filter.h"
#include "filters/sink/video_sink/video_sink_filter.h"
#endif
#include "filters/demux/demuxer_filter.h"
#include "filters/source/media_source/media_source_filter.h"
#include "internal/state_machine.h"
#include "osal/thread/condition_variable.h"
#include "osal/thread/mutex.h"
#include "pipeline/core/error_code.h"
#include "pipeline/core/filter_callback.h"
#include "pipeline/core/media_sync_manager.h"
#include "pipeline/core/pipeline.h"
#include "pipeline/core/pipeline_core.h"
#include "pipeline/filters/codec/audio_decoder/audio_decoder_filter.h"
#include "pipeline/filters/sink/audio_sink/audio_sink_filter.h"
#include "play_executor.h"
#include "scene/common/media_stat_stub.h"
#include "scene/lite/hiplayer.h"

namespace OHOS {
namespace Media {
class HiPlayerImpl : public Pipeline::EventReceiver,
                     public PlayExecutor,
                     public StateChangeCallback,
                     public Pipeline::FilterCallback,
                     public PlayerInterface {
    friend class StateMachine;

public:
    ~HiPlayerImpl() override;

    static std::shared_ptr<HiPlayerImpl> CreateHiPlayerImpl();

    // interface from PlayerInterface
    int32_t Init() override;
    int32_t DeInit() override;
    int32_t SetSource(const Source& source) override;
    int32_t Prepare() override;
    int32_t Play() override;
    bool IsPlaying() override;
    int32_t Pause() override;
    int32_t Stop() override;
    int32_t Reset() override;
    int32_t Release() override;
    int32_t Rewind(int64_t mSeconds, int32_t mode) override;
    int32_t SetVolume(float leftVolume, float rightVolume) override;
#ifndef SURFACE_DISABLED
    int32_t SetSurface(Surface* surface) override;
#endif
    bool IsSingleLooping() override;
    int32_t SetLoop(bool loop) override;
    void SetPlayerCallback(const std::shared_ptr<PlayerCallback>& cb) override;
    int32_t GetPlayerState(int32_t& state) override;
    int32_t GetCurrentPosition(int64_t& currentPositionMs) override;
    int32_t GetDuration(int64_t& outDurationMs) override;
    int32_t GetVideoWidth(int32_t& videoWidth) override;
    int32_t GetVideoHeight(int32_t& videoHeight) override;
    int32_t SetPlaybackSpeed(float speed) override;
    int32_t GetPlaybackSpeed(float& speed) override;
    int32_t SetAudioStreamType(int32_t type) override;
    void GetAudioStreamType(int32_t& type) override;
    int32_t SetParameter(const Format& params) override;

    void OnEvent(const Event& event) override;

    ErrorCode SetBufferSize(size_t size);

    ErrorCode GetSourceMeta(std::shared_ptr<const Plugin::Meta>& meta) const;
    ErrorCode GetTrackCnt(size_t& cnt) const;
    ErrorCode GetTrackMeta(size_t id, std::shared_ptr<const Plugin::Meta>& meta) const;

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
    PlayerStates TransStateId2PlayerState(StateId state);
    const std::string& StringnessPlayerState(PlayerStates state);
    static Plugin::SeekMode Transform2SeekMode(PlayerSeekMode mode);
    HiPlayerImpl();
    HiPlayerImpl(const HiPlayerImpl& other);
    HiPlayerImpl& operator=(const HiPlayerImpl& other);
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

    OSAL::Mutex stateMutex_;
    OSAL::ConditionVariable cond_;
    StateMachine fsm_;
    std::atomic<StateId> curFsmState_;

    std::shared_ptr<Pipeline::PipelineCore> pipeline_;
    std::atomic<PlayerStates> pipelineStates_ {PlayerStates::PLAYER_IDLE}; // only update in UpdateStateNoLock()
    std::queue<PlayerStates> pendingStates_ {};
    std::atomic<bool> initialized_ {false};

    std::shared_ptr<Pipeline::MediaSourceFilter> audioSource_;

    std::shared_ptr<Pipeline::DemuxerFilter> demuxer_;
    std::shared_ptr<Pipeline::AudioDecoderFilter> audioDecoder_;
    std::shared_ptr<Pipeline::AudioSinkFilter> audioSink_;
#ifdef VIDEO_SUPPORT
    std::shared_ptr<Pipeline::VideoDecoderFilter> videoDecoder;
    std::shared_ptr<Pipeline::VideoSinkFilter> videoSink;
#endif

    std::unordered_map<std::string, std::shared_ptr<Pipeline::AudioDecoderFilter>> audioDecoderMap_;

    std::weak_ptr<Plugin::Meta> sourceMeta_;
    std::vector<std::weak_ptr<Plugin::Meta>> streamMeta_;
    int64_t duration_ {-1};
    std::atomic<bool> singleLoop_ {false};

    std::weak_ptr<PlayerCallback> callback_;
    float volume_;
    MediaStatStub mediaStats_;

    std::shared_ptr<Pipeline::MediaSyncManager> syncManager_;
};
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_SCENE_LITE_HIPLAYER_IMPL_H
