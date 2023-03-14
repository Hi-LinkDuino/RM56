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
/**
 * Function: Send an event with default priority, and then check the processed result.
 * @param delayTime Delay time for processing.
 * @param priority Priority of event.
 */
void SendAndCheck(int64_t delayTime, EventQueue::Priority priority)
{
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    bool lValueResult = handler->SendEvent(event, delayTime, priority);
    EXPECT_TRUE(lValueResult);

    bool rValueResult = handler->SendEvent(InnerEvent::Get(RUN_EVENT_ID), delayTime, priority);
    EXPECT_TRUE(rValueResult);

    handler->SendEvent(STOP_EVENT_ID, 0, delayTime + 1);
    myRunner->Run();
    uint32_t runResult = 2;
    EXPECT_EQ(runResult, CommonUtils::EventRunCount());
}

/**
 * Function: Send an event with HIGH or IMMEDIATE priority, and chen check the processed result.
 * @param priority Priority of event.
 */
void SendEventWithPriority(EventQueue::Priority priority)
{
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool lvalueSendResult = false;
    bool rvalueSendResult = false;
    int64_t delayTime = 0;

    if (priority == EventQueue::Priority::HIGH) {
        lvalueSendResult = handler->SendHighPriorityEvent(event, delayTime);
        rvalueSendResult =
            handler->SendHighPriorityEvent(InnerEvent::Get(RUN_EVENT_ID, std::make_unique<int>(1)), delayTime);
    }

    if (priority == EventQueue::Priority::IMMEDIATE) {
        lvalueSendResult = handler->SendImmediateEvent(event);
        rvalueSendResult = handler->SendImmediateEvent(InnerEvent::Get(RUN_EVENT_ID, std::make_unique<int>(1)));
    }

    EXPECT_TRUE(lvalueSendResult);
    EXPECT_TRUE(rvalueSendResult);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    uint32_t runResult = 2;
    EXPECT_EQ(runResult, CommonUtils::EventRunCount());
}

/**
 * Function: Send an event by event id with different priority, and check the processed result.
 * @param priority Priority of event.
 */
void SendEventWithPriorityByEventId(EventQueue::Priority priority)
{
    /**
     * @tc.steps: step1. Set event by event id with different priority (LOW, HIGH, IMMEDIATE).
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    bool sendResult = false;
    int64_t param = 0;
    int64_t delayTime = 0;

    if (priority == EventQueue::Priority::LOW) {
        sendResult = handler->SendEvent(RUN_EVENT_ID, param, delayTime);
    }
    if (priority == EventQueue::Priority::HIGH) {
        sendResult = handler->SendHighPriorityEvent(RUN_EVENT_ID, param, delayTime);
    }
    if (priority == EventQueue::Priority::IMMEDIATE) {
        sendResult = handler->SendImmediateEvent(RUN_EVENT_ID, param);
    }

    /**
     * @tc.steps: step2. Check the send and handled result.
     * @tc.expected: step2. Send successfully and the event handled.
     */
    EXPECT_TRUE(sendResult);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    EXPECT_TRUE(CommonUtils::EventRunGet());
}

/**
 * Function: Send an event with shared pointer or weak pointer, and check the send result.
 * @param handler Event handler.
 * @param priority Priority of event.
 * @param ptr Shared pointer or weak pointer.
 */
template<typename T>
void SendEventWithSharedOrWeakPtr(
    const std::shared_ptr<MyEventHandler> &handler, EventQueue::Priority priority, const T &ptr)
{
    bool retVal = false;
    if (priority == EventQueue::Priority::LOW) {
        retVal = handler->SendEvent(RUN_EVENT_ID, ptr, 0);
    }
    if (priority == EventQueue::Priority::HIGH) {
        retVal = handler->SendHighPriorityEvent(RUN_EVENT_ID, ptr, 0);
    }
    if (priority == EventQueue::Priority::IMMEDIATE) {
        retVal = handler->SendImmediateEvent(RUN_EVENT_ID, ptr);
    }
    EXPECT_TRUE(retVal);
}

/**
 * Function: Send an event with unique pointer, and check the send result.
 * @param handler Event handler.
 * @param priority Priority of event.
 * @param isRValue Is or not right value.
 */
