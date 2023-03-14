/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup UI_Components
 * @{
 *
 * @brief Defines UI components such as buttons, texts, images, lists, and progress bars.
 *
 * @since 5.0
 * @version 3.0
 */

/**
 * @file ui_video.h
 *
 * @brief Declares the functions related to video playbacks.
 *
 * @since 5.0
 * @version 3.0
 */

#ifndef GRAPHIC_LITE_UI_VIDEO_H
#define GRAPHIC_LITE_UI_VIDEO_H

#include "animator/animator.h"
#include "components/ui_label.h"
#include "components/ui_slider.h"
#include "components/ui_surface_view.h"
#include "components/ui_toggle_button.h"
#include "components/ui_view_group.h"
#include "player.h"

#ifndef VERSION_LITE
namespace OHOS {
using namespace OHOS::Media;
/**
 * @brief Provides the functions related to video playbacks.
 *
 * @since 5.0
 * @version 3.0
 */
class UIVideo : public UIViewGroup,
                public UIView::OnClickListener,
                public UIView::OnTouchListener,
                public UISlider::UISliderEventListener {
public:
    /**
     * @brief A constructor used to create a <b>UIVideo</b> instance for playback.
     *
     * @since 5.0
     * @version 3.0
     */
    UIVideo();

    /**
     * @brief A destructor used to delete the <b>UIVideo</b> instance for playback.
     *
     * @since 5.0
     * @version 3.0
     */
    virtual ~UIVideo();

    /**
     * @brief Sets the source file to be played.
     *
     * @param source Indicates the pointer to the source file path.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool SetSrc(const char* source);

    /**
     * @brief Obtains the path of the source file to be played.
     *
     * @return Returns the path of the source file to be played.
     * @since 5.0
     * @version 3.0
     */
    const char* GetSrc()
    {
        return src_;
    }

    /**
     * @brief Prepares for the playback. You must call this function after {@link SetSource}.
     *
     * @return Returns <b>true</b> if the preparation is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool Prepare();

    /**
     * @brief Plays this video.
     *
     * @return Returns <b>true</b> if this video is played; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool Play();

    /**
     * @brief Checks whether this video is playing.
     *
     * @return Returns <b>true</b> if this video is playing; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool IsPlaying();

    /**
     * @brief Pauses the video being played.
     *
     * @return Returns <b>true</b> if the video is paused; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool Pause();

    /**
     * @brief Stops playing this video.
     *
     * @return Returns <b>true</b> if this video is stopped; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool Stop();

    /**
     * @brief Changes the playback position.
     *
     * @param mSeconds Indicates the target playback position, in milliseconds.
     * @return Returns <b>true</b> if the playback position is changed; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool Rewind(int64_t mSeconds);

    /**
     * @brief Sets the playback volume.
     *
     * @param Indicates the volume of the left audio channel to set, ranging from <b>0</b> to <b>100</b>.
     * @param rightVolume Indicates the volume of the right audio channel to set, ranging from <b>0</b> to <b>1</b>.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool SetVolume(float leftVolume, float rightVolume);

    /**
     * @brief Sets whether to loop playback.
     *
     * @param loop Specifies whether to loop playback. The value <b>true</b> means that the playback loops,
     *             and <b>false</b> means the opposite case.
     * @return Returns <b>true</b> if the setting is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool EnableSingleLooping(bool loop);

    /**
     * @brief Checks whether the playback loops.
     *
     * @return Returns <b> true</b> if the playback loops; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool IsSingleLooping();

    /**
     * @brief Obtains the current playback time.
     *
     * @param time Indicates the current playback time, in milliseconds.
     * @return Returns <b> true</b> if the operation is successful; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool GetCurrentTime(int64_t& time);

    /**
     * @brief Obtains the total video duration.
     *
     * @param duration Indicates the total duration, in milliseconds.
     * @return Returns <b> true</b> if the duration is obtained; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool GetDuration(int64_t& duration);

    /**
     * @brief Resets the player to the initial state.
     *
     * @return Returns <b>true</b> if the player is reset; returns <b>false</b> otherwise.
     * @since 5.0
     * @version 3.0
     */
    bool Reset();

    /**
     * @brief Sets whether to show the playback controller.
     *
     * @param show Specifies whether to show the playback controller. The value <b>true</b> means showing
     *             the playback controller, and <b>false</b> means the opposite case.
     * @since 5.0
     * @version 3.0
     */
    void ShowController(bool show);

    /**
     * @brief Provides callbacks for events that occur during video playback.
     *
     * @since 5.0
     * @version 3.0
     */
    class VideoPlayerListener {
    public:
        /**
         * @brief A constructor used to create a <b>VideoPlayerListener</b> instance.
         *
         * @since 5.0
         * @version 3.0
         */
        VideoPlayerListener() {}

        /**
         * @brief A destructor used to delete the <b>VideoPlayerListener</b> instance.
         *
         * @since 5.0
         * @version 3.0
         */
        virtual ~VideoPlayerListener() {}

        /**
         * @brief Called when the playback is complete.
         *
         * @since 5.0
         * @version 3.0
         */
        virtual void OnPlaybackComplete() {}

        /**
         * @brief Called when a video playback error occurs.
         *
         * @param errorType Indicates the error type.
         * @param eerrorCode Indicates the error code.
         * @since 5.0
         * @version 3.0
         */
        virtual void OnError(int32_t errorType, int32_t errorCode) {}

        /**
         * @brief Called when playback information is received.
         *
         * @param type Indicates the information type.
         * @param extra Indicates the information code.
         * @since 5.0
         * @version 3.0
         */
        virtual void OnInfo(int32_t type, int32_t extra) {}

        /**
         * @brief Called when the video image size changes.
         *
         * @param width Indicates the video width.
         * @param height Indicates the video height.
         * @since 5.0
         * @version 3.0
         */
        virtual void OnVideoSizeChanged(int32_t width, int32_t height) {}

        /**
         * @brief Called when this video is rewound.
         *
         * @since 5.0
         * @version 3.0
         */
        virtual void OnRewindToComplete() {}

        /**
         * @brief Called when this video is paused.
         *
         * @since 5.0
         * @version 3.0
         */
        virtual void OnPlaybackPause() {}

        /**
         * @brief Called when this video is played.
         *
         * @since 5.0
         * @version 3.0
         */
        virtual void OnPlaybackPlay() {}

        /**
         * @brief Called when this video is stopped.
         *
         * @since 5.0
         * @version 3.0
         */
        virtual void OnPlaybackStop() {}
    };

    /**
     * @brief Sets a listener for monitoring video playbacks.
     *
     * @param listener Indicates the pointer to the listener to set.
     * @since 5.0
     * @version 3.0
     */
    void SetVideoPlayerListener(VideoPlayerListener* listener);

private:
    class SliderAnimatorCallback : public AnimatorCallback {
    public:
        SliderAnimatorCallback(UIVideo* video, UISlider* slider, UILabel* label)
        {
            video_ = video;
            slider_ = slider;
            timeLabel_ = label;
        }

        virtual ~SliderAnimatorCallback() {}
        void Callback(UIView* view) override;
        void SetPlayButton(UIToggleButton* toggleButton)
        {
            playButton_ = toggleButton;
        }

        void SetSliderAnimator(Animator* animator)
        {
            sliderAnimator_ = animator;
        }

        void SetDuration(int64_t duration)
        {
            duration_ = duration;
        }

        void ResetTickTime()
        {
            if (sliderAnimator_ != nullptr) {
                tickCount_ = sliderAnimator_->GetRunTime();
            }
        }

        friend class UIVideo;
    private:
        uint32_t tickCount_ = 0;
        int64_t duration_ = 0;
        UIVideo* video_ = nullptr;
        UISlider* slider_ = nullptr;
        UILabel* timeLabel_ = nullptr;
        UIToggleButton* playButton_ = nullptr;
        Animator* sliderAnimator_ = nullptr;
    };

    void InitVideo();
    void InitControllerLabel();
    void InitControllerButton();
    void InitControllerSlider();
    void DeleteController();
    void SetSurfaceInfo();
    void OnVideoComplete();
    bool GetTimerFromMSecond(int64_t currentTime, char* timer, int32_t len);

    bool OnClick(UIView& view, const ClickEvent& event) override;
    bool OnPress(UIView& view, const PressEvent& event) override;
    void OnChange(int32_t progress) override;

    class PlayerListener : public Media::PlayerCallback {
    public:
        PlayerListener() {}
        virtual ~PlayerListener() {}
        void OnPlaybackComplete() override
        {
            if (video_ != nullptr) {
                video_->OnVideoComplete();
            }
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackComplete();
            }
        }

        void OnError(int32_t errorType, int32_t errorCode) override
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnError(errorType, errorCode);
            }
        }

        void OnInfo(int32_t type, int32_t extra) override
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnInfo(type, extra);
            }
        }

        void OnVideoSizeChanged(int32_t width, int32_t height) override
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnVideoSizeChanged(width, height);
            }
        }

        void OnRewindToComplete() override
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnRewindToComplete();
            }
        }

        void OnPlaybackPause()
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackPause();
            }
        }

        void OnPlaybackPlay()
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackPlay();
            }
        }

        void OnPlaybackStop()
        {
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackStop();
            }
        }

        void SetVideoPlayer(UIVideo* video)
        {
            video_ = video;
        }

        void SetVideoPlayerListerner(VideoPlayerListener* listener)
        {
            videoPlayerListener_ = listener;
        }

    private:
        UIVideo* video_ = nullptr;
        VideoPlayerListener* videoPlayerListener_ = nullptr;
    };

    bool completeFlag_ = false;
    float leftVolumeValue_ = DEFAULT_VOLUME;
    float rightVolumeValue_ = DEFAULT_VOLUME;
    int64_t duration_ = 0;
    const char* src_ = nullptr;
    std::shared_ptr<Player> videoPlayer_ = nullptr;
    UIViewGroup* controllerGroup_ = nullptr;
    UILabel* totalTimeLabel_ = nullptr;
    UILabel* currentTimeLabel_ = nullptr;
    UILabel* titleLabel_ = nullptr;
    UISlider* playSlider_ = nullptr;
    Animator* sliderAnimator_ = nullptr;
    SliderAnimatorCallback* sliderAnimatorCallback_ = nullptr;
    UIToggleButton* volumeButton_ = nullptr;
    UISurfaceView* surfaceView_ = nullptr;
    UIToggleButton* playButton_ = nullptr;
    UIToggleButton* pauseButton_ = nullptr;
    VideoPlayerListener* videoPlayerListener_ = nullptr;
    std::shared_ptr<PlayerListener> playerListener_ = nullptr;

    static constexpr uint16_t DEFAULT_VIEW_WIDTH = 960;
    static constexpr uint16_t DEFAULT_VIEW_HEIGHT = 480;
    static constexpr uint16_t TOGGLE_BUTTON_WIDTH = 32;
    static constexpr uint16_t TOGGLE_BUTTON_HEIGHT = 32;
    static constexpr uint16_t TIME_LABEL_WIDTH = 300;
    static constexpr uint16_t TIME_LABEL_HEIGHT = 20;
    static constexpr uint16_t KNOB_WIDTH = 18;
    static constexpr uint16_t FONT_DEFAULT_SIZE = 14;
    static constexpr uint16_t MAX_VOLUME = 300;
    static constexpr uint16_t HIDE_MILLI_SECOND = 5000;
    static constexpr float DEFAULT_VOLUME = 0.5;
};
} // namespace OHOS

#endif // VERSION_LITE
#endif // GRAPHIC_LITE_UI_VIDEO_H
