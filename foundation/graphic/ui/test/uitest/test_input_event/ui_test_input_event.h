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

#ifndef UI_TEST_INPUT_EVENT_H
#define UI_TEST_INPUT_EVENT_H

#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "dock/input_device.h"
#include "ui_test.h"

namespace OHOS {
class TestKeyInputListener : public RootView::OnKeyActListener {
public:
    explicit TestKeyInputListener(UILabel* label) : label_(label) {}
    virtual ~TestKeyInputListener() {}
    bool OnKeyAct(UIView& view, const KeyEvent& event) override
    {
        if (label_ == nullptr) {
            return true;
        }
        switch (event.GetState()) {
            case InputDevice::STATE_PRESS:
                label_->SetText("key pressed!");
                break;
            case InputDevice::STATE_RELEASE:
                label_->SetText("key released!");
                break;
            default:
                label_->SetText("");
                break;
        }
        label_->Invalidate();
        return true;
    }

private:
    UILabel* label_;
};

class TestOnClickListener : public UIView::OnClickListener {
public:
    explicit TestOnClickListener(UILabel* label, char* sentence, bool isConsume)
        : label_(label), sentence_(sentence), isConsume_(isConsume)
    {
    }
    virtual ~TestOnClickListener() {}
    virtual bool OnClick(UIView& view, const ClickEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(sentence_);
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    char* sentence_;
    bool isConsume_;
};

class TestOnLongPressListener : public UIView::OnLongPressListener {
public:
    explicit TestOnLongPressListener(UILabel* label, char* sentence, bool isConsume)
        : label_(label), sentence_(sentence), isConsume_(isConsume)
    {
    }
    virtual ~TestOnLongPressListener() {}
    virtual bool OnLongPress(UIView& view, const LongPressEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(sentence_);
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    char* sentence_;
    bool isConsume_;
};

class TestOnTouchListener : public UIView::OnTouchListener {
public:
    explicit TestOnTouchListener(UILabel* label, char* strPress, char* strRelease, char* strCancel, bool isConsume)
        : label_(label), strPress_(strPress), strRelease_(strRelease), strCancel_(strCancel), isConsume_(isConsume)
    {
    }
    virtual ~TestOnTouchListener() {}
    virtual bool OnPress(UIView& view, const PressEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strPress_);
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnRelease(UIView& view, const ReleaseEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strRelease_);
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnCancel(UIView& view, const CancelEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strCancel_);
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    char* strPress_;
    char* strRelease_;
    char* strCancel_;
    bool isConsume_;
};

class TestOnDragListener : public UIView::OnDragListener {
public:
    explicit TestOnDragListener(UILabel* label, char* strDragStart, char* strDrag, char* strDragEnd, bool isConsume)
        : label_(label), strDragStart_(strDragStart), strDrag_(strDrag), strDragEnd_(strDragEnd), isConsume_(isConsume)
    {
    }
    virtual ~TestOnDragListener() {}
    virtual bool OnDragStart(UIView& view, const DragEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strDragStart_);
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnDrag(UIView& view, const DragEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strDrag_);
            label_->Invalidate();
        }
        return isConsume_;
    }

    virtual bool OnDragEnd(UIView& view, const DragEvent& event)
    {
        if (label_ != nullptr) {
            label_->SetText(strDragEnd_);
            label_->Invalidate();
        }
        return isConsume_;
    }

private:
    UILabel* label_;
    char* strDragStart_;
    char* strDrag_;
    char* strDragEnd_;
    bool isConsume_;
};

class UITestInputEvent : public UITest {
public:
    UITestInputEvent() {}
    ~UITestInputEvent() {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    /**
     * @brief Test if dispatch press\release\longpress\cancel event act normal when target is touchable
     */
    void UIKit_Pointer_Input_Test_Dispatch_Simple_Event_001();
    /**
     * @brief Test if dispatch press\release\longpress\cancel\drag event act normal when target is untouchable
     */
    void UIKit_Pointer_Input_Test_Dispatch_Simple_Event_002();
    /**
     * @brief Test if dispatch drag event act normal when target is touchable and draggable and drag parent instead.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Drag_Event_001();
    /**
     * @brief Test if dispatch drag event act normal when target is touchable and draggable and not drag parent instead.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Drag_Event_002();
    /**
     * @brief Test if dispatch drag event act normal when target is untouchable but draggable.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Drag_Event_003();
    /**
     * @brief Test if dispatch drag event act normal when target is untouchable but draggable.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Key_Event_001();
    /**
     * @brief Test if dispatch drag event act normal when target is untouchable but draggable.
     */
    void UIKit_Pointer_Input_Test_Dispatch_InVisible_Event_001();
    /**
     * @brief Test click, release or longClick event bubble act normal when both of parent and child is triggered.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_001();
    /**
     * @brief Test click, release or longClick event bubble act normal when child is triggered but and parent not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_002();
    /**
     * @brief Test click, release or longClick event bubble act normal when child is triggered but and parent not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_003();
    /**
     * @brief Test click, release or longClick event bubble act normal when parent is triggered but and child not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_004();
    /**
     * @brief Test drag event bubble act normal when both of parent and child is triggered.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_005();
    /**
     * @brief Test drag event bubble act normal when child is triggered but and parent not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_006();
    /**
     * @brief Test drag event bubble act normal when child is triggered but and parent not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_007();
    /**
     * @brief Test drag event bubble act normal when both of parent and child is not triggered.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_008();
    /**
     * @brief Test drag event bubble act normal when parent is triggered but and child not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_009();
    /**
     * @brief Test drag event bubble act normal when parent is triggered but and child not.
     */
    void UIKit_Pointer_Input_Test_Dispatch_Bubble_010();

private:
    UIScrollView* container_ = nullptr;
    TestKeyInputListener* keyListener_ = nullptr;
    void InnerTest(const char* title, bool touchable, bool draggable, bool dragParent);
    void InnerBubbleTest(const char* title, bool touchable, bool draggable, bool hasListener, bool isBubble);
    void InnerBubbleDragTest(const char* title,
                             bool childDraggable,
                             bool parentDraggable,
                             bool hasListener,
                             bool isBubble);
};
} // namespace OHOS
#endif // UI_TEST_INPUT_EVENT_H
