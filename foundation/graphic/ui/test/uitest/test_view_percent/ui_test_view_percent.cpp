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

#include "ui_test_view_percent.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "layout/grid_layout.h"

namespace OHOS {
namespace {
    static int16_t g_widthPercent = 100;
    const uint16_t LABEL_HEIGHT = 60;
    const uint16_t DEFAULT_WIDITH = 80;
    const uint16_t DEFAULT_HEIGHT = 60;
    const uint16_t DEFAULT_ELEMENT_COUNT = 10;
    const uint16_t DEFAULT_PERCENT = 10;
    const uint16_t MAX_PERCENT = 100;
    const float DEFAULT_PERCENT_INTERVAL = 100.0f;
    const int16_t TEXT_H = 20;
    const int16_t GAP = 10;
    static int16_t g_view1WidthPercent = DEFAULT_PERCENT;
    static int16_t g_view1HeightPercent = DEFAULT_PERCENT;
    static int16_t g_view2WidthPercent = DEFAULT_PERCENT;
    static int16_t g_view2HeightPercent = DEFAULT_PERCENT;
}

void UITestViewPercent::SetUp()
{
    if (container_ == nullptr) {
        InitContainer();
    }
    viewGroup = new UIViewGroup();
    view1_ = new UIButton();
    view2_ = new UIView();
    view3_ = new UIView();
    resetfBtn_ = new UILabelButton();
    BiggerBtn_ = new UILabelButton();
    SmallerBtn_ = new UILabelButton();
    positionX_ = 20; // default start x-coordinate as 20
    positionY_ = 5; // default start y-coordinate as 5
}

void UITestViewPercent::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
}

const UIView* UITestViewPercent::GetTestView()
{
    InitView();
    return container_;
}

void UITestViewPercent::InitContainer()
{
    container_ = new UIScrollView();
    container_->SetThrowDrag(true);
    container_->SetHorizontalScrollState(false);
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
}

void UITestViewPercent::InitView()
{
    if (container_ == nullptr) {
        InitContainer();
    }

    UILabel* label = new UILabel();
    container_->Add(label);
    positionY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_,
        Screen::GetInstance().GetWidth(), TITLE_LABEL_DEFAULT_HEIGHT);
    label->SetText("Flex布局效果 ");
    label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    positionY_ += (TEXT_H + GAP);

    flexLayout_ = new FlexLayout();
    container_->Add(flexLayout_);
    InitFlexLayout();
    positionY_ += flexLayout_->GetHeight() + GAP;

    InitViewGroup();
    positionY_ += viewGroup->GetHeight() + GAP;

    GridLayout* gridLayout = new GridLayout();
    container_->Add(gridLayout);
    gridLayout->Resize(260, 60); // 260: width; 60: height
    gridLayout->SetLayoutDirection(LAYOUT_HOR);
    gridLayout->SetPosition(0, positionY_); // 260: width; 60: height
    // 2: half of screen width; 2: half of gridLayout
    gridLayout->LayoutLeftOfParent(Screen::GetInstance().GetWidth() / 2 - gridLayout->GetWidth() / 2);
    gridLayout->SetRows(1); // set gridlayout row as 1
    gridLayout->SetCols(3); // set gridlayout column as 3

    SetUpButton(*gridLayout, *resetfBtn_, "reset");
    SetUpButton(*gridLayout, *BiggerBtn_, "Bigger");
    SetUpButton(*gridLayout, *SmallerBtn_, "Smaller");
    gridLayout->LayoutChildren(true);
    container_->LayoutChildren(true);
}

void UITestViewPercent::InitFlexLayout()
{
    positionY_ = VIEW_DISTANCE_TO_TOP_SIDE;
    flexLayout_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_);
    flexLayout_->SetWidthPercent(0.9f); // set default width percent as 0.9
    flexLayout_->SetHeightPercent(0.9f); // set default height percent as 0.9
    flexLayout_->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
    flexLayout_->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
    flexLayout_->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
    flexLayout_->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
    flexLayout_->SetStyle(STYLE_BACKGROUND_OPA, 0);
    for (int16_t i = 1; i <= DEFAULT_ELEMENT_COUNT; i++) {
        AddElement(i);
    }
    flexLayout_->SetFlexWrap(1);
    flexLayout_->SetLayoutDirection(LAYOUT_HOR);
    flexLayout_->SetMajorAxisAlign(ALIGN_START);
    flexLayout_->SetSecondaryAxisAlign(ALIGN_CENTER);
    flexLayout_->LayoutChildren();
}

