/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_task_queue.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

static int32_t HdfThreadTasker(void *data);

#define HDF_LOG_TAG hdf_task_queue

static int32_t HdfCreateThread(struct HdfTaskQueue *queue)
{
    int32_t ret;

    ret = OsalThreadCreate(&queue->thread, (OsalThreadEntry)HdfThreadTasker, queue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("HdfCreateThread: create thread fail!");
    }

    return ret;
}

struct HdfTaskQueue *HdfTaskQueueCreate(HdfTaskFunc func, const char *name)
{
    int32_t ret;
    struct HdfTaskQueue *queue = NULL;

    queue = (struct HdfTaskQueue *)OsalMemCalloc(sizeof(*queue));
    if (queue == NULL) {
        HDF_LOGE("%s malloc fail", __func__);
        return queue;
    }

    DListHeadInit(&queue->head);

    ret = OsalMutexInit(&queue->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalMutexInit fail", __func__);
        OsalMemFree(queue);
        return NULL;
    }

    ret = OsalSemInit(&queue->sem, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalSemInit fail", __func__);
        OsalMutexDestroy(&queue->mutex);
        OsalMemFree(queue);
        return NULL;
    }

    ret = HdfCreateThread(queue);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s HdfCreateThread fail", __func__);
        (void)OsalMutexDestroy(&queue->mutex);
        (void)OsalSemDestroy(&queue->sem);
        OsalMemFree(queue);
        return NULL;
    }

    queue->queueName = name;
    queue->threadRunFlag = false;

    if (func != NULL) {
        queue->queueFunc = func;
    }

    return queue;
}

static void hdfQueueStopThread(struct HdfTaskQueue *queue)
{
    int32_t ret;

    if (queue == NULL) {
        HDF_LOGE("%s queue ptr is null", __func__);
        return;
    }

    queue->threadRunFlag = false;

    ret = OsalSemPost(&queue->sem);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalSemPost fail", __func__);
    }
}

void HdfTaskQueueDestroy(struct HdfTaskQueue *queue)
{
    if (queue == NULL) {
        HDF_LOGE("%s queue ptr is null", __func__);
        return;
    }
    hdfQueueStopThread(queue);
}

static void hdfQueueStartThread(struct HdfTaskQueue *queue)
{
    int32_t ret;
    struct OsalThreadParam param;

    (void)memset_s(&param, sizeof(param), 0, sizeof(param));
    param.name = (char *)queue->queueName;
    param.priority = OSAL_THREAD_PRI_HIGH;
    queue->threadRunFlag = true;

    ret = OsalThreadStart(&queue->thread, &param);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalThreadStart fail", __func__);
    }
}

void HdfTaskEnqueue(struct HdfTaskQueue *queue, struct HdfTaskType *task)
{
    int32_t ret;

    if (queue == NULL || task == NULL) {
        HDF_LOGE("%s ptr is null", __func__);
        return;
    }

    ret = OsalMutexLock(&queue->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalMutexLock fail", __func__);
        return;
    }

    if (!queue->threadRunFlag) {
        hdfQueueStartThread(queue);
    }

    DListInsertTail(&task->node, &queue->head);

    ret = OsalMutexUnlock(&queue->mutex);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalMutexUnlock fail", __func__);
        return;
    }

    ret = OsalSemPost(&queue->sem);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s OsalSemPost fail", __func__);
    }
}

static struct HdfTaskType *HdfTaskDequeue(struct HdfTaskQueue *queue)
{
    struct HdfTaskType *task = NULL;

    if (!DListIsEmpty(&queue->head)) {
        task = DLIST_FIRST_ENTRY(&queue->head, struct HdfTaskType, node);
        DListRemove(&task->node);
    }

    return task;
}

static int32_t HdfThreadTasker(void *data)
{
    int32_t ret;
    struct HdfTaskType *task = NULL;
    struct HdfTaskQueue *queue = (struct HdfTaskQueue *)data;

    while (queue->threadRunFlag) {
        ret = OsalSemWait(&queue->sem, HDF_WAIT_FOREVER);
        if (ret != HDF_SUCCESS) {
            continue;
        }
        ret = OsalMutexLock(&queue->mutex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s OsalMutexLock fail", __func__);
            continue;
        }
        task = HdfTaskDequeue(queue);
        while (task != NULL) {
            if (task->func) {
                task->func(task);
            } else if (queue->queueFunc) {
                queue->queueFunc(task);
            } else {
                HDF_LOGE("%s no task and queue function", __func__);
            }
            task = HdfTaskDequeue(queue);
        }
        ret = OsalMutexUnlock(&queue->mutex);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s OsalMutexUnlock fail", __func__);
        }
    }

    (void)OsalMutexDestroy(&queue->mutex);
    (void)OsalSemDestroy(&queue->sem);
    OsalMemFree(queue);
    HDF_LOGI("%s thread exit", __func__);

    return HDF_SUCCESS;
}
