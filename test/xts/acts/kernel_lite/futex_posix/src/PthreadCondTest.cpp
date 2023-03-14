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
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <gtest/gtest.h>
#include "utils.h"
#include "log.h"
#include "KernelConstants.h"
#include "FutexTest.h"
#include "mt_utils.h"

using namespace testing::ext;

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_INIT_0100
 * @tc.name     pthread_cond_init initializes condition variables
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondInit, Function | MediumTest | Level2)
{
    pthread_condattr_t condattr;
    EXPECT_EQ(pthread_condattr_init(&condattr), 0);
    pthread_cond_t cond1;
    EXPECT_EQ(pthread_cond_init(&cond1, &condattr), 0);

    pthread_cond_t cond2;
    EXPECT_EQ(pthread_cond_init(&cond2, nullptr), 0);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_DESTROY_0100
 * @tc.name     pthread_cond_destroy destroy condition variables
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondDestroy, Function | MediumTest | Level3)
{
    pthread_condattr_t condattr;
    EXPECT_EQ(pthread_condattr_init(&condattr), 0);
    pthread_cond_t cond1;
    EXPECT_EQ(pthread_cond_init(&cond1, &condattr), 0);
    EXPECT_EQ(pthread_cond_destroy(&cond1), 0);

    pthread_cond_t cond2;
    EXPECT_EQ(pthread_cond_init(&cond2, nullptr), 0);
    EXPECT_EQ(pthread_cond_destroy(&cond2), 0);

    pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
    EXPECT_EQ(pthread_cond_destroy(&cond3), 0);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CONDATTR_INIT_0100
 * @tc.name     Init and destroy operations
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondattrInit, Function | MediumTest | Level2)
{
    pthread_condattr_t condattr;

    EXPECT_EQ(pthread_condattr_init(&condattr), 0) << "> return errno";
    EXPECT_EQ(pthread_condattr_destroy(&condattr), 0) << "> return errno";
    EXPECT_EQ(pthread_condattr_init(&condattr), 0) << "> return errno";
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CONDATTR_SETCLOCK_0100
 * @tc.name     Set and get the clock selection variable properties of the condition variable properties
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondattrSetclock, Function | MediumTest | Level3)
{
    clockid_t clk;
    const int invalidClock = -100;
    pthread_condattr_t condattr;
    EXPECT_EQ(pthread_condattr_init(&condattr), 0);

    // default
    EXPECT_EQ(pthread_condattr_getclock(&condattr, &clk), 0) << "> return errno";
    EXPECT_EQ(clk, DEF_PROCESS_CONDATTR_CLOCK);

    EXPECT_EQ(pthread_condattr_setclock(&condattr, CLOCK_REALTIME), 0) << "> return errno";
    EXPECT_EQ(pthread_condattr_getclock(&condattr, &clk), 0) << "> return errno";
    EXPECT_EQ(clk, CLOCK_REALTIME);

    struct timespec ts = {0};
    EXPECT_EQ(clock_getres(CLOCK_MONOTONIC, &ts), 0) << "> return errno";
    EXPECT_EQ(pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC), 0) << "> return errno";
    EXPECT_EQ(pthread_condattr_getclock(&condattr, &clk), 0) << "> return errno";
    EXPECT_EQ(clk, CLOCK_MONOTONIC);

    EXPECT_EQ(pthread_condattr_setclock(&condattr, invalidClock), EINVAL) << "> return errno";
}

pthread_mutex_t g_mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond1 = PTHREAD_COND_INITIALIZER;

// pthread_cond_signal
void *ThreadPthreadCondSignalBefore1(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_mutex_lock(&g_mtx1), 0);
    LOG("step = %lx", CheckStep(2));
    EXPECT_EQ(pthread_cond_signal(&g_cond1), 0);
    LOG("step = %lx", CheckStep(3));
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx1), 0);
    return arg;
}

