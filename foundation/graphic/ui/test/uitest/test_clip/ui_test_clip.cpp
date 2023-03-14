/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "ui_test_clip.h"

#include <cmath>
#include "common/image.h"
#include "draw/clip_utils.h"
#include "gfx_utils/graphic_math.h"
#include "securec.h"
#include "test_resource_config.h"

namespace OHOS {
namespace {
static float g_radius1 = 40.f;
static int16_t g_startAngle = 0;
static int16_t g_endAngle = 50;
}
void UITestClip::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }
    positionY_ = 0;
}

void UITestClip::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

UIView* UITestClip::GetTestView()
{
    UIKitClipTest001();
    UIKitClipTest002();
    UIKitClipTest003();
    UIKitClipTest004();
    UIKitClipTest005();

    return container_;
}

bool UITestClip::OnClick(UIView& view, const ClickEvent& event)
{
    bool caseChange1 = false;
    bool caseChange2 = false;
    if (&view == btnRadiusInc1_) {
        g_radius1 += RADIUS_STEP_1;
        caseChange1 = true;
    } else if (&view == btnRadiusInc5_) {
        g_radius1 += RADIUS_STEP_5;
        caseChange1 = true;
    } else if (&view == btnRadiusDec1_) {
        g_radius1 -= RADIUS_STEP_1;
        caseChange1 = true;
    } else if (&view == btnRadiusDec5_) {
        g_radius1 -= RADIUS_STEP_5;
        caseChange1 = true;
    } else if (&view == btnStartAngleInc_) {
        g_startAngle += ANGLE_STEP;
        caseChange2 = true;
    } else if (&view == btnStartAngleDec_) {
        g_startAngle -= ANGLE_STEP;
        caseChange2 = true;
    } else if (&view == btnEndAngleInc_) {
        g_endAngle += ANGLE_STEP;
        caseChange2 = true;
    } else if (&view == btnEndAngleDec_) {
        g_endAngle -= ANGLE_STEP;
        caseChange2 = true;
    }
    if (caseChange1) {
        char buffer[BUFFER_SIZE];
        if (sprintf_s(buffer, BUFFER_SIZE, "当前半径 = %.0f", g_radius1) < 0) {
            return false;
        }
        radiusText_->SetText(buffer);
        ClipPath path;
        // {70, 50}: center of circle
        path.Circle({70, 50}, g_radius1);
        imageView1_->SetSrc(JPEG_IMAGE_PATH);
        ClipImage(imageView1_, path);
        imageView1_->Invalidate();
    }
    if (caseChange2) {
        ClipPath path;
        // {80, 80}: center; 50: radius
        path.MoveTo({80, 80}).Arc({80, 80}, 50, g_startAngle, g_endAngle);
        imageView2_->SetSrc(JPEG_IMAGE_PATH);
        ClipImage(imageView2_, path);
        imageView2_->Invalidate();
    }

    return true;
}

void UITestClip::CreateTitleLabel(const char* title)
{
    UILabel* titleLabel = new UILabel();
    titleLabel->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    titleLabel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    titleLabel->SetText(title);
    container_->Add(titleLabel);
    positionY_ += TITLE_HEIGHT + 8; // 8: gap
}

UIImageView* UITestClip::CreateImageView()
{
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->SetHeight(BLOCK_HEIGHT);
    viewGroup->SetWidth(BLOCK_WIDTH);
    viewGroup->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_);
    viewGroup->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    container_->Add(viewGroup);
    positionY_ += BLOCK_HEIGHT + GAP;

    UIImageView* imageVIew = new UIImageView();
    imageVIew->SetPosition(IMAGE_POSITION_X, IMAGE_POSITION_Y);
    imageVIew->SetSrc(JPEG_IMAGE_PATH);
    viewGroup->Add(imageVIew);
    return imageVIew;
}

void UITestClip::SetUpButton(UILabelButton* btn, const char* title, int16_t x, int16_t y)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
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
}

void UITestClip::ClipImage(UIImageView* imageView, ClipPath& path)
{
    const ImageInfo* info = imageView->GetImageInfo();
    ClipUtils clip;
    clip.PerformScan(path, info);
}

