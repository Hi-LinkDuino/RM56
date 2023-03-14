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

#include <sys/prctl.h>
#include <thread>

#include "event_handler.h"
#include "event_queue.h"
#include "event_runner.h"
#include "inner_event.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
bool g_isSetLogger = false;
bool g_isDump = false;
const uint32_t EVENT_ID_1 = 1;
const uint32_t EVENT_ID_2 = 2;
const uint32_t EVENT_ID_3 = 3;
const uint32_t EVENT_ID_4 = 4;
const uint32_t EVENT_ID_5 = 5;
const int64_t FLAG_ONE = 1;
const int64_t FLAG_TWO = 2;
const int64_t FLAG_THREE = 3;
const int64_t FLAG_FOUR = 4;
const int64_t FLAG_FIVE = 5;
const uint32_t EVENT_PARAM = 100;
const uint32_t DELAY_TIME = 100;
const std::string THREAD_NAME_TEST1 = "threadNameTest1";
const std::string THREAD_NAME_TEST2 = " ";
const std::string THREAD_NAME_TEST3 = "3243adsafdf";
const std::string THREAD_NAME_TEST4 = std::to_string(0);
const std::string THREAD_NAME_TEST5 = std::to_string(0) + "3243adsafdf";
}  // namespace

class EmsEventRunnerSystemTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EmsEventRunnerSystemTest::SetUpTestCase(void)
{}

void EmsEventRunnerSystemTest::TearDownTestCase(void)
{}

void EmsEventRunnerSystemTest::SetUp(void)
{}

void EmsEventRunnerSystemTest::TearDown(void)
{}

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

class LoggerTest001 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isSetLogger = true;
        GTEST_LOG_(INFO) << line;
    };
    virtual ~LoggerTest001()
    {}
};

class LoggerTest002 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isSetLogger = true;
        GTEST_LOG_(INFO) << line;
    };
    virtual ~LoggerTest002()
    {}
};

class LoggerTest003 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isSetLogger = true;
        GTEST_LOG_(INFO) << line;
    };
    virtual ~LoggerTest003()
    {}
};

class LoggerTest004 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isSetLogger = true;
        GTEST_LOG_(INFO) << line;
    };
    virtual ~LoggerTest004()
    {}
};

class LoggerTest005 : public Logger {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Log(const std::string &line)
    {
        g_isSetLogger = true;
        GTEST_LOG_(INFO) << line;
    };
    virtual ~LoggerTest005()
    {}
};

/**
 * Wait until task is executed.
 *
 * @param f task we post.
 * @param handler handler we use to post task.
 * @param taskCalled flag to record whether task is executed.
 */
template <typename F>
static void WaitUntilTaskCalled(
    F const &f, const std::shared_ptr<EventHandler> &handler, const std::atomic<bool> &taskCalled)
{
    const uint32_t maxRetryCount = 1000;
    const uint32_t sleepTime = 1000;
    uint32_t count = 0;
    if (handler->PostTask(f)) {
        while (!taskCalled.load()) {
            ++count;
            // if delay more than 1 second, break
            if (count >= maxRetryCount) {
                break;
            }

            usleep(sleepTime);
        }
    }
}

/**
 * Create runner with name.
 *
 * @param handler handler we use to post task.
 * @param threadName name of thread we set.
 */
static void CreateRunnerWithName(const std::shared_ptr<EventHandler> &handler, const std::string &threadName)
{
    std::atomic<bool> sameThreadName(false);
    std::atomic<bool> taskCalled(false);
    auto f = [&sameThreadName, &taskCalled, &threadName]() {
        const size_t maxThreadNameSize = 16;
        char thisThreadName[maxThreadNameSize + 1];

        // Get current thread name and compare with the specified one.
        int32_t ret = prctl(PR_GET_NAME, thisThreadName);
        if (ret == 0) {
            thisThreadName[maxThreadNameSize] = '\0';
            sameThreadName.store(threadName == thisThreadName);
        }

        taskCalled.store(true);
    };

    WaitUntilTaskCalled(f, handler, taskCalled);

    auto called = taskCalled.load();
    EXPECT_TRUE(called);
    auto sameThread = sameThreadName.load();
    EXPECT_TRUE(sameThread);
}