// pthread_cond_wait
void *ThreadPthreadCondSignalBefore2(void *arg)
{
    EXPECT_EQ(pthread_mutex_lock(&g_mtx1), 0);
    EXPECT_EQ(pthread_cond_wait(&g_cond1, &g_mtx1), 0);
    LOG("step = %lx", CheckStep(4));
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx1), 0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_SIGNAL_0100
 * @tc.name     Use pthread_cond_signal to release the conditional semaphore, pthread_cond_signal front
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondSignalBefore, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    LOG("step = %lx", CheckStep(1));

    ASSERT_EQ(pthread_create(&tid[0], nullptr, ThreadPthreadCondSignalBefore1, nullptr), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid[1], nullptr, ThreadPthreadCondSignalBefore2, nullptr), 0) << "> return errno";

    Msleep(100);
    pthread_join(tid[0], nullptr);
    pthread_join(tid[1], nullptr);
    EXPECT_EQ(pthread_cond_destroy(&g_cond1), 0);
    EXPECT_EQ(pthread_mutex_destroy(&g_mtx1), 0);
    EXPECT_EQ(CheckStep(5), (uint64_t)0x12345);
}

pthread_mutex_t g_mtx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond2 = PTHREAD_COND_INITIALIZER;

// pthread_cond_signal
void *ThreadTestCondSignalWaitAfter1(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_mutex_lock(&g_mtx2), 0);
    LOG("step = %lx", CheckStep(2));
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx2), 0);
    EXPECT_EQ(pthread_cond_signal(&g_cond2), 0);
    return arg;
}

// pthread_cond_wait
void *ThreadTestCondSignalWaitAfter2(void *arg)
{
    EXPECT_EQ(pthread_mutex_lock(&g_mtx2), 0);
    EXPECT_EQ(pthread_cond_wait(&g_cond2, &g_mtx2), 0);
    LOG("step = %lx", CheckStep(3));
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx2), 0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_SIGNAL_0200
 * @tc.name     Use pthread_cond_signal to release the conditional semaphore, pthread_cond_signal in the back
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondSignalAfter, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    LOG("step = %lx", CheckStep(1));

    ASSERT_EQ(pthread_create(&tid[0], nullptr, ThreadTestCondSignalWaitAfter1, nullptr), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid[1], nullptr, ThreadTestCondSignalWaitAfter2, nullptr), 0) << "> return errno";

    Msleep(100);
    pthread_join(tid[0], nullptr);
    pthread_join(tid[1], nullptr);
    EXPECT_EQ(pthread_cond_destroy(&g_cond2), 0);
    EXPECT_EQ(pthread_mutex_destroy(&g_mtx2), 0);
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
}

pthread_mutex_t g_mtx3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond3 = PTHREAD_COND_INITIALIZER;

// pthread_cond_broadcast
void *ThreadPthreadCondBroadcast1(void *arg)
{
    int *testIntP = (int *)arg;
    Msleep(20);
    EXPECT_EQ(pthread_mutex_lock(&g_mtx3), 0);
    *testIntP = 10;
    EXPECT_EQ(pthread_cond_broadcast(&g_cond3), 0);
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx3), 0);
    return arg;
}

// pthread_cond_wait
void *ThreadPthreadCondBroadcast2(void *arg)
{
    int *testIntP = (int *)arg;
    EXPECT_EQ(pthread_mutex_lock(&g_mtx3), 0);
    EXPECT_EQ(pthread_cond_wait(&g_cond3, &g_mtx3), 0);
    (*testIntP)++;
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx3), 0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_BROADCAST_0100
 * @tc.name     Use pthread_cond_broadcast to release conditional semaphore
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondBroadcast, Function | MediumTest | Level3)
{
    pthread_t tid[3];
    int testInt = 0;

    ASSERT_EQ(pthread_create(&tid[0], nullptr, ThreadPthreadCondBroadcast1, (void*)&testInt), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid[1], nullptr, ThreadPthreadCondBroadcast2, (void*)&testInt), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid[2], nullptr, ThreadPthreadCondBroadcast2, (void*)&testInt), 0) << "> return errno";

    Msleep(100);
    pthread_join(tid[0], nullptr);
    pthread_join(tid[1], nullptr);
    pthread_join(tid[2], nullptr);
    EXPECT_EQ(pthread_cond_destroy(&g_cond3), 0);
    EXPECT_EQ(pthread_mutex_destroy(&g_mtx3), 0);
    EXPECT_EQ(testInt, 12);
}

pthread_mutex_t g_mtx4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond4 = PTHREAD_COND_INITIALIZER;

