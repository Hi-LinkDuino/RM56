/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hc_thread_test.h"

#include <hctest.h>
#include <pthread.h>
#include <securec.h>
#include <stdlib.h>
#include <unistd.h>

#include "hc_thread_mutex_share.h"
#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REQUIRED_PTHREAD_ATTR_COUNT 20
#define TEST_STRING "hello"

typedef struct {
    void *(*function)(void *);
    size_t sz;
    int i;
    char *str;
} ThreadTestArgs;

static void *ThreadRountine(void *args)
{
    TEST_ASSERT_NOT_NULL(args);

    ThreadTestArgs *testArgs = (ThreadTestArgs *)args;
    int sleepSeconds = rand() % (WAIT_IN_MUTEX_SECONDS_MAX - WAIT_IN_MUTEX_SECONDS_MIN) + WAIT_IN_MUTEX_SECONDS_MIN;

    TEST_ASSERT_EQUAL((void *)ThreadRountine, (void *)testArgs->function);
    TEST_ASSERT_EQUAL((size_t)MAX_THREAD_STACK_SIZE, testArgs->sz);
    TEST_ASSERT_GREATER_OR_EQUAL(MIN_THREAD_STACK_SIZE, testArgs->i);
    TEST_ASSERT_LESS_OR_EQUAL(MAX_THREAD_STACK_SIZE, testArgs->i);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, testArgs->str);

    LOGI("test thread begin wait for %d seconds", sleepSeconds);
    sleep(sleepSeconds);
    LOGI("test thread wait for %d seconds successfully", sleepSeconds);

    LOGI("begin time consuming operation");
    unsigned long long res;
    RUN_AND_PRINT_ELAPSED_TIME(res, TimeConsumingOperation());
    LOGI("end time consuming operation, res = %llu", res);
    return args;
}

static int GetStackSize(void)
{
    // use macro instead of enum to avoid error: division by zero [-Werror=div-by-zero]
#if (MAX_THREAD_STACK_SIZE == MIN_THREAD_STACK_SIZE)
    return MIN_THREAD_STACK_SIZE;
#else
    int stackSize = rand() % (MAX_THREAD_STACK_SIZE - MIN_THREAD_STACK_SIZE) + MIN_THREAD_STACK_SIZE;
    LOGI("rand stack size = %d", stackSize);
    return stackSize;
#endif
}

static void TestSpecifiedCount(int count, int detachState)
{
    TEST_ASSERT_GREATER_THAN(0, count);
    TEST_ASSERT_LESS_OR_EQUAL(REQUIRED_THREAD_COUNT, count);
    TEST_ASSERT_TRUE_MESSAGE(detachState == PTHREAD_CREATE_JOINABLE || detachState == PTHREAD_CREATE_DETACHED,
                             "invalid detach state");

    pthread_attr_t attr;
    pthread_t threads[REQUIRED_THREAD_COUNT];
    int res, stackSize;
    static ThreadTestArgs args = {
        .function = ThreadRountine,
        .sz = MAX_THREAD_STACK_SIZE,
        .i = -1,
        .str = (char *)TEST_STRING,
    };

    res = memset_s(threads, sizeof(threads), 0, sizeof(threads));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_init(&attr));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_setdetachstate(&attr, detachState));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < count; ++i) {
        LOGI("test thread %d/%d", i, count);
        stackSize = GetStackSize();
        LOGI("rand stack size = %d", stackSize);
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_setstacksize(&attr, stackSize));
        TEST_ASSERT_EQUAL(0, res);

        args.i = stackSize;
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_create(&threads[i], &attr, ThreadRountine, &args));
        TEST_ASSERT_EQUAL(0, res);
    }

    if (detachState == PTHREAD_CREATE_JOINABLE) {
        for (int i = 0; i < count; ++i) {
            LOGI("test thread %d/%d", i, count);
            RUN_AND_PRINT_ELAPSED_TIME(res, pthread_join(threads[i], NULL));
            LOGI("pthread_join result = %d", res);
            TEST_ASSERT_EQUAL(0, res);
        }
    } else {
        LOGI("detachState is PTHREAD_CREATE_DETACHED, do not test pthread_join");
    }

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_destroy(&attr));
    TEST_ASSERT_EQUAL(0, res);
}

static void TestPthreadAttr(int detachState)
{
    TEST_ASSERT_TRUE_MESSAGE(detachState == PTHREAD_CREATE_JOINABLE || detachState == PTHREAD_CREATE_DETACHED,
                             "invalid detach state");

    int res, stackSize;
    pthread_attr_t attr[REQUIRED_PTHREAD_ATTR_COUNT];
    res = memset_s(attr, sizeof(attr), 0, sizeof(attr));
    TEST_ASSERT_EQUAL(0, res);
    for (int i = 0; i < REQUIRED_PTHREAD_ATTR_COUNT; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_init(&attr[i]));
        TEST_ASSERT_EQUAL(0, res);
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_setdetachstate(&attr[i], detachState));
        TEST_ASSERT_EQUAL(0, res);
        stackSize = GetStackSize();
        LOGI("rand stack size = %d", stackSize);
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_setstacksize(&attr[i], stackSize));
        TEST_ASSERT_EQUAL(0, res);
    }

    for (int i = 0; i < REQUIRED_PTHREAD_ATTR_COUNT; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_destroy(&attr[i]));
        TEST_ASSERT_EQUAL(0, res);
    }
}

static void TestState(int detachState)
{
    TEST_ASSERT_TRUE_MESSAGE(detachState == PTHREAD_CREATE_JOINABLE || detachState == PTHREAD_CREATE_DETACHED,
                             "invalid detach state");

    TestPthreadAttr(detachState);
    LOGI("test one thread");
    TestSpecifiedCount(1, detachState);

    LOGI("test %d threads", REQUIRED_THREAD_COUNT);
    TestSpecifiedCount(REQUIRED_THREAD_COUNT, detachState);
}

void TestHcThread(void)
{
#if TEST_PTHREAD_CREATE_DETACHED
    LOGI("test state PTHREAD_CREATE_DETACHED");
    TestState(PTHREAD_CREATE_DETACHED);
#else // TEST_PTHREAD_CREATE_DETACHED
    LOGI("test state PTHREAD_CREATE_JOINABLE");
    TestState(PTHREAD_CREATE_JOINABLE);
#endif // TEST_PTHREAD_CREATE_DETACHED
}

#ifdef __cplusplus
}
#endif
