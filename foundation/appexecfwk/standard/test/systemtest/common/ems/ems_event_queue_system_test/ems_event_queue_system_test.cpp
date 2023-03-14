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
const uint32_t EVENT_PARAM = 10000000;
const uint32_t DELAY_TIME = 10;
const std::string THREAD_NAME_TEST1 = "threadNameTest1";
const std::string THREAD_NAME_TEST2 = " ";
const std::string THREAD_NAME_TEST3 = "@#￥#3243adsafdf_中文";
const std::string THREAD_NAME_TEST4 = std::to_string(0);
const std::string THREAD_NAME_TEST5 = std::to_string(0) + "@#￥#3243adsafdf_中文";
}  // namespace

class EmsEventQueueSystemTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EmsEventQueueSystemTest::SetUpTestCase(void)
{}

void EmsEventQueueSystemTest::TearDownTestCase(void)
{}

void EmsEventQueueSystemTest::SetUp(void)
{}

void EmsEventQueueSystemTest::TearDown(void)
{}

/*
 * @tc.name: IsQueueEmpty001
 * @tc.desc: check when queue is empty IsQueueEmpty return true
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventQueueSystemTest, IsQueueEmpty001, TestSize.Level1)
{
    std::shared_ptr<EventQueue> queue = std::make_shared<EventQueue>();
    bool ret = queue->IsQueueEmpty();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsQueueEmpty002
 * @tc.desc: check when queue is not empty has low event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventQueueSystemTest, IsQueueEmpty002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, EVENT_PARAM);

    /**
     * @tc.steps: step1. send event and IsQueueEmpty
     * @tc.expected: step1. when queue is not empty has low event IsQueueEmpty return false
     */
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    std::shared_ptr<EventQueue> queue = std::make_shared<EventQueue>();
    bool ret = queue->IsQueueEmpty();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsQueueEmpty003
 * @tc.desc: check when queue is not empty has idle event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventQueueSystemTest, IsQueueEmpty003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(EVENT_ID, EVENT_PARAM);
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::IMMEDIATE);
    std::shared_ptr<EventQueue> queue = std::make_shared<EventQueue>();
    bool ret = queue->IsQueueEmpty();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: IsIdle001
 * @tc.desc: check when queue is not empty has low event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventQueueSystemTest, IsIdle001, TestSize.Level1)
{
    std::shared_ptr<EventQueue> queue = std::make_shared<EventQueue>();
    bool ret = queue->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: HasInnerEvent
 * @tc.desc: check when queue is not empty has low event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventQueueSystemTest, HasInnerEvent001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    std::shared_ptr<EventQueue> queue = std::make_shared<EventQueue>();
    bool ret = queue->HasInnerEvent(handler, EVENT_ID);
    EXPECT_FALSE(ret);
}