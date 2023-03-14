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

#if ENABLE_DEBUG
#include <climits>
#include <gtest/gtest.h>
#include <unistd.h>

#include "common/graphic_startup.h"
#include "common/input_device_manager.h"
#include "common/screen.h"
#include "common/task_manager.h"
#include "components/root_view.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "core/render_manager.h"
#include "dock/screen_device_proxy.h"
#include "font/ui_font.h"
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "graphic_config.h"
#include "imgdecode/cache_manager.h"
#include "layout/grid_layout.h"

using namespace testing::ext;

enum TestEventFlag { FLAG0, FLAG1 };

namespace {
uint8_t REGISTER_POINT_FLAG = FLAG0;
uint8_t REGISTER_KEY_FLAG = FLAG0;
uint8_t CLICK_FLAG = FLAG0;
uint8_t LONG_PRESS_FLAG = FLAG0;
uint8_t PRESS_FLAG = FLAG0;
uint8_t DRAG_FLAG = FLAG0;
uint8_t KEY_FLAG = FLAG0;
} // namespace

namespace OHOS {
class TestEventBubbleView : public UIView, public RootView::OnKeyActListener {
public:
    bool OnLongPressEvent(const LongPressEvent& event) override
    {
        longPressEventFlag_ = FLAG1;
        return true;
    }

    bool OnClickEvent(const ClickEvent& event) override
    {
        clickEventFlag_ = FLAG1;
        return true;
    }

    bool OnDragEvent(const DragEvent& event) override
    {
        dragEventFlag_ = FLAG1;
        return true;
    }

    bool OnKeyAct(UIView& view, const KeyEvent& event) override
    {
        KEY_FLAG = FLAG1;
        return true;
    }

private:
    bool longPressEventFlag_;
    bool clickEventFlag_;
    bool dragEventFlag_;
};

class EventBubbleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static void TestApp();
    static void SetUpTestview(TestEventBubbleView* testView, bool touchable, bool draggable);
    static void DeleteChildren(UIView* view);

    static RootView* rootView_;
    static GridLayout* layout_;
    static TestEventBubbleView* clickView_;
    static TestEventBubbleView* dragView_;
    static TestEventBubbleView* longPressView_;
    static TestEventBubbleView* keyView_;
    static TestEventBubbleView* unTouchView_;
};

class TestOnClickListener : public UIView::OnClickListener {
public:
    explicit TestOnClickListener(bool isConsume) : isConsume_(isConsume) {}
    virtual ~TestOnClickListener() {}
    virtual bool OnClick(UIView& view, const ClickEvent& event)
    {
        CLICK_FLAG = FLAG1;
        return isConsume_;
    }

private:
    bool isConsume_;
};

class TestOnLongPressListener : public UIView::OnLongPressListener {
public:
    explicit TestOnLongPressListener(bool isConsume) : isConsume_(isConsume) {}
    virtual ~TestOnLongPressListener() {}
    virtual bool OnLongPress(UIView& view, const LongPressEvent& event)
    {
        LONG_PRESS_FLAG = FLAG1;
        return isConsume_;
    }

private:
    bool isConsume_;
};

class TestOnTouchListener : public UIView::OnTouchListener {
public:
    explicit TestOnTouchListener(bool isConsume) : isConsume_(isConsume) {}
    virtual ~TestOnTouchListener() {}
    virtual bool OnPress(UIView& view, const PressEvent& event)
    {
        PRESS_FLAG = FLAG1;
        return isConsume_;
    }

private:
    bool isConsume_;
};

class TestOnDragListener : public UIView::OnDragListener {
public:
    explicit TestOnDragListener(bool isConsume) : isConsume_(isConsume) {}
    virtual ~TestOnDragListener() {}
    virtual bool OnDrag(DragEvent& event)
    {
        DRAG_FLAG = FLAG1;
        return isConsume_;
    }

private:
    bool isConsume_;
};

RootView* EventBubbleTest::rootView_ = nullptr;
GridLayout* EventBubbleTest::layout_ = nullptr;
TestEventBubbleView* EventBubbleTest::clickView_ = nullptr;
TestEventBubbleView* EventBubbleTest::dragView_ = nullptr;
TestEventBubbleView* EventBubbleTest::longPressView_ = nullptr;
TestEventBubbleView* EventBubbleTest::keyView_ = nullptr;
TestEventBubbleView* EventBubbleTest::unTouchView_ = nullptr;


void EventBubbleTest::SetUpTestCase(void)
{
    GraphicStartUp::Init();
    TestApp();
}

