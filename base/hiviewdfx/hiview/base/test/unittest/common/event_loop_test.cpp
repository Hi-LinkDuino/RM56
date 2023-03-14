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
#include "event_loop_test.h"

#include <cinttypes>
#include <ctime>
#include <fstream>
#include <iostream>

#include <gtest/gtest.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "audit.h"
#include "event_loop.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace HiviewDFX {
void EventLoopTest::SetUp()
{
    /**
     * @tc.setup: create an event loop and multiple event handlers
     */
    if (currentLooper_ != nullptr) {
        currentLooper_->StopLoop();
        currentLooper_.reset();
    }

    mkdir("/data/log", S_IRWXU | S_IRWXG);
    const ::testing::TestInfo * const info = ::testing::UnitTest::GetInstance()->current_test_info();
    printf("We are in test %s of test case %s.\n", info->name(), info->test_case_name());
    currentLooper_ = std::make_shared<EventLoop>(info->test_case_name());
    currentLooper_->StartLoop();
}

void EventLoopTest::TearDown()
{
    /**
     * @tc.teardown: destroy the event loop we have created
     */
    if (currentLooper_ != nullptr) {
        currentLooper_->StopLoop();
        currentLooper_.reset();
    }
}

bool RealEventHandler::OnEvent(std::shared_ptr<Event>& event)
{
    printf("OnEvent id:%d sender:%s pid:%d time:%" PRId64 ". index:%d\n", event->eventId_, event->sender_.c_str(),
           gettid(), static_cast<int64_t>(time(nullptr)), event->what_);
    lastProcessId_ = event->eventId_;
    if ((event->what_ % 10) == 0) { // 10 : add delay in processing some event
        usleep(100); // 100 : 100us
    }
    processedEventCount_++;
    pid_ = getpid();
    receivedEventNo_.push_back(event->what_);
    return true;
}

void RealEventHandler::DoTask()
{
    printf("RealEventHandler::DoTask pid:%d time:%" PRId64 ".\n", gettid(), static_cast<int64_t>(time(nullptr)));
    processedEventCount_++;
}

bool OverheadCalculateEventHandler::OnEvent(std::shared_ptr<Event>& event)
{
    auto cost = clock() - event->createTime_;
    totalDeliverOverHead_ += cost;
    processedEventCount_++;
    return true;
}

bool DataFileEventReader::OnFileDescriptorEvent(int fd, int type)
{
    printf("DataFileEventReader::OnEvent id:%d  inotifyFd_:%d\n", fd, inotifyFd_);
    const int bufSize = 2048;
    char buffer[bufSize] = {0};
    char *offset = nullptr;
    struct inotify_event *event = nullptr;
    if (inotifyFd_ <= 0) {
        printf("Invalid inotify fd:%d", inotifyFd_);
        return false;
    }

    int len = read(inotifyFd_, buffer, bufSize);
    if (len <= 0) {
        printf("failed to read event");
        return false;
    }
    offset = buffer;
    event = reinterpret_cast<struct inotify_event *>(buffer);
    while ((reinterpret_cast<char *>(event) - buffer) < len) {
        for (const auto &it : fileMap_) {
            if (it.second != event->wd) {
                printf("fail to find check point: %d", event->wd);
                continue;
            }

            std::string filePath = it.first;
            printf("handle file event in %s \n", filePath.c_str());
        }

        int tmpLen = sizeof(struct inotify_event) + event->len;
        event = reinterpret_cast<struct inotify_event *>(offset + tmpLen);
        offset += tmpLen;
    }
    return false;
}

int32_t DataFileEventReader::GetPollFd()
{
    if (inotifyFd_ > 0) {
        return inotifyFd_;
    }

    inotifyFd_ = inotify_init();
    if (inotifyFd_ == -1) {
        printf("failed to init inotify: %s \n", strerror(errno));
        return -1;
    }

    // create file
    FILE *targetLogFile = fopen(EVENT_LOG_PATH.c_str(), "w+");
    if (targetLogFile != nullptr) {
        fclose(targetLogFile);
    }

    int wd = inotify_add_watch(inotifyFd_, EVENT_LOG_PATH.c_str(), IN_CLOSE_WRITE | IN_MOVED_TO);
    if (wd < 0) {
        printf("failed to add watch entry : %s \n", strerror(errno));
        close(inotifyFd_);
        inotifyFd_ = -1;
        return -1;
    }
    fileMap_[EVENT_LOG_PATH] = wd;
    return inotifyFd_;
}

