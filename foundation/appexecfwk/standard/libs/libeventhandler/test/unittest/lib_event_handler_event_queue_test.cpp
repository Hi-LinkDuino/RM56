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

#include <chrono>
#include <thread>

#include <fcntl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "event_handler.h"
#include "event_queue.h"
#include "event_runner.h"
#include "inner_event.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const size_t MAX_PRIORITY_NUM = 4;
const size_t MAX_HIGH_PRIORITY_COUNT = 5;
const uint32_t NUM = 2;
const uint32_t HIGH_PRIORITY_COUNT = 12;
const uint32_t LOW_PRIORITY_COUNT = 2;
const uint32_t IMMEDIATE_PRIORITY_COUNT = 72;
const int64_t DELAY_TIME = 100;
const int64_t REMOVE_DELAY_TIME = 10;
const int64_t HAS_DELAY_TIME = 10;
const int64_t REMOVE_WAIT_TIME = 20000;
const uint32_t REMOVE_EVENT_ID = 0;
const uint32_t HAS_EVENT_ID = 100;
const int64_t HAS_EVENT_PARAM = 1000;
const uint32_t INSERT_DELAY = 10;
bool isDump = false;

std::atomic<bool> eventRan(false);
}  // namespace

class DumpTest : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {
        isDump = true;
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

/**
 * Init FileDescriptor.
 *
 * @param fds[] pipe need.
 * @return Returns fileDescriptor we get.
 */
static int32_t InitFileDescriptor(int32_t fds[])
{
    auto result = pipe(fds);
    EXPECT_GE(result, 0);

    int32_t fileDescriptor = fds[0];
    return fileDescriptor;
}

/**
 * get event from queue and compare.
 *
 * @param eventId of the event we want to get.
 * @param queue we get event from this queue.
 */
static void GetEventAndCompare(uint32_t eventId, EventQueue &queue)
{
    auto event = queue.GetEvent();
    EXPECT_NE(nullptr, event);
    if (event != nullptr) {
        auto id = event->GetInnerEventId();
        EXPECT_EQ(eventId, id);
    }
}

/**
 * set event handler time delay.
 *
 * @param delayTime of the event handle time.
 */
static void DelayTest(uint8_t delayTime)
{
    const uint8_t longDelta = 20;
    const uint8_t shortDelta = 5;
    uint32_t eventId = 0;
    uint8_t maxDelta = shortDelta;
    if (delayTime > 0) {
        maxDelta = longDelta;
    }

    EventQueue queue;
    queue.Prepare();
    auto event = InnerEvent::Get(eventId);
    auto now = InnerEvent::Clock::now();
    // delay event handle time delayTime ms
    auto handleTime = now + std::chrono::milliseconds(static_cast<int64_t>(delayTime));
    event->SetSendTime(now);
    event->SetHandleTime(handleTime);
    queue.Insert(event);
    event = queue.GetEvent();
    // block until get event from queue after delay time
    now = InnerEvent::Clock::now();
    EXPECT_GE(now, handleTime);
    // check if delay time is within acceptable error
    auto errorTime = handleTime + std::chrono::milliseconds(static_cast<int64_t>(maxDelta));
    EXPECT_LE(now, errorTime);
    EXPECT_NE(event, nullptr);
    if (event != nullptr) {
        auto id = event->GetInnerEventId();
        EXPECT_EQ(eventId, id);
    }
}

/**
 * Insert event and get event from queue.
 *
 * @param priorities[] prioritiesof event.
 * @param priorityCount count of event we insert.
 */
static void InsertPriorityTest(const EventQueue::Priority priorities[], size_t priorityCount)
{
    std::list<uint32_t> eventIds;
    auto now = InnerEvent::Clock::now();
    EventQueue queue;
    queue.Prepare();
    uint32_t eventId = 0;

    // insert event into queue from IDLE priority to IMMEDIATE priority
    for (size_t i = 0; i < priorityCount; ++i) {
        eventIds.push_back(eventId);
        auto event = InnerEvent::Get(eventId);
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, priorities[i]);
        ++eventId;
    }

    // get event from queue and check eventId
    for (size_t i = 0; i < priorityCount; ++i) {
        auto event = queue.GetEvent();
        EXPECT_NE(nullptr, event);
        if (event == nullptr) {
            break;
        }

        if (priorities[0] == EventQueue::Priority::IDLE) {
            auto storeId = eventIds.back();
            auto id = event->GetInnerEventId();
            EXPECT_EQ(storeId, id);
            eventIds.pop_back();
        } else {
            auto storeId = eventIds.front();
            auto id = event->GetInnerEventId();
            EXPECT_EQ(storeId, id);
            eventIds.pop_front();
        }
    }
}

/**
 * Break event queue.
 *
 * @param queue we get break.
 * @param eventId eventId of event we insert.
 */
static void BreakQueueTest(EventQueue &queue, uint32_t eventId)
{
    auto event = queue.GetEvent();
    EXPECT_NE(nullptr, event);
    if (event != nullptr) {
        auto id = event->GetInnerEventId();
        EXPECT_EQ(eventId, id);
        queue.Finish();
        queue.Insert(event);
        event = queue.GetEvent();
        EXPECT_EQ(nullptr, event);
    }
}

/**
 * Insert event into queue and get event.
 *
 * @param queue we get event from this queue.
 * @param event event we insert into queue.
 */