void EventBubbleTest::TestApp()
{
    rootView_ = RootView::GetInstance();
    rootView_->SetTouchable(true);
    rootView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    layout_ = new GridLayout();
    layout_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rootView_->Add(layout_);
    layout_->SetLayoutDirection(LAYOUT_VER);
    layout_->SetRows(6); /* 6:rows */
    layout_->SetCols(1);

    clickView_ = new TestEventBubbleView();
    SetUpTestview(clickView_, true, false);

    longPressView_ = new TestEventBubbleView();
    SetUpTestview(longPressView_, true, false);

    dragView_ = new TestEventBubbleView();
    SetUpTestview(dragView_, true, true);

    unTouchView_ = new TestEventBubbleView();
    SetUpTestview(unTouchView_, false, false);

    keyView_ = new TestEventBubbleView();
    RootView::GetInstance()->SetOnKeyActListener(keyView_);
    SetUpTestview(keyView_, true, false);

    layout_->LayoutChildren();
    rootView_->Invalidate();
}

void EventBubbleTest::SetUpTestview(TestEventBubbleView* testView, bool touchable, bool draggable)
{
    layout_->Add(testView);
    testView->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION / 7); /* 7:ratio */
    testView->SetTouchable(touchable);
    testView->SetDraggable(draggable);
}

void EventBubbleTest::DeleteChildren(UIView* view)
{
    if (view == nullptr) {
        return;
    }
    while (view != nullptr) {
        UIView* tempView = view;
        view = view->GetNextSibling();
        if (tempView->IsViewGroup()) {
            DeleteChildren(static_cast<UIViewGroup*>(tempView)->GetChildrenHead());
        }
        if (tempView->GetParent()) {
            static_cast<UIViewGroup*>(tempView->GetParent())->Remove(tempView);
        }
        delete tempView;
    }
}

void EventBubbleTest::TearDownTestCase(void)
{
    DeleteChildren(layout_);
    layout_ = nullptr;
    RootView::GetInstance()->ClearOnKeyActListener();
}

/**
 * @tc.name: Graphic_EventBubbleTest_Test_GetExtraMsg_SetExtraMsg_007
 * @tc.desc: Verify GetExtraMsg, SetExtraMsg function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5C
 */
HWTEST_F(EventBubbleTest, Graphic_EventBubbleTest_Test_GetExtraMsg_SetExtraMsg_007, TestSize.Level0)
{
    /* test for GetExtraMsg */
    TestEventBubbleView* view = new TestEventBubbleView();
    EXPECT_EQ(view->GetExtraMsg(), nullptr);

    /* test for SetExtraMsg */
    UIView::ViewExtraMsg* extraMsg = new UIView::ViewExtraMsg();
    extraMsg->elementPtr = malloc(sizeof(char));
    view->SetExtraMsg(extraMsg);
    EXPECT_EQ(view->GetExtraMsg()->elementPtr, extraMsg->elementPtr);
    free(extraMsg->elementPtr);
    delete extraMsg;
    delete view;
}

/**
 * @tc.name: Graphic_EventBubbleTest_Test_UIView_GetTargetView_008
 * @tc.desc: Verify UIView::GetTargetView equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5C
 */
HWTEST_F(EventBubbleTest, Graphic_EventBubbleTest_Test_UIViewGroup_GetTargetView_008, TestSize.Level0)
{
    if ((clickView_ == nullptr) || (rootView_ == nullptr)) {
        return;
    }
    /* test for touchable view */
    /* 2:ratio, 2:ratio */
    Point clickPoint = {(int16_t)(clickView_->GetRect().GetX() + clickView_->GetWidth() / 2),
                        (int16_t)(clickView_->GetRect().GetY() + clickView_->GetHeight() / 2)};
    UIView* curView = nullptr;
    UIView* targetView = nullptr;
    rootView_->GetTargetView(clickPoint, &curView, &targetView);
    EXPECT_EQ(curView, clickView_);
    EXPECT_EQ(targetView, clickView_);

    /* test for unTouchable view */
    if (unTouchView_ == nullptr) {
        return;
    }
    /* 2:ratio, 2:ratio */
    Point clickPoint2 = {(int16_t)(unTouchView_->GetRect().GetX() + unTouchView_->GetWidth() / 2),
                        (int16_t)(unTouchView_->GetRect().GetY() + unTouchView_->GetHeight() / 2)};
    curView = nullptr;
    targetView = nullptr;
    rootView_->GetTargetView(clickPoint2, &curView, &targetView);

    EXPECT_EQ(curView, rootView_);
    EXPECT_EQ(targetView, unTouchView_);
}

/**
 * @tc.name: Graphic_EventBubbleTest_Test_UIView_GetTargetView_009
 * @tc.desc: Verify UIView::GetTargetView equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5C
 */
