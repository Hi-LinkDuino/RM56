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

#include "hc_mutex_test.h"

#include <hctest.h>
#include <pthread.h>
#include <securec.h>
#include <unistd.h>

#include "hc_thread_mutex_share.h"
#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REQUIRED_MUTEX_COUNT 20

static void TestSpecifiedCount(int count)
{
    pthread_mutex_t mutexes[REQUIRED_MUTEX_COUNT];
    int res;

    TEST_ASSERT_GREATER_THAN(0, count);
    TEST_ASSERT_LESS_OR_EQUAL(REQUIRED_MUTEX_COUNT, count);

    res = memset_s(mutexes, sizeof(mutexes), 0, sizeof(mutexes));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_init(&mutexes[i], NULL));
        TEST_ASSERT_EQUAL(0, res);
    }

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_lock(&mutexes[i]));
        TEST_ASSERT_EQUAL(0, res);
    }

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_unlock(&mutexes[i]));
        TEST_ASSERT_EQUAL(0, res);
    }

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_destroy(&mutexes[i]));
        TEST_ASSERT_EQUAL(0, res);
    }
}

static void *MutexThreadRoutine(void *args)
{
    pthread_mutex_t *mtx = (pthread_mutex_t *)args;
    TEST_ASSERT_NOT_NULL(mtx);

    unsigned long long timeConsumingResult;
    int res;
    int sleepSeconds = rand() % (WAIT_IN_MUTEX_SECONDS_MAX - WAIT_IN_MUTEX_SECONDS_MIN) + WAIT_IN_MUTEX_SECONDS_MIN;

    LOGI("in thread begin phase 1 work");
    RUN_AND_PRINT_ELAPSED_TIME(timeConsumingResult, TimeConsumingOperation());

    LOGI("in thread begin wait for mutex");
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_lock(mtx));
    TEST_ASSERT_EQUAL(0, res);
    LOGI("in thread wait for mutex succeed");

    LOGI("in thread begin wait for %d seconds", sleepSeconds);
    sleep(sleepSeconds);
    LOGI("in thread wait for %d seconds succeed", sleepSeconds);

    LOGI("in thread begin phase 2 work");
    RUN_AND_PRINT_ELAPSED_TIME(timeConsumingResult, TimeConsumingOperation());

    LOGI("in thread begin release mutex");
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_unlock(mtx));
    TEST_ASSERT_EQUAL(0, res);
    LOGI("in thread release mutex successfully");

    LOGI("in thread begin phase 3 work");
    RUN_AND_PRINT_ELAPSED_TIME(timeConsumingResult, TimeConsumingOperation());

    (void)(timeConsumingResult);
    LOGI("in thread all work done");
    return mtx;
}

static void TestMutexInThreads(void)
{
    pthread_mutex_t mtx;
    pthread_attr_t attr;
    int res;
    pthread_t threads[REQUIRED_THREAD_COUNT];
    res = memset_s(&mtx, sizeof(mtx), 0, sizeof(mtx));
    TEST_ASSERT_EQUAL(0, res);
    res = memset_s(&attr, sizeof(attr), 0, sizeof(attr));
    TEST_ASSERT_EQUAL(0, res);
    res = memset_s(&threads, sizeof(threads), 0, sizeof(threads));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_mutex_init(&mtx, NULL));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_init(&attr));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_setstacksize(&attr, MAX_THREAD_STACK_SIZE));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < REQUIRED_THREAD_COUNT; ++i) {
        LOGI("test threads %d/%d", i, REQUIRED_THREAD_COUNT);
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_create(&threads[i], &attr, MutexThreadRoutine, &mtx));
        TEST_ASSERT_EQUAL(0, res);
    }

    for (int i = 0; i < REQUIRED_THREAD_COUNT; ++i) {
        LOGI("test threads %d/%d", i, REQUIRED_THREAD_COUNT);
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_join(threads[i], NULL));
        LOGI("pthread_join result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_destroy(&attr));
    TEST_ASSERT_EQUAL(0, res);
}

void TestHcMutex(void)
{
    LOGI("test one mutex");
    TestSpecifiedCount(1);

    LOGI("test %d mutexes", REQUIRED_MUTEX_COUNT);
    TestSpecifiedCount(REQUIRED_MUTEX_COUNT);

    LOGI("test mutexes in threads");
    TestMutexInThreads();
}

#ifdef __cplusplus
}
#endif
