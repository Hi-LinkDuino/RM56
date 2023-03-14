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

namespace {
const uint32_t MAX_RETRY_COUNT = 1000;
const uint32_t SLEEP_TIME = 1000;

/**
 * Function: Waiting for task run, the most waiting time is 1s.
 * @param f Task callback.
 * @param handler Event handler.
 */
template<typename F>
void WaitTaskCalled(const F &f, const std::shared_ptr<EventHandler> &handler)
{
    uint32_t count = 0;
    if (handler->PostTask(f)) {
        while (true) {
            if (CommonUtils::EventRunGet()) {
                break;
            }
            if (CommonUtils::TaskCalledGet()) {
                break;
            }
            // If delay more than 1s, break
            if (count >= MAX_RETRY_COUNT) {
                break;
            }

            usleep(SLEEP_TIME);
            count++;
        }
    }
}

/**
 * Function: Send event with different shared pointer, and then check the processed result.
 * @param smartPointerType One of smart pointer.
 */
void SendEventWithSmartPtr(SmartPointerType smartPointerType)
{
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto sharedPtr = std::make_shared<int>(1);
    auto weakPtr = std::weak_ptr<int>(sharedPtr);
    auto f = [](int *intPtr) { delete intPtr; };
    auto uniquePtr = std::unique_ptr<int, void (*)(int *)>((new int(1)), f);

    int32_t result = 0;
    auto task = [&handler, &result, &sharedPtr, &weakPtr, &uniquePtr, &f, smartPointerType]() {
        switch (smartPointerType) {
            case SmartPointerType::SHARED_PTR: {
                result = handler->SendSyncEvent(RUN_EVENT_ID, sharedPtr);
                break;
            }
            case SmartPointerType::WEAK_PTR: {
                result = handler->SendSyncEvent(RUN_EVENT_ID, weakPtr);
                break;
            }
            case SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR: {
                result = handler->SendSyncEvent(RUN_EVENT_ID, uniquePtr);
                break;
            }
            case SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR: {
                result = handler->SendSyncEvent(RUN_EVENT_ID, std::unique_ptr<int, void (*)(int *)>((new int(1)), f));
                break;
            }
            default:
                break;
        }
    };

    WaitTaskCalled(task, handler);

    uint32_t count = 0;
    while (true) {
        if (result || (count >= MAX_RETRY_COUNT)) {
            break;
        }
        usleep(SLEEP_TIME);
        count++;
    }

    EXPECT_EQ(1, result);
}
}  // unnamed namespace

class EventHandlerSendSyncEventModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerSendSyncEventModuleTest::SetUpTestCase(void)
{}

void EventHandlerSendSyncEventModuleTest::TearDownTestCase(void)
{}

void EventHandlerSendSyncEventModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the value of flags to the default.
     */
    CommonUtils::EventRunSet(false);
    CommonUtils::TaskCalledSet(false);
}

void EventHandlerSendSyncEventModuleTest::TearDown(void)
{}

/**
 * @tc.name: SendSync001
 * @tc.desc: Send null event
 * @tc.type: FUNC
 * @tc.require: AR000CQ2AF SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send a null event.
     * @tc.expected: step1. Send failed.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto nullPtr = InnerEvent::Pointer(nullptr, nullptr);
    bool result = handler->SendSyncEvent(nullPtr, EventQueue::Priority::LOW);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SendSync002
 * @tc.desc: Send event with IDLE priority
 * @tc.type: FUNC
 * @tc.require: SR000CQ2A7 SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send an event with IDLE priority.
     * @tc.expected: step1. Send failed, and event will not be run.
     */
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    bool result = handler->SendSyncEvent(event, EventQueue::Priority::IDLE);
    EXPECT_FALSE(result);

    bool runResult = CommonUtils::EventRunGet();
    EXPECT_FALSE(runResult);
}

/**
 * @tc.name: SendSync003
 * @tc.desc: Send non-null event with priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send a non-idle priority event in a task.
     * @tc.expected: step1. Send successfully, and event will be run.
     */
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    auto f = [&handler, &event]() {
        bool result = handler->SendSyncEvent(event, EventQueue::Priority::IMMEDIATE);
        EXPECT_TRUE(result);
    };
    WaitTaskCalled(f, handler);

    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync004
 * @tc.desc: Send non-null event with priority which not IDLE in new runner
 * @tc.type: FUNC
 * @tc.require: SR000CQ2AQ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send an event with priority which not IDLE in new runner.
     * @tc.expected: step1. Send successfully, and event will be run.
     */
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    bool result = handler->SendSyncEvent(event, EventQueue::Priority::HIGH);
    EXPECT_TRUE(result);

    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync005
 * @tc.desc: Send r-value event with priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send r-value event with priority which not IDLE in a task.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    auto f = [&handler]() {
        bool rValueResult = handler->SendSyncEvent(InnerEvent::Get(RUN_EVENT_ID), EventQueue::Priority::LOW);
        EXPECT_TRUE(rValueResult);
    };
    WaitTaskCalled(f, handler);

    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync006
 * @tc.desc: Send event with eventId, param and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event in a task with eventId, param and priority which not IDLE.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    auto f = [&handler]() {
        bool result = handler->SendSyncEvent(RUN_EVENT_ID, 0);
        EXPECT_TRUE(result);
    };
    WaitTaskCalled(f, handler);

    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync007
 * @tc.desc: Send event with eventId, shared_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event in a task with eventId, shared_ptr object and priority which not IDLE.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    SendEventWithSmartPtr(SmartPointerType::SHARED_PTR);
    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync008
 * @tc.desc: Send event with eventId, weak_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event in a task with eventId, weak_ptr object and priority which not IDLE.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    SendEventWithSmartPtr(SmartPointerType::WEAK_PTR);
    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync009
 * @tc.desc: Send event with eventId, unique_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event in a task with eventId, unique_ptr object and priority which not IDLE.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    SendEventWithSmartPtr(SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR);
    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync010
 * @tc.desc: Send event with eventId, rvalue reference unique_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event in a task with eventId,
     *                   rvalue reference unique_ptr object and priority which not IDLE.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    SendEventWithSmartPtr(SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR);
    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync011
 * @tc.desc: Post task with callback, taskName and priority
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post task in another task with callback, taskName and priority.
     * @tc.expected: step1. Post successfully, the task will be run.
     */
    string taskName = "myTask";
    auto myTask = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto f = [&handler, &myTask, &taskName]() {
        bool result = handler->PostSyncTask(myTask, taskName);
        EXPECT_TRUE(result);
    };
    WaitTaskCalled(f, handler);

    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: SendSync012
 * @tc.desc: Post task with callback and priority
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendSyncEventModuleTest, SendSync012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post task in another task with callback and priority.
     * @tc.expected: step1. Post successfully, the task will be run.
     */
    auto myTask = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto f = [&handler, &myTask]() {
        bool result = handler->PostSyncTask(myTask, EventQueue::Priority::HIGH);
        EXPECT_TRUE(result);
    };
    WaitTaskCalled(f, handler);

    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}