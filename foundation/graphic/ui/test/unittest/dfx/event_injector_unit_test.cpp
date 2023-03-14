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

#include "dfx/event_injector.h"

#if ENABLE_DEBUG
#include <climits>
#include <gtest/gtest.h>
#include <pthread.h>
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
#include "window/window.h"

using namespace testing::ext;

enum TestEventInjectorFlag { FLAG0, FLAG1 };

namespace {
uint8_t REGISTER_POINT_FLAG = FLAG0;
uint8_t REGISTER_KEY_FLAG = FLAG0;
uint8_t UNREGISTER_POINT_FLAG = FLAG0;
uint8_t UNREGISTER_KEY_FLAG = FLAG0;
uint8_t CLICK_FLAG = FLAG0;
uint8_t LONG_PRESS_FLAG = FLAG0;
uint8_t DRAG_FLAG = FLAG0;
uint8_t KEY_FLAG = FLAG0;
uint8_t MAX_LOOP = 200;
} // namespace

namespace OHOS {
class TestEventInjectorView : public UIView, public RootView::OnKeyActListener {
public:
    bool OnLongPressEvent(const LongPressEvent& event) override
    {
        LONG_PRESS_FLAG = FLAG1;
        return true;
    }

    bool OnClickEvent(const ClickEvent& event) override
    {
        CLICK_FLAG = FLAG1;
        return true;
    }

    bool OnDragEvent(const DragEvent& event) override
    {
        DRAG_FLAG = FLAG1;
        return true;
    }

    bool OnKeyAct(UIView& view, const KeyEvent& event) override
    {
        KEY_FLAG = FLAG1;
        return true;
    }
};

class EventInjectorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static void TestApp();
    static void SetUpTestview(TestEventInjectorView* testView, bool touchable, bool draggable);
    static void* MainTask(void* args);
    static void DeleteChildren(UIView* view);

    static pthread_t mainTaskThread_;
    static bool isRepeat_;
    static RootView* rootView_;
    static GridLayout* layout_;
    static TestEventInjectorView* clickView_;
    static TestEventInjectorView* dragView_;
    static TestEventInjectorView* longPressView_;
    static TestEventInjectorView* keyView_;
    static Window* window_;
};

pthread_t EventInjectorTest::mainTaskThread_ = -1;
bool EventInjectorTest::isRepeat_ = true;
RootView* EventInjectorTest::rootView_ = nullptr;
GridLayout* EventInjectorTest::layout_ = nullptr;
TestEventInjectorView* EventInjectorTest::clickView_ = nullptr;
TestEventInjectorView* EventInjectorTest::dragView_ = nullptr;
TestEventInjectorView* EventInjectorTest::longPressView_ = nullptr;
TestEventInjectorView* EventInjectorTest::keyView_ = nullptr;
Window* EventInjectorTest::window_ = nullptr;

void EventInjectorTest::SetUpTestCase(void)
{
    GraphicStartUp::Init();
    TestApp();
    if (pthread_create(&mainTaskThread_, nullptr, MainTask, nullptr) != 0) {
        return;
    }
}

void* EventInjectorTest::MainTask(void* args)
{
    while (isRepeat_) {
        /* Periodically call TaskHandler(). It could be done in a timer interrupt or an OS task too. */
        OHOS::TaskManager::GetInstance()->TaskHandler();
        usleep(1000 * 10); /* 1000 * 10:10ms Just to let the system breathe */
    }
    return nullptr;
}

void EventInjectorTest::TestApp()
{
    WindowConfig config = {};
    config.rect.SetRect(0, 0, Screen::GetInstance().GetWidth() - 1, Screen::GetInstance().GetHeight() - 1);
    window_ = Window::CreateWindow(config);
    if (window_ == nullptr) {
        GRAPHIC_LOGE("Create window false!");
        return;
    }
    window_->BindRootView(RootView::GetInstance());

    if (EventInjector::GetInstance()->RegisterEventInjector(EventDataType::POINT_TYPE)) {
        REGISTER_POINT_FLAG = FLAG1;
    }
    if (EventInjector::GetInstance()->RegisterEventInjector(EventDataType::KEY_TYPE)) {
        REGISTER_KEY_FLAG = FLAG1;
    }
    EventInjector::GetInstance()->SetWindowId(window_->GetWindowId());
    rootView_ = RootView::GetInstance();
    rootView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    layout_ = new GridLayout();
    layout_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rootView_->Add(layout_);
    layout_->SetLayoutDirection(LAYOUT_VER);
    layout_->SetRows(4); /* 4:rows */
    layout_->SetCols(1);
    clickView_ = new TestEventInjectorView();
    SetUpTestview(clickView_, true, false);

    longPressView_ = new TestEventInjectorView();
    SetUpTestview(longPressView_, true, false);

    dragView_ = new TestEventInjectorView();
    SetUpTestview(dragView_, true, true);

    keyView_ = new TestEventInjectorView();
    RootView::GetInstance()->SetOnKeyActListener(keyView_);
    SetUpTestview(keyView_, true, false);

    layout_->LayoutChildren();
    rootView_->Invalidate();
}

