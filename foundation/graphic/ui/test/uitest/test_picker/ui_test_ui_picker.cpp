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

#include "ui_test_ui_picker.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

#include <string>

namespace OHOS {
namespace {
static int16_t g_blank = 38;
static int16_t g_ListW = 200;
static int16_t g_ListH = 300;
static const char* g_pickerRange[] = {"A0", "B1", "C2", "D3", "E4", "F5", "G6", "H7", "I8", "J9", "K10", "L11"};
} // namespace

void UITestUIPicker::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestUIPicker::TearDown()
{
    container_->Remove(setLoopBtn_);
    container_->Remove(setLoopOffBtn_);
    container_->Remove(setSelectBtn_);
    container_->Remove(setLeftToRightBtn_);
    container_->Remove(setRightToLeftBtn_);
    container_->Remove(setMarginBtn_);
    container_->Remove(selectIndex_);
    container_->Remove(selectTime_);
    container_->Remove(picker1_);
    container_->Remove(picker2_);
    container_->Remove(picker3_);
    DeleteChildren(container_);
    setLoopBtn_ = nullptr;
    setLoopOffBtn_ = nullptr;
    setSelectBtn_ = nullptr;
    setLeftToRightBtn_ = nullptr;
    setRightToLeftBtn_ = nullptr;
    setMarginBtn_ = nullptr;
    selectIndex_ = nullptr;
    selectTime_ = nullptr;
    container_ = nullptr;
    picker1_ = nullptr;
    picker2_ = nullptr;
    picker3_ = nullptr;

    lastX_ = 0;
    lastY_ = 0;
}

const UIView* UITestUIPicker::GetTestView()
{
    UIKit_Picker_Test_Base_001();
    UIKit_Picker_Test_Base_002();
    UIKit_Picker_Test_Time_Picker_001();
    return container_;
}

void UITestUIPicker::OnPickerStoped(UIPicker& picker)
{
    uint16_t index = picker.GetSelected();
    selectIndex_->SetText(std::to_string(index).c_str());
    selectIndex_->Invalidate();
}

void UITestUIPicker::OnTimePickerStoped(UITimePicker& picker)
{
    selectTime_->SetText(picker.GetSelectValue());
    selectTime_->Invalidate();
}

void UITestUIPicker::UIKit_Picker_Test_Base_001()
{
    if (container_ == nullptr) {
        return;
    }
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    UILabel* label = GetTitleLabel("UIPicker动态字体  ");
    container_->Add(label);
    label->SetPosition(positionX_, 0);
    if (picker1_ == nullptr) {
        picker1_ = new UIPicker();
    }
    picker1_->SetPosition(positionX_, label->GetY() + g_blank, g_ListW, g_ListH);
    picker1_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    picker1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    picker1_->SetFontId(16, 18); // 16:back font id 18:high light font id
    picker1_->SetItemHeight(50); // 50: height
    picker1_->SetTextColor(Color::Gray(), Color::Red());
    picker1_->SetDirect(UITextLanguageDirect::TEXT_DIRECT_RTL);
    picker1_->SetValues(g_pickerRange, sizeof(g_pickerRange) / sizeof(g_pickerRange[0]));
    picker1_->SetSelected(3); // 3 : default selected value
    picker1_->RegisterSelectedListener(this);

    positionX_ = picker1_->GetWidth();
    positionY_ = picker1_->GetY();
    CreatButtons();
    if (selectIndex_ == nullptr) {
        selectIndex_ = new UILabel();
    }
    selectIndex_ = GetTitleLabel("NULL");
    selectIndex_->SetPosition(positionX_ + 40, label->GetY() + g_blank); // 40: increase x-coordinate
    positionY_ += label->GetY() + g_blank;
    container_->Add(selectIndex_);
    SetUpButton(setLoopBtn_, "开启循环 ");
    SetUpButton(setLoopOffBtn_, "关闭循环 ");
    SetUpButton(setSelectBtn_, "定位到第5个 ");
    SetUpButton(setLeftToRightBtn_, "从左往右 ");
    SetUpButton(setRightToLeftBtn_, "从右往左 ");
    container_->Add(picker1_);
    SetLastPos(picker1_);
}

void UITestUIPicker::CreatButtons()
{
    if (setLoopBtn_ == nullptr) {
        setLoopBtn_ = new UILabelButton();
    }
    if (setLoopOffBtn_ == nullptr) {
        setLoopOffBtn_ = new UILabelButton();
    }
    if (setSelectBtn_ == nullptr) {
        setSelectBtn_ = new UILabelButton();
    }
    if (setLeftToRightBtn_ == nullptr) {
        setLeftToRightBtn_ = new UILabelButton();
    }
    if (setRightToLeftBtn_ == nullptr) {
        setRightToLeftBtn_ = new UILabelButton();
    }
    if (setMarginBtn_ == nullptr) {
        setMarginBtn_ = new UILabelButton();
    }
}

void UITestUIPicker::UIKit_Picker_Test_Base_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIPicker区间数字  ");
    container_->Add(label);
    label->SetPosition(390, 0); // 390 x-coordinate
    if (picker3_ == nullptr) {
        picker3_ = new UIPicker();
    }
    picker3_->SetPosition(390, g_blank, g_ListW, g_ListH); // 390: x-coordinate
    picker3_->SetFontId(16, 18);                           // 16:back font id 18:high light font id
    picker3_->SetItemHeight(50);                           // 50: height
    picker3_->SetTextColor(Color::Gray(), Color::Red());
    picker3_->SetValues(-5, 20); // -5: start 20:end
    picker3_->SetTextFormatter(new TextFormatter());

