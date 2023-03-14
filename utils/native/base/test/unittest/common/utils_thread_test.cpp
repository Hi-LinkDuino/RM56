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
#include "thread_ex.h"
#include <iostream>
#include <cstdio>
#include <sys/prctl.h>

using namespace testing::ext;
using namespace OHOS;
using namespace std;

static int times = 0;
using ThreadRunFunc = bool (*)(int& data);

class UtilsThreadTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsThreadTest::SetUpTestCase(void)
{
    // input testsuit setup step
}

void UtilsThreadTest::TearDownTestCase(void)
{
    // input testsuit teardown step
}

void UtilsThreadTest::SetUp(void)
{
    // recover times
    times = 0;
}

void UtilsThreadTest::TearDown(void)
{
    // recover times
    times = 0;
}

// get priority of thread
int GetThreadPriority(const pthread_t& thread)
{
    sched_param param;
    int priority;
    int policy;
    int ret;

    // scheduling parameters of target thread
    ret = pthread_getschedparam(thread, &policy, &param);
    if (ret != 0) {
        printf("pthread_getschedparam failed! thread:%lu, ret:%d\n", thread, ret);
        return -1;
    }

    // sched_priority contains the priority of the thread
    priority = param.sched_priority;
    return priority;
}


bool TestRun01(int& data)
{
    ++data;
    return false;
}

bool TestRun02(int& data)
{
    ++data;
    return true;
}

bool TestRun03(int& data)
{
    static const int TRY_TIMES = 10;
    if (times <= TRY_TIMES) {
        ++data;
        return true;
    }

    return false;
}

constexpr int DEFAULT_PRIO = 0;
const std::string& DEFAULT_THREAD_NAME = "default";

class TestThread : public OHOS::Thread {
public:
    TestThread(const int data, const bool readyToWork, ThreadRunFunc runFunc)
        : data_(data), priority_(DEFAULT_PRIO), name_(DEFAULT_THREAD_NAME), readyToWork_(readyToWork), runFunc_(runFunc)
        {};

    TestThread() = delete;
    ~TestThread() {}

    bool ReadyToWork() override;

    int data_;
    int priority_;
    std::string name_;
protected:
    bool Run() override;

private:
    bool readyToWork_;
    ThreadRunFunc runFunc_;
};

bool TestThread::ReadyToWork()
{
    return readyToWork_;
}

bool TestThread::Run()
{
    priority_ = getpriority(PRIO_PROCESS, 0);
    char threadName[MAX_THREAD_NAME_LEN] = {0};
    prctl(PR_GET_NAME, threadName, 0, 0);
    name_ = threadName;

    if (runFunc_ != nullptr) {
        return (*runFunc_)(data_);
    }

    return false;
}

/*
 * @tc.name: testThread001
 * @tc.desc: ThreadTest
 */
HWTEST_F(UtilsThreadTest, testThread001, TestSize.Level0)
{
    times = 0;
    std::unique_ptr<TestThread> test = std::make_unique<TestThread>(0, false, TestRun01);
    ThreadStatus status = test->Start("test_thread_01", THREAD_PROI_LOW, 1024);
    EXPECT_EQ(status == ThreadStatus::OK, true);

    pthread_t thread = test->GetThread();

    // pthread_equal return non-zero if equal
    EXPECT_EQ(pthread_equal(thread, -1) != 0, (test->IsRunning() ? false : true));

    // ReadyToWork return false, RUN will not be called!
    EXPECT_EQ(test->priority_, DEFAULT_PRIO);
    EXPECT_EQ(test->name_, DEFAULT_THREAD_NAME);

    // get stacksize of threa, may be different because of system memory align
    EXPECT_EQ(test->data_, 0);
    EXPECT_EQ(times, 0);
    test->NotifyExitSync();
    sleep(1);
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));
}

/*
 * @tc.name: testThread002
 * @tc.desc: ThreadTest
 */
HWTEST_F(UtilsThreadTest, testThread002, TestSize.Level0)
{
    times = 0;
    std::unique_ptr<TestThread> test = std::make_unique<TestThread>(0, true, TestRun01);
    ThreadStatus status = test->Start("test_thread_02", THREAD_PROI_LOW, 1024);

    EXPECT_EQ(status == ThreadStatus::OK, true);

    sleep(1); // let the new thread has chance to run

    // pthread_equal return non-zero if equal, RUN return false,may exit already
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));

    // ReadyToWork return true, RUN will be called!
    EXPECT_EQ(test->priority_, THREAD_PROI_LOW);
    EXPECT_EQ(test->name_, "test_thread_02");

    EXPECT_EQ(test->data_, 1);
    EXPECT_EQ(times, 0);
    test->NotifyExitSync();
    sleep(1);
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));
}

/*
 * @tc.name: testThread003
 * @tc.desc: ThreadTest
 */
HWTEST_F(UtilsThreadTest, testThread003, TestSize.Level0)
{
    times = 0;
    std::unique_ptr<TestThread> test = std::make_unique<TestThread>(0, false, TestRun02);
    ThreadStatus status = test->Start("test_thread_03", THREAD_PROI_LOW, 1024);
    EXPECT_EQ(status == ThreadStatus::OK, true);

    pthread_t thread = test->GetThread();

    // pthread_equal return non-zero if equal
    EXPECT_EQ(pthread_equal(thread , -1) != 0, (test->IsRunning() ? false : true));

    // ReadyToWork return false, RUN will not be called!
    EXPECT_EQ(test->priority_, DEFAULT_PRIO);
    EXPECT_EQ(test->name_, DEFAULT_THREAD_NAME);

    EXPECT_EQ(test->data_, 0);
    EXPECT_EQ(times, 0);
    test->NotifyExitSync();
    sleep(1);
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));
}

/*
 * @tc.name: testThread004
 * @tc.desc: ThreadTest
 */
HWTEST_F(UtilsThreadTest, testThread004, TestSize.Level0)
{
    times = 0;
    std::unique_ptr<TestThread> test = std::make_unique<TestThread>(0, true, TestRun02);
    ThreadStatus status = test->Start("test_thread_04", THREAD_PROI_LOW, 1024);

    EXPECT_EQ(status == ThreadStatus::OK, true);

    sleep(1); // let the new thread has chance to run

    // pthread_equal return non-zero if equal, RUN return false,may exit already
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));

    // ReadyToWork return true, RUN will be called!
    EXPECT_EQ(test->priority_, THREAD_PROI_LOW);
    EXPECT_EQ(test->name_, "test_thread_04");

    EXPECT_GT(test->data_, 1);
    EXPECT_EQ(times, 0);
    test->NotifyExitSync();
    sleep(1);
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));
}

/*
 * @tc.name: testThread005
 * @tc.desc: ThreadTest
 */
HWTEST_F(UtilsThreadTest, testThread005, TestSize.Level0)
{
    times = 0;
    std::unique_ptr<TestThread> test = std::make_unique<TestThread>(0, false, TestRun03);
    ThreadStatus status = test->Start("test_thread_05", THREAD_PROI_LOW, 1024);
    EXPECT_EQ(status == ThreadStatus::OK, true);

    pthread_t thread = test->GetThread();

    // pthread_equal return non-zero if equal
    EXPECT_EQ(pthread_equal(thread , -1) != 0, (test->IsRunning() ? false : true));

    // ReadyToWork return false, RUN will not be called!
    EXPECT_EQ(test->priority_, DEFAULT_PRIO);
    EXPECT_EQ(test->name_, DEFAULT_THREAD_NAME);

    EXPECT_EQ(test->data_, 0);
    EXPECT_EQ(times, 0);
    test->NotifyExitSync();
    sleep(1);
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));
}

/*
 * @tc.name: testThread006
 * @tc.desc: ThreadTest
 */
HWTEST_F(UtilsThreadTest, testThread006, TestSize.Level0)
{
    times = 0;
    std::unique_ptr<TestThread> test = std::make_unique<TestThread>(0, true, TestRun03);
    ThreadStatus status = test->Start("test_thread_06", THREAD_PROI_LOW, 1024);

    EXPECT_EQ(status == ThreadStatus::OK, true);

    sleep(1); // let the new thread has chance to run

    // pthread_equal return non-zero if equal, RUN return false,may exit already
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));

    // ReadyToWork return true, RUN will be called!
    EXPECT_EQ(test->priority_, THREAD_PROI_LOW);
    EXPECT_EQ(test->name_, "test_thread_06");

    EXPECT_GT(test->data_, 10);
    EXPECT_EQ(times, 0);

    times = 100;
    EXPECT_GT(test->data_, 10);

    sleep(1); // let the new thread has chance to run

    // times > 10, TestRun03 return false, thread exit
    EXPECT_EQ(pthread_equal(test->GetThread(), -1) != 0, (test->IsRunning() ? false : true));
}

