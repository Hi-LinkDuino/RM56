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

#include "ui_test_circle_progress.h"
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

void UITestCircleProgress::SetUp()
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
        // 288: x-coordinate; 48: y-coordinate
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("UICircleProgress效果");
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

        circleProgress_ = new UICircleProgress();
        circleProgress_->SetPosition(10, 10, 150, 150);    // 10: x 10:y 150:width 150: height
        circleProgress_->SetCenterPosition(75, 75);        // 75: position x 75: position y
        circleProgress_->SetRadius(50);                    // 50: radius
        circleProgress_->SetValue(20);                     // 20: value
        circleProgress_->SetStyle(STYLE_PADDING_LEFT, 10); // 10: value
        circleProgress_->SetStyle(STYLE_PADDING_TOP, 5);   // 5: value
        uiViewGroupFrame_->Add(circleProgress_);
        circleProgress_->LayoutCenterOfParent();
    }

    if (scroll_ == nullptr) {
        scroll_ = new UIScrollView();
        scroll_->SetPosition(336, 11);                      // 336: x-coordinate; 11: y-coordinate
        scroll_->Resize(624, container_->GetHeight() - 11); // 624: width; // 11: decrease height
        container_->Add(scroll_);
        positionY_ = 0;
    }
}

void UITestCircleProgress::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    circleProgress_ = nullptr;
    scroll_ = nullptr;
}

const UIView* UITestCircleProgress::GetTestView()
{
    UIKit_CircleProgress_Test_UICircleProgress_001();
    UIKit_CircleProgress_Test_SetValue_002();
    UIKit_CircleProgress_Test_SetImage_003();
    UIKit_CircleProgress_Test_SetStyle_004();
    UIKit_CircleProgress_Test_GetStyle_005();
    UIKit_CircleProgress_Test_SetCapType_006();
    UIKit_CircleProgress_Test_SetStep_007();
    UIKit_CircleProgress_Test_SetRange_008();
    UIKit_CircleProgress_Test_EnableBackground_009();
    UIKit_CircleProgress_Test_SetCenter_010();
    UIKit_CircleProgress_Test_SetRadius_011();
    UIKit_CircleProgress_Test_SetLineWidth_012();
    UIKit_CircleProgress_Test_SetImagePosition_013();
    UIKit_CircleProgress_Test_SetLineColor_014();
    UIKit_CircleProgress_Test_SetAngle_015();

    return container_;
}

void UITestCircleProgress::SetUpLabel(const char* title, int16_t x, int16_t y) const
{
    UILabel* label = new UILabel();
    scroll_->Add(label);
    label->SetPosition(x, y, 288, 30); // 288: width; 30: height
    label->SetText(title);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
}
void UITestCircleProgress::SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y)
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