/*
 * @tc.name: SetLogger001
 * @tc.desc: check SetLogger001 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger002
 * @tc.desc: check SetLogger002 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger003
 * @tc.desc: check SetLogger001 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger004
 * @tc.desc: check SetLogger001 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger005
 * @tc.desc: check SetLogger001 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger005, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger006
 * @tc.desc: check SetLogger006 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger006, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostTask(task, EventHandler::Priority::HIGH);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger007
 * @tc.desc: check SetLogger007 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger007, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostTask(task, EventHandler::Priority::LOW);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger008
 * @tc.desc: check SetLogger008 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger008, TestSize.Level1)
{
    auto runner = EventRunner::Create();
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostTask(task, EventHandler::Priority::IMMEDIATE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger009
 * @tc.desc: check SetLogger009 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger009, TestSize.Level1)
{
    auto runner = EventRunner::Create();
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostTask(task, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger010
 * @tc.desc: check SetLogger010 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger010, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostTask(task, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger011
 * @tc.desc: check SetLogger011 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger011, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_ONE, std::string());
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger012
 * @tc.desc: check SetLogger012 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger012, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_TWO, std::string());
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger013
 * @tc.desc: check SetLogger013 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger013, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_THREE, std::string());
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger014
 * @tc.desc: check SetLogger014 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger014, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_FOUR, std::string());
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger015
 * @tc.desc: check SetLogger015 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger015, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_FIVE, std::string());
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger016
 * @tc.desc: check SetLogger016 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger016, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_ONE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger017
 * @tc.desc: check SetLogger017 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger017, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_TWO);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger018
 * @tc.desc: check SetLogger018 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger018, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_THREE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger019
 * @tc.desc: check SetLogger019 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger019, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_FOUR);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger020
 * @tc.desc: check SetLogger020 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger020, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_FIVE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger021
 * @tc.desc: check SetLogger021 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger021, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_ONE, EventHandler::Priority::IMMEDIATE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger022
 * @tc.desc: check SetLogger022 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger022, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_TWO, EventHandler::Priority::LOW);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger023
 * @tc.desc: check SetLogger023 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger023, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_THREE, EventHandler::Priority::HIGH);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger024
 * @tc.desc: check SetLogger024 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger024, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_FOUR, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger025
 * @tc.desc: check SetLogger025 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger025, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostTask(task, FLAG_FIVE, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger026
 * @tc.desc: check SetLogger026 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger026, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task, EventHandler::Priority::IMMEDIATE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger027
 * @tc.desc: check SetLogger027 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger027, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task, EventHandler::Priority::LOW);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger028
 * @tc.desc: check SetLogger028 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger028, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task, EventHandler::Priority::HIGH);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger029
 * @tc.desc: check SetLogger029 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger029, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_FALSE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger030
 * @tc.desc: check SetLogger030 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger030, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_FALSE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger031
 * @tc.desc: check SetLogger031 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger031, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger032
 * @tc.desc: check SetLogger032 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger032, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger033
 * @tc.desc: check SetLogger033 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger033, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger034
 * @tc.desc: check SetLogger034 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger034, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger035
 * @tc.desc: check SetLogger035 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger035, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostSyncTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger036
 * @tc.desc: check SetLogger036 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger036, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };

    g_isSetLogger = false;
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_ONE, EventHandler::Priority::IMMEDIATE);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger037
 * @tc.desc: check SetLogger037 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger037, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_TWO, EventHandler::Priority::LOW);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger038
 * @tc.desc: check SetLogger038 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger038, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_THREE, EventHandler::Priority::HIGH);
    usleep(100 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger039
 * @tc.desc: check SetLogger039 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger039, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_FOUR, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: SetLogger040
 * @tc.desc: check SetLogger040 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, SetLogger040, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    g_isSetLogger = false;
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    runner->SetLogger(logtest);
    handler->PostTimingTask(task, FLAG_FIVE, EventHandler::Priority::IDLE);
    usleep(500 * 1000);
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: Stop001
 * @tc.desc: stop eventrunner in synchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop001, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    ErrCode result = EVENT_HANDLER_ERR_RUNNER_NO_PERMIT;
    auto f = [&result, runner]() { result = runner->Stop(); };

    if (handler->PostTask(f)) {
        runner->Run();
    }

    EXPECT_EQ(ERR_OK, result);
}

/*
 * @tc.name: Stop002
 * @tc.desc: stop eventrunner in asynchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop002, TestSize.Level1)
{
    auto runner = EventRunner::Create();

    auto runResult = runner->Run();
    auto stopResult = runner->Stop();
    EXPECT_EQ(EVENT_HANDLER_ERR_RUNNER_NO_PERMIT, runResult);
    EXPECT_EQ(EVENT_HANDLER_ERR_RUNNER_NO_PERMIT, stopResult);
}

/*
 * @tc.name: Stop003
 * @tc.desc: execute Stop() function from one EventRunner multi times
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop003, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = [&runner]() { runner->Stop(); };

    handler->PostTask(task);
    runner->Run();
    usleep(10000);
    auto stopResult = runner->Stop();
    EXPECT_EQ(ERR_OK, stopResult);
}

/*
 * @tc.name: Stop004
 * @tc.desc: stop eventrunner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop004, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    if (handler->SendEvent(EVENT_ID_1, EVENT_PARAM, DELAY_TIME)) {
        ErrCode result = runner->Stop();
        EXPECT_EQ(ERR_OK, result);
    }
}

/*
 * @tc.name: Stop004
 * @tc.desc: stop eventrunner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop005, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    if (handler->SendEvent(EVENT_ID_1)) {
        ErrCode result = runner->Stop();
        EXPECT_EQ(ERR_OK, result);
    }
}

/*
 * @tc.name: Stop006
 * @tc.desc: stop eventrunner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop006, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    if (handler->SendEvent(EVENT_ID_2)) {
        ErrCode result = runner->Stop();
        EXPECT_EQ(ERR_OK, result);
    }
}

/*
 * @tc.name: Stop007
 * @tc.desc: stop eventrunner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop007, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    if (handler->SendEvent(EVENT_ID_3)) {
        ErrCode result = runner->Stop();
        EXPECT_EQ(ERR_OK, result);
    }
}

/*
 * @tc.name: Stop008
 * @tc.desc: stop eventrunner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop008, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    if (handler->SendEvent(EVENT_ID_4)) {
        ErrCode result = runner->Stop();
        EXPECT_EQ(ERR_OK, result);
    }
}

/*
 * @tc.name: Stop009
 * @tc.desc: stop eventrunner success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Stop009, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    if (handler->SendEvent(EVENT_ID_5)) {
        ErrCode result = runner->Stop();
        EXPECT_EQ(ERR_OK, result);
    }
}

/*
 * @tc.name: CreateAndRun001
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun001, TestSize.Level1)
{
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    WaitUntilTaskCalled(f, handler, taskCalled);
    auto called = taskCalled.load();
    EXPECT_TRUE(called);
}

/*
 * @tc.name: CreateAndRun002
 * @tc.desc: run ha no permit
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun002, TestSize.Level1)
{
    auto runner = EventRunner::Create();

    auto runResult = runner->Run();
    EXPECT_EQ(EVENT_HANDLER_ERR_RUNNER_NO_PERMIT, runResult);
}

/*
 * @tc.name: CreateAndRun003
 * @tc.desc: create eventrunner and run eventrunner in synchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun003, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto f = [&runner]() { runner->Stop(); };

    ErrCode result = EVENT_HANDLER_ERR_RUNNER_NO_PERMIT;
    if (handler->PostTask(f)) {
        result = runner->Run();
    }

    EXPECT_EQ(ERR_OK, result);
}

/*
 * @tc.name: CreateAndRun004
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with threadname
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun004, TestSize.Level1)
{
    std::string threadName(THREAD_NAME_TEST1);
    auto handler = std::make_shared<EventHandler>(EventRunner::Create(threadName));

    CreateRunnerWithName(handler, threadName);
}

/*
 * @tc.name: CreateAndRun005
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with threadname
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun005, TestSize.Level1)
{
    std::string threadName(THREAD_NAME_TEST2);
    auto handler = std::make_shared<EventHandler>(EventRunner::Create(threadName));

    CreateRunnerWithName(handler, threadName);
}

/*
 * @tc.name: CreateAndRun006
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with threadname
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun006, TestSize.Level1)
{
    std::string threadName(THREAD_NAME_TEST3);
    auto handler = std::make_shared<EventHandler>(EventRunner::Create(threadName));

    CreateRunnerWithName(handler, threadName);
}

/*
 * @tc.name: CreateAndRun007
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with threadname
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun007, TestSize.Level1)
{
    std::string threadName(THREAD_NAME_TEST4);
    auto handler = std::make_shared<EventHandler>(EventRunner::Create(threadName));

    CreateRunnerWithName(handler, threadName);
}

/*
 * @tc.name: CreateAndRun008
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with threadname
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun008, TestSize.Level1)
{
    std::string threadName(THREAD_NAME_TEST5);
    auto handler = std::make_shared<EventHandler>(EventRunner::Create(threadName));

    CreateRunnerWithName(handler, threadName);
}

/*
 * @tc.name: CreateAndRun009
 * @tc.desc: execute Run() function from one EventRunner multi times
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, CreateAndRun009, TestSize.Level1)
{
    const int64_t delayTime = 30;
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto mainTask = [&runner]() { runner->Stop(); };

    auto f = [&runner]() {
        usleep(10000);
        auto runResult = runner->Run();
        EXPECT_EQ(EVENT_HANDLER_ERR_RUNNER_ALREADY, runResult);
    };
    handler->PostTask(mainTask, delayTime);
    std::thread newThread(f);
    newThread.detach();
    runner->Run();
}

/*
 * @tc.name: GetCurrentEventQueue
 * @tc.desc: GetCurrentEventQueue success and then process IsQueueEmpty success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetCurrentEventQueue001, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {
        bool ret = EventRunner::GetCurrentEventQueue()->IsQueueEmpty();
        EXPECT_TRUE(ret);
    };
    handler->PostTask(task);
}

/*
 * @tc.name: GetCurrentEventQueue002
 * @tc.desc: GetCurrentEventQueue success and then process IsQueueEmpty success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetCurrentEventQueue002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {
        bool ret = EventRunner::GetCurrentEventQueue()->IsQueueEmpty();
        EXPECT_TRUE(ret);
    };
    handler->PostTask(task);
}

/*
 * @tc.name: GetCurrentEventQueue003
 * @tc.desc: GetCurrentEventQueue success and then process IsQueueEmpty success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetCurrentEventQueue003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {
        bool ret = EventRunner::GetCurrentEventQueue()->IsQueueEmpty();
        EXPECT_TRUE(ret);
    };
    handler->PostTask(task);
}

/*
 * @tc.name: GetCurrentEventQueue004
 * @tc.desc: GetCurrentEventQueue success and then process IsQueueEmpty success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetCurrentEventQueue004, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {
        bool ret = EventRunner::GetCurrentEventQueue()->IsIdle();
        EXPECT_TRUE(ret);
    };
    handler->PostTask(task);
}

/*
 * @tc.name: GetCurrentEventQueue005
 * @tc.desc: GetCurrentEventQueue success and then process IsQueueEmpty success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetCurrentEventQueue005, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {
        bool ret = EventRunner::GetCurrentEventQueue()->HasInnerEvent(nullptr, EVENT_ID_1);
        EXPECT_FALSE(ret);
    };
    handler->PostTask(task);
}

/*
 * @tc.name: GetEventQueue
 * @tc.desc: GetEventQueue success and then process IsQueueEmpty success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetEventQueue001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    bool ret = runner->GetEventQueue()->IsQueueEmpty();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: GetEventQueue
 * @tc.desc: GetEventQueue success and then check when queue is not empty has low event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetEventQueue002, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event = InnerEvent::Get(task);

    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    bool ret = runner->GetEventQueue()->IsQueueEmpty();
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: GetEventQueue003
 * @tc.desc: GetEventQueue success and then check when queue is not empty has idle event IsQueueEmpty return false
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetEventQueue003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() { ; };
    auto event = InnerEvent::Get(task);

    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::IDLE);
    bool ret = runner->GetEventQueue()->IsQueueEmpty();
    EXPECT_FALSE(ret);
}

/*
 * @tc.name: GetEventQueue
 * @tc.desc: GetEventQueue success and then process IsIdle success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetEventQueue004, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST1);
    auto handler = std::make_shared<EventHandler>(runner);
    bool ret = runner->GetEventQueue()->IsIdle();
    EXPECT_TRUE(ret);
}

/*
 * @tc.name: Current001
 * @tc.desc: start a runner, use Current() to get the running runner then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Current001, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        EXPECT_EQ(currentRunner, runner);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: Current002
 * @tc.desc: start a runner, use Current() to get the running runner then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Current002, TestSize.Level1)
{
    auto runner = EventRunner::Create();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        EXPECT_EQ(currentRunner, runner);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: Current003
 * @tc.desc: start a runner, use Current() to get the running runner then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Current003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        EXPECT_EQ(currentRunner, runner);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: Current004
 * @tc.desc: start a runner, use Current() to get the running runner then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Current004, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST1);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        EXPECT_EQ(currentRunner, runner);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: Current005
 * @tc.desc: start a runner, use Current() to get the running runner then compare
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, Current005, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST2);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        EXPECT_EQ(currentRunner, runner);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: IsCurrentRunnerThread001
 * @tc.desc: start a runner, use Current() to get the running runner then compare IsCurrentRunnerThread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, IsCurrentRunnerThread001, TestSize.Level1)
{
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        bool result = currentRunner->IsCurrentRunnerThread();
        EXPECT_TRUE(result);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: IsCurrentRunnerThread002
 * @tc.desc: start a runner, use Current() to get the running runner then compare IsCurrentRunnerThread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, IsCurrentRunnerThread002, TestSize.Level1)
{
    auto runner = EventRunner::Create();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        bool result = currentRunner->IsCurrentRunnerThread();
        EXPECT_TRUE(result);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: IsCurrentRunnerThread003
 * @tc.desc: start a runner, use Current() to get the running runner then compare IsCurrentRunnerThread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, IsCurrentRunnerThread003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        bool result = currentRunner->IsCurrentRunnerThread();
        EXPECT_TRUE(result);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: IsCurrentRunnerThread004
 * @tc.desc: start a runner, use Current() to get the running runner then compare IsCurrentRunnerThread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, IsCurrentRunnerThread004, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST1);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        bool result = currentRunner->IsCurrentRunnerThread();
        EXPECT_TRUE(result);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: IsCurrentRunnerThread005
 * @tc.desc: start a runner, use Current() to get the running runner then compare IsCurrentRunnerThread
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, IsCurrentRunnerThread005, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST2);
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        bool result = currentRunner->IsCurrentRunnerThread();
        EXPECT_TRUE(result);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: GetThreadId001
 * @tc.desc: start a runner, use Current() to get the running runner then compare GetThreadId is equal
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetThreadId001, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST5);
    uint64_t id1 = runner->GetThreadId();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&]() {
        auto currentRunner = EventRunner::Current();
        uint64_t id2 = runner->GetThreadId();
        EXPECT_EQ(id1, id2);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: GetThreadId002
 * @tc.desc: start a runner, use Current() to get the running runner then compare GetThreadId is equal
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetThreadId002, TestSize.Level1)
{
    auto runner = EventRunner::Create();
    uint64_t id1 = runner->GetThreadId();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&]() {
        auto currentRunner = EventRunner::Current();
        uint64_t id2 = currentRunner->GetThreadId();
        EXPECT_EQ(id1, id2);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: IsCurrentRunnerThread003
 * @tc.desc: start a runner, use Current() to get the running runner then compare GetThreadId is equal
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetThreadId003, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    uint64_t id1 = runner->GetThreadId();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&]() {
        auto currentRunner = EventRunner::Current();
        uint64_t id2 = currentRunner->GetThreadId();
        EXPECT_EQ(id1, id2);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: GetThreadId004
 * @tc.desc: start a runner, use Current() to get the running runner then compare GetThreadId is equal
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetThreadId004, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST1);
    uint64_t id1 = runner->GetThreadId();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&]() {
        auto currentRunner = EventRunner::Current();
        uint64_t id2 = currentRunner->GetThreadId();
        EXPECT_EQ(id1, id2);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: GetThreadId005
 * @tc.desc: start a runner, use Current() to get the running runner then compare GetThreadId is equal
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetThreadId005, TestSize.Level1)
{
    auto runner = EventRunner::Create(THREAD_NAME_TEST2);
    uint64_t id1 = runner->GetThreadId();
    auto handler = std::make_shared<EventHandler>(runner);

    auto f = [&]() {
        auto currentRunner = EventRunner::Current();
        uint64_t id2 = currentRunner->GetThreadId();
        EXPECT_EQ(id1, id2);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: GetMainEventRunner001
 * @tc.desc: check GetMainEventRunner001 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetMainEventRunner001, TestSize.Level1)
{
    std::shared_ptr<EventHandler> handler = std::make_shared<EventHandler>(EventRunner::GetMainEventRunner());
    std::shared_ptr<LoggerTest001> logtest = std::make_shared<LoggerTest001>();
    EventRunner::GetMainEventRunner()->SetLogger(logtest);
    auto task = []() { EventRunner::GetMainEventRunner()->Stop(); };
    handler->PostTask(task);
    EventRunner::GetMainEventRunner()->Run();
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: GetMainEventRunner002
 * @tc.desc: check GetMainEventRunner002 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetMainEventRunner002, TestSize.Level1)
{
    std::shared_ptr<EventHandler> handler = std::make_shared<EventHandler>(EventRunner::GetMainEventRunner());
    std::shared_ptr<LoggerTest002> logtest = std::make_shared<LoggerTest002>();
    EventRunner::GetMainEventRunner()->SetLogger(logtest);
    auto task = []() { EventRunner::GetMainEventRunner()->Stop(); };
    handler->PostTask(task);
    EventRunner::GetMainEventRunner()->Run();
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: GetMainEventRunner003
 * @tc.desc: check GetMainEventRunner003 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetMainEventRunner003, TestSize.Level1)
{
    std::shared_ptr<EventHandler> handler = std::make_shared<EventHandler>(EventRunner::GetMainEventRunner());
    std::shared_ptr<LoggerTest003> logtest = std::make_shared<LoggerTest003>();
    EventRunner::GetMainEventRunner()->SetLogger(logtest);
    auto task = []() { EventRunner::GetMainEventRunner()->Stop(); };
    handler->PostTask(task);
    EventRunner::GetMainEventRunner()->Run();
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: GetMainEventRunner004
 * @tc.desc: check GetMainEventRunner004 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetMainEventRunner004, TestSize.Level1)
{
    std::shared_ptr<EventHandler> handler = std::make_shared<EventHandler>(EventRunner::GetMainEventRunner());
    std::shared_ptr<LoggerTest004> logtest = std::make_shared<LoggerTest004>();
    EventRunner::GetMainEventRunner()->SetLogger(logtest);
    auto task = []() { EventRunner::GetMainEventRunner()->Stop(); };
    handler->PostTask(task);
    EventRunner::GetMainEventRunner()->Run();
    EXPECT_TRUE(g_isSetLogger);
}

/*
 * @tc.name: GetMainEventRunner001
 * @tc.desc: check GetMainEventRunner001 success
 * @tc.type: FUNC
 */
HWTEST_F(EmsEventRunnerSystemTest, GetMainEventRunner005, TestSize.Level1)
{
    std::shared_ptr<EventHandler> handler = std::make_shared<EventHandler>(EventRunner::GetMainEventRunner());
    std::shared_ptr<LoggerTest005> logtest = std::make_shared<LoggerTest005>();
    EventRunner::GetMainEventRunner()->SetLogger(logtest);
    auto task = []() { EventRunner::GetMainEventRunner()->Stop(); };
    handler->PostTask(task);
    EventRunner::GetMainEventRunner()->Run();
    EXPECT_TRUE(g_isSetLogger);
}
