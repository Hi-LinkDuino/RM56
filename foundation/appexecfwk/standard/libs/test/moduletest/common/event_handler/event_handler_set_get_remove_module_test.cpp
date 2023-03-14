/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "event_handler_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

class EventHandlerSetGetRemoveModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerSetGetRemoveModuleTest::SetUpTestCase(void)
{}

void EventHandlerSetGetRemoveModuleTest::TearDownTestCase(void)
{}

void EventHandlerSetGetRemoveModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the value of flags to the default.
     */
    CommonUtils::EventRunSet(false);
    CommonUtils::TaskCalledSet(false);
}

void EventHandlerSetGetRemoveModuleTest::TearDown(void)
{}

/**
 * @tc.name: SetRunner001
 * @tc.desc: Set the EventRunner for the EventHandler
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, SetRunner001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set the event runner for current handler.
     * @tc.expected: step1. Set successfully and the event can be send.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    handler->SetEventRunner(myRunner);
    bool result = handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: SetRunner002
 * @tc.desc: Set a null EventRunner for the EventHandler
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, SetRunner002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Set a null event runner for current handler.
     * @tc.expected: step1. Set failed and the event can not be send.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SetEventRunner(nullptr);
    bool allowSend = handler->SendEvent(Random());
    EXPECT_FALSE(allowSend);
}

/**
 * @tc.name: GetRunner001
 * @tc.desc: Get the EventRunner of the EventHandler
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, GetRunner001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get the event runner of current handler.
     * @tc.expected: step1. The got runner has the same addr as created runner.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto currentRunner = handler->GetEventRunner();
    EXPECT_EQ(myRunner, currentRunner);
}

/**
 * @tc.name: GetRunner002
 * @tc.desc: Get a null EventRunner of the EventHandler
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, GetRunner002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get the event runner of handler which not loaded the runner.
     * @tc.expected: step1. Return a nullptr.
     */
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    auto currentRunner = handler->GetEventRunner();
    EXPECT_EQ(nullptr, currentRunner);
}

/**
 * @tc.name: DistributeEvent001
 * @tc.desc: Distribute an event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, DistributeEvent001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create and distribute the event.
     * @tc.expected: step1. Distribute successfully and the event handled.
     */
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    handler->DistributeEvent(event);
    bool result = CommonUtils::EventRunGet();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DistributeEvent002
 * @tc.desc: Distribute a task
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, DistributeEvent002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create and distribute the task.
     * @tc.expected: step1. Distribute successfully and the task executed.
     */
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto event = InnerEvent::Get(f);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    handler->DistributeEvent(event);
    bool result = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DistributeEvent003
 * @tc.desc: Distribute a null event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, DistributeEvent003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Distribute a null event.
     * @tc.expected: step1. Distribute failed and the event not handled.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto nullPtr = InnerEvent::Pointer(nullptr, nullptr);

    handler->DistributeEvent(nullPtr);
    bool result = CommonUtils::EventRunGet();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CurrentHandler001
 * @tc.desc: Get current handler when handler running
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, CurrentHandler001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get current handler when handler running.
     * @tc.expected: step1. Get successfully and the same as created handler.
     */
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler;
    auto f = [&myHandler, &handler]() { myHandler = handler->Current(); };

    handler->PostTask(f);
    uint32_t sleepTime = 20000;
    usleep(sleepTime);
    EXPECT_EQ(handler, myHandler);
}

/**
 * @tc.name: CurrentHandler002
 * @tc.desc: Get current handler when handler not running
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, CurrentHandler002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get current handler when handler not running.
     * @tc.expected: step1. Return a nullptr.
     */
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler = handler->Current();

    EXPECT_EQ(nullptr, myHandler);
}

