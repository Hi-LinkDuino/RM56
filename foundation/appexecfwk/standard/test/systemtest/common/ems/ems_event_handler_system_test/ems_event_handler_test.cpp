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

#include <atomic>
#include <gtest/gtest.h>

#include "event_handler.h"
#include "event_queue.h"
#include "event_runner.h"
#include "inner_event.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
std::mutex g_mtx;
const uint32_t EVENT_ID = 0;
const uint32_t EVENT_ID_ONE = 100;
const uint32_t EVENT_ID_TWO = 101;
const uint32_t EVENT_ID_THREE = 102;
const uint32_t EVENT_ID_FOUR = 103;
const uint32_t EVENT_ID_FIVE = 104;
const int64_t EVENT_PARAM_ONE = 1000;
const int64_t EVENT_PARAM_TWO = 10000;
const int64_t EVENT_PARAM_THREE = 100000;
const int64_t EVENT_PARAM_FOUR = 10000000;
const int64_t EVENT_PARAM_FIVE = 100000000;
const int64_t FLAG_ONE = 1;
const int64_t FLAG_TWO = 2;
const int64_t FLAG_THREE = 3;
const int64_t FLAG_FOUR = 4;
const int64_t FLAG_FIVE = 5;
const int64_t DELAY_TIME = 10;
const int64_t DELAYWAITTIME = 100000;
bool g_isDump = true;
std::atomic<bool> eventRan(false);
const std::string THREAD_NAME_TEST1 = "threadNameTest1";
const std::string THREAD_NAME_TEST2 = " ";
const std::string THREAD_NAME_TEST3 = "@#￥#3243adsafdf_中文";
const std::string THREAD_NAME_TEST4 = std::to_string(0);
const std::string THREAD_NAME_TEST5 = std::to_string(0) + "@#￥#3243adsafdf_中文";
}  // namespace

class EmsEventHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class CommonUtils {
public:
    CommonUtils() = delete;
    ~CommonUtils() = delete;
    DISALLOW_COPY_AND_MOVE(CommonUtils);

    /**
     * Function: Get the processed result of event.
     * @return Returns true if event has been processed.
     */
    static inline bool EventRunGet()
    {
        GTEST_LOG_(INFO) << "CommonUtils: EventRunGet begain";
        return g_eventRun_.load();
    }

    /**
     * Function: Set the processed result of event.
     * @param run Set false or true for processed result.
     */
    static inline void EventRunSet(bool run)
    {
        GTEST_LOG_(INFO) << "CommonUtils: EventRunSet begain";
        g_eventRun_.store(run);
    }

    /**
     * Function: Get the called result of task.
     * @return Returns true if task has been called.
     */
    static inline bool TaskCalledGet()
    {
        return g_taskCalled_.load();
    }

    /**
     * Function: Set the called result of task.
     * @param run Set false or true for called result.
     */
    static inline void TaskCalledSet(bool called)
    {
        g_taskCalled_.store(called);
    }

    /**
     * Function: Get the processed times of event.
     * @return Returns processed times.
     */
    static inline uint32_t EventRunCount()
    {
        return g_eventRunTimes_;
    }

    /**
     * Function: Increment of event processed times.
     */
    static inline void EventRunCountIncrement()
    {
        g_eventRunTimes_++;
    }

    /**
     * Function: Reset the count of event processed to 0.
     */
    static inline void EventRunCountReset()
    {
        g_eventRunTimes_ = 0;
    }

private:
    static std::atomic<bool> g_eventRun_;
    static std::atomic<bool> g_taskCalled_;
    static std::atomic<uint32_t> g_eventRunTimes_;
};

std::atomic<bool> CommonUtils::g_eventRun_ = {false};
std::atomic<bool> CommonUtils::g_taskCalled_ = {false};
std::atomic<uint32_t> CommonUtils::g_eventRunTimes_ = {0};

void EmsEventHandlerTest::SetUpTestCase(void)
{}

void EmsEventHandlerTest::TearDownTestCase(void)
{}

