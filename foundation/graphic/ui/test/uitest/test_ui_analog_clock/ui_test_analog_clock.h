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

#ifndef UI_TEST_ANALOG_CLOCK_H
#define UI_TEST_ANALOG_CLOCK_H

#include "ui_test.h"

#include "components/ui_analog_clock.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"

namespace OHOS {
class ClockAnimatorCallback : public AnimatorCallback {
public:
    explicit ClockAnimatorCallback(UIAnalogClock* clock) : clock_(clock) {}
    ~ClockAnimatorCallback() {}
    virtual void Callback(UIView* view)
    {
        count_++;
        if ((count_ == 30) && (clock_ != nullptr)) { // 30: run every tick (~30ms)
            clock_->IncOneSecond();
            count_ = 0;
        }
    }
private:
    UIAnalogClock* clock_;
    int16_t count_ = 0;
};

class UITestAnalogClock : public UITest {
public:
    UITestAnalogClock() {}
    ~UITestAnalogClock() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void CreateButtons001(UIViewGroup* group, UIImageView* curFace, UIAnalogClock* clock);
    void CreateButtons002(UIViewGroup* group, UIImageView* curFace, UIAnalogClock* clock);

    void UIKit_TestLineHandAnalogClock_001();
    void UIKit_TestImageHandAnalogClock_002();

private:
    UIScrollView* container_ = nullptr;
    ClockAnimatorCallback* callback_ = nullptr;
    Animator* animator_ = nullptr;
    ClockAnimatorCallback* callback2_ = nullptr;
    Animator* animator2_ = nullptr;
    UIView::OnClickListener* changeModeListener_ = nullptr;
    UIView::OnClickListener* clickMoveLeftListener_ = nullptr;
    UIView::OnClickListener* clickMoveRightListener_ = nullptr;
    UIView::OnClickListener* clickMoveTopListener_ = nullptr;
    UIView::OnClickListener* clickMoveBottomListener_ = nullptr;
    UIView::OnClickListener* changeModeListener1_ = nullptr;
    UIView::OnClickListener* clickMoveLeftListener1_ = nullptr;
    UIView::OnClickListener* clickMoveRightListener1_ = nullptr;
    UIView::OnClickListener* clickMoveTopListener1_ = nullptr;
    UIView::OnClickListener* clickMoveBottomListener1_ = nullptr;
    UILabelButton* SetUpButton(const char* title)
    {
        UILabelButton* btn = new UILabelButton();
        btn->Resize(BUTTON_WIDHT2, BUTTON_HEIGHT2);
        btn->SetText(title);
        btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
        return btn;
    }
};
}
#endif // UI_TEST_ANALOG_CLOCK_H