static void InsertAndGet(EventQueue &queue, InnerEvent::Pointer &event)
{
    // insert event before prepare queue
    queue.Insert(event);
    event = queue.GetEvent();
    EXPECT_EQ(nullptr, event);
    if (event != nullptr) {
        // If event is not nullptr, the queue must be empty, so need to insert it again.
        queue.Insert(event);
    }
}

/**
 * Insert event and get event from queue.
 *
 * @param queue we insert event into this queue.
 * @param length length of events.
 */
static void InsertPriorityEvent(EventQueue &queue, size_t length)
{
    // insert two low priority events
    for (uint32_t eventId = 0; eventId < NUM; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::LOW);
    }

    // avoid time accuracy problem
    usleep(INSERT_DELAY);

    // insert MAX_HIGH_PRIORITY_COUNT high priority events
    for (uint32_t eventId = NUM; eventId < NUM * length + NUM; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::HIGH);
    }
}

/**
 * Insert all priority event and get event from queue.
 *
 * @param queue we insert event into this queue.
 */
static void InsertAllPriorityEvent(EventQueue &queue)
{
    // insert low priority events
    for (uint32_t eventId = 0; eventId < LOW_PRIORITY_COUNT; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::LOW);
    }

    // avoid time accuracy problem
    usleep(INSERT_DELAY);

    // insert high priority events
    for (uint32_t eventId = LOW_PRIORITY_COUNT; eventId < HIGH_PRIORITY_COUNT; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::HIGH);
    }

    // avoid time accuracy problem
    usleep(INSERT_DELAY);

    // insert immediate priority events
    for (uint32_t eventId = HIGH_PRIORITY_COUNT; eventId < IMMEDIATE_PRIORITY_COUNT; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::IMMEDIATE);
    }
}

class LibEventHandlerEventQueueTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void LibEventHandlerEventQueueTest::SetUpTestCase(void)
{}

void LibEventHandlerEventQueueTest::TearDownTestCase(void)
{}

void LibEventHandlerEventQueueTest::SetUp(void)
{
    /**
     * @tc.setup: reset the eventRan value.
     */
    eventRan.store(false);
}

void LibEventHandlerEventQueueTest::TearDown(void)
{}

class MyEventHandler : public EventHandler {
public:
    explicit MyEventHandler(const std::shared_ptr<EventRunner> &runner) : EventHandler(runner)
    {}
    ~MyEventHandler()
    {}

    void ProcessEvent(const InnerEvent::Pointer &) override
    {
        eventRan.store(true);
    }

    MyEventHandler(const MyEventHandler &) = delete;
    MyEventHandler &operator=(const MyEventHandler &) = delete;
    MyEventHandler(MyEventHandler &&) = delete;
    MyEventHandler &operator=(MyEventHandler &&) = delete;
};

class MyFileDescriptorListener : public FileDescriptorListener {
public:
    MyFileDescriptorListener()
    {}
    ~MyFileDescriptorListener()
    {}

    /* @param int32_t fileDescriptor */
    void OnReadable(int32_t)
    {}

    /* @param int32_t fileDescriptor */
    void OnWritable(int32_t)
    {}

    /* @param int32_t fileDescriptor */
    void OnException(int32_t)
    {}

    MyFileDescriptorListener(const MyFileDescriptorListener &) = delete;
    MyFileDescriptorListener &operator=(const MyFileDescriptorListener &) = delete;
    MyFileDescriptorListener(MyFileDescriptorListener &&) = delete;
    MyFileDescriptorListener &operator=(MyFileDescriptorListener &&) = delete;
};

/*
 * @tc.name: WakeAndBreak001
 * @tc.desc: check events inserted in queue when Prepare() and
 *           Finish() are called in right order
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, WakeAndBreak001, TestSize.Level1)
{
    /**
     * @tc.setup: get event and queue.
     */
    EventQueue queue;
    uint32_t eventId = 0;
    auto event = InnerEvent::Get(eventId);

    /**
     * @tc.steps: step1. prepare queue and inserted in queue when Prepare() and Finish() are called in right order.
     * @tc.expected: step1. event and event id is valid when Prepare() and Finish() are called in right order.
     */
    queue.Prepare();
    queue.Insert(event);
    BreakQueueTest(queue, eventId);
}

/*
 * @tc.name: WakeAndBreak002
 * @tc.desc: check events inserted in queue when queue is prepared
 *           and broken in wrong order
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, WakeAndBreak002, TestSize.Level1)
{
    /**
     * @tc.setup: get event and queue.
     */
    EventQueue queue;
    uint32_t eventId = 0;
    auto event = InnerEvent::Get(eventId);

    /**
     * @tc.steps: step1. prepare queue and inserted in queue when Prepare() and Finish() are called in wrong order.
     * @tc.expected: step1. event and event id is invalid when Prepare() and Finish() are called in wrong order.
     */
    InsertAndGet(queue, event);
    queue.Prepare();
    BreakQueueTest(queue, eventId);
}

/*
 * @tc.name: WakeAndBreak003
 * @tc.desc: check events inserted in queue when queue is broken
 *           and prepared in wrong order
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, WakeAndBreak003, TestSize.Level1)
{
    /**
     * @tc.setup: get event and queue.
     */
    EventQueue queue;
    uint32_t eventId = 0;
    auto event = InnerEvent::Get(eventId);

    InsertAndGet(queue, event);

    /**
     * @tc.steps: step1. get and check event Finish() is called in wrong order.
     * @tc.expected: step1. event is null.
     */
    queue.Finish();
    event = queue.GetEvent();
    EXPECT_EQ(nullptr, event);
    if (event != nullptr) {
        // If event is not nullptr, the queue must be empty, so need to insert it again.
        queue.Insert(event);
    }

    /**
     * @tc.steps: step2. prepare queue and get event from queue when Prepare() is called in right order.
     * @tc.expected: step2. event and event id is invalid .
     */
    queue.Prepare();
    GetEventAndCompare(eventId, queue);
}

