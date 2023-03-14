/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "core/common/platform_window.h"
#include "core/components/test/json/json_frontend.h"
#include "core/event/ace_event_helper.h"
#include "core/event/event_trigger.h"
#include "core/event/key_event.h"
#include "core/event/key_event_recognizer.h"
#include "core/event/mouse_raw_recognizer.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"

using namespace testing::ext;
namespace OHOS::Ace {
namespace {

const std::string EVENT_ID = "1";
const std::string MOUSE_EVENT_TYPE = "onMouse";
const double MOUSE_OFFSET = 10.0;
} // namespace

using AsyncEventCallBack = std::function<void(const EventMarker&)>;
using SyncEventCallBack = std::function<void(const EventMarker&, bool&)>;

class TestEventHandler final : public AceEventHandler {
public:
    TestEventHandler() = default;
    ~TestEventHandler() = default;

    void SetAsyncEventCallBack(const AsyncEventCallBack& eventCallBack)
    {
        asyncCallBack_ = eventCallBack;
    }

    void SetSyncEventCallBack(const SyncEventCallBack& eventCallBack)
    {
        syncCallBack_ = eventCallBack;
    }

    void HandleAsyncEvent(const EventMarker& eventMarker) override
    {
        if (asyncCallBack_) {
            asyncCallBack_(eventMarker);
        }
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override
    {
        if (asyncCallBack_) {
            asyncCallBack_(eventMarker);
        }
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override
    {
        if (asyncCallBack_) {
            asyncCallBack_(eventMarker);
        }
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& info) override
    {
        if (asyncCallBack_) {
            asyncCallBack_(eventMarker);
        }
    }

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override
    {
        if (asyncCallBack_) {
            asyncCallBack_(eventMarker);
        }
    }

    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override
    {
        if (syncCallBack_) {
            syncCallBack_(eventMarker, result);
        }
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override
    {
        if (syncCallBack_) {
            syncCallBack_(eventMarker, result);
        }
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& info, bool& result) override
    {
        if (syncCallBack_) {
            syncCallBack_(eventMarker, result);
        }
    }

    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override
    {
        if (syncCallBack_) {
            bool resultTmp = false;
            syncCallBack_(eventMarker, resultTmp);
            result = resultTmp ? "true" : "false";
        }
    }

    void HandleSyncEvent(
        const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override
    {
        if (syncCallBack_) {
            bool resultTmp = false;
            syncCallBack_(eventMarker, resultTmp);
        }
    }

private:
    AsyncEventCallBack asyncCallBack_ = nullptr;
    SyncEventCallBack syncCallBack_ = nullptr;
};

class TestWindow final : public PlatformWindow {
public:
    explicit TestWindow(AceView* aceView) {}
    ~TestWindow() override = default;

    // Platform window interface
    void RequestFrame() override {}
    void RegisterVsyncCallback(AceVsyncCallback&& callback) override {}
    void SetRootRenderNode(const RefPtr<RenderNode>& root) override {}

private:
    ACE_DISALLOW_COPY_AND_MOVE(TestWindow);
};

class EventTriggerResult {
public:
    EventTriggerResult() = default;
    ~EventTriggerResult() = default;

    void SetEventMaker(const EventMarker& eventMarker)
    {
        eventMarker_ = eventMarker;
    }

    const EventMarker& GetEventMaker()
    {
        return eventMarker_;
    }

private:
    EventMarker eventMarker_;
};

class MouseEventResult {
public:
    explicit MouseEventResult(const std::string& type) : mouseEventInfo_(type) {}
    ~MouseEventResult() = default;

    const MouseEventInfo& GetMouseEventInfo() const
    {
        return mouseEventInfo_;
    }

    void SetMouseEventInfo(const MouseEventInfo& mouseEventInfo)
    {
        mouseEventInfo_ = mouseEventInfo;
    }

private:
    MouseEventInfo mouseEventInfo_;
};

class EventsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void EventsTest::SetUpTestCase() {}

void EventsTest::TearDownTestCase() {}

void EventsTest::SetUp() {}

void EventsTest::TearDown() {}

/**
 * @tc.name: AsyncEventTrigger001
 * @tc.desc: Verify the async event trigger corresponding asyn event.
 * @tc.type: FUNC
 * @tc.require: AR000DAVOU AR000DD68J
 * @tc.author: huye
 */
HWTEST_F(EventsTest, AsyncEventTrigger001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create event trigger and event handler, set event callback on handler and register to
     * trigger.
     */
    EventTriggerResult onTriggerResult;
    auto testEventHandler = AceType::MakeRefPtr<TestEventHandler>();
    testEventHandler->SetAsyncEventCallBack(
        [&onTriggerResult](const EventMarker& eventMarker) { onTriggerResult.SetEventMaker(eventMarker); });
    EventTrigger eventTrigger;
    eventTrigger.RegisterEventHandler(testEventHandler);

    /**
     * @tc.steps: step2. send async event and trigger.
     * @tc.expected: step2. receive event callback and result is right.
     */
    EventMarker maker(EVENT_ID);
    eventTrigger.TriggerAsyncEvent(maker);

    /**
     * @tc.steps: step3. check event info.
     * @tc.expected: step3. the event info is right.
     */
    ASSERT_EQ(onTriggerResult.GetEventMaker(), EVENT_ID);
}

/**
 * @tc.name: AsyncEventTrigger002
 * @tc.desc: Verify the async event trigger corresponding create asyn event callback.
 * @tc.type: FUNC
 * @tc.require: AR000DAVOU AR000DD68J
 * @tc.author: huye
 */
HWTEST_F(EventsTest, AsyncEventTrigger002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create PipelineContext and register event callback handler.
     */
    auto platformWindow = std::make_unique<TestWindow>(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto frontend = Frontend::CreateDefault();
    auto context =
        AceType::MakeRefPtr<PipelineContext>(std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);

    EventTriggerResult onTriggerResult;
    auto testEventHandler = AceType::MakeRefPtr<TestEventHandler>();
    testEventHandler->SetAsyncEventCallBack(
        [&onTriggerResult](const EventMarker& eventMarker) { onTriggerResult.SetEventMaker(eventMarker); });

    context->RegisterEventHandler(testEventHandler);

    /**
     * @tc.steps: step2. create ace async event call back function and touch the function.
     * @tc.expected: step2. receive event callback.
     */
    EventMarker maker(EVENT_ID);
    auto aceAsyncEvent = AceAsyncEvent<void(const std::string&)>::Create(maker, context);
    aceAsyncEvent("");

    /**
     * @tc.steps: step3. check event call back info.
     * @tc.expected: step3. the event info is right.
     */
    ASSERT_EQ(onTriggerResult.GetEventMaker(), EVENT_ID);
}

/**
 * @tc.name: SyncEventTrigger001
 * @tc.desc: Verify the sync event trigger corresponding sync event.
 * @tc.type: FUNC
 * @tc.require: AR000DC4F9 AR000DD68J
 * @tc.author: huye
 */
HWTEST_F(EventsTest, SyncEventTrigger001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create event trigger and event handler, set event callback on handler and register to
     * trigger.
     */
    EventTriggerResult onTriggerResult;
    auto testEventHandler = AceType::MakeRefPtr<TestEventHandler>();
    testEventHandler->SetSyncEventCallBack([&onTriggerResult](const EventMarker& eventMarker, bool& result) {
        onTriggerResult.SetEventMaker(eventMarker);
        result = true;
    });
    EventTrigger eventTrigger;
    eventTrigger.RegisterEventHandler(testEventHandler);

    /**
     * @tc.steps: step2. send sync event and trigger.
     * @tc.expected: step2. receive event callback and result is right.
     */
    EventMarker maker(EVENT_ID);
    bool result = false;
    eventTrigger.TriggerSyncEvent(maker, result);

    /**
     * @tc.steps: step3. check event info.
     * @tc.expected: step3. the event info is right.
     */
    ASSERT_EQ(onTriggerResult.GetEventMaker(), EVENT_ID);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: SyncEventTrigger002
 * @tc.desc: Verify the sync event trigger corresponding create asyn event callback.
 * @tc.type: FUNC
 * @tc.require: AR000DC4F9 AR000DD68J
 * @tc.author: huye
 */
HWTEST_F(EventsTest, SyncEventTrigger002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create PipelineContext and register event callback handler.
     */
    auto platformWindow = std::make_unique<TestWindow>(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto frontend = Frontend::CreateDefault();
    auto context =
        AceType::MakeRefPtr<PipelineContext>(std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);

    EventTriggerResult onTriggerResult;
    auto testEventHandler = AceType::MakeRefPtr<TestEventHandler>();
    testEventHandler->SetSyncEventCallBack([&onTriggerResult](const EventMarker& eventMarker, bool& result) {
        onTriggerResult.SetEventMaker(eventMarker);
        result = true;
    });

    context->RegisterEventHandler(testEventHandler);

    /**
     * @tc.steps: step2. create ace sync event call back function and touch the function.
     * @tc.expected: step2. receive event callback.
     */
    EventMarker maker(EVENT_ID);
    auto aceSyncEvent = AceSyncEvent<void(const std::string&, std::string&)>::Create(maker, context);

    std::string result = "false";
    aceSyncEvent("", result);

    /**
     * @tc.steps: step3. check event call back info.
     * @tc.expected: step3. the event info is right.
     */
    ASSERT_EQ(result, "true");
    ASSERT_EQ(onTriggerResult.GetEventMaker(), EVENT_ID);
}

/**
 * @tc.name: KeyLongPress001
 * @tc.desc: Verify the event corresponding touch key long press.
 * @tc.type: FUNC
 * @tc.require: AR000DB0UK
 * @tc.author: huye
 */
HWTEST_F(EventsTest, KeyLongPress001, TestSize.Level0)
{
    /**
     * @tc.steps: step1.create key event recognizer
     */
    KeyEventRecognizer keyRecognizer;

    /**
     * @tc.steps: step2. touch key down touch firstly.
     * @tc.expected: step2. receive key events and check info right.
     */
    auto keyEvents = keyRecognizer.GetKeyEvents(
        static_cast<int32_t>(KeyCode::TV_CONTROL_LEFT), static_cast<int32_t>(KeyAction::DOWN), 0);
    ASSERT_FALSE(keyEvents.empty());
    if (!keyEvents.empty()) {
        ASSERT_EQ(keyEvents.back().code, KeyCode::TV_CONTROL_LEFT);
        ASSERT_EQ(keyEvents.back().action, KeyAction::DOWN);
    }

    /**
     * @tc.steps: step3. touch key down touch secondly as platform.
     * @tc.expected: step3. receive key events and check info right.
     */
    keyEvents = keyRecognizer.GetKeyEvents(
        static_cast<int32_t>(KeyCode::TV_CONTROL_LEFT), static_cast<int32_t>(KeyAction::DOWN), 1);
    ASSERT_FALSE(keyEvents.empty());
    if (!keyEvents.empty()) {
        ASSERT_EQ(keyEvents.back().code, KeyCode::TV_CONTROL_LEFT);
        ASSERT_EQ(keyEvents.back().action, KeyAction::LONG_PRESS);
    }

    /**
     * @tc.steps: step4. touch key up touch
     * @tc.expected: step4. receive key events and check info right.
     */
    keyEvents = keyRecognizer.GetKeyEvents(
        static_cast<int32_t>(KeyCode::TV_CONTROL_LEFT), static_cast<int32_t>(KeyAction::UP), 0);
    ASSERT_FALSE(keyEvents.empty());
    if (!keyEvents.empty()) {
        ASSERT_EQ(keyEvents.back().code, KeyCode::TV_CONTROL_LEFT);
        ASSERT_EQ(keyEvents.back().action, KeyAction::UP);
    }
}

/**
 * @tc.name: KeyLongPress002
 * @tc.desc: Verify the event corresponding touch key click press.
 * @tc.type: FUNC
 * @tc.require: AR000DB0UK
 * @tc.author: huye
 */
HWTEST_F(EventsTest, KeyLongPress002, TestSize.Level0)
{
    /**
     * @tc.steps: step1.create key event recognizer
     */
    KeyEventRecognizer keyRecognizer;

    /**
     * @tc.steps: step2. touch key down touch firstly.
     * @tc.expected: step2. receive key events and check info right.
     */
    auto keyEvents = keyRecognizer.GetKeyEvents(
        static_cast<int32_t>(KeyCode::TV_CONTROL_LEFT), static_cast<int32_t>(KeyAction::DOWN), 0);
    ASSERT_FALSE(keyEvents.empty());
    if (!keyEvents.empty()) {
        ASSERT_EQ(keyEvents.back().code, KeyCode::TV_CONTROL_LEFT);
        ASSERT_EQ(keyEvents.back().action, KeyAction::DOWN);
    }

    /**
     * @tc.steps: step3. touch key up touch
     * @tc.expected: step3. receive key events and check info right.
     */
    keyEvents = keyRecognizer.GetKeyEvents(
        static_cast<int32_t>(KeyCode::TV_CONTROL_LEFT), static_cast<int32_t>(KeyAction::UP), 0);
    ASSERT_FALSE(keyEvents.empty());
    if (!keyEvents.empty()) {
        ASSERT_EQ(keyEvents.back().code, KeyCode::TV_CONTROL_LEFT);
        ASSERT_EQ(keyEvents.back().action, KeyAction::CLICK);
    }
}

/**
 * @tc.name: RawMouseRecognizer001
 * @tc.desc: Verify the raw recognizer recognizes corresponding mouse event.
 * @tc.type: FUNC
 * @tc.require: AR000DQ249
 * @tc.author: huye
 */
HWTEST_F(EventsTest, RawMouseRecognizer001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. create raw mouse recognizer and set mouse event callback.
     */
    MouseEventResult onMouse("");
    auto mouseRawRecognizer = AceType::MakeRefPtr<MouseRawRecognizer>();
    mouseRawRecognizer->SetOnMouse([&onMouse](const MouseEventInfo& info) { onMouse.SetMouseEventInfo(info); });

    /**
     * @tc.steps: step2. send mouse event.
     * @tc.expected: step2. receive mouse callback and result is right.
     */
    MouseEvent event { .x = 100,
        .y = 100,
        .z = 100,
        .deltaX = 50,
        .deltaY = 50.0,
        .deltaZ = 50.0,
        .scrollX = 0.0,
        .scrollY = 0.0,
        .scrollZ = 0.0,
        .action = MouseAction::PRESS,
        .button = MouseButton::LEFT_BUTTON,
        .pressedButtons = 0x11,
        .time = std::chrono::high_resolution_clock::now() };

    mouseRawRecognizer->SetCoordinateOffset(Offset(MOUSE_OFFSET, MOUSE_OFFSET));
    mouseRawRecognizer->HandleEvent(event);

    MouseEventInfo mouseInfo = onMouse.GetMouseEventInfo();
    ASSERT_EQ(mouseInfo.GetType(), MOUSE_EVENT_TYPE);

    /**
     * @tc.steps: step3. check the mouse location info.
     * @tc.expected: step3. the mouse location info is right.
     */
    const MouseEvent& globalMouse = mouseInfo.GetGlobalMouse();
    const MouseEvent& localMouse = mouseInfo.GetLocalMouse();

    ASSERT_EQ(globalMouse.x, event.x);
    ASSERT_EQ(globalMouse.y, event.y);
    ASSERT_EQ(globalMouse.z, event.z);
    ASSERT_EQ(globalMouse.deltaX, event.deltaX);
    ASSERT_EQ(globalMouse.deltaY, event.deltaY);
    ASSERT_EQ(globalMouse.deltaZ, event.deltaZ);
    ASSERT_EQ(globalMouse.scrollX, event.scrollX);
    ASSERT_EQ(globalMouse.scrollY, event.scrollY);
    ASSERT_EQ(globalMouse.scrollZ, event.scrollZ);
    ASSERT_EQ(globalMouse.action, event.action);
    ASSERT_EQ(globalMouse.button, event.button);
    ASSERT_EQ(globalMouse.pressedButtons, event.pressedButtons);
    ASSERT_EQ(globalMouse.time, event.time);

    ASSERT_EQ(localMouse.x, event.x - MOUSE_OFFSET);
    ASSERT_EQ(localMouse.y, event.y - MOUSE_OFFSET);
}
} // namespace OHOS::Ace
