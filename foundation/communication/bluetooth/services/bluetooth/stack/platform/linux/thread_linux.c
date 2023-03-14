/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "platform/include/thread.h"
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include "platform/include/mutex.h"
#include "platform/include/platform_def.h"
#include "platform/include/queue.h"
#include "platform/include/reactor.h"
#include "platform/include/semaphore.h"
#include "securec.h"

#define THREAD_QUEUE_SIZE 128
const char THREAD_DEFAULT_NAME[THREAD_NAME_SIZE] = "bt-stack";

typedef struct Thread {
    pid_t tid;
    bool isStopped;
    pthread_t pthread;
    Reactor *reactor;
    Queue *taskQueue;
    Mutex *apiMutex;
} ThreadInternal;

typedef struct {
    Thread *thread;
    Semaphore *sync;
    char name[THREAD_NAME_SIZE + 1];
} StartPromise;

typedef struct {
    void (*func)(void *context);
    void *context;
} TaskItem;

static void ReadyToRead(void *queue)
{
    ASSERT(queue);
    TaskItem *item = (TaskItem *)QueueDequeue((Queue *)queue);
    if (item == NULL) {
        LOG_ERROR("Thread: Queue Dequeue failed.");
        return;
    }
    item->func(item->context);
    free(item);
}

static void *ThreadStartFunc(void *promise)
{
    StartPromise *startPromise = (StartPromise *)promise;
    Thread *thread = startPromise->thread;

    thread->tid = (long int)syscall(__NR_gettid);
    prctl(PR_SET_NAME, startPromise->name);
    ReactorSetThreadId(thread->reactor, (unsigned long)pthread_self());

    SemaphorePost(startPromise->sync);

    int fd = QueueGetDequeueFd(thread->taskQueue);
    ReactorItem *reactorItem = ReactorRegister(thread->reactor, fd, (void *)thread->taskQueue, ReadyToRead, NULL);

    // Start Running reactor.
    if (ReactorStart(thread->reactor) != 0) {
        LOG_ERROR("ThreadStartFunc: Reactor run failed.");
    }
    ReactorUnregister(reactorItem);

    int num = 0;
    // Execute all remain tasks in queue after stop Reactor.
    TaskItem *task = (TaskItem *)QueueTryDequeue(thread->taskQueue);
    while (num <= THREAD_QUEUE_SIZE && task) {
        task->func(task->context);
        free(task);
        task = (TaskItem *)QueueTryDequeue(thread->taskQueue);
        num++;
    }

    return NULL;
}

int32_t ThreadIsSelf(const Thread *thread)
{
    ASSERT(thread);
    if (pthread_equal(thread->pthread, pthread_self()) != 0) {
        return 0;
    } else {
        return -1;
    }
}

static void ThreadStop(Thread *thread)
{
    MutexLock(thread->apiMutex);
    if (ThreadIsSelf((const Thread *)thread) == 0) {
        LOG_ERROR("ThreadStop: Cannot stop thread by itself.");
    }
    if (!thread->isStopped) {
        ReactorStop(thread->reactor);
        pthread_join(thread->pthread, NULL);
        thread->isStopped = true;
    }
    MutexUnlock(thread->apiMutex);
}

Thread *ThreadCreate(const char *name)
{
    Thread *thread = (Thread *)calloc(1, (sizeof(Thread)));
    if (thread == NULL) {
        return NULL;
    }

    thread->reactor = ReactorCreate();
    if (thread->reactor == NULL) {
        goto ERROR;
    }

    thread->taskQueue = QueueCreate(THREAD_QUEUE_SIZE);
    if (thread->taskQueue == NULL) {
        goto ERROR;
    }

    thread->apiMutex = MutexCreate();
    if (thread->apiMutex == NULL) {
        goto ERROR;
    }

    StartPromise *promise = (StartPromise *)calloc(1, sizeof(StartPromise));
    promise->thread = thread;
    promise->sync = SemaphoreCreate(0);
    if (promise->sync == NULL) {
        free(promise);
        goto ERROR;
    }

    if (name != NULL) {
        (void)strncpy_s(promise->name, THREAD_NAME_SIZE + 1, name, THREAD_NAME_SIZE);
    } else {
        (void)strncpy_s(promise->name, THREAD_NAME_SIZE + 1, THREAD_DEFAULT_NAME, THREAD_NAME_SIZE);
    }

    (void)pthread_create(&thread->pthread, NULL, ThreadStartFunc, promise);

    SemaphoreWait(promise->sync);
    SemaphoreDelete(promise->sync);
    free(promise);

    return thread;

ERROR:
    if (thread != NULL) {
        ReactorDelete(thread->reactor);
        QueueDelete(thread->taskQueue, free);
        MutexDelete(thread->apiMutex);
        free(thread);
    }
    return NULL;
}

void ThreadDelete(Thread *thread)
{
    if (thread == NULL) {
        return;
    }

    ThreadStop(thread);
    MutexDelete(thread->apiMutex);
    QueueDelete(thread->taskQueue, free);
    ReactorDelete(thread->reactor);

    free(thread);
}

void ThreadPostTask(Thread *thread, TaskFunc func, void *context)
{
    ASSERT(thread);
    ASSERT(func);

    TaskItem *task = (TaskItem *)malloc(sizeof(TaskItem));
    if (task == NULL) {
        return;
    }
    task->func = func;
    task->context = context;
    QueueEnqueue(thread->taskQueue, task);
}

Reactor *ThreadGetReactor(const Thread *thread)
{
    ASSERT(thread);
    return thread->reactor;
}
