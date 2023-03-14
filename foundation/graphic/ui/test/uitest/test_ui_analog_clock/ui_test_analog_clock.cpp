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

#include "ui_test_analog_clock.h"

#include "common/screen.h"
#include "components/ui_analog_clock.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
const uint8_t BUTTON_TEXT_SIZE = 16;
const uint8_t BUTTON_WIDTH = 120;
const uint8_t BUTTON_HEIGHT = 40;
} // namespace

void UITestAnalogClock::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }
}

void UITestAnalogClock::TearDown()
{
    if (animator_ != nullptr) {
        delete animator_;
        delete callback_;
        animator_ = nullptr;
        callback_ = nullptr;
    }

    if (animator2_ != nullptr) {
        delete animator2_;
        delete callback2_;
        animator2_ = nullptr;
        callback2_ = nullptr;
    }
    if (changeModeListener_ != nullptr) {
        delete changeModeListener_;
        changeModeListener_ = nullptr;
    }
    if (clickMoveLeftListener_ != nullptr) {
        delete clickMoveLeftListener_;
        clickMoveLeftListener_ = nullptr;
    }
    if (clickMoveRightListener_ != nullptr) {
        delete clickMoveRightListener_;
        clickMoveRightListener_ = nullptr;
    }
    if (clickMoveTopListener_ != nullptr) {
        delete clickMoveTopListener_;
        clickMoveTopListener_ = nullptr;
    }
    if (clickMoveBottomListener_ != nullptr) {
        delete clickMoveBottomListener_;
        clickMoveBottomListener_ = nullptr;
    }
    if (changeModeListener1_ != nullptr) {
        delete changeModeListener1_;
        changeModeListener1_ = nullptr;
    }
    if (clickMoveLeftListener1_ != nullptr) {
        delete clickMoveLeftListener1_;
        clickMoveLeftListener1_ = nullptr;
    }
    if (clickMoveRightListener1_ != nullptr) {
        delete clickMoveRightListener1_;
        clickMoveRightListener1_ = nullptr;
    }
    if (clickMoveTopListener1_ != nullptr) {
        delete clickMoveTopListener1_;
        clickMoveTopListener1_ = nullptr;
    }
    if (clickMoveBottomListener1_ != nullptr) {
        delete clickMoveBottomListener1_;
        clickMoveBottomListener1_ = nullptr;
    }
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestAnalogClock::GetTestView()
{
    UIKit_TestLineHandAnalogClock_001();
    UIKit_TestImageHandAnalogClock_002();
    return container_;
}

class TestBtnOnClickChangeModeListener : public UIView::OnClickListener {
public:
    TestBtnOnClickChangeModeListener(UIView* uiView, UIAnalogClock* clock) : uiView_(uiView), clock_(clock) {}
    ~TestBtnOnClickChangeModeListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        if (clock_->GetWorkMode() == UIAbstractClock::ALWAYS_ON) {
            clock_->SetWorkMode(UIAbstractClock::NORMAL);
        } else {
            clock_->SetWorkMode(UIAbstractClock::ALWAYS_ON);
        }
        return true;
    }

private:
    UIView* uiView_;
    UIAnalogClock* clock_;
};

class TestBtnOnClickMovePositionListener : public UIView::OnClickListener {
public:
    enum class MoveType {
        MOVE_LEFT,
        MOVE_RIGHT,
        MOVE_TOP,
        MOVE_BOTTOM,
    };
    TestBtnOnClickMovePositionListener(UIView* uiView, UIAnalogClock* clock, MoveType type, uint16_t grid)
        : uiView_(uiView), clock_(clock), type_(type), grid_(grid)
    {
    }
    ~TestBtnOnClickMovePositionListener() {}
    bool OnClick(UIView& view, const ClickEvent& event) override
    {
        int16_t xPos = clock_->GetX();
        int16_t yPos = clock_->GetY();
        clock_->Invalidate();
        if (type_ == MoveType::MOVE_LEFT) {
            xPos -= grid_;
        } else if (type_ == MoveType::MOVE_RIGHT) {
            xPos += grid_;
        } else if (type_ == MoveType::MOVE_TOP) {
            yPos -= grid_;
        } else if (type_ == MoveType::MOVE_BOTTOM) {
            yPos += grid_;
        }
        clock_->SetPosition(xPos, yPos);
        return true;
    }

private:
    UIView* uiView_;
    UIAnalogClock* clock_;
    MoveType type_;
    uint16_t grid_;
};

