/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_COMPONENT_H

#include <string>

#include "base/utils/utils.h"
#include "core/components/image/image_component.h"
#include "core/components/video/resource/player.h"
#include "core/components/video/resource/texture.h"
#include "core/components/video/texture_component.h"
#include "core/components/video/video_utils.h"
#include "core/event/ace_event_helper.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

class VideoController : public virtual AceType {
    DECLARE_ACE_TYPE(VideoController, AceType);

public:
    using StartImpl = std::function<void()>;
    using PauseImpl = std::function<void()>;
    using StopImpl = std::function<void()>;
    using SeekToImpl = std::function<void(float, SeekMode)>;
    using RequestFullscreenImpl = std::function<void(bool)>;
    using ExitFullscreenImpl = std::function<void(bool)>;

    void Start()
    {
        if (startImpl_) {
            startImpl_();
        }
    }

    void Pause()
    {
        if (pauseImpl_) {
            pauseImpl_();
        }
    }

    void Stop()
    {
        if (stopImpl_) {
            stopImpl_();
        }
    }

    void SeekTo(float pos, SeekMode seekMode = SeekMode::SEEK_PREVIOUS_SYNC)
    {
        if (seekToImpl_) {
            seekToImpl_(pos, seekMode);
        }
    }

    void RequestFullscreen(bool isPortrait)
    {
        if (requestFullscreenImpl_) {
            requestFullscreenImpl_(isPortrait);
        }
    }

    void ExitFullscreen(bool isSync)
    {
        if (exitFullscreenImpl_) {
            exitFullscreenImpl_(isSync);
        }
    }

    void SetStartImpl(StartImpl&& startImpl)
    {
        startImpl_ = std::move(startImpl);
    }

    void SetPausetImpl(PauseImpl&& pauseImpl)
    {
        pauseImpl_ = std::move(pauseImpl);
    }

    void SetStopImpl(StopImpl&& stopImpl)
    {
        stopImpl_ = std::move(stopImpl);
    }

    void SetSeekToImpl(SeekToImpl&& seekToImpl)
    {
        seekToImpl_ = std::move(seekToImpl);
    }

    void SetRequestFullscreenImpl(RequestFullscreenImpl&& requestFullscreenImpl)
    {
        requestFullscreenImpl_ = std::move(requestFullscreenImpl);
    }

    void SetExitFullscreenImpl(ExitFullscreenImpl&& exitFullscreenImpl)
    {
        exitFullscreenImpl_ = std::move(exitFullscreenImpl);
    }

private:
    StartImpl startImpl_;
    PauseImpl pauseImpl_;
    StopImpl stopImpl_;
    SeekToImpl seekToImpl_;
    RequestFullscreenImpl requestFullscreenImpl_;
    ExitFullscreenImpl exitFullscreenImpl_;
};

// A component can show Video.
class ACE_EXPORT VideoComponent : public TextureComponent {
    DECLARE_ACE_TYPE(VideoComponent, TextureComponent);

public:
    using FullscreenEvent =
        std::function<RefPtr<Component>(bool, const WeakPtr<Player>&, const WeakPtr<Texture>&)>;

    explicit VideoComponent()
    {
        videoController_ = AceType::MakeRefPtr<VideoController>();
    };

    ~VideoComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    const std::string& GetSrc() const
    {
        return src_;
    }

    void SetSrc(const std::string& src)
    {
        src_ = src;
    }

    const std::string& GetPoster() const
    {
        return poster_;
    }

    void SetPoster(const std::string& poster)
    {
        poster_ = poster;
    }

    const RefPtr<ImageComponent>& GetPosterImage() const
    {
        return posterImage_;
    }

    void SetPosterImage(const RefPtr<ImageComponent>& posterImage)
    {
        posterImage_ = posterImage;
    }

    bool NeedControls() const
    {
        return needControls_;
    }

    void SetNeedControls(bool needControls)
    {
        needControls_ = needControls;
    }

    bool IsMute() const
    {
        return isMute_;
    }

    void SetMute(bool mute)
    {
        isMute_ = mute;
    }

    bool IsLoop() const
    {
        return isLoop_;
    }

    void SetLoop(bool loop)
    {
        isLoop_ = loop;
    }

    void SetSpeed(float speed)
    {
        if (GreatOrEqual(speed, 0.1) && LessOrEqual(speed, 20.0)) {
            speed_ = speed;
        }
    }

    float GetSpeed() const
    {
        return speed_;
    }

    void SetDirection(const std::string& direction)
    {
        if (direction == "vertical" || direction == "horizontal" || direction == "adapt" || direction == "auto") {
            direction_ = direction;
        }
    }

