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
#include "PthreadTest.h"
#include "mt_utils.h"

using namespace testing::ext;

void *ThreadPthreadCreateBasic(void *arg)
{
    char *s = (char*)arg;
    EXPECT_STREQ(s, "1234567890 !@#$%^&*()_= ZXCVBNM [];'./>?:\" +-*/qwertyuiopasdfghjklzxcvbnm");
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CREATE_0100
 * @tc.name     pthread_create create a thread
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCreateBasic, Function | MediumTest | Level2)
{
    pthread_t tid;
    char str[] = "1234567890 !@#$%^&*()_= ZXCVBNM [];'./>?:\" +-*/qwertyuiopasdfghjklzxcvbnm";
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCreateBasic, (void*)str), 0) << "> return errno";

    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
}

void *ThreadPthreadJoinWait(void *arg)
{
    Msleep(20);
    CheckStep(2);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_JOIN_0100
 * @tc.name     Test the waiting function of pthread_join
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadJoinWait, Function | MediumTest | Level2)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadJoinWait, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123) << "pthread_join no wait";
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_JOIN_0200
 * @tc.name     Test the function of pthread_join to get the return value
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testJoinReturn, Function | MediumTest | Level3)
{
    pthread_t tid;
    int num = 4;
    void *joinRe = nullptr;

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPublic, (void*)&num), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid, &joinRe), 0) << "> return errno";

    int *p = (int*)joinRe;
    EXPECT_TRUE(&num == p);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_JOIN_0300
 * @tc.name     Test the function about pthread_join, but child thread Exited
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testJoinExited, Function | MediumTest | Level3)
{
    pthread_t tid;
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPublic, nullptr), 0) << "> return errno";
    Msleep(50);
    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
}

void *ThreadPthreadExitThread(void *arg)
{
    pthread_exit(arg);
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_EXIT_0100
 * @tc.name     Test the return function of pthread_exit in the child thread
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadExitThread, Function | MediumTest | Level3)
{
    pthread_t tid;
    int num = 4;
    void *joinRe = nullptr;

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadExitThread, (void*)&num), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid, &joinRe), 0) << "> return errno";
    int *p = (int*)joinRe;
    EXPECT_TRUE(&num == p);
}

void FunPthreadExit(void *arg)
{
    pthread_exit(arg);
}

void *ThreadPthreadExitFunction(void *arg)
{
    FunPthreadExit(arg);
    return nullptr;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_EXIT_0200
 * @tc.name     Test the return function of pthread_exit in the child thread function
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadExitFunction, Function | MediumTest | Level3)
{
    pthread_t tid;
    int num = 4;
    void *joinRe = nullptr;

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadExitFunction, (void*)&num), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid, &joinRe), 0) << "> return errno";
    int *p = (int*)joinRe;
    EXPECT_TRUE(&num == p);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_DETACH_0100
 * @tc.name     Use pthread_detach to detach child threads
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadDetach, Function | MediumTest | Level3)
{
    pthread_t tid;
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPublic, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_detach(tid), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid, nullptr), EINVAL) << "> return errno";
}

