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

pthread_rwlock_t g_rwlock1;

void *PthreadRWlockWW1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock1), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock1), 0) << "> return errno";
    return arg;
}

void *PthreadRWlockWW2(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock1), 0) << "> return errno";
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock1), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0100
 * @tc.name       test rdlock and wrlock, write - write
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRWlockWW, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock1, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadRWlockWW1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadRWlockWW2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock1), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock2;

void *PthreadRWlockWR1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock2), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock2), 0) << "> return errno";
    return arg;
}

void *PthreadRWlockWR2(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_rwlock_rdlock(&g_rwlock2), 0) << "> return errno";
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock2), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0200
 * @tc.name       test rdlock and wrlock, write - read
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRWlockWR, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock2, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadRWlockWR1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadRWlockWR2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock2), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock3;

void *PthreadRWlockRR1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_rdlock(&g_rwlock3), 0) << "> return errno";
    Msleep(50);
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock3), 0) << "> return errno";
    return arg;
}

void *PthreadRWlockRR2(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_rwlock_rdlock(&g_rwlock3), 0) << "> return errno";
    CheckStep(2);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock3), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0300
 * @tc.name       test rdlock and wrlock, read - read
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRWlockRR, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock3, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadRWlockRR1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadRWlockRR2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock3), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock4;

void *PthreadRWlockRW1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_rdlock(&g_rwlock4), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock4), 0) << "> return errno";
    return arg;
}

void *PthreadRWlockRW2(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock4), 0) << "> return errno";
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock4), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0400
 * @tc.name       test rdlock and wrlock, read - write
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRWlockRW, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock4, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadRWlockRW1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadRWlockRW2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock4), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock5;

void *PthreadRWtrylockWR1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_trywrlock(&g_rwlock5), 0) << "> return errno";
    Msleep(50);
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock5), 0) << "> return errno";
    return arg;
}

void *PthreadRWtrylockWR2(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_rwlock_tryrdlock(&g_rwlock5), EBUSY) << "> should return EBUSY";
    CheckStep(2);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0500
 * @tc.name       test tryrdlock and trywrlock, write - read
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRWtrylockWR, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock5, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadRWtrylockWR1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadRWtrylockWR2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock5), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock6;

void *PthreadRWtrylockRW1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_tryrdlock(&g_rwlock6), 0) << "> return errno";
    Msleep(50);
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock6), 0) << "> return errno";
    return arg;
}

void *PthreadRWtrylockRW2(void *arg)
{
    Msleep(20);
    EXPECT_EQ(pthread_rwlock_trywrlock(&g_rwlock6), EBUSY) << "> should return EBUSY";
    CheckStep(2);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0600
 * @tc.name       test tryrdlock and trywrlock, read - write
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRWtrylockRW, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock6, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadRWtrylockRW1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadRWtrylockRW2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock6), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock7;

void *PthreadTimdNoOutRWlockWR1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock7), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock7), 0) << "> return errno";
    return arg;
}

void *PthreadTimdNoOutRWlockWR2(void *arg)
{
    struct timespec ts = {0};
    Msleep(20);
    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_rwlock_timedrdlock(&g_rwlock7, &ts), 0) << "> return errno";
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock7), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0700
 * @tc.name       test pthread_rwlock_timedrdlock with no timeout , write - read
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadTimdNoOutRWlockWR, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock7, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadTimdNoOutRWlockWR1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadTimdNoOutRWlockWR2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock7), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock8;

void *PthreadTimdOutRWlockWR1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock8), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    Msleep(100);
    CheckStep(4);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock8), 0) << "> return errno";
    return arg;
}

void *PthreadTimdOutRWlockWR2(void *arg)
{
    struct timespec ts = {0};
    struct timespec tsNow = {0};
    Msleep(20);
    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_rwlock_timedrdlock(&g_rwlock8, &ts), ETIMEDOUT) << "> return errno";
    CheckStep(3);
    clock_gettime(CLOCK_REALTIME, &tsNow);
    int timeDiff = GetTimeDiff(tsNow, ts); // calculate time different
    EXPECT_GE(timeDiff, 0);
    EXPECT_LE(timeDiff, 20);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0800
 * @tc.name       test pthread_rwlock_timedrdlock with timeout, write - read
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadTimdOutRWlockWR, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock8, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadTimdOutRWlockWR1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadTimdOutRWlockWR2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(5), (uint64_t)0x12345);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock8), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlock9;

void *PthreadTimdOutRWlockWW1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlock9), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    Msleep(100);
    CheckStep(4);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlock9), 0) << "> return errno";
    return arg;
}

void *PthreadTimdOutRWlockWW2(void *arg)
{
    struct timespec ts = {0};
    struct timespec tsNow = {0};
    Msleep(20);
    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_rwlock_timedwrlock(&g_rwlock9, &ts), ETIMEDOUT) << "> return errno";
    CheckStep(3);
    clock_gettime(CLOCK_REALTIME, &tsNow);
    int timeDiff = GetTimeDiff(tsNow, ts); // calculate time different
    EXPECT_GE(timeDiff, 0);
    EXPECT_LE(timeDiff, 20);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_0900
 * @tc.name       test pthread_rwlock_timedwrlock with timeout, write - write
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadTimdOutRWlockWW, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlock9, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadTimdOutRWlockWW1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadTimdOutRWlockWW2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(5), (uint64_t)0x12345);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlock9), 0);
}

/********************************************* Test case dividing line ***********************************************/

pthread_rwlock_t g_rwlockA;

void *PthreadTimdNoOutRWlockWW1(void *arg)
{
    EXPECT_EQ(pthread_rwlock_wrlock(&g_rwlockA), 0) << "> return errno";
    Msleep(50);
    CheckStep(2);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlockA), 0) << "> return errno";
    return arg;
}

void *PthreadTimdNoOutRWlockWW2(void *arg)
{
    struct timespec ts = {0};
    Msleep(20);
    GetDelayedTime(&ts, 100);
    EXPECT_EQ(pthread_rwlock_timedwrlock(&g_rwlockA, &ts), 0) << "> return errno";
    CheckStep(3);
    EXPECT_EQ(pthread_rwlock_unlock(&g_rwlockA), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCK_ALL_1000
 * @tc.name       test pthread_rwlock_timedwrlock with no timeout, write - write
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadTimdNoOutRWlockWW, Function | MediumTest | Level3)
{
    pthread_t tid[2];
    CheckStep(1);
    EXPECT_EQ(pthread_rwlock_init(&g_rwlockA, nullptr), 0);

    EXPECT_EQ(pthread_create(&tid[0], nullptr, PthreadTimdNoOutRWlockWW1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_create(&tid[1], nullptr, PthreadTimdNoOutRWlockWW2, nullptr), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
    EXPECT_EQ(pthread_rwlock_destroy(&g_rwlockA), 0);
}

/********************************************* Test case dividing line ***********************************************/

/**
 * @tc.number     SUB_KERNEL_FUTEX_RWLOCKATTR_0100
 * @tc.name       test pthread_rwlockattr_init api
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(FutexTest, testPthreadRwlockAttr, Function | MediumTest | Level1)
{
    pthread_rwlockattr_t attr;
    EXPECT_EQ(pthread_rwlockattr_init(&attr), 0);
    EXPECT_EQ(pthread_rwlockattr_destroy(&attr), 0);
}
