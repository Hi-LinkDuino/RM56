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

#include "ui_test_ui_swipe_view.h"

#include <string>

#include "common/screen.h"
#include "components/ui_label.h"
#include "components/ui_swipe_view.h"

namespace OHOS {
namespace {
static int16_t g_buttonH = 80;
static int16_t g_buttonW = 400;
static int16_t g_blank = 20;
static int16_t g_swipeH = 200;
static int16_t g_swipeW = 400;
static int16_t g_swipeHorH = 110;
static int16_t g_deltaCoordinateY = 19;
static int16_t g_deltaCoordinateY2 = 37;
static uint16_t g_MaxIndex = 3;
} // namespace

void UITestUISwipeView::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetThrowDrag(true);
        container_->SetHorizontalScrollState(false);
    }
}

void UITestUISwipeView::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    addBtnInHead_ = nullptr;
    addBtnInTail_ = nullptr;
    addBtnInMid_ = nullptr;
    removeHeadBtn_ = nullptr;
    removeMidBtn_ = nullptr;
    removeAllBtn_ = nullptr;
    loopBtn_ = nullptr;
    changePageBtn_ = nullptr;
    lastX_ = 0;
    lastY_ = 0;
}

const UIView* UITestUISwipeView::GetTestView()
{
    UIKit_Swipe_View_Test_Horizontal_001();
    UIKit_Swipe_View_Test_Horizontal_002();
    UIKit_Swipe_View_Test_Horizontal_003();
    UIKit_Swipe_View_Test_Align_001(UISwipeView::ALIGN_LEFT);
    UIKit_Swipe_View_Test_Align_001(UISwipeView::ALIGN_CENTER);
    UIKit_Swipe_View_Test_Align_001(UISwipeView::ALIGN_RIGHT);

    UIKit_Swipe_View_Test_Ver_001();
    UIKit_Swipe_View_Test_Ver_002();
    UIKit_Swipe_View_Test_Remove_001();

    UIKit_Swipe_View_Test_SetCurrentPage();

    return container_;
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Horizontal_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("两个子元素UISwipeView循环水平滑动");
    container_->Add(label);
    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE;
    positionY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetLoopState(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeW, g_swipeHorH);
    swipe->SetBlankSize(100); // 100: is blank size
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    swipe->Add(button2);
    SetLastPos(swipe);
    positionY_ += g_swipeHorH;
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Horizontal_002()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UISwipeView水平滑动");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeW, g_swipeHorH);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonW, g_buttonH);
    button3->SetText("button3");
    swipe->Add(button3);
    SetLastPos(swipe);
    positionY_ += g_swipeHorH;
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Horizontal_003()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UISwipeView循环水平滑动");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeW, g_swipeHorH);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW, g_buttonH);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW, g_buttonH);
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonW, g_buttonH);
    button3->SetText("button3");
    swipe->Add(button3);
    SetLastPos(swipe);
    positionY_ += g_swipeHorH;
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Ver_001()
{
    if (container_ == nullptr) {
        return;
    }
    positionX_ = Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE; // 2: half of screen width
    positionY_ = TEXT_DISTANCE_TO_TOP_SIDE;
    UILabel* label = GetTitleLabel("UISwipeView垂直滑动");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::VERTICAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeH, g_swipeW);
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonH, g_buttonW);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonH, g_buttonW);
    button2->SetText("button2");
    swipe->Add(button2);
    SetLastPos(swipe);
    positionY_ += g_swipeW + g_deltaCoordinateY;
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Ver_002()
{
    if (container_ == nullptr) {
        return;
    }

    UILabel* label = GetTitleLabel("UISwipeView循环垂直滑动");
    container_->Add(label);
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::VERTICAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeH, g_swipeW);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: is animator drag time(ms)
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonH, g_buttonW);
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonH, g_buttonW);
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonH, g_buttonW);
    button3->SetText("button3");
    swipe->Add(button3);
    SetLastPos(swipe);
    positionY_ += g_swipeW;
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Remove_001()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("UISwipeView增加和删除子节点");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(positionX_, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(positionX_, positionY_, g_swipeH, g_swipeW);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    currentSwipe_ = swipe;
    container_->Add(swipe);

    if (addBtnInHead_ == nullptr) {
        addBtnInHead_ = new UILabelButton();
    }
    if (addBtnInTail_ == nullptr) {
        addBtnInTail_ = new UILabelButton();
    }
    if (addBtnInMid_ == nullptr) {
        addBtnInMid_ = new UILabelButton();
    }
    if (removeHeadBtn_ == nullptr) {
        removeHeadBtn_ = new UILabelButton();
    }
    if (removeMidBtn_ == nullptr) {
        removeMidBtn_ = new UILabelButton();
    }
    if (removeAllBtn_ == nullptr) {
        removeAllBtn_ = new UILabelButton();
    }

    positionX_ = positionX_ + swipe->GetWidth() + 20; // 20: is interval between button and swipe
    SetUpButton(addBtnInHead_, "增加至头部 ");
    SetUpButton(addBtnInTail_, "增加至尾部 ");
    SetUpButton(addBtnInMid_, "增加至头部后 ");
    SetUpButton(removeHeadBtn_, "删除头部节点 ");
    SetUpButton(removeMidBtn_, "删除中间节点 ");
    SetUpButton(removeAllBtn_, "删除全部节点 ");

    SetLastPos(swipe);
}

