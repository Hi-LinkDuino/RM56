/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include "hdf_slist.h"
#include "osal_sem.h"
#include "osal_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HdfBlockingQueue {
    struct HdfSList list;
    struct OsalSem sem;
    struct OsalMutex mutex;
};

struct HdfSListEntry {
    struct HdfSListNode node;
    void *data;
};

void HdfBlockingQueue_Init(struct HdfBlockingQueue *queue);

void HdfBlockingQueueDestroy(struct HdfBlockingQueue *queue);

void HdfBlockingQueueFlush(struct HdfBlockingQueue *queue);

void *HdfBlockingQueueTake(struct HdfBlockingQueue *queue);

void *HdfBlockingQueueGet(struct HdfBlockingQueue *queue);

void HdfBlockingQueueRemove(struct HdfBlockingQueue *queue, void *data);

void *HdfBlockingQueuePoll(struct HdfBlockingQueue *queue, long timeout);

void HdfBlockingQueueOffer(struct HdfBlockingQueue *queue, void *val);

void *HdfBlockingQueueFind(struct HdfBlockingQueue *queue, long match_key, SlList_Comparer comparer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BLOCKING_QUEUE_H */