HWTEST_F(EventBubbleTest, Graphic_EventBubbleTest_Test_UIView_GetTargetView_009, TestSize.Level1)
{
    if ((clickView_ == nullptr) || (rootView_ == nullptr)) {
        return;
    }
    /* test for touchable view */
    /* 2:ratio, 2:ratio */
    Point clickPoint = {(int16_t)(clickView_->GetRect().GetX() + clickView_->GetWidth() / 2),
                        (int16_t)(clickView_->GetRect().GetY() + clickView_->GetHeight() / 2)};
    UIView* curView = nullptr;
    UIView* targetView = nullptr;
    clickView_->GetTargetView(clickPoint, &curView, &targetView);
    EXPECT_EQ(curView, clickView_);
    EXPECT_EQ(targetView, clickView_);

    /* test for unTouchable view */
    if (unTouchView_ == nullptr) {
        return;
    }
    /* 2:ratio, 2:ratio */
    Point clickPoint2 = {(int16_t)(unTouchView_->GetRect().GetX() + unTouchView_->GetWidth() / 2),
                        (int16_t)(unTouchView_->GetRect().GetY() + unTouchView_->GetHeight() / 2)};
    curView = nullptr;
    targetView = nullptr;
    unTouchView_->GetTargetView(clickPoint2, &curView, &targetView);

    EXPECT_EQ(curView, nullptr);
    EXPECT_EQ(targetView, unTouchView_);
}

/**
 * @tc.name: Graphic_EventBubbleTest_Test_UIView_OnClick_010
 * @tc.desc: Verify UIView::OnClick equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEA
 */
HWTEST_F(EventBubbleTest, Graphic_EventBubbleTest_Test_UIView_OnClick_010, TestSize.Level0)
{
    /* 2:ratio, 2:ratio */
    Point clickPoint = {(int16_t)(clickView_->GetRect().GetX() + clickView_->GetWidth() / 2),
                        (int16_t)(clickView_->GetRect().GetY() + clickView_->GetHeight() / 2)};
    ClickEvent clickEvent(clickPoint);

    /* test for not comsumed view */
    TestOnClickListener* clickListener = new TestOnClickListener(false);
    bool ret = clickListener->OnClick(*clickView_, clickEvent);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(CLICK_FLAG, true);
    delete clickListener;

    /* test for comsumed view */
    CLICK_FLAG = false;
    TestOnClickListener* clickListener2 = new TestOnClickListener(true);
    ret = clickListener->OnClick(*clickView_, clickEvent);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(CLICK_FLAG, true);
    delete clickListener2;
}

/**
 * @tc.name: Graphic_EventBubbleTest_Test_UIView_OnLongPress_011
 * @tc.desc: Verify UIView::OnLongPress equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5C
 */
HWTEST_F(EventBubbleTest, Graphic_EventBubbleTest_Test_UIView_OnLongPress_011, TestSize.Level0)
{
    /* 2:ratio, 2:ratio */
    Point longPressPoint = {(int16_t)(longPressView_->GetRect().GetX() + longPressView_->GetWidth() / 2),
                        (int16_t)(longPressView_->GetRect().GetY() + longPressView_->GetHeight() / 2)};
    uint32_t pressTimeStamp = 2; /* 2:second */
    LongPressEvent longPressEvent(longPressPoint, pressTimeStamp);

    /* test for not comsumed view */
    TestOnLongPressListener* longPressListener = new TestOnLongPressListener(false);
    bool ret = longPressListener->OnLongPress(*clickView_, longPressEvent);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(LONG_PRESS_FLAG, true);
    delete longPressListener;

    /* test for comsumed view */
    LONG_PRESS_FLAG = false;
    TestOnLongPressListener* longPressListener2 = new TestOnLongPressListener(true);
    ret = longPressListener2->OnLongPress(*clickView_, longPressEvent);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(LONG_PRESS_FLAG, true);
    delete longPressListener2;
}

/**
 * @tc.name: Graphic_EventBubbleTest_Test_UIView_OnDrag_012
 * @tc.desc: Verify UIView::OnDrag equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5C
 */
HWTEST_F(EventBubbleTest, Graphic_EventBubbleTest_Test_UIView_OnDrag_012, TestSize.Level0)
{
    /* 2:ratio, 2:ratio */
    Point dragCurPoint = {(int16_t)(dragView_->GetRect().GetX() + dragView_->GetWidth() / 2),
                        (int16_t)(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    /* 3:ratio, 3:ratio */
    Point dragLastPoint = {(int16_t)(dragView_->GetRect().GetX() + dragView_->GetWidth() / 3),
                        (int16_t)(dragView_->GetRect().GetY() + dragView_->GetHeight() / 3)};
    Point dragLen;
    dragLen.x = dragCurPoint.x - dragLastPoint.x;
    dragLen.y = dragCurPoint.y - dragLastPoint.y;
    DragEvent dragEvent(dragCurPoint, dragLastPoint, dragLen);

    /* test for not comsumed view */
    TestOnDragListener* dragListener = new TestOnDragListener(false);
    bool ret = dragListener->OnDrag(dragEvent);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(DRAG_FLAG, true);
    delete dragListener;

    /* test for comsumed view */
    LONG_PRESS_FLAG = false;
    TestOnDragListener* dragListener2 = new TestOnDragListener(true);
    ret = dragListener2->OnDrag(dragEvent);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(DRAG_FLAG, true);
    delete dragListener2;
}
} // namespace OHOS
#endif // ENABLE_DEBUG
