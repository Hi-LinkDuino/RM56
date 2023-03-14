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

class EventHandlerPostTaskModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerPostTaskModuleTest::SetUpTestCase(void)
{}

void EventHandlerPostTaskModuleTest::TearDownTestCase(void)
{}

void EventHandlerPostTaskModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the value of test flags to the default.
     */
    CommonUtils::TaskCalledSet(false);
}

void EventHandlerPostTaskModuleTest::TearDown(void)
{}

/**
 * @tc.name: Post001
 * @tc.desc: Post a task with callback, name, delayTime and priority
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a task with callback, name, delayTime and priority.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    int64_t delayTime = 1;
    string taskName = std::to_string(Random());
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostTask(f, taskName, delayTime, EventQueue::Priority::LOW);
    EXPECT_TRUE(postResult);
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime + 1);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Post002
 * @tc.desc: Post a task with callback, delayTime and priority
 * @tc.type: FUNC
 * @tc.require: SR000BTOPM AR000CQ2AE
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a task with callback, delayTime and priority.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    int64_t delayTime = 0;
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostTask(f, delayTime, EventQueue::Priority::HIGH);
    EXPECT_TRUE(postResult);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Post003
 * @tc.desc: Post an immediate task with callback and name
 * @tc.type: FUNC
 * @tc.require: AR000CQ2A8 SR000CQ2AH
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post an immediate task with callback and name.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    string taskName = std::to_string(Random());
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostImmediateTask(f, taskName);
    EXPECT_TRUE(postResult);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Post004
 * @tc.desc: Post a high priority with callback, name and delayTime
 * @tc.type: FUNC
 * @tc.require: AR000CQ2A9 SR000CQ2AH
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a high priority with callback, name and delayTime.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    int64_t delayTime = 0;
    string taskName = std::to_string(Random());
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostHighPriorityTask(f, taskName, delayTime);
    EXPECT_TRUE(postResult);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Post005
 * @tc.desc: Post a high priority with callback and delayTime
 * @tc.type: FUNC
 * @tc.require: AR000CQ2AA SR000CQ2AH
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a high priority with callback and delayTime.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    int64_t delayTime = 0;
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostHighPriorityTask(f, delayTime);
    EXPECT_TRUE(postResult);
    handler->SendEvent(STOP_EVENT_ID);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Post006
 * @tc.desc: Post a idle priority with callback, name and delayTime
 * @tc.type: FUNC
 * @tc.require: AR000CQ2AB SR000BTOPJ
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a idle priority with callback, name and delayTime.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    int64_t delayTime = 0;
    string taskName = std::to_string(Random());
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostIdleTask(f, taskName, delayTime);
    EXPECT_TRUE(postResult);
    auto fStop = [&myRunner]() { myRunner->Stop(); };
    handler->PostIdleTask(fStop, delayTime + 1);
    int64_t param = 0;
    int64_t offsetTime = 2;
    handler->SendEvent(RUN_EVENT_ID, param, delayTime + offsetTime);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}

/**
 * @tc.name: Post007
 * @tc.desc: Post a idle priority with callback and delayTime
 * @tc.type: FUNC
 * @tc.require: AR000CQ2AC SR000BTOPJ
 */
HWTEST_F(EventHandlerPostTaskModuleTest, Post007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Post a idle priority with callback and delayTime.
     * @tc.expected: step1. Post successfully and the task handled.
     */
    int64_t delayTime = 0;
    int64_t param = 0;
    auto f = []() { CommonUtils::TaskCalledSet(true); };
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    bool postResult = handler->PostIdleTask(f, delayTime);
    EXPECT_TRUE(postResult);
    auto fStop = [&myRunner]() { myRunner->Stop(); };
    handler->PostIdleTask(fStop, delayTime + 1);
    int64_t offsetTime = 2;
    handler->SendEvent(RUN_EVENT_ID, param, delayTime + offsetTime);
    myRunner->Run();
    bool runResult = CommonUtils::TaskCalledGet();
    EXPECT_TRUE(runResult);
}