void EmsEventHandlerTest::SetUp(void)
{
    /**
     * @tc.setup: reset the eventRan value.
     */
    eventRan.store(false);
    /**
     * @tc.setup: Set the value of flags to the default.
     */
    CommonUtils::EventRunSet(false);
    CommonUtils::TaskCalledSet(false);
}

void EmsEventHandlerTest::TearDown(void)
{}

class MyEventHandler : public EventHandler {
public:
    explicit MyEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
    {}
    ~MyEventHandler()
    {}

    void ProcessEvent(const InnerEvent::Pointer &) override
    {
        GTEST_LOG_(INFO) << "MyEventHandler: ProcessEvent begain";
        CommonUtils::EventRunSet(true);
    }

    MyEventHandler(const MyEventHandler &) = delete;
    MyEventHandler &operator=(const MyEventHandler &) = delete;
    MyEventHandler(MyEventHandler &&) = delete;
    MyEventHandler &operator=(MyEventHandler &&) = delete;
};

class DumpTest : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {
        g_isDump = true;
        GTEST_LOG_(INFO) << message;
    }

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return "DumpTest";
    }
};

class TestEventHandler : public EventHandler {
public:
    explicit TestEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
    {}
    ~TestEventHandler() override
    {}

    void ProcessEvent(const InnerEvent::Pointer &event) override
    {
        g_mtx.unlock();
        switch (event->GetParam()) {
            case FLAG_ONE: {
                EXPECT_EQ(event->GetParam(), FLAG_ONE);
                break;
            }
            case FLAG_TWO: {
                EXPECT_EQ(event->GetParam(), FLAG_TWO);
                break;
            }
            case FLAG_THREE: {
                EXPECT_EQ(event->GetParam(), FLAG_THREE);
                break;
            }
            case FLAG_FOUR: {
                EXPECT_EQ(event->GetParam(), FLAG_FOUR);
                break;
            }
            case FLAG_FIVE: {
                EXPECT_EQ(event->GetParam(), FLAG_FIVE);
                break;
            }
            default:
                break;
        }
    }
};

static void ProcessEventTest(int flagParam)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<TestEventHandler>(runner);
    uint32_t innerEventId = 0;
    auto event = InnerEvent::Get(innerEventId, flagParam);
    g_mtx.lock();
    handler->SendEvent(event);
    int count = 0;
    while (!g_mtx.try_lock()) {
        if (count == 0) {
            GTEST_LOG_(INFO) << "Wait ProcessEvent callback function process";
            count = 1;
        } else {
            usleep(DELAYWAITTIME);
        }
    }

    g_mtx.unlock();
}