void UITestClip::UIKitClipTest001()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("圆形裁剪 ");
    int16_t x = VIEW_DISTANCE_TO_LEFT_SIDE + BLOCK_WIDTH + GAP;
    int16_t y = positionY_;
    btnRadiusInc1_ = new UILabelButton();
    SetUpButton(btnRadiusInc1_, "半径+1", x, y);
    btnRadiusDec1_ = new UILabelButton();
    SetUpButton(btnRadiusDec1_, "半径-1", x + BUTTON_WIDHT2 + GAP, y);
    btnRadiusInc5_ = new UILabelButton();
    y += BUTTON_HEIGHT2 + GAP;
    SetUpButton(btnRadiusInc5_, "半径+5", x, y);
    btnRadiusDec5_ = new UILabelButton();
    SetUpButton(btnRadiusDec5_, "半径-5", x + BUTTON_WIDHT2 + GAP, y);
    y += BUTTON_HEIGHT2 + GAP;

    radiusText_ = new UILabel();
    radiusText_->SetPosition(x, y, Screen::GetInstance().GetWidth(), TITLE_HEIGHT);
    radiusText_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    char buffer[BUFFER_SIZE];
    if (sprintf_s(buffer, BUFFER_SIZE, "当前半径 = %.0f", g_radius1) >= 0) {
        radiusText_->SetText(buffer);
    }
    container_->Add(radiusText_);

    ClipPath path;
    // {70, 50}: center of circle
    path.Circle({70, 50}, g_radius1);

    imageView1_ = CreateImageView();
    ClipImage(imageView1_, path);
}

void UITestClip::UIKitClipTest002()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("弧形裁剪 ");
    int16_t x = VIEW_DISTANCE_TO_LEFT_SIDE + BLOCK_WIDTH + GAP;
    int16_t y = positionY_;
    btnStartAngleInc_ = new UILabelButton();
    SetUpButton(btnStartAngleInc_, "起始角度+", x, y);
    btnStartAngleDec_ = new UILabelButton();
    SetUpButton(btnStartAngleDec_, "起始角度-", x + BUTTON_WIDHT2 + GAP, y);
    btnEndAngleInc_ = new UILabelButton();
    y += BUTTON_HEIGHT2 + GAP;
    SetUpButton(btnEndAngleInc_, "结束角度+", x, y);
    btnEndAngleDec_ = new UILabelButton();
    SetUpButton(btnEndAngleDec_, "结束角度-", x + BUTTON_WIDHT2 + GAP, y);
    y += BUTTON_HEIGHT2 + GAP;

    ClipPath path;
    // {80, 80}: center; 50: radius
    path.MoveTo({80, 80}).Arc({80, 80}, 50, g_startAngle, g_endAngle);

    imageView2_ = CreateImageView();
    ClipImage(imageView2_, path);
}

void UITestClip::UIKitClipTest003()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("多边形裁剪----五角星 ");

    int32_t rot = 0;
    // 80: Radius of outer circle
    int32_t outerR = 80;
    // 40: Radius of inner circle
    int32_t innerR = 40;
    // 50: The x-coordinate of the starting point
    int32_t x = 50;
    // 50: The y-coordinate of the starting point
    int32_t y = 50;
    // 180.0: The angle corresponding to PI
    float pi = 180.0;
    ClipPath path;
    // 5: Needs to calculate five vertices
    for (int32_t i = 0; i < 5; i++) {
        // 18: constant 72: constant;
        path.LineTo({static_cast<float>(cos((18 + 72 * i - rot) / pi * UI_PI) * outerR + x),
            static_cast<float>(-sin((18 + 72 * i - rot) / pi * UI_PI) * outerR + y)}); // 18: constant 72: constant;
        // 54: constant 72: constant;
        path.LineTo({static_cast<float>(cos((54 + 72 * i - rot) / pi * UI_PI) * innerR + x),
            static_cast<float>(-sin((54 + 72 * i - rot) / pi * UI_PI) * innerR + y)}); // 54: constant 72: constant;
    }

    UIImageView* imageView = CreateImageView();
    ClipImage(imageView, path);
}

void UITestClip::UIKitClipTest004()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("贝塞尔曲线裁剪 ");

    ClipPath path;
    // {50, 50}, {100, 50}, {100, 100}, {50, 100}: path points; {60, 110}, {80, 10}: control points of curve
    path.MoveTo({50, 50}).CurveTo({60, 110}, {80, 10}, {100, 50}).LineTo({100, 100}).LineTo({50, 100});

    UIImageView* imageView = CreateImageView();
    ClipImage(imageView, path);
}

void UITestClip::UIKitClipTest005()
{
    if (container_ == nullptr) {
        return;
    }
    CreateTitleLabel("多边形裁剪 ");

    ClipPath path;
    // {20, 70}, {50, 60}, {110, 80}, {110, 130}, {50, 100}, {20, 120}: path points
    path.MoveTo({20, 70}).LineTo({50, 60}).LineTo({110, 80}).LineTo({110, 130}).LineTo({50, 100}).LineTo({20, 120});

    UIImageView* imageView = CreateImageView();
    ClipImage(imageView, path);
}
};
