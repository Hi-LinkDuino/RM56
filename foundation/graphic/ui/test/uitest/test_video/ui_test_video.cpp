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

#include "ui_test_video.h"
#include "securec.h"
#include "test_resource_config.h"
#ifndef VERSION_LITE
namespace OHOS {
void UITestVideo::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - 30); // 30:offset
    }
}

void UITestVideo::TearDown()
{
    if (container_ != nullptr) {
        container_->RemoveAll();
        delete container_;
        container_ = nullptr;
    }
    if (video_ != nullptr) {
        video_->Stop();
        video_->Reset();
        delete video_;
        video_ = nullptr;
    }
    if (infoLabel_ != nullptr) {
        delete infoLabel_;
        infoLabel_ = nullptr;
    }
    if (testVideoListener_ != nullptr) {
        delete testVideoListener_;
        testVideoListener_ = nullptr;
    }
    DeleteTestButtons();
}

void UITestVideo::DeleteTestButtons()
{
    if (loopButton_ != nullptr) {
        delete loopButton_;
        loopButton_ = nullptr;
    }
    if (totleTimeButton_ != nullptr) {
        delete totleTimeButton_;
        totleTimeButton_ = nullptr;
    }
    if (volButtonAdd_ != nullptr) {
        delete volButtonAdd_;
        volButtonAdd_ = nullptr;
    }
    if (volButtonSub_ != nullptr) {
        delete volButtonSub_;
        volButtonSub_ = nullptr;
    }
    if (rewindButtonAdd_ != nullptr) {
        delete rewindButtonAdd_;
        rewindButtonAdd_ = nullptr;
    }
    if (rewindButtonSub_ != nullptr) {
        delete rewindButtonSub_;
        rewindButtonSub_ = nullptr;
    }
    if (playButton_ != nullptr) {
        delete playButton_;
        playButton_ = nullptr;
    }
    if (pauseButton_ != nullptr) {
        delete pauseButton_;
        pauseButton_ = nullptr;
    }
    if (stopButton_ != nullptr) {
        delete stopButton_;
        stopButton_ = nullptr;
    }
    if (resetButton_ != nullptr) {
        delete resetButton_;
        resetButton_ = nullptr;
    }
}

void UITestVideo::SetupTestButton(UILabelButton* button, const char* text)
{
    if (button != nullptr) {
        int16_t buttonWidth = 60;  // 60:button width
        int16_t buttonHeight = 40; // 40:button height
        button->SetText(text);
        button->Resize(buttonWidth, buttonHeight);
        button->SetOnClickListener(this);
    }
}

void UITestVideo::InitTestButtons()
{
    int16_t posX1 = Screen::GetInstance().GetWidth() - 160; // 160:posX1 offset
    int16_t posX2 = Screen::GetInstance().GetWidth() - 80;  // 80:posX2 offset

    loopButton_ = new UILabelButton();
    loopButton_->SetPosition(posX1, 50); // 50:position y
    SetupTestButton(loopButton_, "loop");

    totleTimeButton_ = new UILabelButton();
    totleTimeButton_->SetPosition(posX2, 50); // 50:position y
    SetupTestButton(totleTimeButton_, "time");

    volButtonAdd_ = new UILabelButton();
    volButtonAdd_->SetPosition(posX1, 100); // 100:position y
    SetupTestButton(volButtonAdd_, "vol+");

    volButtonSub_ = new UILabelButton();
    volButtonSub_->SetPosition(posX2, 100); // 100:position y
    SetupTestButton(volButtonSub_, "vol-");

    rewindButtonAdd_ = new UILabelButton();
    rewindButtonAdd_->SetPosition(posX1, 150); // 150:position y
    SetupTestButton(rewindButtonAdd_, "pos+");

    rewindButtonSub_ = new UILabelButton();
    rewindButtonSub_->SetPosition(posX2, 150); // 150:position y
    SetupTestButton(rewindButtonSub_, "pos-");

    playButton_ = new UILabelButton();
    playButton_->SetPosition(posX1, 200); // 200:position y
    SetupTestButton(playButton_, "play");

    pauseButton_ = new UILabelButton();
    pauseButton_->SetPosition(posX2, 200); // 200:position y
    SetupTestButton(pauseButton_, "pause");

    stopButton_ = new UILabelButton();
    stopButton_->SetPosition(posX1, 250); // 250:position y
    SetupTestButton(stopButton_, "stop");

    resetButton_ = new UILabelButton();
    resetButton_->SetPosition(posX2, 250); // 250:position y
    SetupTestButton(resetButton_, "reset");
}

const UIView* UITestVideo::GetTestView()
{
    if (video_ == nullptr) {
        UIKit_UIVideo_Test_SetVideo_001();
    }
    return container_;
}