/*
 * @tc.name: GetEventName001
 * @tc.desc: check when send event has no task return event id
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventName001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event001 = InnerEvent::Get(EVENT_ID);

    std::string eventName = handler->GetEventName(event001);
    EXPECT_EQ(eventName, std::to_string(EVENT_ID));
}

/*
 * @tc.name: GetEventName002
 * @tc.desc: check when send event with param but has no task return event id
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventName002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event002 = InnerEvent::Get(EVENT_ID, EVENT_PARAM_ONE);

    std::string eventName = handler->GetEventName(event002);
    EXPECT_EQ(eventName, std::to_string(EVENT_ID));
}

/*
 * @tc.name: GetEventName003
 * @tc.desc: check when send event has task event name is "name" return event name "name"
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventName003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event003 = InnerEvent::Get(task, THREAD_NAME_TEST1);

    std::string eventName = handler->GetEventName(event003);
    EXPECT_EQ(eventName, THREAD_NAME_TEST1);
}

/*
 * @tc.name: GetEventName004
 * @tc.desc: check when send event has task task name is "" return event name ""
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventName004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event004 = InnerEvent::Get(task, THREAD_NAME_TEST2);

    std::string eventName = handler->GetEventName(event004);
    EXPECT_EQ(eventName, THREAD_NAME_TEST2);
}

/*
 * @tc.name: GetEventName005
 * @tc.desc: check when send event has task task name is special characters return event name is right
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventName005, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event005 = InnerEvent::Get(task, THREAD_NAME_TEST3);

    std::string eventName = handler->GetEventName(event005);
    EXPECT_EQ(eventName, THREAD_NAME_TEST3);
}

/*
 * @tc.name: GetEventName006
 * @tc.desc: check when send event has task task name is special characters return event name is right
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventName006, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event005 = InnerEvent::Get(task, THREAD_NAME_TEST4);

    std::string eventName = handler->GetEventName(event005);
    EXPECT_EQ(eventName, THREAD_NAME_TEST4);
}

/*
 * @tc.name: GetEventRunner001
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/*
 * @tc.name: GetEventRunner002
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner002, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/*
 * @tc.name: GetEventRunner001
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner003, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST1);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/*
 * @tc.name: GetEventRunner002
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner004, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST2);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/*
 * @tc.name: GetEventRunner001
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner005, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST3);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/*
 * @tc.name: GetEventRunner002
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner006, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST4);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/*
 * @tc.name: GetEventRunner001
 * @tc.desc: check get event runner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, GetEventRunner007, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST5);
    auto handler = std::make_shared<EventHandler>(runner);

    auto result = handler->GetEventRunner();
    EXPECT_EQ(result, runner);
}

/**
 * @tc.name: ProcessEvent001
 * @tc.desc: Check process event success with the flag
 *           HITRACE_FLAG_INCLUDE_ASYNC.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, ProcessEvent001, TestSize.Level1)
{
    int flagParam = FLAG_ONE;
    ProcessEventTest(flagParam);
}

/**
 * @tc.name: ProcessEvent002
 * @tc.desc: Check process event success with the flag
 *           HITRACE_FLAG_DONOT_CREATE_SPAN.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, ProcessEvent002, TestSize.Level1)
{
    int flagParam = FLAG_TWO;
    ProcessEventTest(flagParam);
}

/**
 * @tc.name: ProcessEvent003
 * @tc.desc: Check process event success with the flag
 *           HITRACE_FLAG_TP_INFO
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, ProcessEvent003, TestSize.Level1)
{
    int flagParam = FLAG_THREE;
    ProcessEventTest(flagParam);
}

/**
 * @tc.name: ProcessEvent004
 * @tc.desc: Check process event success with the flag
 *           HITRACE_FLAG_NO_BE_INFO | HITRACE_FLAG_INCLUDE_ASYNC.
 * @tc.type: FUNC
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, ProcessEvent004, TestSize.Level1)
{
    int flagParam = FLAG_FOUR;
    ProcessEventTest(flagParam);
}

/**
 * @tc.name: ProcessEvent005
 * @tc.desc: Check process event success with the flag
 *           HITRACE_FLAG_DONOT_ENABLE_LOG | HITRACE_FLAG_INCLUDE_ASYNC.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, ProcessEvent005, TestSize.Level1)
{
    int flagParam = FLAG_FIVE;
    ProcessEventTest(flagParam);
}

/*
 * @tc.name: HasEventWithID001
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_ONE, object);

    handler->SendTimingEvent(event, FLAG_ONE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID002
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_TWO, object);

    handler->SendTimingEvent(event, FLAG_TWO);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID003
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_THREE, object);

    handler->SendTimingEvent(event, FLAG_THREE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID004
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_FOUR, object);

    handler->SendTimingEvent(event, FLAG_FOUR);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FOUR);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID005
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID005, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_FIVE, object);

    handler->SendTimingEvent(event, FLAG_FIVE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID006
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID006, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID_ONE, FLAG_ONE, 0);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID007
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID007, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID_TWO, FLAG_TWO, 0);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID008
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID008, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID_THREE, FLAG_THREE, 0);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID009
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID009, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID_FOUR, FLAG_FOUR, 0);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FOUR);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID010
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID010, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID_FIVE, FLAG_FIVE, 0);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID011
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID011, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_ONE, FLAG_ONE, object);

    handler->SendEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID002
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID012, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_TWO, FLAG_ONE, object);

    handler->SendEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID003
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID013, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_THREE, FLAG_ONE, object);

    handler->SendEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID014
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID014, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_FOUR, FLAG_ONE, object);

    handler->SendEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FOUR);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID015
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID015, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto object = std::make_shared<uint32_t>();
    auto event = InnerEvent::Get(EVENT_ID_FOUR, FLAG_ONE, object);

    handler->SendEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID016
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID016, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_ONE);

    handler->SendEvent(event, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID017
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID017, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID018
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID018, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, TestEventHandler::Priority::IDLE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID019
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID019, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, TestEventHandler::Priority::HIGH);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID020
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID020, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, TestEventHandler::Priority::LOW);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID021
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID021, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(EVENT_ID, FLAG_ONE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID022
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID022, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(EVENT_ID, FLAG_TWO);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID023
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID023, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(EVENT_ID, FLAG_THREE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID024
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID024, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(EVENT_ID, FLAG_FOUR);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID025
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID025, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(EVENT_ID, FLAG_FIVE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID026
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID026, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, FLAG_ONE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID027
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID027, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, FLAG_TWO);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID028
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID028, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, FLAG_THREE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID029
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID029, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, FLAG_FOUR);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID030
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID030, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, FLAG_FIVE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam031
 * @tc.desc: check whether an event with the given id can be found among the events that have been
 *           sent and not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID031, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_ONE);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::LOW);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_TRUE(hasInnerEvent);
    uint64_t delaywaittime = 100000;
    usleep(delaywaittime);
    hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam032
 * @tc.desc: check whether an event with the id can be found among the events that have been
 *           sent and not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID032, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::LOW);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_TRUE(hasInnerEvent);
    uint64_t delaywaittime = 100000;
    usleep(delaywaittime);
    hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam033
 * @tc.desc: check whether an event with the id can be found among the events that have been
 *           sent and not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID033, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::HIGH);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_TRUE(hasInnerEvent);
    uint64_t delaywaittime = 100000;
    usleep(delaywaittime);
    hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam034
 * @tc.desc: check whether an event with the id can be found among the events that have been
 *           sent and not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID034, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::IMMEDIATE);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_TRUE(hasInnerEvent);
    uint64_t delaywaittime = 100000;
    usleep(delaywaittime);
    hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam035
 * @tc.desc: check whether an event with the id can be found among the events that have been
 *           sent and not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID035, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::IDLE);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_TRUE(hasInnerEvent);
    uint64_t delaywaittime = 100000;
    usleep(delaywaittime);
    hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID036
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID036, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID_ONE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID037
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID037, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID038
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID038, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, TestEventHandler::Priority::IDLE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID039
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID039, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(EVENT_ID, TestEventHandler::Priority::HIGH);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID040
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID040, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, TestEventHandler::Priority::LOW);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID041
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID041, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, FLAG_ONE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID042
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID042, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, FLAG_TWO, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID043
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID043, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, FLAG_THREE, TestEventHandler::Priority::IDLE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID044
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID044, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(EVENT_ID, FLAG_FOUR, TestEventHandler::Priority::HIGH);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID045
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID045, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendEvent(EVENT_ID, FLAG_FIVE, TestEventHandler::Priority::LOW);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);

    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID046
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID046, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendSyncEvent(event, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID047
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID047, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_ONE);

    handler->SendSyncEvent(event, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID048
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID048, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendSyncEvent(event, TestEventHandler::Priority::IDLE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID049
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID049, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendSyncEvent(EVENT_ID, TestEventHandler::Priority::HIGH);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID050
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID050, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendSyncEvent(EVENT_ID, TestEventHandler::Priority::LOW);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID046
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID051, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_ONE);

    handler->SendSyncEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID052
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID052, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_TWO);

    handler->SendSyncEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID053
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID053, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_THREE);

    handler->SendSyncEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID054
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID054, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_FOUR);

    handler->SendSyncEvent(event);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FOUR);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID055
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID055, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID_FIVE);

    handler->SendSyncEvent(event);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID056
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID056, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_ONE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID057
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID057, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_TWO, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID058
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID058, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_THREE, TestEventHandler::Priority::IDLE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID059
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID059, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_FOUR, TestEventHandler::Priority::HIGH);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID060
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID060, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_FIVE, TestEventHandler::Priority::LOW);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID061
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID061, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_ONE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID062
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID062, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_TWO);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID063
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID063, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_THREE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID064
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID064, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_FOUR);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID065
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID065, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendSyncEvent(EVENT_ID_FIVE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID066
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID066, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(event, FLAG_ONE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID067
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID067, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(event, FLAG_TWO, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID068
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID068, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(event, FLAG_THREE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID069
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID069, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(event, FLAG_FOUR, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID070
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID070, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendTimingEvent(event, FLAG_FIVE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID071
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID071, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendTimingEvent(EVENT_ID_ONE, FLAG_ONE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID072
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID072, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendTimingEvent(EVENT_ID_TWO, FLAG_TWO, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID073
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID073, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendTimingEvent(EVENT_ID_THREE, FLAG_THREE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID074
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID074, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendTimingEvent(EVENT_ID_FOUR, FLAG_FOUR, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FOUR);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID075
 * @tc.desc: check whether an event with the given ID can not be found among the events that have been
 *           sent and processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID075, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    handler->SendTimingEvent(EVENT_ID_FIVE, FLAG_FIVE, TestEventHandler::Priority::IMMEDIATE);
    usleep(DELAYWAITTIME);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_ID_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithID076
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithID076, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);

    auto event = InnerEvent::Get(EVENT_ID);

    bool hasInnerEvent = runner->GetEventQueue()->HasInnerEvent(nullptr, EVENT_ID);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam001
 * @tc.desc: check whether an event with the given param can be found among the events that have been
 *           sent but not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithParam001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, EVENT_PARAM_ONE);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::LOW);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_ONE);
    EXPECT_TRUE(hasInnerEvent);
    usleep(DELAYWAITTIME);
    hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_ONE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam002
 * @tc.desc: check whether an event with the given param can be found among the events that have been
 *           sent but not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithParam002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, EVENT_PARAM_TWO);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::HIGH);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_TWO);
    EXPECT_TRUE(hasInnerEvent);
    usleep(DELAYWAITTIME);
    hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_TWO);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam003
 * @tc.desc: check whether an event with the given param can be found among the events that have been
 *           sent but not processed.
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithParam003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, EVENT_PARAM_THREE);

    handler->SendEvent(event, DELAY_TIME, TestEventHandler::Priority::IMMEDIATE);
    bool hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_THREE);
    EXPECT_TRUE(hasInnerEvent);
    usleep(DELAYWAITTIME);
    hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_THREE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam004
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithParam004, TestSize.Level1)
{
    auto handler = std::make_shared<EventHandler>(nullptr);
    auto event = InnerEvent::Get(EVENT_PARAM_FOUR);

    bool hasInnerEvent = handler->HasInnerEvent(EVENT_PARAM_FOUR);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam005
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, HasEventWithParam005, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);

    auto event = InnerEvent::Get(EVENT_PARAM_FIVE);

    bool hasInnerEvent = runner->GetEventQueue()->HasInnerEvent(nullptr, EVENT_PARAM_FIVE);
    EXPECT_FALSE(hasInnerEvent);
}

/*
 * @tc.name: RemoveAllEvents001
 * @tc.desc: remove all the events which belong to one handler
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveAllEvents001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    if (handler->PostTask(f, DELAY_TIME, EventHandler::Priority::LOW)) {
        handler->RemoveAllEvents();
        usleep(DELAYWAITTIME);
        auto called = taskCalled.load();
        EXPECT_FALSE(called);
    }
}

/*
 * @tc.name: RemoveAllEvents002
 * @tc.desc: remove all the events which belong to one handler
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveAllEvents002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    if (handler->PostTask(f, DELAY_TIME, EventHandler::Priority::HIGH)) {
        handler->RemoveAllEvents();
        usleep(DELAYWAITTIME);
        auto called = taskCalled.load();
        EXPECT_FALSE(called);
    }
}

/*
 * @tc.name: RemoveAllEvents003
 * @tc.desc: remove all the events which belong to one handler
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveAllEvents003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    if (handler->PostTask(f, DELAY_TIME, EventHandler::Priority::IMMEDIATE)) {
        handler->RemoveAllEvents();
        usleep(DELAYWAITTIME);
        auto called = taskCalled.load();
        EXPECT_FALSE(called);
    }
}

/*
 * @tc.name: RemoveAllEvents004
 * @tc.desc: remove all the events which belong to one handler
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveAllEvents004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    if (handler->PostTask(f, DELAY_TIME, EventHandler::Priority::IDLE)) {
        handler->RemoveAllEvents();
        usleep(DELAYWAITTIME);
        auto called = taskCalled.load();
        EXPECT_FALSE(called);
    }
}

/*
 * @tc.name: RemoveAllEvents005
 * @tc.desc: check when runner is null ptr remove all events process fail
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveAllEvents005, TestSize.Level1)
{
    auto handler = std::make_shared<EventHandler>(nullptr);
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    if (handler->PostTask(f, DELAY_TIME, EventHandler::Priority::IMMEDIATE)) {
        handler->RemoveAllEvents();
        usleep(DELAYWAITTIME);
        auto called = taskCalled.load();
        EXPECT_TRUE(called);
    }
}

/*
 * @tc.name: RemoveEvent001
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventID001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::LOW);
    handler->RemoveEvent(EVENT_ID);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent002
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventID002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::IMMEDIATE);
    handler->RemoveEvent(EVENT_ID);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent003
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventID003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::HIGH);
    handler->RemoveEvent(EVENT_ID);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent004
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventID004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::IDLE);
    handler->RemoveEvent(EVENT_ID);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent005
 * @tc.desc: check when runner is null ptr remove event process fail
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventID005, TestSize.Level1)
{
    auto handler = std::make_shared<EventHandler>(nullptr);
    handler->RemoveEvent(EVENT_ID);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent001
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventIDParam001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, FLAG_ONE);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::LOW);
    handler->RemoveEvent(EVENT_ID, FLAG_ONE);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent002
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventIDParam002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, FLAG_TWO);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::IMMEDIATE);
    handler->RemoveEvent(EVENT_ID, FLAG_TWO);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent003
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventIDParam003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, FLAG_THREE);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::HIGH);
    handler->RemoveEvent(EVENT_ID, FLAG_THREE);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent004
 * @tc.desc: remove event success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventIDParam004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, FLAG_FOUR);

    handler->SendEvent(event, DELAY_TIME, EventHandler::Priority::IDLE);
    handler->RemoveEvent(EVENT_ID, FLAG_FOUR);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent005
 * @tc.desc: check when runner is null ptr remove event process fail
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, RemoveEventIDParam005, TestSize.Level1)
{
    auto handler = std::make_shared<EventHandler>(nullptr);
    handler->RemoveEvent(EVENT_ID, FLAG_FIVE);
    usleep(DELAYWAITTIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: IsIdle
 * @tc.desc: check when idle IsIdle return true
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, IsIdle001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    bool ret = handler->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsIdle
 * @tc.desc: check when idle IsIdle return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, IsIdle002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    handler->PostTask(task, FLAG_ONE, EventHandler::Priority::IMMEDIATE);
    bool ret = handler->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsIdle
 * @tc.desc: check when idle IsIdle return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, IsIdle003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    handler->PostTask(task, FLAG_TWO, EventHandler::Priority::LOW);
    bool ret = handler->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsIdle
 * @tc.desc: check when idle IsIdle return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, IsIdle004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    handler->PostTask(task, FLAG_THREE, EventHandler::Priority::HIGH);
    bool ret = handler->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsIdle
 * @tc.desc: check when idle IsIdle return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, IsIdle005, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    handler->PostTask(task, FLAG_THREE, EventHandler::Priority::IDLE);
    bool ret = handler->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: Dump001
 * @tc.desc: Check Dump
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, Dump001, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event = InnerEvent::Get(task, THREAD_NAME_TEST4);
    DumpTest dumptest;

    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump002
 * @tc.desc: Check Dump after post task
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, Dump002, TestSize.Level1)
{
    g_isDump = false;
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    DumpTest dumptest;

    handler->PostTask(task, DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump003
 * @tc.desc: Check Dump after send event with event id
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, Dump003, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID);
    DumpTest dumptest;
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump004
 * @tc.desc: Check Dump after send event with event id and param
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, Dump004, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, FLAG_FOUR);
    DumpTest dumptest;
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump005
 * @tc.desc: check when send event and post task dump success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, Dump005, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, FLAG_FIVE);
    auto task = []() { ; };
    DumpTest dumptest;
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    handler->PostTask(task, DELAY_TIME * 2, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(g_isDump);
}

/**
 * @tc.name: DistributeEvent001
 * @tc.desc: Distribute an event
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, DistributeEvent001, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    handler->DistributeEvent(event);
    bool result = CommonUtils::EventRunGet();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DistributeEvent002
 * @tc.desc: Distribute an event
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, DistributeEvent002, TestSize.Level1)
{
    auto event = InnerEvent::Get();
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    handler->DistributeEvent(event);
    bool result = CommonUtils::EventRunGet();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DistributeEvent003
 * @tc.desc: Distribute an event
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, DistributeEvent003, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID, FLAG_ONE);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    handler->DistributeEvent(event);
    bool result = CommonUtils::EventRunGet();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DistributeEvent004
 * @tc.desc: Distribute a task
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, DistributeEvent004, TestSize.Level1)
{
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto event = InnerEvent::Get(f);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    handler->DistributeEvent(event);
    bool result = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: DistributeEvent005
 * @tc.desc: Distribute a null event
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, DistributeEvent005, TestSize.Level1)
{
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
 */