/*
 * @tc.name: WakeAndBreak004
 * @tc.desc: check events inserted in queue and get event by function GetExpiredEvent
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, WakeAndBreak004, TestSize.Level1)
{
    /**
     * @tc.setup: get event and queue.
     */
    EventQueue myQueue;
    uint32_t eventId = 0;
    auto event = InnerEvent::Get(eventId);
    InsertAndGet(myQueue, event);

    /**
     * @tc.steps: step1. get and check event Finish() is called in wrong order.
     * @tc.expected: step1. event is null.
     */
    myQueue.Finish();
    event = myQueue.GetEvent();
    EXPECT_EQ(nullptr, event);
    if (event != nullptr) {
        // If event is not nullptr, the queue must be empty, so need to insert it again.
        myQueue.Insert(event);
    }

    /**
     * @tc.steps: step2. prepare queue and get event from queue when Prepare() is called in right order.
     * @tc.expected: step2. event and event id is invalid .
     */
    myQueue.Prepare();
    InnerEvent::TimePoint nextWakeUpTime = InnerEvent::TimePoint::max();
    auto resultEvent = myQueue.GetExpiredEvent(nextWakeUpTime);
    EXPECT_NE(nullptr, resultEvent);
    EXPECT_EQ(eventId, resultEvent->GetInnerEventId());
}

/*
 * @tc.name: InsertEvent001
 * @tc.desc: insert() event of different priorities into Queue,
 *           from IDLE to IMMEDIATE.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent001, TestSize.Level1)
{
    /**
     * @tc.setup: init priority array, insert event with the order int the array.
     */
    const EventQueue::Priority priorities[MAX_PRIORITY_NUM] = {
        EventQueue::Priority::IDLE,
        EventQueue::Priority::LOW,
        EventQueue::Priority::HIGH,
        EventQueue::Priority::IMMEDIATE,
    };

    /**
     * @tc.steps: step1. insert and get event, check whether the order of the event
     *            we get from queue is the same as we expect.
     * @tc.expected: step1. the order is the same as we expect.
     */
    InsertPriorityTest(priorities, MAX_PRIORITY_NUM);
}

/*
 * @tc.name: InsertEvent002
 * @tc.desc: insert() event of different priorities into Queue,
 *           from IMMEDIATE to IDLE
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent002, TestSize.Level1)
{
    /**
     * @tc.setup: init priority array, insert event with the order int the array.
     */
    const EventQueue::Priority priorities[MAX_PRIORITY_NUM] = {
        EventQueue::Priority::IMMEDIATE,
        EventQueue::Priority::HIGH,
        EventQueue::Priority::LOW,
        EventQueue::Priority::IDLE,
    };

    /**
     * @tc.steps: step1. insert and get event, check whether the order of the event
     *            we get from queue is the same as we expect.
     * @tc.expected: step1. the order is the same as we expect.
     */
    InsertPriorityTest(priorities, MAX_PRIORITY_NUM);
}

/*
 * @tc.name: InsertEvent003
 * @tc.desc: insert nullptr event and normal event into queue,
 *           then get event from queue
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent003, TestSize.Level1)
{
    /**
     * @tc.setup: prepare queue. and insert event into queue, insert event with the order int the array.
     */
    uint32_t eventId = 0;
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. insert nullptr event and insert normal event into queue, and get event from queue, check
     *            whether the event we get from queue is valid as we expect.
     * @tc.expected: step1. the event we get after we insert normal event into queue is valid.
     */
    auto event = InnerEvent::Pointer(nullptr, nullptr);
    queue.Insert(event);
    event = InnerEvent::Get(eventId);
    queue.Insert(event);
    GetEventAndCompare(eventId, queue);
}

/*
 * @tc.name: InsertEvent004
 * @tc.desc: avoid starvation in queue when insert event of
 *           different priorities into Queue
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent004, TestSize.Level1)
{
    /**
     * @tc.setup: prepare queue.
     */
    const uint32_t num = 3;
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. first insert MAX_HIGH_PRIORITY_COUNT high priority events, then insert two low priority events.
     */
    for (uint32_t eventId = 0; eventId < MAX_HIGH_PRIORITY_COUNT + 1; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::HIGH);
    }

    for (uint32_t eventId = MAX_HIGH_PRIORITY_COUNT + 1; eventId < MAX_HIGH_PRIORITY_COUNT + num; eventId++) {
        auto event = InnerEvent::Get(eventId);
        auto now = InnerEvent::Clock::now();
        event->SetSendTime(now);
        event->SetHandleTime(now);
        queue.Insert(event, EventQueue::Priority::LOW);
    }

    /**
     * @tc.steps: step2. get event from queue one by one, and check whether the event id we get from queue is the
     *            same as we expect.
     * @tc.expected: step2. event id we get from queue is the same as we expect.
     */
    for (uint32_t eventId = 0; eventId < MAX_HIGH_PRIORITY_COUNT + num; eventId++) {
        GetEventAndCompare(eventId, queue);
    }
}

