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
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <gtest/gtest.h>
#include "utils.h"
#include "mt_utils.h"
#include "log.h"
#include "FutexTest.h"

using namespace testing::ext;

/********************************************* Test case dividing line ***********************************************/

void *ThreadMutex(void *arg)
{
    int delayTime = GetRandom(50);
    pthread_mutex_t *mtx = (pthread_mutex_t *)arg;
    EXPECT_EQ(pthread_mutex_lock(mtx), 0) << "> return errno";
    CheckStep(1);
    Msleep(delayTime);
    EXPECT_EQ(CheckStep(2), (uint64_t)0x12);
    EXPECT_EQ(pthread_mutex_unlock(mtx), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_MUTEX_ALL_0100
 * @tc.name       basic test of pthread_mutex
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMutex, Function | MediumTest | Level3)
{
    pthread_mutex_t mtx;
    const int loopNum = 5;
    pthread_t tid[loopNum];

    EXPECT_EQ(pthread_mutex_init(&mtx, nullptr), 0) << "> return errno";

    for (int i = 0; i < loopNum; i++) {
        EXPECT_EQ(pthread_create(&tid[i], nullptr, ThreadMutex, (void*)&mtx), 0) << "> return errno";
    }
    for (int i = 0; i < loopNum; i++) {
        EXPECT_EQ(pthread_join(tid[i], nullptr), 0) << "> return errno";
    }
    EXPECT_EQ(pthread_mutex_destroy(&mtx), 0) << "> return errno";
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadMtrylock1(void *arg)
{
    pthread_mutex_t *mtx = (pthread_mutex_t *)arg;
    EXPECT_EQ(pthread_mutex_trylock(mtx), 0) << "> return errno";
    CheckStep(1);
    Msleep(50);
    CheckStep(4);
    EXPECT_EQ(pthread_mutex_unlock(mtx), 0) << "> return errno";
    CheckStep(5);
    return arg;
}

void *ThreadMtrylock2(void *arg)
{
    Msleep(20);
    pthread_mutex_t *mtx = (pthread_mutex_t *)arg;
    CheckStep(2);
    EXPECT_EQ(pthread_mutex_trylock(mtx), EBUSY) << "> should return errno";
    CheckStep(3);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_MUTEX_ALL_0200
 * @tc.name       basic test of pthread_mutex_trylock
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMtrylock, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    pthread_mutex_t mtx;

    EXPECT_EQ(pthread_mutex_init(&mtx, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[0], nullptr, ThreadMtrylock1, (void*)&mtx), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, ThreadMtrylock2, (void*)&mtx), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_mutex_destroy(&mtx), 0) << "> return errno";
    EXPECT_EQ(CheckStep(6), (uint64_t)0x123456);
}

/********************************************* Test case dividing line ***********************************************/

struct PthreadMutexCond {
    const int loopNum = 10;
    const int countMax = 5;
    int count = 0;
    int top = 0;
    int bottom = 0;
    pthread_cond_t notfull = PTHREAD_COND_INITIALIZER;
    pthread_cond_t notempty = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
} g_st1;

void *PthreadProduce(void *arg)
{
    for (int i = 0; i < g_st1.loopNum; i++) {
        EXPECT_EQ(pthread_mutex_lock(&g_st1.mutex), 0) << "> return errno";
        // check full
        if ((g_st1.top + 1) % g_st1.countMax == g_st1.bottom) {
            EXPECT_EQ(pthread_cond_wait(&g_st1.notempty, &g_st1.mutex), 0);
        }
        // Produce
        g_st1.top = (g_st1.top + 1) % g_st1.countMax;
        g_st1.count++;
        LOG("producer g_st1.top = %d", g_st1.top);

        EXPECT_EQ(pthread_cond_signal(&g_st1.notempty), 0);
        EXPECT_EQ(pthread_mutex_unlock(&g_st1.mutex), 0) << "> return errno";
        Msleep(10);
    }
    return arg;
}

void *PthreadConsume(void *arg)
{
    for (int i = 0; i < g_st1.loopNum; i++) {
        EXPECT_EQ(pthread_mutex_lock(&g_st1.mutex), 0) << "> return errno";
        // check empty
        if (g_st1.top == g_st1.bottom) {
            EXPECT_EQ(pthread_cond_wait(&g_st1.notempty, &g_st1.mutex), 0);
        }
        // Consume
        g_st1.bottom = (g_st1.bottom + 1) % g_st1.countMax;
        g_st1.count--;
        LOG("consume g_st1.bottom = %d", g_st1.bottom);

        EXPECT_EQ(pthread_cond_signal(&g_st1.notempty), 0);
        EXPECT_EQ(pthread_mutex_unlock(&g_st1.mutex), 0) << "> return errno";

        Msleep(10);
    }
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_MUTEX_ALL_0300
 * @tc.name       test pthread_mutex with condition variable, produce and consume
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMutexCond, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    g_st1.count = 0;
    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadProduce, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadConsume, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(g_st1.count, 0);
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadPthreadMtimedlockOut(void *arg)
{
    pthread_mutex_t *mtx = (pthread_mutex_t*)arg;
    struct timespec ts = {0};
    struct timespec tsNow = {0};

    Msleep(20);
    CheckStep(1);
    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_mutex_timedlock(mtx, &ts), ETIMEDOUT) << "> return should errno";

    CheckStep(3);
    clock_gettime(CLOCK_REALTIME, &tsNow);
    int timeDiff = GetTimeDiff(tsNow, ts); // calculate time different
    EXPECT_GE(timeDiff, 0);
    EXPECT_LE(timeDiff, 20);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_FUTEX_PTHREAD_TIMEDLOCK_0100
 * @tc.name     test pthread_mutex_timedlock whith timeout
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMtimedlockOut, Function | MediumTest | Level3)
{
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_t tid;
    EXPECT_EQ(pthread_create(&tid, nullptr, ThreadPthreadMtimedlockOut, (void*)&mtx), 0) << "> return errno";

    EXPECT_EQ(pthread_mutex_lock(&mtx), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    Msleep(100);
    CheckStep(4);
    EXPECT_EQ(pthread_mutex_unlock(&mtx), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(5), (uint64_t)0x12345);
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadPthreadMtimedlockNoOut(void *arg)
{
    pthread_mutex_t *mtx = (pthread_mutex_t*)arg;
    struct timespec ts = {0};

    Msleep(20);
    CheckStep(1);
    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_mutex_timedlock(mtx, &ts), 0) << "> return errno";
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_FUTEX_PTHREAD_TIMEDLOCK_0200
 * @tc.name     test pthread_mutex_timedlock with no timeout
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMtimedlockNoOut, Function | MediumTest | Level3)
{
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_t tid;
    EXPECT_EQ(pthread_create(&tid, nullptr, ThreadPthreadMtimedlockNoOut, (void*)&mtx), 0) << "> return errno";

    EXPECT_EQ(pthread_mutex_lock(&mtx), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    EXPECT_EQ(pthread_mutex_unlock(&mtx), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
}

/********************************************* Test case dividing line ***********************************************/

/**
 * @tc.number     SUB_KERNEL_FUTEX_MTYPE_ALL_0100
 * @tc.name       test pthread_mutexattr_init api
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMattrType, Function | MediumTest | Level1)
{
    int type;
    pthread_mutexattr_t mutexTypeAttr;
    int errorType = 100;

    EXPECT_EQ(pthread_mutexattr_init(&mutexTypeAttr), 0);

    EXPECT_EQ(pthread_mutexattr_settype(&mutexTypeAttr, PTHREAD_MUTEX_DEFAULT), 0) << "> return errno";
    EXPECT_EQ(pthread_mutexattr_gettype(&mutexTypeAttr, &type), 0);
    EXPECT_EQ(type, PTHREAD_MUTEX_DEFAULT);

    EXPECT_EQ(pthread_mutexattr_settype(&mutexTypeAttr, PTHREAD_MUTEX_NORMAL), 0) << "> return errno";
    EXPECT_EQ(pthread_mutexattr_gettype(&mutexTypeAttr, &type), 0);
    EXPECT_EQ(type, PTHREAD_MUTEX_NORMAL);

    EXPECT_EQ(pthread_mutexattr_settype(&mutexTypeAttr, PTHREAD_MUTEX_RECURSIVE), 0) << "> return errno";
    EXPECT_EQ(pthread_mutexattr_gettype(&mutexTypeAttr, &type), 0);
    EXPECT_EQ(type, PTHREAD_MUTEX_RECURSIVE);

    EXPECT_EQ(pthread_mutexattr_settype(&mutexTypeAttr, PTHREAD_MUTEX_ERRORCHECK), 0) << "> return errno";
    EXPECT_EQ(pthread_mutexattr_gettype(&mutexTypeAttr, &type), 0);
    EXPECT_EQ(type, PTHREAD_MUTEX_ERRORCHECK);

    EXPECT_EQ(pthread_mutexattr_settype(&mutexTypeAttr, errorType), EINVAL) << "> return errno";
    EXPECT_EQ(pthread_mutexattr_gettype(&mutexTypeAttr, &type), 0);
    EXPECT_NE(type, errorType);
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadMattrTypeRecursive1(void *arg)
{
    pthread_mutex_t *mtx = (pthread_mutex_t *)arg;
    ChildAssertEQ(pthread_mutex_lock(mtx), 0);
    Msleep(30);
    ChildAssertEQ(pthread_mutex_unlock(mtx), 0);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_MTYPE_ALL_0200
 * @tc.name       type is PTHREAD_MUTEX_RECURSIVE, unlock when not in use
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMattrTypeRecursive1, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        int exitCode = 0;
        int type;
        pthread_t tid;
        pthread_mutex_t mtx;
        pthread_mutexattr_t mtxTypeAttr;

        ChildAssertEQ(pthread_mutexattr_init(&mtxTypeAttr), 0);
        ChildAssertEQ(pthread_mutexattr_settype(&mtxTypeAttr, PTHREAD_MUTEX_RECURSIVE), 0);
        ChildAssertEQ(pthread_mutexattr_gettype(&mtxTypeAttr, &type), 0);
        ChildAssertEQ(type, PTHREAD_MUTEX_RECURSIVE);
        ChildAssertEQ(pthread_mutex_init(&mtx, &mtxTypeAttr), 0);
        ChildAssertEQ(pthread_create(&tid, nullptr, ThreadMattrTypeRecursive1, (void*)&mtx), 0);
        Msleep(10);
        exitCode = ChildExpectEQ(pthread_mutex_unlock(&mtx), EPERM);
        ChildAssertEQ(pthread_join(tid, nullptr), 0);
        ChildAssertEQ(pthread_mutexattr_destroy(&mtxTypeAttr), 0);
        exit(exitCode);
    }
    Msleep(80);
    WaitProcExitedOK(pid);
}

/********************************************* Test case dividing line ***********************************************/

/**
 * @tc.number     SUB_KERNEL_FUTEX_MTYPE_ALL_0300
 * @tc.name       type is PTHREAD_MUTEX_RECURSIVE, lock when locked
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMattrTypeRecursive2, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        int exitCode = 0;
        int type;
        pthread_mutex_t mtx;
        pthread_mutexattr_t mtxTypeAttr;

        ChildAssertEQ(pthread_mutexattr_init(&mtxTypeAttr), 0);
        ChildAssertEQ(pthread_mutexattr_settype(&mtxTypeAttr, PTHREAD_MUTEX_RECURSIVE), 0);
        ChildAssertEQ(pthread_mutexattr_gettype(&mtxTypeAttr, &type), 0);
        ChildAssertEQ(type, PTHREAD_MUTEX_RECURSIVE);
        ChildAssertEQ(pthread_mutex_init(&mtx, &mtxTypeAttr), 0);

        ChildAssertEQ(pthread_mutex_lock(&mtx), 0);
        exitCode = ChildExpectEQ(pthread_mutex_lock(&mtx), 0);
        ChildAssertEQ(pthread_mutex_unlock(&mtx), 0);

        ChildAssertEQ(pthread_mutexattr_destroy(&mtxTypeAttr), 0);
        exit(exitCode);
    }
    Msleep(50);
    AssertProcExitedOK(pid);
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadMattrTypeRecursive3(void *arg)
{
    pthread_mutex_t *mtx = (pthread_mutex_t *)arg;
    ChildAssertEQ(pthread_mutex_lock(mtx), 0);
    Msleep(30);
    ChildAssertEQ(pthread_mutex_unlock(mtx), 0);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_MTYPE_ALL_0400
 * @tc.name       type is PTHREAD_MUTEX_ERRORCHECK, unlock when not in use
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMattrTypeRecursive3, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        int exitCode;
        int type;
        pthread_t tid;
        pthread_mutex_t mtx;
        pthread_mutexattr_t mtxTypeAttr;

        ChildAssertEQ(pthread_mutexattr_init(&mtxTypeAttr), 0);
        ChildAssertEQ(pthread_mutexattr_settype(&mtxTypeAttr, PTHREAD_MUTEX_ERRORCHECK), 0);
        ChildAssertEQ(pthread_mutexattr_gettype(&mtxTypeAttr, &type), 0);
        ChildAssertEQ(type, PTHREAD_MUTEX_ERRORCHECK);
        ChildAssertEQ(pthread_mutex_init(&mtx, &mtxTypeAttr), 0);
        ChildAssertEQ(pthread_create(&tid, nullptr, ThreadMattrTypeRecursive3, (void*)&mtx), 0);
        Msleep(10);
        exitCode = ChildExpectEQ(pthread_mutex_unlock(&mtx), EPERM);
        ChildAssertEQ(pthread_join(tid, nullptr), 0);
        ChildAssertEQ(pthread_mutexattr_destroy(&mtxTypeAttr), 0);
        exit(exitCode);
    }
    Msleep(80);
    WaitProcExitedOK(pid);
}

/********************************************* Test case dividing line ***********************************************/

/**
 * @tc.number     SUB_KERNEL_FUTEX_MTYPE_ALL_0500
 * @tc.name       type is PTHREAD_MUTEX_ERRORCHECK, lock when locked
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMattrTypeRecursive4, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        int exitCode = 0;
        int type;
        pthread_mutex_t mtx;
        pthread_mutexattr_t mtxTypeAttr;

        ChildAssertEQ(pthread_mutexattr_init(&mtxTypeAttr), 0);
        ChildAssertEQ(pthread_mutexattr_settype(&mtxTypeAttr, PTHREAD_MUTEX_ERRORCHECK), 0);
        ChildAssertEQ(pthread_mutexattr_gettype(&mtxTypeAttr, &type), 0);
        ChildAssertEQ(type, PTHREAD_MUTEX_ERRORCHECK);
        ChildAssertEQ(pthread_mutex_init(&mtx, &mtxTypeAttr), 0);

        ChildAssertEQ(pthread_mutex_lock(&mtx), 0);
        exitCode = ChildExpectEQ(pthread_mutex_lock(&mtx), EDEADLK);
        ChildAssertEQ(pthread_mutex_unlock(&mtx), 0);

        ChildAssertEQ(pthread_mutexattr_destroy(&mtxTypeAttr), 0);
        exit(exitCode);
    }
    Msleep(50);
    AssertProcExitedOK(pid);
}

/********************************************* Test case dividing line ***********************************************/

/**
 * @tc.number     SUB_KERNEL_FUTEX_MTYPE_ALL_0600
 * @tc.name       type is PTHREAD_MUTEX_NORMAL, lock when locked
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadMattrTypeRecursive5, Function | MediumTest | Level2)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        int type;
        pthread_mutex_t mtx;
        pthread_mutexattr_t mtxTypeAttr;

        LOG("> child pid = %d", getpid());
        ChildAssertEQ(pthread_mutexattr_init(&mtxTypeAttr), 0);
        ChildAssertEQ(pthread_mutexattr_settype(&mtxTypeAttr, PTHREAD_MUTEX_NORMAL), 0);
        ChildAssertEQ(pthread_mutexattr_gettype(&mtxTypeAttr, &type), 0);
        ChildAssertEQ(type, PTHREAD_MUTEX_NORMAL);
        ChildAssertEQ(pthread_mutex_init(&mtx, &mtxTypeAttr), 0);
        LOG("> lock");
        ChildAssertEQ(pthread_mutex_lock(&mtx), 0);

        // if add pthread_mutex_lock Unreachable
        LOG("> unlock");
        ChildAssertEQ(pthread_mutex_unlock(&mtx), 0);
        ChildAssertEQ(pthread_mutexattr_destroy(&mtxTypeAttr), 0);
        exit(0);
    }
    LOG("> parent pid = %d", getpid());
    Msleep(50);
    WaitProcExitedOK(pid);
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadSpinlock(void *arg)
{
    pthread_spinlock_t *spinLock = (pthread_spinlock_t *)arg;
    EXPECT_EQ(pthread_spin_lock(spinLock), 0) << "> return errno";
    CheckStep(1);
    Msleep(10);
    EXPECT_EQ(CheckStep(2), (uint64_t)0x12);
    EXPECT_EQ(pthread_spin_unlock(spinLock), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_SPINLOCK_ALL_0100
 * @tc.name       basic test of pthread_spin_lock
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadSpinlock, Function | MediumTest | Level3)
{
    pthread_spinlock_t spinLock;
    const int loopNum = 5;
    pthread_t tid[loopNum];

    EXPECT_EQ(pthread_spin_init(&spinLock, 0), 0);

    for (int i = 0; i < loopNum; i++) {
        EXPECT_EQ(pthread_create(&tid[i], nullptr, ThreadSpinlock, (void*)&spinLock), 0) << "> return errno";
    }
    for (int i = 0; i < loopNum; i++) {
        EXPECT_EQ(pthread_join(tid[i], nullptr), 0) << "> return errno";
    }
    EXPECT_EQ(pthread_spin_destroy(&spinLock), 0);
}

/********************************************* Test case dividing line ***********************************************/

void *ThreadSpinTrylock1(void *arg)
{
    pthread_spinlock_t *spinLock = (pthread_spinlock_t *)arg;
    EXPECT_EQ(pthread_spin_trylock(spinLock), 0) << "> return errno";
    CheckStep(1);
    Msleep(50);
    CheckStep(4);
    EXPECT_EQ(pthread_spin_unlock(spinLock), 0) << "> return errno";
    CheckStep(5);
    return arg;
}

void *ThreadSpinTrylock2(void *arg)
{
    Msleep(20);
    pthread_spinlock_t *spinLock = (pthread_spinlock_t *)arg;
    CheckStep(2);
    EXPECT_EQ(pthread_spin_trylock(spinLock), EBUSY) << "> should return errno";
    CheckStep(3);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_SPINLOCK_ALL_0200
 * @tc.name       basic test of pthread_spin_trylock
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadSpinTrylock, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    pthread_spinlock_t spinLock;

    EXPECT_EQ(pthread_spin_init(&spinLock, 0), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[0], nullptr, ThreadSpinTrylock1, (void*)&spinLock), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, ThreadSpinTrylock2, (void*)&spinLock), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_spin_destroy(&spinLock), 0);
    EXPECT_EQ(pthread_spin_trylock(&spinLock), 0) << "> return errno";
    EXPECT_EQ(CheckStep(6), (uint64_t)0x123456);
}
