/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_ORDERED_LIST_H
#define HDF_ORDERED_LIST_H

#include "hdf_slist.h"
#include "osal_mutex.h"
#include "osal_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HdfOrderedList {
    struct HdfSList head;
    struct OsalSem sem;
    struct OsalMutex mutex;
};

struct HdfOrderedListEntity {
    struct HdfSListNode node;
    long key;
};

typedef void (*HdfOrderedListEntityDeleter)(struct HdfOrderedListEntity *);

typedef bool (*HdfOrderedListComparer)(long, void *);

void HdfOrderedListInit(struct HdfOrderedList *list);

void HdfOrderedListDestroy(struct HdfOrderedList *list);

int HdfOrderedListIsEmpty(struct HdfOrderedList *list);

void HdfOrderedListOffer(struct HdfOrderedList *list, struct HdfOrderedListEntity *newEntity);

struct HdfOrderedListEntity *HdfOrderedListTake(struct HdfOrderedList *list);

struct HdfOrderedListEntity *HdfOrderedListFetch(
    struct HdfOrderedList *list, long match_key, HdfOrderedListComparer comparer);

int HdfOrderedListWait(struct HdfOrderedList *list, long timeout);

long HdfOrderedListPeekKey(struct HdfOrderedList *list);

void HdfOrderedListFlush(struct HdfOrderedList *list, HdfOrderedListEntityDeleter deleter);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_ORDERED_LIST_H */