/*
 * @tc.name: InsertEvent005
 * @tc.desc: avoid starvation in queue when insert event of different priorities into Queue.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent005, TestSize.Level1)
{
    /**
     * @tc.setup: prepare queue.
     */
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. first insert two low priority events, insert MAX_HIGH_PRIORITY_COUNT high priority events.
     */
    InsertPriorityEvent(queue, MAX_HIGH_PRIORITY_COUNT);

    /**
     * @tc.steps: step2. get MAX_HIGH_PRIORITY_COUNT events from queue, and compare the event id.
     * @tc.expected: step2. event we get is high priority events.
     */
    for (uint32_t eventId = 2; eventId < MAX_HIGH_PRIORITY_COUNT + NUM; eventId++) {
        GetEventAndCompare(eventId, queue);
    }

    /**
     * @tc.steps: step3. get one event from queue, and compare the event id .
     * @tc.expected: step3. event we get is low priority events.
     */
    uint32_t lowEventId = 0;
    GetEventAndCompare(lowEventId, queue);

    /**
     * @tc.steps: step4. get MAX_HIGH_PRIORITY_COUNT events from queue, and compare the event id .
     * @tc.expected: step4. event we get is high priority events.
     */
    for (uint32_t eventId = MAX_HIGH_PRIORITY_COUNT + NUM; eventId < NUM * MAX_HIGH_PRIORITY_COUNT + NUM; eventId++) {
        GetEventAndCompare(eventId, queue);
    }

    /**
     * @tc.steps: step5. get one event from queue, and compare the event id .
     * @tc.expected: step5. event we get is low priority events.
     */
    lowEventId = 1;
    GetEventAndCompare(lowEventId, queue);
}

/*
 * @tc.name: InsertEvent006
 * @tc.desc: avoid starvation in queue when insert event of different priorities into queue.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent006, TestSize.Level1)
{
    /**
     * @tc.setup: prepare queue.
     */
    const uint32_t count = 5;
    const uint32_t highEventCount = 6;
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. insert events from low priority to immediate priority into queue.
     */
    InsertAllPriorityEvent(queue);

    uint32_t highCount = 1;
    uint32_t highEventId = 0;
    uint32_t lowCount = 0;
    uint32_t immediateCount = 1;

    /**
     * @tc.steps: step2. get events from queue, and compare the event id .
     * @tc.expected: step2. first we get five immediate priority events , then get one high priority event, every five
     *               high priority events, we will get one low priority event.
     */
    for (uint32_t eventId = 0; eventId < IMMEDIATE_PRIORITY_COUNT - HIGH_PRIORITY_COUNT; eventId++) {
        if (immediateCount % count == 0) {
            GetEventAndCompare(HIGH_PRIORITY_COUNT + eventId, queue);
            immediateCount++;
            if (highCount % highEventCount == 0) {
                GetEventAndCompare(lowCount, queue);
                lowCount++;
                highCount++;
            } else {
                GetEventAndCompare(LOW_PRIORITY_COUNT + highEventId, queue);
                highCount++;
                highEventId++;
            }
        } else {
            GetEventAndCompare(HIGH_PRIORITY_COUNT + eventId, queue);
            immediateCount++;
        }
    }
}

/*
 * @tc.name: InsertEvent007
 * @tc.desc: delay event handle time, get event after delaytime
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent007, TestSize.Level1)
{
    const uint8_t delayTime = 100;
    /**
     * @tc.steps: step1. insert event into queue and set handle time delay 100ms from now, then get event from queue.
     * @tc.expected: step1. the delay time we get event from queue is about 100ms with tolerable error.
     */
    DelayTest(delayTime);
}

/*
 * @tc.name: InsertEvent008
 * @tc.desc: delayTime = 0, send event and get event from queue
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, InsertEvent008, TestSize.Level1)
{
    const uint8_t delayTime = 0;
    /**
     * @tc.steps: step1. insert event into queue and set handle time delay 0ms from now, then get event from queue.
     * @tc.expected: step1. the delay time we get event from queue is about 0ms with tolerable error.
     */
    DelayTest(delayTime);
}

/*
 * @tc.name: RemoveEvent001
 * @tc.desc: remove all the events which belong to one handler
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, RemoveEvent001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    /**
     * @tc.steps: step1. post a task with delay time, then remove this task ,check whether the task is executed
     *            after delay time passed.
     * @tc.expected: step1. the task is not executed after delay time.
     */
    if (handler->PostTask(f, REMOVE_DELAY_TIME, EventQueue::Priority::LOW)) {
        handler->RemoveAllEvents();
        usleep(REMOVE_WAIT_TIME);
        auto called = taskCalled.load();
        EXPECT_FALSE(called);
    }
}

/*
 * @tc.name: RemoveEvent002
 * @tc.desc: remove all the events which belong to one handler with same id
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, RemoveEvent002, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(runner);
    auto event = InnerEvent::Get(REMOVE_EVENT_ID);

    /**
     * @tc.steps: step1. send an event with delay time, then remove this event with event id,
     *                   then check whether the task is executed after delay time.
     * @tc.expected: step1. the task is not executed after delay time.
     */
    handler->SendEvent(event, REMOVE_DELAY_TIME, EventQueue::Priority::LOW);
    handler->RemoveEvent(REMOVE_EVENT_ID);
    usleep(REMOVE_WAIT_TIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent003
 * @tc.desc: remove all the events which belong to one handler with same id and param
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, RemoveEvent003, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */
    int64_t eventParam = 0;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(runner);
    auto event = InnerEvent::Get(REMOVE_EVENT_ID, eventParam);

    /**
     * @tc.steps: step1. send an event with delay time, then remove this event with event id and param,
     *                   then check whether the task is executed after delay time.
     * @tc.expected: step1. the task is not executed after delay time.
     */
    handler->SendEvent(event, REMOVE_DELAY_TIME, EventQueue::Priority::LOW);
    handler->RemoveEvent(REMOVE_EVENT_ID, eventParam);
    usleep(REMOVE_WAIT_TIME);
    auto ran = eventRan.load();
    EXPECT_FALSE(ran);
}

