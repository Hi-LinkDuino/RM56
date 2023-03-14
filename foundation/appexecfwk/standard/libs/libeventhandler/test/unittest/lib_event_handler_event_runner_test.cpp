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

#include <atomic>
#include <cerrno>
#include <thread>

#include <sys/prctl.h>

#include "event_handler.h"
#include "event_runner.h"

#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

static const uint32_t HAS_EVENT_ID = 100;
static const int64_t HAS_EVENT_PARAM = 1000;
bool isSetLogger = false;

/**
 * Wait until task is executed.
 *
 * @param f task we post.
 * @param handler handler we use to post task.
 * @param taskCalled flag to record whether task is executed.
 */
template<typename F>
static void WaitUntilTaskCalled(F const &f, const std::shared_ptr<EventHandler> &handler, const std::atomic<bool> &taskCalled)
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
        const size_t MAX_THREAD_NAME_SIZE = 16;
        char thisThreadName[MAX_THREAD_NAME_SIZE + 1];

        // Get current thread name and compare with the specified one.
        int32_t ret = prctl(PR_GET_NAME, thisThreadName);
        if (ret == 0) {
            thisThreadName[MAX_THREAD_NAME_SIZE] = '\0';
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

class LibEventHandlerEventRunnerTest : public testing::Test {
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

void LibEventHandlerEventRunnerTest::SetUpTestCase(void)
{}

void LibEventHandlerEventRunnerTest::TearDownTestCase(void)
{}

void LibEventHandlerEventRunnerTest::SetUp(void)
{}

void LibEventHandlerEventRunnerTest::TearDown(void)
{}

/*
 * @tc.name: CreateAndRun001
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, CreateAndRun001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and task.
     */
    auto handler = std::make_shared<EventHandler>(EventRunner::Create());
    std::atomic<bool> taskCalled(false);
    auto f = [&taskCalled]() { taskCalled.store(true); };

    /**
     * @tc.steps: step1. post task and wait until the task is executed.
     * @tc.expected: step1. the task has been executed and has not timed out.
     */
    WaitUntilTaskCalled(f, handler, taskCalled);
    auto called = taskCalled.load();
    EXPECT_TRUE(called);
}

/*
 * @tc.name: CreateAndRun002
 * @tc.desc: create eventrunner and run eventrunner in synchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, CreateAndRun002, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, runner is not deposit, init task to stop runner.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto f = [&runner]() { runner->Stop(); };

    /**
     * @tc.steps: step1. post task and run the runner .
     * @tc.expected: step1. start runner success.
     */
    ErrCode result = EVENT_HANDLER_ERR_RUNNER_NO_PERMIT;
    if (handler->PostTask(f)) {
        result = runner->Run();
    }

    EXPECT_EQ(ERR_OK, result);
}

/*
 * @tc.name: CreateAndRun003
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with threadname
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, CreateAndRun003, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and init runner with thread name.
     */
    std::string threadName("threadName");
    auto handler = std::make_shared<EventHandler>(EventRunner::Create(threadName));

    /**
     * @tc.steps: step1. post task to compare whether the runner thread name is the same as we set.
     * @tc.expected: step1. runner thread name is the same.
     */
    CreateRunnerWithName(handler, threadName);
}

/*
 * @tc.name: CreateAndRun004
 * @tc.desc: create eventrunner and run eventrunner in asynchronous thread with const
             char* type threadname
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, CreateAndRun004, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and init runner with  with const char* type threadname.
     */
    auto handler = std::make_shared<EventHandler>(EventRunner::Create("threadName"));

    /**
     * @tc.steps: step1. post task to compare whether the runner thread name is the same as we set.
     * @tc.expected: step1. runner thread name is the same.
     */
    CreateRunnerWithName(handler, "threadName");
}

/*
 * @tc.name: CreateAndRun005
 * @tc.desc: execute Run() function from one EventRunner multi times
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, CreateAndRun005, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, init a task to stop runner.
     */
    const int64_t DELAY_TIME = 30;
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto mainTask = [&runner]() { runner->Stop(); };

    /**
     * @tc.steps: step1. post delay task to start runner then new a thread to start the same runner again.
     * @tc.expected: step1. return runner is already running error.
     */
    auto f = [&runner]() {
        usleep(10000);
        auto runResult = runner->Run();
        EXPECT_EQ(EVENT_HANDLER_ERR_RUNNER_ALREADY, runResult);
    };
    handler->PostTask(mainTask, DELAY_TIME);
    std::thread newThread(f);
    newThread.detach();
    runner->Run();
}

/*
 * @tc.name: Stop001
 * @tc.desc: stop eventrunner in synchronous thread
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, Stop001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, runner is not deposit, init a task to stop runner.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    ErrCode result = EVENT_HANDLER_ERR_RUNNER_NO_PERMIT;
    auto f = [&result, runner]() { result = runner->Stop(); };

    /**
     * @tc.steps: step1. post task and start runner, check whether the runner
     *            is stopped successfully in synchronous thread.
     * @tc.expected: step1. runner is stopped successfully.
     */
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
HWTEST_F(LibEventHandlerEventRunnerTest, Stop002, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, runner is deposit.
     */
    auto runner = EventRunner::Create();

    /**
     * @tc.steps: step1. call run() and stop() of runner.
     * @tc.expected: step1. runner do not allow us to start or stop is manually when it is in asynchronous thread.
     */
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
HWTEST_F(LibEventHandlerEventRunnerTest, Stop003, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, runner is not deposit,  init a task to stop runner.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = [&runner]() { runner->Stop(); };

    /**
     * @tc.steps: step1. post the stop stop task then run the runner, then stop runner again.
     * @tc.expected: step1. stop() could be called multi times.
     */
    handler->PostTask(task);
    runner->Run();
    usleep(10000);
    auto stopResult = runner->Stop();
    EXPECT_EQ(ERR_OK, stopResult);
}

/*
 * @tc.name: Current001
 * @tc.desc: start a runner, use Current() to get the running runner then compare
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, Current001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner, runner is not deposit.
     */
    auto runner = EventRunner::Create(false);
    auto handler = std::make_shared<EventHandler>(runner);

    /**
     * @tc.steps: step1. post a task to compare the current running runner and the runner the handler binded to.
     * @tc.expected: step1. the current running runner is the same runner we run the task.
     */
    auto f = [&runner]() {
        auto currentRunner = EventRunner::Current();
        EXPECT_EQ(currentRunner, runner);
        runner->Stop();
    };
    handler->PostTask(f);
    runner->Run();
}

/*
 * @tc.name: SetLogger001
 * @tc.desc: check SetLogger001 success
 * @tc.type: FUNC
 */
HWTEST_F(LibEventHandlerEventRunnerTest, SetLogger001, TestSize.Level1)
{
    /**
     * @tc.setup: init handler and runner
     */
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get(HAS_EVENT_ID, HAS_EVENT_PARAM);
    auto task = []() {; };
    /**
     * @tc.steps: step1. send event
     * @tc.expected: step1. SetLogger success
     */
    std::shared_ptr<LoggerTest> logtest = std::make_shared<LoggerTest>();
    runner->SetLogger(logtest);
    handler->SendEvent(event, EventQueue::Priority::LOW);
    handler->PostTask(task, "task1");
    handler->PostTask(task);
    usleep(100 * 1000);
    EXPECT_TRUE(isSetLogger);
}
