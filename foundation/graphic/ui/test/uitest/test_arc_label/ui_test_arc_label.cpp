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

#include "ui_test_arc_label.h"
#include "common/screen.h"
#include "font/ui_font.h"

namespace OHOS {
namespace {
const int16_t GAP = 5;
const int16_t TITLE_HEIGHT = 29;
const uint16_t CENTER_X = 185;
const uint16_t CENTER_Y = 155;
const uint16_t RADIUS = 150;
} // namespace

void UITestArcLabel::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        positionX_ = 0;
        positionY_ = 0;
    }
}

void UITestArcLabel::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

void UITestArcLabel::InnerTestTitle(const char* title, int16_t x, int16_t y) const
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(x, y, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    container_->Add(titleLabel);
}

const UIView* UITestArcLabel::GetTestView()
{
    UIKit_UIArcLabel_Test_Display_001();
    UIKit_UIArcLabel_Test_Display_002();
    UIKit_UIArcLabel_Test_Display_003();
    UIKit_UIArcLabel_Test_Display_004();

    UIKit_UIArcLabel_Test_Align_001();
    UIKit_UIArcLabel_Test_Align_002();
    UIKit_UIArcLabel_Test_Align_003();
    return container_;
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Display_001()
{
    const char* title = "显示内部角度从0到270的弧形文本 ";
    // 0: start angle  270: end angle
    TestArcLabelDisplay(title, 0, 270, UIArcLabel::TextOrientation::INSIDE, VIEW_DISTANCE_TO_LEFT_SIDE,
                        VIEW_DISTANCE_TO_TOP_SIDE);
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Display_002()
{
    const char* title = "显示内部角度从0到-270的弧形文本 ";
    // 0: start angle  -270: end angle  100: y-coordinate
    TestArcLabelDisplay(title, 0, -270, UIArcLabel::TextOrientation::INSIDE, VIEW_DISTANCE_TO_LEFT_SIDE, 100);
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Display_003()
{
    const char* title = "显示外部角度从0到270的弧形文本 ";
    // 0: start angle  270: end angle  200: y-coordinate
    TestArcLabelDisplay(title, 0, 270, UIArcLabel::TextOrientation::OUTSIDE, VIEW_DISTANCE_TO_LEFT_SIDE, 200);
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Display_004()
{
    const char* title = "显示外部角度从0到-270的弧形文本 ";
    // 0: start angle  -270: end angle  300: y-coordinate
    TestArcLabelDisplay(title, 0, -270, UIArcLabel::TextOrientation::OUTSIDE, VIEW_DISTANCE_TO_LEFT_SIDE, 300);
}

void UITestArcLabel::TestArcLabelDisplay(const char* title,
                                         const int16_t startAngle,
                                         const int16_t endAngle,
                                         const UIArcLabel::TextOrientation orientation,
                                         int16_t x,
                                         int16_t y)
{
    if (container_ != nullptr) {
        InnerTestTitle(title, x, positionY_);
        UIArcLabel* label = new UIArcLabel();
        label->SetArcTextCenter(CENTER_X, CENTER_Y + positionY_ + GAP * 4); // 4: 4 times GAP
        label->SetArcTextRadius(RADIUS);
        label->SetArcTextAngle(startAngle, endAngle);
        label->SetArcTextOrientation(orientation);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        label->SetText("012345678ABCDEF0123456789ABCDE");
        label->SetStyle(STYLE_LETTER_SPACE, 10); // 10: space
        container_->Add(label);
        positionY_ += (RADIUS * 2) + GAP + TITLE_HEIGHT; // 2: diameter
    }
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Align_001()
{
    if (container_ != nullptr) {
        // 2: half of screen width
        positionX_ = Screen::GetInstance().GetWidth() / 2 + VIEW_DISTANCE_TO_LEFT_SIDE;
        positionY_ = 0;
        InnerTestTitle("弧形文本在0到90度之间左对齐 ", positionX_, positionY_);
        positionY_ += TITLE_HEIGHT + GAP;
        UIView* back = new UIView();
        back->SetPosition(positionX_, positionY_ + GAP);
        back->Resize(RADIUS, RADIUS);
        back->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
        back->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

        UIArcLabel* label = new UIArcLabel();
        label->SetArcTextCenter(positionX_, CENTER_Y + positionY_);
        label->SetArcTextRadius(RADIUS);
        label->SetArcTextAngle(0, 90);                    // 0: start angle, 90: end angle
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        label->SetText("LEFT");
        label->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_LEFT);
        label->SetStyle(STYLE_LETTER_SPACE, 6); // 6: space

        container_->Add(back);
        container_->Add(label);
        positionY_ += RADIUS + GAP + TITLE_HEIGHT;
    }
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Align_002()
{
    if (container_ != nullptr) {
        InnerTestTitle("弧形文本在0到90度之间居中对齐 ", positionX_, positionY_);
        positionY_ += TITLE_HEIGHT + GAP;
        UIView* back = new UIView();
        back->SetPosition(positionX_, positionY_ + GAP);
        back->Resize(RADIUS, RADIUS);
        back->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
        back->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

        UIArcLabel* label = new UIArcLabel();
        label->SetArcTextCenter(positionX_, CENTER_Y + positionY_);
        label->SetArcTextRadius(RADIUS);
        label->SetArcTextAngle(0, 90);                    // 0: start angle, 90: end angle
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        label->SetText("CENTER");
        label->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_CENTER);
        label->SetStyle(STYLE_LETTER_SPACE, 6); // 6: space

        container_->Add(back);
        container_->Add(label);
        positionY_ += RADIUS + GAP + TITLE_HEIGHT;
    }
}

void UITestArcLabel::UIKit_UIArcLabel_Test_Align_003()
{
    if (container_ != nullptr) {
        InnerTestTitle("弧形文本在0到90度之间右对齐 ", positionX_, positionY_);
        positionY_ += TITLE_HEIGHT + GAP;
        UIView* back = new UIView();
        back->SetPosition(positionX_, positionY_ + GAP);
        back->Resize(RADIUS, RADIUS);
        back->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
        back->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

        UIArcLabel* label = new UIArcLabel();
        label->SetArcTextCenter(positionX_, CENTER_Y + positionY_);
        label->SetArcTextRadius(RADIUS);
        label->SetArcTextAngle(0, 90);                    // 0: start angle, 90: end angle
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
        label->SetText("RIGHT");
        label->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
        label->SetStyle(STYLE_LETTER_SPACE, 6); // 6: space

        container_->Add(back);
        container_->Add(label);
        positionY_ += RADIUS + GAP + TITLE_HEIGHT;
    }
}
} // namespace OHOS
