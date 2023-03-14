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

#include "ui_test_animator.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
const int16_t LABEL_HEIGHT = 29;
const int16_t BUTTON_WIDTH = BUTTON_WIDHT3;
const int16_t BUTTON_HEIGHT = BUTTON_HEIGHT3;
const int16_t DELTA_X_COORDINATE = 5;
const int16_t DELTA_Y_COORDINATE = 12;
} // namespace

void UITestAnimator::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE);
        container_->Add(uiViewGroupFrame_);
        UILabel* label = new UILabel();
        container_->Add(label);
        // 264: label width; 48: label height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 264, 48);
        label->SetText("动画效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        image_ = new UIImageView();
        image_->SetSrc(RED_IMAGE_PATH);
        image_->SetPosition(0, 5);                                 // 5:y-coordinate
        callback_ = new ImageEaseAnimatorCallback(image_, 0, 338); // 338:endPos
        animator_ = callback_->GetAnimator();
        uiViewGroupFrame_->Add(image_);
        uiViewGroupFrame_->Resize(Screen::GetInstance().GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE,
                                  image_->GetHeight() + 10); // 10: increase y-coordinate
    }
    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        // 20: increase y-coordinate
        uint16_t y = uiViewGroupFrame_->GetY() + uiViewGroupFrame_->GetHeight() + 20;
        scroll_->SetPosition(0, y, Screen::GetInstance().GetWidth(),
                             Screen::GetInstance().GetHeight() - y - 64); // 64: decrease y-coordinate
        container_->Add(scroll_);
        positionX_ = 0;
    }
}

void UITestAnimator::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    image_ = nullptr;
    scroll_ = nullptr;
    delete callback_;
    callback_ = nullptr;
    uiViewGroupFrame_ = nullptr;
}

const UIView* UITestAnimator::GetTestView()
{
    UIKit_Animator_Test_BackEasing_001();
    UIKit_Animator_Test_CircEasing_002();
    UIKit_Animator_Test_CubicEasing_003();
    UIKit_Animator_Test_SineEasing_004();
    UIKit_Animator_Test_QuadEasing_005();
    UIKit_Animator_Test_QuintEasing_006();
    UIKit_Animator_Test_LinearEasing_007();
    UIKit_Animator_Test_FPS_008();
    return container_;
}

void UITestAnimator::SetUpLabel(const char* title, int16_t x, int16_t y)
{
    UILabel* label = GetTitleLabel(title);
    scroll_->Add(label);
    label->SetPosition(x, y, 288, LABEL_HEIGHT); // 288: label width
}

void UITestAnimator::SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y)
{
    if (btn == nullptr) {
        return;
    }
    scroll_->Add(btn);
    btn->SetPosition(x, y, BUTTON_WIDHT3, BUTTON_HEIGHT3);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    scroll_->Invalidate();
}