int32_t DataFileEventReader::GetPollType()
{
    return EPOLLIN;
}

/**
 * @tc.name: EventLoopStartAndStopTest001
 * @tc.desc: start the event loop and stop the event loop
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, StartAndStopTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler
     */
    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto event1 = std::make_shared<Event>("test1");
    currentLooper_->AddEvent(eventhandler1, event1, nullptr);
    /**
     * @tc.expected: step1. fetch the thread id of the event loop
     */
    int curPid = getpid();
    EXPECT_NE(curPid, eventhandler1->pid_);
}

/**
 * @tc.name: EventLoopEventProcessTest001
 * @tc.desc: send multiple events to event loop
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventProcessTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create event handler and events
     */
    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto eventhandler2 = std::make_shared<RealEventHandler>();
    auto eventhandler3 = std::make_shared<RealEventHandler>();
    auto eventhandler4 = std::make_shared<RealEventHandler>();

    auto event1 = std::make_shared<Event>("test1");
    auto event2 = std::make_shared<Event>("test2");
    auto event3 = std::make_shared<Event>("test3");
    auto event4 = std::make_shared<Event>("test4");
    auto event5 = std::make_shared<Event>("test5");
    auto event6 = std::make_shared<Event>("test6");
    auto event7 = std::make_shared<Event>("test7");

    currentLooper_->AddEvent(eventhandler1, event1, nullptr);
    currentLooper_->AddEvent(eventhandler2, event2, nullptr);
    currentLooper_->AddEvent(eventhandler3, event3, nullptr);
    currentLooper_->AddEvent(eventhandler4, event4, nullptr);
    currentLooper_->AddEvent(eventhandler1, event5, nullptr);
    currentLooper_->AddEvent(eventhandler4, event6, nullptr);
    currentLooper_->AddEvent(eventhandler1, event7, nullptr);
    sleep(2);

    /**
     * @tc.expected: step1. the event has been processed
     */
    EXPECT_EQ(3, eventhandler1->processedEventCount_);
    EXPECT_EQ(1, eventhandler2->processedEventCount_);
    EXPECT_EQ(1, eventhandler3->processedEventCount_);
    EXPECT_EQ(2, eventhandler4->processedEventCount_);
}

/**
 * @tc.name: EventLoopEventProcessTest002
 * @tc.desc: Add Fd callback and check the result from fd
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventProcessTest002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a timer event and send to target
     */
    auto eventReader = std::make_shared<DataFileEventReader>();
    currentLooper_->AddFileDescriptorEventCallback("test1", eventReader);

    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto event1 = std::make_shared<Event>("test1");
    auto event2 = std::make_shared<Event>("test2");
    auto event3 = std::make_shared<Event>("test3");

    currentLooper_->AddEvent(eventhandler1, event1, nullptr);
    currentLooper_->AddEvent(eventhandler1, event2, nullptr);
    currentLooper_->AddEvent(eventhandler1, event3, nullptr);

    std::ofstream testFile;
    testFile.open(DataFileEventReader::EVENT_LOG_PATH);
    testFile << "Writing this to a file.\n";
    testFile.close();

    sleep(1);
    testFile.open(DataFileEventReader::EVENT_LOG_PATH);
    testFile << "Writing this to a file.\n";
    testFile.close();
    sleep(1);
    testFile.open(DataFileEventReader::EVENT_LOG_PATH);
    testFile << "Writing this to a file.\n";
    testFile.close();
    sleep(1);
    /**
     * @tc.expected: step1. the event has been processed
     */
    EXPECT_EQ(3, eventhandler1->processedEventCount_);
}

/**
 * @tc.name: EventLoopEventProcessTest003
 * @tc.desc: Exec period event
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventProcessTest003, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a timer event and send to target
     */
    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto event1 = std::make_shared<Event>("test1");
    currentLooper_->AddTimerEvent(eventhandler1, event1, nullptr, 2, true);
    sleep(10);
    /**
     * @tc.expected: step1. the event has been processed
     */
    EXPECT_EQ(4, eventhandler1->processedEventCount_);
}

