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

#include "graphic_config.h"

#if ENABLE_DEBUG
#include "components/root_view.h"
#include "dfx/event_injector.h"
#include "engines/gfx/gfx_engine_manager.h"
#include "ui_test_event_injector.h"
#include <string>
#if ENABLE_WINDOW
#include "window/window.h"
#endif

namespace OHOS {
namespace {
const int16_t ITEM_H = 50;
const int16_t TEXT_H = 29;
const int16_t TEXT_W = 250;
const int16_t TEST_VIEW_H = 50;
const int16_t TEST_VIEW_W = 50;
const int16_t GAP = 5;
const int16_t TEST_VIEW_GAP = 80;
const int16_t TEST_BUTTON_W = 100;
const int16_t TEST_BUTTON_H = 30;
const int16_t LAYOUT_HEIGHT = 150;
const int16_t LAYOUT_WIDTH = 480;
const int16_t BLANK = 20;
const int16_t SCROLL_WIDTH = 300;
const int16_t SCROLL_HEIGHT = 300;
const int16_t SCROLL_BUTTON_W = 800;
const int16_t SCROLL_BUTTON_H = 400;
const int16_t DRAG_TIME_OFFSET = 20;
const int16_t RATIO_X = 3;
const int16_t RATIO_Y = 10;
const int16_t POINT_OFFSET = 4;
} // namespace

class TestEventInjectorView : public UIView, public RootView::OnKeyActListener {
public:
    TestEventInjectorView() : label_(nullptr) {}
    virtual ~TestEventInjectorView()
    {
        RootView::GetInstance()->ClearOnKeyActListener();
    }

    void InitListener()
    {
        RootView::GetInstance()->SetOnKeyActListener(this);
    }

    bool OnLongPressEvent(const LongPressEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("long press!");
            label_->Invalidate();
        }
        return UIView::OnLongPressEvent(event);
    }

    bool OnDragEvent(const DragEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("drag!");
            label_->Invalidate();
        }
        return UIView::OnDragEvent(event);
    }

    void SetSentence(const char* sentence)
    {
        sentence_ = sentence;
    }

    bool OnClickEvent(const ClickEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText(sentence_);
            label_->Invalidate();
        }
        return UIView::OnClickEvent(event);
    }

    bool OnPressEvent(const PressEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("press!");
            label_->Invalidate();
        }
        return UIView::OnPressEvent(event);
    }

    bool OnReleaseEvent(const ReleaseEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("release!");
            label_->Invalidate();
        }
        return UIView::OnReleaseEvent(event);
    }

    bool OnCancelEvent(const CancelEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("cancel!");
            label_->Invalidate();
        }
        return UIView::OnCancelEvent(event);
    }

    void SetLabel(UILabel* label)
    {
        label_ = label;
    }

    bool OnKeyAct(UIView& view, const KeyEvent& event) override
    {
        if (label_ == nullptr) {
            return false;
        }
        label_->SetText("key!");
        label_->Invalidate();
        return true;
    }

private:
    UILabel* label_;
    const char* sentence_ = "click";
};

void UITestEventInjector::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
        container_->SetThrowDrag(true);
    }
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::POINT_TYPE);
    EventInjector::GetInstance()->RegisterEventInjector(EventDataType::KEY_TYPE);

#if ENABLE_WINDOW
    Window* window = RootView::GetInstance()->GetBoundWindow();
    if (window != nullptr) {
        EventInjector::GetInstance()->SetWindowId(window->GetWindowId());
    }
#endif
}

void UITestEventInjector::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    layout_ = nullptr;
    positionX_ = 20; // 20 : start x
    positionY_ = 20; // 20 : start y
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::POINT_TYPE);
    }
    if (EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::KEY_TYPE)) {
        EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::KEY_TYPE);
    }
}

const UIView* UITestEventInjector::GetTestView()
{
    UIKit_Event_Injector_Click_Event_001();
    UIKit_Event_Injector_Drag_Event_002();
    UIKit_Event_Injector_Long_Press_Event_003();
    UIKit_Event_Injector_Key_Event_004();

    SetUpScrollView();
    UIKit_Event_Injector_Up_to_Down_005();
    UIKit_Event_Injector_Down_to_Up_006();
    UIKit_Event_Injector_Left_to_Right_007();
    UIKit_Event_Injector_Right_to_Left_008();
    UIKit_Event_Injector_ULeft_to_LRight_009();
    UIKit_Event_Injector_LRight_to_ULeft_010();
    IncreaseDragTime();
    DecreaseDragTime();
    DragTimeDisplay();

    layout_->LayoutChildren();
    return container_;
}

void UITestEventInjector::UIKit_Event_Injector_Click_Event_001()
{
    clickBtn_ = new UILabelButton();
    InnerTest("模拟点击事件 ", true, false, false, "click", clickBtn_, clickTestView_);
}