void SendEventWithUniquePtr(
    const std::shared_ptr<MyEventHandler> &handler, EventQueue::Priority priority, bool isRValue = false)
{
    auto f = [](int *intPtr) { delete intPtr; };
    auto uniquePtr = std::unique_ptr<int, void (*)(int *)>((new int(1)), f);
    bool retVal = false;
    switch (priority) {
        case EventQueue::Priority::LOW: {
            if (!isRValue) {
                retVal = handler->SendEvent(RUN_EVENT_ID, uniquePtr, 0);
            } else {
                retVal = handler->SendEvent(RUN_EVENT_ID, std::unique_ptr<int, void (*)(int *)>((new int(1)), f), 0);
            }
            break;
        }
        case EventQueue::Priority::HIGH: {
            if (!isRValue) {
                retVal = handler->SendHighPriorityEvent(RUN_EVENT_ID, uniquePtr, 0);
            } else {
                retVal = handler->SendHighPriorityEvent(
                    RUN_EVENT_ID, std::unique_ptr<int, void (*)(int *)>((new int(1)), f), 0);
            }
            break;
        }
        case EventQueue::Priority::IMMEDIATE: {
            if (!isRValue) {
                retVal = handler->SendImmediateEvent(RUN_EVENT_ID, uniquePtr);
            } else {
                retVal =
                    handler->SendImmediateEvent(RUN_EVENT_ID, std::unique_ptr<int, void (*)(int *)>((new int(1)), f));
            }
            break;
        }
        default:
            break;
    }

    EXPECT_TRUE(retVal);
}

/**
 * Function: Send event with smart pointer, and then check the processed result.
 * @param smartPointerType One of smart pointer.
 * @param priority Priority of event.
 */
void SendEventWithSmartPtr(SmartPointerType smartPointerType, EventQueue::Priority priority)
{
    /**
     * @tc.steps: step1. Send event with different priority and smart pointer.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto sharedPtr = std::make_shared<int>(1);
    auto weakPtr = std::weak_ptr<int>(sharedPtr);

    switch (smartPointerType) {
        case SmartPointerType::SHARED_PTR: {
            SendEventWithSharedOrWeakPtr(handler, priority, sharedPtr);
            break;
        }
        case SmartPointerType::WEAK_PTR: {
            SendEventWithSharedOrWeakPtr(handler, priority, weakPtr);
            break;
        }
        case SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR: {
            SendEventWithUniquePtr(handler, priority);
            break;
        }
        case SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR: {
            SendEventWithUniquePtr(handler, priority, true);
            break;
        }
        default:
            break;
    }

    handler->SendEvent(STOP_EVENT_ID, 0, 1);
    myRunner->Run();
    EXPECT_TRUE(CommonUtils::EventRunGet());
}
}  // unnamed namespace

class EventHandlerSendEventModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerSendEventModuleTest::SetUpTestCase(void)
{}

void EventHandlerSendEventModuleTest::TearDownTestCase(void)
{}

void EventHandlerSendEventModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the value of test flags to the default.
     */
    CommonUtils::EventRunSet(false);
    CommonUtils::EventRunCountReset();
}

void EventHandlerSendEventModuleTest::TearDown(void)
{}

/**
 * @tc.name: Send001
 * @tc.desc: Send event with delayTime = 0 and priority = LOW
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with delayTime and priority.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    int64_t delayTime = 0;
    SendAndCheck(delayTime, EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send002
 * @tc.desc: Send event with delayTime > 0 and priority = LOW
 * @tc.type: FUNC
 * @tc.require: SR000CQ2AL SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with delayTime and priority.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    int64_t delayTime = 1;
    SendAndCheck(delayTime, EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send003
 * @tc.desc: Send event with delayTime < 0 and priority = LOW
 * @tc.type: FUNC
 * @tc.require: AR000CQ2AD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with delayTime and priority.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    int64_t delayTime = -1;
    SendAndCheck(delayTime, EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send004
 * @tc.desc: Send event with priority = HIGH
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with HIGH priority.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    int64_t delayTime = 1;
    SendAndCheck(delayTime, EventQueue::Priority::HIGH);
}

/**
 * @tc.name: Send005
 * @tc.desc: Send event with priority = IDLE
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with IDLE priority.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    int64_t delayTime = 0;
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    bool sendResult = handler->SendEvent(event, delayTime, EventQueue::Priority::IDLE);
    EXPECT_TRUE(sendResult);

    auto f = [&myRunner]() { myRunner->Stop(); };
    handler->PostIdleTask(f, delayTime);
    uint32_t newEventId = 1;
    int64_t param = 0;
    handler->SendEvent(newEventId, param, delayTime + 1);
    myRunner->Run();
    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Send006
 * @tc.desc: Send event with delayTime and IMMEDIATE priority
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with delayTime and IMMEDIATE priority.
     * @tc.expected: step1. Send successfully and the event handled.
     */
    int64_t delayTime = 10;
    SendAndCheck(delayTime, EventQueue::Priority::IMMEDIATE);
}