void UITestVideo::UIKit_UIVideo_Test_SetVideo_001()
{
    if (container_ != nullptr) {
        if (video_ == nullptr) {
            video_ = new UIVideo();
        }

        infoLabel_ = new UILabel;
        infoLabel_->SetPosition(20, 60); // 20:position x 60:position y
        infoLabel_->Resize(200, 40);     // 200:label width 40:label height

        if (!video_->SetSrc(VIDEO_SOURCE_DIRECTORY)) {
            infoLabel_->SetText("SetSrc error");
            return;
        }
        video_->SetPosition(0, 0);
        video_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
        if (!video_->Prepare()) {
            infoLabel_->SetText("Prepare error");
            return;
        }

        testVideoListener_ = new TestVideoListener();
        testVideoListener_->SetInfoLabel(infoLabel_);
        video_->SetVideoPlayerListener(testVideoListener_);

        InitTestButtons();
        container_->Add(video_);
        container_->Add(infoLabel_);
        container_->Add(loopButton_);
        container_->Add(totleTimeButton_);
        container_->Add(volButtonAdd_);
        container_->Add(volButtonSub_);
        container_->Add(rewindButtonAdd_);
        container_->Add(rewindButtonSub_);
        container_->Add(playButton_);
        container_->Add(pauseButton_);
        container_->Add(stopButton_);
        container_->Add(resetButton_);

        if (!video_->Play()) {
            infoLabel_->SetText("Play error");
            return;
        }
        video_->ShowController(true);
    }
}

void UITestVideo::DealClickLoopButton() const
{
    if ((video_ == nullptr) || (infoLabel_ == nullptr)) {
        return;
    }
    if (video_->IsSingleLooping()) {
        if (video_->EnableSingleLooping(false)) {
            infoLabel_->SetText("single");
        } else {
            infoLabel_->SetText("single error");
        }
    } else {
        if (video_->EnableSingleLooping(true)) {
            infoLabel_->SetText("loop");
        } else {
            infoLabel_->SetText("loop error");
        }
    }
}

bool UITestVideo::OnClick(UIView &view, const ClickEvent& event)
{
    if ((video_ == nullptr) || (infoLabel_ == nullptr)) {
        return true;
    }
    if (&view == loopButton_) {
        DealClickLoopButton();
    } else if (&view == totleTimeButton_) {
        int64_t duration = 0;
        if (video_->GetDuration(duration)) {
            char buf[10] = {0}; // 10:buf size
            if (sprintf_s(buf, sizeof(buf), "%lld", duration) < 0) {
                return true;
            }
            infoLabel_->SetText(&buf[0]);
        } else {
            infoLabel_->SetText("GetDuration error");
        }
    } else if (&view == volButtonAdd_) {
        if (video_->SetVolume(0.8, 0.8)) { // 0.8:left volume 0.8:right volume
            infoLabel_->SetText("volume 0.8");
        } else {
            infoLabel_->SetText("SetVolume error");
        }
    } else if (&view == volButtonSub_) {
        if (video_->SetVolume(0, 0)) {
            infoLabel_->SetText("volume 0");
        } else {
            infoLabel_->SetText("SetVolume error");
        }
    } else {
        return OnClick1(view, event);
    }
    return true;
}

bool UITestVideo::OnClick1(UIView &view, const ClickEvent& event)
{
    if ((video_ == nullptr) || (infoLabel_ == nullptr)) {
        return true;
    }
    if (&view == rewindButtonAdd_) {
        int64_t time = 0;
        if (video_->GetCurrentTime(time)) {
            if (video_->Rewind(time + 3000)) { // 3000:rewind add ms
                infoLabel_->SetText("Rewind + 3000");
            } else {
                infoLabel_->SetText("Rewind error");
            }
        } else {
            infoLabel_->SetText("GetCurrentTime error");
        }
    } else if (&view == rewindButtonSub_) {
        int64_t time = 0;
        if (video_->GetCurrentTime(time)) {
            if (video_->Rewind(time - 3000)) { // 3000:rewind sub ms
                infoLabel_->SetText("Rewind - 3000");
            } else {
                infoLabel_->SetText("Rewind error");
            }
        } else {
            infoLabel_->SetText("GetCurrentTime error");
        }
    } else {
        return OnClick2(view, event);
    }
    return true;
}

bool UITestVideo::OnClick2(UIView &view, const ClickEvent& event)
{
    if ((video_ == nullptr) || (infoLabel_ == nullptr)) {
        return true;
    }
    if (&view == playButton_) {
        if (video_->Play()) {
            infoLabel_->SetText("Play");
        } else {
            infoLabel_->SetText("Play error");
        }
    } else if (&view == pauseButton_) {
        if (video_->Pause()) {
            infoLabel_->SetText("Pause");
        } else {
            infoLabel_->SetText("Pause error");
        }
    } else if (&view == stopButton_) {
        if (video_->Stop()) {
            infoLabel_->SetText("Stop");
        } else {
            infoLabel_->SetText("Stop error");
        }
    } else if (&view == resetButton_) {
        if (video_->Reset()) {
            infoLabel_->SetText("Reset");
        } else {
            infoLabel_->SetText("Reset error");
        }
    }
    return true;
}
} // namespace OHOS

#endif // VERSION_LITE
