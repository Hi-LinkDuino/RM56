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

#include "ui_test_box_progress.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
const int16_t LABEL_HEIGHT = 29;
const int16_t BUTTON_WIDTH = 120;
const int16_t BUTTON_HEIGHT = 40;
const int16_t DELTA_X_COORDINATE = 8;
const int16_t DELTA_X_COORDINATE_2 = 336;
const int16_t DELTA_Y_COORDINATE = 12;
const int16_t DELTA_Y_COORDINATE_2 = 19;
} // namespace

void UITestBoxProgress::SetUp()
{
    if (container_ == nullptr) {
        positionX_ = 0;
        positionY_ = 0;
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(positionX_, positionY_, 320, 390); // 320: width; 390: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: width; 48: height
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("UIBoxProgress效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        uiViewGroupFrame_ = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame_);
        // 288: width; 336: height
        uiViewGroupFrame_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_LEFT_SIDE, 288, 336);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame_->SetStyle(STYLE_BACKGROUND_OPA, 0);

        boxProgress_ = new UIBoxProgress();
        boxProgress_->SetPosition(10, 10, 100, 10); // 10: x 10: y 100: width 10: height
        boxProgress_->SetValue(20);                 // 20: value
        boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        boxProgress_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
        boxProgress_->SetStyle(STYLE_BORDER_COLOR, Color::Yellow().full);
        uiViewGroupFrame_->Add(boxProgress_);
        boxProgress_->LayoutCenterOfParent();
    }

    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        scroll_->SetPosition(336, 11);                           // 336: x-coordinate; 11: y-coordinate
        // 624: width; 100: Adjust the visible height of scroll
        scroll_->Resize(624, Screen::GetInstance().GetHeight() - 100);
        container_->Add(scroll_);
        positionY_ = 0;
    }
}

void UITestBoxProgress::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    boxProgress_ = nullptr;
    scroll_ = nullptr;
    uiViewGroupFrame_ = nullptr;
}

const UIView* UITestBoxProgress::GetTestView()
{
    UIKit_BoxProgress_Test_UIBoxProgress_001();
    UIKit_BoxProgress_Test_SetRange_002();
    UIKit_BoxProgress_Test_SetValue_003();
    UIKit_BoxProgress_Test_SetImage_004();
    UIKit_BoxProgress_Test_SetStyle_005();
    UIKit_BoxProgress_Test_GetStyle_006();
    UIKit_BoxProgress_Test_SetCapType_007();
    UIKit_BoxProgress_Test_SetDirection_008();
    UIKit_BoxProgress_Test_EnableBackground_009();
    UIKit_BoxProgress_Test_SetStep_010();
    UIKit_BoxProgress_Test_SetValidSize_011();
    UIKit_BoxProgress_Test_SetBorderAndPadding_012();

    return container_;
}

void UITestBoxProgress::SetUpLabel(const char* title, int16_t x, int16_t y) const
{
    UILabel* label = new UILabel();
    scroll_->Add(label);
    label->SetPosition(x, y, 288, 30); // 288: width; 30: height
    label->SetText(title);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
}
void UITestBoxProgress::SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y)
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

