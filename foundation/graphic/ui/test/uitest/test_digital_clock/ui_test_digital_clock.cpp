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

#include "ui_test_digital_clock.h"
#include "common/screen.h"
#include "components/ui_digital_clock.h"
#include "components/ui_label.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
const int16_t GAP = 10;
const int16_t TITLE_HEIGHT = 30;
const uint16_t DIGITAL_CLOCK_WIDTH = 350;
const uint16_t DIGITAL_CLOCK_HEIGHT = 50;
} // namespace

void UITestDigitalClock::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        positionX_ = 50; // 50: init position x
        positionY_ = 5;  // 5: init position y
    }
}

void UITestDigitalClock::TearDown()
{
    UIView* view = static_cast<UIViewGroup*>(container_)->GetChildrenHead();
    while (view != nullptr) {
        UIView* tempView = view;
        view = view->GetNextSibling();
        if (tempView->GetParent()) {
            static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
    }
    delete container_;
    container_ = nullptr;
}

void UITestDigitalClock::InnerTestTitle(const char* title)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(),
                            TITLE_LABEL_DEFAULT_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + GAP;
}

const UIView* UITestDigitalClock::GetTestView()
{
    UIKit_UIDigitalClock_Test_Display_001();
    UIKit_UIDigitalClock_Test_Display_002();
    UIKit_UIDigitalClock_Test_SetColor();
    UIKit_UIDigitalClock_Test_DisplayMode_001();
    UIKit_UIDigitalClock_Test_DisplayMode_002();
    UIKit_UIDigitalClock_Test_DisplayMode_003();
    UIKit_UIDigitalClock_Test_DisplayMode_004();
    UIKit_UIDigitalClock_Test_DisplayLeadingZero();
    UIKit_UIDigitalClock_Test_SetOpacity();
    return container_;
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_Display_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("UIDigitalClock效果");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(7, 25, 50);                  // 7: hour, 25: minute, 50: second
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_Display_002()
{
    if (container_ != nullptr) {
        InnerTestTitle("UIDigitalClock效果");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(7, 25, 50);                  // 7:hour, 25:minute, 50:second
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_SetColor()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock颜色");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(7, 25, 50);                  // 7:hour, 25:minute, 50:second
        dClock->SetColor(Color::Red());
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_DisplayMode_001()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock仅显示小时、分钟 ");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(17, 25, 50);                 // 17:hour, 25:minute, 50:second
        dClock->SetDisplayMode(UIDigitalClock::DISPLAY_12_HOUR_NO_SECONDS);
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_DisplayMode_002()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock 24小时制仅显示小时、分钟 ");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(17, 25, 50);                 // 17:hour, 25:minute, 50:second
        dClock->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR_NO_SECONDS);
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_DisplayMode_003()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock 12小时制 ");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(17, 25, 50);                 // 17:hour, 25:minute, 50:second
        dClock->SetDisplayMode(UIDigitalClock::DISPLAY_12_HOUR);
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_DisplayMode_004()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock 24小时制 ");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(17, 25, 50);                 // 17:hour, 25:minute, 50:second
        dClock->SetDisplayMode(UIDigitalClock::DISPLAY_24_HOUR);
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_DisplayLeadingZero()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock不显示首位的0 ");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(7, 5, 9);                    // 7:hour, 5:minute, 9:second
        dClock->DisplayLeadingZero(false);
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}

void UITestDigitalClock::UIKit_UIDigitalClock_Test_SetOpacity()
{
    if (container_ != nullptr) {
        InnerTestTitle("设置Digital Clock透明度 ");
        UIDigitalClock* dClock = new UIDigitalClock();
        dClock->SetPosition(positionX_, positionY_);
        dClock->Resize(DIGITAL_CLOCK_WIDTH, DIGITAL_CLOCK_HEIGHT);
        dClock->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        dClock->SetTime24Hour(7, 5, 9);                    // 7:hour, 5:minute, 9:second
        dClock->SetOpacity(178);                           // 178: num of opacity
        container_->Add(dClock);
        positionY_ += DIGITAL_CLOCK_HEIGHT + GAP;
    }
}
} // namespace OHOS