void *ThreadPthreadEqual(void *arg)
{
    pthread_t *tid = (pthread_t*)arg;
    EXPECT_NE(pthread_equal(*tid, pthread_self()), 0) << "pthread_equal should be equal";
    Msleep(10);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_EQUAL_0100
 * @tc.name     Use pthread_equal checks process equality
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadEqual, Function | MediumTest | Level3)
{
    pthread_t tid;
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadEqual, (void*)&tid), 0) << "> return errno";
    EXPECT_EQ(pthread_equal(tid, pthread_self()), 0) << "pthread_equal should be no equal";
    EXPECT_EQ(pthread_join(tid, nullptr), 0) << "> return errno";
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_KEY_CREATE_0100
 * @tc.name     Check the parameter PTHREAD_KEYS_MAX of pthread_key_create
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadKeysMax, Function | MediumTest | Level3)
{
    int creatCnt = -1;
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> parent: fork errno = " << errno;
    if (pid == 0) {
        int re;
        int exitCode = 0;
        pthread_key_t keys[PTHREAD_KEYS_MAX];
        for (int i = 0; i < PTHREAD_KEYS_MAX; i++) {
            re = pthread_key_create(&keys[i], nullptr);
            if (re == EAGAIN) {
                LOG("pthread_key_create %d errno = %d", i, re);
                creatCnt = i;
                break;
            } else if (re != 0) {
                LOG("pthread_key_create %d errno = %d", i, re);
                exitCode = 1;
                creatCnt = i;
                break;
            }
        }

        if (creatCnt == -1) {
            creatCnt = PTHREAD_KEYS_MAX;
        }

        for (int i = 0; i < creatCnt; i++) {
            re = pthread_key_delete(keys[i]);
            if (re != 0) {
                LOG("pthread_key_delete %d errno = %d", i, re);
                exitCode = 1;
                break;
            }
        }
        exit(exitCode);
    }
    WaitProcExitedOK(pid);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_GETSPECIFIC_0100
 * @tc.name     Use pthread_getspecific to get the value corresponding to the parameter key
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadGetspecific, Function | MediumTest | Level3)
{
    pthread_key_t keys;
    int testInt = 100;
    void *testVoidP = (void*)&testInt;
    EXPECT_EQ(pthread_key_create(&keys, nullptr), 0) << "> return errno";

    void *reVoidP = pthread_getspecific(keys);
    EXPECT_EQ(reVoidP, nullptr);

    EXPECT_EQ(pthread_setspecific(keys, testVoidP), 0) << "> return errno";

    reVoidP = pthread_getspecific(keys);
    EXPECT_EQ(reVoidP, testVoidP);

    EXPECT_EQ(pthread_key_delete(keys), 0) << "> return errno";

    reVoidP = pthread_getspecific(keys);
    EXPECT_EQ(reVoidP, nullptr);
}

pthread_key_t g_keys;
pthread_once_t g_once = PTHREAD_ONCE_INIT;
int g_intPthreadSpecificAll = 0;

// Destructor function, excute after ThreadPthreadSpecificAll
void DestructorPthreadSpecificAll(void *param)
{
    int *p = (int*)param;
    *p += 1;
}

void OnceFunc()
{
    // DestructorPthreadSpecificAll() execute twice
    EXPECT_EQ(pthread_key_create(&g_keys, DestructorPthreadSpecificAll), 0) << "> return errno";
    EXPECT_EQ(pthread_getspecific(g_keys), nullptr);
    // execute once
    g_intPthreadSpecificAll++;
    EXPECT_EQ(g_intPthreadSpecificAll, 1);
}

void *ThreadPthreadSpecificAll(void *arg)
{
    // OnceFunc() execute once
    int reInt = pthread_once(&g_once, OnceFunc);
    EXPECT_TRUE((reInt == 0) || (reInt == 2));

    if (pthread_getspecific(g_keys) == nullptr) {
        // execute twice
        g_intPthreadSpecificAll++;
        EXPECT_EQ(pthread_setspecific(g_keys, (void*)&g_intPthreadSpecificAll), 0) << "> return errno";
    }
    // only 2 or 4
    EXPECT_GE(g_intPthreadSpecificAll, 2);
    EXPECT_NE(g_intPthreadSpecificAll, 3);
    EXPECT_LE(g_intPthreadSpecificAll, 4);
    return arg;
}

void DestructorPthreadSpecificAllDelete(void *param)
{
    int *p = (int*)param;
    *p += 1;
}