void UITestAnalogClock::UIKit_TestLineHandAnalogClock_001()
{
    if (container_ != nullptr) {
        UIViewGroup* group1 = new UIViewGroup();
        group1->SetPosition(0, 0);
        group1->SetWidth(Screen::GetInstance().GetWidth());
        group1->SetHeight(Screen::GetInstance().GetHeight() + 100); // 100: height bigger value
        container_->Add(group1);

        UILabel* label = new UILabel();
        group1->Add(label);
        // 30: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, Screen::GetInstance().GetWidth(), 30);
        label->SetText("直线指针效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIAnalogClock* clock1 = new UIAnalogClock();
        group1->Add(clock1);
        clock1->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE,
                            Screen::GetInstance().GetWidth() / 2, // 2: half of screen width
                            Screen::GetInstance().GetHeight());

        UIImageView* curFace = new UIImageView();
        curFace->SetSrc(B024_002_IMAGE_PATH);
        clock1->Add(curFace);
        curFace->SetPosition(0, 0);
        // 214: position x 3:position y 14:center x 223:center y 1: width 223:height 255: opacity
        clock1->SetHandLine(UIAnalogClock::HandType::SECOND_HAND, {214, 3}, {14, 223}, Color::White(), 1, 223, 255);
        // 202: position x 25:position y 25:center x 201:center y 3: width 201:height 255: opacity
        clock1->SetHandLine(UIAnalogClock::HandType::MINUTE_HAND, {202, 25}, {25, 201}, Color::Green(), 3, 201, 255);
        // 202: position x 95:position y 25:center x 132:center y 5: width 132: height 255: opacity
        clock1->SetHandLine(UIAnalogClock::HandType::HOUR_HAND, {202, 95}, {25, 132}, Color::Red(), 5, 132, 255);
        clock1->SetInitTime24Hour(0, 0, 0);
        clock1->SetWorkMode(UIAnalogClock::WorkMode::NORMAL);
        clock1->SetTime24Hour(10, 3, 20); // 10: hour; 3: minute: 20: second
        CreateButtons001(group1, curFace, clock1);
        callback_ = new ClockAnimatorCallback(clock1);
        animator_ = new Animator(callback_, clock1, 0, true);
        animator_->Start();
    }
}

void UITestAnalogClock::CreateButtons001(UIViewGroup* group, UIImageView* curFace, UIAnalogClock* clock)
{
    if ((group == nullptr) || (curFace == nullptr) || (clock == nullptr)) {
        return;
    }
    UILabelButton* button1 = SetUpButton("切换模式");
    group->Add(button1);
    // 58: increase x-position; 48: y-position
    button1->SetPosition(curFace->GetWidth() + 58, 48, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (changeModeListener_ == nullptr) {
        changeModeListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickChangeModeListener(static_cast<UIView*>(button1), clock));
    }
    button1->SetOnClickListener(changeModeListener_);

    UILabelButton* button2 = SetUpButton("左移");
    group->Add(button2);
    // 58: increase x-position; 58: increase y-position
    button2->SetPosition(curFace->GetWidth() + 58, BUTTON_HEIGHT + 58, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveLeftListener_ == nullptr) {
        clickMoveLeftListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button2), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_LEFT, 10)); // 10: grid
    }
    button2->SetOnClickListener(clickMoveLeftListener_);

    UILabelButton* button3 = SetUpButton("右移");
    group->Add(button3);
    // 58: increase x-position; 148: y-position
    button3->SetPosition(curFace->GetWidth() + 58, 148, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveRightListener_ == nullptr) {
        clickMoveRightListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button3), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_RIGHT, 10)); // 10: grid
    }
    button3->SetOnClickListener(clickMoveRightListener_);

    UILabelButton* button4 = SetUpButton("上移");
    group->Add(button4);
    // 58: increase x-position; 198: y-position
    button4->SetPosition(curFace->GetWidth() + 58, 198, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveTopListener_ == nullptr) {
        clickMoveTopListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button4), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_TOP, 10)); // 10: move value
    }
    button4->SetOnClickListener(clickMoveTopListener_);

    UILabelButton* button5 = SetUpButton("下移");
    group->Add(button5);
    // 58: increase x-position; 248: y-position
    button5->SetPosition(curFace->GetWidth() + 58, 248, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveBottomListener_ == nullptr) {
        clickMoveBottomListener_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button5), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_BOTTOM, 10)); // 10: move value
    }
    button5->SetOnClickListener(clickMoveBottomListener_);
}