    const std::string& GetDirection() const
    {
        return direction_;
    }

    int32_t GetStartTime() const
    {
        return startTime_;
    }

    void SetStartTime(int32_t startTime)
    {
        startTime_ = startTime;
    }

    bool IsAutoPlay() const
    {
        return isAutoPlay_;
    }

    void SetAutoPlay(bool isAutoPlay)
    {
        isAutoPlay_ = isAutoPlay;
    }

    const EventMarker& GetPreparedEventId() const
    {
        return preparedEventId_;
    }

    void SetPreparedEventId(const EventMarker& eventId)
    {
        preparedEventId_ = eventId;
    }

    const EventMarker& GetStartEventId() const
    {
        return startEventId_;
    }

    void SetStartEventId(const EventMarker& eventId)
    {
        startEventId_ = eventId;
    }

    const EventMarker& GetPauseEventId() const
    {
        return pauseEventId_;
    }

    void SetPauseEventId(const EventMarker& eventId)
    {
        pauseEventId_ = eventId;
    }

    const EventMarker& GetStopEventId() const
    {
        return stopEventId_;
    }

    void SetStopEventId(const EventMarker& eventId)
    {
        stopEventId_ = eventId;
    }

    const EventMarker& GetFinishEventId() const
    {
        return finishEventId_;
    }

    void SetFinishEventId(const EventMarker& eventId)
    {
        finishEventId_ = eventId;
    }

    const EventMarker& GetErrorEventId() const
    {
        return errorEventId_;
    }

    void SetErrorEventId(const EventMarker& eventId)
    {
        errorEventId_ = eventId;
    }

    const EventMarker& GetSeekingEventId() const
    {
        return seekingEventId_;
    }

    void SetSeekingEventId(const EventMarker& eventId)
    {
        seekingEventId_ = eventId;
    }

    const EventMarker& GetSeekedEventId() const
    {
        return seekedEventId_;
    }

    void SetSeekedEventId(const EventMarker& eventId)
    {
        seekedEventId_ = eventId;
    }

    const EventMarker& GetTimeUpdateEventId() const
    {
        return timeUpdateEventId_;
    }

    void SetTimeUpdateEventId(const EventMarker& eventId)
    {
        timeUpdateEventId_ = eventId;
    }

    const EventMarker& GetFullscreenChangeEventId() const
    {
        return fullscreenChangeEventId_;
    }

    void SetFullscreenChangeEventId(const EventMarker& eventId)
    {
        fullscreenChangeEventId_ = eventId;
    }

    RefPtr<VideoController> GetVideoController() const
    {
        return videoController_;
    }

    void SetFullscreenEvent(FullscreenEvent&& fullscreenEvent)
    {
        fullscreenEvent_ = std::move(fullscreenEvent);
    }

    const FullscreenEvent& GetFullscreenEvent() const
    {
        return fullscreenEvent_;
    }

    bool IsFullscreen() const
    {
        return isFullscreen_;
    }

    void SetFullscreen(bool isFullscreen)
    {
        isFullscreen_ = isFullscreen;
    }

    void SetPlayer(const WeakPtr<Player>& player)
    {
        player_ = player;
    }

    WeakPtr<Player> GetPlayer() const
    {
        return player_;
    }

    void SetTexture(const WeakPtr<Texture>& texture)
    {
        texture_ = texture;
    }

    WeakPtr<Texture> GetTexture() const
    {
        return texture_;
    }

    virtual void SetVideoController(const RefPtr<VideoController>& videoController)
    {
        if (videoController) {
            videoController_ = videoController;
        }
    }

private:
    std::string src_;
    std::string poster_;
    bool isAutoPlay_ = false;
    bool needControls_ = true;
    bool isMute_ = false;
    bool isFullscreen_ = false;
    bool isLoop_ = false;
    int32_t startTime_ = 0;
    float speed_ = 1.0f;
    std::string direction_ = "auto";

    EventMarker preparedEventId_;
    EventMarker startEventId_;
    EventMarker pauseEventId_;
    EventMarker stopEventId_;
    EventMarker finishEventId_;
    EventMarker errorEventId_;
    EventMarker seekingEventId_;
    EventMarker seekedEventId_;
    EventMarker timeUpdateEventId_;
    EventMarker fullscreenChangeEventId_;
    FullscreenEvent fullscreenEvent_;

    RefPtr<ImageComponent> posterImage_;
    RefPtr<VideoController> videoController_;
    WeakPtr<Player> player_;
    WeakPtr<Texture> texture_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_COMPONENT_H
