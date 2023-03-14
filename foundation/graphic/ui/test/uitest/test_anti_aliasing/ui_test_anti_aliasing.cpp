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

#include "ui_test_anti_aliasing.h"
#include "common/screen.h"
#include "components/ui_canvas.h"
#include "components/ui_circle_progress.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"

namespace OHOS {
void UITestAntiAliasing::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestAntiAliasing::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestAntiAliasing::GetTestView()
{
    UIKit_Anti_Aliasing_Test_Rect_001();
    UIKit_Anti_Aliasing_Test_Circle_001();
    UIKit_Anti_Aliasing_Test_Line_001();
    return container_;
}

UIViewGroup* UITestAntiAliasing::CreateTestCaseGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth(), 200); // 200: group height
    return group;
}

UILabel* UITestAntiAliasing::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    // 2: half of screen width
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, 0, Screen::GetInstance().GetWidth() / 2, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

void UITestAntiAliasing::UIKit_Anti_Aliasing_Test_Rect_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    // 2: half of screen width, 110: group height
    group->Resize(Screen::GetInstance().GetWidth() / 2, 110);
    group->SetViewId("UIKit_Draw_Rect_Test_Rect_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形抗锯齿效果:");
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE);
    group->Add(label);

    UIView* view = new UIView();
    Style style = view->GetStyleConst();
    style.bgColor_ = Color::Green();
    style.bgOpa_ = OPA_OPAQUE;
    style.borderRadius_ = 5; // 5:border radius
    style.borderColor_ = Color::Gray();
    style.borderOpa_ = OPA_OPAQUE;
    style.borderWidth_ = 3; // 3:border width
    view->SetStyle(style);
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->Resize(120, 50); // 120:new width, 50:new width
    group->Add(view);

    container_->Add(group);
}

void UITestAntiAliasing::UIKit_Anti_Aliasing_Test_Circle_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    // 336: x-coordinate
    group->SetPosition(336, 0);
    // 2: half of screen width; 180: group height
    group->Resize(Screen::GetInstance().GetWidth() / 2, 180);
    group->SetViewId("UIKit_Anti_Aliasing_Test_Circle_001");

    UILabel* label = CreateTitleLabel();
    label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_TOP_SIDE);
    label->SetText("弧形/圆形抗锯齿效果:");
    group->Add(label);

    UICircleProgress* circleProgress = new UICircleProgress();
    circleProgress->SetPosition(0, 23, 150, 150); // 23:y-coordinate, 150:new width, 150:new height
    circleProgress->SetCenterPosition(75, 75); // 75:x-coordinate, 75:y-coordinate
    circleProgress->SetRadius(50); // 50:outer radius
    circleProgress->SetValue(20); // 20:current value of progress bar
    circleProgress->SetLineColor(Color::White());
    group->Add(circleProgress);

    UICircleProgress* circleProgress1 = new UICircleProgress();
    circleProgress1->SetPosition(130, 23, 150, 150); // 130:x-coordinate, 23:y-coordinate, 150:new width, 150:new height
    circleProgress1->SetCenterPosition(75, 75); // 75:x-coordinate, 75:y-coordinate
    circleProgress1->SetRadius(50); // 50:outer radius
    circleProgress1->SetValue(100); // 100:current value of progress bar
    circleProgress1->SetLineColor(Color::White());
    group->Add(circleProgress1);

    container_->Add(group);
}

void UITestAntiAliasing::UIKit_Anti_Aliasing_Test_Line_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    // 648: x-coordinate
    group->SetPosition(648, 0);
    group->Resize(Screen::GetInstance().GetWidth() / 2, 250); // 2: half of screen width; // 250: y-coordinate
    group->SetViewId("UIKit_Anti_Aliasing_Test_Line_001");

    UILabel* label = CreateTitleLabel();
    label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, TEXT_DISTANCE_TO_TOP_SIDE);
    label->SetText("直线抗锯齿效果:");
    group->Add(label);

    UICanvas* canvas = new UICanvas();
    canvas->SetHeight(200); // 200:height
    canvas->SetWidth(200); // 200:width
    canvas->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE2, VIEW_DISTANCE_TO_TOP_SIDE);
    canvas->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    Paint paint;
    // 0:start point x-coordinate, 50:start point y-coordinate, 150:end point x-coordinate, 150:end point y-coordinate
    canvas->DrawLine({0, 50}, {150, 50}, paint);
    // 0:start point x-coordinate, 50:start point y-coordinate, 150:end point x-coordinate, 150:end point y-coordinate
    canvas->DrawLine({0, 50}, {150, 100}, paint);
    // 0:start point x-coordinate, 50:start point y-coordinate, 150:end point x-coordinate, 150:end point y-coordinate
    canvas->DrawLine({0, 50}, {150, 200}, paint);
    // 0:start point x-coordinate, 50:start point y-coordinate, 150:end point x-coordinate, 150:end point y-coordinate
    canvas->DrawLine({0, 50}, {100, 200}, paint);
    group->Add(canvas);
    container_->Add(group);
}
} // namespace OHOS
