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

#include "ui_test_input_event.h"

#include "common/screen.h"
#include "components/root_view.h"

namespace OHOS {
namespace {
const int16_t ITEM_H = 50;
const int16_t TEXT_H = 29;
const int16_t TEXT_W = 250;
const int16_t TEST_VIEW_H = 40;
const int16_t TEST_VIEW_W = 40;
const int16_t GAP = 5;
const int16_t TEST_VIEW_GAP = 80;
} // namespace

class TestView : public UIView {
public:
    TestView() {}
    virtual ~TestView() {}
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

    void SetLabel2(UILabel* label)
    {
        label2_ = label;
    }

private:
    UILabel* label_ = nullptr;
    UILabel* label2_ = nullptr;
    const char* sentence_ = "click";
};

class TestUIScrollView : public UIScrollView {
public:
    TestUIScrollView() {}
    virtual ~TestUIScrollView() {}
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
        return UIScrollView::OnDragEvent(event);
    }

    bool OnDragStartEvent(const DragEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("drag start!");
            label_->Invalidate();
        }
        return UIScrollView::OnDragStartEvent(event);
    }

    bool OnDragEndEvent(const DragEvent& event) override
    {
        if (label_ != nullptr) {
            label_->SetText("drag end!");
            label_->Invalidate();
        }
        return UIScrollView::OnDragEndEvent(event);
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

private:
    UILabel* label_ = nullptr;
    const char* sentence_ = "click";
};

void UITestInputEvent::SetUp()
{
    if (container_ == nullptr) {
        container_ = new UIScrollView();
        container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight() - BACK_BUTTON_HEIGHT);
        container_->SetHorizontalScrollState(false);
    }
    positionX_ = 48; // 48: position x
    positionY_ = 0;
}

void DeleteChildrenAndListener(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView* tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup()) {
            DeleteChildrenAndListener(static_cast<UIViewGroup*>(tempView)->GetChildrenHead());
        }
        if (tempView->GetParent()) {
            static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
        }

        if (tempView->GetOnClickListener()) {
            delete tempView->GetOnClickListener();
        }
        if (tempView->GetOnDragListener()) {
            delete tempView->GetOnDragListener();
        }
        if (tempView->GetOnLongPressListener()) {
            delete tempView->GetOnLongPressListener();
        }
        if (tempView->GetTouchListener()) {
            delete tempView->GetTouchListener();
        }

        delete tempView;
    }
}

void UITestInputEvent::TearDown()
{
    DeleteChildrenAndListener(container_);
    container_ = nullptr;
    RootView::GetInstance()->ClearOnKeyActListener();
    if (keyListener_ != nullptr) {
        delete keyListener_;
        keyListener_ = nullptr;
    }
}

