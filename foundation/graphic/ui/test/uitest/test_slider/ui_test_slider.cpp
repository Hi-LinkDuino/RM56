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

#include "ui_test_slider.h"
#include "common/screen.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
constexpr int16_t LABEL_HEIGHT = 29;
constexpr int16_t BUTTON_WIDTH = 120;
constexpr int16_t BUTTON_HEIGHT = 40;
constexpr int16_t DELTA_X_COORDINATE = 8;
constexpr int16_t DELTA_Y_COORDINATE = 12;
constexpr int16_t DELTA_Y_COORDINATE_2 = 19;
static bool g_onChange = false;
static bool g_onRelease = false;
static bool g_onClick = false;
static bool g_onDrag = false;
static uint16_t g_progress = 20;
static uint16_t g_step = 0;
static uint16_t g_width = 50;
static uint16_t g_height = 250;
static int16_t g_min = 0;
static int16_t g_max = 100;
static int16_t g_knobWidth = 50;
constexpr int8_t BACKGROUND_OPA = 38;
constexpr uint8_t FOREGROUND_COLOR_R = 0x1f;
constexpr uint8_t FOREGROUND_COLOR_G = 0x71;
constexpr uint8_t FOREGROUND_COLOR_B = 0xff;
} // namespace

void TestUISliderEventListener::OnChange(int32_t progress)
{
    value_ = progress;
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return;
        }
    }
    if (g_onChange) {
        label_->SetText("OnChange");
    }
}

void TestUISliderEventListener::OnRelease(int32_t progress)
{
    value_ = progress;
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return;
        }
    }
    if (g_onRelease) {
        label_->SetText("OnRelease");
    }
}

bool TestUISliderEventListener::OnClick(UIView& view, const ClickEvent& event)
{
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return false;
        }
    }
    if (g_onClick) {
        label_->SetText("OnClick");
    }
    return true;
}

bool TestUISliderEventListener::MallocLabel()
{
    label_ = new UILabel();
    if (label_ == nullptr) {
        return false;
    }
    viewGroup_->Add(label_);
    label_->SetPosition(0, 0, 100, 30); // 100: width; 30: height
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label_->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    return true;
}

bool TestUISliderEventListener::OnDrag(UIView& view, const DragEvent& event)
{
    if (label_ == nullptr) {
        if (!MallocLabel()) {
            return false;
        }
    }

    if (g_onDrag) {
        label_->SetText("OnDrag");
    }
    return true;
}

void UITestSlider::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        positionX_ = 0;
        positionY_ = 0;
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(positionX_, positionY_, 320, 390); // 320: width; 390: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: width; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("UISlider效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame_);
        // 288: width; 336: height
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 288, 336);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);

        slider_ = new UISlider();
        slider_->SetPosition(10, 10, 50, 300); // 10:position x; 10: position y; 50: width; 300: height
        slider_->SetValidHeight(250);          // 250: valid height;
        slider_->SetValue(20);                 // 20:  progress bar current value
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        listener_ = new TestUISliderEventListener(uiViewGroupFrame_);
        slider_->SetSliderEventListener(listener_);
        slider_->SetOnClickListener(listener_);
        slider_->SetOnDragListener(listener_);
        uiViewGroupFrame_->Add(slider_);
        slider_->LayoutCenterOfParent();
    }

    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        scroll_->SetIntercept(true);
        scroll_->SetPosition(336, 11);                                // 336: x-coordinate; 11: y-coordinate
        scroll_->Resize(624, Screen::GetInstance().GetHeight() - 70); // 624: width; 70: decrease height
        container_->Add(scroll_);
        positionY_ = 0;
    }
}

void UITestSlider::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    slider_ = nullptr;
    scroll_ = nullptr;
    delete listener_;
    listener_ = nullptr;
    uiViewGroupFrame_ = nullptr;
}

const UIView* UITestSlider::GetTestView()
{
    UIKit_Slider_Test_UISlider_001();
    UIKit_Slider_Test_SetRange_002();
    UIKit_Slider_Test_SetValue_003();
    UIKit_Slider_Test_SetImage_004();
    UIKit_Slider_Test_SetStyle_005();
    UIKit_Slider_Test_GetStyle_006();
    UIKit_Slider_Test_SetKnobWidth_007();
    UIKit_Slider_Test_SetStep_008();
    UIKit_Slider_Test_SetColor_009();
    UIKit_Slider_Test_SetDirection_010();
    UIKit_Slider_Test_SetValidSize_011();
    UIKit_Slider_Test_SetRadius_012();
    UIKit_Slider_Test_SetOnChangeListener_013();
    UIKit_Slider_Test_SetOnReleaseListener_014();
    UIKit_Slider_Test_SetOnClickCallback_015();
    UIKit_Slider_Test_SetOnDragCallback_016();
    UIKit_Slider_Test_SetCircularStyle_017();

    return container_;
}