void UITestEventInjector::UIKit_Event_Injector_Drag_Event_002()
{
    dragBtn_ = new UILabelButton();
    InnerTest("模拟滑动事件 ", true, true, false, "drag", dragBtn_, dragTestView_);
}

void UITestEventInjector::UIKit_Event_Injector_Long_Press_Event_003()
{
    longPressBtn_ = new UILabelButton();
    InnerTest("模拟长按事件 ", true, true, true, "long press", longPressBtn_, longPressTestView_);
}

void UITestEventInjector::UIKit_Event_Injector_Key_Event_004()
{
    keyBtn_ = new UILabelButton();
    InnerTest("模拟按键输入 ", true, false, false, "key event", keyBtn_, KeyEventTestView_);
}

void UITestEventInjector::UIKit_Event_Injector_Up_to_Down_005()
{
    upToDownBtn_ = new UILabelButton();
    SetUpButton(upToDownBtn_, "up to down");
}

void UITestEventInjector::UIKit_Event_Injector_Down_to_Up_006()
{
    downToUpBtn_ = new UILabelButton();
    SetUpButton(downToUpBtn_, "down to up");
}

void UITestEventInjector::UIKit_Event_Injector_Left_to_Right_007()
{
    leftToRightBtn_ = new UILabelButton();
    SetUpButton(leftToRightBtn_, "left to right");
}

void UITestEventInjector::UIKit_Event_Injector_Right_to_Left_008()
{
    rightToLeftBtn_ = new UILabelButton();
    SetUpButton(rightToLeftBtn_, "right to left");
}

void UITestEventInjector::UIKit_Event_Injector_ULeft_to_LRight_009()
{
    uLeftTolRightBtn_ = new UILabelButton();
    SetUpButton(uLeftTolRightBtn_, "uLeft to lRight");
}

void UITestEventInjector::UIKit_Event_Injector_LRight_to_ULeft_010()
{
    lRightTouLeftBtn_ = new UILabelButton();
    SetUpButton(lRightTouLeftBtn_, "lRight to uleft");
}

void UITestEventInjector::IncreaseDragTime()
{
    increaseDragTimeBtn_ = new UILabelButton();
    SetUpButton(increaseDragTimeBtn_, "increase drag time");
}

void UITestEventInjector::DecreaseDragTime()
{
    decreaseDragTimeBtn_ = new UILabelButton();
    SetUpButton(decreaseDragTimeBtn_, "decrease drag time");
}

void UITestEventInjector::DragTimeDisplay()
{
    DragTimeDisplayBtn_ = new UILabelButton();
    std::string dragTimestr = "dragTime:" + std::to_string(dragTime_);
    SetUpButton(DragTimeDisplayBtn_, dragTimestr.c_str());
}

void UITestEventInjector::InnerTest(const char* title,
                                    bool touchable,
                                    bool draggable,
                                    bool dragParent,
                                    const char* btnTitle,
                                    UILabelButton* btn,
                                    TestEventInjectorView*& testView)
{
    if ((container_ != nullptr) && (btn != nullptr)) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_, Screen::GetInstance().GetWidth(),
                           TITLE_LABEL_DEFAULT_HEIGHT);
        label->SetText(title);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ += (TEXT_H + GAP);
        testView = new TestEventInjectorView();
        testView->InitListener();
        container_->Add(testView);
        testView->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_, TEST_VIEW_W, TEST_VIEW_H);
        testView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
        testView->SetTouchable(touchable);
        testView->SetDraggable(draggable);
        testView->SetDragParentInstead(dragParent);
        UILabel* label1 = new UILabel();
        container_->Add(label1);
        label1->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + TEST_VIEW_GAP,
            positionY_ + 2 * GAP, TEXT_W, TEXT_H); // 2 : ratio
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        testView->SetLabel(label1);

        container_->Add(btn);
        btn->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + 3 * TEST_VIEW_GAP, positionY_ + GAP, // 3 : ratio
                         BUTTON_WIDHT2, BUTTON_HEIGHT2);
        btn->SetText(btnTitle);
        btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_LABEL_SIZE);
        btn->SetOnClickListener(this);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
        btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);

        positionY_ += ITEM_H;
    }
}