void UITestAnalogClock::UIKit_TestImageHandAnalogClock_002()
{
    if (container_ != nullptr) {
        UIViewGroup* group2 = new UIViewGroup();
        group2->SetPosition(0, Screen::GetInstance().GetHeight() + 50); // 50: increase y-coordinate
        group2->SetWidth(Screen::GetInstance().GetWidth());
        group2->SetHeight(Screen::GetInstance().GetHeight() + 100); // 100: increase y-coordinate
        container_->Add(group2);

        UILabel* label = new UILabel();
        group2->Add(label);
        label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, 0, Screen::GetInstance().GetWidth(), 30); // 30: height
        label->SetText("图片指针效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);

        UIImageView* image1 = new UIImageView();
        image1->SetSrc(B024_010_IMAGE_PATH);
        UIImageView* image2 = new UIImageView();
        image2->SetSrc(B024_011_IMAGE_PATH);
        UIImageView* image3 = new UIImageView();
        image3->SetSrc(B024_009_IMAGE_PATH);

        UIAnalogClock* clock2 = new UIAnalogClock();
        group2->Add(clock2);
        clock2->SetDragParentInstead(true);
        clock2->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE,
                            Screen::GetInstance().GetWidth() / 2, // 2: half of screen width
                            Screen::GetInstance().GetHeight());

        UIImageView* curFace = new UIImageView();
        curFace->SetSrc(B024_002_IMAGE_PATH);
        clock2->Add(curFace);
        curFace->SetPosition(0, 0);
        // {214, 3}: position x, y {14, 223}: center x, y
        clock2->SetHandImage(UIAnalogClock::HandType::SECOND_HAND, *image1, {214, 3}, {14, 223});
        // {202, 15}: position x, y {25, 211}: center x, y
        clock2->SetHandImage(UIAnalogClock::HandType::MINUTE_HAND, *image2, {202, 15}, {25, 211});
        // {202, 15}: position x, y {25, 211}: center x, y
        clock2->SetHandImage(UIAnalogClock::HandType::HOUR_HAND, *image3, {202, 15}, {25, 211});

        clock2->SetInitTime24Hour(0, 0, 0);
        clock2->SetWorkMode(UIAnalogClock::WorkMode::NORMAL);
        clock2->SetTime24Hour(3, 3, 15); // 3: hour; 3: minute: 15: second
        CreateButtons002(group2, curFace, clock2);
        callback2_ = new ClockAnimatorCallback(clock2);
        animator2_ = new Animator(callback2_, clock2, 0, true);
        animator2_->Start();
    }
}

void UITestAnalogClock::CreateButtons002(UIViewGroup* group, UIImageView* curFace, UIAnalogClock* clock)
{
    if ((group == nullptr) || (curFace == nullptr) || (clock == nullptr)) {
        return;
    }
    UILabelButton* button1 = SetUpButton("切换模式");
    group->Add(button1);
    // 58: increase x-coordinate; 48: y-coordinate
    button1->SetPosition(curFace->GetWidth() + 58, 48, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (changeModeListener1_ == nullptr) {
        changeModeListener1_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickChangeModeListener(static_cast<UIView*>(button1), clock));
    }
    button1->SetOnClickListener(changeModeListener1_);

    UILabelButton* button2 = SetUpButton("左移");
    group->Add(button2);
    // 58: increase x-coordinate; 98: y-coordinate
    button2->SetPosition(curFace->GetWidth() + 58, 98, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveLeftListener1_ == nullptr) {
        clickMoveLeftListener1_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button2), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_LEFT, 10)); // 10: grid
    }
    button2->SetOnClickListener(clickMoveLeftListener1_);

    UILabelButton* button3 = SetUpButton("右移");
    group->Add(button3);
    // 58: increase x-coordinate; 148: y-coordinate
    button3->SetPosition(curFace->GetWidth() + 58, 148, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveRightListener1_ == nullptr) {
        clickMoveRightListener1_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button3), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_RIGHT, 10)); // 10: move value
    }
    button3->SetOnClickListener(clickMoveRightListener1_);

    UILabelButton* button4 = SetUpButton("上移");
    group->Add(button4);
    // 58: increase x-coordinate; 198: y-coordinate
    button4->SetPosition(curFace->GetWidth() + 58, 198, BUTTON_WIDTH, BUTTON_HEIGHT);
    if (clickMoveTopListener1_ == nullptr) {
        clickMoveTopListener1_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button4), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_TOP, 10)); // 10: move value
    }
    button4->SetOnClickListener(clickMoveTopListener1_);
    UILabelButton* button5 = SetUpButton("下移");
    group->Add(button5);
    // 58: increase x-coordinate; 248: y-coordinate
    button5->SetPosition(curFace->GetWidth() + 58, 248, BUTTON_WIDTH, BUTTON_HEIGHT);
    /* Move bottom */
    if (clickMoveBottomListener1_ == nullptr) {
        clickMoveBottomListener1_ = static_cast<UIView::OnClickListener*>(
            new TestBtnOnClickMovePositionListener(
            static_cast<UIView*>(button5), clock,
            TestBtnOnClickMovePositionListener::MoveType::MOVE_BOTTOM, 10)); // 10: move value
    }
    button5->SetOnClickListener(clickMoveBottomListener1_);
}
} // namespace OHOS