/*
 * @tc.name: RemoveEvent004
 * @tc.desc: remove events with task from queue
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, RemoveEvent004, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, get event with callback and name.
     */
    int64_t delayTime = 5;
    int64_t delayWaitTime = 10000;
    std::string taskName("taskName");
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(f, taskName);

    /**
     * @tc.steps: step1. send an event with delay time, then remove this event with taskname,
     *                   then check whether the task is executed after delay time.
     * @tc.expected: step1. the task is not executed after delay time.
     */
    handler->SendEvent(event, delayTime, EventQueue::Priority::LOW);
    handler->RemoveTask(taskName);
    usleep(delayWaitTime);
    auto called = taskCalled.load();
    EXPECT_FALSE(called);
}

/*
 * @tc.name: NotifyQueue001
 * @tc.desc: wake up the queue which is blocked when we need to execute a task
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, NotifyQueue001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */
    std::atomic<bool> taskCalled(false);
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    /**
     * @tc.steps: step1. post a delay task to block handler thread, then new a thread to post a task to wake up the
     *            blocked handler.
     * @tc.expected: step1. the task is executed as expect.
     */
    auto mainTask = [&taskCalled, &runner]() {
        taskCalled.store(false);
        runner->Stop();
    };
    handler->PostTask(mainTask, DELAY_TIME);
    auto f = [&taskCalled, &handler]() {
        usleep(10000);
        auto task = [&taskCalled]() { taskCalled.store(true); };
        handler->PostTask(task);
        usleep(10000);
        auto called = taskCalled.load();
        EXPECT_TRUE(called);
    };
    std::thread newThread(f);
    newThread.detach();
    runner->Run();
    auto called = taskCalled.load();
    EXPECT_FALSE(called);
}

/*
 * @tc.name: NotifyQueue002
 * @tc.desc: add FileDescriptor and wake up the queue with epoll which is blocked when we need to execute a task
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, NotifyQueue002, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */
    std::atomic<bool> taskCalled(false);
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    /**
     * @tc.steps: step1. add file descripter listener to handler, handler will use epoll to wake up block thread.
     */
    int32_t fds[] = {-1, -1};
    int32_t fileDescriptor = InitFileDescriptor(fds);
    uint32_t event = 1;

    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    handler->AddFileDescriptorListener(fileDescriptor, event, fileDescriptorListener);

    /**
     * @tc.steps: step2. post a delay task to block handler thread, then new a thread to post a task to wake up the
     *            blocked handler.
     * @tc.expected: step2. the task is executed as expect.
     */
    auto mainThreadTask = [&taskCalled, &runner]() {
        taskCalled.store(false);
        runner->Stop();
    };
    handler->PostTask(mainThreadTask, DELAY_TIME);
    auto newThreadTask = [&taskCalled, &handler]() {
        usleep(10000);
        auto tempTask = [&taskCalled]() { taskCalled.store(true); };
        handler->PostTask(tempTask);
        usleep(10000);
        auto called = taskCalled.load();
        EXPECT_TRUE(called);
    };
    std::thread newThread(newThreadTask);
    newThread.detach();
    runner->Run();
    auto called = taskCalled.load();
    EXPECT_FALSE(called);

    /**
     * @tc.steps: step3. remove file descripter listener and close pipe.
     */
    handler->RemoveFileDescriptorListener(fileDescriptor);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: NotifyQueue003
 * @tc.desc: wake up the queue with epoll which is blocked when we need to execute a task
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, NotifyQueue003, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */
    std::atomic<bool> taskCalled(false);
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    auto called = taskCalled.load();
    auto main = [&taskCalled, &runner]() {
        taskCalled.store(false);
        runner->Stop();
    };

    /**
     * @tc.steps: step1. post delay task to block handler.
     */
    handler->PostTask(main, DELAY_TIME);
    int32_t fds[] = {-1, -1};
    int32_t fileDescriptor = InitFileDescriptor(fds);
    uint32_t event = 1;

    /**
     * @tc.steps: step2. new a thread to post a delay task to add file descriptor listener to handler,
     *            then post a new task.
     * @tc.expected: step2. all the task is executed as expect.
     */
    auto newTask = [&handler, &fileDescriptor, &event, &fileDescriptorListener, &taskCalled]() {
        usleep(10000);
        handler->AddFileDescriptorListener(fileDescriptor, event, fileDescriptorListener);
        usleep(10000);
        auto newCalled = taskCalled.load();
        EXPECT_FALSE(newCalled);
        auto innerTask = [&taskCalled]() { taskCalled.store(true); };
        handler->PostTask(innerTask);
        usleep(10000);
        newCalled = taskCalled.load();
        EXPECT_TRUE(newCalled);
    };
    std::thread newThread(newTask);
    newThread.detach();
    runner->Run();
    called = taskCalled.load();
    EXPECT_FALSE(called);

    /**
     * @tc.steps: step3. remove file descripter listener and close pipe.
     */
    handler->RemoveFileDescriptorListener(fileDescriptor);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: RemoveOrphan001
 * @tc.desc: Remove event without owner, and check remove result
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, RemoveOrphan001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init orphan handler and post a task.
     */
    std::atomic<bool> orphanTaskCalled(false);
    std::atomic<bool> commonTaskCalled(false);
    auto runner = EventRunner::Create(false);
    auto orphanHandler = std::make_shared<EventHandler>(runner);
    auto g = [&orphanTaskCalled]() { orphanTaskCalled.store(true); };
    orphanHandler->PostTask(g);

    /**
     * @tc.steps: step2. init common handler and post a task.
     */
    auto commonHandler = std::make_shared<EventHandler>(runner);
    auto f = [&commonTaskCalled, &runner]() {
        commonTaskCalled.store(true);
        runner->Stop();
    };
    commonHandler->PostTask(f);

    /**
     * @tc.steps: step3. reset orphan handler and start runner.
     * @tc.expected: step3. the task post through orphan handler is not executed, the task
     *               post through common handler is executed.
     */
    orphanHandler.reset();
    usleep(10000);
    runner->Run();
    auto orphanCalled = orphanTaskCalled.load();
    EXPECT_FALSE(orphanCalled);
    auto commonCalled = commonTaskCalled.load();
    EXPECT_TRUE(commonCalled);
}