void EventInjectorTest::SetUpTestview(TestEventInjectorView* testView, bool touchable, bool draggable)
{
    layout_->Add(testView);
    testView->Resize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION / 5); /* 5:ratio */
    testView->SetTouchable(touchable);
    testView->SetDraggable(draggable);
}

void EventInjectorTest::DeleteChildren(UIView* view)
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

void EventInjectorTest::TearDownTestCase(void)
{
    isRepeat_ = false;
    pthread_join(mainTaskThread_, nullptr);
    Window::DestoryWindow(window_);
    DeleteChildren(layout_);
    layout_ = nullptr;

    EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::POINT_TYPE);
    EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::KEY_TYPE);
    RootView::GetInstance()->ClearOnKeyActListener();
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_RegisterEventInjector_001
 * @tc.desc: Verify RegisterEventInjector function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F74SS
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_RegisterEventInjector_001, TestSize.Level0)
{
    EXPECT_EQ(REGISTER_POINT_FLAG, FLAG1);
    EXPECT_EQ(REGISTER_KEY_FLAG, FLAG1);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetClickEvent_001
 * @tc.desc: Verify SetClickEvent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetClickEvent_001, TestSize.Level2)
{
    if (clickView_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    CLICK_FLAG = FLAG0;
    /* 2:ratio, 2:ratio */
    Point clickPoint = {static_cast<int16_t>(clickView_->GetRect().GetX() + clickView_->GetWidth() / 2),
                        static_cast<int16_t>(clickView_->GetRect().GetY() + clickView_->GetHeight() / 2)};
    bool ret = EventInjector::GetInstance()->SetClickEvent(clickPoint);
    EXPECT_EQ(ret, true);
    uint8_t loop = 0;
    while ((loop < MAX_LOOP) && !CLICK_FLAG) {
        loop++;
        usleep(10000); /* 10000:10ms */
    }
    EXPECT_EQ(CLICK_FLAG, FLAG1);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetDragEvent_001
 * @tc.desc: Verify SetDragEvent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetDragEvent_001, TestSize.Level2)
{
    if (dragView_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    DRAG_FLAG = FLAG0;
    /* 4:ratio, 2:ratio */
    Point startPoint = {static_cast<int16_t>(dragView_->GetRect().GetX() + dragView_->GetWidth() / 4),
                        static_cast<int16_t>(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    /* 2:ratio, 2:ratio */
    Point endPoint = {static_cast<int16_t>(dragView_->GetRect().GetX() + dragView_->GetWidth() / 2),
                      static_cast<int16_t>(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    bool ret = EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, 100); /* 100:drag time (ms) */
    EXPECT_EQ(ret, true);
    uint8_t loop = 0;
    while ((loop < MAX_LOOP) && !DRAG_FLAG) {
        loop++;
        usleep(10000); /* 10000:10ms */
    }
    EXPECT_EQ(DRAG_FLAG, FLAG1);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetDragEvent_002
 * @tc.desc: Verify SetDragEvent function, abnormal branch, SetDragEvent failure(dragTime is too short), equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetDragEvent_002, TestSize.Level0)
{
    if (dragView_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    /* 4:ratio, 2:ratio */
    Point startPoint = {static_cast<int16_t>(dragView_->GetRect().GetX() + dragView_->GetWidth() / 4),
                        static_cast<int16_t>(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    /* 2:ratio, 2:ratio */
    Point endPoint = {static_cast<int16_t>(dragView_->GetRect().GetX() + dragView_->GetWidth() / 2),
                      static_cast<int16_t>(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    bool ret = EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, INDEV_READ_PERIOD);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetDragEvent_003
 * @tc.desc: Verify SetDragEvent function, abnormal branch, SetDragEvent failure(dragTime is too long), equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetDragEvent_003, TestSize.Level0)
{
    if (dragView_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    /* 4:ratio, 2:ratio */
    Point startPoint = {static_cast<int16_t>(dragView_->GetRect().GetX() + dragView_->GetWidth() / 4),
                        static_cast<int16_t>(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    /* 2:ratio, 2:ratio */
    Point endPoint = {static_cast<int16_t>(dragView_->GetRect().GetX() + dragView_->GetWidth() / 2),
                      static_cast<int16_t>(dragView_->GetRect().GetY() + dragView_->GetHeight() / 2)};
    bool ret = EventInjector::GetInstance()->SetDragEvent(startPoint, endPoint, 6000); /* 6000:drag time (ms) */
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetLongPressEvent_001
 * @tc.desc: Verify SetLongPressEvent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetLongPressEvent_001, TestSize.Level2)
{
    if (longPressView_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    sleep(2); /* 2:2s */
    LONG_PRESS_FLAG = FLAG0;
    /* 2:ratio, 2:ratio */
    Point longPressPoint = {static_cast<int16_t>(longPressView_->GetRect().GetX() + longPressView_->GetWidth() / 2),
                            static_cast<int16_t>(longPressView_->GetRect().GetY() + longPressView_->GetHeight() / 2)};
    bool ret = EventInjector::GetInstance()->SetLongPressEvent(longPressPoint);
    EXPECT_EQ(ret, true);
    uint8_t loop = 0;
    while ((loop < MAX_LOOP) && !LONG_PRESS_FLAG) {
        loop++;
        usleep(10000); /* 10000:10ms */
    }
    EXPECT_EQ(LONG_PRESS_FLAG, FLAG1);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetLongPressEvent_002
 * @tc.desc: Verify SetLongPressEvent function, abnormal branch, SetLongPressEvent failure, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetLongPressEvent_002, TestSize.Level0)
{
    if (longPressView_ == nullptr) {
        ADD_FAILURE();
        return;
    }
    /* 2:ratio, 2:ratio */
    Point longPressPoint = {static_cast<int16_t>(longPressView_->GetRect().GetX() + longPressView_->GetWidth() / 2),
                            static_cast<int16_t>(longPressView_->GetRect().GetY() + longPressView_->GetHeight() / 2)};
    /* 20:loop */
    for (uint8_t i = 0; i < 20; i++) {
        EventInjector::GetInstance()->SetLongPressEvent(longPressPoint);
    }
    bool ret = EventInjector::GetInstance()->SetLongPressEvent(longPressPoint);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetKeyEvent_001
 * @tc.desc: Verify SetKeyEvent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetKeyEvent_001, TestSize.Level2)
{
    /* 26:keyId */
    uint16_t keyId = 26;
    EventInjector::GetInstance()->SetKeyEvent(keyId, InputDevice::STATE_PRESS);
    uint8_t loop = 0;
    while ((loop < MAX_LOOP) && !KEY_FLAG) {
        loop++;
        usleep(10000); /* 10000:10ms */
    }
    EXPECT_EQ(KEY_FLAG, FLAG1);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_SetKeyEvent_002
 * @tc.desc: Verify SetKeyEvent function, abnormal branch, SetKeyEvent failure, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_SetKeyEvent_002, TestSize.Level0)
{
    /* 26:keyId */
    uint16_t keyId = 26;
    /* 200:loop */
    for (uint8_t i = 0; i < 200; i++) {
        EventInjector::GetInstance()->SetKeyEvent(keyId, InputDevice::STATE_PRESS);
    }
    bool ret = EventInjector::GetInstance()->SetKeyEvent(keyId, InputDevice::STATE_PRESS);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: Graphic_EventInjectorTest_Test_UnregisterEventInjector_001
 * @tc.desc: Verify UnregisterEventInjector function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F74ST
 */
HWTEST_F(EventInjectorTest, Graphic_EventInjectorTest_Test_UnregisterEventInjector_001, TestSize.Level0)
{
    isRepeat_ = false;
    usleep(10000); /* 10000:10ms */
    EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::POINT_TYPE);
    if (!EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::POINT_TYPE)) {
        UNREGISTER_POINT_FLAG = FLAG1;
    }
    EventInjector::GetInstance()->UnregisterEventInjector(EventDataType::KEY_TYPE);
    if (!EventInjector::GetInstance()->IsEventInjectorRegistered(EventDataType::KEY_TYPE)) {
        UNREGISTER_KEY_FLAG = FLAG1;
    }
    EXPECT_EQ(UNREGISTER_POINT_FLAG, FLAG1);
    EXPECT_EQ(UNREGISTER_KEY_FLAG, FLAG1);
}
} // namespace OHOS
#endif // ENABLE_DEBUG
