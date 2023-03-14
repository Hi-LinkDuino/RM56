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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_ELEMENT_H

#include "core/components/common/properties/edge.h"
#include "core/components/multimodal/render_multimodal.h"
#include "core/components/slider/slider_theme.h"
#include "core/components/video/resource/player.h"
#include "core/components/video/resource/texture.h"
#include "core/components/video/video_component.h"
#include "core/components/video/video_theme.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

#ifdef OHOS_STANDARD_SYSTEM
#include "core/components/video/media_player_callback.h"
#include "foundation/multimedia/media_standard/interfaces/innerkits/native/media/include/player.h"
#include "window_manager.h"
#endif

namespace OHOS::Ace {

constexpr double VIDEO_CHILD_COMMON_FLEX_GROW = 1.0;
constexpr double VIDEO_CHILD_COMMON_FLEX_SHRINK = 1.0;
constexpr double VIDEO_CHILD_COMMON_FLEX_BASIS = 0.0;
constexpr uint32_t VIDEO_SEEK_STEP = 5;

class VideoElement : public RenderElement, public FocusNode {
    DECLARE_ACE_TYPE(VideoElement, RenderElement, FocusNode);

public:
    using EventCallback = std::function<void(const std::string&)>;
    using FullscreenEvent = std::function<RefPtr<Component>(bool, const WeakPtr<Player>&, const WeakPtr<Texture>&)>;
    using ErrorCallback = std::function<void(const std::string&, const std::string&)>;

    VideoElement() = default;
    ~VideoElement() override;

    void SetNewComponent(const RefPtr<Component>& newComponent) override;
    void Prepare(const WeakPtr<Element>& parent) override;
    void PerformBuild() override;
    void Dump() override;

    void Start();
    void Pause();
    void Stop();
    void SetCurrentTime(float currentPos, SeekMode seekMode = SeekMode::SEEK_PREVIOUS_SYNC);
    void FullScreen();
    void ExitFullScreen();
    void SetVolume(float volume);
    void SetSpeed(float speed);

protected:
    virtual void InitStatus(const RefPtr<VideoComponent>& videoComponent);
    bool IsDeclarativePara();
    virtual void OnPreFullScreen(bool isPortrait) {}

    bool isFullScreen_ = false;
    bool isInitialState_ = true; // Initial state is true. Play or seek will set it to true.
    bool isError_ = false;
    std::string direction_ = "auto";
    bool isExternalResource_ = false;
    RefPtr<Player> player_;
    RefPtr<Texture> texture_;
    EventCallback onFullScreenChange_;
    int32_t currentPlatformVersion_ = 0;

private:
    void OnError(const std::string& errorId, const std::string& param);
    void OnPrepared(
        uint32_t width, uint32_t height, bool isPlaying, uint32_t duration, uint32_t currentPos, bool needFireEvent);
    void OnPlayerStatus(PlaybackStatus status);
    void OnCurrentTimeChange(uint32_t currentPos);
    void OnCompletion();
    void OnStartBtnClick();
    void OnFullScreenBtnClick();
    void OnSliderChange(const std::string& param);
    void OnSliderMoving(const std::string& param);
    void IntTimeToText(uint32_t time, std::string& timeText);
    void InitEvent(const RefPtr<VideoComponent>& videoComponent);
    void SetMethodCall(const RefPtr<VideoComponent>& videoComponent);
    void SetRespondChildEvent();
    void CreatePlatformResource();
    void CreatePlayer(int64_t id, ErrorCallback&& errorCallback);
    void ReleasePlatformResource();
    void UpdataChild(const RefPtr<Component>& childComponent);
    void InitListener();
    void ResetStatus();
    bool OnKeyEvent(const KeyEvent& keyEvent) override;
    void OnKeyLeft();
    void OnKeyRight();
    void OnTextureRefresh();
    void HiddenChange(bool hidden);
    void OnTextureSize(int64_t textureId, int32_t textureWidth, int32_t textureHeight);
    void EnableLooping(bool loop);

    const RefPtr<Component> CreateChild();
    const RefPtr<Component> CreatePoster();
    const RefPtr<Component> CreateControl();
    const RefPtr<Component> CreateCurrentText();
    const RefPtr<Component> CreateDurationText();
    const RefPtr<Component> CreateSlider();
    const RefPtr<Component> CreatePlayBtn();
    const RefPtr<Component> CreateFullScreenBtn();
    const RefPtr<Component> CreateErrorText(const std::string& errorMsg);
    const RefPtr<Component> SetPadding(const RefPtr<Component>& component, Edge&& edge);

    void PrepareMultiModalEvent();
    bool SubscribeMultiModal();
    bool UnSubscribeMultiModal();
    bool HasPlayer() const;

    bool isSubscribeMultimodal_ = false;
    RefPtr<VideoTheme> theme_;
    RefPtr<SliderTheme> sliderTheme_;
    bool isStop_ = false;

    ImageFit imageFit_ { ImageFit::CONTAIN };
    ImageObjectPosition imagePosition_;

    bool hasSrcChanged_ = true;
    bool needControls_ = true;
    bool isAutoPlay_ = false;
    bool isMute_ = false;
    std::string src_;
    std::string poster_;
    RefPtr<ImageComponent> posterImage_;
    uint32_t duration_ = 0;
    uint32_t currentPos_ = 0;
    bool isPlaying_ = false;
    bool pastPlayingStatus_ = false; // Record the player status before dragging the progress bar.
    bool isReady_ = false;
    bool isElementPrepared_ = false;
    double videoWidth_ = 0.0;
    double videoHeight_ = 0.0;
    bool isLoop_ = false;
    int32_t startTime_ = 0;
    std::string durationText_;
    std::string currentPosText_;
    TextDirection textDirection_ = TextDirection::LTR;
    float speed_ = -1.0f;

    EventMarker shieldId_; // Shield the event on the control bar.
    EventMarker startBtnClickId_;
    EventMarker fullscreenBtnClickId_;
    EventMarker sliderMovedCallbackId_;
    EventMarker sliderMovingCallbackId_;
    EventCallback onPrepared_;
    EventCallback onStart_;
    EventCallback onPause_;
    EventCallback onStop_;
    EventCallback onFinish_;
    EventCallback onError_;
    EventCallback onSeeking_;
    EventCallback onSeeked_;
    EventCallback onTimeUpdate_;

    // multimodal required param
    MultimodalEventCallback multimodalEventFullscreen_;
    MultimodalEventCallback multimodalEventFullscreenExit_;
    MultimodalEventCallback multimodalEventPlay_;
    MultimodalEventCallback multimodalEventPause_;
    WeakPtr<MultiModalScene> multiModalScene_;
    VoiceEvent playVoiceEvent_;
    VoiceEvent pauseVoiceEvent_;
    VoiceEvent fullscreenVoiceEvent_;
    VoiceEvent exitFullscreenVoiceEvent_;

    FullscreenEvent fullscreenEvent_;

#ifdef OHOS_STANDARD_SYSTEM
    ::OHOS::sptr<::OHOS::Subwindow> CreateSubwindow();
    void RegistMediaPlayerEvent();
    void CreateMediaPlayer();
    void PreparePlayer();
    std::string GetAssetAbsolutePath(const std::string& fileName);

    std::shared_ptr<OHOS::Media::Player> mediaPlayer_ = nullptr;
    ::OHOS::sptr<::OHOS::Subwindow> subWindow_ = nullptr;
    std::shared_ptr<MediaPlayerCallback> mediaPlayerCallback_ = nullptr;
#endif
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_ELEMENT_H
