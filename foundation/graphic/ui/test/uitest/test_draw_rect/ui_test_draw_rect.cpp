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

#include "ui_test_draw_rect.h"
#include "common/screen.h"
#include "components/ui_button.h"
#include "components/ui_label.h"
#include "components/ui_view_group.h"

namespace OHOS {
void UITestDrawRect::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestDrawRect::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestDrawRect::GetTestView()
{
    UIKit_DrawRect_Test_RectNoBorder_001();
    UIKit_DrawRect_Test_RectNoBorder_002();
    UIKit_DrawRect_Test_RectNoBorder_003();
    UIKit_DrawRect_Test_RectNoBorder_004();
    UIKit_DrawRect_Test_RectNoBorder_005();
    UIKit_DrawRect_Test_RectNoBorder_006();
    UIKit_DrawRect_Test_RectNoBorder_007();
    UIKit_DrawRect_Test_RectNoBorder_008();
    UIKit_DrawRect_Test_RectNoBorder_009();
    UIKit_DrawRect_Test_RectNoBorder_0010();
    UIKit_DrawRect_Test_RectBorderNoRadius_001();
    UIKit_DrawRect_Test_RectBorderNoRadius_002();
    UIKit_DrawRect_Test_RectBorderRadius_001();
    UIKit_DrawRect_Test_RectBorderRadius_002();
    UIKit_DrawRect_Test_RectBorderRadius_003();
    UIKit_DrawRect_Test_RectBorderRadius_004();
    UIKit_DrawRect_Test_RectBorderRadius_005();
    UIKit_DrawRect_Test_RectBorderRadius_006();
    UIKit_DrawRect_Test_RectBorderRadius_007();
    UIKit_DrawRect_Test_RectBorderRadius_008();
    UIKit_DrawRect_Test_RectBorderRadius_009();
    UIKit_DrawRect_Test_RectBorderRadius_010();
    UIKit_DrawRect_Test_RectBorderRadius_011();

    UIKit_DrawRect_Test_Rect_001();
    UIKit_DrawRect_Test_RectColor_001();
    UIKit_DrawRect_Test_RectRadius_001();
    UIKit_DrawRect_Test_RectRadius_002();
    UIKit_DrawRect_Test_RectRadius_003();
    UIKit_DrawRect_Test_RectRadius_004();
    UIKit_DrawRect_Test_RectOpacity_001();
    UIKit_DrawRect_Test_RectOpacity_002();
    UIKit_DrawRect_Test_RectOpacity_003();
    UIKit_DrawRect_Test_RectBorderColor_001();
    UIKit_DrawRect_Test_RectBorderWidth_001();
    UIKit_DrawRect_Test_RectBorderWidth_002();
    UIKit_DrawRect_Test_RectBorderOpacity_001();
    UIKit_DrawRect_Test_RectBorderOpacity_002();
    UIKit_DrawRect_Test_RectBorderOpacity_003();
    UIKit_DrawRect_Test_RectPadding_001();
    UIKit_DrawRect_Test_RectPadding_002();
    UIKit_DrawRect_Test_RectMargin_001();
    UIKit_DrawRect_Test_RectMargin_002();
    UIKit_DrawRect_Test_RectPaddingMargin_001();
    UIKit_DrawRect_Test_RectPaddingMargin_002();
    UIKit_DrawRect_Test_RectWidthAndHeight_001();
    UIKit_DrawRect_Test_RectWidthAndHeight_002();
    UIKit_DrawRect_Test_RectWidthAndHeight_003();
    return container_;
}

UIViewGroup* UITestDrawRect::CreateTestCaseGroup() const
{
    UIViewGroup* group = new UIViewGroup();
    group->Resize(Screen::GetInstance().GetWidth(), 180); // 180 : value
    return group;
}

UILabel* UITestDrawRect::CreateTitleLabel() const
{
    UILabel* label = new UILabel();
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_DISTANCE_TO_TOP_SIDE,
        Screen::GetInstance().GetWidth() - VIEW_DISTANCE_TO_LEFT_SIDE, TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    return label;
}

UIView* UITestDrawRect::CreateUIView() const
{
    UIView* view = new UIView();
    view->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    view->SetStyle(STYLE_BACKGROUND_OPA, 127); // 127 : background opacity
    view->SetStyle(STYLE_BORDER_RADIUS, 5); // 5 : radius value
    view->SetStyle(STYLE_BORDER_COLOR, Color::Gray().full);
    view->SetStyle(STYLE_BORDER_OPA, 127); // 127 : border opacity
    view->SetStyle(STYLE_BORDER_WIDTH, 20); // 20 : border width
    view->SetStyle(STYLE_PADDING_LEFT, 0);
    view->SetStyle(STYLE_PADDING_RIGHT, 0);
    view->SetStyle(STYLE_PADDING_TOP, 0);
    view->SetStyle(STYLE_PADDING_BOTTOM, 0);
    view->SetStyle(STYLE_MARGIN_LEFT, 0);
    view->SetStyle(STYLE_MARGIN_RIGHT, 0);
    view->SetStyle(STYLE_MARGIN_TOP, 0);
    view->SetStyle(STYLE_MARGIN_BOTTOM, 0);
    return view;
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius=0");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, 0);
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_002");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width > height, radius > height/2");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 + 10); // 2 : half height 10 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_003");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width > height, radius = height/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width size
    int16_t height = 50; // 50 : height size
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2); // 2 : half height
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_004()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_004");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width > height, radius < height/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width size
    int16_t height = 50; // 50 : height size
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 - 10); // 2 : half height 10 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_003", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_005()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_005");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width = height, radius > height/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = width;
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 + 10); // 2 : half height 10 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_004", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_006()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_006");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width = height, radius = height/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = width;
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2); // 2 : half
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_005", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_007()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_007");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width = height, radius < height/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = width;
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 - 10); // 2 : half height 10 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_006", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_008()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_008");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width < height, radius > width/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 120; // 120 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + 10); // 2 : half width 10 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_007", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_009()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_009");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width < height, radius = width/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 51; // 51 : width
    int16_t height = 120; // 120 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2); // 2 : half width
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_008", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectNoBorder_0010()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectNoBorder_0010");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形无border, radius>0, width < height, radius < width/2 ");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 120; // 120 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 - 10); // 2 : half width 10 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_009", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderNoRadius_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderNoRadius_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形有border, radius=0");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 10); // 10 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 0);
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectNoBorder_0010", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderNoRadius_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderNoRadius_002");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形有border = 1, radius=0");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 1); // 1 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 0);
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderNoRadius_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形有border, radius<border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, borderWidth - 5); // 5 : math_num
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderNoRadius_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_002");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度 >高度，radius > 高度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 + borderWidth + 5); // 2 : half height 5 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_003");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度>高度，radius = 高度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 + borderWidth); // 2 : half height
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_004()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_004");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度>高度，radius < 高度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, height / 2 + borderWidth - 5); // 2 : half height 5 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_003", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_005()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_005");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度=高度，radius > 宽度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + borderWidth + 5); // 2 : half width 5 ： offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_004", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_006()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_006");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度=高度，radius = 宽度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + borderWidth); // 2 : half width
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_005", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_007()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_007");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度=高度，radius < 宽度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + borderWidth - 5); // 2 : half width 5 : offset
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_006", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_008()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_008");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度<高度，radius > 宽度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 100; // 100 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + borderWidth + 5); // 2 : half width 5 : math_num
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_007", 10); // 10 : value
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_009()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_009");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度<高度，radius = 宽度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 100; // 100 : height
    int16_t borderWidth = 10; // 10 : value
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + borderWidth); // 2 : math_num
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_008", 10); // 10 : value
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_010()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_010");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("border场景，radius>border.width，宽度<高度，radius < 宽度/2 + border.width");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 50; // 50 : width
    int16_t height = 100; // 100 : height
    int16_t borderWidth = 10;   // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, borderWidth);
    view->SetStyle(STYLE_BORDER_RADIUS, width / 2 + borderWidth - 5); // 2 : math_num 5 : math_num
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_009", 10); // 10 : value
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderRadius_011()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderRadius_011");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形有border = 2, radius = 1");
    group->Add(label);

    UIView* view = CreateUIView();
    int16_t width = 120; // 120 : width
    int16_t height = 50; // 50 : height
    int16_t borderWidth = 10; // 10 : borderWidth
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 2); // 2: borderWidth
    view->SetStyle(STYLE_BORDER_RADIUS, 1); // 1 : math_num
    view->Resize(width, height);
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_010", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_Rect_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_Rect_001");
    group->SetPosition(0, 0);

    UILabel* label = CreateTitleLabel();
    label->SetText("矩形绘制: ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 15); // 15 : value
    view->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view->SetStyle(STYLE_BORDER_RADIUS, 10); // 10 : offset size
    view->Resize(120, 50);   // 120 : width 50 : weight
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderRadius_009", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectColor_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectColor_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不同颜色的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 20); // 20 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 30); // 30 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : start x 50 : start y
    view1->SetStyle(STYLE_BORDER_WIDTH, 3); // 3 : value
    view1->SetStyle(STYLE_BORDER_RADIUS, 3); // 3 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_Rect_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectRadius_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectRadius_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置圆角为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 20); // 20 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 10); // 10 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectColor_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectRadius_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectRadius_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置圆角等于宽度的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 20); // 20 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 50); // 50 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectRadius_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectRadius_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectRadius_003");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置圆角等于宽度的正方形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 20); // 20 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 50); // 50 : value
    view->Resize(50, 50); // 50 : value 50 : value
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectRadius_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectRadius_004()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectRadius_004");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不同圆角大小的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 20); // 20 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 10); // 10 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : offset 50 : offset
    view1->SetStyle(STYLE_BORDER_WIDTH, 3); // 3 : value
    view1->SetStyle(STYLE_BORDER_RADIUS, 5); // 5 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectRadius_003", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectOpacity_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectOpacity_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不透明背景矩形绘制: ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectRadius_004", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectOpacity_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectOpacity_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置全透明背景矩形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectOpacity_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectOpacity_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectOpacity_003");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不同透明度矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BACKGROUND_OPA, 102); // 102 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : start x 50 : start y
    view1->SetStyle(STYLE_BACKGROUND_OPA, 204); // 204 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectOpacity_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderColor_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderColor_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不同边框颜色的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_COLOR, Color::Yellow().full);
    view->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value

    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : start x 50 : start y
    view1->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    view1->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectOpacity_003", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderWidth_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderWidth_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置边框宽度为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderColor_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderWidth_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderWidth_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不同宽度边框大小的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : offset 50 : offset
    view1->SetStyle(STYLE_BORDER_WIDTH, 10); // 10 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderWidth_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderOpacity_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderOpacity_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不透明边框矩形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value
    view->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderWidth_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderOpacity_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderOpacity_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置全透明边框矩形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value
    view->SetStyle(STYLE_BORDER_OPA, OPA_TRANSPARENT);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderOpacity_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectBorderOpacity_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectBorderOpacity_003");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置不同透明度边框矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value
    view->SetStyle(STYLE_BORDER_OPA, 153); // 153 : value
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : start x 50 : start y
    view1->SetStyle(STYLE_BORDER_WIDTH, 5); // 5 : value
    view1->SetStyle(STYLE_BORDER_OPA, 204); // 204 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderOpacity_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectPadding_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectPadding_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置padding为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_PADDING_LEFT, 0);
    view->SetStyle(STYLE_PADDING_RIGHT, 0);
    view->SetStyle(STYLE_PADDING_TOP, 0);
    view->SetStyle(STYLE_PADDING_BOTTOM, 0);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectBorderOpacity_003", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectPadding_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectPadding_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置padding不为零的矩形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(0, 50); // 0 : start x 50 : start y
    view->Resize(100, 50); // 100 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(120, 50); // 120 : start x 50 : start y
    view1->SetStyle(STYLE_PADDING_LEFT, 10); // 10 : value
    view1->SetStyle(STYLE_PADDING_RIGHT, 10); // 10 : value
    view1->SetStyle(STYLE_PADDING_TOP, 10); // 10 : value
    view1->SetStyle(STYLE_PADDING_BOTTOM, 10); // 10 : value
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    UIView* view2 = CreateUIView();
    view2->SetPosition(280, 50); // 280 : start x 50 : start y
    view2->SetStyle(STYLE_PADDING_LEFT, 20); // 20 : value
    view2->SetStyle(STYLE_PADDING_RIGHT, 10); // 10 : value
    view2->SetStyle(STYLE_PADDING_TOP, 20); // 20 : value
    view2->SetStyle(STYLE_PADDING_BOTTOM, 10); // 10 : value
    view2->Resize(120, 50); // 120 : width 50 : height
    group->Add(view2);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectPadding_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectMargin_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectMargin_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置margin为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_MARGIN_LEFT, 0);
    view->SetStyle(STYLE_MARGIN_RIGHT, 0);
    view->SetStyle(STYLE_MARGIN_TOP, 0);
    view->SetStyle(STYLE_MARGIN_BOTTOM, 0);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectPadding_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectMargin_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectMargin_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置margin不为零的矩形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_MARGIN_LEFT, 10); // 10 : margin left
    view->SetStyle(STYLE_MARGIN_RIGHT, 10); // 10 : margin right
    view->SetStyle(STYLE_MARGIN_TOP, 10); // 10 : margin top
    view->SetStyle(STYLE_MARGIN_BOTTOM, 10); // 10 : margin bottom
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : start x 50 : start y
    view1->SetStyle(STYLE_MARGIN_LEFT, 20); // 20 : margin left
    view1->SetStyle(STYLE_MARGIN_RIGHT, 10); // 10 : margin right
    view1->SetStyle(STYLE_MARGIN_TOP, 20); // 20 : margin top
    view1->SetStyle(STYLE_MARGIN_BOTTOM, 10); // 10 : margin bottom
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectMargin_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectPaddingMargin_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectPaddingMargin_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置 padding/margin 均为零的矩形绘制:");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_MARGIN_LEFT, 0);
    view->SetStyle(STYLE_MARGIN_RIGHT, 0);
    view->SetStyle(STYLE_MARGIN_TOP, 0);
    view->SetStyle(STYLE_MARGIN_BOTTOM, 0);
    view->SetStyle(STYLE_PADDING_LEFT, 0);
    view->SetStyle(STYLE_PADDING_RIGHT, 0);
    view->SetStyle(STYLE_PADDING_TOP, 0);
    view->SetStyle(STYLE_PADDING_BOTTOM, 0);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectMargin_002", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectPaddingMargin_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectPaddingMargin_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置padding/margin均不为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_MARGIN_LEFT, 0);
    view->SetStyle(STYLE_MARGIN_RIGHT, 0);
    view->SetStyle(STYLE_MARGIN_TOP, 0);
    view->SetStyle(STYLE_MARGIN_BOTTOM, 0);
    view->SetStyle(STYLE_PADDING_LEFT, 0);
    view->SetStyle(STYLE_PADDING_RIGHT, 0);
    view->SetStyle(STYLE_PADDING_TOP, 0);
    view->SetStyle(STYLE_PADDING_BOTTOM, 0);
    view->Resize(120, 50); // 120 : width 50 : height
    group->Add(view);

    UIView* view1 = CreateUIView();
    view1->SetPosition(250, 50); // 250 : start 50 : start
    view1->SetStyle(STYLE_MARGIN_LEFT, 10); // 10 : margin left
    view1->SetStyle(STYLE_MARGIN_RIGHT, 10); // 10 : margin right
    view1->SetStyle(STYLE_MARGIN_TOP, 10); // 10 : margin top
    view1->SetStyle(STYLE_MARGIN_BOTTOM, 10); // 10 : margin bottom
    view1->SetStyle(STYLE_PADDING_LEFT, 10); // 10 : padding left
    view1->SetStyle(STYLE_PADDING_RIGHT, 10); // 10 : padding right
    view1->SetStyle(STYLE_PADDING_TOP, 10); // 10 : padding top
    view1->SetStyle(STYLE_PADDING_BOTTOM, 10); // 10 : padding bottom
    view1->Resize(120, 50); // 120 : width 50 : height
    group->Add(view1);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectPaddingMargin_001", 10); // 10 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectWidthAndHeight_001()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectWidthAndHeight_001");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置宽度为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, 10); // 10 : border radius
    view->Resize(0, 50); // 0 : width 50 : height
    container_->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectPaddingMargin_002", 20); // 20 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectWidthAndHeight_002()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectWidthAndHeight_002");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置高度为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0); // 10 : value
    view->SetStyle(STYLE_BORDER_RADIUS, 0); // 10 : value
    view->Resize(120, 0); // 120 : width 0 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectWidthAndHeight_001", 20); // 20 : offset size
}

void UITestDrawRect::UIKit_DrawRect_Test_RectWidthAndHeight_003()
{
    if (container_ == nullptr) {
        return;
    }

    UIViewGroup* group = CreateTestCaseGroup();
    group->SetViewId("UIKit_DrawRect_Test_RectWidthAndHeight_003");

    UILabel* label = CreateTitleLabel();
    label->SetText("设置宽度和高度都为零的矩形绘制 ");
    group->Add(label);

    UIView* view = CreateUIView();
    view->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, VIEW_DISTANCE_TO_TOP_SIDE);
    view->SetStyle(STYLE_BORDER_WIDTH, 0);
    view->SetStyle(STYLE_BORDER_RADIUS, 10); // 10 : border radius
    view->Resize(0, 0);  // 0 : width 0 : height
    group->Add(view);

    container_->Add(group);
    group->LayoutBottomToSibling("UIKit_DrawRect_Test_RectWidthAndHeight_002", 20); // 20 : offset size
}
} // namespace OHOS
