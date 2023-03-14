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
#include "event_handler_utils.h"

#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS::AppExecFwk;

namespace {
const uint32_t MAX_RETRY_COUNT = 2000;
const uint32_t SLEEP_TIME = 1000;

int64_t GetNowSysTime()
{
    InnerEvent::TimePoint nowSys = InnerEvent::Clock::now();
    auto epoch = nowSys.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    int64_t duration = value.count();
    return duration;
}

/**
 * Function: Waiting for task run, the most waiting time is 1s.
 * @param f Task callback.
 * @param handler Event handler.
 */
template<typename F>
void WaitTaskCalled(const F &f, const std::shared_ptr<EventHandler> &handler)
{
    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 1;
    uint32_t count = 0;
    if (handler->PostTimingTask(f, nowTime + delayTime)) {
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
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto sharedPtr = std::make_shared<int>(1);
    auto weakPtr = std::weak_ptr<int>(sharedPtr);
    auto f = [](int *intPtr) { delete intPtr; };
    auto uniquePtr = std::unique_ptr<int, void (*)(int *)>((new int(1)), f);

    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 10;
    switch (smartPointerType) {
        case SmartPointerType::SHARED_PTR: {
            handler->SendTimingEvent(RUN_EVENT_ID, sharedPtr, nowTime + delayTime);
            break;
        }
        case SmartPointerType::WEAK_PTR: {
            handler->SendTimingEvent(RUN_EVENT_ID, weakPtr, nowTime + delayTime);
            break;
        }
        case SmartPointerType::LVALUE_REFERENCE_UNIQUE_PTR: {
            handler->SendTimingEvent(RUN_EVENT_ID, uniquePtr, nowTime + delayTime);
            break;
        }
        case SmartPointerType::RVALUE_REFERENCE_UNIQUE_PTR: {
            handler->SendTimingEvent(
                RUN_EVENT_ID, std::unique_ptr<int, void (*)(int *)>((new int(1)), f), nowTime + delayTime);
            break;
        }
        default:
            break;
    }
    handler->SendTimingEvent(STOP_EVENT_ID, delayTime + delayTime + nowTime);
    myRunner->Run();

    bool runResult = CommonUtils::EventRunGet();
    EXPECT_TRUE(runResult);
}
}  // unnamed namespace

class EventHandlerSendTimingEventModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerSendTimingEventModuleTest::SetUpTestCase(void)
{}

void EventHandlerSendTimingEventModuleTest::TearDownTestCase(void)
{}

void EventHandlerSendTimingEventModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the value of flags to the default.
     */
    CommonUtils::EventRunSet(false);
    CommonUtils::EventRunCountReset();
    CommonUtils::TaskCalledSet(false);
}

void EventHandlerSendTimingEventModuleTest::TearDown(void)
{}

/**
 * @tc.name: SendTiming001
 * @tc.desc: Send null event
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send a null event.
     * @tc.expected: step1. Send failed.
     */
    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 2;
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto nullPtr = InnerEvent::Pointer(nullptr, nullptr);
    bool result = handler->SendTimingEvent(nullPtr, nowTime + delayTime, EventQueue::Priority::LOW);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SendTiming002
 * @tc.desc: Send r-value event with event and event id
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send r-value event with priority which not IDLE in a task.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 10;
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SendTimingEvent(event, delayTime + nowTime);
    handler->SendTimingEvent(STOP_EVENT_ID, delayTime * 2 + nowTime);
    myRunner->Run();

    uint32_t runResult = 1;
    EXPECT_EQ(runResult, CommonUtils::EventRunCount());
}

/**
 * @tc.name: SendTiming003
 * @tc.desc: Send r-value event with event, event id and priority
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send r-value event with priority which not IDLE in a task.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 10;
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SendTimingEvent(RUN_EVENT_ID, delayTime + nowTime, EventQueue::Priority::LOW);
    handler->SendTimingEvent(STOP_EVENT_ID, delayTime * 2 + nowTime);
    myRunner->Run();

    uint32_t runResult = 1;
    EXPECT_EQ(runResult, CommonUtils::EventRunCount());
}

/**
 * @tc.name: SendTiming004
 * @tc.desc: Send r-value event with event, event id and param
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send r-value event with priority which not IDLE in a task.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 10;
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SendTimingEvent(RUN_EVENT_ID, delayTime + nowTime, 1);
    handler->SendTimingEvent(STOP_EVENT_ID, delayTime * 2 + nowTime);
    myRunner->Run();

    uint32_t runResult = 1;
    EXPECT_EQ(runResult, CommonUtils::EventRunCount());
}
/**
 * @tc.name: SendTiming005
 * @tc.desc: Send event with eventId, shared_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming005, TestSize.Level1)
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
 * @tc.name: SendTiming006
 * @tc.desc: Send event with eventId, weak_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming006, TestSize.Level1)
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
 * @tc.name: SendTiming007
 * @tc.desc: Send event with eventId, unique_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming007, TestSize.Level1)
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
 * @tc.name: SendTiming008
 * @tc.desc: Send event with eventId, rvalue reference unique_ptr object and priority which not IDLE
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming008, TestSize.Level1)
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
 * @tc.name: SendTiming009
 * @tc.desc: Send r-value event with event, event id and param at past time
 * @tc.type: FUNC
 * @tc.require: SR000DPU9L AR000DPVJS
 */
HWTEST_F(EventHandlerSendTimingEventModuleTest, SendTiming009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Send r-value event with priority which not IDLE in a task.
     * @tc.expected: step1. Send successfully, the event will be run.
     */
    int64_t nowTime = GetNowSysTime();
    int64_t delayTime = 10;
    auto event = InnerEvent::Get(RUN_EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    handler->SendTimingEvent(RUN_EVENT_ID, nowTime - delayTime, 1);
    handler->SendTimingEvent(STOP_EVENT_ID, delayTime + nowTime);
    myRunner->Run();

    uint32_t runResult = 1;
    EXPECT_EQ(runResult, CommonUtils::EventRunCount());
}
