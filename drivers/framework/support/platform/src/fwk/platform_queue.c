/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "platform_queue.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_mutex.h"
#include "osal_thread.h"
#include "platform_log.h"

#define PLAT_QUEUE_THREAD_STAK 20000

static void PlatformQueueDoDestroy(struct PlatformQueue *queue)
{
    (void)OsalThreadDestroy(&queue->thread);
    (void)OsalSemDestroy(&queue->sem);
    (void)OsalSpinDestroy(&queue->spin);
    OsalMemFree(queue);
}

static int32_t PlatformQueueThreadWorker(void *data)
{
    int32_t ret;
    struct PlatformQueue *queue = (struct PlatformQueue *)data;
    struct PlatformMsg *msg = NULL;

    while (true) {
        /* wait envent */
        ret = OsalSemWait(&queue->sem, HDF_WAIT_FOREVER);
        if (ret != HDF_SUCCESS) {
            continue;
        }

        if (!queue->start) {
            PlatformQueueDoDestroy(queue);
            break;
        }

        (void)OsalSpinLock(&queue->spin);
        if (DListIsEmpty(&queue->msgs)) {
            msg = NULL;
        } else {
            msg = DLIST_FIRST_ENTRY(&queue->msgs, struct PlatformMsg, node);
            DListRemove(&msg->node);
        }
        (void)OsalSpinUnlock(&queue->spin);
        /* message process */
        if (msg != NULL) {
            (void)(queue->handle(queue, msg));
        }
    }
    return HDF_SUCCESS;
}

struct PlatformQueue *PlatformQueueCreate(PlatformMsgHandle handle, const char *name, void *data)
{
    int32_t ret;
    struct PlatformQueue *queue = NULL;

    if (handle == NULL) {
        return NULL;
    }

    queue = (struct PlatformQueue *)OsalMemCalloc(sizeof(*queue));
    if (queue == NULL) {
        PLAT_LOGE("PlatformQueueCreate: alloc queue fail!");
        return NULL;
    }

    ret = OsalThreadCreate(&queue->thread, (OsalThreadEntry)PlatformQueueThreadWorker, (void *)queue);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("PlatformQueueCreate: create thread fail!");
        OsalMemFree(queue);
        return NULL;
    }

    (void)OsalSpinInit(&queue->spin);
    (void)OsalSemInit(&queue->sem, 0);
    DListHeadInit(&queue->msgs);
    queue->name = (name == NULL) ? "PlatformWorkerThread" : name;
    queue->handle = handle;
    queue->data = data;
    queue->start = false;
    return queue;
}

int32_t PlatformQueueStart(struct PlatformQueue *queue)
{
    int32_t ret;
    struct OsalThreadParam cfg;

    if (queue == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    cfg.name = (char *)queue->name;
    cfg.priority = OSAL_THREAD_PRI_HIGHEST;
    cfg.stackSize = PLAT_QUEUE_THREAD_STAK;
    ret = OsalThreadStart(&queue->thread, &cfg);
    if (ret != HDF_SUCCESS) {
        PLAT_LOGE("PlatformQueueStart: start thread fail:%d", ret);
        return ret;
    }
    queue->start = true;

    return HDF_SUCCESS;
}

void PlatformQueueDestroy(struct PlatformQueue *queue)
{
    if (queue == NULL) {
        return;
    }

    if (queue->start) {
        queue->start = false;
        (void)OsalSemPost(&queue->sem);
    } else {
        PlatformQueueDoDestroy(queue);
    }
}

int32_t PlatformQueueAddMsg(struct PlatformQueue *queue, struct PlatformMsg *msg)
{
    if (queue == NULL || msg == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    DListHeadInit(&msg->node);
    msg->error = HDF_SUCCESS;
    (void)OsalSpinLock(&queue->spin);
    DListInsertTail(&msg->node, &queue->msgs);
    (void)OsalSpinUnlock(&queue->spin);
    /* notify the worker thread */
    (void)OsalSemPost(&queue->sem);
    return HDF_SUCCESS;
}
