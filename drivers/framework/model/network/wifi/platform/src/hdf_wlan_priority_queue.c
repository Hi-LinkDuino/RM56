/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#include "hdf_wlan_priority_queue.h"
#include "securec.h"
#include "osal/osal_sem.h"
#include "osal/osal_mem.h"
#include "utils/hdf_log.h"
#include "hdf_wlan_queue.h"

#define MAX_PRIORITY_LEVEL 8

#define HDF_LOG_TAG HDF_WIFI_CORE

typedef struct {
    PriorityQueue priorityQueue;
    uint8_t priorityLevelCount;
    OSAL_DECLARE_SEMAPHORE(messageSemaphore);
    HdfWlanQueue *queues[0];
} PriorityQueueImpl;

PriorityQueue *CreatePriorityQueue(uint16_t queueSize, uint8_t priorityLevelCount)
{
    uint8_t i;
    uint32_t queueMemSize;
    PriorityQueueImpl *priorityQueue = NULL;
    uint32_t ret = HDF_SUCCESS;
    HDF_STATUS status;
    if (priorityLevelCount > MAX_PRIORITY_LEVEL || priorityLevelCount == 0) {
        HDF_LOGE("%s:priorityLevelCount must in 1 to 8", __func__);
        return NULL;
    }
    queueMemSize = sizeof(PriorityQueueImpl) + (priorityLevelCount * sizeof(HdfWlanQueue *));
    priorityQueue = (PriorityQueueImpl *)OsalMemCalloc(queueMemSize);
    if (priorityQueue == NULL) {
        return NULL;
    }
    priorityQueue->priorityLevelCount = priorityLevelCount;
    for (i = 0; i < priorityLevelCount; i++) {
        priorityQueue->queues[i] = CreateQueue(queueSize);
        if (priorityQueue->queues[i] == NULL) {
            ret = HDF_FAILURE;
            HDF_LOGE("%s:Init message queue failed!QueueID=%d,ret=%d", __func__, i, ret);
            break;
        }
    }
    if (ret != HDF_SUCCESS) {
        DestroyPriorityQueue((PriorityQueue *)priorityQueue);
        return NULL;
    }
    status = OsalSemInit(&priorityQueue->messageSemaphore, 0);
    if (status != HDF_SUCCESS) {
        DestroyPriorityQueue((PriorityQueue *)priorityQueue);
        return NULL;
    }

    return (PriorityQueue *)priorityQueue;
}

void DestroyPriorityQueue(PriorityQueue *queue)
{
    uint8_t i;
    HDF_STATUS status;
    PriorityQueueImpl *queueImpl = (PriorityQueueImpl *)queue;
    if (queue == NULL) {
        return;
    }

    for (i = 0; i < queueImpl->priorityLevelCount; i++) {
        if (queueImpl->queues[i] == NULL) {
            continue;
        }
        DestroyQueue(queueImpl->queues[i]);
        queueImpl->queues[i] = NULL;
    }
    status = OsalSemDestroy(&queueImpl->messageSemaphore);
    if (status != HDF_SUCCESS) {
        HDF_LOGE("%s:Destroy message queue semaphore failed!status=%d", __func__, status);
    }

    OsalMemFree(queueImpl);
}

int32_t PushPriorityQueue(PriorityQueue *queue, const uint8_t priority, void *context)
{
    uint32_t ret;
    uint8_t pri;
    PriorityQueueImpl *queueImpl = NULL;
    if (queue == NULL || context == NULL) {
        return HDF_FAILURE;
    }
    queueImpl = (PriorityQueueImpl *)queue;
    pri = priority;
    if (priority >= queueImpl->priorityLevelCount) {
        pri = queueImpl->priorityLevelCount - 1;
    }

    ret = PushQueue(queueImpl->queues[pri], context);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:Write queue failed!ret=%d", __func__, ret);
        return ret;
    }

    (void)OsalSemPost(&queueImpl->messageSemaphore);
    return HDF_SUCCESS;
}

static void *PopQueueByPri(PriorityQueueImpl *queue)
{
    void *context = NULL;
    uint8_t i;
    for (i = 0; i < queue->priorityLevelCount; i++) {
        context = PopQueue(queue->queues[i]);
        if (context != NULL) {
            return context;
        }
    }
    return NULL;
}

void *PopPriorityQueue(PriorityQueue *queue, uint32_t waitInMS)
{
    PriorityQueueImpl *queueImpl = (PriorityQueueImpl *)queue;
    void *context = NULL;
    HDF_STATUS status;
    if (queue == NULL) {
        return NULL;
    }

    context = PopQueueByPri(queueImpl);
    if (context != NULL || waitInMS == 0) {
        return context;
    }

    status = OsalSemWait(&queueImpl->messageSemaphore, waitInMS);
    if (status != HDF_SUCCESS) {
        return NULL;
    }
    return PopQueueByPri(queueImpl);
}