void UITestViewPercent::InitViewGroup() const
{
    container_->Add(viewGroup);
    viewGroup->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_);
    viewGroup->SetWidthPercent(0.9f); // set default width percent as 0.9
    viewGroup->SetHeight(viewGroup->GetParent()->GetHeight());
    viewGroup->SetStyle(STYLE_BORDER_COLOR, Color::White().full);
    viewGroup->SetStyle(STYLE_BORDER_OPA, HALF_OPA_OPAQUE);
    viewGroup->SetStyle(STYLE_BORDER_WIDTH, VIEW_STYLE_BORDER_WIDTH);
    viewGroup->SetStyle(STYLE_BORDER_RADIUS, VIEW_STYLE_BORDER_RADIUS);
    viewGroup->SetStyle(STYLE_BACKGROUND_OPA, 0);
    viewGroup->Add(view1_);
    view1_->SetViewId("view1");
    view1_->SetPositionPercent(0.1, 0.1); // set view1 position percent x-percent=0.1, y-percent=0.1
    view1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    view1_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view1_->SetStyle(STYLE_BORDER_COLOR, Color::Black().full);
    view1_->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view1_->SetStyle(STYLE_BORDER_WIDTH, 1); // set border width 1
    view1_->SetStyle(STYLE_BORDER_RADIUS, 5); // set border radius 5
    view1_->SetHeightPercent(g_view1WidthPercent / DEFAULT_PERCENT_INTERVAL);
    view1_->SetWidthPercent(g_view1HeightPercent / DEFAULT_PERCENT_INTERVAL);

    viewGroup->Add(view2_);
    view2_->SetViewId("view2");
    view2_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    view2_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view2_->SetStyle(STYLE_BORDER_COLOR, Color::Blue().full);
    view2_->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view2_->SetStyle(STYLE_BORDER_WIDTH, 1); // set border width 1
    view2_->SetStyle(STYLE_BORDER_RADIUS, 5);  // set border radius 5
    view2_->SetPositionPercent(
        0.3, 0.1, /* set view2 position percent x-percent=0.3, y-percent=0.1 */
        g_view2WidthPercent / DEFAULT_PERCENT_INTERVAL, g_view2HeightPercent / DEFAULT_PERCENT_INTERVAL);

    viewGroup->Add(view3_);
    view3_->SetViewId("view3");
    view3_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    view3_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    view3_->SetStyle(STYLE_BORDER_COLOR, Color::Red().full);
    view3_->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    view3_->SetStyle(STYLE_BORDER_WIDTH, 1);  // set border width 1
    view3_->SetStyle(STYLE_BORDER_RADIUS, 5);  // set border radius 5
    view3_->SetXPercent(0.5); // set view3 x-percent=0.5
    view3_->SetYPercent(0.1); // set view3 , y-percent=0.1
    view3_->SetWidthPercent(g_view2WidthPercent / DEFAULT_PERCENT_INTERVAL);
    view3_->SetHeightPercent(g_view2HeightPercent / DEFAULT_PERCENT_INTERVAL);
}

void UITestViewPercent::AddElement(int16_t index)
{
    if (flexLayout_ == nullptr) {
        InitView();
    }

    UILabel* label = new UILabel();
    flexLayout_->Add(label);
    float percent = DEFAULT_PERCENT / DEFAULT_PERCENT_INTERVAL;
    label->SetWidthPercent(percent);
    label->SetHeightPercent(percent);
    label->SetText(std::to_string(index).c_str());
    label->SetFont("HYQiHei-65S.otf", 20); // set default font size as 20
    label->SetStyle(STYLE_TEXT_COLOR, Color::Black().full);
    label->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    label->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    label->SetStyle(STYLE_BORDER_COLOR, Color::Black().full);
    label->SetStyle(STYLE_BORDER_OPA, OPA_OPAQUE);
    label->SetStyle(STYLE_BORDER_WIDTH, 1); // set border width 1
    label->SetStyle(STYLE_BORDER_RADIUS, 5);  // set border radius 5
    label->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
}

bool UITestViewPercent::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == resetfBtn_) {
        g_view1HeightPercent = DEFAULT_PERCENT;
        g_view1WidthPercent = DEFAULT_PERCENT;
        g_view2HeightPercent = DEFAULT_PERCENT;
        g_view2WidthPercent = DEFAULT_PERCENT;
        view1_->ResizePercent(g_view1WidthPercent / DEFAULT_PERCENT_INTERVAL,
            g_view1HeightPercent / DEFAULT_PERCENT_INTERVAL);
        view2_->ResizePercent(g_view2WidthPercent / DEFAULT_PERCENT_INTERVAL,
            g_view2HeightPercent / DEFAULT_PERCENT_INTERVAL);
        g_widthPercent = DEFAULT_PERCENT;
        flexLayout_->SetWidthPercent(g_widthPercent / DEFAULT_PERCENT_INTERVAL);
    } else if (&view == BiggerBtn_) {
        if (g_widthPercent < DEFAULT_PERCENT) {
            g_widthPercent++;
            flexLayout_->SetWidthPercent(g_widthPercent / DEFAULT_PERCENT_INTERVAL);
        }
        g_view1HeightPercent++;
        g_view1WidthPercent++;
        g_view2HeightPercent++;
        g_view2WidthPercent++;
        view1_->ResizePercent(g_view1WidthPercent / DEFAULT_PERCENT_INTERVAL,
            g_view1HeightPercent / DEFAULT_PERCENT_INTERVAL);
        view2_->ResizePercent(g_view2WidthPercent / DEFAULT_PERCENT_INTERVAL,
            g_view2HeightPercent / DEFAULT_PERCENT_INTERVAL);
    } else if (&view == SmallerBtn_) {
        g_view1HeightPercent--;
        g_view1WidthPercent--;
        g_view2HeightPercent--;
        g_view2WidthPercent--;
        view1_->ResizePercent(g_view1WidthPercent / DEFAULT_PERCENT_INTERVAL,
            g_view1HeightPercent / DEFAULT_PERCENT_INTERVAL);
        view2_->ResizePercent(g_view2WidthPercent / DEFAULT_PERCENT_INTERVAL,
            g_view2HeightPercent / DEFAULT_PERCENT_INTERVAL);
        flexLayout_->SetWidthPercent((g_widthPercent--) / DEFAULT_PERCENT_INTERVAL);
    }
    view2_->GetParent()->Invalidate();
    return true;
}
}

