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

#include "ui_test_ui_scroll_view.h"
#include "common/screen.h"
#include "components/ui_label_button.h"

#define VISIBLE_XBAR(visible)                                               \
    if (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) { \
        scroll->SetXScrollBarVisible(visible);                              \
    }                                                                       \

namespace OHOS {
namespace {
static int16_t g_buttonH = 400;
static int16_t g_buttonW = 400;
static int16_t g_smallButtonH = 40;
static int16_t g_smallButtonW = 40;
static int16_t g_blank = 20;
static int16_t g_scrollH = 300;
static int16_t g_scrollW = 300;
static UILabel* g_scrollSateLabel = nullptr;
const int16_t DELTA_Y_COORDINATE = 19;
const int16_t DELTA_Y_COORDINATE_2 = 37;
} // namespace

class TestOnScrollListener : public UIScrollView::OnScrollListener {
    void OnScrollStart() override
    {
        g_scrollSateLabel->SetText("moving");
        g_scrollSateLabel->Invalidate();
    }

    void OnScrollEnd() override
    {
        g_scrollSateLabel->SetText("stop");
        g_scrollSateLabel->Invalidate();
    }
};

static TestOnScrollListener* g_listener = nullptr;

void UITestUIScrollView::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
    }
    if (g_scrollSateLabel == nullptr) {
        g_scrollSateLabel = GetTitleLabel("stop");
        g_scrollSateLabel->SetPosition(0, 0, 100, TITLE_LABEL_DEFAULT_HEIGHT); // 100: width
        g_scrollSateLabel->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    }

    if (g_listener == nullptr) {
        g_listener = new TestOnScrollListener();
    }
}

void UITestUIScrollView::TearDown()
{
    container_->Remove(g_scrollSateLabel);
    DeleteChildren(container_);
    container_ = nullptr;
    if (g_scrollSateLabel != nullptr) {
        delete g_scrollSateLabel;
        g_scrollSateLabel = nullptr;
    }
    if (g_listener != nullptr) {
        delete g_listener;
        g_listener = nullptr;
    }
    lastX_ = 0;
    lastY_ = 0;
    UITest::TearDown();
}

const UIView* UITestUIScrollView::GetTestView()
{
    UIKit_UIScrollView_Test_bar_001();
    UIKit_UIScrollView_Test_bar_002();
    UIKit_UIScrollView_Test_bar_003();
    UIKit_UIScrollView_Test_bar_004();
    UIKit_UIScrollView_Test_Scrollable_Set_001();
    UIKit_UIScrollView_Test_Scrollable_Set_002();
    UIKit_UIScrollView_Test_Scroll_Blank_Set_001();
    UIKit_UIScrollView_Test_Scroll_listener_001();
    return container_;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_bar_001()
{
    if (container_ == nullptr) {
        return;
    }
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    UILabel* label = GetTitleLabel("UIScrollView设置游标可见 ");
    label->SetPosition(positionX_, positionY_);
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    VISIBLE_XBAR(false);
    scroll->SetYScrollBarVisible(true);
    scroll->SetReboundSize(50); // 50: rebound size
    container_->Add(scroll);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    scroll->Add(button1);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_bar_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView设置游标可见 ");
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    VISIBLE_XBAR(true);
    scroll->SetYScrollBarVisible(true);
    container_->Add(scroll);
    UIButton* button1 = new UIButton();
    button1->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    scroll->Add(button1);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_bar_003()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView游标滑动状态 ");
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    VISIBLE_XBAR(true);
    scroll->SetYScrollBarVisible(true);
    container_->Add(scroll);
    UILabelButton* button1 = new UILabelButton();
    button1->SetText("button1");
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    UILabelButton* button2 = new UILabelButton();
    button2->SetText("button2");
    button2->SetPosition(0, g_buttonH, g_buttonW, g_buttonH);
    scroll->Add(button1);
    scroll->Add(button2);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_bar_004()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView设置游标宽度为20");
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    VISIBLE_XBAR(true);
    scroll->SetYScrollBarVisible(true);
    container_->Add(scroll);
    UIButton* button1 = new UIButton();
    button1->SetPosition(0, 0, g_smallButtonW, g_smallButtonH);
    scroll->Add(button1);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_Scrollable_Set_001()
{
    if (container_ == nullptr) {
        return;
    }
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE; // 2: half of screen width
    positionY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    UILabel* label = GetTitleLabel("UIScrollView设置仅垂直方向可滑动 ");
    container_->Add(label);
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    scroll->SetScrollBlankSize(20); // 20: means scroll blank size
    scroll->SetHorizontalScrollState(false);
    scroll->SetVerticalScrollState(true);
    VISIBLE_XBAR(true);
    scroll->SetYScrollBarVisible(true);
    container_->Add(scroll);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    scroll->Add(button1);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_Scrollable_Set_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView设置仅垂直方向不可滑动 ");
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    scroll->SetHorizontalScrollState(true);
    scroll->SetVerticalScrollState(false);
    VISIBLE_XBAR(true);
    scroll->SetYScrollBarVisible(true);
    container_->Add(scroll);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    scroll->Add(button1);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_Scroll_Blank_Set_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView设置高为50的可滑动空白区域 ");
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    scroll->SetScrollBlankSize(50); // (50): means scroll blank size
    VISIBLE_XBAR(true);
    scroll->SetYScrollBarVisible(true);
    scroll->SetScrollBarSide(SCROLL_BAR_LEFT_SIDE);
    container_->Add(scroll);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    scroll->Add(button1);
    SetLastPos(scroll);
    positionY_ += g_scrollH;
}

void UITestUIScrollView::UIKit_UIScrollView_Test_Scroll_listener_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UIScrollView设置滑动状态监听 ");
    container_->Add(label);
    positionY_ += DELTA_Y_COORDINATE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += DELTA_Y_COORDINATE_2;

    UIScrollView* scroll = new UIScrollView();
    scroll->SetIntercept(true);
    scroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scroll->SetPosition(positionX_, positionY_, g_scrollW, g_scrollH);
    container_->Add(scroll);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    scroll->Add(button1);
    scroll->RegisterScrollListener(g_listener);
    g_scrollSateLabel->SetPosition(positionX_ + scroll->GetWidth() + 10, scroll->GetY()); // 10: increase width
    container_->Add(g_scrollSateLabel);

    SetLastPos(scroll);
}

void UITestUIScrollView::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS
