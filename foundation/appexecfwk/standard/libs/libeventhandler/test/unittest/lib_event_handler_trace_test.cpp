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

#include <gtest/gtest.h>

#include <thread>

#include "hitrace/trace.h"
#define private public
#include "event_handler.h"
#include "event_runner.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS::HiviewDFX;

using WaitFunc = std::function<bool()>;

std::atomic<bool> g_eventProcess(false);
const int32_t USLEEP_STEP = 100;
const int32_t USLEEP_TIME_OUT = 50000;

class FirstTestEventHandler : public EventHandler {
public:
    explicit FirstTestEventHandler(const std::shared_ptr<EventRunner>& runner) : EventHandler(runner) {}
    ~FirstTestEventHandler() override {}

    void ProcessEvent(const InnerEvent::Pointer& event) override
    {
        auto traceId = event->GetOrCreateTraceId();
        switch (event->GetParam()) {
            case HITRACE_FLAG_INCLUDE_ASYNC: {
                EXPECT_TRUE(traceId->IsFlagEnabled(HITRACE_FLAG_INCLUDE_ASYNC));
                break;
            }
            case (HITRACE_FLAG_TP_INFO | HITRACE_FLAG_INCLUDE_ASYNC): {
                EXPECT_TRUE(traceId->IsFlagEnabled(HITRACE_FLAG_TP_INFO));
                break;
            }
            case HITRACE_FLAG_DONOT_CREATE_SPAN: {
                EXPECT_TRUE(traceId->IsValid());
                break;
            }
            case (HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_NO_BE_INFO): {
                EXPECT_TRUE(traceId->IsFlagEnabled(HITRACE_FLAG_NO_BE_INFO));
                break;
            }
            case (HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_DONOT_ENABLE_LOG): {
                EXPECT_TRUE(traceId->IsFlagEnabled(HITRACE_FLAG_DONOT_ENABLE_LOG));
                break;
            }
            case (HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_FAULT_TRIGGER): {
                EXPECT_TRUE(traceId->IsFlagEnabled(HITRACE_FLAG_FAULT_TRIGGER));
                break;
            }
            default:
                break;
        }
        g_eventProcess.store(true);
    }
};

class SecondTestEventHandler : public EventHandler {
public:
    explicit SecondTestEventHandler(const std::shared_ptr<EventRunner>& runner) : EventHandler(runner) {}
    ~SecondTestEventHandler() override {}

    void ProcessEvent(const InnerEvent::Pointer& event) override
    {
        auto eventTraceId = event->GetOrCreateTraceId();
        EXPECT_TRUE(eventTraceId->IsValid());
        g_eventProcess.store(true);
    }
};

class ThirdTestEventHandler : public EventHandler {
public:
    explicit ThirdTestEventHandler(const std::shared_ptr<EventRunner>& runner) : EventHandler(runner) {}
    ~ThirdTestEventHandler() override {}

    void ProcessEvent(const InnerEvent::Pointer& event) override
    {
        auto eventTraceId = event->GetOrCreateTraceId();
        EXPECT_FALSE(eventTraceId->IsValid());
        g_eventProcess.store(true);
    }
};

/**
 * wait until waitFunc return true.
 */
void Wait(const WaitFunc& waitFunc)
{
    uint32_t sleepTime = 0;
    bool timeOut = false;
    while (waitFunc()) {
        usleep(USLEEP_STEP);
        sleepTime += USLEEP_STEP;
        if (sleepTime > USLEEP_TIME_OUT) {
            timeOut = true;
            break;
        }
    }
    EXPECT_FALSE(timeOut);
}

void TraceFlagTest(int flagParam)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<FirstTestEventHandler>(runner);
    uint32_t innerEventId = 0;
    auto event = InnerEvent::Get(innerEventId, flagParam);
    handler->SendEvent(event);
    auto process = []() {
        return !g_eventProcess.load();
    };
    Wait(process);
}

class LibEventHandlerTraceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LibEventHandlerTraceTest::SetUpTestCase() {}

void LibEventHandlerTraceTest::TearDownTestCase() {}

void LibEventHandlerTraceTest::SetUp() {}

void LibEventHandlerTraceTest::TearDown()
{
    /**
     * @tc.teardown:reset g_eventProcess value.
     */
    g_eventProcess.store(false);
}

/**
 * @tc.name: EventTrace001
 * @tc.desc: trace id of event is not valid when not add trace id to event.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, EventTrace001, TestSize.Level0)
{
    /**
     * @tc.setup: get event with eventId and param.
     */
    uint32_t eventId = 0;
    int64_t eventParam = 0;
    auto event = InnerEvent::Get(eventId, eventParam);

    /**
     * @tc.steps: step1. get trace id from event and check wheather the trace id is valid.
     * @tc.expected: step1. trace id is not valid.
     */
    auto traceId = event->GetOrCreateTraceId();
    auto eventTraceId = event->GetTraceId();
    EXPECT_FALSE(traceId->IsValid());
    EXPECT_FALSE(eventTraceId->IsValid());
}

/**
 * @tc.name: EventTrace002
 * @tc.desc: Check wheather trace id of event is valid when begin hitrace.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, EventTrace002, TestSize.Level0)
{
    /**
     * @tc.setup: begin hitrace and get event with eventId and param.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_INCLUDE_ASYNC);
    uint32_t eventId = 0;
    int64_t eventParam = 0;
    auto event = InnerEvent::Get(eventId, eventParam);

    /**
     * @tc.steps: step1. call GetOrCreateTraceId to add trace into event and get trace id from event .
     * @tc.expected: step1. trace id is valid.
     */
    auto traceId = event->GetOrCreateTraceId();
    auto eventTraceId = event->GetTraceId();
    EXPECT_TRUE(traceId->IsValid());
    EXPECT_TRUE(eventTraceId->IsValid());

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: BeginTrace001
 * @tc.desc: Check wheather trace id of event is valid when begin hitrace.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, BeginTrace001, TestSize.Level0)
{
    /**
     * @tc.setup: begin hitrace with flag HITRACE_FLAG_INCLUDE_ASYNC and HITRACE_FLAG_TP_INFO.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_TP_INFO);

    /**
     * @tc.steps: step1. post task and check trace id of event until the task is executed.
     * @tc.expected: step1. trace id is valid.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto f = []() {
        HiTraceId traceId = HiTrace::GetId();
        EXPECT_TRUE(traceId.IsValid());
        g_eventProcess.store(true);
    };
    handler->PostTask(f);
    auto process = []() {
        return !g_eventProcess.load();
    };
    Wait(process);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: BeginTrace002
 * @tc.desc: Check wheather trace id of event is valid when not begin hitrace.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, BeginTrace002, TestSize.Level0)
{
    /**
     * @tc.setup: create runner and handler.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    /**
     * @tc.steps: step1. post task and check trace id of event until the task is executed.
     * @tc.expected: step1. trace id is not valid.
     */
    auto f = []() {
        HiTraceId traceId = HiTrace::GetId();
        EXPECT_FALSE(traceId.IsValid());
        g_eventProcess.store(true);
    };
    handler->PostTask(f);
    auto process = []() {
        return !g_eventProcess.load();
    };
    Wait(process);
}

/**
 * @tc.name: TraceFlag001
 * @tc.desc: Check wheather the flag of trace id of event is the same as the flag set when begin trace with the flag
 *           HITRACE_FLAG_INCLUDE_ASYNC.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, TraceFlag001, TestSize.Level0)
{
    /**
     * @tc.setup: begin trace and set flag HITRACE_FLAG_INCLUDE_ASYNC.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_INCLUDE_ASYNC);

    /**
     * @tc.steps: step1. create runner and handler, get event with id and param and send the event, check wheather the
     *            flag is the same as the flag set when begin trace
     * @tc.expected: step1. the flag is the same.
     */
    int flagParam = HITRACE_FLAG_INCLUDE_ASYNC;
    TraceFlagTest(flagParam);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: TraceFlag002
 * @tc.desc: Check wheather the flag of trace id of event is the same as the flag set when begin trace with the flag
 *           HITRACE_FLAG_DONOT_CREATE_SPAN.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, TraceFlag002, TestSize.Level0)
{
    /**
     * @tc.setup:begin trace and set flag HITRACE_FLAG_DONOT_CREATE_SPAN.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_DONOT_CREATE_SPAN);

    /**
     * @tc.steps: step1.  create runner and handler, get event with id and param and send the event, check wheather the
     *            flag is the same as the flag set when begin trace
     * @tc.expected: step1. the flag is the same.
     */
    int flagParam = HITRACE_FLAG_DONOT_CREATE_SPAN;
    TraceFlagTest(flagParam);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: TraceFlag003
 * @tc.desc: Check wheather the flag of trace id of event is the same as the flag set when begin trace with the flag
 *           HITRACE_FLAG_TP_INFO.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, TraceFlag003, TestSize.Level0)
{
    /**
     * @tc.setup:begin trace and set flag HITRACE_FLAG_TP_INFO.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_TP_INFO | HITRACE_FLAG_INCLUDE_ASYNC);

    /**
     * @tc.steps: step1. create runner and handler,  get event with id and param and send the event, check wheather the
     *            flag is the same as the flag set when begin trace
     * @tc.expected: step1. the flag is the same.
     */
    int flagParam = HITRACE_FLAG_TP_INFO | HITRACE_FLAG_INCLUDE_ASYNC;
    TraceFlagTest(flagParam);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: TraceFlag004
 * @tc.desc: Check wheather the flag of trace id of event is the same as the flag set when begin trace with the flag
 *           HITRACE_FLAG_NO_BE_INFO.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, TraceFlag004, TestSize.Level0)
{
    /**
     * @tc.setup: begin trace and set flag HITRACE_FLAG_NO_BE_INFO.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_NO_BE_INFO | HITRACE_FLAG_INCLUDE_ASYNC);

    /**
     * @tc.steps: step1. create runner and handler, get event with id and param and send the event, check wheather the
     *            flag is the same as the flag set when begin trace
     * @tc.expected: step1. the flag is the same.
     */
    int flagParam = HITRACE_FLAG_NO_BE_INFO | HITRACE_FLAG_INCLUDE_ASYNC;
    TraceFlagTest(flagParam);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: TraceFlag005
 * @tc.desc: Check wheather the flag of trace id of event is the same as the flag set when begin trace with the flag
 *           HITRACE_FLAG_DONOT_ENABLE_LOG.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, TraceFlag005, TestSize.Level0)
{
    /**
     * @tc.setup: begin trace and set flag HITRACE_FLAG_DONOT_ENABLE_LOG.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_DONOT_ENABLE_LOG | HITRACE_FLAG_INCLUDE_ASYNC);

    /**
     * @tc.steps: step1. create runner and handler, get event with id and param and send the event, check wheather the
     *            flag is the same as the flag set when begin trace
     * @tc.expected: step1. the flag is the same.
     */
    int flagParam = HITRACE_FLAG_DONOT_ENABLE_LOG | HITRACE_FLAG_INCLUDE_ASYNC;
    TraceFlagTest(flagParam);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: TraceFlag006
 * @tc.desc: Check wheather the flag of trace id of event is the same as the flag set when begin trace with the flag
 *           HITRACE_FLAG_FAULT_TRIGGER.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, TraceFlag006, TestSize.Level0)
{
    /**
     * @tc.setup: begin trace and set flag HITRACE_FLAG_FAULT_TRIGGER.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_FAULT_TRIGGER | HITRACE_FLAG_INCLUDE_ASYNC);

    /**
     * @tc.steps: step1. create runner and handler, get event with id and param and send the event, check wheather the
     *            flag is the same as the flag set when begin trace
     * @tc.expected: step1. the flag is the same.
     */
    int flagParam = HITRACE_FLAG_FAULT_TRIGGER | HITRACE_FLAG_INCLUDE_ASYNC;
    TraceFlagTest(flagParam);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: DepositTrace001
 * @tc.desc: Check wheather trace id of event is valid when runner and handler belong to the same thread.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, DepositTrace001, TestSize.Level0)
{
    /**
     * @tc.setup: create runner and handler, begin trace and set flag.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_TP_INFO);

    /**
     * @tc.steps: step1. create runner and handler in the same thread,  get event and sendEvent, then
     *            check wheather the trace is valid.
     * @tc.expected: step1. trace id is valid.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<SecondTestEventHandler>(runner);
    uint32_t innerEventId = 0;
    auto event = InnerEvent::Get(innerEventId);

    auto process = []() {
        return !g_eventProcess.load();
    };
    auto f = [&runner, &process]() {
        Wait(process);
        runner->Stop();
    };
    std::thread newThread(f);
    newThread.detach();
    handler->SendEvent(event);
    auto result = runner->Run();
    EXPECT_EQ(OHOS::ERR_OK, result);

    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: SyncSendTrace001
 * @tc.desc: Check wheather trace id of event is valid when SendSyncEvent.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, SyncSendTrace001, TestSize.Level0)
{
    /**
     * @tc.setup: create runner and handler, begin trace and set flag.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_TP_INFO);

    /**
     * @tc.steps: step1. create runner and handler, make sure the runner is the current runner, then get event and
     *            send Sync Event, then check wheather the trace is valid.
     * @tc.expected: step1. trace id is not valid.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<ThirdTestEventHandler>(runner);
    uint32_t innerEventId = 0;
    auto event = InnerEvent::Get(innerEventId);

    auto process = []() {
        return !g_eventProcess.load();
    };

    auto running = [&runner]() {
        return !runner->IsRunning();
    };

    auto f = [&runner, &handler, &event, &process, &running]() {
        Wait(running);
        handler->SendSyncEvent(event);
        Wait(process);
    };
    auto m = [&handler, &f, &running]() {
        Wait(running);
        handler->PostTask(f);
    };

    std::thread newThread(m);
    newThread.join();
    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}

/**
 * @tc.name: SyncSendTrace002
 * @tc.desc: Check wheather trace id of event is valid when SendSyncEvent.
 * @tc.type: FUNC
 * @tc.require: AR000GGRVV
 * @tc.author: liuyanzhi
 */
HWTEST_F(LibEventHandlerTraceTest, SyncSendTrace002, TestSize.Level0)
{
    /**
     * @tc.setup: create runner and handler, begin trace and set flag.
     */
    auto initId = HiTrace::Begin("EventHandler", HITRACE_FLAG_INCLUDE_ASYNC | HITRACE_FLAG_TP_INFO);

    /**
     * @tc.steps: step1. create runner and handler, make sure the runner is not current runner, then get event and
     *            send Sync Event, then check wheather the trace is valid.
     * @tc.expected: step1. trace id is valid.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<SecondTestEventHandler>(runner);
    uint32_t innerEventId = 0;
    auto event = InnerEvent::Get(innerEventId);

    auto process = []() {
        return !g_eventProcess.load();
    };

    auto running = [&runner]() {
        return !runner->IsRunning();
    };

    Wait(running);
    handler->SendSyncEvent(event);
    Wait(process);
    /**
     * @tc.teardown: end trace id.
     */
    HiTrace::End(initId);
}