void *ThreadPthreadSpecificAllDelete(void *arg)
{
    pthread_key_t keys;
    EXPECT_EQ(pthread_key_create(&keys, DestructorPthreadSpecificAllDelete), 0) << "> return errno";
    EXPECT_EQ(pthread_getspecific(keys), nullptr);
    EXPECT_EQ(pthread_setspecific(keys, arg), 0) << "> return errno";
    EXPECT_EQ(pthread_getspecific(keys), arg);
    EXPECT_EQ(pthread_key_delete(keys), 0) << "> return errno";
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_SPECIFIC_ALL_0200
 * @tc.name     Delete the key before the child thread exits
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadSpecificAllDelete, Function | MediumTest | Level3)
{
    int data = 1;
    pthread_t tid;
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadSpecificAllDelete, (void*)&data), 0) << "> return errno";
    Msleep(100);
    pthread_join(tid, nullptr);
    EXPECT_EQ(data, 1);
}

void TestPushHandler1(void *arg)
{
    CheckStep(3);
}

void TestPushHandler2(void *arg)
{
    CheckStep(2);
}

void *ThreadTestPush(void *arg)
{
    pthread_cleanup_push(TestPushHandler1, nullptr);
    pthread_cleanup_push(TestPushHandler2, nullptr);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CLEANUP_ALL_0100
 * @tc.name     pthread_cleanup_push and pthread_cleanup_pop basic test
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCleanupPushPopBasic, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadTestPush, nullptr), 0) << "> return errno";
    Msleep(100);
    pthread_join(tid, nullptr);
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
}

void TestPushParamHandler(void *arg)
{
    int *testInt = (int*)arg;
    EXPECT_EQ(*testInt, 1);
    CheckStep(2);
}

void *ThreadTestPushParam(void *arg)
{
    int testInt = 1;
    pthread_cleanup_push(TestPushParamHandler, (void*)&testInt);
    pthread_cleanup_pop(1);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CLEANUP_ALL_0200
 * @tc.name     pthread_cleanup_push and pthread_cleanup_pop test with param
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCleanupPushPopParam, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadTestPushParam, nullptr), 0) << "> return errno";
    Msleep(100);
    pthread_join(tid, nullptr);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}

void TestPopHandler1(void *arg)
{
    CheckStep(2);
}

void TestPopHandler2(void *arg)
{
    // unreachable
    LOG("step = %lx", CheckStep(10));
}

static void *ThreadTestop(void *arg)
{
    pthread_cleanup_push(TestPopHandler1, nullptr);
    pthread_cleanup_push(TestPopHandler2, nullptr);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CLEANUP_ALL_0300
 * @tc.name     pthread_cleanup_pop use different parameters
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPop, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadTestop, nullptr), 0) << "> return errno";
    Msleep(100);
    pthread_join(tid, nullptr);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_BARRIER_INIT_0100
 * @tc.name     basic test of pthread_barrier_init
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testBarrierInit, Function | MediumTest | Level2)
{
    pthread_barrier_t barrier;
    EXPECT_EQ(pthread_barrier_init(&barrier, nullptr, 0), EINVAL) << "> return errno";
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_BARRIERATTR_INIT_0100
 * @tc.name     basic test with pthread_barrierattr_init
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testBarrierattrInit, Function | MediumTest | Level3)
{
    pthread_barrierattr_t barrierAttr;
    pthread_barrier_t barrier;
    EXPECT_EQ(pthread_barrierattr_init(&barrierAttr), 0);
    EXPECT_EQ(pthread_barrierattr_destroy(&barrierAttr), 0);
    EXPECT_EQ(pthread_barrier_init(&barrier, &barrierAttr, 1), 0) << "> return errno";
    EXPECT_EQ(pthread_barrier_destroy(&barrier), 0) << "> return errno";
}

static pthread_barrier_t g_barrier;
void *ThreadTestBarrierWait1(void *arg)
{
    int *intP = (int*)arg;
    int reInt = pthread_barrier_wait(&g_barrier);
    if (reInt == PTHREAD_BARRIER_SERIAL_THREAD) {
        CheckStep(2);
    } else if (reInt == 0) {
        Msleep(30);
        *intP += 1;
    } else {
        ADD_FAILURE();
    }
    return arg;
}

void *ThreadTestBarrierWait2(void *arg)
{
    int *intP = (int*)arg;
    int reInt = pthread_barrier_wait(&g_barrier);
    if (reInt == PTHREAD_BARRIER_SERIAL_THREAD) {
        CheckStep(2);
    } else if (reInt == 0) {
        Msleep(20);
        *intP += 1;
    } else {
        ADD_FAILURE();
    }
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_BARRIER_ALL_0100
 * @tc.name     test pthread_barrier_wait return value
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testBarrierWait, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_t tid1;
    CheckStep(1);
    int intParam = 0;

    EXPECT_EQ(pthread_barrier_init(&g_barrier, nullptr, 3), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadTestBarrierWait1, (void*)&intParam), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid1, nullptr, ThreadTestBarrierWait2, (void*)&intParam), 0) << "> return errno";
    int reInt = pthread_barrier_wait(&g_barrier);
    if (reInt == PTHREAD_BARRIER_SERIAL_THREAD) {
        CheckStep(2);
    } else if (reInt == 0) {
        Msleep(10);
        intParam++;
    } else {
        ADD_FAILURE();
    }
    Msleep(50);
    pthread_join(tid, nullptr);
    pthread_join(tid1, nullptr);
    EXPECT_EQ(pthread_barrier_destroy(&g_barrier), 0) << "> return errno";
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
    EXPECT_EQ(intParam, 2);
}

void *ThreadTestBarrierAlwaysWait(void *arg)
{
    pthread_barrier_t *barrier = (pthread_barrier_t*)arg;
    int reInt = pthread_barrier_wait(barrier);
    if ((reInt != PTHREAD_BARRIER_SERIAL_THREAD) && (reInt != 0)) {
        ADD_FAILURE();
    }
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_BARRIER_ALL_0200
 * @tc.name     Comprehensive test with barrier properties, always waiting
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testBarrierAlwaysWait, Function | MediumTest | Level3)
{
    pthread_t tid;
    pthread_t tid1;
    pthread_barrier_t barrier;
    CheckStep(1);

    EXPECT_EQ(pthread_barrier_init(&barrier, nullptr, 3), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadTestBarrierAlwaysWait, (pthread_barrier_t*)&barrier), 0)
        << "> return errno";
    ASSERT_EQ(pthread_create(&tid1, nullptr, ThreadTestBarrierAlwaysWait, (pthread_barrier_t*)&barrier), 0)
        << "> return errno";

    Msleep(100);
    EXPECT_EQ(CheckStep(2), (uint64_t)0x12);  // childs threads always waiting
    int reInt = pthread_barrier_wait(&barrier);
    if ((reInt != PTHREAD_BARRIER_SERIAL_THREAD) && (reInt != 0)) {
        ADD_FAILURE();
    }
    pthread_join(tid, nullptr);
    pthread_join(tid1, nullptr);
    EXPECT_EQ(pthread_barrier_destroy(&barrier), 0) << "> return errno";
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_SETCANCELSTATE_0100
 * @tc.name     basic test about pthread_setcancelstate
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadSetcancelstate, Function | MediumTest | Level3)
{
    int oldState = 0;
    EXPECT_EQ(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldState), 0) << "> return errno";
    EXPECT_EQ(oldState, PTHREAD_CANCEL_ENABLE);
    EXPECT_EQ(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState), 0) << "> return errno";
    EXPECT_EQ(oldState, PTHREAD_CANCEL_DISABLE);
    EXPECT_EQ(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr), 0) << "> return errno";
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_SETCANCELTYPE_0100
 * @tc.name     basic test about pthread_setcanceltype
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadSetcanceltype, Function | MediumTest | Level3)
{
    int oldState = 0;
    EXPECT_EQ(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldState), 0) << "> return errno";
    EXPECT_EQ(oldState, PTHREAD_CANCEL_DEFERRED);
    EXPECT_EQ(pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &oldState), 0) << "> return errno";
    EXPECT_EQ(oldState, PTHREAD_CANCEL_ASYNCHRONOUS);
    EXPECT_EQ(pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr), 0) << "> return errno";
}