void UITestSlider::SetUpLabel(const char* title, int16_t x, int16_t y) const
{
    UILabel* label = new UILabel();
    scroll_->Add(label);
    // 288: width
    label->SetPosition(x, y, 288, LABEL_HEIGHT);
    label->SetText(title);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
}
void UITestSlider::SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y)
{
    if (btn == nullptr) {
        return;
    }
    scroll_->Add(btn);
    btn->SetPosition(x, y, BUTTON_WIDHT2, BUTTON_HEIGHT2);
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

void UITestSlider::UIKit_Slider_Test_UISlider_001()
{
    resetBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ = 0;
    SetUpLabel("重置Slider:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(resetBtn_, "重置", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetRange_002()
{
    incMinProgressBtn_ = new UILabelButton();
    decMinProgressBtn_ = new UILabelButton();
    incMaxProgressBtn_ = new UILabelButton();
    decMaxProgressBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider范围:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incMinProgressBtn_, "下限+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decMinProgressBtn_, "下限-", positionX_, positionY_);
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(incMaxProgressBtn_, "上限+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decMaxProgressBtn_, "上限-", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetValue_003()
{
    incProgressBtn_ = new UILabelButton();
    decProgressBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider当前值：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incProgressBtn_, "进度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decProgressBtn_, "进度-", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetImage_004()
{
    imageBtn_ = new UILabelButton();
    noImageBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置图片:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(imageBtn_, "设置图片", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(noImageBtn_, "取消图片", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetStyle_005()
{
    setStyleBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(setStyleBtn_, "设置样式", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_GetStyle_006()
{
    getStyleBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(getStyleBtn_, "设置样式", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetKnobWidth_007()
{
    incKnobWidthBtn_ = new UILabelButton();
    decKnobWidthBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider滑块宽度:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incKnobWidthBtn_, "滑块宽度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decKnobWidthBtn_, "滑块宽度-", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetStep_008()
{
    stepBtn_ = new UILabelButton();
    positionX_ = 24; // 24: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider步长:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(stepBtn_, "步长+", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetColor_009()
{
    colorBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ = 0;
    SetUpLabel("设置Slider颜色:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(colorBtn_, "设置颜色", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetDirection_010()
{
    dirL2RBtn_ = new UILabelButton();
    dirR2LBtn_ = new UILabelButton();
    dirT2BBtn_ = new UILabelButton();
    dirB2TBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider方向:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(dirL2RBtn_, "从左到右", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(dirR2LBtn_, "从右到左", positionX_, positionY_);
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(dirT2BBtn_, "从上到下", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(dirB2TBtn_, "从下到上", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetValidSize_011()
{
    widthBtn_ = new UILabelButton();
    heightBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider有效区域宽高:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(heightBtn_, "box高度 ", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(widthBtn_, "box宽度 ", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetRadius_012()
{
    radiusBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置Slider圆角弧度:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(radiusBtn_, "圆角弧度", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetOnChangeListener_013()
{
    onChangeBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置点击和滑动监听： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onChangeBtn_, "OnChange", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetOnReleaseListener_014()
{
    onReleaseBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置滑动结束监听:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onReleaseBtn_, "OnRelease", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetOnClickCallback_015()
{
    onClickBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置点击事件回调:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onClickBtn_, "OnClick回调", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetOnDragCallback_016()
{
    onDragBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置滑动事件回调:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(onDragBtn_, "OnDrag回调", positionX_, positionY_);
}

void UITestSlider::UIKit_Slider_Test_SetCircularStyle_017()
{
    circularStyleBtn_ = new UILabelButton();
    positionX_ = 336; // 336: x-coordinate
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("Slider设置圆角无滑块样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(circularStyleBtn_, "圆角无滑块样式", positionX_, positionY_);
}
bool UITestSlider::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == resetBtn_) {
        slider_->LayoutCenterOfParent();
        g_progress = 20; // 20: progress
        g_step = 0;
        g_width = 50;     // 50: width
        g_height = 250;   // 250: height
        g_knobWidth = 50; // 50: knobWidth
        g_min = 0;
        g_max = 100; // 100: max
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->SetKnobWidth(50); // 50: knob width
        slider_->SetRange(g_max, g_min);
        slider_->SetValue(g_progress);
        slider_->SetStep(g_step);
        slider_->SetBackgroundStyle(StyleDefault::GetProgressBackgroundStyle());
        slider_->SetForegroundStyle(StyleDefault::GetProgressForegroundStyle());
        slider_->SetKnobStyle(StyleDefault::GetSliderKnobStyle());
        slider_->SetImage(static_cast<ImageInfo*>(nullptr), static_cast<ImageInfo*>(nullptr));
        slider_->SetKnobImage(static_cast<ImageInfo*>(nullptr));
        slider_->EnableBackground(true);
    } else if (&view == incProgressBtn_) {
        g_progress++;
        slider_->SetValue(g_progress);
    } else if (&view == decProgressBtn_) {
        g_progress--;
        slider_->SetValue(g_progress);
    } else if (&view == stepBtn_) {
        g_step++;
        slider_->SetStep(g_step);
    } else {
        ExpandClick1(view, event);
    }
    container_->Invalidate();
    return true;
}

bool UITestSlider::ExpandClick1(UIView& view, const ClickEvent& event)
{
    if (&view == widthBtn_) {
        g_width--;
        slider_->SetValidWidth(g_width);
    } else if (&view == heightBtn_) {
        g_height--;
        slider_->SetValidHeight(g_height);
    } else if (&view == incMinProgressBtn_) {
        g_min++;
        slider_->SetRange(slider_->GetRangeMax(), g_min);
    } else if (&view == decMinProgressBtn_) {
        g_min--;
        slider_->SetRange(slider_->GetRangeMax(), g_min);
    } else if (&view == incMaxProgressBtn_) {
        g_max++;
        slider_->SetRange(g_max, slider_->GetRangeMin());
    } else if (&view == decMaxProgressBtn_) {
        g_max--;
        slider_->SetRange(g_max, slider_->GetRangeMin());
    } else if (&view == dirL2RBtn_) {
        g_width = DEFAULT_HEIGHT;
        g_height = DEFAULT_WIDTH;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
        slider_->LayoutCenterOfParent();
    } else if (&view == dirR2LBtn_) {
        g_width = DEFAULT_HEIGHT;
        g_height = DEFAULT_WIDTH;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_RIGHT_TO_LEFT);
        slider_->LayoutCenterOfParent();
    } else if (&view == dirT2BBtn_) {
        g_width = DEFAULT_WIDTH;
        g_height = DEFAULT_HEIGHT;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_TOP_TO_BOTTOM);
        slider_->LayoutCenterOfParent();
    } else if (&view == dirB2TBtn_) {
        g_width = DEFAULT_WIDTH;
        g_height = DEFAULT_HEIGHT;
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->Resize(g_width, g_height);
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
        slider_->LayoutCenterOfParent();
    } else {
        ExpandClick2(view, event);
    }
    return true;
}

bool UITestSlider::ExpandClick2(UIView& view, const ClickEvent& event)
{
    if (&view == imageBtn_) {
        slider_->SetValidHeight(DEFAULT_HEIGHT);
        slider_->SetValidWidth(5); // 5 valid width
        slider_->SetKnobWidth(40); // 40: knob width
        slider_->SetImage(SLIDER_BACKGROUND_IMAGE_PATH, SLIDER_INDICATOR_IMAGE_PATH);
        slider_->SetKnobImage(SLIDER_KNOB_IMAGE_PATH);
        slider_->SetDirection(UISlider::Direction::DIR_BOTTOM_TO_TOP);
    } else if (&view == noImageBtn_) {
        slider_->SetValidHeight(g_height);
        slider_->SetValidWidth(g_width);
        slider_->SetKnobWidth(g_knobWidth);
        slider_->SetImage(static_cast<ImageInfo*>(nullptr), static_cast<ImageInfo*>(nullptr));
        slider_->SetKnobImage(static_cast<ImageInfo*>(nullptr));
    } else if (&view == setStyleBtn_) {
        Style style = StyleDefault::GetDefaultStyle();
        style.bgColor_ = Color::Green();
        slider_->SetBackgroundStyle(style);
        style.bgColor_ = Color::Red();
        slider_->SetForegroundStyle(style);
        style.bgColor_ = Color::Gray();
        slider_->SetKnobStyle(style);
    } else if (&view == getStyleBtn_) {
        slider_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        slider_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
        slider_->SetKnobStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    } else if (&view == incKnobWidthBtn_) {
        g_knobWidth++;
        slider_->SetKnobWidth(g_knobWidth);
    } else if (&view == decKnobWidthBtn_) {
        g_knobWidth--;
        slider_->SetKnobWidth(g_knobWidth);
    } else if (&view == colorBtn_) {
        slider_->SetSliderColor(Color::Silver(), Color::Blue());
        slider_->SetKnobColor(Color::White());
    } else if (&view == radiusBtn_) {
        slider_->SetSliderRadius(DEFAULT_RADIUS, DEFAULT_RADIUS);
        slider_->SetKnobRadius(DEFAULT_RADIUS);
    } else if (&view == onChangeBtn_) {
        g_onChange = true;
    } else if (&view == onReleaseBtn_) {
        g_onRelease = true;
    } else if (&view == onClickBtn_) {
        g_onClick = true;
    } else if (&view == onDragBtn_) {
        g_onDrag = true;
    } else if (&view == circularStyleBtn_) {
        slider_->SetKnobWidth(0);
        slider_->SetBackgroundStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
        slider_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, BACKGROUND_OPA);
        slider_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        slider_->SetForegroundStyle(STYLE_LINE_CAP, CapType::CAP_ROUND);
        slider_->SetForegroundStyle(STYLE_BACKGROUND_COLOR,
            Color::GetColorFromRGB(FOREGROUND_COLOR_R, FOREGROUND_COLOR_G, FOREGROUND_COLOR_B).full);
    }
    return true;
}
} // namespace OHOS