/*
 * @tc.name: AddAndRemoveFileDescriptorListener001
 * @tc.desc: add file descriptor listener and remove file descriptor listener with fd
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddAndRemoveFileDescriptorListener001, TestSize.Level1)
{
    /**
     * @tc.setup: init queue and prepare queue.
     */
    EventQueue queue;
    queue.Prepare();

    int32_t fds[] = {-1, -1};
    EXPECT_GE(pipe(fds), 0);

    /**
     * @tc.steps: step1. add file descriptor listener to queue, then remove file descriptor listener with fd,
     *                   close pipe.
     * @tc.expected: step1. add file descriptor listener success.
     */
    int32_t fileDescriptor = fds[0];
    uint32_t event = 1;
    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    auto result = queue.AddFileDescriptorListener(fileDescriptor, event, fileDescriptorListener);
    EXPECT_EQ(result, ERR_OK);
    queue.RemoveFileDescriptorListener(-1);
    queue.RemoveFileDescriptorListener(fileDescriptor);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: AddAndRemoveFileDescriptorListener002
 * @tc.desc: add file descriptor listener and remove file descriptor listener with handler
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddAndRemoveFileDescriptorListener002, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, prepare queue.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. add file descriptor listener to queue, then remove file descriptor listener with handler,
     *                   close pipe.
     * @tc.expected: step1. add file descriptor listener success.
     */
    int32_t fds[] = {-1, -1};
    int32_t fileDescriptor = InitFileDescriptor(fds);
    uint32_t event = 1;

    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    fileDescriptorListener->SetOwner(handler);
    auto result = queue.AddFileDescriptorListener(fileDescriptor, event, fileDescriptorListener);
    EXPECT_EQ(result, ERR_OK);
    queue.RemoveFileDescriptorListener(nullptr);
    queue.RemoveFileDescriptorListener(handler);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: AddFileDescriptorListener001
 * @tc.desc: add file descriptor listener multi times
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddFileDescriptorListener001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, prepare queue.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    EventQueue queue;
    queue.Prepare();

    int32_t fds[] = {-1, -1};
    int32_t fileDescriptor = InitFileDescriptor(fds);
    uint32_t listenEvent = 1;

    /**
     * @tc.steps: step1. add file descriptor listener to queue multi times, then remove file descriptor listener
     *                   with handler, close pipe.
     * @tc.expected: step1. first time add file descriptor listener success, second time failed.
     */
    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    fileDescriptorListener->SetOwner(handler);
    auto result = queue.AddFileDescriptorListener(fileDescriptor, listenEvent, fileDescriptorListener);
    EXPECT_EQ(result, ERR_OK);
    result = queue.AddFileDescriptorListener(fileDescriptor, listenEvent, fileDescriptorListener);
    EXPECT_EQ(result, EVENT_HANDLER_ERR_FD_ALREADY);
    queue.RemoveFileDescriptorListener(handler);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: AddFileDescriptorListener002
 * @tc.desc: add file descriptor listener with wrong type of event
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddFileDescriptorListener002, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, prepare queue.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. add file descriptor listener to queue with wrong type of event,
     *                   then remove file descriptor listener with handler, close pipe.
     * @tc.expected: step1. add file descriptor listener failed.
     */
    int32_t fds[] = {-1, -1};
    int32_t fileDescriptor = InitFileDescriptor(fds);
    uint32_t newEvent = 0;

    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    fileDescriptorListener->SetOwner(handler);
    auto result = queue.AddFileDescriptorListener(fileDescriptor, newEvent, fileDescriptorListener);
    EXPECT_EQ(result, EVENT_HANDLER_ERR_INVALID_PARAM);
    queue.RemoveFileDescriptorListener(handler);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: AddFileDescriptorListener003
 * @tc.desc: add file descriptor listener with nullptr listener function
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddFileDescriptorListener003, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, prepare queue.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. add file descriptor listener to queue with nullptr listener,
     *                   then remove file descriptor listener with handler, close pipe.
     * @tc.expected: step1. add file descriptor listener failed.
     */
    int32_t fds[] = {-1, -1};
    int32_t fileDescriptor = InitFileDescriptor(fds);
    uint32_t event = 1;

    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    fileDescriptorListener->SetOwner(handler);
    auto result = queue.AddFileDescriptorListener(fileDescriptor, event, nullptr);
    EXPECT_EQ(result, EVENT_HANDLER_ERR_INVALID_PARAM);
    queue.RemoveFileDescriptorListener(handler);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: AddFileDescriptorListener004
 * @tc.desc: add file descriptor listener with wrong fd
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddFileDescriptorListener004, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, prepare queue.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    EventQueue queue;
    queue.Prepare();

    /**
     * @tc.steps: step1. add file descriptor listener to queue with wrong pipe, then remove
     *            file descriptor listener with handler, close pipe.
     * @tc.expected: step1. add file descriptor listener failed.
     */
    int32_t fds[] = {-1, -1};

    int32_t fileDescriptor = fds[0];
    uint32_t event = 1;
    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    fileDescriptorListener->SetOwner(handler);
    auto result = queue.AddFileDescriptorListener(fileDescriptor, event, fileDescriptorListener);
    EXPECT_EQ(result, EVENT_HANDLER_ERR_INVALID_PARAM);
    queue.RemoveFileDescriptorListener(handler);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: AddFileDescriptorListener005
 * @tc.desc: add file descriptor listener when there are too many open files
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, AddFileDescriptorListener005, TestSize.Level1)
{
    /**
     * @tc.setup: init queue, prepare queue.
     */
    int32_t fds[] = {-1, -1};
    auto result = pipe(fds);
    EXPECT_GE(result, 0);
    EventQueue queue;
    queue.Prepare();
    int32_t readFileDescriptor = fds[0];

    /**
     * @tc.steps: step1. get max num of files the system could support, and open max files
     */
    struct rlimit rLimit {};
    result = getrlimit(RLIMIT_NOFILE, &rLimit);
    EXPECT_EQ(result, 0);
    for (uint64_t pos = 1; pos < rLimit.rlim_cur; pos++) {
        dup(readFileDescriptor);
    }

    /**
     * @tc.steps: step2. add file descriptor listener to queue, then remove
     *            file descriptor listener with handler, close pipe.
     * @tc.expected: step2. add file descriptor listener failed.
     */
    uint32_t event = (FILE_DESCRIPTOR_INPUT_EVENT | FILE_DESCRIPTOR_OUTPUT_EVENT);
    auto fileDescriptorListener = std::make_shared<MyFileDescriptorListener>();
    result = queue.AddFileDescriptorListener(readFileDescriptor, event, fileDescriptorListener);
    EXPECT_EQ(result, EVENT_HANDLER_ERR_FD_NOT_SUPPORT);
    close(fds[0]);
    close(fds[1]);
}

