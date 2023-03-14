/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "platform/include/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "platform/include/platform_def.h"
#include "platform/include/semaphore.h"

typedef struct Queue {
    uint32_t capacity;
    Mutex *mutex;
    Semaphore *enqueueSem;
    Semaphore *dequeueSem;
    List *list;
} QueueInternal;

Queue *QueueCreate(uint32_t capacity)
{
    if (capacity == 0) {
        LOG_WARN("[QueueCreate]queue capacity can't be 0 or less than 0");
        return NULL;
    }
    Queue *queue = (Queue *)calloc(1, (sizeof(Queue)));
    if (queue != NULL) {
        queue->capacity = capacity;

        queue->mutex = MutexCreate();
        if (queue->mutex == NULL) {
            goto ERROR;
        }
        queue->enqueueSem = SemaphoreCreate(capacity);
        if (queue->enqueueSem == NULL) {
            goto ERROR;
        }
        queue->dequeueSem = SemaphoreCreate(0);
        if (queue->dequeueSem == NULL) {
            goto ERROR;
        }
        queue->list = ListCreate(NULL);
        if (queue->list == NULL) {
            goto ERROR;          
        }
    }
    return queue;
ERROR:
    if (queue != NULL) {
        MutexDelete(queue->mutex);
        SemaphoreDelete(queue->enqueueSem);
        SemaphoreDelete(queue->dequeueSem);
        ListDelete(queue->list);
        free(queue);
    }
    return NULL;
}

void QueueDelete(Queue *queue, NodeDataFreeCb cb)
{
    if (queue == NULL) {
        return;
    }
    if (cb) {
        int i = 0;
        ListNode *node = ListGetFirstNode(queue->list);
        for (; i < ListGetSize(queue->list); i++) {
            cb(ListGetNodeData(node));
            node = ListGetNextNode(node);
        }
    }

    ListDelete(queue->list);
    SemaphoreDelete(queue->enqueueSem);
    SemaphoreDelete(queue->dequeueSem);
    MutexDelete(queue->mutex);
    free(queue);
}

void QueueEnqueue(Queue *queue, void *data)
{
    ASSERT(queue);
    ASSERT(data);
    SemaphoreWait(queue->enqueueSem);

    MutexLock(queue->mutex);
    ListAddLast(queue->list, data);
    MutexUnlock(queue->mutex);

    SemaphorePost(queue->dequeueSem);
}

bool QueueTryEnqueue(Queue *queue, void *data)
{
    ASSERT(queue);
    ASSERT(data);

    if (SemaphoreTryWait(queue->enqueueSem) != 0) {
        return false;
    }

    MutexLock(queue->mutex);
    ListAddLast(queue->list, data);
    MutexUnlock(queue->mutex);

    SemaphorePost(queue->dequeueSem);

    return true;
}

void *QueueDequeue(Queue *queue)
{
    ASSERT(queue);
    void *data = NULL;
    ListNode *listNode = NULL;
    SemaphoreWait(queue->dequeueSem);

    MutexLock(queue->mutex);
    listNode = ListGetFirstNode(queue->list);
    data = ListGetNodeData(listNode);
    ListRemoveFirst(queue->list);
    MutexUnlock(queue->mutex);

    SemaphorePost(queue->enqueueSem);

    return data;
}

void *QueueTryDequeue(Queue *queue)
{
    ASSERT(queue);
    ListNode *listNode = NULL;
    void *data = NULL;

    if (SemaphoreTryWait(queue->dequeueSem) != 0) {
        return NULL;
    }

    MutexLock(queue->mutex);
    listNode = ListGetFirstNode(queue->list);
    data = ListGetNodeData(listNode);
    ListRemoveFirst(queue->list);
    MutexUnlock(queue->mutex);

    SemaphorePost(queue->enqueueSem);

    return data;
}

int32_t QueueGetEnqueueFd(const Queue *queue)
{
    ASSERT(queue);
    return SemaphoreGetfd(queue->enqueueSem);
}

int32_t QueueGetDequeueFd(const Queue *queue)
{
    ASSERT(queue);
    return SemaphoreGetfd(queue->dequeueSem);
}

void QueueFlush(Queue* queue, NodeDataFreeCb cb)
{
    ASSERT(queue);

    while (!QueueIsEmpty(queue)) {
        void* data = QueueTryDequeue(queue);
        if (cb != NULL) {
            cb(data);
        }
    }
}

bool QueueIsEmpty(Queue* queue)
{
    ASSERT(queue);
    bool ret = false;
    MutexLock(queue->mutex);
    ret = ListIsEmpty(queue->list);
    MutexUnlock(queue->mutex);
    return ret;
}

int32_t QueueGetSize(Queue *queue)
{
    ASSERT(queue);
    int32_t ret = 0;
    MutexLock(queue->mutex);
    ret = ListGetSize(queue->list);
    MutexUnlock(queue->mutex);
    return ret;
}
