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

class SemAbnormalTest : public::testing::Test {
};

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_INIT_0200
 * @tc.name     Use sem_init initialized value when value is SEM_VALUE_MAX
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemInitAbnormalSemvaluemax, Function | MediumTest | Level3)
{
    sem_t sem;
    int semValue = 0;
    LOG("> SEM_VALUE_MAX = %d", SEM_VALUE_MAX);

    EXPECT_NE(sem_init(&sem, 0, SEM_VALUE_MAX), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, SEM_VALUE_MAX);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_INIT_0210
 * @tc.name     Use sem_init initialized value when value is greater than SEM_VALUE_MAX
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemInitAbnormalGtsemvaluemax, Function | MediumTest | Level3)
{
    sem_t sem;
    unsigned int gtSemMax = (unsigned int)SEM_VALUE_MAX + 1;
    LOG("> SEM_VALUE_MAX = %d", SEM_VALUE_MAX);

    if (sem_init(&sem, 0, gtSemMax) == -1) {
        EXPECT_EQ(errno, EINVAL) << "> sem_init errno = " << errno;
    } else {
        LOG("> sem_init return unexpected");
        ADD_FAILURE();
    }
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_INIT_0220
 * @tc.name     Use sem_init initialized value twice
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemInitAbnormalInitTwice, Function | MediumTest | Level3)
{
    sem_t sem;
    EXPECT_NE(sem_init(&sem, 0, 1), -1) << "> sem_init errno = " << errno;
    EXPECT_NE(sem_init(&sem, 0, 1), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_POST_0200
 * @tc.name     sem_post increases the semaphore count near the maximum value
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemPostAbnormal, Function | MediumTest | Level3)
{
    sem_t sem;
    int semValue = 0;

    // = SEM_VALUE_MAX
    EXPECT_NE(sem_init(&sem, 0, SEM_VALUE_MAX), -1) << "> sem_init errno = " << errno;
    if (sem_post(&sem) == -1) {
        EXPECT_EQ(errno, EOVERFLOW) << "> sem_post errno = " << errno;
    } else {
        LOG("> sem_post return unexpected");
        ADD_FAILURE();
    }
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, SEM_VALUE_MAX);

    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_TIMEDWAIT_0200
 * @tc.name     sem_timedwait get semaphore, wait time abnormal, tv_nsec less than 0
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemTimedwaitAbnormalA, Function | MediumTest | Level3)
{
    struct timespec ts = {0};
    sem_t sem;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    ts.tv_sec = time(nullptr);
    ts.tv_nsec = -2;
    if (sem_timedwait(&sem, &ts) == -1) {
        EXPECT_EQ(errno, EINVAL) << "> sem_timedwait errno = " << errno;
    } else {
        LOG("> sem_timedwait return unexpected");
        ADD_FAILURE();
    }

    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_TIMEDWAIT_0300
 * @tc.name     sem_timedwait get semaphore, wait time abnormal
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemTimedwaitAbnormalB, Function | MediumTest | Level3)
{
    struct timespec ts = {0};
    sem_t sem;
    int semValue = 0;

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    ts.tv_sec = time(nullptr);
    ts.tv_nsec = KERNEL_NS_PER_SECOND;
    if (sem_timedwait(&sem, &ts) == -1) {
        EXPECT_EQ(errno, EINVAL) << "> sem_timedwait errno = " << errno;
    } else {
        LOG("> sem_timedwait return unexpected");
        ADD_FAILURE();
    }

    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);
    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}

/**
 * @tc.number   SUB_KERNEL_IPC_SEM_TRYWAIT_0110
 * @tc.name     test sem_trywait with EAGAIN
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(SemAbnormalTest, testSemTryWaitEagain, Function | MediumTest | Level2)
{
    sem_t sem;
    int semValue = 0;

    ASSERT_NE(sem_init(&sem, 0, 0), -1) << "> sem_init errno = " << errno;
    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    EXPECT_EQ(sem_trywait(&sem), -1) << "> sem_trywait no err";
    EXPECT_EQ(errno, EAGAIN) << "> errno = " << errno;

    EXPECT_EQ(sem_getvalue(&sem, &semValue), 0) << "> sem_getvalue errno = " << errno;
    EXPECT_EQ(semValue, 0);

    EXPECT_EQ(sem_destroy(&sem), 0) << "> sem_destroy errno = " << errno;
}