/*
 * @tc.name: HasEventWithID001
 * @tc.desc: check whether an event with the given ID can be found among the events that have been
 *           sent but not processed.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, HasEventWithID001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */

    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID);

    /**
     * @tc.steps: step1. send a event with delay time, then check has this event with this id,
     *                   then check executed after delay time has no this event with this id.
     * @tc.expected: step1. Has this event with event id.
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    bool HasInnerEvent = handler->HasInnerEvent(HAS_EVENT_ID);
    EXPECT_TRUE(HasInnerEvent);
    int64_t delayWaitTime = 100000;
    usleep(delayWaitTime);
    HasInnerEvent = handler->HasInnerEvent(HAS_EVENT_ID);
    EXPECT_FALSE(HasInnerEvent);
}

/*
 * @tc.name: HasEventWithID002
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, HasEventWithID002, TestSize.Level1)
{
    /**
     * @tc.setup: init runner.
     */
    auto handler = std::make_shared<EventHandler>(nullptr);
    auto event = InnerEvent::Get(HAS_EVENT_ID);

    /**
     * @tc.steps: step1. HasInnerEvent process
     *
     * @tc.expected: step1. HasInnerEvent process fail.
     */
    bool HasInnerEvent = handler->HasInnerEvent(HAS_EVENT_ID);
    EXPECT_FALSE(HasInnerEvent);
}

/*
 * @tc.name: HasEventWithID003
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, HasEventWithID003, TestSize.Level1)
{
    /**
     * @tc.setup: init runner.
     */
    auto runner = EventRunner::Create(true);

    auto event = InnerEvent::Get(HAS_EVENT_ID);

    /**
     * @tc.steps: step1. HasInnerEvent process
     *
     * @tc.expected: step1. HasInnerEvent process fail.
     */
    bool HasInnerEvent = runner->GetEventQueue()->HasInnerEvent(nullptr, HAS_EVENT_ID);
    EXPECT_FALSE(HasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam001
 * @tc.desc: check whether an event with the given param can be found among the events that have been
 *           sent but not processed.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, HasEventWithParam001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner.
     */

    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID, HAS_EVENT_PARAM);

    /**
     * @tc.steps: step1. send a event with delay time, then check has this event with this param,
     *                   then check executed after delay time has no this event with this param.
     * @tc.expected: step1. Has this event with event param.
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    bool HasInnerEvent = handler->HasInnerEvent(HAS_EVENT_PARAM);
    EXPECT_TRUE(HasInnerEvent);
    int64_t delayWaitTime = 100000;
    usleep(delayWaitTime);
    HasInnerEvent = handler->HasInnerEvent(HAS_EVENT_PARAM);
    EXPECT_FALSE(HasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam002
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, HasEventWithParam002, TestSize.Level1)
{
    /**
     * @tc.setup: init runner.
     */
    auto handler = std::make_shared<EventHandler>(nullptr);
    auto event = InnerEvent::Get(HAS_EVENT_PARAM);

    /**
     * @tc.steps: step1. HasInnerEvent process
     *
     * @tc.expected: step1. HasInnerEvent process fail.
     */
    bool HasInnerEvent = handler->HasInnerEvent(HAS_EVENT_PARAM);
    EXPECT_FALSE(HasInnerEvent);
}

