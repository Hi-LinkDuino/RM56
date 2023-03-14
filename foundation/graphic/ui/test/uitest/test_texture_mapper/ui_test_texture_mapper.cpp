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

#include "ui_test_texture_mapper.h"
#include "common/screen.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
static int16_t g_rotateStart = 0;      // 0: the start angle of rotation
static int16_t g_rotateEnd = 360;      // 360: the end angle of rotation
static float g_scaleStart = 1;         // 1: the start ratio
static float g_scaleEnd = 0.5;         // 0.5: the end ratio
static uint16_t g_durationTime = 2000; // 2000: the duration for this animator
static uint16_t g_delayTime = 0;
static EasingFunc g_easingFunc = EasingEquation::CubicEaseInOut;
static Point g_pivot = {58, 58}; // 58: the x-coordinate,; 58: the y-coordinate
} // namespace

void UITestTextureMapper::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        UIViewGroup* uiViewGroup = new UIViewGroup();
        uiViewGroup->SetPosition(0, 0, 320, 390); // 320: width; 390: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 288: x-coordinate; 48: y-coordinate
        label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, 288, 48);
        label->SetText("UITextureMapper效果");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        UIViewGroup* uiViewGroupFrame = new UIViewGroup();
        uiViewGroup->Add(uiViewGroupFrame);
        // 288: x-coordinate; 336: y-coordinate
        uiViewGroupFrame->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE, 288, 336);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
        uiViewGroupFrame->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
        uiViewGroupFrame->SetStyle(STYLE_BACKGROUND_OPA, 0);
        textureMapper_ = new UITextureMapper();
        textureMapper_->SetPosition(200, 50, 200, 200); // 200:position x; 50: position y; 200: width; 200: height
        textureMapper_->SetSrc(RED_IMAGE_PATH);
        textureMapper_->SetRotateStart(0);
        textureMapper_->SetRotateEnd(360);     // 360: the end angle of rotation
        textureMapper_->SetScaleStart(1);      // 1: the start ratio
        textureMapper_->SetScaleEnd(0.5);      // 0.5: the end ratio
        textureMapper_->SetPivot(58, 58);      // 58: the x-coordinate,; 58: the y-coordinate
        textureMapper_->SetDurationTime(2000); // 2000: the duration for this animator
        listener_ = new TestTextureMapperStopListener(uiViewGroupFrame);
        uiViewGroupFrame->Add(textureMapper_);
        textureMapper_->LayoutCenterOfParent();
    }
    if (layout_ == nullptr) {
        layout_ = new GridLayout();
        layout_->SetPosition(336, 48, 516, 232); // 336: x-coordinate; 48: y-coordinate; 516: y value; 232: height
        container_->Add(layout_);
        layout_->SetLayoutDirection(LAYOUT_HOR);
        layout_->SetRows(4); // 4: the number of rows
        layout_->SetCols(4); // 4: the number of columns
    }
    resetBtn_ = new UILabelButton();
    SetUpButton(resetBtn_, "重置");
}

void UITestTextureMapper::TearDown()
{
    delete listener_;
    listener_ = nullptr;
    DeleteChildren(container_);
    container_ = nullptr;
    textureMapper_ = nullptr;
    layout_ = nullptr;
}

const UIView* UITestTextureMapper::GetTestView()
{
    UIKit_TextureMapper_Test_SetRotateEnd_001();
    UIKit_TextureMapper_Test_SetRotateStart_002();
    UIKit_TextureMapper_Test_SetScaleEnd_003();
    UIKit_TextureMapper_Test_SetScaleStart_004();
    UIKit_TextureMapper_Test_SetPivot_005();
    UIKit_TextureMapper_Test_Start_006();
    UIKit_TextureMapper_Test_Reset_007();
    UIKit_TextureMapper_Test_CancelAnimation_008();
    UIKit_TextureMapper_Test_SetAnimatorStopListener_009();

    layout_->LayoutChildren();
    return container_;
}

void UITestTextureMapper::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    layout_->Add(btn);
    btn->Resize(BUTTON_WIDHT2, BUTTON_HEIGHT2);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}

