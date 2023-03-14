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
#include "KernelConstants.h"
#include "PthreadTest.h"

using namespace testing::ext;

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_SETDETACHSTATE_0100
 * @tc.name     basic test about pthread_attr_setdetachstate
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrSetdetachstate, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_attr_t attr;
    int param;

    // PTHREAD_CREATE_DETACHED
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getdetachstate(&attr, &param), 0);
    EXPECT_EQ(param, PTHREAD_CREATE_DETACHED);

    ASSERT_EQ(pthread_create(&tid, &attr, ThreadPublic, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
    EXPECT_EQ(pthread_join(tid, nullptr), EINVAL) << "> return errno";

    // PTHREAD_CREATE_JOINABLE
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getdetachstate(&attr, &param), 0);
    EXPECT_EQ(param, PTHREAD_CREATE_JOINABLE);

    ASSERT_EQ(pthread_create(&tid, &attr, ThreadPublic, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
}

void *ThreadPthreadAttrSetscope(void *arg)
{
    pthread_attr_t attr;
    int getScope;
    int setScope = PTHREAD_SCOPE_PROCESS;

    EXPECT_EQ(pthread_getattr_np(pthread_self(), &attr), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getscope(&attr, &getScope), 0) << "> return errno";
    LOG("getScope = %d", getScope);
    EXPECT_EQ(getScope, setScope);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_SCOPE_ALL_0100
 * @tc.name     Comprehensivetest test about competitive scope
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrSetscope, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_attr_t attr;
    int getScope;
    const int setScope = PTHREAD_SCOPE_PROCESS;
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_getscope(&attr, &getScope), 0) << "> return errno";
    LOG("getScope = %d", getScope);

    EXPECT_EQ(pthread_attr_setscope(&attr, setScope), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getscope(&attr, &getScope), 0) << "> return errno";
    LOG("getScope = %d", getScope);
    EXPECT_EQ(getScope, setScope);

    ASSERT_EQ(pthread_create(&tid, &attr, ThreadPthreadAttrSetscope, nullptr), 0) << "> return errno";

    Msleep(20);
    pthread_join(tid, nullptr);
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
}

void *ThreadPthreadAttrSetguardsize(void *arg)
{
    pthread_attr_t attr;
    size_t getGuardSize;
    const size_t setGuardSize = DEF_PROCESS_GUARD_SIZE * 2;

    EXPECT_EQ(pthread_getattr_np(pthread_self(), &attr), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getguardsize(&attr, &getGuardSize), 0) << "> return errno";
    LOG("getGuardSize = %ld", getGuardSize);
    EXPECT_EQ(getGuardSize, setGuardSize);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_SETGUARDSIZE_0100
 * @tc.name     basic test about pthread_attr_setguardsize
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrSetguardsize, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_attr_t attr;
    size_t getGuardSize;
    const size_t setGuardSize = DEF_PROCESS_GUARD_SIZE * 2;
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_getguardsize(&attr, &getGuardSize), 0) << "> return errno";
    LOG("getGuardSize = %ld", getGuardSize);

    EXPECT_EQ(pthread_attr_setguardsize(&attr, setGuardSize), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getguardsize(&attr, &getGuardSize), 0) << "> return errno";
    LOG("getGuardSize = %ld", getGuardSize);
    EXPECT_EQ(getGuardSize, setGuardSize);

    ASSERT_EQ(pthread_create(&tid, &attr, ThreadPthreadAttrSetguardsize, nullptr), 0) << "> return errno";

    Msleep(20);
    pthread_join(tid, nullptr);
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
}

void *ThreadPthreadAttrSetstacksize(void *arg)
{
    pthread_attr_t attr;
    size_t stackSize;
    size_t guardSize;

    EXPECT_EQ(pthread_getattr_np(pthread_self(), &attr), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getstacksize(&attr, &stackSize), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getguardsize(&attr, &guardSize), 0) << "> return errno";
    LOG("stackSize = %ld", stackSize);
    LOG("guardSize = %ld", guardSize);
    // must >= setsize
    EXPECT_GE(stackSize, PTHREAD_STACK_MIN);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_SETSTACKSIZE_0100
 * @tc.name     basic test about pthread_attr_setstacksize
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrSetstacksize, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_attr_t attr;
    size_t stackSize;

    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_getstacksize(&attr, &stackSize), 0) << "> return errno";
    stackSize = PTHREAD_STACK_MIN;
    LOG("PTHREAD_STACK_MIN = %d", PTHREAD_STACK_MIN);
    EXPECT_EQ(pthread_attr_setstacksize(&attr, stackSize), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getstacksize(&attr, &stackSize), 0) << "> return errno";
    LOG("stackSize = %d", stackSize);
    EXPECT_EQ(stackSize, PTHREAD_STACK_MIN);

    ASSERT_EQ(pthread_create(&tid, &attr, ThreadPthreadAttrSetstacksize, nullptr), 0) << "> return errno";

    Msleep(20);
    pthread_join(tid, nullptr);
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_SETSTACKSIZE_0200
 * @tc.name     test pthread_attr_setstacksize EINVAL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrSetstacksizeEINVAL, Function | MediumTest | Level3)
{
    pthread_attr_t attr;
    size_t stackSize;

    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_getstacksize(&attr, &stackSize), 0) << "> return errno";
    stackSize = PTHREAD_STACK_MIN - 1;
    LOG("PTHREAD_STACK_MIN = %d", PTHREAD_STACK_MIN);
    EXPECT_EQ(pthread_attr_setstacksize(&attr, stackSize), EINVAL) << "> return errno";
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
}

void *ThreadPthreadAttrSetstack(void *arg)
{
    pthread_attr_t attr;
    void *stackAddr1 = nullptr;
    size_t stackSize = 0;

    EXPECT_EQ(pthread_getattr_np(pthread_self(), &attr), 0) << "> return errno";
    EXPECT_EQ(pthread_attr_getstack(&attr, &stackAddr1, &stackSize), 0) << "> return errno";
    EXPECT_GE(stackSize, PTHREAD_STACK_MIN);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_SETSTACK_0100
 * @tc.name     basic test about pthread_attr_setstack
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrSetstack, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_attr_t attr;
    void *stackAddr = nullptr;
    size_t stackSize;

    EXPECT_EQ(pthread_attr_init(&attr), 0);

    stackAddr = malloc(PTHREAD_STACK_MIN);
    ASSERT_NE(stackAddr, nullptr);
    LOG("stackAddr = %p", stackAddr);

    stackSize = PTHREAD_STACK_MIN;
    posix_memalign(&stackAddr, getpagesize(), stackSize);
    LOG("getpagesize() = %d", getpagesize());
    LOG("stackAddr = %p", stackAddr);

    EXPECT_EQ(pthread_attr_setstack(&attr, stackAddr, stackSize), 0) << "> return errno";
    LOG("stackAddr = %p", stackAddr);
    EXPECT_EQ(pthread_attr_getstack(&attr, &stackAddr, &stackSize), 0) << "> return errno";
    LOG("stackAddr = %p", stackAddr);
    EXPECT_EQ(stackSize, PTHREAD_STACK_MIN);

    EXPECT_EQ(pthread_create(&tid, &attr, ThreadPthreadAttrSetstack, stackAddr), 0) << "> return errno";
    Msleep(100);
    pthread_join(tid, nullptr);
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATTR_GETSTACK_0100
 * @tc.name     test pthread_attr_setstack about return EINVAL
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAttrGetstackEINVAL, Function | MediumTest | Level3)
{
    pthread_attr_t attr;
    void *stackAddr = nullptr;
    size_t stackSize;
    EXPECT_EQ(pthread_attr_init(&attr), 0);
    EXPECT_EQ(pthread_attr_getstack(&attr, &stackAddr, &stackSize), EINVAL) << "> return errno != EINVAL";
    EXPECT_EQ(pthread_attr_destroy(&attr), 0);
}

void *ThreadPthreadGetattrNp(void *arg)
{
    pthread_attr_t attr;
    size_t stackSize;
    size_t guardSize;
    int param;

    EXPECT_EQ(pthread_getattr_np(pthread_self(), &attr), 0) << "> return errno";

    EXPECT_EQ(pthread_attr_getstacksize(&attr, &stackSize), 0) << "> return errno";
    // must >= DEF_PROCESS_STACK_SIZE
    EXPECT_GE(stackSize, DEF_PROCESS_STACK_SIZE);

    EXPECT_EQ(pthread_attr_getguardsize(&attr, &guardSize), 0) << "> return errno";
    EXPECT_EQ(guardSize, DEF_PROCESS_GUARD_SIZE);

    EXPECT_EQ(pthread_attr_getdetachstate(&attr, &param), 0);
    EXPECT_EQ(param, DEF_PROCESS_DETACHSTATE);

    EXPECT_EQ(pthread_attr_destroy(&attr), 0);

    LOG("stackSize = %ld", stackSize);
    LOG("guardSize; = %ld", guardSize);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_GETATTR_NP_0100
 * @tc.name     basic about about pthread_getattr_np
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadGetattrNp, Function | MediumTest | Level3)
{
    pthread_t tid;
    EXPECT_EQ(pthread_create(&tid, nullptr, ThreadPthreadGetattrNp, nullptr), 0) << "> return errno";
    Msleep(100);
    pthread_join(tid, nullptr);
}
