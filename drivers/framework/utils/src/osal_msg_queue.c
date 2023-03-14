/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_msg_queue.h"
#include "osal_message.h"
#include "osal_time.h"

void OsalMessageQueueInit(struct HdfMessageQueue *queue)
{
    if (queue != NULL) {
        OsalMutexInit(&queue->mutex);
        OsalSemInit(&queue->semaphore, 0);
        HdfSListInit(&queue->list);
    }
}

void OsalMessageQueueDestroy(struct HdfMessageQueue *queue)
{
    if (queue != NULL) {
        OsalMutexDestroy(&queue->mutex);
        OsalSemDestroy(&queue->semaphore);
        HdfSListFlush(&queue->list, HdfMessageDelete);
    }
}

void HdfMessageQueueEnqueue(
    struct HdfMessageQueue *queue, struct HdfMessage *message, long delayed)
{
    if (queue == NULL || message == NULL) {
        return;
    }

    struct HdfSListIterator it;
    (void)delayed;
    message->timeStamp += OsalGetSysTimeMs();
    OsalMutexLock(&queue->mutex);
    HdfSListIteratorInit(&it, &queue->list);
    while (HdfSListIteratorHasNext(&it)) {
        struct HdfMessage *next = (struct HdfMessage *)HdfSListIteratorNext(&it);
        if (next->timeStamp > message->timeStamp) {
            HdfSListIteratorInsert(&it, &message->entry);
            goto COMPLETE;
        }
    }

    HdfSListAddTail(&queue->list, &message->entry);
COMPLETE:
    OsalMutexUnlock(&queue->mutex);
    OsalSemPost(&queue->semaphore);
}

struct HdfMessage* HdfMessageQueueNext(struct HdfMessageQueue *queue)
{
    struct HdfSListIterator it;
    struct HdfMessage *message = NULL;
    uint64_t currentTime = OsalGetSysTimeMs();

    OsalMutexLock(&queue->mutex);
    HdfSListIteratorInit(&it, &queue->list);
    while (HdfSListIteratorHasNext(&it)) {
        message = (struct HdfMessage *)HdfSListIteratorNext(&it);
        if (message->timeStamp <= currentTime) {
            HdfSListIteratorRemove(&it);
            OsalMutexUnlock(&queue->mutex);
            return message;
        }
    }

    OsalMutexUnlock(&queue->mutex);
    OsalSemWait(&queue->semaphore, OSAL_WAIT_FOREVER);
    return NULL;
}

void HdfMessageQueueFlush(struct HdfMessageQueue *queue)
{
    struct HdfSListIterator it;
    OsalMutexLock(&queue->mutex);
    HdfSListIteratorInit(&it, &queue->list);
    while (HdfSListIteratorHasNext(&it)) {
        struct HdfMessage *msgNode = (struct HdfMessage *)HdfSListIteratorNext(&it);
        HdfSListIteratorRemove(&it);
        HdfMessageRecycle(msgNode);
    }
    OsalMutexUnlock(&queue->mutex);
}

