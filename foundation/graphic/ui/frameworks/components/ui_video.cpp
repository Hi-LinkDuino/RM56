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

#include "components/ui_video.h"
#include "securec.h"

#ifndef VERSION_LITE
namespace OHOS {
UIVideo::UIVideo()
{
    SetTouchable(true);
    SetOnTouchListener(this);
}

UIVideo::~UIVideo()
{
    if (videoPlayer_ != nullptr) {
        videoPlayer_->Stop();
        videoPlayer_->Reset();
        videoPlayer_->Release();
    }
    if (sliderAnimator_ != nullptr) {
        delete sliderAnimator_;
        sliderAnimator_ = nullptr;
    }
    if (surfaceView_ != nullptr) {
        Remove(surfaceView_);
        delete surfaceView_;
        surfaceView_ = nullptr;
    }
    DeleteController();
}

bool UIVideo::SetSrc(const char* source)
{
    if (source == nullptr) {
        return false;
    }

    InitVideo();
    src_ = source;
    std::string uri(source);
    std::map<std::string, std::string> header;
    Source videoSource(uri, header);
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->SetSource(videoSource);
        if (ret == 0) {
            return true;
        }
    }
    return false;
}

bool UIVideo::Prepare()
{
    if (src_ == nullptr) {
        return false;
    }
    if (videoPlayer_ == nullptr) {
        return false;
    }

    int32_t ret = videoPlayer_->Prepare();
    if (ret != 0) {
        return false;
    }
    SetVolume(DEFAULT_VOLUME, DEFAULT_VOLUME);
    videoPlayer_->GetDuration(duration_);
    if (totalTimeLabel_ != nullptr) {
        char timer[10] = {0}; // 10:timer length
        if (!GetTimerFromMSecond(duration_, timer, sizeof(timer))) {
            return false;
        }
        totalTimeLabel_->SetText(&timer[0]);
    }

    if (titleLabel_ != nullptr) {
        const char* fileName = nullptr;
        fileName = strrchr(src_, '/');
        if (fileName != nullptr) {
            titleLabel_->SetText(fileName + 1);
        }
    }
    if (sliderAnimatorCallback_) {
        sliderAnimatorCallback_->SetDuration(duration_);
    }
    SetSurfaceInfo();

    playerListener_ = std::make_shared<PlayerListener>();
    playerListener_->SetVideoPlayer(this);
    videoPlayer_->SetPlayerCallback(playerListener_);
    return true;
}

bool UIVideo::Play()
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->Play();
        if (ret == 0) {
            if (sliderAnimator_ != nullptr) {
                sliderAnimator_->Start();
            }
            if (pauseButton_ != nullptr) {
                pauseButton_->SetVisible(false);
            }
            if (playButton_ != nullptr) {
                playButton_->SetState(true);
            }
            if (surfaceView_ != nullptr) {
                surfaceView_->SetVisible(true);
            }
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackPlay();
            }
            return true;
        }
    }
    return false;
}

bool UIVideo::IsPlaying()
{
    if (videoPlayer_ != nullptr) {
        return videoPlayer_->IsPlaying();
    }
    return false;
}

bool UIVideo::Pause()
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->Pause();
        if (ret == 0) {
            if (sliderAnimator_ != nullptr) {
                sliderAnimator_->Pause();
            }
            if (playButton_ != nullptr) {
                playButton_->SetState(false);
            }
            if (pauseButton_ != nullptr) {
                pauseButton_->SetVisible(true);
            }
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackPause();
            }
            return true;
        }
    }
    return false;
}

bool UIVideo::Stop()
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->Stop();
        if (ret == 0) {
            if (sliderAnimator_ != nullptr) {
                sliderAnimator_->Stop();
            }
            if (playSlider_ != nullptr) {
                playSlider_->SetValue(0);
            }
            if (currentTimeLabel_ != nullptr) {
                currentTimeLabel_->SetText("00:00:00");
            }
            if (totalTimeLabel_ != nullptr) {
                totalTimeLabel_->SetText("00:00:00");
            }
            if (playButton_ != nullptr) {
                playButton_->SetState(false);
            }
            if (pauseButton_ != nullptr) {
                pauseButton_->SetVisible(true);
            }
            if (surfaceView_ != nullptr) {
                surfaceView_->SetVisible(false);
            }
            if (videoPlayerListener_ != nullptr) {
                videoPlayerListener_->OnPlaybackStop();
            }
            return true;
        }
    }
    return false;
}

