/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <cerrno>
#include <thread>

#include <sys/prctl.h>

#include <gtest/gtest.h>
#include "event_handler.h"
#include "event_runner.h"
#include "hichecker.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;
using namespace OHOS::HiviewDFX;

bool isSetLogger = false;

class LibEventHandlerCheckTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class LoggerTest : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        isSetLogger = true;
        GTEST_LOG_(INFO) << line;
    };
    virtual ~LoggerTest()
    {}
};

void LibEventHandlerCheckTest::SetUpTestCase(void)
{}

void LibEventHandlerCheckTest::TearDownTestCase(void)
{}

void LibEventHandlerCheckTest::SetUp(void)
{}

void LibEventHandlerCheckTest::TearDown(void)
{}

/*
 * @tc.name: EventTimeout001
 * @tc.desc: Do not invoke AddRule distribution execution times out.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerCheckTest, EventTimeout001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner and deliveryTimeout and distributeTimeout.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto fs = []() {
        usleep(2000);
    };
    auto f = []() {
        usleep(10000);
    };
    bool deliveryTimeout_ = false;
    auto deliveryTimeoutThread = [&deliveryTimeout_]() {
        deliveryTimeout_ = true;
    };
    bool distributeTimeout_ = false;
    auto distributeTimeoutThread = [&distributeTimeout_]() {
        distributeTimeout_ = true;
    };
    int64_t deliveryTimeout = 1;
    int64_t distributeTimeout = 5;

    /**
     * @tc.steps: step1. post task and run the runner.
     * @tc.expected: step1. timeout success, but no hiCheck log.
     */
   
    runner->SetDeliveryTimeout(deliveryTimeout);
    runner->SetDistributeTimeout(distributeTimeout);
    handler->PostTask(fs);
    handler->PostTask(f);

    handler->SetDeliveryTimeoutCallback(deliveryTimeoutThread);
    handler->SetDistributeTimeoutCallback(distributeTimeoutThread);

    usleep(100 * 1000);
    EXPECT_EQ(false, deliveryTimeout_);
    EXPECT_EQ(false, distributeTimeout_);
}

/*
 * @tc.name: EventTimeout002
 * @tc.desc: Invocation AddRule distribution time out.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerCheckTest, EventTimeout002, TestSize.Level1)
{
    /**
     * @tc.setup: begin check.
     */
    HiChecker::AddRule(Rule::RULE_CHECK_SLOW_EVENT);

    /**
     * @tc.setup: init handler and runner and deliveryTimeout and distributeTimeout.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto fs = []() {
        usleep(2000);
    };
    auto f = []() {
        usleep(10000);
    };
    bool deliveryTimeout_ = false;
    auto deliveryTimeoutThread = [&deliveryTimeout_]() {
        deliveryTimeout_ = true;
    };
    bool distributeTimeout_ = false;
    auto distributeTimeoutThread = [&distributeTimeout_]() {
        distributeTimeout_ = true;
    };
    int64_t deliveryTimeout = 1;
    int64_t distributeTimeout = 100;

    /**
     * @tc.steps: step1. post task and run the runner.
     * @tc.expected: step1. timeout success, but no hiCheck log.
     */
   
    runner->SetDeliveryTimeout(deliveryTimeout);
    runner->SetDistributeTimeout(distributeTimeout);
    handler->PostTask(fs);
    handler->PostTask(f);

    handler->SetDeliveryTimeoutCallback(deliveryTimeoutThread);
    handler->SetDistributeTimeoutCallback(distributeTimeoutThread);

    usleep(100 * 1000);
    EXPECT_EQ(true, deliveryTimeout_);
    EXPECT_EQ(false, distributeTimeout_);
}
/*
 * @tc.name: EventTimeout003
 * @tc.desc: Invocation AddRule execution time out.
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerCheckTest, EventTimeout003, TestSize.Level1)
{
    /**
     * @tc.setup: begin check.
     */
    HiChecker::AddRule(Rule::RULE_CHECK_SLOW_EVENT);

    /**
     * @tc.setup: init handler and runner and deliveryTimeout and distributeTimeout.
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto fs = []() {
        usleep(2000);
    };
    auto f = []() {
        usleep(10000);
    };
    bool deliveryTimeout_ = false;
    auto deliveryTimeoutThread = [&deliveryTimeout_]() {
        deliveryTimeout_ = true;
    };
    bool distributeTimeout_ = false;
    auto distributeTimeoutThread = [&distributeTimeout_]() {
        distributeTimeout_ = true;
    };
    int64_t deliveryTimeout = 4;
    int64_t distributeTimeout = 6;

    /**
     * @tc.steps: step1. post task and run the runner.
     * @tc.expected: step1. timeout success, but no hiCheck log.
     */
   
    runner->SetDeliveryTimeout(deliveryTimeout);
    runner->SetDistributeTimeout(distributeTimeout);
    handler->PostTask(fs);
    handler->PostTask(f, "eventHandlerCheckTest");

    handler->SetDeliveryTimeoutCallback(deliveryTimeoutThread);
    handler->SetDistributeTimeoutCallback(distributeTimeoutThread);

    usleep(100 * 1000);
    EXPECT_EQ(false, deliveryTimeout_);
    EXPECT_EQ(true, distributeTimeout_);
}