    positionX_ = picker3_->GetWidth();
    positionY_ = picker3_->GetY();

    container_->Add(picker3_);
    SetLastPos(picker3_);
}

void UITestUIPicker::UIKit_Picker_Test_Time_Picker_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("Time Picker");
    container_->Add(label);
    label->SetPosition(672, 0); // 672: x-coordinate
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);

    container_->Add(setMarginBtn_);
    setMarginBtn_->SetPosition(672, 30, 120, 30); // 672: x-coordinate, 30 : y-coordinate, 120 : width, 30 : height
    setMarginBtn_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    setMarginBtn_->SetOnClickListener(this);
    setMarginBtn_->SetText("增加margin");

    if (picker2_ == nullptr) {
        picker2_ = new UITimePicker();
    }
    picker2_->EnableSecond(true);
    picker2_->SetPosition(652, setMarginBtn_->GetY() + 40, g_ListW, g_ListH); // 652: x-coordinate, 40: offset
    picker2_->SetItemHeight(50);                                           // 50 height
    picker2_->RegisterSelectedListener(this);
    picker2_->SetSelected("12:20:30");
    positionX_ = picker2_->GetWidth();
    positionY_ = picker2_->GetY();

    if (selectTime_ == nullptr) {
        selectTime_ = new UILabel();
    }
    selectTime_ = GetTitleLabel(picker2_->GetSelectValue());
    picker2_->RegisterSelectedListener(this);
    selectTime_->SetPosition(662 + picker2_->GetWidth(), label->GetY() + g_blank); // 662: increase x-coordinate
    container_->Add(selectTime_);
    container_->Add(picker2_);
    SetLastPos(picker2_);
}

bool UITestUIPicker::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == setLoopBtn_) {
        picker1_->SetLoopState(true);
    } else if (&view == setLoopOffBtn_) {
        picker1_->SetLoopState(false);
    } else if (&view == setSelectBtn_) {
        picker1_->SetSelected(5); // 5: index
    } else if (&view == setLeftToRightBtn_) {
        picker1_->SetDirect(UITextLanguageDirect::TEXT_DIRECT_LTR);
    } else if (&view == setRightToLeftBtn_) {
        picker1_->SetDirect(UITextLanguageDirect::TEXT_DIRECT_RTL);
    } else if (&view == setMarginBtn_) {
        picker2_->Invalidate();
        picker2_->SetStyle(STYLE_MARGIN_LEFT, ++margin_);
        picker2_->SetStyle(STYLE_MARGIN_RIGHT, margin_);
        picker2_->SetStyle(STYLE_MARGIN_TOP, margin_);
        picker2_->SetStyle(STYLE_MARGIN_BOTTOM, margin_);
        picker2_->SetWidth(++g_ListW);
        picker2_->Invalidate();
    }
    return true;
}

void UITestUIPicker::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(positionX_ + 30, positionY_, BUTTON_WIDHT2, BUTTON_HEIGHT2); // 30: increase x-coordinate;
    positionY_ += btn->GetHeight() + 12;                                          // 12: increase y-coordinate
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    container_->Invalidate();
}

void UITestUIPicker::SetLastPos(UIView* view)
{
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS
