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

#include "flutter/common/task_runners.h"
#include "flutter/fml/task_runner.h"
#include "gtest/gtest.h"

#include "base/thread/background_task_executor.h"
#include "base/thread/task_executor.h"
#include "core/common/flutter/flutter_task_executor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string UITASK = "ui task has executed";
const std::string UISYNCTASK = "ui sync task has executed";
const std::string PLATFORMTASK = "platform task has executed";
const std::string PLATFORMSYNCTASK = "platform sync task has executed";
const std::string IOTASK = "io task has executed";
const std::string IOSYNCTASK = "io sync task has executed";
const std::string JSTASK = "js task has executed";
const std::string JSSYNCTASK = "js sync task has executed";
const std::string BACKGROUNDTASK = "background task has executed";
const std::string BACKGROUNDSYNCTASK = "background sync task has executed";
const std::string LABEL = "task executor test";
const std::string THREADFIRST = "thread_1";
const std::string THREADSECOND = "thread_2";
const std::string THREADTHIRD = "thread_3";
const std::string THREADFOURTH = "thread_4";
const uint32_t DELAYTIME = 5;

} // namespace

class TaskExecutorsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void InitTaskExecutor();
    void GetString(const std::string& task, std::promise<std::string>&& taskPromise);

protected:
    RefPtr<TaskExecutor> taskExecutor_;
    std::string label_ = LABEL;
    std::unique_ptr<fml::Thread> ThreadFirst_ = std::make_unique<fml::Thread>(THREADFIRST);
    std::unique_ptr<fml::Thread> ThreadSecond_ = std::make_unique<fml::Thread>(THREADSECOND);
    std::unique_ptr<fml::Thread> ThreadThird_ = std::make_unique<fml::Thread>(THREADTHIRD);
    std::unique_ptr<fml::Thread> ThreadFourth_ = std::make_unique<fml::Thread>(THREADFOURTH);
    fml::RefPtr<fml::TaskRunner> platform_ = ThreadFirst_->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> gpu_ = ThreadSecond_->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> ui_ = ThreadThird_->GetTaskRunner();
    fml::RefPtr<fml::TaskRunner> io_ = ThreadFourth_->GetTaskRunner();
};

void TaskExecutorsTest::SetUpTestCase() {}

void TaskExecutorsTest::TearDownTestCase() {}

void TaskExecutorsTest::SetUp()
{
    InitTaskExecutor();
}

void TaskExecutorsTest::TearDown() {}

void TaskExecutorsTest::InitTaskExecutor()
{
    std::promise<void> promiseInit;
    std::future<void> futureInit = promiseInit.get_future();
    flutter::TaskRunners taskRunner(label_, platform_, gpu_, ui_, io_);
    auto flutterTaskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();

    platform_->PostTask([&promiseInit, flutterTaskExecutor]() {
        flutterTaskExecutor->InitPlatformThread();
        promiseInit.set_value();
    });

    flutterTaskExecutor->InitJsThread();
    flutterTaskExecutor->InitOtherThreads(taskRunner);
    futureInit.wait();

    taskExecutor_ = flutterTaskExecutor;
}

void TaskExecutorsTest::GetString(const std::string& task, std::promise<std::string>&& taskPromise)
{
    taskPromise.set_value(task);
}

/**
 * @tc.name: TaskExecutorsTest001
 * @tc.desc: test synchronous and asynchronous execution of the UI thread
 * @tc.type: FUNC
 */
HWTEST_F(TaskExecutorsTest, TaskExecutorsTest001, TestSize.Level0)
{
    /**
     * @tc.steps: step1. set the synchronous task.
     * @tc.expected: step1. task gets executed.
     */
    std::string uiTask;
    taskExecutor_->PostSyncTask([&uiTask]() { uiTask = UISYNCTASK; }, TaskExecutor::TaskType::UI);
    ASSERT_TRUE(uiTask == UISYNCTASK);

    /**
     * @tc.steps: step2. set asynchronous task.
     * @tc.expected: step2. task gets executed.
     */
    std::promise<std::string> taskPromise;
    std::future<std::string> taskFuture = taskPromise.get_future();
    taskExecutor_->PostDelayedTask([this, &taskPromise]() { this->GetString(UITASK, std::move(taskPromise)); },
        TaskExecutor::TaskType::UI, DELAYTIME);
    ASSERT_TRUE(taskFuture.get() == UITASK);

    /**
     * @tc.steps: step3. set empty task.
     * @tc.expected: step3. result return failed.
     */
    ASSERT_FALSE(taskExecutor_->PostTask(nullptr, TaskExecutor::TaskType::UI));
}

/**
 * @tc.name: TaskExecutorsTest002
 * @tc.desc: test synchronous and asynchronous execution of the PLATFORM thread
 * @tc.type: FUNC
 */