void UITestEventInjector::SetUpScrollView()
{
    if (container_ == nullptr) {
        return;
    }
    UILabel* label = GetTitleLabel("模拟Drag事件测试");
    container_->Add(label);
    label->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_);
    SetLastPos(label);

    scrollTestView_ = new UIScrollView();
    scrollTestView_->SetIntercept(true);
    scrollTestView_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    scrollTestView_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE, positionY_, SCROLL_WIDTH, SCROLL_HEIGHT);
    if (Screen::GetInstance().GetScreenShape() == ScreenShape::RECTANGLE) {
        scrollTestView_->SetXScrollBarVisible(true);
    }
    scrollTestView_->SetYScrollBarVisible(true);
    scrollTestView_->SetThrowDrag(true);
    container_->Add(scrollTestView_);
    UILabelButton* button1 = new UILabelButton();
    button1->SetText("button1");
    button1->SetPosition(0, 0, SCROLL_BUTTON_W, SCROLL_BUTTON_H);
    UILabelButton* button2 = new UILabelButton();
    button2->SetText("button2");
    button2->SetPosition(0, SCROLL_BUTTON_H, SCROLL_BUTTON_W, SCROLL_BUTTON_H);
    scrollTestView_->Add(button1);
    scrollTestView_->Add(button2);

    if (layout_ == nullptr) {
        layout_ = new GridLayout();
        layout_->SetPosition(VIEW_DISTANCE_TO_LEFT_SIDE + SCROLL_WIDTH + GAP, positionY_, LAYOUT_WIDTH, LAYOUT_HEIGHT);
        container_->Add(layout_);
        layout_->SetLayoutDirection(LAYOUT_VER);
        layout_->SetRows(3); // 3 : rows
        layout_->SetCols(3); // 3 : columns
    }
}

void UITestEventInjector::SetLastPos(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    lastX_ = view->GetX();
    lastY_ = view->GetY() + view->GetHeight();
    positionY_ = lastY_ + 8 * GAP; /* 8:ratio */
}

void UITestEventInjector::SetUpButton(UILabelButton* btn, const char* title)
{
    if (btn == nullptr) {
        return;
    }
    layout_->Add(btn);
    btn->Resize(BUTTON_WIDHT3, BUTTON_HEIGHT3);
    btn->SetText(title);
    btn->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
    btn->SetOnClickListener(this);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BORDER_RADIUS, BUTTON_STYLE_BORDER_RADIUS_VALUE, UIButton::INACTIVE);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::RELEASED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::PRESSED);
    btn->SetStyleForState(STYLE_BACKGROUND_COLOR, BUTTON_STYLE_BACKGROUND_COLOR_VALUE, UIButton::INACTIVE);
}

void UITestEventInjector::SetDragTimeDisplay(uint16_t dragTime)
{
    std::string dragTimestr = "dragTime:" + std::to_string(dragTime);
    DragTimeDisplayBtn_->SetText(dragTimestr.c_str());
    DragTimeDisplayBtn_->Invalidate();
}

/* drag range:0 < x < 300, 62 < y < 362, click range:20 < x < 60, 392 < y < 432 */
bool UITestEventInjector::OnClick(UIView& view, const ClickEvent& event)
{
    Point point;
    Point scrollPoint;
    if (&view == clickBtn_) {
        point = {static_cast<int16_t>(clickTestView_->GetRect().GetX() + TEST_VIEW_W / POINT_OFFSET),
                 clickBtn_->GetRect().GetBottom()};
        EventInjector::GetInstance()->SetClickEvent(point);
    } else if (&view == dragBtn_) {
        point = {static_cast<int16_t>(dragTestView_->GetRect().GetX() + TEST_VIEW_W / POINT_OFFSET),
                 dragBtn_->GetRect().GetBottom()};
        Point endPoint = {static_cast<int16_t>(point.x + (RATIO_X * TEST_VIEW_W) / POINT_OFFSET), point.y};
        // 80:ms
        EventInjector::GetInstance()->SetDragEvent(point, endPoint, 80);
    } else if (&view == longPressBtn_) {
        point = {static_cast<int16_t>(longPressTestView_->GetRect().GetX() + TEST_VIEW_W / POINT_OFFSET),
                 longPressBtn_->GetRect().GetBottom()};
        EventInjector::GetInstance()->SetLongPressEvent(point);
    } else if (&view == keyBtn_) {
        // 26:key id
        uint16_t keyId = 26;
        EventInjector::GetInstance()->SetKeyEvent(keyId, InputDevice::STATE_PRESS);
    } else if (&view == upToDownBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                            static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_Y)};
        Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                          static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == downToUpBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                            static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                          static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_Y)};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == leftToRightBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_Y),
                            static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                          static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == rightToLeftBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                            static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_Y),
                          static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == uLeftTolRightBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_Y),
                            static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_Y)};
        Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                          static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == lRightTouLeftBtn_) {
        scrollPoint = {scrollTestView_->GetRect().GetX(), scrollTestView_->GetRect().GetY()};
        Point startPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_X),
                            static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_X)};
        Point endPoint = {static_cast<int16_t>(scrollPoint.x + SCROLL_WIDTH / RATIO_Y),
                          static_cast<int16_t>(scrollPoint.y + SCROLL_HEIGHT / RATIO_Y)};
        EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, dragTime_);
    } else if (&view == increaseDragTimeBtn_) {
        dragTime_ += DRAG_TIME_OFFSET;
        SetDragTimeDisplay(dragTime_);
    } else if (&view == decreaseDragTimeBtn_) {
        dragTime_ -= DRAG_TIME_OFFSET;
        SetDragTimeDisplay(dragTime_);
    }
    return true;
}
} // namespace OHOS
#endif // ENABLE_DEBUG