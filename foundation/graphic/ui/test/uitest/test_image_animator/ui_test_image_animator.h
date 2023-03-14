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

#ifndef UI_TEST_IMAGE_ANIMATOR_H
#define UI_TEST_IMAGE_ANIMATOR_H

#include "components/ui_image_animator.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "layout/grid_layout.h"
#include "ui_test.h"

namespace OHOS {
class TestAnimatorStopListener : public UIImageAnimatorView::AnimatorStopListener {
public:
    explicit TestAnimatorStopListener(UIViewGroup* viewGroup) : viewGroup_(viewGroup)
    {
        viewGroup_ = viewGroup;
    }
    ~TestAnimatorStopListener() {}

    void Init()
    {
        label_ = new UILabel();
        viewGroup_->Add(label_);
        label_->SetPosition(24, 50, 200, 29); // 24: position x 50: position y 200: width 29: height
        label_->SetText("OnAnimatorStop");
        label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        label_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
        label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        label_->SetStyle(STYLE_BACKGROUND_OPA, 255); // 255: value
        label_->SetVisible(false);
    }

    void OnAnimatorStop(UIView& view) override
    {
        label_->SetVisible(true);
        label_->Invalidate();
    }

private:
    UILabel* label_ = nullptr;
    UIViewGroup* viewGroup_ = nullptr;
};

class UITestImageAnimator : public UITest, public UIView::OnClickListener {
public:
    UITestImageAnimator() {}
    ~UITestImageAnimator() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void SetUpButton(UILabelButton* btn, const char* title);

    bool OnClick(UIView& view, const ClickEvent& event) override;

    void UIKit_ImageAnimator_Test_Start_001();
    void UIKit_ImageAnimator_Test_Stop_002();
    void UIKit_ImageAnimator_Test_Pause_003();
    void UIKit_ImageAnimator_Test_Resume_004();
    void UIKit_ImageAnimator_Test_SetImageAnimatorSrc_005();
    void UIKit_ImageAnimator_Test_SetTickOfUpdate_006();
    void UIKit_ImageAnimator_Test_SetSizeFixed_007();
    void UIKit_ImageAnimator_Test_SetRepeat_008();
    void UIKit_ImageAnimator_Test_SetReverse_009();
    void UIKit_ImageAnimator_Test_SetAnimatorStopListener_010();
    void UIKit_ImageAnimator_Test_SetRepeatTimes_011();
    void UIKit_ImageAnimator_Test_SetFillMode_True_Forward_012();
    void UIKit_ImageAnimator_Test_SetFillMode_True_Backward_013();
    void UIKit_ImageAnimator_Test_SetFillMode_False_Forward_014();
    void UIKit_ImageAnimator_Test_SetFillMode_False_Backward_015();
    void UIKit_ImageAnimator_Test_SetImageInfo_016();

private:
    void InitImageInfo();
    UIScrollView* container_ = nullptr;
    GridLayout* layout_ = nullptr;
    UIImageAnimatorView* imageAnimator_ = nullptr;
    TestAnimatorStopListener* listener_ = nullptr;

    UILabelButton* startBtn_ = nullptr;
    UILabelButton* stopBtn_ = nullptr;
    UILabelButton* pauseBtn_ = nullptr;
    UILabelButton* resumeBtn_ = nullptr;
    UILabelButton* setImageBtn_ = nullptr;
    UILabelButton* setSpeedBtn_ = nullptr;
    UILabelButton* fixedBtn_ = nullptr;
    UILabelButton* repeatBtn_ = nullptr;
    UILabelButton* noRepeatBtn_ = nullptr;
    UILabelButton* reverseOrderBtn_ = nullptr;
    UILabelButton* positiveOrderBtn_ = nullptr;
    UILabelButton* listenerBtn_ = nullptr;
    UILabelButton* repeatTimesBtn_ = nullptr;
    UILabelButton* fillModeTrueForwardBtn_ = nullptr;
    UILabelButton* fillModeTrueBackwardBtn_ = nullptr;
    UILabelButton* fillModeFalseForwardBtn_ = nullptr;
    UILabelButton* fillModeFalseBackwardBtn_ = nullptr;
    UILabelButton* setImageInfoBtn_ = nullptr;

    ImageInfo* imageInfo1_ = nullptr;
    ImageInfo* imageInfo2_ = nullptr;
    ImageInfo* imageInfo3_ = nullptr;
    ImageAnimatorInfo imageAnimatorImageInfo_[3] = { 0 };
};
} // namespace OHOS
#endif // UI_TEST_IMAGE_ANIMATOR_H