void UITestBoxProgress::UIKit_BoxProgress_Test_UIBoxProgress_001()
{
    resetBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ = 0;
    SetUpLabel("创建条形进度条：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(resetBtn_, "重置", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetRange_002()
{
    incMinProgressBtn_ = new UILabelButton();
    decMinProgressBtn_ = new UILabelButton();
    incMaxProgressBtn_ = new UILabelButton();
    decMaxProgressBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条范围：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incMinProgressBtn_, "下限+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decMinProgressBtn_, "下限-", positionX_, positionY_);
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(incMaxProgressBtn_, "上限+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decMaxProgressBtn_, "上限-", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetValue_003()
{
    incProgressBtn_ = new UILabelButton();
    decProgressBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条当前值：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incProgressBtn_, "进度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decProgressBtn_, "进度-", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetImage_004()
{
    imageBtn_ = new UILabelButton();
    noImageBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条图片：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(imageBtn_, "设置图片", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(noImageBtn_, "取消图片", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetStyle_005()
{
    setStyleBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(setStyleBtn_, "设置样式", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_GetStyle_006()
{
    getStyleBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(getStyleBtn_, "设置样式", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetCapType_007()
{
    roundCapBtn_ = new UILabelButton();
    noneCapBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ = 0;
    SetUpLabel("设置条形进度条端点：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(roundCapBtn_, "圆形端点", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(noneCapBtn_, "方形端点", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetDirection_008()
{
    dirL2RBtn_ = new UILabelButton();
    dirR2LBtn_ = new UILabelButton();
    dirT2BBtn_ = new UILabelButton();
    dirB2TBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条方向：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(dirL2RBtn_, "从左到右", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(dirR2LBtn_, "从右到左", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(dirT2BBtn_, "从上到下", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(dirB2TBtn_, "从下到上", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_EnableBackground_009()
{
    enableBgBtn_ = new UILabelButton();
    disableBgBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条背景：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(enableBgBtn_, "设置背景", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(disableBgBtn_, "隐藏背景", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetStep_010()
{
    stepBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条步长：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(stepBtn_, "步长+", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetValidSize_011()
{
    incWidthBtn_ = new UILabelButton();
    incHeightBtn_ = new UILabelButton();
    decWidthBtn_ = new UILabelButton();
    decHeightBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置条形进度条有效区域宽高：", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += LABEL_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpButton(incHeightBtn_, "box高度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE_2;
    SetUpButton(incWidthBtn_, "box宽度+", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += LABEL_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpButton(decHeightBtn_, "box高度-", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE_2;
    SetUpButton(decWidthBtn_, "box宽度-", positionX_, positionY_);
}

void UITestBoxProgress::UIKit_BoxProgress_Test_SetBorderAndPadding_012()
{
    borderIncBtn_ = new UILabelButton();
    borderDecBtn_ = new UILabelButton();
    paddingLeftIncBtn_ = new UILabelButton();
    paddingRightIncBtn_ = new UILabelButton();
    paddingTopIncBtn_ = new UILabelButton();
    paddingBottomIncBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置边宽和内边距:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(borderIncBtn_, "边宽+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE_2;
    SetUpButton(borderDecBtn_, "边宽-", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpButton(paddingLeftIncBtn_, "左内边距+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE_2;
    SetUpButton(paddingRightIncBtn_, "右内边距+", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpButton(paddingTopIncBtn_, "上内边距+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE_2;
    SetUpButton(paddingBottomIncBtn_, "下内边距+", positionX_, positionY_);
}

bool UITestBoxProgress::OnClick(UIView& view, const ClickEvent& event)
{
    static uint16_t progress = 20; // 20: progress
    static uint16_t step = 0;
    static uint16_t width = 100; // 100: width
    static uint16_t height = 10; // 10: height
    static int16_t min = 0;
    static int16_t max = 100; // 100: max value
    if (&view == resetBtn_) {
        boxProgress_->SetPosition(10, 10, 100, 10); // 10: x 10: y 100: width 15: height
        progress = 20;                              // 20: progress
        step = 0;
        width = 100; // 100: width
        height = 10; // 10: height
        min = 0;
        max = 100; // 100: max value
        boxProgress_->SetDirection(UIBoxProgress::Direction::DIR_LEFT_TO_RIGHT);
        boxProgress_->SetValidHeight(height);
        boxProgress_->SetValidWidth(width);
        boxProgress_->SetRange(max, min);
        boxProgress_->SetValue(progress);
        boxProgress_->SetStep(step);
        // 182, 191: color
        boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(0, 182, 191).full);
        // 255, 152: color
        boxProgress_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::GetColorFromRGB(255, 152, 0).full);
        boxProgress_->SetImage(static_cast<char*>(nullptr));
        boxProgress_->EnableBackground(true);
        boxProgress_->LayoutCenterOfParent();
    } else if (&view == incProgressBtn_) {
        progress++;
        boxProgress_->SetValue(progress);
    } else if (&view == decProgressBtn_) {
        progress--;
        boxProgress_->SetValue(progress);
    } else if (&view == stepBtn_) {
        step++;
        boxProgress_->SetStep(step);
    } else if (&view == incWidthBtn_) {
        width++;
        boxProgress_->SetValidWidth(width);
    } else if (&view == incHeightBtn_) {
        height++;
        boxProgress_->SetValidHeight(height);
    } else if (&view == decWidthBtn_) {
        width--;
        boxProgress_->SetValidWidth(width);
    } else if (&view == decHeightBtn_) {
        height--;
        boxProgress_->SetValidHeight(height);
    } else if (&view == incMinProgressBtn_) {
        min++;
        boxProgress_->SetRange(boxProgress_->GetRangeMax(), min);
    } else if (&view == decMinProgressBtn_) {
        min--;
        boxProgress_->SetRange(boxProgress_->GetRangeMax(), min);
    } else if (&view == incMaxProgressBtn_) {
        max++;
        boxProgress_->SetRange(max, boxProgress_->GetRangeMin());
    } else if (&view == decMaxProgressBtn_) {
        max--;
        boxProgress_->SetRange(max, boxProgress_->GetRangeMin());
    } else if (&view == dirL2RBtn_) {
        width = 100; // 100: width
        height = 10; // 10: height
        boxProgress_->Resize(width, height);
        boxProgress_->SetDirection(UIBoxProgress::Direction::DIR_LEFT_TO_RIGHT);
    } else if (&view == dirR2LBtn_) {
        width = 100; // 100: width
        height = 10; // 10: height
        boxProgress_->Resize(width, height);
        boxProgress_->SetDirection(UIBoxProgress::Direction::DIR_RIGHT_TO_LEFT);
    } else if (&view == dirT2BBtn_) {
        width = 10;   // 10: width
        height = 100; // 100: height
        boxProgress_->Resize(width, height);
        boxProgress_->SetDirection(UIBoxProgress::Direction::DIR_TOP_TO_BOTTOM);
    } else if (&view == dirB2TBtn_) {
        width = 10;   // 10: width
        height = 100; // 100: height
        boxProgress_->Resize(width, height);
        boxProgress_->SetDirection(UIBoxProgress::Direction::DIR_BOTTOM_TO_TOP);
    } else if (&view == imageBtn_) {
        boxProgress_->SetImage(RED_IMAGE_PATH, BLUE_IMAGE_PATH);
    } else if (&view == noImageBtn_) {
        boxProgress_->SetImage(static_cast<ImageInfo*>(nullptr));
    } else if (&view == setStyleBtn_) {
        Style style = StyleDefault::GetDefaultStyle();
        style.bgColor_ = Color::White();
        boxProgress_->SetBackgroundStyle(style);
        style.bgColor_ = Color::Blue();
        boxProgress_->SetForegroundStyle(style);
    } else if (&view == getStyleBtn_) {
        boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, 128); // 128: opa
        boxProgress_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    } else if (&view == roundCapBtn_) {
        boxProgress_->SetCapType(CapType::CAP_ROUND);
    } else if (&view == noneCapBtn_) {
        boxProgress_->SetCapType(CapType::CAP_NONE);
    } else if (&view == enableBgBtn_) {
        boxProgress_->EnableBackground(true);
    } else if (&view == disableBgBtn_) {
        boxProgress_->EnableBackground(false);
    } else if (&view == borderIncBtn_) {
        int64_t bodrderWidth = boxProgress_->GetStyle(STYLE_BORDER_WIDTH);
        // 20: max width; 5: step
        bodrderWidth = (bodrderWidth + 5 > 20) ? 20 : (bodrderWidth + 5);
        boxProgress_->SetStyle(STYLE_BORDER_WIDTH, bodrderWidth);
    } else if (&view == borderDecBtn_) {
        int64_t bodrderWidth = boxProgress_->GetStyle(STYLE_BORDER_WIDTH);
        // 0: min width; 5: step
        bodrderWidth = (bodrderWidth - 5 < 0) ? 0 : (bodrderWidth - 5);
        boxProgress_->SetStyle(STYLE_BORDER_WIDTH, bodrderWidth);
    } else if (&view == paddingLeftIncBtn_) {
        int64_t paddingWidth = boxProgress_->GetStyle(STYLE_PADDING_LEFT);
        // 20: max width; 5: step
        paddingWidth = (paddingWidth + 5 > 20) ? 20 : (paddingWidth + 5);
        boxProgress_->SetStyle(STYLE_PADDING_LEFT, paddingWidth);
    } else if (&view == paddingRightIncBtn_) {
        int64_t paddingWidth = boxProgress_->GetStyle(STYLE_PADDING_RIGHT);
        // 20: max width; 5: step
        paddingWidth = (paddingWidth + 5 > 20) ? 20 : (paddingWidth + 5);
        boxProgress_->SetStyle(STYLE_PADDING_RIGHT, paddingWidth);
    } else if (&view == paddingTopIncBtn_) {
        int64_t paddingWidth = boxProgress_->GetStyle(STYLE_PADDING_TOP);
        // 20: max width; 5: step
        paddingWidth = (paddingWidth + 5 > 20) ? 20 : (paddingWidth + 5);
        boxProgress_->SetStyle(STYLE_PADDING_TOP, paddingWidth);
    } else if (&view == paddingBottomIncBtn_) {
        int64_t paddingWidth = boxProgress_->GetStyle(STYLE_PADDING_BOTTOM);
        // 20: max width; 5: step
        paddingWidth = (paddingWidth + 5 > 20) ? 20 : (paddingWidth + 5);
        boxProgress_->SetStyle(STYLE_PADDING_BOTTOM, paddingWidth);
    }
    container_->Invalidate();
    return true;
}
} // namespace OHOS