/**
 * @tc.name: EventLoopEventProcessTest004
 * @tc.desc: Exec period task
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventProcessTest004, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a timer event and send to target
     */
    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto task = std::bind(&RealEventHandler::DoTask, eventhandler1.get());
    currentLooper_->AddTimerEvent(nullptr, nullptr, task, 2, true);
    printf("On Main Thread:%d \n", gettid());
    sleep(10);
    /**
     * @tc.expected: step1. the event has been processed
     */
    EXPECT_EQ(4, eventhandler1->processedEventCount_);
}

/**
 * @tc.name: EventLoopEventProcessTest005
 * @tc.desc: Exec period task and remove it
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventProcessTest005, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a timer event and send to target
     */
    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto task = std::bind(&RealEventHandler::DoTask, eventhandler1.get());
    auto seqId = currentLooper_->AddTimerEvent(nullptr, nullptr, task, 2, true);
    printf("On Main Thread:%d \n", gettid());
    sleep(5);
    currentLooper_->RemoveEvent(seqId);
    sleep(5);
    /**
     * @tc.expected: step1. the event has been processed
     */
    EXPECT_EQ(2, eventhandler1->processedEventCount_);
}

/**
 * @tc.name: EventLoopEventProcessTest006
 * @tc.desc: Exec timer event and remove it
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventProcessTest006, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a timer event and send to target
     */
    auto eventhandler1 = std::make_shared<RealEventHandler>();
    auto event1 = std::make_shared<Event>("test1");
    currentLooper_->AddTimerEvent(eventhandler1, event1, nullptr, 2, false);
    sleep(3);
    EXPECT_EQ(1, eventhandler1->processedEventCount_);
    auto event2 = std::make_shared<Event>("test2");
    auto seq = currentLooper_->AddTimerEvent(eventhandler1, event2, nullptr, 2, false);
    sleep(1);
    currentLooper_->RemoveEvent(seq);
    sleep(2);
    EXPECT_EQ(1, eventhandler1->processedEventCount_);
}

/**
 * @tc.name: EventLoopOverheadTest001
 * @tc.desc: send an event and check the deliver cost
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopOverheadTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a handler and event
     * @tc.steps: step2. calculate event deliver overhead
     */
    auto overheadCounter = std::make_shared<OverheadCalculateEventHandler>();
    const int totalTestCount = 100;
    for (int i = 0; i < totalTestCount; i++) {
        auto event = std::make_shared<Event>("test");
        event->createTime_ = clock();
        currentLooper_->AddEvent(overheadCounter, event, nullptr);
    }
    sleep(10);
    printf("TotalDeliverOverHeadCost:%" PRIu64 ".\n", overheadCounter->totalDeliverOverHead_);
    printf("TotalProcessEventCount:%" PRIu64 ".\n", overheadCounter->processedEventCount_);
    printf("EventTransferOverhead:%" PRIu64 ".\n",
        overheadCounter->totalDeliverOverHead_/overheadCounter->processedEventCount_);
}

/**
 * @tc.name: EventLoopEventOrderTest001
 * @tc.desc: send 1000 events and check the receiving order
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopEventOrderTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. create a handler and 1000 events in a loop
     * @tc.steps: step2. check the received order
     */
    auto eventhandler = std::make_shared<RealEventHandler>();
    const uint16_t totalTestCount = 1000;
    for (uint16_t i = 0; i < totalTestCount; i++) {
        auto event = std::make_shared<Event>("testevent");
        event->what_ = i;
        currentLooper_->AddEvent(eventhandler, event, nullptr);
    }
    sleep(10); // 10: sleep 10 seconds
    ASSERT_EQ(eventhandler->receivedEventNo_.size(), totalTestCount);
    for (uint16_t i = 0; i < totalTestCount; i++) {
        ASSERT_EQ(eventhandler->receivedEventNo_[i], i);
    }
}

/**
 * @tc.name: EventLoopWrongInputParamsTest001
 * @tc.desc: pass invalid params to add event interface
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventLoopTest, EventLoopWrongInputParamsTest001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. pass invalid params to add event interface
     * @tc.steps: step2. check the return future
     */
    auto result0 = currentLooper_->AddEventForResult(nullptr, nullptr);
    ASSERT_EQ(result0.get(), false);

    auto eventhandler = std::make_shared<RealEventHandler>();
    auto result1 = currentLooper_->AddEventForResult(eventhandler, nullptr);
    ASSERT_EQ(result1.get(), false);

    auto event = std::make_shared<Event>("testevent");
    auto result2 = currentLooper_->AddEventForResult(eventhandler, event);
    ASSERT_EQ(result2.get(), true);
}
}
}