/**
 * @tc.name: CurrentHandler003
 * @tc.desc: Get current handler when running in different handler
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, CurrentHandler003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get current handler when handler running in different handler.
     * @tc.expected: step1. Get successfully and the same as created handler.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler;
    auto f = [&handler, &myHandler]() {
        auto inRunner = EventRunner::Create(false);
        auto inHandler = std::make_shared<MyEventHandler>(inRunner);
        auto fIn = [&inHandler]() { EXPECT_EQ(inHandler, inHandler->Current()); };
        inHandler->PostTask(fIn);
        inHandler->SendEvent(STOP_EVENT_ID);
        inRunner->Run();
        myHandler = handler->Current();
    };

    handler->PostTask(f);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    EXPECT_EQ(handler, myHandler);
}

/**
 * @tc.name: Remove001
 * @tc.desc: Remove all events
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send multi events to event queue.
     */
    int64_t delayTime = 0;
    uint32_t taskCalledCount = 0;
    uint32_t expectResult = 0;
    auto f = [&taskCalledCount]() { taskCalledCount++; };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    for (uint32_t i = 0; i < 2; ++i) {
        handler->PostTask(f, delayTime, EventQueue::Priority::HIGH);
        delayTime++;
    }

    /**
     * @tc.steps: step2. Remove all events, and run the runner.
     * @tc.expected: step2. Remove successfully and no event run.
     */
    handler->RemoveAllEvents();
    int64_t param = 0;
    int64_t offsetTime = 3;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime + offsetTime);
    myRunner->Run();
    EXPECT_EQ(expectResult, taskCalledCount);
}

/**
 * @tc.name: Remove002
 * @tc.desc: Remove all events without runner
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Remove all events without runner.
     * @tc.expected: step1. Remove failed.
     */
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    handler->RemoveAllEvents();
    bool result = handler->SendEvent(RUN_EVENT_ID);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Remove003
 * @tc.desc: Remove an event by eventId
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event to event queue.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SendEvent(RUN_EVENT_ID);

    /**
     * @tc.steps: step2. Remove the event by eventId.
     * @tc.expected: step2. Remove successfully and removed event not run.
     */
    handler->RemoveEvent(RUN_EVENT_ID);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool result = CommonUtils::EventRunGet();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Remove004
 * @tc.desc: Remove an event by eventId without runner
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Remove an event by eventId without runner.
     * @tc.expected: step1. Remove failed.
     */
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    handler->RemoveEvent(Random());
    bool result = handler->SendEvent(RUN_EVENT_ID);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Remove005
 * @tc.desc: Remove an event by eventId and param
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event to event queue.
     */
    int64_t param = 0;
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SendEvent(RUN_EVENT_ID, param, 0);

    /**
     * @tc.steps: step2. Remove the event by eventId and param.
     * @tc.expected: step2. Remove successfully and removed event not run.
     */
    handler->RemoveEvent(RUN_EVENT_ID, param);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool result = CommonUtils::EventRunGet();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Remove006
 * @tc.desc: Remove an event by eventId and param without runner
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Remove an event by eventId and param without runner.
     * @tc.expected: step1. Remove failed.
     */
    int64_t param = 0;
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    handler->RemoveEvent(Random(), param);
    bool result = handler->SendEvent(RUN_EVENT_ID);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Remove007
 * @tc.desc: Remove a task by name
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a task to event queue.
     */
    string taskName = std::to_string(Random());
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->PostTask(f, taskName);

    /**
     * @tc.steps: step2. Remove the task by name.
     * @tc.expected: step2. Remove successfully and removed task not run.
     */
    handler->RemoveTask(taskName);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool result = CommonUtils::TaskCalledGet();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Remove008
 * @tc.desc: Remove a task by name without runner
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSetGetRemoveModuleTest, Remove008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Remove a task by name without runner.
     * @tc.expected: step1. Remove failed.
     */
    string taskName = std::to_string(Random());
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    handler->RemoveTask(taskName);
    bool result = handler->SendEvent(RUN_EVENT_ID);
    EXPECT_FALSE(result);
}