/*
 * @tc.name: HasEventWithParam003
 * @tc.desc: check when runner is null ptr Has Inner Event process fail
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, HasEventWithParam003, TestSize.Level1)
{
    /**
     * @tc.setup: init runner.
     */
    auto runner = EventRunner::Create(true);

    auto event = InnerEvent::Get(HAS_EVENT_PARAM);

    /**
     * @tc.steps: step1. HasInnerEvent process
     *
     * @tc.expected: step1. HasInnerEvent process fail.
     */
    bool HasInnerEvent = runner->GetEventQueue()->HasInnerEvent(nullptr, HAS_EVENT_PARAM);
    EXPECT_FALSE(HasInnerEvent);
}

/*
 * @tc.name: GetEventName001
 * @tc.desc: check when send event has no task return event id
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, GetEventName001, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID);

    /**
     * @tc.steps: step1. GetEventName
     * @tc.expected: step1. GetEventName return event id
     */
    std::string eventName = handler->GetEventName(event);
    EXPECT_EQ(eventName, std::to_string(HAS_EVENT_ID));
}

/*
 * @tc.name: GetEventName002
 * @tc.desc: check when send event has task event name is "name" return event name "name"
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, GetEventName002, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {; };
    auto event = InnerEvent::Get(task, "name");

    /**
     * @tc.steps: step1. GetEventName
     * @tc.expected: step1. GetEventName return name
     */
    std::string eventName = handler->GetEventName(event);
    EXPECT_EQ(eventName, "name");
}

/*
 * @tc.name: GetEventName003
 * @tc.desc: check when send event has task task name is "" return event name ""
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, GetEventName003, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {; };
    auto event = InnerEvent::Get(task, "");

    /**
     * @tc.steps: step1. GetEventName
     * @tc.expected: step1. GetEventName return name
     */
    std::string eventName = handler->GetEventName(event);
    EXPECT_EQ(eventName, "");
}

/*
 * @tc.name: Dump001
 * @tc.desc: Check Dump
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, Dump001, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {; };
    auto event = InnerEvent::Get(task, "");
    DumpTest dumptest;
    /**
     * @tc.steps: step1. Dump
     * @tc.expected: step1. Dump Success
     */
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(isDump);
}

/*
 * @tc.name: Dump002
 * @tc.desc: Check Dump after post task
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, Dump002, TestSize.Level1)
{
    isDump = false;
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {; };
    DumpTest dumptest;
    /**
     * @tc.steps: step1. PosTask then PostTask
     * @tc.expected: step1. PostTask success
     */
    handler->PostTask(task, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(isDump);
}

/*
 * @tc.name: Dump003
 * @tc.desc: Check Dump after send event with event id
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, Dump003, TestSize.Level1)
{
    isDump = false;
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID);
    DumpTest dumptest;
    /**
     * @tc.steps: step1. SendEvent then Dump
     * @tc.expected: step1. Dump Success
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(isDump);
}

/*
 * @tc.name: Dump004
 * @tc.desc: Check Dump after send event with event id and param
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, Dump004, TestSize.Level1)
{
    isDump = false;
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID, HAS_EVENT_PARAM);
    DumpTest dumptest;
    /**
     * @tc.steps: step1. SendEvent then Dump
     * @tc.expected: step1. Dump Success
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(isDump);
}

/*
 * @tc.name: Dump005
 * @tc.desc: check when send event and post task dump success
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, Dump005, TestSize.Level1)
{
    isDump = false;
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID, HAS_EVENT_PARAM);
    auto task = []() {; };
    DumpTest dumptest;

    /**
     * @tc.steps: step1. send event and post task then dump
     * @tc.expected: step1. dump success
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    handler->PostTask(task, HAS_DELAY_TIME * 2, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dumptest);
    EXPECT_TRUE(isDump);
}

/*
 * @tc.name: IsIdle
 * @tc.desc: check when idle IsIdle return true
 * @tc.type: FUNC

 */
HWTEST_F(LibEventHandlerEventQueueTest, IsIdle001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    /**
     * @tc.steps: step1. IsIdle
     * @tc.expected: step1. when idle IsIdle return true
     */
    bool ret = handler->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsQueueEmpty001
 * @tc.desc: check when queue is empty IsQueueEmpty return true
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, IsQueueEmpty001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    bool ret = runner->GetEventQueue()->IsQueueEmpty();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsQueueEmpty002
 * @tc.desc: check when queue is not empty has low event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, IsQueueEmpty002, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID, HAS_EVENT_PARAM);

    /**
     * @tc.steps: step1. send event and IsQueueEmpty
     * @tc.expected: step1. when queue is not empty has low event IsQueueEmpty return false
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::LOW);
    bool ret = runner->GetEventQueue()->IsQueueEmpty();
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: IsQueueEmpty003
 * @tc.desc: check when queue is not empty has idle event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventQueueTest, IsQueueEmpty003, TestSize.Level1)
{
    /**
     * @tc.setup: init runner and handler
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID, HAS_EVENT_PARAM);

    /**
     * @tc.steps: step1. send event and IsQueueEmpty
     * @tc.expected: step1. when queue is not empty has idle event IsQueueEmpty return false
     */
    handler->SendEvent(event, HAS_DELAY_TIME, EventQueue::Priority::IDLE);
    bool ret = runner->GetEventQueue()->IsQueueEmpty();
    EXPECT_FALSE(ret);
}