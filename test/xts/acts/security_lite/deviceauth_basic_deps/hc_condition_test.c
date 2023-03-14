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

#include "hc_condition_test.h"

#include <hctest.h>
#include <parameter.h>
#include <pthread.h>
#include <securec.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "print_log.h"
#include "test_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    sem_t *sem;
    int32_t semState;
    int32_t condState;
} ThreadTestArgs;

#define REQUIRED_SEM_DEFAULT_COUNT 20
#define REQUIRED_SEM_COUNT 20
#define REQUIRED_ATTR_DEFAULT_COUNT 20
#define REQUIRED_ATTR_COUNT 20

/* sem_wait thread test */
static void *ThreadSemWait(void *args)
{
    LOGI("into ThreadSemWait");
    int res;
    ThreadTestArgs *semArgs = (ThreadTestArgs *)args;
    TEST_ASSERT_NOT_NULL(semArgs->sem);
    TEST_ASSERT_NOT_EQUAL(0, semArgs->semState);
    RUN_AND_PRINT_ELAPSED_TIME(res, sem_wait((sem_t *)semArgs->sem));
    // after sem_post
    LOGI("sem_wait result = %d", res);
    TEST_ASSERT_EQUAL(0, res);
    semArgs->semState = 0;
    return args;
}

/* sem_post thread test */
static void *ThreadSemPost(void *args)
{
    LOGI("into ThreadSemPost");
    int res;
    ThreadTestArgs *semArgs = (ThreadTestArgs *)args;
    TEST_ASSERT_NOT_NULL(semArgs->sem);
    RUN_AND_PRINT_ELAPSED_TIME(res, sem_post((sem_t *)semArgs->sem));
    LOGI("sem_post result = %d", res);
    TEST_ASSERT_EQUAL(0, res);
    return args;
}

/* pthread_cond_wait thread test */
static void *ThreadCondWait(void *args)
{
    LOGI("into ThreadCondWait");
    int res;
    ThreadTestArgs *condArgs = (ThreadTestArgs *)args;
    TEST_ASSERT_NOT_NULL(condArgs->cond);
    TEST_ASSERT_NOT_NULL(condArgs->mutex);
    TEST_ASSERT_NOT_EQUAL(0, condArgs->condState);
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_wait(condArgs->cond, condArgs->mutex));
    // after pthread_cond_signal
    LOGI("pthread_cond_wait result = %d", res);
    TEST_ASSERT_EQUAL(0, res);
    condArgs->condState = 0;
    return args;
}

/* pthread_cond_signal thread test */
static void *ThreadCondSignal(void *args)
{
    LOGI("into ThreadCondSignal");
    int res;
    ThreadTestArgs *testArgs = (ThreadTestArgs *)args;
    TEST_ASSERT_NOT_NULL(testArgs->cond);
    TEST_ASSERT_NOT_NULL(testArgs->mutex);
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_signal(testArgs->cond));
    LOGI("pthread_cond_signal result = %d", res);
    return args;
}

/* sem_init and sem_destroy test */
static void TestSemInit(int count)
{
    int res;
    sem_t sem[count];
    res = memset_s(sem, sizeof(sem), 0, sizeof(sem));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_init(&sem[i], 0, 0));
        LOGI("sem_init result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_destroy(&sem[i]));
        LOGI("sem_destroy result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
}

/* sem_wait test */
static void TestSemWait(int count)
{
    int res;
    pthread_attr_t attr;
    pthread_t threads[count];

    res = memset_s(threads, sizeof(threads), 0, sizeof(threads));
    TEST_ASSERT_EQUAL(0, res);

    sem_t sem = {0};
    TEST_ASSERT_EQUAL(0, res);

    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_init(&attr));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_init(&sem, 0, 0));
        LOGI("sem_init result = %d", res);
        ThreadTestArgs args = {
            .sem = &sem,
            .semState = -1,
        };
        TEST_ASSERT_EQUAL(0, res);
        /* create phread to wait */
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_create(&threads[i], &attr, ThreadSemWait, &args));
        LOGI("create sem_wait thread result = %d", res);
        int sleepSeconds = rand() % (WAIT_IN_SEM_SECONDS_MAX - WAIT_IN_SEM_SECONDS_MIN) + WAIT_IN_SEM_SECONDS_MIN;
        sleep(sleepSeconds);
        /* sleep seconds then create thread to post */
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_create(&threads[i], &attr, ThreadSemPost, &args));
        LOGI("create sem_post thread result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
        sleep(sleepSeconds);
        TEST_ASSERT_EQUAL(0, args.semState);
    }
}

