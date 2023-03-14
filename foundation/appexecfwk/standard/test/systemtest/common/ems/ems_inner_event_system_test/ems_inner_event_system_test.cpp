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

#include "event_handler.h"
#include "event_queue.h"
#include "event_runner.h"
#include "inner_event.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const uint32_t EVENT_ID = 0;
const std::string THREAD_NAME_TEST1 = "threadNameTest1";
const std::string THREAD_NAME_TEST2 = " ";
const std::string THREAD_NAME_TEST3 = "@#￥#3243adsafdf_中文";
const std::string THREAD_NAME_TEST4 = std::to_string(0);
const std::string THREAD_NAME_TEST5 = std::to_string(0) + "@#￥#3243adsafdf_中文";
}  // namespace

/**
 * test for task information.
 */
static void TestTaskInfo1()
{
    string taskName(THREAD_NAME_TEST1);
    bool callbackCalled = false;
    auto f = [&callbackCalled]() { callbackCalled = true; };
    auto event = InnerEvent::Get(f, taskName);
    auto getName = event->GetTaskName();
    EXPECT_EQ(taskName, getName);
    // execute callback function, check whether the callback function is the one we set
    (event->GetTaskCallback())();
    // drop event, execute destructor function
    EXPECT_TRUE(callbackCalled);
}

static void TestTaskInfo2()
{
    string taskName(THREAD_NAME_TEST2);
    bool callbackCalled = false;
    auto f = [&callbackCalled]() { callbackCalled = true; };
    auto event = InnerEvent::Get(f, taskName);
    auto getName = event->GetTaskName();
    EXPECT_EQ(taskName, getName);
    // execute callback function, check whether the callback function is the one we set
    (event->GetTaskCallback())();
    // drop event, execute destructor function
    EXPECT_TRUE(callbackCalled);
}

static void TestTaskInfo3()
{
    string taskName(THREAD_NAME_TEST3);
    bool callbackCalled = false;
    auto f = [&callbackCalled]() { callbackCalled = true; };
    auto event = InnerEvent::Get(f, taskName);
    auto getName = event->GetTaskName();
    EXPECT_EQ(taskName, getName);
    // execute callback function, check whether the callback function is the one we set
    (event->GetTaskCallback())();
    // drop event, execute destructor function
    EXPECT_TRUE(callbackCalled);
}

static void TestTaskInfo4()
{
    string taskName(THREAD_NAME_TEST4);
    bool callbackCalled = false;
    auto f = [&callbackCalled]() { callbackCalled = true; };
    auto event = InnerEvent::Get(f, taskName);
    auto getName = event->GetTaskName();
    EXPECT_EQ(taskName, getName);
    // execute callback function, check whether the callback function is the one we set
    (event->GetTaskCallback())();
    // drop event, execute destructor function
    EXPECT_TRUE(callbackCalled);
}

static void TestTaskInfo5()
{
    string taskName(THREAD_NAME_TEST5);
    bool callbackCalled = false;
    auto f = [&callbackCalled]() { callbackCalled = true; };
    auto event = InnerEvent::Get(f, taskName);
    auto getName = event->GetTaskName();
    EXPECT_EQ(taskName, getName);
    // execute callback function, check whether the callback function is the one we set
    (event->GetTaskCallback())();
    // drop event, execute destructor function
    EXPECT_TRUE(callbackCalled);
}

class EmsInnerEventSystemTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EmsInnerEventSystemTest::SetUpTestCase(void)
{}

void EmsInnerEventSystemTest::TearDownTestCase(void)
{}

void EmsInnerEventSystemTest::SetUp(void)
{}

void EmsInnerEventSystemTest::TearDown(void)
{}

/*
 * @tc.name: GetHandleTime001
 * @tc.desc: set event handleTime and get event handleTime then compare,
 *           check whether the callback function is the one we set
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetHandleTime001, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID);
    InnerEvent::TimePoint now = InnerEvent::Clock::now();
    event->SetHandleTime(now);
    auto handleTime = event->GetHandleTime();
    EXPECT_EQ(now, handleTime);
}

/*
 * @tc.name: GetHandleTime002
 * @tc.desc: set event handleTime and get event handleTime then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetHandleTime002, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID);
    InnerEvent::TimePoint now = InnerEvent::TimePoint::max();
    event->SetHandleTime(now);
    auto handleTime = event->GetHandleTime();
    EXPECT_EQ(now, handleTime);
}

/*
 * @tc.name: GetHandleTime003
 * @tc.desc: set event handleTime and get event handleTime then compare,
 *           check whether the callback function is the one we set
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetHandleTime003, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID);
    InnerEvent::TimePoint now = InnerEvent::Clock::time_point();
    event->SetHandleTime(now);
    auto handleTime = event->GetHandleTime();
    EXPECT_EQ(now, handleTime);
}

/*
 * @tc.name: GetHandleTime001
 * @tc.desc: set event handleTime and get event handleTime then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetHandleTime004, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID);
    InnerEvent::TimePoint now = InnerEvent::TimePoint::min();
    event->SetHandleTime(now);
    auto handleTime = event->GetHandleTime();
    EXPECT_EQ(now, handleTime);
}

/*
 * @tc.name: GetHandleTime005
 * @tc.desc: set event handleTime and get event handleTime then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetHandleTime005, TestSize.Level1)
{
    auto event = InnerEvent::Get(EVENT_ID);
    InnerEvent::TimePoint now;
    event->SetHandleTime(now);
    auto handleTime = event->GetHandleTime();
    EXPECT_EQ(now, handleTime);
}

/*
 * @tc.name: GetTaskCallback
 * @tc.desc: set event callback taskName1 get event callback taskName1 and then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetTaskCallback001, TestSize.Level1)
{
    TestTaskInfo1();
}

/*
 * @tc.name: GetEventInfo002
 * @tc.desc: set event callback taskName2 and get event callback taskName2 then compare, check whether the callback
 *           function is the one we set
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetTaskCallback002, TestSize.Level1)
{
    TestTaskInfo2();
}

/*
 * @tc.name: GetEventInfo003
 * @tc.desc: set event callback taskName3 and get event callback taskName3 then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetTaskCallback003, TestSize.Level1)
{
    TestTaskInfo3();
}

/*
 * @tc.name: GetEventInfo004
 * @tc.desc: set event callback taskName4 and get event callback taskName4 then compare, check whether the callback
 *           function is the one we set
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetTaskCallback004, TestSize.Level1)
{
    TestTaskInfo4();
}

/*
 * @tc.name: GetEventInfo005
 * @tc.desc: set event callback taskName5 and get event callback taskName5 then compare, check whether the callback
 *           function is the one we set
 * @tc.type: FUNC
 */
HWTEST_F(EmsInnerEventSystemTest, GetTaskCallback005, TestSize.Level1)
{
    TestTaskInfo5();
}