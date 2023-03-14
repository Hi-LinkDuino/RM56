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

#include "btm_thread.h"

#include <stddef.h>

#include "btstack.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/queue.h"
#include "platform/include/semaphore.h"
#include "platform/include/thread.h"

typedef struct {
    void (*task)(void *context);
    void *context;
} BtmTask;

typedef struct {
    uint8_t id;
    Queue *queue;
    ReactorItem *reactorItem;
} BtmProcessingQueue;

static Thread *g_processingThread = NULL;
static List *g_processingQueueList = NULL;
static Mutex *g_processingQueueLock = NULL;

static BtmTask *AllocTask(void (*task)(void *context), void *context)
{
    BtmTask *block = MEM_MALLOC.alloc(sizeof(BtmTask));
    if (block != NULL) {
        block->task = task;
        block->context = context;
    }
    return block;
}

static void FreeTask(void *task)
{
    MEM_MALLOC.free(task);
}

NO_SANITIZE("cfi") static void RunTask(void *context)
{
    BtmTask *task = QueueTryDequeue((Queue *)context);
    if (task != NULL) {
        task->task(task->context);

        FreeTask(task);
    }
}

static BtmProcessingQueue *AllocProcessingQueue(uint8_t id, uint32_t size)
{
    BtmProcessingQueue *block = MEM_MALLOC.alloc(sizeof(BtmProcessingQueue));
    if (block != NULL) {
        block->id = id;
        block->queue = QueueCreate(size);
        if (block->queue != NULL) {
            Reactor *reactor = ThreadGetReactor(g_processingThread);
            block->reactorItem = ReactorRegister(reactor, QueueGetDequeueFd(block->queue), block->queue, RunTask, NULL);
        }
    }
    return block;
}

typedef struct {
    Queue *queue;
    Semaphore *semaphore;
} RunAllTaskContext;

static void RunAllTaskInQueueTask(void *param)
{
    RunAllTaskContext *context = (RunAllTaskContext *)param;

    BtmTask *task = QueueTryDequeue(context->queue);
    while (task != NULL) {
        task->task(task->context);

        FreeTask(task);

        task = QueueTryDequeue(context->queue);
    }

    if (context->semaphore != NULL) {
        SemaphorePost(context->semaphore);
    }
}

static void RunAllTaskInQueue(Queue *queue)
{
    RunAllTaskContext context = {
        .queue = queue,
        .semaphore = NULL,
    };

    if (ThreadIsSelf(g_processingThread) == 0) {
        RunAllTaskInQueueTask(&context);
    } else {
        context.semaphore = SemaphoreCreate(0);
        ThreadPostTask(g_processingThread, RunAllTaskInQueueTask, &context);
        SemaphoreWait(context.semaphore);
        SemaphoreDelete(context.semaphore);
    }
}

static void FreeProcessingQueue(void *queue)
{
    BtmProcessingQueue *block = (BtmProcessingQueue *)queue;
    if (block->reactorItem != NULL) {
        ReactorUnregister(block->reactorItem);
        block->reactorItem = NULL;
    }
    if (block->queue != NULL) {
        QueueDelete(block->queue, FreeTask);
        block->queue = NULL;
    }
    MEM_MALLOC.free(queue);
}

static BtmProcessingQueue *FindProcessingQueueById(uint8_t queueId)
{
    BtmProcessingQueue *queue = NULL;

    ListNode *node = NULL;
    node = ListGetFirstNode(g_processingQueueList);
    while (node != NULL) {
        queue = ListGetNodeData(node);
        if (queue->id == queueId) {
            break;
        } else {
            queue = NULL;
        }
        node = ListGetNextNode(node);
    }

    return queue;
}

void BtmInitThread()
{
    g_processingThread = ThreadCreate("Stack");
    g_processingQueueList = ListCreate(FreeProcessingQueue);
    g_processingQueueLock = MutexCreate();
}

void BtmCloseThread()
{
    if (g_processingQueueList != NULL) {
        ListDelete(g_processingQueueList);
        g_processingQueueList = NULL;
    }

    if (g_processingThread != NULL) {
        ThreadDelete(g_processingThread);
        g_processingThread = NULL;
    }

    if (g_processingQueueLock != NULL) {
        MutexDelete(g_processingQueueLock);
        g_processingQueueLock = NULL;
    }
}

Thread *BTM_GetProcessingThread()
{
    return g_processingThread;
}

int BTM_CreateProcessingQueue(uint8_t queueId, uint16_t size)
{
    int result = BT_NO_ERROR;
    MutexLock(g_processingQueueLock);

    BtmProcessingQueue *queue = FindProcessingQueueById(queueId);
    if (queue != NULL) {
        result = BT_BAD_STATUS;
    } else {
        queue = AllocProcessingQueue(queueId, size);
        ListAddLast(g_processingQueueList, queue);
    }

    MutexUnlock(g_processingQueueLock);
    return result;
}

int BTM_DeleteProcessingQueue(uint8_t queueId)
{
    int result = BT_NO_ERROR;

    Queue *taskQueue = NULL;

    MutexLock(g_processingQueueLock);
    BtmProcessingQueue *queue = FindProcessingQueueById(queueId);
    if (queue != NULL) {
        taskQueue = queue->queue;
        queue->queue = NULL;
        ListRemoveNode(g_processingQueueList, queue);
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_processingQueueLock);

    if (taskQueue != NULL) {
        RunAllTaskInQueue(taskQueue);
        QueueDelete(taskQueue, FreeTask);
        taskQueue = NULL;
    }

    return result;
}

int BTM_RunTaskInProcessingQueue(uint8_t queueId, void (*task)(void *context), void *context)
{
    int result = BT_NO_ERROR;
    MutexLock(g_processingQueueLock);
    BtmProcessingQueue *queue = FindProcessingQueueById(queueId);
    if (queue != NULL) {
        BtmTask *block = AllocTask(task, context);
        if (task != NULL) {
            QueueEnqueue(queue->queue, block);
        } else {
            result = BT_NO_MEMORY;
        }
    } else {
        result = BT_BAD_STATUS;
    }
    MutexUnlock(g_processingQueueLock);
    return result;
}