const UIView* UITestInputEvent::GetTestView()
{
    UIKit_Pointer_Input_Test_Dispatch_Simple_Event_001();
    UIKit_Pointer_Input_Test_Dispatch_Simple_Event_002();
    UIKit_Pointer_Input_Test_Dispatch_Drag_Event_001();
    UIKit_Pointer_Input_Test_Dispatch_Drag_Event_002();
    UIKit_Pointer_Input_Test_Dispatch_Drag_Event_003();
    UIKit_Pointer_Input_Test_Dispatch_Key_Event_001();
    UIKit_Pointer_Input_Test_Dispatch_InVisible_Event_001();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_001();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_002();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_003();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_004();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_005();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_006();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_007();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_008();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_009();
    UIKit_Pointer_Input_Test_Dispatch_Bubble_010();
    return container_;
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Simple_Event_001()
{
    InnerTest("可点击对象事件测试 ", true, false, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Simple_Event_002()
{
    InnerTest("不可点击对象事件测试 ", false, false, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Drag_Event_001()
{
    InnerTest("可点击可拖拽dragparent测试 ", true, true, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Drag_Event_002()
{
    InnerTest("可点击可拖拽非dragparent测试 ", true, true, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Drag_Event_003()
{
    InnerTest("不可点击可拖拽测试 ", false, true, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_001()
{
    positionY_ = 0;
    InnerBubbleTest("可点击有监听事件不消费冒泡测试 ", true, true, true, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_002()
{
    InnerBubbleTest("可点击有监听事件消费冒泡测试 ", true, true, true, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_003()
{
    InnerBubbleTest("可点击无监听事件不消费冒泡测试 ", true, true, false, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_004()
{
    InnerBubbleTest("不可点击有监听事件消费冒泡测试 ", false, false, true, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_005()
{
    InnerBubbleDragTest("子父可拖拽有监听事件不消费冒泡测试 ", true, true, true, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_006()
{
    InnerBubbleDragTest("子父可拖拽有监听事件消费冒泡测试 ", true, true, true, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_007()
{
    InnerBubbleDragTest("子父可拖拽无监听事件消费冒泡测试 ", true, true, false, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_008()
{
    InnerBubbleDragTest("子父不可拖拽有监听事件消费冒泡测试 ", false, false, true, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_009()
{
    InnerBubbleDragTest("子不可拖拽父可拖拽有监听事件消费冒泡测试 ", false, true, true, true);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Bubble_010()
{
    InnerBubbleDragTest("子不可拖拽父可拖拽有监听事件不消费冒泡测试 ", false, true, true, false);
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_Key_Event_001()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        // 2: half of screen width;
        uiViewGroup->SetPosition(
            TEXT_DISTANCE_TO_LEFT_SIDE, positionY_,
            (Screen::GetInstance().GetWidth() / 2 - TEXT_DISTANCE_TO_LEFT_SIDE), // 2: half of screen width;
            128);                                                                // 128: height
        container_->Add(uiViewGroup);

        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 2: half of screen width;
        label->SetPosition(0, 0, Screen::GetInstance().GetWidth() / 2, TEXT_H);
        label->SetText("物理按键事件测试 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ += (TEXT_H + GAP);
        UILabel* label1 = new UILabel();
        uiViewGroup->Add(label1);
        label1->SetPosition(0, TEXT_H + GAP, TEXT_W, TEXT_H);
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        if (keyListener_ == nullptr) {
            keyListener_ = new TestKeyInputListener(label1);
        }
        RootView::GetInstance()->SetOnKeyActListener(keyListener_);
        positionY_ += ITEM_H;
    }
}

void UITestInputEvent::UIKit_Pointer_Input_Test_Dispatch_InVisible_Event_001()
{
    if (container_ != nullptr) {
        UIViewGroup* uiViewGroup = new UIViewGroup();
        // 2: half of screen width; 36: decrease x-coordinate; 90: y-coordinate
        uiViewGroup->SetPosition(TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, 480, 128); // 480: width; 128: height
        container_->Add(uiViewGroup);
        UILabel* label = new UILabel();
        uiViewGroup->Add(label);
        // 2: half of screen width;
        label->SetPosition(0, 0, Screen::GetInstance().GetWidth() / 2, TEXT_H);
        label->SetText("不可见对象事件传递测试 ");
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ = 0;
        positionY_ += (TEXT_H + GAP * 2); // 2: double GAP
        UIViewGroup* group1 = new UIViewGroup();
        uiViewGroup->Add(group1);
        // 20: increase width 20: increase  height
        group1->SetPosition(positionX_, positionY_, TEST_VIEW_W + 20, TEST_VIEW_H + 20);
        group1->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        group1->SetVisible(false);
        TestView* testView = new TestView();
        group1->Add(testView);
        testView->SetPosition(5, 5, TEST_VIEW_W, TEST_VIEW_H); // 5: position x 5:position y
        testView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
        testView->SetTouchable(true);
        testView->SetSentence("Click From test 0!");
        UIViewGroup* group2 = new UIViewGroup();
        uiViewGroup->Add(group2);
        // 20: increase width 20: increase  height
        group2->SetPosition(0, positionY_, TEST_VIEW_W + 20, TEST_VIEW_H + 20);
        group2->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
        TestView* testView1 = new TestView();
        group2->Add(testView1);
        testView1->SetPosition(5, 5, TEST_VIEW_W, TEST_VIEW_H); // 5: position x 5:position y
        testView1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        testView1->SetTouchable(true);
        testView1->SetSentence("Click From test 1!");
        UILabel* label1 = new UILabel();
        uiViewGroup->Add(label1);
        // 50: increase width; 2:double GAP
        label1->SetPosition(positionX_ + 50, positionY_ + 2 * GAP, TEXT_W, TEXT_H);
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        testView->SetLabel(label1);
        testView1->SetLabel(label1);
        positionY_ += ITEM_H;
    }
}

void UITestInputEvent::InnerTest(const char* title, bool touchable, bool draggable, bool dragParent)
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 2: half Screen width
        label->SetPosition(positionX_, positionY_, Screen::GetInstance().GetWidth() / 2, TEXT_H);
        label->SetText(title);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ += (TEXT_H + GAP);
        auto testView = new TestView();
        container_->Add(testView);
        testView->SetPosition(positionX_, positionY_, TEST_VIEW_W, TEST_VIEW_H);
        testView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
        testView->SetTouchable(touchable);
        testView->SetDraggable(draggable);
        testView->SetDragParentInstead(dragParent);
        UILabel* label1 = new UILabel();
        container_->Add(label1);
        label1->SetPosition(positionX_ + TEST_VIEW_GAP, positionY_ + 2 * GAP, TEXT_W, TEXT_H); // 2: double GAP
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        testView->SetLabel(label1);
        positionY_ += ITEM_H;
    }
}

void UITestInputEvent::InnerBubbleTest(const char* title,
                                       bool touchable,
                                       bool draggable,
                                       bool hasListener,
                                       bool isBubble)
{
    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        label->SetPosition((Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE), // 2: half screen width
                           positionY_,
                           (Screen::GetInstance().GetWidth() / 2 - TEXT_DISTANCE_TO_LEFT_SIDE), // 2: half screen width
                           TEXT_H);
        label->SetText(title);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ += (TEXT_H + GAP);

        OHOS::UIScrollView* parentContainer = new UIScrollView();
        // 2: half screen width
        parentContainer->SetPosition(Screen::GetInstance().GetWidth() / 2 + TEXT_DISTANCE_TO_LEFT_SIDE, positionY_,
                                     Screen::GetInstance().GetWidth() - TEXT_DISTANCE_TO_LEFT_SIDE, ITEM_H);
        container_->Add(parentContainer);
        TestView* testView = new TestView();
        parentContainer->Add(testView);
        testView->SetPosition(0, 0, TEST_VIEW_W, TEST_VIEW_H);
        testView->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        testView->SetTouchable(touchable);
        testView->SetDraggable(draggable);
        testView->SetDragParentInstead(true);
        UILabel* label1 = new UILabel();
        parentContainer->Add(label1);
        label1->SetPosition(positionX_ + TEST_VIEW_GAP, 2 * GAP, TEXT_W, TEXT_H); // 2: tow gap
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        testView->SetLabel(label1);

        UILabel* label2 = new UILabel();
        parentContainer->Add(label2);
        label2->SetPosition(positionX_ + 2 * TEST_VIEW_GAP, 2 * GAP, TEXT_W, TEXT_H); // 2: tow gap
        label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        testView->SetLabel2(label2);

        if (hasListener) {
            UIView::OnClickListener* clickListenerParent =
                new TestOnClickListener(label2, const_cast<char*>("l-parent click"), isBubble);
            UIView::OnClickListener* clickListenerChild =
                new TestOnClickListener(label1, const_cast<char*>("l-click"), isBubble);
            testView->SetOnClickListener(clickListenerChild);
            parentContainer->SetOnClickListener(clickListenerParent);

            UIView::OnLongPressListener* longTouchListenerParent =
                new TestOnLongPressListener(label2, const_cast<char*>("l-parent long press"), isBubble);
            UIView::OnLongPressListener* longTouchListenerChild =
                new TestOnLongPressListener(label1, const_cast<char*>("l-long press"), isBubble);
            testView->SetOnLongPressListener(longTouchListenerChild);
            parentContainer->SetOnLongPressListener(longTouchListenerParent);

            UIView::OnTouchListener* touchListenerParent = new TestOnTouchListener(
                label2, const_cast<char*>("l-parent press"), const_cast<char*>("l-parent release"),
                const_cast<char*>("l-parent cancel"), isBubble);
            UIView::OnTouchListener* touchListenerChild =
                new TestOnTouchListener(label1, const_cast<char*>("l-press"), const_cast<char*>("l-release"),
                                        const_cast<char*>("l-cancel"), isBubble);
            testView->SetOnTouchListener(touchListenerChild);
            parentContainer->SetOnTouchListener(touchListenerParent);
        }

        positionY_ += ITEM_H;
    }
}

void UITestInputEvent::InnerBubbleDragTest(const char* title,
                                           bool childDraggable,
                                           bool parentDraggable,
                                           bool hasListener,
                                           bool isBubble)
{
    int32_t itemH1 = ITEM_H * 2; // 2 times of ITEM_H
    int32_t itemH2 = itemH1 + ITEM_H;
    int32_t itemH3 = itemH2 + ITEM_H;
    int32_t halfScreenWith = Screen::GetInstance().GetWidth() / 2; // 2: half screen width
    int32_t offset = 30;                                           // 40 pixel offset

    if (container_ != nullptr) {
        UILabel* label = new UILabel();
        container_->Add(label);
        // 2: half screen width
        label->SetPosition(halfScreenWith + TEXT_DISTANCE_TO_LEFT_SIDE, positionY_,
                           halfScreenWith - TEXT_DISTANCE_TO_LEFT_SIDE, TEXT_H);
        label->SetText(title);
        label->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        positionY_ += (TEXT_H + GAP);

        OHOS::TestUIScrollView* parentScroll = new TestUIScrollView();
        parentScroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
        parentScroll->SetPosition(halfScreenWith + TEXT_DISTANCE_TO_LEFT_SIDE, positionY_, itemH1, itemH1);
        parentScroll->SetThrowDrag(parentDraggable);
        parentScroll->SetDraggable(parentDraggable);
        container_->Add(parentScroll);

        OHOS::TestUIScrollView* childScroll = new TestUIScrollView();
        childScroll->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
        childScroll->SetPosition(offset, offset, itemH2, itemH2);
        childScroll->SetThrowDrag(childDraggable);
        childScroll->SetDraggable(childDraggable);
        parentScroll->Add(childScroll);

        UILabelButton* button1 = new UILabelButton();
        button1->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
        button1->SetText("button1");
        button1->SetPosition(offset, offset, itemH3 * 2, itemH3); // 2: tow width
        childScroll->Add(button1);

        UILabel* label1 = new UILabel();
        container_->Add(label1);
        label1->SetPosition(itemH3 + offset + halfScreenWith, positionY_ + 2 * GAP, TEXT_W, TEXT_H); // 2: tow gap
        label1->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        childScroll->SetLabel(label1);

        UILabel* label2 = new UILabel();
        container_->Add(label2);
        label2->SetPosition(itemH3 + offset + halfScreenWith, positionY_ + 6 * GAP, TEXT_W, TEXT_H); // 6: tow gap
        label2->SetFont(DEFAULT_VECTOR_FONT_FILENAME, FONT_DEFAULT_SIZE);
        parentScroll->SetLabel(label2);

        if (hasListener) {
            UIView::OnClickListener* clickListenerParent =
                new TestOnClickListener(label2, const_cast<char*>("l-parent click"), isBubble);
            UIView::OnClickListener* clickListenerChild =
                new TestOnClickListener(label1, const_cast<char*>("l-click"), isBubble);
            childScroll->SetOnClickListener(clickListenerChild);
            parentScroll->SetOnClickListener(clickListenerParent);

            UIView::OnLongPressListener* longTouchListenerParent =
                new TestOnLongPressListener(label2, const_cast<char*>("l-parent long press"), isBubble);
            UIView::OnLongPressListener* longTouchListenerChild =
                new TestOnLongPressListener(label1, const_cast<char*>("l-long press"), isBubble);
            childScroll->SetOnLongPressListener(longTouchListenerChild);
            parentScroll->SetOnLongPressListener(longTouchListenerParent);

            UIView::OnTouchListener* touchListenerParent = new TestOnTouchListener(
                label2, const_cast<char*>("l-parent press"), const_cast<char*>("l-parent release"),
                const_cast<char*>("l-parent cancel"), isBubble);
            UIView::OnTouchListener* touchListenerChild =
                new TestOnTouchListener(label1, const_cast<char*>("l-press"), const_cast<char*>("l-release"),
                                        const_cast<char*>("l-cancel"), isBubble);
            childScroll->SetOnTouchListener(touchListenerChild);
            parentScroll->SetOnTouchListener(touchListenerParent);

            UIView::OnDragListener* dragListenerParent = new TestOnDragListener(
                label2, const_cast<char*>("l-dragStart parent"), const_cast<char*>("l-drag parent"),
                const_cast<char*>("l-dragEnd parent"), isBubble);
            UIView::OnDragListener* dragListenerChild =
                new TestOnDragListener(label1, const_cast<char*>("l-dragStart"), const_cast<char*>("l-drag"),
                                       const_cast<char*>("l-dragEnd"), isBubble);
            childScroll->SetOnDragListener(dragListenerChild);
            parentScroll->SetOnDragListener(dragListenerParent);
        }
        positionY_ += itemH1;
    }
}
} // namespace OHOS