/* sem_post test */
static void TestSemPost(int count)
{
    int res;
    sem_t sem[count];
    res = memset_s(sem, sizeof(sem), 0, sizeof(sem));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_init(&sem[i], 0, 0));
        LOGI("sem_init int TestSemPost result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_post(&sem[i]));
        LOGI("sem_post result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_post(&sem[i]));
        LOGI("sem_post result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, sem_destroy(&sem[i]));
        LOGI("sem_destroy in TestSemPost result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
}

/* pthread_condattr_init and pthread_condattr_destroy test */
static void TestAttrConInit(int count)
{
    int res;
    pthread_condattr_t attr[count];
    res = memset_s(attr, sizeof(attr), 0, sizeof(attr));
    TEST_ASSERT_EQUAL(0, res);
    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_condattr_init(&attr[i]));
        LOGI("pthread_condattr_init result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_condattr_destroy(&attr[i]));
        LOGI("pthread_condattr_destroy result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
}

/* pthread_cond_init and pthread_cond_destroy test */
static void TestConInit(int count)
{
    int res;
    pthread_condattr_t attr = {0};
    pthread_cond_t cond[count];
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_condattr_init(&attr));
    LOGI("pthread_condattr_init in TestConInit result = %d", res);
    TEST_ASSERT_EQUAL(0, res);
    res = memset_s(cond, sizeof(cond), 0, sizeof(cond));
    TEST_ASSERT_EQUAL(0, res);

    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_init(&cond[i], &attr));
        LOGI("pthread_cond_init result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_destroy(&cond[i]));
        LOGI("pthread_cond_destroy result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
}

/* pthread_cond_wait and pthread_cond_signal test */
static void TestConWait(int count)
{
    int res;
    pthread_attr_t pthreadAttr;
    pthread_condattr_t attr = {0};
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    pthread_t threads[count];
    res = memset_s(&mutex, sizeof(mutex), 0, sizeof(mutex));
    TEST_ASSERT_EQUAL(0, res);
    res = memset_s(threads, sizeof(threads), 0, sizeof(threads));
    TEST_ASSERT_EQUAL(0, res);
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_attr_init(&pthreadAttr));
    TEST_ASSERT_EQUAL(0, res);
    // init attr and cond and mutex
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_condattr_init(&attr));
    LOGI("pthread_condattr_init in TestConWait result = %d", res);
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_init(&cond, &attr));
    LOGI("pthread_cond_init in TestConWait result = %d", res);

    for (int i = 0; i < count; ++i) {
        ThreadTestArgs args = {
            .cond = &cond,
            .mutex = &mutex,
            .condState = -1,
        };
        /* create phread to wait */
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_create(&threads[i], &pthreadAttr, ThreadCondWait, &args));
        LOGI("create pthread_cond_wait thread result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
        int sleepSeconds = rand() % (WAIT_IN_SEM_SECONDS_MAX - WAIT_IN_SEM_SECONDS_MIN) + WAIT_IN_SEM_SECONDS_MIN;
        sleep(sleepSeconds);
        /* sleep seconds then create thread to send signal */
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_create(&threads[i], &pthreadAttr, ThreadCondSignal, &args));
        LOGI("create pthread_cond_signal thread result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
        sleep(sleepSeconds);
        TEST_ASSERT_EQUAL(0, args.condState);
    }
}

/* pthread_cond_signal test */
static void TestConSignal(int count)
{
    int res;
    pthread_condattr_t attr = {0};
    pthread_cond_t cond;
    RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_init(&cond, &attr));
    LOGI("pthread_cond_init in TestConWait result = %d", res);
    TEST_ASSERT_EQUAL(0, res);
    for (int i = 0; i < count; ++i) {
        RUN_AND_PRINT_ELAPSED_TIME(res, pthread_cond_signal(&cond));
        LOGI("pthread_cond_signal result = %d", res);
        TEST_ASSERT_EQUAL(0, res);
    }
}

/* test semaphore with sem series */
void TestHcSemCondition(void)
{
    TestSemInit(REQUIRED_SEM_DEFAULT_COUNT);
    TestSemPost(REQUIRED_SEM_DEFAULT_COUNT);
    TestSemWait(REQUIRED_SEM_DEFAULT_COUNT);
}

/* test semaphore with phread cond */
void TestHcPthreadCondition(void)
{
    TestConWait(REQUIRED_ATTR_DEFAULT_COUNT);
    TestAttrConInit(REQUIRED_ATTR_DEFAULT_COUNT);
    TestConInit(REQUIRED_ATTR_DEFAULT_COUNT);
    TestConSignal(REQUIRED_ATTR_DEFAULT_COUNT);
}

#ifdef __cplusplus
}
#endif
