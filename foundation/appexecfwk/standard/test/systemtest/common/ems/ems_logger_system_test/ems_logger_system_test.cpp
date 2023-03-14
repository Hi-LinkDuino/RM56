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
bool g_isLog = false;
const std::string LOG_TEST1 = "LogSystemTest1";
const std::string LOG_TEST2 = " ";
const std::string LOG_TEST3 = "@#￥#3243adsafdf_中文";
const std::string LOG_TEST4 = std::to_string(0);
const std::string LOG_TEST5 = std::to_string(0) + "@#￥#3243adsafdf_中文";
}  // namespace

class EmsLoggerSystemTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EmsLoggerSystemTest::SetUpTestCase(void)
{}

void EmsLoggerSystemTest::TearDownTestCase(void)
{}

void EmsLoggerSystemTest::SetUp(void)
{}

void EmsLoggerSystemTest::TearDown(void)
{}

class LogTest1 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isLog = true;
        GTEST_LOG_(INFO) << line;
    }
};

class LogTest2 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isLog = true;
        GTEST_LOG_(INFO) << line;
    }
};

class LogTest3 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isLog = true;
        GTEST_LOG_(INFO) << line;
    }
};

class LogTest4 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isLog = true;
        GTEST_LOG_(INFO) << line;
    }
};

class LogTest5 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isLog = true;
        GTEST_LOG_(INFO) << line;
    }
};

/*
 * @tc.name: Log001
 * @tc.desc: Check LogTest1 process Log success
 * @tc.type: FUNC
 */
HWTEST_F(EmsLoggerSystemTest, Log001, TestSize.Level1)
{
    g_isLog = false;
    LogTest1 logTest;
    logTest.Log(LOG_TEST1);
    EXPECT_TRUE(g_isLog);
}

/*
 * @tc.name: Log002
 * @tc.desc: Check LogTest2 process Log success
 * @tc.type: FUNC
 */
HWTEST_F(EmsLoggerSystemTest, Log002, TestSize.Level1)
{
    g_isLog = false;
    LogTest2 logTest;
    logTest.Log(LOG_TEST2);
    EXPECT_TRUE(g_isLog);
}

/*
 * @tc.name: Log003
 * @tc.desc: Check LogTest3 process Log success
 * @tc.type: FUNC
 */
HWTEST_F(EmsLoggerSystemTest, Log003, TestSize.Level1)
{
    g_isLog = false;
    LogTest3 logTest;
    logTest.Log(LOG_TEST3);
    EXPECT_TRUE(g_isLog);
}

/*
 * @tc.name: Log004
 * @tc.desc: Check LogTest4 process Log success
 * @tc.type: FUNC
 */
HWTEST_F(EmsLoggerSystemTest, Log004, TestSize.Level1)
{
    g_isLog = false;
    LogTest4 logTest;
    logTest.Log(LOG_TEST4);
    EXPECT_TRUE(g_isLog);
}

/*
 * @tc.name: Log005
 * @tc.desc: Check LogTest5 process Log success
 * @tc.type: FUNC
 */
HWTEST_F(EmsLoggerSystemTest, Log005, TestSize.Level1)
{
    g_isLog = false;
    LogTest5 logTest;
    logTest.Log(LOG_TEST5);
    EXPECT_TRUE(g_isLog);
}