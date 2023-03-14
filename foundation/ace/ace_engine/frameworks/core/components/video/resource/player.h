/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_PLAYER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_PLAYER_H

#include <list>

#include "core/animation/scheduler.h"
#include "core/components/common/layout/constants.h"
#include "core/components/video/resource/resource.h"
#include "core/components/video/resource/texture.h"

namespace OHOS::Ace {

constexpr uint32_t FREAUENCY_GET_CURRENT_TIME = 250; // Millisecond

class ACE_EXPORT Player : public Resource {
    DECLARE_ACE_TYPE(Player, Resource);

public:
    using PreparedListener = std::function<void(uint32_t, uint32_t, bool, uint32_t, uint32_t, bool)>;
    using PlayStatusListener = std::function<void(bool)>;
    using CurrentPosListener = std::function<void(uint32_t)>;
    using CompletionListener = std::function<void()>;
    using RefreshRenderListener = std::function<void()>;

    Player(int64_t textureId, const std::string& src, const WeakPtr<PipelineContext>& context, ErrorCallback&& onError)
        : Resource("video", context, std::move(onError)), textureId_(textureId), src_(src)
    {}
    ~Player() override = default;

    void Create(const std::function<void(int64_t)>& onCreate);
    void CreatePlayer(const std::function<void(int64_t)>& onCreate);

    uint32_t GetWidth() const
    {
        return width_;
    }

    uint32_t GetHeight() const
    {
        return height_;
    }

    uint32_t GetDuration() const
    {
        return duration_;
    }

    uint32_t GetCurrent() const
    {
        return currentPos_;
    }

    bool IsPlaying() const
    {
        return isPlaying_;
    }

    void SetMute(bool isMute)
    {
        isMute_ = isMute;
    }

    void SetAutoPlay(bool isAutoPlay)
    {
        isAutoPlay_ = isAutoPlay;
    }

    void AddPreparedListener(PreparedListener&& listener);
    void AddPlayStatusListener(PlayStatusListener&& listener);
    void AddCurrentPosListener(CurrentPosListener&& listener);
    void AddCompletionListener(CompletionListener&& listener);
    void AddRefreshRenderListener(RefreshRenderListener&& listener);

    void PopListener();
    void OnPopListener();
    void UnregisterEvent();

    // The following public functions should be called in UI thread.
    void Start();
    void Pause();
    void Stop();
    void SeekTo(uint32_t pos);
    void SetVolume(float volume);
    void EnterFullScreen();
    void EnableLooping(bool loop);
    void SetSpeed(float speed);
    void SetDirection(std::string& direction);
    void SetLandscape();

private:
    void OnAddPreparedListener(PreparedListener&& listener);
    void OnAddPlayStatusListener(PlayStatusListener&& listener);
    void OnAddCurrentPosListener(CurrentPosListener&& listener);
    void OnAddCompletionListener(CompletionListener&& listener);

    void OnStarted();
    void OnPaused();
    void OnTimeGetted(const std::string& result);

    void InitPlay();
    void OnPrepared(const std::string& param);
    void OnCompletion(const std::string& param);
    void OnSeekComplete(const std::string& param);
    void OnPlayStatus(const std::string& param);
    void GetCurrentTime();
    void OnTick(uint64_t timeStamp);
    void SetTickActive(bool isActive);

    int64_t textureId_ = INVALID_ID;
    std::string src_;
    uint32_t duration_ = 0;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t currentPos_ = 0;
    bool isPlaying_ = false;
    bool isMute_ = false;
    bool isAutoPlay_ = false;
    bool isPrepared_ = false;
    bool isNeedFreshForce_ = false;
    Method getCurrentPosMethod_;
    Method playMethod_;
    Method pauseMethod_;
    Method stopMethod_;
    Method seekMethod_;
    Method setVolumeMethod_;
    Method fullscreenMethod_;
    Method enableloopingMethod_;
    Method setSpeedMethod_;
    Method setDirectionMethod_;
    Method setLandsacpeMethod_;

    std::list<PreparedListener> onPreparedListener_;
    std::list<PlayStatusListener> onPlayStatusListener_;
    std::list<CurrentPosListener> onCurrentPosListener_;
    std::list<CompletionListener> onCompletionListener_;
    std::list<RefreshRenderListener> onRefreshRenderListener_;

    RefPtr<Scheduler> scheduler_;
    uint64_t timeStamp_ = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_RESOURCE_PLAYER_H