bool UIVideo::Rewind(int64_t mSeconds)
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->Rewind(mSeconds, PLAYER_SEEK_PREVIOUS_SYNC);
        if (ret == 0) {
            return true;
        }
    }
    return false;
}

bool UIVideo::SetVolume(float leftVolume, float rightVolume)
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->SetVolume(leftVolume * MAX_VOLUME, rightVolume * MAX_VOLUME);
        if (ret == 0) {
            leftVolumeValue_ = leftVolume;
            rightVolumeValue_ = rightVolume;
            if (volumeButton_ != nullptr) {
                if ((leftVolumeValue_ == 0) && (rightVolumeValue_ == 0)) {
                    volumeButton_->SetState(true);
                } else {
                    volumeButton_->SetState(false);
                }
            }
            return true;
        }
    }
    return false;
}

bool UIVideo::IsSingleLooping()
{
    if (videoPlayer_ != nullptr) {
        return videoPlayer_->IsSingleLooping();
    }
    return false;
}

bool UIVideo::GetCurrentTime(int64_t& time)
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->GetCurrentTime(time);
        if (ret == 0) {
            return true;
        }
    }
    return false;
}

bool UIVideo::GetDuration(int64_t& durationMs)
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->GetDuration(durationMs);
        if (ret == 0) {
            return true;
        }
    }
    return false;
}

void UIVideo::SetVideoPlayerListener(VideoPlayerListener* listener)
{
    videoPlayerListener_ = listener;

    if (playerListener_ != nullptr) {
        playerListener_->SetVideoPlayerListerner(videoPlayerListener_);
    }
}

bool UIVideo::Reset()
{
    if (videoPlayer_ != nullptr) {
        if (sliderAnimator_ != nullptr) {
            sliderAnimator_->Stop();
        }
        if (playSlider_ != nullptr) {
            playSlider_->SetValue(0);
        }
        if (currentTimeLabel_ != nullptr) {
            currentTimeLabel_->SetText("00:00");
        }
        if (totalTimeLabel_ != nullptr) {
            totalTimeLabel_->SetText("00:00");
        }
        if (pauseButton_ != nullptr) {
            pauseButton_->SetVisible(true);
        }
        if (playButton_ != nullptr) {
            playButton_->SetState(false);
        }
        int32_t ret = videoPlayer_->Reset();
        if (ret == 0) {
            videoPlayer_->Release();
            return true;
        }
    }
    return false;
}

bool UIVideo::EnableSingleLooping(bool loop)
{
    if (videoPlayer_ != nullptr) {
        int32_t ret = videoPlayer_->EnableSingleLooping(loop);
        if (ret == 0) {
            return true;
        }
    }
    return false;
}

void UIVideo::ShowController(bool show)
{
    if (controllerGroup_ != nullptr) {
        if (controllerGroup_->IsVisible() != show) {
            controllerGroup_->SetVisible(show);
            Invalidate();
        }
    }
}

void UIVideo::InitVideo()
{
    if (videoPlayer_ == nullptr) {
        videoPlayer_ = std::make_shared<Player>();
    }

    InitControllerLabel();
    InitControllerButton();
    InitControllerSlider();

    if (surfaceView_ == nullptr) {
        surfaceView_ = new UISurfaceView();
        if (surfaceView_ == nullptr) {
            GRAPHIC_LOGE("new UISurfaceView fail");
            return;
        }
        surfaceView_->SetPosition(0, 0);
        surfaceView_->SetWidth(DEFAULT_VIEW_WIDTH);
        surfaceView_->SetHeight(DEFAULT_VIEW_HEIGHT);
        Add(surfaceView_);
    }
    if (controllerGroup_ == nullptr) {
        controllerGroup_ = new UIViewGroup();
        if (controllerGroup_ == nullptr) {
            GRAPHIC_LOGE("new UIViewGroup fail");
            return;
        }
        controllerGroup_->SetPosition(0, 0, DEFAULT_VIEW_WIDTH, DEFAULT_VIEW_HEIGHT);
        controllerGroup_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        controllerGroup_->Add(titleLabel_);
        controllerGroup_->Add(totalTimeLabel_);
        controllerGroup_->Add(currentTimeLabel_);
        controllerGroup_->Add(playSlider_);
        controllerGroup_->Add(volumeButton_);
        controllerGroup_->Add(playButton_);
        controllerGroup_->Add(pauseButton_);
        Add(controllerGroup_);
    }
    if (sliderAnimatorCallback_ == nullptr) {
        sliderAnimatorCallback_ = new SliderAnimatorCallback(this, playSlider_, currentTimeLabel_);
        if (sliderAnimatorCallback_ == nullptr) {
            GRAPHIC_LOGE("new SliderAnimatorCallback fail");
            return;
        }
        sliderAnimatorCallback_->SetPlayButton(playButton_);
    }
    if (sliderAnimator_ == nullptr) {
        sliderAnimator_ = new Animator(sliderAnimatorCallback_, this, 0, true);
        if (sliderAnimator_ == nullptr) {
            GRAPHIC_LOGE("new Animator fail");
            return;
        }
        sliderAnimatorCallback_->SetSliderAnimator(sliderAnimator_);
    }
}