void UITestTextureMapper::UIKit_TextureMapper_Test_SetRotateEnd_001()
{
    incRotateEndBtn_ = new UILabelButton();
    SetUpButton(incRotateEndBtn_, "旋转结束角度+");

    decRotateEndBtn_ = new UILabelButton();
    SetUpButton(decRotateEndBtn_, "旋转结束角度-");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_SetRotateStart_002()
{
    startAngleBtn_ = new UILabelButton();
    SetUpButton(startAngleBtn_, "旋转起始角度+");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_SetScaleEnd_003()
{
    incScaleEndBtn_ = new UILabelButton();
    SetUpButton(incScaleEndBtn_, "缩放结束比例+");

    decScaleEndBtn_ = new UILabelButton();
    SetUpButton(decScaleEndBtn_, "缩放结束比例-");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_SetScaleStart_004()
{
    startScaleBtn_ = new UILabelButton();
    SetUpButton(startScaleBtn_, "缩放起始比例+");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_SetPivot_005()
{
    incPivotBtn_ = new UILabelButton();
    SetUpButton(incPivotBtn_, "中心+");

    decPivotBtn_ = new UILabelButton();
    SetUpButton(decPivotBtn_, "中心-");

    durationTimeBtn_ = new UILabelButton();
    SetUpButton(durationTimeBtn_, "持续时间+");

    delayTimeBtn_ = new UILabelButton();
    SetUpButton(delayTimeBtn_, "延时时间+");

    easingBtn_ = new UILabelButton();
    SetUpButton(easingBtn_, "动效");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_Start_006()
{
    startBtn_ = new UILabelButton();
    SetUpButton(startBtn_, "启动动画");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_Reset_007()
{
    resetImageBtn_ = new UILabelButton();
    SetUpButton(resetImageBtn_, "显示原始图片");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_CancelAnimation_008()
{
    cancelBtn_ = new UILabelButton();
    SetUpButton(cancelBtn_, "取消动画 ");
}

void UITestTextureMapper::UIKit_TextureMapper_Test_SetAnimatorStopListener_009()
{
    listenerBtn_ = new UILabelButton();
    SetUpButton(listenerBtn_, "动画结束监听");
}

bool UITestTextureMapper::OnClick(UIView& view, const ClickEvent& event)
{
    g_rotateStart = 0;     // 0: the start angle of rotation
    g_rotateEnd = 360;     // 360: the end angle of rotation
    g_scaleStart = 1;      // 1: the start ratio
    g_scaleEnd = 0.5;      // 0.5: the end ratio
    g_durationTime = 2000; // 2000: the duration for this animator
    g_delayTime = 0;
    g_easingFunc = EasingEquation::CubicEaseInOut;
    if (&view == resetBtn_) {
        g_rotateStart = 0; // 0: the start angle of rotation
        g_rotateEnd = 360; // 360: the end angle of rotation
        g_scaleStart = 1;  // 1: the start ratio
        g_scaleEnd = 0.5;  // 0.5: the end ratio

        g_durationTime = 2000; // 2000: the duration for this animator
        g_delayTime = 0;
        g_easingFunc = EasingEquation::CubicEaseInOut;
        g_pivot = {58, 58}; // 58: the x-coordinate,; 58: the y-coordinate
        textureMapper_->SetRotateStart(g_rotateStart);
        textureMapper_->SetScaleStart(g_scaleStart);
        textureMapper_->SetPivot(g_pivot.x, g_pivot.y);
        textureMapper_->Reset();
    } else if (&view == incRotateEndBtn_) {
        g_rotateEnd += 90; // 90: the end angle of rotation augmentation
        textureMapper_->SetRotateEnd(g_rotateEnd);
    } else if (&view == decRotateEndBtn_) {
        g_rotateEnd -= 90; // 90: the end angle of rotation decrease
        textureMapper_->SetRotateEnd(g_rotateEnd);
    } else if (&view == durationTimeBtn_) {
        g_durationTime += 200; // 200: the duration for this animator augmentation
        textureMapper_->SetDurationTime(g_durationTime);
    } else if (&view == delayTimeBtn_) {
        g_delayTime += 200; // 200: the delay for this animator augmentation
        textureMapper_->SetDelayTime(g_delayTime);
    } else if (&view == easingBtn_) {
        g_easingFunc = EasingEquation::SineEaseIn;
        textureMapper_->SetEasingFunc(g_easingFunc);
    } else if (&view == startAngleBtn_) {
        g_rotateStart += 90; // 90: the start angle of rotation augmentation
        textureMapper_->SetRotateStart(g_rotateStart);
    } else {
        ExpandClick(view, event);
    }
    return true;
}

bool UITestTextureMapper::ExpandClick(UIView& view, const ClickEvent& event)
{
    if (&view == incScaleEndBtn_) {
        g_scaleEnd += 0.2; // 0.2: the end ratio augmentation
        textureMapper_->SetScaleEnd(g_scaleEnd);
    } else if (&view == decScaleEndBtn_) {
        g_scaleEnd -= 0.2; // 0.2: the end ratio decrease
        textureMapper_->SetScaleEnd(g_scaleEnd);
    } else if (&view == startScaleBtn_) {
        g_scaleStart += 0.2; // 0.2: the start ratio augmentation
        textureMapper_->SetScaleStart(g_scaleStart);
    } else if (&view == incPivotBtn_) {
        g_pivot.x += 5; // 5: pivot offset pixel
        g_pivot.y += 5; // 5: pivot offset pixel
        textureMapper_->SetPivot(g_pivot.x, g_pivot.y);
    } else if (&view == decPivotBtn_) {
        g_pivot.x -= 5; // 5: pivot offset pixel
        g_pivot.y -= 5; // 5: pivot offset pixel
        textureMapper_->SetPivot(g_pivot.x, g_pivot.y);
    } else if (&view == startBtn_) {
        textureMapper_->Start();
    } else if (&view == resetImageBtn_) {
        textureMapper_->Reset();
    } else if (&view == cancelBtn_) {
        textureMapper_->Cancel();
    } else if (&view == listenerBtn_) {
        textureMapper_->SetAnimatorStopListener(listener_);
    }
    return true;
}
} // namespace OHOS
