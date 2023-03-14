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
#include "thread_pool.h"
#include <chrono>
#include <cstdio>

using namespace testing::ext;
using namespace OHOS;

class UtilsThreadPoolTest : public testing::Test
{
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

int g_times = 0;
bool g_ready = false;
std::mutex g_mutex;
std::condition_variable g_cv;

void UtilsThreadPoolTest::SetUpTestCase(void)
{
    // step 2: input testsuit setup step
}

void UtilsThreadPoolTest::TearDownTestCase(void)
{
    // step 2: input testsuit teardown step
}

void UtilsThreadPoolTest::SetUp(void)
{
    // recover
    g_times = 0;
    g_ready = false;
}

void UtilsThreadPoolTest::TearDown(void)
{
    // recover
    g_times = 0;
    g_ready = false;

}

HWTEST_F(UtilsThreadPoolTest, test_01, TestSize.Level0)
{
    ThreadPool pool;
    EXPECT_EQ(pool.GetName(), "");
    EXPECT_EQ((int)pool.GetMaxTaskNum(), 0);
    EXPECT_EQ((int)pool.GetThreadsNum(), 0);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
}

HWTEST_F(UtilsThreadPoolTest, test_02, TestSize.Level0)
{
    ThreadPool pool("test_02_pool");
    EXPECT_EQ(pool.GetName(), "test_02_pool");
    EXPECT_EQ((int)pool.GetMaxTaskNum(), 0);
    EXPECT_EQ((int)pool.GetThreadsNum(), 0);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
}

HWTEST_F(UtilsThreadPoolTest, test_03, TestSize.Level0)
{
    ThreadPool pool("test_02_pool");
    pool.SetMaxTaskNum(10);
    EXPECT_EQ(pool.GetName(), "test_02_pool");
    EXPECT_EQ((int)pool.GetMaxTaskNum(), 10);
    EXPECT_EQ((int)pool.GetThreadsNum(), 0);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
}

HWTEST_F(UtilsThreadPoolTest, test_04, TestSize.Level0)
{
    ThreadPool pool;
    pool.Start(4);
    EXPECT_EQ(pool.GetName(), "");
    EXPECT_EQ((int)pool.GetMaxTaskNum(), 0);
    EXPECT_EQ((int)pool.GetThreadsNum(), 4);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);

    // add no task, g_times has no change
    EXPECT_EQ(g_times, 0);
    pool.Stop();
}

void TestFuncAddOneTime(int& i)
{
    ++g_times;
}

void TestFuncSubOneTime(int& i)
{
    --g_times;
}

// simple task, total task num less than the MaxTaskNum
HWTEST_F(UtilsThreadPoolTest, test_05, TestSize.Level0)
{
    ThreadPool pool;
    pool.Start(5);
    EXPECT_EQ(pool.GetName(), "");
    EXPECT_EQ((int)pool.GetThreadsNum(), 5);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);

    for (int i = 0; i < 3; ++i)
    {
        auto task = std::bind(TestFuncAddOneTime, i);
        pool.AddTask(task);
    }

    for (int i = 0; i < 2; ++i)
    {
        auto task = std::bind(TestFuncSubOneTime, i);
        pool.AddTask(task);
    }

    sleep(1);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
    // add 5 tasks, add 3 times and sub 2 times
    EXPECT_EQ(g_times, 1);
    pool.Stop();
}

// simaple task, total task num exceed the MaxTaskNum and the threads num
HWTEST_F(UtilsThreadPoolTest, test_06, TestSize.Level0)
{
    ThreadPool pool;
    pool.Start(5);
    EXPECT_EQ(pool.GetName(), "");
    EXPECT_EQ((int)pool.GetThreadsNum(), 5);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);

    pool.SetMaxTaskNum(10);

    for (int i = 0; i < 8; ++i)
    {
        auto task = std::bind(TestFuncAddOneTime, i);
        pool.AddTask(task);
    }

    for (int i = 0; i < 7; ++i)
    {
        auto task = std::bind(TestFuncSubOneTime, i);
        pool.AddTask(task);
    }

    sleep(1);
    // 1 second should be enough to complete these tasks. if not, this case would be fail
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
    // add 5 task, add 3 times and sub 2 times
    EXPECT_EQ(g_times, 1);
    pool.Stop();
}

void TestFuncAddWait(int& i)
{
    ++g_times;
    printf("after func:%s0%d called, :%d\n", __func__, i, g_times);
    std::unique_lock<std::mutex> lk(g_mutex);
    g_cv.wait(lk, [] {return g_ready;});
    printf("func:%s0%d received ready signal!\n", __func__, i);
}

void TestFuncSubWait(int& i)
{
    --g_times;
    printf("after func:%s0%d called, :%d\n", __func__, i, g_times);
    std::unique_lock<std::mutex> lk(g_mutex);
    g_cv.wait(lk, [] {return g_ready;});
    printf("func:%s0%d received ready signal!\n", __func__, i);
}

// complex task, wait for notify by the main thread
// total task num less than the threads num and the MaxTaskNum
HWTEST_F(UtilsThreadPoolTest, test_07, TestSize.Level0)
{
    ThreadPool pool;
    pool.Start(5);
    EXPECT_EQ(pool.GetName(), "");
    EXPECT_EQ((int)pool.GetThreadsNum(), 5);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);

    for (int i = 0; i < 3; ++i)
    {
        auto task = std::bind(TestFuncAddWait, i);
        pool.AddTask(task);
    }

    for (int i = 0; i < 2; ++i)
    {
        auto task = std::bind(TestFuncSubWait, i);
        pool.AddTask(task);
    }

    std::this_thread::sleep_for(std::chrono::seconds(1)); // release cpu proactively, let the task threads go into wait
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_ready = true;
    }

    g_cv.notify_all();

    // these tasks are endless Loop, 5 threads process 5 tasks, zero task remains in the task queue
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
    // add 5 task, add 3 times and sub 2 times
    EXPECT_EQ(g_times, 1);
    pool.Stop();
}

HWTEST_F(UtilsThreadPoolTest, test_08, TestSize.Level0)
{
    ThreadPool pool;
    pool.Start(5);
    EXPECT_EQ(pool.GetName(), "");
    EXPECT_EQ((int)pool.GetThreadsNum(), 5);
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);

    pool.SetMaxTaskNum(10);

    // ADD 15 tasks
    for (int i = 0; i < 8; ++i)
    {
        auto task = std::bind(TestFuncAddWait, i);
        pool.AddTask(task);
    }

    for (int i = 0; i < 7; ++i)
    {
        auto task = std::bind(TestFuncSubWait, i);
        pool.AddTask(task);
    }

    sleep(1);
    // at this time, the first 5 tasks execute and wait for notify, the rest 10 tasks stay in the task queue.
    EXPECT_EQ((int)pool.GetCurTaskNum(), 10);
    // FIFO,
    EXPECT_EQ(g_times, 5);

    // notify_all
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_ready = true;
    }
    g_cv.notify_all();

    // after noity, task thread wake up, and g_ready is true, new tasks didn't need to wait
    sleep(1);
    // these tasks are endless Loop, and total num of task exceed the MaxTaskNum
    EXPECT_EQ((int)pool.GetCurTaskNum(), 0);
    EXPECT_EQ(g_times, 1);
    pool.Stop();
}