void *ThreadPthreadNoCancelPoint(void *arg)
{
    CheckStep(2);
    KeepRun(50);
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CANCEL_0100
 * @tc.name     test pthread_cancel with no cancel point
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadNoCancelPoint, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadNoCancelPoint, nullptr), 0) << "> return errno";
    Msleep(10);
    EXPECT_EQ(pthread_cancel(tid), 0);
    Msleep(100);
    EXPECT_EQ(pthread_join(tid, NULL), 0);
}

void *ThreadPthreadCancelPoint(void *arg)
{
    CheckStep(2);
    KeepRun(50);
    pthread_testcancel();

    // unreachable
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CANCEL_0200
 * @tc.name     test pthread_cancel with cancel point
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCancelPoint, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCancelPoint, nullptr), 0) << "> return errno";
    Msleep(20);
    EXPECT_EQ(pthread_cancel(tid), 0);
    Msleep(100);
    EXPECT_EQ(pthread_join(tid, NULL), 0);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}

void *ThreadPthreadCancelDisable(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
    CheckStep(2);
    Msleep(50);
    pthread_testcancel();
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CANCEL_ALL_0100
 * @tc.name     test pthread_cancel with PTHREAD_CANCEL_DISABLE
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCancelDisable, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCancelDisable, nullptr), 0) << "> return errno";
    Msleep(10);
    EXPECT_EQ(pthread_cancel(tid), 0);
    Msleep(100);
    EXPECT_EQ(pthread_join(tid, NULL), 0);
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
}

void *ThreadPthreadCancelAsynchronous(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
    CheckStep(2);
    KeepRun(50);

    // unreachable
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CANCEL_ALL_0200
 * @tc.name     test pthread_cancel with PTHREAD_CANCEL_ASYNCHRONOUS
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCancelAsynchronous, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCancelAsynchronous, nullptr), 0) << "> return errno";
    Msleep(10);
    EXPECT_EQ(pthread_cancel(tid), 0);
    Msleep(100);
    EXPECT_EQ(pthread_join(tid, NULL), 0);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}

void *ThreadPthreadCancelDeferred(void *arg)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);
    CheckStep(2);
    KeepRun(50);
    CheckStep(3);
    Msleep(100);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CANCEL_ALL_0300
 * @tc.name     test pthread_cancel with PTHREAD_CANCEL_DEFERRED
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCancelDeferred, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);

    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCancelDeferred, nullptr), 0) << "> return errno";
    Msleep(10);
    EXPECT_EQ(pthread_cancel(tid), 0);
    Msleep(100);
    EXPECT_EQ(pthread_join(tid, NULL), 0);
    EXPECT_EQ(CheckStep(4), (uint64_t)0x1234);
}

void *ThreadPthreadCancelEnable(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    CheckStep(2);
    Msleep(50);
    pthread_testcancel();
    // unreachable
    CheckStep(3);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_CANCEL_ALL_0400
 * @tc.name     test pthread_cancel with PTHREAD_CANCEL_ENABLE
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadCancelEnable, Function | MediumTest | Level3)
{
    pthread_t tid;
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid, nullptr, ThreadPthreadCancelEnable, nullptr), 0) << "> return errno";
    Msleep(10);
    EXPECT_EQ(pthread_cancel(tid), 0);
    Msleep(100);
    EXPECT_EQ(pthread_join(tid, NULL), 0);
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}

void PreparePthreadAtfork(void)
{
    CheckStep(2);
}

void ParentPthreadAtfork(void)
{
    Msleep(20);
    CheckStep(5);
}