// pthread_cond_signal
void *ThreadPthreadCondTimedwait1(void *arg)
{
    Msleep(50);
    EXPECT_EQ(pthread_mutex_lock(&g_mtx4), 0);
    LOG("step = %lx", CheckStep(2));
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx4), 0);
    EXPECT_EQ(pthread_cond_signal(&g_cond4), 0);
    return arg;
}

// pthread_cond_timedwait
void *ThreadPthreadCondTimedwait2(void *arg)
{
    const unsigned int nsecPerSec = 1000000000;
    const unsigned int nsecPer100Ms = 100000000;
    struct timespec ts = {0};
    EXPECT_EQ(pthread_mutex_lock(&g_mtx4), 0);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec = ts.tv_sec + (ts.tv_nsec + nsecPer100Ms) / nsecPerSec;
    ts.tv_nsec = (ts.tv_nsec + nsecPer100Ms) % nsecPerSec;

    EXPECT_EQ(pthread_cond_timedwait(&g_cond4, &g_mtx4,  &ts), 0);
    LOG("step = %lx", CheckStep(3));
    EXPECT_EQ(pthread_mutex_unlock(&g_mtx4), 0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_TIMEDWAIT_0100
 * @tc.name     Use pthread_cond_timedwait to get conditional semaphore
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondTimedwait, Function | MediumTest | Level3)
{
    pthread_t tid1;
    pthread_t tid2;
    LOG("step = %lx", CheckStep(1));

    ASSERT_EQ(pthread_create(&tid1, nullptr, ThreadPthreadCondTimedwait1, nullptr), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid2, nullptr, ThreadPthreadCondTimedwait2, nullptr), 0) << "> return errno";

    Msleep(100);
    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);
    EXPECT_EQ(pthread_cond_destroy(&g_cond4), 0);
    EXPECT_EQ(pthread_mutex_destroy(&g_mtx4), 0);
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
}

pthread_mutex_t g_mtx5 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond5 = PTHREAD_COND_INITIALIZER;

// pthread_cond_timedwait
void *ThreadPthreadCondTimedwaitOut(void *arg)
{
    struct timespec ts = {0};
    struct timespec tsNow = {0};
    EXPECT_EQ(pthread_mutex_lock(&g_mtx5), 0);

    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_cond_timedwait(&g_cond5, &g_mtx5, &ts), ETIMEDOUT) << "> return should errno";
    LOG("step = %lx", CheckStep(2));
    clock_gettime(CLOCK_REALTIME, &tsNow);

    int timeDiff = GetTimeDiff(tsNow, ts); // calculate time different
    EXPECT_GE(timeDiff, 0);
    EXPECT_LE(timeDiff, 20);

    EXPECT_EQ(pthread_mutex_unlock(&g_mtx5), 0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_TIMEDWAIT_0200
 * @tc.name     Use pthread_cond_timedwait to time out time measurement
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondTimedwaitOut, Function | MediumTest | Level3)
{
    pthread_t tid;
    LOG("step = %lx", CheckStep(1));

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCondTimedwaitOut, nullptr), 0) << "> return errno";

    Msleep(200);
    pthread_join(tid, nullptr);
    EXPECT_EQ(pthread_cond_destroy(&g_cond5), 0);
    EXPECT_EQ(pthread_mutex_destroy(&g_mtx5), 0);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}

pthread_mutex_t g_mtx6 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond6 = PTHREAD_COND_INITIALIZER;

// pthread_cond_timedwait
void *ThreadPthreadCondTimedwaitEinval(void *arg)
{
    const long einvalNsec = 1000000000;
    struct timespec ts = {0};
    EXPECT_EQ(pthread_mutex_lock(&g_mtx6), 0);

    ts.tv_sec = 1;
    ts.tv_nsec = einvalNsec;
    EXPECT_EQ(pthread_cond_timedwait(&g_cond6, &g_mtx6, &ts), EINVAL) << "> return should errno";
    CheckStep(2);

    EXPECT_EQ(pthread_mutex_unlock(&g_mtx6), 0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_COND_TIMEDWAIT_0300
 * @tc.name     test pthread_cond_timedwait EINVAL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadCondTimedwaitEinval, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCondTimedwaitEinval, nullptr), 0) << "> return errno";

    Msleep(200);
    pthread_join(tid, nullptr);
    EXPECT_EQ(pthread_cond_destroy(&g_cond6), 0);
    EXPECT_EQ(pthread_mutex_destroy(&g_mtx6), 0);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}