HWTEST_F(TaskExecutorsTest, TaskExecutorsTest002, TestSize.Level0)
{
    /**
     * @tc.steps: step1. set the synchronous task.
     * @tc.expected: step1. task gets executed.
     */
    std::string platformTask;
    taskExecutor_->PostSyncTask(
        [&platformTask]() { platformTask = PLATFORMSYNCTASK; }, TaskExecutor::TaskType::PLATFORM);
    ASSERT_TRUE(platformTask == PLATFORMSYNCTASK);

    /**
     * @tc.steps: step2. set asynchronous task.
     * @tc.expected: step2. task gets executed.
     */
    std::promise<std::string> taskPromise;
    std::future<std::string> taskFuture = taskPromise.get_future();
    taskExecutor_->PostDelayedTask([this, &taskPromise]() { this->GetString(PLATFORMTASK, std::move(taskPromise)); },
        TaskExecutor::TaskType::PLATFORM, DELAYTIME);
    ASSERT_TRUE(taskFuture.get() == PLATFORMTASK);

    /**
     * @tc.steps: step3. set empty task.
     * @tc.expected: step3. result return failed.
     */
    ASSERT_FALSE(taskExecutor_->PostTask(nullptr, TaskExecutor::TaskType::PLATFORM));
}

/**
 * @tc.name: TaskExecutorsTest003
 * @tc.desc: test synchronous and asynchronous execution of the IO thread
 * @tc.type: FUNC
 */
HWTEST_F(TaskExecutorsTest, TaskExecutorsTest003, TestSize.Level0)
{
    /**
     * @tc.steps: step1. set the synchronous task.
     * @tc.expected: step1. task gets executed.
     */
    std::string ioTask;
    taskExecutor_->PostSyncTask([&ioTask]() { ioTask = IOSYNCTASK; }, TaskExecutor::TaskType::IO);
    ASSERT_TRUE(ioTask == IOSYNCTASK);

    /**
     * @tc.steps: step2. set asynchronous task.
     * @tc.expected: step2. task gets executed.
     */
    std::promise<std::string> taskPromise;
    std::future<std::string> taskFuture = taskPromise.get_future();
    taskExecutor_->PostDelayedTask([this, &taskPromise]() { this->GetString(IOTASK, std::move(taskPromise)); },
        TaskExecutor::TaskType::IO, DELAYTIME);
    ASSERT_TRUE(taskFuture.get() == IOTASK);

    /**
     * @tc.steps: step3. set empty task.
     * @tc.expected: step3. result return failed.
     */
    ASSERT_FALSE(taskExecutor_->PostTask(nullptr, TaskExecutor::TaskType::IO));
}

/**
 * @tc.name: TaskExecutorsTest004
 * @tc.desc: test synchronous and asynchronous execution of the JS thread
 * @tc.type: FUNC
 */
HWTEST_F(TaskExecutorsTest, TaskExecutorsTest004, TestSize.Level0)
{
    /**
     * @tc.steps: step1. set the synchronous task.
     * @tc.expected: step1. task gets executed.
     */
    std::string jsTask;
    taskExecutor_->PostSyncTask([&jsTask]() { jsTask = JSSYNCTASK; }, TaskExecutor::TaskType::JS);
    ASSERT_TRUE(jsTask == JSSYNCTASK);

    /**
     * @tc.steps: step2. set asynchronous task.
     * @tc.expected: step2. task gets executed.
     */
    std::promise<std::string> taskPromise;
    std::future<std::string> taskFuture = taskPromise.get_future();
    taskExecutor_->PostDelayedTask([this, &taskPromise]() { this->GetString(JSTASK, std::move(taskPromise)); },
        TaskExecutor::TaskType::JS, DELAYTIME);
    ASSERT_TRUE(taskFuture.get() == JSTASK);

    /**
     * @tc.steps: step3. set empty task.
     * @tc.expected: step3. result return failed.
     */
    ASSERT_FALSE(taskExecutor_->PostTask(nullptr, TaskExecutor::TaskType::JS));
}

/**
 * @tc.name: TaskExecutorsTest005
 * @tc.desc: test synchronous and asynchronous execution of the BACKGROUND thread
 * @tc.type: FUNC
 */
HWTEST_F(TaskExecutorsTest, TaskExecutorsTest005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. set the synchronous task.
     * @tc.expected: step1. result return failed.
     */
    ASSERT_FALSE(taskExecutor_->PostSyncTask([]() {}, TaskExecutor::TaskType::BACKGROUND));

    /**
     * @tc.steps: step2. set asynchronous task.
     * @tc.expected: step2. task gets executed.
     */
    std::promise<std::string> taskPromise;
    std::future<std::string> taskFuture = taskPromise.get_future();
    taskExecutor_->PostTask([this, &taskPromise]() { this->GetString(BACKGROUNDTASK, std::move(taskPromise)); },
        TaskExecutor::TaskType::BACKGROUND);
    ASSERT_TRUE(taskFuture.get() == BACKGROUNDTASK);

    /**
     * @tc.steps: step3. set empty task.
     * @tc.expected: step3. result return failed.
     */
    ASSERT_FALSE(taskExecutor_->PostTask(nullptr, TaskExecutor::TaskType::BACKGROUND));
}

} // namespace OHOS::Ace