void UIVideo::InitControllerLabel()
{
    if (titleLabel_ == nullptr) {
        titleLabel_ = new UILabel();
        if (titleLabel_ == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        titleLabel_->SetPosition(322, 28, 316, 29); // 322:pos x, 28:pos y, 316:width, 29:height
        titleLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER,
                              UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
        titleLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 20); // 20:font size
        titleLabel_->SetText("title title");
        titleLabel_->SetTextColor(Color::White());
    }

    if (totalTimeLabel_ == nullptr) {
        totalTimeLabel_ = new UILabel();
        if (totalTimeLabel_ == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        // 580:pos x, 56:pos y offset
        totalTimeLabel_->SetPosition(580, DEFAULT_VIEW_HEIGHT - 56, TIME_LABEL_WIDTH, TIME_LABEL_HEIGHT);
        totalTimeLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT,
                                  UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
        totalTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        totalTimeLabel_->SetTextColor(Color::White());
        totalTimeLabel_->SetText("00:00:00");
    }

    if (currentTimeLabel_ == nullptr) {
        currentTimeLabel_ = new UILabel();
        if (currentTimeLabel_ == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        // 80:pos x, 56:pos y offset
        currentTimeLabel_->SetPosition(80, DEFAULT_VIEW_HEIGHT - 56, TIME_LABEL_WIDTH, TIME_LABEL_HEIGHT);
        currentTimeLabel_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT,
                                    UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
        currentTimeLabel_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        currentTimeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        currentTimeLabel_->SetText("00:00:00");
        currentTimeLabel_->SetTextColor(Color::White());
    }
}

void UIVideo::InitControllerButton()
{
    if (playButton_ == nullptr) {
        playButton_ = new UIToggleButton();
        if (playButton_ == nullptr) {
            GRAPHIC_LOGE("new UIToggleButton fail");
            return;
        }
        // 24:pox x, 88:pos y offset
        playButton_->SetPosition(24, DEFAULT_VIEW_HEIGHT - 88, TOGGLE_BUTTON_WIDTH, TOGGLE_BUTTON_HEIGHT);
        playButton_->SetImages(MEDIA_IMAGE_PAUSE, MEDIA_IMAGE_PLAY);
        playButton_->SetState(true);
        playButton_->SetOnClickListener(this);
    }

    if (pauseButton_ == nullptr) {
        pauseButton_ = new UIToggleButton();
        if (pauseButton_ == nullptr) {
            GRAPHIC_LOGE("new UIToggleButton fail");
            return;
        }
        // 448:pox x, 208:pos y 2:double width 2:double height
        pauseButton_->SetPosition(448, 208, 2 * TOGGLE_BUTTON_WIDTH, 2 * TOGGLE_BUTTON_HEIGHT);
        pauseButton_->SetImages(MEDIA_IMAGE_PLAY_CENTER, MEDIA_IMAGE_PLAY_CENTER);
        pauseButton_->SetVisible(false);
        pauseButton_->SetOnClickListener(this);
    }

    if (volumeButton_ == nullptr) {
        volumeButton_ = new UIToggleButton();
        if (volumeButton_ == nullptr) {
            GRAPHIC_LOGE("new UIToggleButton fail");
            return;
        }
        // 56:pox x offset, 88:pos y offset
        volumeButton_->SetPosition(DEFAULT_VIEW_WIDTH - 56, DEFAULT_VIEW_HEIGHT - 88, TOGGLE_BUTTON_WIDTH,
                                   TOGGLE_BUTTON_HEIGHT);
        volumeButton_->SetImages(MEDIA_IMAGE_MUTE, MEDIA_IMAGE_VOLUME);
        volumeButton_->SetOnClickListener(this);
    }
}

void UIVideo::OnVideoComplete()
{
    if (videoPlayer_ != nullptr) {
        videoPlayer_->Stop();
        completeFlag_ = true;

        if (sliderAnimator_ != nullptr) {
            sliderAnimator_->Stop();
        }
        if (playSlider_ != nullptr) {
            playSlider_->SetValue(playSlider_->GetRangeMax());
        }
        if (playButton_ != nullptr) {
            playButton_->SetState(false);
        }
        if (pauseButton_ != nullptr) {
            pauseButton_->SetVisible(true);
        }
        if (controllerGroup_ != nullptr) {
            controllerGroup_->SetVisible(true);
        }
        Invalidate();
    }
}

void UIVideo::InitControllerSlider()
{
    if (playSlider_ == nullptr) {
        playSlider_ = new UISlider();
        if (playSlider_ == nullptr) {
            GRAPHIC_LOGE("new UISlider fail");
            return;
        }
        // 80:pos x, 74:width offset, 5:multiply, 6:divide, 4:height
        playSlider_->SetPosition(80, DEFAULT_VIEW_HEIGHT - 74, DEFAULT_VIEW_WIDTH * 5 / 6, 4);
        playSlider_->SetValidHeight(4);                         // 4:height
        playSlider_->SetValidWidth(DEFAULT_VIEW_WIDTH * 5 / 6); // 5:multiply, 6:divide
        playSlider_->SetRange(100, 0);                          // 100:range max
        playSlider_->SetValue(0);
        playSlider_->SetKnobWidth(KNOB_WIDTH);
        playSlider_->SetSliderRadius(5, 5); // 5:background radius, 5:foreground radius
        playSlider_->SetKnobRadius(KNOB_WIDTH / 2); // 2:half
        playSlider_->SetKnobStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        playSlider_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, 0x1A888888); // 0x1A888888:slider background color
        playSlider_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, 90);           // 90:background opa
        playSlider_->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
        playSlider_->SetSliderEventListener(this);
    }
}