void UITestUISwipeView::UIKit_Swipe_View_Test_Align_001(UISwipeView::AlignMode alignMode)
{
    static uint8_t divNum = 4;
    if (container_ == nullptr) {
        return;
    }

    const char* leftTitle = "UISwipeView 子组件左对齐";
    const char* centerTitle = "UISwipeView 子组件Center对齐";
    const char* rightTitle = "UISwipeView 子组件右对齐";
    UILabel* label;
    if (alignMode == UISwipeView::ALIGN_LEFT) {
        label = GetTitleLabel(leftTitle);
    } else if (alignMode == UISwipeView::ALIGN_RIGHT) {
        label = GetTitleLabel(rightTitle);
    } else {
        label = GetTitleLabel(centerTitle);
    }

    container_->Add(label);
    label->SetPosition(positionX_, lastY_ + g_blank);

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    // 100: offset
    swipe->SetPosition(positionX_, label->GetY() + g_blank + g_deltaCoordinateY, g_swipeW - 100, g_swipeH);
    swipe->SetLoopState(true);
    swipe->SetAnimatorTime(100); // 100: mean animator drag time(ms)
    swipe->SetAlignMode(alignMode);
    container_->Add(swipe);
    UILabelButton* button1 = new UILabelButton();
    button1->SetPosition(0, 0, g_buttonW / divNum, g_buttonH / 2); // 2: half
    button1->SetText("button1");
    swipe->Add(button1);
    UILabelButton* button2 = new UILabelButton();
    button2->SetPosition(0, 0, g_buttonW / divNum, g_buttonH / 2); // 2: half
    button2->SetText("button2");
    swipe->Add(button2);
    UILabelButton* button3 = new UILabelButton();
    button3->SetPosition(0, 0, g_buttonW / divNum, g_buttonH / 2); // 2: half
    button3->SetText("button3");
    swipe->Add(button3);

    UILabel* tmpLabel = nullptr;
    const char* tmpOkTitle = "The result of GetAlignMode is OK.";
    const char* tmpErrTitle = "The result of GetAlignMode is Error.";
    uint8_t tmpAlignMode = swipe->GetAlignMode();
    if (tmpAlignMode == alignMode) {
        tmpLabel = GetTitleLabel(tmpOkTitle);
    } else {
        tmpLabel = GetTitleLabel(tmpErrTitle);
    }
    tmpLabel->SetPosition(positionX_, 100, 250, 25); // 100: y, 250:width, 25:height
    swipe->Add(tmpLabel);

    SetLastPos(swipe);
}

