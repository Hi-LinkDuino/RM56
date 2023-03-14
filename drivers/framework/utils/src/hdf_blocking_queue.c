/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "blocking_queue.h"

void HdfBlockingQueueInit(struct HdfBlockingQueue *queue)
{
    HdfSListInit(&queue->list);
    OsalSemInit(&queue->sem);
    OsalMutexInit(&queue->mutex);
}

void HdfBlockingQueueDestroy(struct HdfBlockingQueue *queue)
{
    HdfSListInit(&queue->list);
    OsalSemDestroy(&queue->sem);
    OsalMutexDestroy(&queue->mutex);
}

void HdfBlockingQueueFlush(struct HdfBlockingQueue *queue)
{
    OsalMutexLock(&queue->mutex);
    HdfSListFlush(&queue->list, HdfSListRemove);
    OsalMutexUnlock(&queue->mutex);
    OsalSemPost(&queue->sem);
}

void *HdfBlockingQueueTake(struct HdfBlockingQueue *queue)
{
    void *data = HdfBlockingQueueGet(queue);
    if (data == NULL) {
        OsalSemWait(&queue->sem, OSAL_WAIT_FOREVER);
        data = HdfBlockingQueueGet(queue);
    }
    return data;
}

void *HdfBlockingQueueGet(struct HdfBlockingQueue *queue)
{
    void *data = NULL;
    struct HdfSListEntry *entry;
    OsalMutexLock(&queue->mutex);
    entry = (struct HdfSListEntry *)HdfSListPeek(&queue->list);
    OsalMutexUnlock(&queue->mutex);
    if (entry != NULL) {
        data = entry->data;
        HdfSListEntryFree(entry);
    }
    return data;
}


void *HdfBlockingQueueFind(struct HdfBlockingQueue *queue, long matchKey, SlList_Comparer comparer)
{
    void *matchData = NULL;
    struct HdfSListIterator it;
    struct HdfSListEntry *entry = NULL;
    if (comparer == NULL) {
        return NULL;
    }
    OsalMutexLock(&queue->mutex);
    HdfSListIteratorInit(&it, &queue->list);
    while (HdfSListIteratorHasNext(&it)) {
        entry = (struct HdfSListEntry *) HdfSListIteratorNext(&it);
        if (comparer(matchKey, entry->data)) {
            matchData = entry->data;
            break;
        }
    }
    OsalMutexUnlock(&queue->mutex);
    return matchData;
}

void HdfBlockingQueueRemove(struct HdfBlockingQueue *queue, void *data)
{
    bool targetListChanged = false;
    struct HdfSListIterator it;
    struct HdfSListEntry *entry = NULL;
    OsalMutexLock(&queue->mutex);
    HdfSListIteratorInit(&it, &queue->list);
    while (HdfSListIteratorHasNext(&it)) {
        entry = (struct HdfSListEntry *)HdfSListIteratorNext(&it);
        if (entry->data == data) {
            HdfSListIteratorRemove(&it);
            HdfSListEntryFree(entry);
            targetListChanged = true;
            break;
        }
    }
    OsalMutexUnlock(&queue->mutex);
    if (targetListChanged) {
        OsalSemPost(&queue->sem);
    }
}

void *HdfBlockingQueuePoll(struct HdfBlockingQueue *queue, long timeout)
{
    void *data = HdfBlockingQueueGet(queue);
    if (data == NULL) {
        OsalSemWait(&queue->sem, timeout);
        data = HdfBlockingQueueGet(queue);
    }
    return data;
}

int HdfBlockingQueueOffer(struct HdfBlockingQueue *queue, void *val, long timeout)
{
    struct HdfSListEntry *entry = NULL;
    if (OsalSemWait(&queue->sem, timeout) != 0) {
        return -1;
    }
    entry = HdfSListEntryNew(val);
    if (entry != NULL) {
        OsalMutexLock(&queue->mutex);
        HdfSListAddTail(&queue->list, &entry->node);
        OsalMutexUnlock(&queue->mutex);
    }
    OsalSemPost(&queue->sem);
}