HWTEST_F(EmsEventHandlerTest, CurrentHandler001, TestSize.Level1)
{
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler;
    auto f = [&]() { myHandler = handler->Current(); };

    handler->PostTask(f);
    uint32_t sleepTime = 20000;
    usleep(sleepTime);
    EXPECT_EQ(handler, myHandler);
}

/**
 * @tc.name: CurrentHandler002
 * @tc.desc: Get current handler when handler running
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, CurrentHandler002, TestSize.Level1)
{
    auto myRunner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler;
    auto f = [&]() { myHandler = handler->Current(); };

    handler->PostTask(f);
    uint32_t sleepTime = 20000;
    usleep(sleepTime);
    EXPECT_EQ(handler, myHandler);
}

/**
 * @tc.name: CurrentHandler003
 * @tc.desc: Get current handler when handler running
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, CurrentHandler003, TestSize.Level1)
{
    auto myRunner = EventRunner::Create(THREAD_NAME_TEST1);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler;
    auto f = [&]() { myHandler = handler->Current(); };

    handler->PostTask(f);
    uint32_t sleepTime = 20000;
    usleep(sleepTime);
    EXPECT_EQ(handler, myHandler);
}

/**
 * @tc.name: CurrentHandler004
 * @tc.desc: Get current handler when handler not running
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, CurrentHandler004, TestSize.Level1)
{
    auto myRunner = EventRunner::Create();
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler = handler->Current();

    EXPECT_EQ(nullptr, myHandler);
}

/**
 * @tc.name: CurrentHandler005
 * @tc.desc: Get current handler when handler running
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventHandlerTest, CurrentHandler005, TestSize.Level1)
{
    auto myRunner = EventRunner::Create(THREAD_NAME_TEST2);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    std::shared_ptr<EventHandler> myHandler;
    auto f = [&]() { myHandler = handler->Current(); };

    handler->PostTask(f);
    uint32_t sleepTime = 20000;
    usleep(sleepTime);
    EXPECT_EQ(handler, myHandler);
}
