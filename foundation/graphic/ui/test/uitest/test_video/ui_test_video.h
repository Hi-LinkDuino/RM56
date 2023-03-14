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

#ifndef UI_TEST_VIDEO_H
#define UI_TEST_VIDEO_H

#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_video.h"
#include "ui_test.h"

#ifndef VERSION_LITE
namespace OHOS {
class UITestVideo : public UITest, public UIView::OnClickListener {
public:
    UITestVideo() {}
    ~UITestVideo() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView &view, const ClickEvent& event) override;
    bool OnClick1(UIView &view, const ClickEvent& event);
    bool OnClick2(UIView &view, const ClickEvent& event);
    void DealClickLoopButton() const;
    void InitTestButtons();
    void SetupTestButton(UILabelButton* button, const char* text);
    void DeleteTestButtons();
    void UIKit_UIVideo_Test_SetVideo_001();

    class TestVideoListener : public UIVideo::VideoPlayerListener {
    public:
        TestVideoListener() {}
        ~TestVideoListener() {}

        void OnPlaybackComplete() override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnPlaybackComplete");
            }
        }

        void OnError(int32_t errorType, int32_t errorCode) override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnError");
            }
        }

        void OnInfo(int type, int extra) override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnInfo");
            }
        }

        void OnVideoSizeChanged(int width, int height) override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnVideoSizeChanged");
            }
        }

        void OnRewindToComplete() override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnRewindToComplete");
            }
        }

        void OnPlaybackPause() override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnPlaybackPause");
            }
        }

        void OnPlaybackPlay() override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnPlaybackPlay");
            }
        }

        void OnPlaybackStop() override
        {
            if (infoLabel_ != nullptr) {
                infoLabel_->SetText("OnPlaybackStop");
            }
        }

        void SetInfoLabel(UILabel* label)
        {
            infoLabel_ = label;
        }

    private:
        UILabel* infoLabel_ = nullptr;
    };

private:
    UIScrollView* container_ = nullptr;
    UIVideo* video_ = nullptr;
    UILabel* infoLabel_ = nullptr;
    UILabelButton* loopButton_ = nullptr;
    UILabelButton* volButtonAdd_ = nullptr;
    UILabelButton* volButtonSub_ = nullptr;
    UILabelButton* rewindButtonAdd_ = nullptr;
    UILabelButton* rewindButtonSub_ = nullptr;
    UILabelButton* pauseButton_ = nullptr;
    UILabelButton* playButton_ = nullptr;
    UILabelButton* stopButton_ = nullptr;
    UILabelButton* resetButton_ = nullptr;
    UILabelButton* totleTimeButton_ = nullptr;
    TestVideoListener* testVideoListener_ = nullptr;
};
} // namespace OHOS
#endif // VERSION_LITE
#endif // UI_TEST_VIDEO_H
