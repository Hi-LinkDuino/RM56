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

#include "ui_test_draw_line.h"
#include "common/screen.h"
#include "components/ui_canvas.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"

namespace OHOS {
namespace {
const uint16_t DELTA_X = 24;
}

void UITestDrawLine::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestDrawLine::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestDrawLine::GetTestView()
{
    UIKit_Draw_Line_Test_Line_001();
    UIKit_Draw_Line_Test_Line_002();
    UIKit_Draw_Line_Test_Line_003();
    UIKit_Draw_Line_Test_Line_004();
    UIKit_Draw_Line_Test_Line_005();
    UIKit_Draw_Line_Test_Line_006();
    UIKit_Draw_Line_Test_Line_007();
    UIKit_Draw_Line_Test_Line_008();
    UIKit_Draw_Line_Test_Line_009();
    UIKit_Draw_Line_Test_Line_010();
    UIKit_Draw_Line_Test_Line_011();
    UIKit_Draw_Line_Test_Line_012();
    return container_;
}

UIViewGroup* UITestDrawLine::CreateTestCaseGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    // 288: group width; 250: group height
    group->Resize(288, 250);
    return group;
}

UILabel* UITestDrawLine::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    // 2: half of screen width;
    label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE, Screen::GetInstance().GetWidth() / 2,
                       TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

UICanvas* UITestDrawLine::CreateCanvas() const
{
    UICanvas* canvas = new UICanvas();
    canvas->SetHeight(200); // 200 : height size
    canvas->SetWidth(200);  // 200 : width size
    canvas->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    canvas->SetStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    return canvas;
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("水平直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({50, 50}, {150, 50}, paint); // 50 : start x 50 : start y 150 : end x 50 : end y
    group->Add(canvas);

    container_->Add(group);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_002");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("垂直直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({100, 50}, {100, 150}, paint); // 100 : start x 50 : start y 100 : end x 150 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_001", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_003");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("左下-右上方向直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({20, 180}, {160, 20}, paint); // 20 : start x 180 : start y 160 : end x 20 : end y
    canvas->DrawLine({20, 180}, {180, 20}, paint); // 20 : start x 180 : start y 180 : end x 20 : end y
    canvas->DrawLine({20, 180}, {180, 40}, paint); // 20 : start x 180 : start y 180 : end x 40 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_002", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_004()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_004");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("左上-右下方向直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({20, 20}, {160, 180}, paint); // 20 : start x 20 : start y 160 : end x 180 : end y
    canvas->DrawLine({20, 20}, {180, 180}, paint); // 20 : start x 20 : start y 180 : end x 180 : end y
    canvas->DrawLine({20, 20}, {180, 160}, paint); // 20 : start x 20 : start y 180 : end x 160 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_001", 10); // 10 : offset size
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_005()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_005");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("右上-左下方向直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({180, 20}, {20, 160}, paint); // 180 : start x 20 : start y 20 : end x 160 : end y
    canvas->DrawLine({180, 20}, {20, 180}, paint); // 180 : start x 20 : start y 20 : end x 180 : end y
    canvas->DrawLine({180, 20}, {40, 180}, paint); // 180 : start x 20 : start y 40 : end x 180 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_002", 10); // 10: offset
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_004", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_006()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_006");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("左上-右下方向直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    canvas->DrawLine({180, 180}, {20, 40}, paint); // 180 : start x 180 : start y 20 : end x 40 : end y
    canvas->DrawLine({180, 180}, {20, 20}, paint); // 180 : start x 180 : start y 20 : end x 20 : end y
    canvas->DrawLine({180, 180}, {40, 20}, paint); // 180 : start x 180 : start y 40 : end x 20 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_003", 10); // 10: offset
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_005", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_007()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_007");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("宽度为零直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeWidth(0);                       // 0 : stroke width
    canvas->DrawLine({10, 190}, {190, 10}, paint); // 10 : start x 190 : start y 190 : end x 10 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_004", 10); // 10 : offset size
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_008()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_008");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("不同宽度的直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeWidth(2);                       // 2 : stroke width
    canvas->DrawLine({10, 190}, {190, 10}, paint); // 10 : start x 190 : start y 190 : end x 10 : end y
    Paint paint2;
    paint2.SetStrokeWidth(4);                       // 4 : stroke width
    canvas->DrawLine({30, 190}, {190, 30}, paint2); // 30 : start x 190 : start y 190 : end x 30 : end y
    Paint paint3;
    paint3.SetStrokeWidth(5);                       // 5 : stroke width
    canvas->DrawLine({50, 190}, {190, 50}, paint3); // 50 : start x 190 : start y 190 : end x 50 : end y
    Paint paint4;
    paint4.SetStrokeWidth(90);                    // 90 : stroke width
    canvas->DrawLine({40, 90}, {90, 40}, paint4); // 40 : start x 90 : start y 90 : end x 40 : end y
    group->Add(canvas);
    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_005", 10); // 10: offset
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_007", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_009()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_009");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("全透明的直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetOpacity(OPA_TRANSPARENT);
    canvas->DrawLine({10, 190}, {190, 10}, paint); // 10 : start x 190 : start y 190 : end x 10 : end y
    group->Add(canvas);
    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_006", 10); // 10: offset
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_008", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_010()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_010");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("不透明的直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetOpacity(OPA_OPAQUE);
    canvas->DrawLine({10, 190}, {190, 10}, paint); // 10 : start x 190 : start y 190 : end x 10 : end y
    group->Add(canvas);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_007", 10); // 10 : offset size
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_011()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_011");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("不同透明度的直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetOpacity(51);                          // 51 : opacity size
    canvas->DrawLine({10, 190}, {190, 10}, paint); // 10 : start x 190 : start y 190 : end x 10 : end y
    Paint paint2;
    paint2.SetOpacity(127);                         // 127 : opacity size
    canvas->DrawLine({30, 190}, {190, 30}, paint2); // 30 : start x 190 : start y 190 : end x 30 : end y
    Paint paint3;
    paint3.SetOpacity(178);                         // 178 : opacity size
    canvas->DrawLine({50, 190}, {190, 50}, paint3); // 50 : start x 190 : start y 190 : end x 50 : end y
    group->Add(canvas);
    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_008", 10); // 10 : offset size
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_010", DELTA_X);
}

void UITestDrawLine::UIKit_Draw_Line_Test_Line_012()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_Draw_Line_Test_Line_012");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    group->Add(label);
    label->SetText("不同颜色的直线绘制 ");

    UICanvas* canvas = CreateCanvas();
    Paint paint;
    paint.SetStrokeColor(Color::Red());
    canvas->DrawLine({10, 190}, {190, 10}, paint); // 10 : start x 190 : start y 190 : end x 10 : end y
    Paint paint2;
    paint2.SetStrokeColor(Color::Yellow());
    canvas->DrawLine({30, 190}, {190, 30}, paint2); // 30 : start x 190 : start y 190 : end x 30 : end y
    Paint paint3;
    paint3.SetStrokeColor(Color::Green());
    canvas->DrawLine({50, 190}, {190, 50}, paint3); // 50 : start x 190 : start y 190 : end x 50 : end y
    group->Add(canvas);
    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_Draw_Line_Test_Line_009", 10); // 10 : offset size
    group->LayoutRightToSibling("UIKit_Draw_Line_Test_Line_011", DELTA_X);
}
} // namespace OHOS