void UIVideo::DeleteController()
{
    if (controllerGroup_ != nullptr) {
        controllerGroup_->RemoveAll();
        Remove(controllerGroup_);
        delete controllerGroup_;
        controllerGroup_ = nullptr;
    }
    if (playButton_ != nullptr) {
        delete playButton_;
        playButton_ = nullptr;
    }
    if (pauseButton_ != nullptr) {
        delete pauseButton_;
        pauseButton_ = nullptr;
    }
    if (volumeButton_ != nullptr) {
        delete volumeButton_;
        volumeButton_ = nullptr;
    }
    if (playSlider_ != nullptr) {
        delete playSlider_;
        playSlider_ = nullptr;
    }
    if (sliderAnimatorCallback_ != nullptr) {
        delete sliderAnimatorCallback_;
        sliderAnimatorCallback_ = nullptr;
    }
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (totalTimeLabel_ != nullptr) {
        delete totalTimeLabel_;
        totalTimeLabel_ = nullptr;
    }
    if (currentTimeLabel_ != nullptr) {
        delete currentTimeLabel_;
        currentTimeLabel_ = nullptr;
    }
}

bool UIVideo::OnClick(UIView& view, const ClickEvent& event)
{
    if (videoPlayer_ == nullptr) {
        return true;
    }
    if (&view == playButton_) {
        if (completeFlag_) {
            completeFlag_ = false;
            if (pauseButton_ != nullptr) {
                pauseButton_->SetVisible(true);
            }
            if (controllerGroup_ != nullptr) {
                controllerGroup_->SetVisible(true);
            }
            return true;
        }

        if ((sliderAnimatorCallback_ != nullptr) && (pauseButton_ != nullptr) && (playButton_ != nullptr)) {
            sliderAnimatorCallback_->ResetTickTime();
            if (playButton_->GetState()) {
                Play();
                pauseButton_->SetVisible(false);
            } else {
                Pause();
                pauseButton_->SetVisible(true);
            }
            pauseButton_->Invalidate();
        }
    } else if (&view == pauseButton_) {
        if ((pauseButton_ != nullptr) && pauseButton_->IsVisible()) {
            pauseButton_->SetVisible(false);
            Play();
        }
    } else if (&view == volumeButton_) {
        if ((volumeButton_ != nullptr) && volumeButton_->GetState()) {
            videoPlayer_->SetVolume(0, 0);
        } else {
            SetVolume(leftVolumeValue_, rightVolumeValue_);
        }
    }
    return true;
}