void ChildPthreadAtfork(void)
{
    CheckStep(3);
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATFORK_0100
 * @tc.name     Basic test about pthread_atfork whit one thread
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadAtfork, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        CheckStep(1);
        if (pthread_atfork(PreparePthreadAtfork, ParentPthreadAtfork, ChildPthreadAtfork) != 0) {
            exit(1);
        }

        pid_t pid = fork();
        if (pid < 0){
            exit(1);
        } else if (pid == 0) {
            if (CheckStep(4) != 0x1234) {
                exit(1);
            }
            exit(0);
        }
        if (CheckStep(6) != 0x123456) {
            exit(1);
        }

        // check child-child exit
        int exitCode;
        int procStat = CheckProcStatus(pid, &exitCode, 0);
        if (procStat != 1) {
            exit(1);
        }
        if (exitCode != 0) {
            LOG("> target process should exited 0");
            exit(1);
        }
        exit(0);
    }
    WaitProcExitedOK(pid);
}

void PrepareNPthreadAtfork(void)
{
    CheckStep(2);
}

void ParentNPthreadAtfork(void)
{
    Msleep(20);
    CheckStep(5);
}

void ChildNPthreadAtfork(void)
{
    CheckStep(3);
}

void *ThreadNPthreadAtfork(void *arg)
{
    CheckStep(1);
    if (pthread_atfork(PrepareNPthreadAtfork, ParentNPthreadAtfork, ChildNPthreadAtfork) != 0) {
        exit(1);
    }
    pid_t pid = fork();
    if (pid < 0) {
        LOG("> fork errno = %d", errno);
        exit(1);
    } else if (pid == 0) {
        if (CheckStep(4) != 0x1234) {
            exit(1);
        }
        exit(0);
    }

    if (CheckStep(6) != 0x123456) {
        exit(1);
    }
    // check child-child exit
    int exitCode;
    int procStat = CheckProcStatus(pid, &exitCode, 0);
    if (procStat != 1) {
        exit(1);
    }
    if (exitCode != 0) {
        LOG("> target process should exited 0");
        exit(1);
    }
    exit(0);
    return arg;
}

/**
 * @tc.number   SUB_KERNEL_PTHREAD_ATFORK_0200
 * @tc.name     Basic test about pthread_atfork whit two thread
 * @tc.desc     [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testNPthreadAtfork, Function | MediumTest | Level3)
{
    pid_t pid = fork();
    ASSERT_TRUE(pid >= 0) << "> fork errno = " << errno;
    if (pid == 0) {
        pthread_t tid;
        if (pthread_create(&tid, nullptr, ThreadNPthreadAtfork, nullptr) != 0) {
            exit(1);
        }
        if (pthread_join(tid, nullptr) != 0) {
            exit(1);
        }
        exit(0);
    }
    WaitProcExitedOK(pid);
}

void FunOnce(void)
{
    CheckStep(2);
}

void *ThreadOnce(void *arg)
{
    pthread_once_t *once = (pthread_once_t*)arg;
    EXPECT_EQ(pthread_once(once, FunOnce), 0);
    return arg;
}

/**
 * @tc.number     SUB_KERNEL_PTHREAD_ONCE_0100
 * @tc.name       pthread_once basic test
 * @tc.desc       [C- SOFTWARE -0200]
 */
HWTEST_F(PthreadTest, testPthreadOnce, Function | MediumTest | Level3)
{
    pthread_once_t once = PTHREAD_ONCE_INIT;
    pthread_t tid[2];
    CheckStep(1);
    ASSERT_EQ(pthread_create(&tid[0], nullptr, ThreadOnce, (void*)&once), 0) << "> return errno";
    ASSERT_EQ(pthread_create(&tid[1], nullptr, ThreadOnce, (void*)&once), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[0], nullptr), 0) << "> return errno";
    EXPECT_EQ(pthread_join(tid[1], nullptr), 0) << "> return errno";
    EXPECT_EQ(CheckStep(3), (uint64_t)0x123);
}
