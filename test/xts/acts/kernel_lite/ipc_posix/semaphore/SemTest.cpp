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
#include <semaphore.h>
#include <pthread.h>
#include <gtest/gtest.h>
#include "utils.h"
#include "log.h"

using namespace testing::ext;

static const unsigned int KERNEL_NS_PER_SECOND = 1000000000;
static const unsigned int KERNEL_100MS_BY_NS = 100000000;

class SemTest : public::testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_INIT_0100
 * @tc.name     Use sem_init initialize the semaphore with 0
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemInitA, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;
    int testValue = 0;

    EXPECT_NE(sem_init(&sem, 0, testValue), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_TRUE(semValue == testValue);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_INIT_0110
 * @tc.name     Use sem_init initialize the semaphore with 1
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemInitB, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;
    int testValue = 1;

    EXPECT_NE(sem_init(&sem, 0, testValue), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, testValue);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_INIT_0120
 * @tc.name     Use sem_init initialize the semaphore with 100
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemInitC, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;
    int testValue = 10;

    EXPECT_NE(sem_init(&sem, 0, testValue), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, testValue);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_POST_0100
 * @tc.name     sem_post increases the semaphore count
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemPost, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;

    ASSERT_NE(sem_init(&sem, 0, 0), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    EXPECT_EQ(sem_post(&sem), 0) << "> sem_post errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);

    EXPECT_EQ(sem_post(&sem), 0) << "> sem_post errno = " << errno;
    EXPECT_EQ(sem_post(&sem), 0) << "> sem_post errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 3);

    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_WAIT_0100
 * @tc.name     sem_wait get semaphore
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemWait, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;

    ASSERT_NE(sem_init(&sem, 0, 3), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 3);

    EXPECT_EQ(sem_wait(&sem), 0) << "> sem_wait errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 2);

    EXPECT_EQ(sem_wait(&sem), 0) << "> sem_wait errno = " << errno;
    EXPECT_EQ(sem_wait(&sem), 0) << "> sem_wait errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_TRYWAIT_0100
 * @tc.name     sem_trywait get semaphore
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemTryWait, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;

    ASSERT_NE(sem_init(&sem, 0, 3), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 3);

    EXPECT_EQ(sem_trywait(&sem), 0) << "> sem_trywait errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 2);

    EXPECT_EQ(sem_trywait(&sem), 0) << "> sem_trywait errno = " << errno;
    EXPECT_EQ(sem_trywait(&sem), 0) << "> sem_trywait errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_DESTROY_0100
 * @tc.name     check sem_destroy function
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testSemdestroy, Function | MediumTest | Level3)
{
    sem_t sem;
    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

void *ThreadChat(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;

    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_wait(sem), 0) << "sem_wait errno = " << errno;
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_CHAT_0100
 * @tc.name     Inter-thread communication, check sem_wait function
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testThreadChat, Function | MediumTest | Level3)
{
    pthread_t tid;
    sem_t sem;
    int reInt = 0;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    reInt = pthread_create(&tid, nullptr, ThreadChat, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    Msleep(20);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;
    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;

    Msleep(20);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);

    reInt = pthread_join(tid, nullptr);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

void *ThreadSemTryWait(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;
    int loop = 10;

    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    for (int i = 0; i < loop; i++) {
        if (sem_trywait(sem) == 0) {
            break;
            
        }
        Msleep(10);
    }
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_CHAT_0200
 * @tc.name     basic function test : Inter-thread communication, check sem_trywait function
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testThreadSemTryWait, Function | MediumTest | Level3)
{
    sem_t sem;
    int reInt = 0;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    pthread_t tid;
    reInt = pthread_create(&tid, nullptr, ThreadSemTryWait, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    Msleep(20);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;
    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;

    reInt = pthread_join(tid, nullptr);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

void *ThreadSemTimedWait(void *arg)
{
    struct timespec ts = {0};
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;

    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec = ts.tv_sec + (ts.tv_nsec + KERNEL_100MS_BY_NS) / KERNEL_NS_PER_SECOND;
    ts.tv_nsec = (ts.tv_nsec + KERNEL_100MS_BY_NS) % KERNEL_NS_PER_SECOND;
    Msleep(50);
    EXPECT_EQ(sem_timedwait(sem, &ts), 0) << "sem_timedwait errno = " << errno;
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;

    Msleep(100);
    EXPECT_EQ(semValue, 1);
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_CHAT_0300
 * @tc.name     Inter-thread communication, check sem_timedwait function
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testThreadSemTimedWait, Function | MediumTest | Level3)
{
    pthread_t tid;
    sem_t sem;
    int reInt = 0;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    reInt = pthread_create(&tid, nullptr, ThreadSemTimedWait, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    Msleep(10);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_post(&sem), 0) << "> sem_post errno = " << errno;
    EXPECT_EQ(sem_post(&sem), 0) << "> sem_post errno = " << errno;

    reInt = pthread_join(tid, nullptr);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}
 
void *ThreadNThreadWait1(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;

    Msleep(100);
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_wait(sem), 0) << "sem_wait errno = " << errno;
    return nullptr;
}

void *ThreadNThreadWait2(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    Msleep(300);
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    Msleep(200);
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);
    EXPECT_EQ(sem_wait(sem), 0) << "sem_wait errno = " << errno;
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_CHAT_0400
 * @tc.name     N threads wait, main thread post
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testNThreadWait, Function | MediumTest | Level4)
{
    pthread_t tid1;
    pthread_t tid2;
    sem_t sem;
    int reInt = 0;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    reInt = pthread_create(&tid1, nullptr, ThreadNThreadWait1, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    reInt = pthread_create(&tid2, nullptr, ThreadNThreadWait2, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    Msleep(200);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;

    Msleep(20);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    Msleep(200);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;

    Msleep(20);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);

    reInt = pthread_join(tid1, nullptr);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    reInt = pthread_join(tid2, nullptr);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

void *ThreadNThreadPost1(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;

    Msleep(200);
    EXPECT_EQ(sem_post(sem), 0) << "sem_post errno = " << errno;
    Msleep(20);
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    return nullptr;
}

void *ThreadNThreadPost2(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    int semValue = 0;

    Msleep(300);
    EXPECT_EQ(sem_post(sem), 0) << "sem_post errno = " << errno;
    Msleep(20);
    EXPECT_EQ(sem_getvalue(sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_CHAT_0500
 * @tc.name     N threads post, main thread wait
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemTest, testNThreadPost, Function | MediumTest | Level4)
{
    pthread_t tid1;
    pthread_t tid2;
    struct timespec ts = {0};
    sem_t sem;
    int reInt = 0;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    reInt = pthread_create(&tid1, nullptr, ThreadNThreadPost1, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    reInt = pthread_create(&tid2, nullptr, ThreadNThreadPost2, (void*)&sem);
    ASSERT_EQ(reInt, 0) << "> pthread_create errno, reInt = " << reInt;

    Msleep(100);
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec = ts.tv_sec + 1;
    EXPECT_EQ(sem_timedwait(&sem, &ts), 0) << "> sem_timedwait errno = " << errno;

    Msleep(400);
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 1);
    EXPECT_EQ(sem_trywait(&sem), 0) << "> sem_trywait errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    reInt = pthread_join(tid1, nullptr);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    reInt = pthread_join(tid2, nullptr);
    EXPECT_EQ(reInt, 0) << "pthread_join failed, errno=" << reInt;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}