void UITestUISwipeView::UIKit_Swipe_View_Test_SetCurrentPage()
{
    if (container_ == nullptr) {
        return;
    }
    positionY_ += g_deltaCoordinateY;
    UILabel* label = GetTitleLabel("UISwipeView切换页面");
    container_->Add(label);
    positionY_ += g_deltaCoordinateY;
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_);
    positionY_ += g_deltaCoordinateY2;

    UISwipeView* swipe = new UISwipeView(UISwipeView::HORIZONTAL);
    swipe->SetIntercept(true);
    swipe->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    swipe->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, g_swipeW, g_swipeH);
    swipe->SetLoopState(loop_);
    swipe->SetAnimatorTime(1000); // 1000: mean animator drag time(ms)
    currentSwipe_ = swipe;
    container_->Add(swipe);
    UIView* view1 = new UIView();
    view1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    view1->Resize(g_swipeW, g_swipeH);
    swipe->Add(view1);
    UIView* view2 = new UIView();
    view2->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    view2->Resize(g_swipeW, g_swipeH);
    swipe->Add(view2);
    UIView* view3 = new UIView();
    view3->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    view3->Resize(g_swipeW, g_swipeH);
    swipe->Add(view3);
    UIView* view4 = new UIView();
    view4->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    view4->Resize(g_swipeW, g_swipeH);
    swipe->Add(view4);
    if (loopBtn_ == nullptr) {
        loopBtn_ = new UILabelButton();
    }
    if (changePageBtn_ == nullptr) {
        changePageBtn_ = new UILabelButton();
    }

    positionX_ = TEXT_DISTANCE_TO_LEFT_SIDE + swipe->GetWidth() + 20; // 20: is interval between button and swipe
    positionY_ += g_deltaCoordinateY2;
    SetUpButton(loopBtn_, "设置循环 关 ");
    SetUpButton(changePageBtn_, "切换页面 ");
}

bool UITestUISwipeView::OnClick(UIView& view, const ClickEvent& event)
{
    if (currentSwipe_ == nullptr) {
        return true;
    }
    if (&view == addBtnInHead_) {
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(0, 0, g_buttonH, g_buttonW);
        btn->SetText(std::to_string(btnNum_).c_str());
        currentSwipe_->Insert(nullptr, btn);
    } else if (&view == addBtnInTail_) {
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(0, 0, g_buttonH, g_buttonW);
        btn->SetText(std::to_string(btnNum_).c_str());
        currentSwipe_->Add(btn);
    } else if (&view == addBtnInMid_) {
        UILabelButton* btn = new UILabelButton();
        btn->SetPosition(0, 0, g_buttonH, g_buttonW);
        btn->SetText(std::to_string(btnNum_).c_str());
        currentSwipe_->Insert(currentSwipe_->GetChildrenHead(), btn);
    } else if (&view == removeHeadBtn_) {
        currentSwipe_->Remove(currentSwipe_->GetChildrenHead());
    } else if (&view == removeMidBtn_) {
        UIView* view = currentSwipe_->GetViewByIndex(1);
        currentSwipe_->Remove(view);
    } else if (&view == removeAllBtn_) {
        currentSwipe_->RemoveAll();
    } else if (&view == loopBtn_) {
        loop_ = !loop_;
        currentSwipe_->SetLoopState(loop_);
        if (!loop_) {
            loopBtn_->SetText("设置循环 关 ");
        } else {
            loopBtn_->SetText("设置循环 开 ");
        }
    } else if (&view == changePageBtn_) {
        uint16_t currentIndex = currentSwipe_->GetCurrentPage();
        if (currentIndex < g_MaxIndex) {
            currentSwipe_->SetCurrentPage(++currentIndex, true);
        } else {
            currentSwipe_->SetCurrentPage(0, true);
        }
    }
    currentSwipe_->Invalidate();
    btnNum_++;
    return true;
}

void UITestUISwipeView::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    container_->Add(btn);
    btn->SetPosition(positionX_, positionY_, BUTTON_WIDHT2, BUTTON_HEIGHT2);
    positionY_ += btn->GetHeight() + 10; // 10: increase y-coordinate
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
    container_->Invalidate();
}

void UITestUISwipeView::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
}
} // namespace OHOS