void UITestAnimator::UIKit_Animator_Test_BackEasing_001()
{
    backOvershootBtn_ = new UILabelButton();
    backEaseInBtn_ = new UILabelButton();
    backEaseOutBtn_ = new UILabelButton();
    backEaseInOutBtn_ = new UILabelButton();
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 0;
    SetUpLabel("back动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(backOvershootBtn_, "overshoot+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(backEaseInBtn_, "BackEaseIn", positionX_, positionY_);
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(backEaseOutBtn_, "BackEaseOut", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(backEaseInOutBtn_, "BackEaseInOut", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_CircEasing_002()
{
    circEaseInBtn_ = new UILabelButton();
    circEaseOutBtn_ = new UILabelButton();
    circEaseInOutBtn_ = new UILabelButton();
    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 0;
    SetUpLabel("circ动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(circEaseInBtn_, "CircEaseIn", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(circEaseOutBtn_, "CircEaseOut", positionX_, positionY_);
    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(circEaseInOutBtn_, "CircEaseInOut", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_CubicEasing_003()
{
    cubicEaseInBtn_ = new UILabelButton();
    cubicEaseOutBtn_ = new UILabelButton();
    cubicEaseInOutBtn_ = new UILabelButton();
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 148; // 148: y-coordinate
    SetUpLabel("cubic动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(cubicEaseInBtn_, "CubicEaseIn", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(cubicEaseOutBtn_, "CubicEaseOut", positionX_, positionY_);
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(cubicEaseInOutBtn_, "CubicEaseInOut", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_SineEasing_004()
{
    sineEaseInBtn_ = new UILabelButton();
    sineEaseOutBtn_ = new UILabelButton();
    sineEaseInOutBtn_ = new UILabelButton();
    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 148; // 148: y-coordinate
    SetUpLabel("sine动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(sineEaseInBtn_, "SineEaseIn", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(sineEaseOutBtn_, "SineEaseOut", positionX_, positionY_);
    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(sineEaseInOutBtn_, "SineEaseInOut", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_QuadEasing_005()
{
    quadEaseInBtn_ = new UILabelButton();
    quadEaseOutBtn_ = new UILabelButton();
    quadEaseInOutBtn_ = new UILabelButton();
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 296; // 296: y-coordinate
    SetUpLabel("quad动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(quadEaseInBtn_, "QuadEaseIn", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(quadEaseOutBtn_, "QuadEaseOut", positionX_, positionY_);
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(quadEaseInOutBtn_, "QuadEaseInOut", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_QuintEasing_006()
{
    quintEaseInBtn_ = new UILabelButton();
    quintEaseOutBtn_ = new UILabelButton();
    quintEaseInOutBtn_ = new UILabelButton();
    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 296; // 296: y-coordinate
    SetUpLabel("quint动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(quintEaseInBtn_, "QuintEaseIn", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(quintEaseOutBtn_, "QuintEaseOut", positionX_, positionY_);
    // 2: half of screen width
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(quintEaseInOutBtn_, "QuintEaseInOut", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_LinearEasing_007()
{
    linearEaseNoneBtn_ = new UILabelButton();
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = 444; // 444: y-coordinate
    SetUpLabel("linear动画效果： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(linearEaseNoneBtn_, "LinearEaseNone", positionX_, positionY_);
}

void UITestAnimator::UIKit_Animator_Test_FPS_008()
{
    fpsLabel_ = new UILabel();
    // 90:x-coordinate, 0:y-coordinate, 90:new width, 20:new height
    fpsLabel_->SetPosition(Screen::GetInstance().GetWidth() - 90, TEXT_DISTANCE_TO_TOP_SIDE, 90,
                           TITLE_LABEL_DEFAULT_HEIGHT);
    fpsLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    char buf[10] = "FPS";
    fpsLabel_->SetText(reinterpret_cast<char*>(buf));
    OnFPSChangedListener::SetFPSCalculateType(SysInfo::FPS_CT_PRECISE_SAMPLING);
    container_->Add(fpsLabel_);
    fpsLabel_->Invalidate();
}

bool UITestAnimator::OnClick(UIView& view, const ClickEvent& event)
{
    static double overshoot = 1.7;
    if (&view == backOvershootBtn_) {
        overshoot += 1;
        EasingEquation::SetBackOvershoot(overshoot);
    } else if (&view == backEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::BackEaseIn);
    } else if (&view == backEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::BackEaseOut);
    } else if (&view == backEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::BackEaseInOut);
    } else if (&view == circEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::CircEaseIn);
    } else if (&view == circEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::CircEaseOut);
    } else if (&view == circEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::CircEaseInOut);
    } else if (&view == cubicEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::CubicEaseIn);
    } else if (&view == cubicEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::CubicEaseOut);
    } else if (&view == cubicEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::CubicEaseInOut);
    } else if (&view == linearEaseNoneBtn_) {
        callback_->SetEasingFunc(EasingEquation::LinearEaseNone);
    } else if (&view == quadEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::QuadEaseIn);
    } else if (&view == quadEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::QuadEaseOut);
    } else if (&view == quadEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::QuadEaseInOut);
    } else if (&view == quintEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::QuintEaseIn);
    } else if (&view == quintEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::QuintEaseOut);
    } else if (&view == quintEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::QuintEaseInOut);
    } else if (&view == sineEaseInBtn_) {
        callback_->SetEasingFunc(EasingEquation::SineEaseIn);
    } else if (&view == sineEaseOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::SineEaseOut);
    } else if (&view == sineEaseInOutBtn_) {
        callback_->SetEasingFunc(EasingEquation::SineEaseInOut);
    }
    animator_->Start();
    container_->Invalidate();
    return true;
}

void UITestAnimator::OnFPSChanged(float newFPS)
{
    char buf[10] = "FPS";
    fpsLabel_->SetText(reinterpret_cast<char*>(buf));
    fpsLabel_->Invalidate();
}
} // namespace OHOS