void UITestCircleProgress::UIKit_CircleProgress_Test_UICircleProgress_001()
{
    resetBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ = 0;
    SetUpLabel("创建环形进度条：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(resetBtn_, "重置", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetRange_008()
{
    incMinProgressBtn_ = new UILabelButton();
    decMinProgressBtn_ = new UILabelButton();
    incMaxProgressBtn_ = new UILabelButton();
    decMaxProgressBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条范围：", positionX_, positionY_);
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

void UITestCircleProgress::UIKit_CircleProgress_Test_SetValue_002()
{
    incProgressBtn_ = new UILabelButton();
    decProgressBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条当前值：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incProgressBtn_, "进度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decProgressBtn_, "进度-", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetImage_003()
{
    imageBtn_ = new UILabelButton();
    noImageBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条图片：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(imageBtn_, "设置图片", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(noImageBtn_, "取消图片", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetStyle_004()
{
    setStyleBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(setStyleBtn_, "设置样式", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_GetStyle_005()
{
    getStyleBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条样式:", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(getStyleBtn_, "设置样式", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetCapType_006()
{
    roundCapBtn_ = new UILabelButton();
    noneCapBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条端点：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(roundCapBtn_, "圆形端点", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(noneCapBtn_, "方形端点", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetStep_007()
{
    stepBtn_ = new UILabelButton();
    positionX_ = VIEW_DISTANCE_TO_LEFT_SIDE2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条步长：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(stepBtn_, "步长+", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_EnableBackground_009()
{
    enableBgBtn_ = new UILabelButton();
    disableBgBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ = 0;
    SetUpLabel("环形进度条隐藏背景：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(enableBgBtn_, "设置背景", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(disableBgBtn_, "隐藏背景", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetCenter_010()
{
    centerXBtn_ = new UILabelButton();
    centerYBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条圆心：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(centerXBtn_, "圆心X+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(centerYBtn_, "圆心Y+", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetRadius_011()
{
    incRadiusBtn_ = new UILabelButton();
    decRadiusBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条半径：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incRadiusBtn_, "半径+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decRadiusBtn_, "半径-", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetLineWidth_012()
{
    incWidthBtn_ = new UILabelButton();
    decWidthBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条线宽：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incWidthBtn_, "线宽+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decWidthBtn_, "线宽-", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetImagePosition_013()
{
    imgPosXBtn_ = new UILabelButton();
    imgPosYBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条图片位置：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(imgPosXBtn_, "图片X+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(imgPosYBtn_, "图片Y+", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetLineColor_014()
{
    lineColorBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条前景线条颜色： ", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(lineColorBtn_, "前景绿色", positionX_, positionY_);
}

void UITestCircleProgress::UIKit_CircleProgress_Test_SetAngle_015()
{
    incStartAngleBtn_ = new UILabelButton();
    decStartAngleBtn_ = new UILabelButton();
    incEndAngleBtn_ = new UILabelButton();
    decEndAngleBtn_ = new UILabelButton();
    swapAngleBtn_ = new UILabelButton();
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE_2;
    SetUpLabel("设置环形进度条角度：", positionX_, positionY_);
    positionY_ += LABEL_HEIGHT + DELTA_X_COORDINATE;
    SetUpButton(incStartAngleBtn_, "起始角度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decStartAngleBtn_, "起始角度-", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(incEndAngleBtn_, "结束角度+", positionX_, positionY_);
    positionX_ += BUTTON_WIDTH + DELTA_Y_COORDINATE;
    SetUpButton(decEndAngleBtn_, "结束角度-", positionX_, positionY_);
    positionX_ = DELTA_X_COORDINATE_2;
    positionY_ += BUTTON_HEIGHT + DELTA_Y_COORDINATE;
    SetUpButton(swapAngleBtn_, "转换方向", positionX_, positionY_);
}

bool UITestCircleProgress::OnClick(UIView& view, const ClickEvent& event)
{
    static uint16_t progress = 20; // 20: progress
    static uint16_t step = 0;
    static uint16_t width = 150;  // 150: width
    static uint16_t height = 150; // 150: height
    static int16_t min = 0;
    static int16_t max = 100;       // 100: max value
    static Point center = {75, 75}; // 75: position x 75: position y
    static Point imgPos = {0, 0};
    static int16_t startAngle = 0;
    static int16_t endAngle = 360; // 360: end angle
    static int16_t radius = 50;    // 50: radius

    if (&view == resetBtn_) {
        circleProgress_->SetPosition(10, 10, 150, 150); // 10: x 10: y 150: width 150: height
        progress = 100;                                 // 100 progress
        step = 0;
        width = 150;  // 150: width
        height = 150; // 150: height
        min = 0;
        max = 100;         // 100: max value
        center = {75, 75}; // 75: position x 75: position y
        startAngle = 0;
        endAngle = 360; // 360: end angle
        radius = 50;    // 50: radius
        imgPos = {0, 0};

        circleProgress_->SetRange(max, min);
        circleProgress_->SetValue(progress);
        circleProgress_->SetCenterPosition(center.x, center.y);
        circleProgress_->SetRadius(radius);
        circleProgress_->SetStartAngle(startAngle);
        circleProgress_->SetEndAngle(endAngle);
        circleProgress_->SetStep(step);
        circleProgress_->SetBackgroundStyle(StyleDefault::GetProgressBackgroundStyle());
        circleProgress_->SetForegroundStyle(StyleDefault::GetProgressForegroundStyle());
        circleProgress_->SetLineColor(Color::Red());
        circleProgress_->SetForegroundStyle(STYLE_LINE_WIDTH, 40); // 40: value
        circleProgress_->SetForegroundStyle(STYLE_LINE_OPA, 127);  // 127: value
        circleProgress_->SetBackgroundImagePosition(imgPos.x, imgPos.y);
        circleProgress_->SetProgressImagePosition(imgPos.x, imgPos.y);
        circleProgress_->SetImage(static_cast<ImageInfo*>(nullptr));
        circleProgress_->EnableBackground(false);
        circleProgress_->LayoutCenterOfParent();
    } else if (&view == incProgressBtn_) {
        progress++;
        circleProgress_->SetValue(progress);
    } else if (&view == decProgressBtn_) {
        progress--;
        circleProgress_->SetValue(progress);
    } else if (&view == stepBtn_) {
        step++;
        circleProgress_->SetStep(step);
    } else if (&view == incMinProgressBtn_) {
        min++;
        circleProgress_->SetRange(circleProgress_->GetRangeMax(), min);
    } else if (&view == decMinProgressBtn_) {
        min--;
        circleProgress_->SetRange(circleProgress_->GetRangeMax(), min);
    } else if (&view == incMaxProgressBtn_) {
        max++;
        circleProgress_->SetRange(max, circleProgress_->GetRangeMin());
    } else if (&view == decMaxProgressBtn_) {
        max--;
        circleProgress_->SetRange(max, circleProgress_->GetRangeMin());
    } else if (&view == imageBtn_) {
        circleProgress_->SetImage(RED_IMAGE_PATH, BLUE_IMAGE_PATH);
    } else if (&view == noImageBtn_) {
        circleProgress_->SetImage(static_cast<char*>(nullptr));
    } else if (&view == setStyleBtn_) {
        Style style = StyleDefault::GetDefaultStyle();
        style.lineColor_ = Color::White();
        circleProgress_->SetBackgroundStyle(style);
        style.lineColor_ = Color::Blue();
        circleProgress_->SetForegroundStyle(style);
    } else if (&view == getStyleBtn_) {
        circleProgress_->SetBackgroundStyle(STYLE_LINE_COLOR, Color::Red().full);
        circleProgress_->SetForegroundStyle(STYLE_LINE_COLOR, Color::Yellow().full);
    } else if (&view == roundCapBtn_) {
        circleProgress_->SetCapType(CapType::CAP_ROUND);
    } else if (&view == noneCapBtn_) {
        circleProgress_->SetCapType(CapType::CAP_NONE);
    } else if (&view == enableBgBtn_) {
        circleProgress_->EnableBackground(true);
    } else if (&view == disableBgBtn_) {
        circleProgress_->EnableBackground(false);
    } else if (&view == incStartAngleBtn_) {
        startAngle++;
        circleProgress_->SetStartAngle(startAngle);
    } else if (&view == decStartAngleBtn_) {
        startAngle--;
        circleProgress_->SetStartAngle(startAngle);
    } else if (&view == incEndAngleBtn_) {
        endAngle++;
        circleProgress_->SetEndAngle(endAngle);
    } else if (&view == decEndAngleBtn_) {
        endAngle--;
        circleProgress_->SetEndAngle(endAngle);
    } else if (&view == swapAngleBtn_) {
        int16_t angle = startAngle;
        startAngle = endAngle;
        endAngle = angle;
        circleProgress_->SetStartAngle(startAngle);
        circleProgress_->SetEndAngle(endAngle);
    } else if (&view == centerXBtn_) {
        center.x++;
        circleProgress_->SetCenterPosition(center.x, circleProgress_->GetCenterPosition().y);
    } else if (&view == centerYBtn_) {
        center.y++;
        circleProgress_->SetCenterPosition(circleProgress_->GetCenterPosition().x, center.y);
    } else if (&view == incRadiusBtn_) {
        radius++;
        circleProgress_->SetRadius(radius);
    } else if (&view == decRadiusBtn_) {
        radius--;
        circleProgress_->SetRadius(radius);
    } else if (&view == incWidthBtn_) {
        circleProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH,
                                            circleProgress_->GetBackgroundStyle(STYLE_LINE_WIDTH) + 1);
        circleProgress_->SetForegroundStyle(STYLE_LINE_WIDTH,
                                            circleProgress_->GetForegroundStyle(STYLE_LINE_WIDTH) + 1);
    } else if (&view == decWidthBtn_) {
        circleProgress_->SetBackgroundStyle(STYLE_LINE_WIDTH,
                                            circleProgress_->GetBackgroundStyle(STYLE_LINE_WIDTH) - 1);
        circleProgress_->SetForegroundStyle(STYLE_LINE_WIDTH,
                                            circleProgress_->GetForegroundStyle(STYLE_LINE_WIDTH) - 1);
    } else if (&view == imgPosXBtn_) {
        imgPos.x++;
        circleProgress_->SetBackgroundImagePosition(imgPos.x, imgPos.y);
        circleProgress_->SetProgressImagePosition(imgPos.x, imgPos.y);
    } else if (&view == imgPosYBtn_) {
        imgPos.y++;
        circleProgress_->SetBackgroundImagePosition(imgPos.x, imgPos.y);
        circleProgress_->SetProgressImagePosition(imgPos.x, imgPos.y);
    } else if (&view == lineColorBtn_) {
        circleProgress_->SetLineColor(Color::Green());
    }
    container_->Invalidate();
    return true;
}
} // namespace OHOS