/**
 * @tc.name: Send007
 * @tc.desc: Send event with nullptr
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event with nullptr.
     * @tc.expected: step1. Send failed.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto nullPtr = InnerEvent::Pointer(nullptr, nullptr);
    bool result = handler->SendEvent(nullPtr);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Send008
 * @tc.desc: Send event without event runner
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send event without event runner.
     * @tc.expected: step1. Send failed.
     */
    uint32_t eventId = Random();
    int64_t param = 0;
    int64_t delayTime = 0;
    auto event = InnerEvent::Get(eventId, param);
    auto handler = std::make_shared<MyEventHandler>(nullptr);
    bool result = handler->SendEvent(event, delayTime, EventQueue::Priority::LOW);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: Send009
 * @tc.desc: Send event with eventId, param and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send009, TestSize.Level1)
{
    SendEventWithPriorityByEventId(EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send010
 * @tc.desc: Send event with eventId, shared_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send010, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::SHARED_PTR, EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send011
 * @tc.desc: Send event with eventId, weak_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send011, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::WEAK_PTR, EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send012
 * @tc.desc: Send event with eventId, unique_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send012, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR, EventQueue::Priority::LOW);
}

/**
 * @tc.name: Send013
 * @tc.desc: Send event with eventId, rvalue reference unique_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, Send013, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR, EventQueue::Priority::LOW);
}

/**
 * @tc.name: SendHigh001
 * @tc.desc: Send high priority event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendHigh001, TestSize.Level1)
{
    SendEventWithPriority(EventQueue::Priority::HIGH);
}

/**
 * @tc.name: SendHigh002
 * @tc.desc: Send high priority event with eventId, param and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendHigh002, TestSize.Level1)
{
    SendEventWithPriorityByEventId(EventQueue::Priority::HIGH);
}

/**
 * @tc.name: SendHigh003
 * @tc.desc: Send high priority event with eventId, shared_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendHigh003, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::SHARED_PTR, EventQueue::Priority::HIGH);
}

/**
 * @tc.name: SendHigh004
 * @tc.desc: Send high priority event with eventId, weak_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendHigh004, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::WEAK_PTR, EventQueue::Priority::HIGH);
}

/**
 * @tc.name: SendHigh005
 * @tc.desc: Send high priority event with eventId, unique_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendHigh005, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR, EventQueue::Priority::HIGH);
}

/**
 * @tc.name: SendHigh006
 * @tc.desc: Send high priority event with eventId, rvalue reference unique_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendHigh006, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR, EventQueue::Priority::HIGH);
}

/**
 * @tc.name: SendImmediate001
 * @tc.desc: Send immediate priority event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendImmediate001, TestSize.Level1)
{
    SendEventWithPriority(EventQueue::Priority::IMMEDIATE);
}

/**
 * @tc.name: SendImmediate002
 * @tc.desc: Send immediate priority event with eventId and param
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendImmediate002, TestSize.Level1)
{
    SendEventWithPriorityByEventId(EventQueue::Priority::IMMEDIATE);
}

/**
 * @tc.name: SendImmediate003
 * @tc.desc: Send immediate priority event with eventId, shared_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendImmediate003, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::SHARED_PTR, EventQueue::Priority::IMMEDIATE);
}

/**
 * @tc.name: SendImmediate004
 * @tc.desc: Send immediate priority event with eventId, weak_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendImmediate004, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::WEAK_PTR, EventQueue::Priority::IMMEDIATE);
}

/**
 * @tc.name: SendImmediate005
 * @tc.desc: Send immediate priority event with eventId, unique_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendImmediate005, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR, EventQueue::Priority::IMMEDIATE);
}

/**
 * @tc.name: SendImmediate006
 * @tc.desc: Send immediate priority event with eventId, rvalue reference unique_ptr and delayTime
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerSendEventModuleTest, SendImmediate006, TestSize.Level1)
{
    SendEventWithSmartPtr(SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR, EventQueue::Priority::IMMEDIATE);
}