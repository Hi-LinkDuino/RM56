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

#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <gtest/gtest.h>
#include "log.h"
#include "utils.h"
#include "mt_utils.h"
#include "KernelConstants.h"

using namespace testing::ext;
// global variables used to communicate between threads
static int g_policy = 0;
static int g_prioriy = 0;

class PthreadSchedApiTest : public testing::Test {
};
class PthreadAllPrioTest : public testing::TestWithParam<int> {
};

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_INHERIT_0100
 * @tc.name     test the default value of inheritsched.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrGetInheritsched, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int inheritsched = -1;
    int rt = pthread_attr_getinheritsched(&attr, &inheritsched);
    EXPECT_EQ(rt, 0) << "pthread_attr_getinheritsched failed. errno=" << errno;
    EXPECT_EQ(inheritsched, PTHREAD_INHERIT_SCHED)
        << "check default inheritsched failed. expect PTHREAD_INHERIT_SCHED";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_INHERIT_0200
 * @tc.name     test set and get inheritsched.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetInheritsched, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int rt = pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
    EXPECT_EQ(rt, 0) << "pthread_attr_setinheritsched failed. errno=" << errno;
    int inheritsched = -1;
    rt = pthread_attr_getinheritsched(&attr, &inheritsched);
    EXPECT_EQ(rt, 0) << "pthread_attr_getinheritsched failed. errno=" << errno;
    EXPECT_EQ(inheritsched, PTHREAD_INHERIT_SCHED) << "check inheritsched failed";

    rt = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    EXPECT_EQ(rt, 0) << "pthread_attr_setinheritsched failed. errno=" << errno;
    inheritsched = -1;
    rt = pthread_attr_getinheritsched(&attr, &inheritsched);
    EXPECT_EQ(rt, 0) << "pthread_attr_getinheritsched failed. errno=" << errno;
    EXPECT_EQ(inheritsched, PTHREAD_EXPLICIT_SCHED) << "check inheritsched failed";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_INHERIT_0300
 * @tc.name     pthread_attr_setinheritsched error test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetInheritschedError, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int n = -GetRandom(100);
    int rt = pthread_attr_setinheritsched(&attr, n);
    EXPECT_EQ(rt, EINVAL) << "pthread_attr_setinheritsched(" << n << ") should fail";
    n = 2 + GetRandom(100);
    rt = pthread_attr_setinheritsched(&attr, n);
    EXPECT_EQ(rt, EINVAL) << "pthread_attr_setinheritsched(" << n << ") should fail";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCHEDPARAM_0100
 * @tc.name     test the default value of sched_param.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrGetSchedParam, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    struct sched_param param = {0};
    int rt = pthread_attr_getschedparam(&attr, &param);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedparam failed. errno=" << errno;
    EXPECT_EQ(param.sched_priority, DEFAULT_THREAD_PRIORITY) << "check default pthread priority failed";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCHEDPARAM_0200
 * @tc.name     test set and get sched param.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetSchedParam, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    struct sched_param param = {0};
    int rt = pthread_attr_getschedparam(&attr, &param);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedparam failed. errno=" << errno;

    param.sched_priority = 22;
    rt = pthread_attr_setschedparam(&attr, &param);
    EXPECT_EQ(rt, 0) << "pthread_attr_setschedparam failed. errno=" << errno;

    rt = pthread_attr_getschedparam(&attr, &param);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedparam failed. errno=" << errno;
    EXPECT_EQ(param.sched_priority, 22) << "check pthread priority failed";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCHEDPARAM_0300
 * @tc.name     pthread_attr_setschedparam error test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetSchedParamError, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    struct sched_param param = {0};
    int rt = pthread_attr_getschedparam(&attr, &param);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedparam failed. errno=" << errno;

    param.sched_priority = LOWEST_USER_THREAD_PRIORITY + 1;
    rt = pthread_attr_setschedparam(&attr, &param);
    EXPECT_EQ(rt, EINVAL);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCHEDPOLICY_0100
 * @tc.name     test the default value of sched policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrGetSchedPolicy, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int policy = -1;
    int rt = pthread_attr_getschedpolicy(&attr, &policy);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedpolicy failed. errno=" << errno;
    EXPECT_EQ(policy, SCHED_RR) << "check default policy failed, expect SCHED_RR";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCHEDPOLICY_0200
 * @tc.name     test set and get sched policy.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetSchedPolicy, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int rt = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    EXPECT_EQ(rt, 0) << "pthread_attr_setschedparam failed. errno=" << errno;
    int policy = -1;
    rt = pthread_attr_getschedpolicy(&attr, &policy);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedpolicy failed. errno=" << errno;
    EXPECT_EQ(policy, SCHED_FIFO);

    rt = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    EXPECT_EQ(rt, 0) << "pthread_attr_setschedparam failed. errno=" << errno;
    rt = pthread_attr_getschedpolicy(&attr, &policy);
    EXPECT_EQ(rt, 0) << "pthread_attr_getschedpolicy failed. errno=" << errno;
    EXPECT_EQ(policy, SCHED_RR);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCHEDPOLICY_0300
 * @tc.name     pthread_attr_setschedpolicy error test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetSchedPolicyError, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);
    int rt;

    // SCHED_FIFO is 1, and SCHED_RR is 2
    const int testLoop = 7;
    int invalidPolicy[testLoop] = {SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_DEADLINE, SCHED_RESET_ON_FORK};
    invalidPolicy[5] = -GetRandom(10000);
    invalidPolicy[6] = GetRandom(10000) + 6;
    for (int i = 0; i < testLoop; i++) {
        rt = pthread_attr_setschedpolicy(&attr, invalidPolicy[i]);
        EXPECT_EQ(rt, EINVAL) << "errno check fail for policy=" << invalidPolicy[i];
    }
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCOPE_0100
 * @tc.name     test the default value of sched scope.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrGetScope, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int scope = -1;
    int rt = pthread_attr_getscope(&attr, &scope);
    EXPECT_EQ(rt, 0) << "pthread_attr_getscope failed. errno=" << errno;
    EXPECT_EQ(scope, PTHREAD_SCOPE_PROCESS) << "check default scope failed. expect PTHREAD_SCOPE_PROCESS";
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PATTR_SCOPE_0200
 * @tc.name     test set and get scope.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testAttrSetScope, Function | MediumTest | Level1)
{
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);

    int rt = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    EXPECT_EQ(rt, ENOTSUP); // liteos support PTHREAD_SCOPE_PROCESS only

    rt = pthread_attr_setscope(&attr, -GetRandom(10000));
    EXPECT_EQ(rt, EINVAL);

    rt = pthread_attr_setscope(&attr, GetRandom(10000) + 2);
    EXPECT_EQ(rt, EINVAL);

    int scope = -1;
    rt = pthread_attr_getscope(&attr, &scope);
    EXPECT_EQ(rt, 0) << "pthread_attr_getscope failed. errno=" << errno;
    EXPECT_EQ(scope, PTHREAD_SCOPE_PROCESS) << "check scope failed. expect PTHREAD_SCOPE_PROCESS";
}