bool UIVideo::OnPress(UIView& view, const PressEvent& event)
{
    if (sliderAnimatorCallback_ != nullptr) {
        sliderAnimatorCallback_->ResetTickTime();
        if (controllerGroup_ != nullptr) {
            controllerGroup_->SetVisible(true);
        }
        Invalidate();
    }
    return true;
}

void UIVideo::OnChange(int32_t progress)
{
    if (videoPlayer_ != nullptr) {
        if (sliderAnimatorCallback_ != nullptr) {
            sliderAnimatorCallback_->ResetTickTime();
        }
        int64_t currentValue = progress * duration_ / 100; // 100:percent
        if (currentTimeLabel_ != nullptr) {
            char timer[10] = {0}; // 10:timer length
            if (!GetTimerFromMSecond(currentValue, timer, sizeof(timer))) {
                return;
            }
            currentTimeLabel_->SetText(&timer[0]);
        }
        videoPlayer_->Rewind(currentValue, PLAYER_SEEK_PREVIOUS_SYNC);
    }
}

void UIVideo::SetSurfaceInfo()
{
    if (videoPlayer_ == nullptr) {
        return;
    }
    int32_t width = 0;
    int32_t height = 0;
    videoPlayer_->GetVideoWidth(width);
    videoPlayer_->GetVideoHeight(height);

    if ((width <= 0) || (height <= 0)) {
        videoPlayer_->Reset();
        return;
    }

    int16_t viewWidth = GetWidth();
    int16_t viewHeight = GetHeight();
    if ((viewWidth <= 0) || (viewHeight <= 0)) {
        videoPlayer_->Reset();
        return;
    }
    float ratioX = static_cast<float>(width) / viewWidth;
    float ratioY = static_cast<float>(height) / viewHeight;
    uint16_t surfaceViewWidth;
    uint16_t surfaceViewHeight;
    uint16_t surfaceViewPositionX = 0;
    uint16_t surfaceViewPositionY = 0;
    if (ratioX > ratioY) {
        surfaceViewWidth = viewWidth;
        surfaceViewHeight = height / ratioX;
        surfaceViewPositionY = (viewHeight - surfaceViewHeight) / 2; // 2:half
    } else {
        surfaceViewWidth = width / ratioY;
        surfaceViewHeight = viewHeight;
        surfaceViewPositionX = (viewWidth - surfaceViewWidth) / 2; // 2:half
    }
    if (surfaceView_ != nullptr) {
        surfaceView_->SetPosition(surfaceViewPositionX, surfaceViewPositionY);
        surfaceView_->SetWidth(surfaceViewWidth - 1);
        surfaceView_->SetHeight(surfaceViewHeight);
        videoPlayer_->SetVideoSurface(surfaceView_->GetSurface());
    }
}

bool UIVideo::GetTimerFromMSecond(int64_t currentTime, char* timer, int32_t len)
{
    int64_t currentSecond = currentTime / 1000; // 1000:millisecond
    int32_t second = currentSecond % 60;        // 60:second
    int32_t minute = (currentSecond / 60) % 60; // 60:minute 60:second
    int32_t hour = (currentSecond / 60) / 60;   // 60:minute 60:second
    int32_t ret = 0;
    ret = sprintf_s(timer, len, "%02d:%02d:%02d", hour, minute, second);
    if (ret < 0) {
        return false;
    }
    return true;
}

void UIVideo::SliderAnimatorCallback::Callback(UIView* view)
{
    if (video_ != nullptr) {
        int64_t currentTime = 0;
        video_->GetCurrentTime(currentTime);
        if (timeLabel_ != nullptr) {
            char timer[10] = {0}; // 10:timer length
            if (!video_->GetTimerFromMSecond(currentTime, timer, sizeof(timer))) {
                return;
            }
            timeLabel_->SetText(&timer[0]);
            timeLabel_->Invalidate();
        }
        if (slider_ != nullptr) {
            int64_t curPosition = currentTime * slider_->GetRangeMax() / duration_;
            slider_->SetValue(curPosition);
            slider_->Invalidate();
        }

        uint32_t runTime = 0;
        if (sliderAnimator_ != nullptr) {
            runTime = sliderAnimator_->GetRunTime();
        }
        if ((runTime > tickCount_) && (runTime - tickCount_ >= HIDE_MILLI_SECOND)) {
            video_->ShowController(false);
            tickCount_ = runTime;
        }
    }
}
} // namespace OHOS

#endif // VERSION_LITE
