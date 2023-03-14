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

#include "ui_test_view_scale_rotate.h"
#include "common/screen.h"
#include "components/ui_label.h"
#include "core/render_manager.h"
#include "test_resource_config.h"

namespace OHOS {
void UITestViewScaleRotate::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
    }

    if (list_ == nullptr) {
        list_ = new ListLayout(UISwipeView::VERTICAL);
        uint16_t width = Screen::GetInstance().GetWidth();
        uint16_t height = Screen::GetInstance().GetHeight();
        list_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, 0, width, height + 1);
        container_->Add(list_);
    }
}

void UITestViewScaleRotate::TearDown()
{
    animator_.Stop();
    DeleteChildren(container_);
    container_ = nullptr;
    list_ = nullptr;
}

const UIView* UITestViewScaleRotate::GetTestView()
{
    UIKit_View_Scale_Rotate_Test_Label_001();
    UIKit_View_Scale_Rotate_Test_LabelButton_002();
    UIKit_View_Scale_Rotate_Test_Canvas_003();
    UIKit_View_Scale_Rotate_Test_UIBoxProgress_004();
    UIKit_View_Scale_Rotate_Test_UICircleProgress_005();
    UIKit_View_Scale_Rotate_Test_UIDigitalClock_006();
    UIKit_View_Scale_Rotate_Test_Group_007();

    animator_.Start();
    return container_;
}

void UITestViewScaleRotate::Callback(UIView* view)
{
    angleValue_++;

    if (scaleValue_.x_ < 0.2f) {
        scaleStep_ = 0.01f;
    } else if (scaleValue_.x_ > 1.5f) {
        scaleStep_ = -0.01f;
    }
    scaleValue_.x_ += scaleStep_;
    scaleValue_.y_ += scaleStep_;

    button1_->Rotate(angleValue_, VIEW_CENTER);
    button1_->Scale(scaleValue_, VIEW_CENTER);
    group1_->Rotate(angleValue_, GROUP_CENTER);
    group1_->Scale(scaleValue_, GROUP_CENTER);
    canvas_->Rotate(angleValue_, GROUP_CENTER);
    canvas_->Scale(scaleValue_, GROUP_CENTER);
    boxProgress_->Rotate(angleValue_, VIEW_CENTER);
    boxProgress_->Scale(scaleValue_, VIEW_CENTER);
    circleProgress_->Rotate(angleValue_, VIEW_CENTER);
    circleProgress_->Scale(scaleValue_, VIEW_CENTER);
    dClock_->Rotate(angleValue_, VIEW_CENTER);
    dClock_->Scale(scaleValue_, VIEW_CENTER);
    label_->Rotate(angleValue_, VIEW_CENTER);
    label_->Scale(scaleValue_, VIEW_CENTER);
}

void UITestViewScaleRotate::SetUpLabel(const char* title) const
{
    UILabel* label = new UILabel();
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE,
        Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText(title);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    list_->Add(label);
}

void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_Label_001()
{
    SetUpLabel("UILabel组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    label_ = new UILabel();
    label_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    label_->SetText("Test UILabel");
    label_->SetPosition(100, 100, 200, 50); // 100: x 100: y 200: width 50: height
    group->Add(label_);
    list_->Add(group);
}

void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_LabelButton_002()
{
    SetUpLabel("UILabelButton组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    list_->Add(group);
    button1_ = new UILabelButton();
    button1_->SetPosition(100, 100, 200, 50); // 100: x 100: y 200: width 50: height
    button1_->SetText("Test UILabelButton");
    group->Add(button1_);
}

void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_Canvas_003()
{
    SetUpLabel("UICanvas组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    list_->Add(group);
    canvas_ = new UICanvas();
    group->Add(canvas_);

    canvas_->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    canvas_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);

    Paint paint;
    paint.SetStrokeWidth(20); // 20: StrokeWidth
    canvas_->DrawLine({150, 150}, {300, 300}, paint); // 150: x 150: y 300: x 300: y

    paint.SetStrokeColor(Color::Red());
    canvas_->DrawCurve({100, 50}, {150, 50}, {150, 50}, {150, 100}, paint);
    canvas_->DrawRect({10, 10}, 50, 50, paint); // 10: x 10: y 50: width 50: height
}

void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_UIBoxProgress_004()
{
    SetUpLabel("UIBoxProgress组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    list_->Add(group);

    boxProgress_ = new UIBoxProgress();
    boxProgress_->EnableBackground(true);
    boxProgress_->SetPosition(100, 100, 200, 50); // 100: x 100:y 200:width 50: height
    boxProgress_->SetValue(80); // 80: value
    boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    boxProgress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    group->Add(boxProgress_);
}

void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_UICircleProgress_005()
{
    SetUpLabel("UICircleProgress组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    list_->Add(group);
    circleProgress_ = new UICircleProgress();
    circleProgress_->SetPosition(10, 10, 150, 150); // 10: x 10: y 150: width 150: height
    circleProgress_->SetCenterPosition(75, 75);     // 75: position x 75: position y
    circleProgress_->SetRadius(50);                 // 50: radius
    circleProgress_->SetValue(20);                  // 20: value
    circleProgress_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    circleProgress_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    group->Add(circleProgress_);
}
void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_UIDigitalClock_006()
{
    SetUpLabel("UIDigitalClock组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    list_->Add(group);
    dClock_ = new UIDigitalClock();
    dClock_->SetPosition(10, 10, 150, 150);             // 10: x 10:y 150: width 150: height
    dClock_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, 30); // 30: font size
    dClock_->SetTime24Hour(7, 25, 50);                  // 7: hour, 25: minute, 50: second
    group->Add(dClock_);
}

void UITestViewScaleRotate::UIKit_View_Scale_Rotate_Test_Group_007()
{
    SetUpLabel("UIViewGroup组件旋转 ");
    UIViewGroup* group = new UIViewGroup();
    group->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    list_->Add(group);

    group1_ = new UIViewGroup();
    group1_->SetPosition(0, 0, GROUP_WIDHT, GROUP_HEIGHT);
    group1_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    group1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    group->Add(group1_);

    UISlider* slider = new UISlider();
    slider->SetPosition(10, 10, 200, 50); // 10: x 10: y 200: width 50: height
    slider->SetValue(50); // 50: value
    slider->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
    group1_->Add(slider);

    UILabelButton* button = new UILabelButton();
    button->SetPosition(10, 100, 200, 50); // 10: x 100:y 200: width 50: height
    button->SetText("test ViewGroup");
    group1_->Add(button);
}
} // namespace OHOS