void* ThreadFunc1(void* arg)
{
    sem_t *sem = (sem_t*)arg;

    int policy;
    struct sched_param param = {0};
    EXPECT_EQ(pthread_getschedparam(pthread_self(), &policy, &param), 0);
    EXPECT_EQ(policy, g_policy);
    EXPECT_EQ(param.sched_priority, g_prioriy);

    LOG("wait main thread check this thread para...");
    sem_wait(sem);
    return nullptr;
}
/**
 * @tc.number   SUB_KERNEL_SCHED_API_PTHREAD_GETSCHEDPARAM_0100
 * @tc.name     pthread_getschedparam basic test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testGetSchedParam, Function | MediumTest | Level1)
{
    sem_t sem;
    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "sem_init errno = " << errno;

    pthread_t ptSub;
    pthread_attr_t attr;
    struct sched_param param = {0};
    ASSERT_EQ(pthread_attr_init(&attr), 0);
    g_policy = SCHED_FIFO;
    param.sched_priority = g_prioriy = GetRandom(LOWEST_USER_THREAD_PRIORITY);

    EXPECT_EQ(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED), 0);
    EXPECT_EQ(pthread_attr_setschedpolicy(&attr, g_policy), 0);
    EXPECT_EQ(pthread_attr_setschedparam(&attr, &param), 0);
    int rt = pthread_create(&ptSub, &attr, ThreadFunc1, (void*)&sem);
    ASSERT_EQ(rt, 0) << "pthread_create failed!";

    LOG("check sub thread's para");
    int policy;
    EXPECT_EQ(pthread_getschedparam(ptSub, &policy, &param), 0);
    EXPECT_EQ(policy, g_policy);
    EXPECT_EQ(param.sched_priority, g_prioriy);

    LOGD("main:before post sem...");
    sem_post(&sem);
    LOGD("main:after post sem...");
    pthread_join(ptSub, nullptr);
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PTHREAD_SETSCHEDPARAM_0100
 * @tc.name     pthread_setschedparam basic test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_P(PthreadAllPrioTest, testSetSchedParam, Function | MediumTest | Level1)
{
    pthread_t ptSub;
    pthread_t ptSelf = pthread_self();
    sem_t sem;
    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "sem_init errno = " << errno;

    // change self sched para
    struct sched_param param = {0};
    int policy;
    EXPECT_EQ(pthread_getschedparam(ptSelf, &policy, &param), 0);
    param.sched_priority = g_prioriy = GetParam();
    if (GetRandom(100)%2) {
        policy = g_policy = SCHED_RR;
    } else {
        policy = g_policy = SCHED_FIFO;
    }
    LOG("========= Test policy(%d) and prioriy(%d) =========", g_policy, g_prioriy);

    int rt = pthread_setschedparam(ptSelf, policy, &param);
    ASSERT_EQ(rt, 0) << "pthread_setschedparam failed!";

    LOG("create sub thread");
    pthread_attr_t attr;
    ASSERT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED), 0);
    rt = pthread_create(&ptSub, &attr, ThreadFunc1, (void*)&sem);
    ASSERT_EQ(rt, 0) << "pthread_create failed!";

    LOG("check self and sub thread's para");
    EXPECT_EQ(pthread_getschedparam(ptSelf, &policy, &param), 0);
    EXPECT_EQ(policy, g_policy);
    EXPECT_EQ(param.sched_priority, g_prioriy);

    EXPECT_EQ(pthread_getschedparam(ptSub, &policy, &param), 0);
    EXPECT_EQ(policy, g_policy);
    EXPECT_EQ(param.sched_priority, g_prioriy);

    LOGD("main:before post sem...");
    sem_post(&sem);
    LOGD("main:after post sem...");
    pthread_join(ptSub, nullptr);


    LOG("restore...");
    policy = SCHED_RR;
    param.sched_priority = DEFAULT_THREAD_PRIORITY;
    rt = pthread_setschedparam(ptSelf, policy, &param);
    ASSERT_EQ(rt, 0) << "pthread_setschedparam failed!";
}
INSTANTIATE_TEST_CASE_P(PthreadSchedApiTest, PthreadAllPrioTest,
    testing::Range(HIGHEST_USER_THREAD_PRIORITY, LOWEST_USER_THREAD_PRIORITY + 1));

void* ThrdFuncForSetSchedParamTest(void* arg)
{
    sem_t *sem = (sem_t*)arg;
    pthread_t ptSelf = pthread_self();
    int rt, policy;
    struct sched_param param = {0};
    EXPECT_EQ(pthread_getschedparam(ptSelf, &policy, &param), 0);

    LOG("invalid policy test:");
    const int testLoop = 7;
    int invalidPolicy[testLoop] = {
        SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_DEADLINE, SCHED_RESET_ON_FORK,
        -(int)GetRandom(10000), (int)GetRandom(10000) + SCHED_DEADLINE
    };
    for (int i = 0; i < testLoop; i++) {
        rt = pthread_setschedparam(ptSelf, invalidPolicy[i], &param);
        EXPECT_EQ(rt, EINVAL) << "pthread_setschedparam should fail for policy=" << invalidPolicy[i];
    }

    LOG("invalid 'priority' test:");
    param.sched_priority = HIGHEST_USER_THREAD_PRIORITY - 1;
    EXPECT_EQ(pthread_setschedparam(ptSelf, SCHED_RR, &param), EINVAL);
    param.sched_priority = LOWEST_USER_THREAD_PRIORITY + 1;
    EXPECT_EQ(pthread_setschedparam(ptSelf, SCHED_RR, &param), EINVAL);

    LOG("tell main thread check this thread para...");
    sem_post(sem);
    Msleep(100);
    LOG("thread exit...");
    return nullptr;
}
/**
 * @tc.number   SUB_KERNEL_SCHED_API_PTHREAD_SETSCHEDPARAM_0200
 * @tc.name     pthread_setschedparam api error test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testSetSchedParamError, Function | MediumTest | Level3)
{
    sem_t sem;
    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "sem_init errno = " << errno;

    pthread_t ptSub;
    pthread_attr_t attr;
    struct sched_param param = {0};
    ASSERT_EQ(pthread_attr_init(&attr), 0);
    g_policy = SCHED_FIFO;
    param.sched_priority = g_prioriy = GetRandom(LOWEST_USER_THREAD_PRIORITY);

    EXPECT_EQ(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED), 0);
    EXPECT_EQ(pthread_attr_setschedpolicy(&attr, g_policy), 0);
    EXPECT_EQ(pthread_attr_setschedparam(&attr, &param), 0);
    int rt = pthread_create(&ptSub, &attr, ThrdFuncForSetSchedParamTest, (void*)&sem);
    ASSERT_EQ(rt, 0) << "pthread_create failed!";

    LOGD("main:wait sub-thread...");
    sem_wait(&sem);
    LOG("check sub thread's para doesn't changed");
    int policy;
    EXPECT_EQ(pthread_getschedparam(ptSub, &policy, &param), 0);
    EXPECT_EQ(policy, g_policy);
    EXPECT_EQ(param.sched_priority, g_prioriy);

    pthread_join(ptSub, nullptr);
}

void* ThrdFuncForSetSchedPrioTest(void* arg)
{
    sem_t *sem = (sem_t*)arg;
    pthread_t ptSelf = pthread_self();

    LOG("invalid 'priority' test:");
    EXPECT_EQ(pthread_setschedprio(ptSelf, HIGHEST_USER_THREAD_PRIORITY - 1), EINVAL);
    EXPECT_EQ(pthread_setschedprio(ptSelf, LOWEST_USER_THREAD_PRIORITY + 1), EINVAL);

    LOG("valid 'priority' test:");
    EXPECT_EQ(pthread_setschedprio(ptSelf, LOWEST_USER_THREAD_PRIORITY), 0);

    LOG("tell main thread check this thread para...");
    sem_post(sem);
    Msleep(100);
    LOG("thread exit...");
    return nullptr;
}
/**
 * @tc.number   SUB_KERNEL_SCHED_API_PTHREAD_SETSCHEDPRIO_0100
 * @tc.name     pthread_setschedprio api basic test.
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testSetSchedPrio, Function | MediumTest | Level1)
{
    sem_t sem;
    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "sem_init errno = " << errno;

    pthread_t ptSub;
    pthread_attr_t attr;
    struct sched_param param = {0};
    ASSERT_EQ(pthread_attr_init(&attr), 0);
    g_policy = SCHED_RR;
    param.sched_priority = g_prioriy = HIGHEST_USER_THREAD_PRIORITY;

    EXPECT_EQ(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED), 0);
    EXPECT_EQ(pthread_attr_setschedpolicy(&attr, g_policy), 0);
    EXPECT_EQ(pthread_attr_setschedparam(&attr, &param), 0);
    int rt = pthread_create(&ptSub, &attr, ThrdFuncForSetSchedPrioTest, (void*)&sem);
    ASSERT_EQ(rt, 0) << "pthread_create failed!";

    LOGD("main:wait sub-thread...");
    sem_wait(&sem);
    LOG("check sub thread's priority has changed");
    int policy;
    EXPECT_EQ(pthread_getschedparam(ptSub, &policy, &param), 0);
    EXPECT_EQ(policy, g_policy);
    EXPECT_EQ(param.sched_priority, LOWEST_USER_THREAD_PRIORITY);
    pthread_join(ptSub, nullptr);
}

// second
void *ThreadTestFifoSched1(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    EXPECT_EQ(sem_wait(sem), 0) << "sem_wait errno = " << errno;
    CheckStep(3);
    LOG("> This is ThreadSched1");
    return arg;
}

// first
void *ThreadTestFifoSched2(void *arg)
{
    sem_t *sem = (sem_t*)arg;
    EXPECT_EQ(sem_wait(sem), 0) << "sem_wait errno = " << errno;
    CheckStep(2);
    LOG("> This is ThreadSched2");
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_SCHED_API_PTHREAD_SCHED_ALL_0100
 * @tc.name     test thread high ang low priority function
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadSchedApiTest, testSchedFifo, Function | MediumTest | Level3)
{
    pthread_t tid1;
    pthread_t tid2;
    int param;
    pthread_attr_t attr;
    struct sched_param schedParam = {0};
    sem_t sem;
    CheckStep(1);

    ASSERT_EQ(sem_init(&sem, 0, 0), 0) << "> sem_init errno = " << errno;

    // low
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_setschedpolicy(&attr, SCHED_FIFO), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getschedpolicy(&attr, &param), 0);
    EXPECT_EQ(param, SCHED_FIFO);

    schedParam.sched_priority = 22;
    EXPECT_EQ(pthread_attr_setschedparam(&attr, &schedParam), 0) << "> return errno";

    ASSERT_EQ(pthread_create(&tid1, &attr, ThreadTestFifoSched1, (void*)&sem), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);

    // high
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_setschedpolicy(&attr, SCHED_FIFO), 0) << "> return errno";

    schedParam.sched_priority = 21;
    EXPECT_EQ(pthread_attr_setschedparam(&attr, &schedParam), 0) << "> return errno";

    ASSERT_EQ(pthread_create(&tid2, &attr, ThreadTestFifoSched2, (void*)&sem), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);

    Msleep(10);
    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;
    Msleep(10);
    EXPECT_EQ(sem_post(&sem), 0) << "sem_post errno = " << errno;

    EXPECT_EQ(pthread_join(tid1, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid2, nullptr), 0) << "> return errno";

    EXPECT_EQ(CheckStep(4), 0x1234Ull);    // make sure ThreadTestFifoSched2 ahead
}