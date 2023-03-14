/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_ordered_list.h"

void HdfOrderedListInit(struct HdfOrderedList *list)
{
    OsalMutexInit(&list->mutex);
    OsalSemInit(&list->sem);
    HdfSListInit(&list->head);
}

void HdfOrderedListDestroy(struct HdfOrderedList *list)
{
    HdfSListInit(&list->head);
    OsalSemDestroy(&list->sem);
    OsalMutexDestroy(&list->mutex);
}

int HdfOrderedListIsEmpty(struct HdfOrderedList *list)
{
    return HdfSListIsEmpty(&list->head);
}

void HdfOrderedListOffer(struct HdfOrderedList *list, struct HdfOrderedListEntity *newEntity)
{
    struct HdfSListIterator it;
    struct HdfOrderedListEntity *matchEntity;
    if ((list == NULL) || (newEntity == NULL)) {
        return;
    }
    OsalMutexLock(&list->mutex);
    if (HdfSListIsEmpty(&list->head)) {
        HdfSListAdd(&list->head, &newEntity->node);
        goto FINISHED;
    }
    HdfSListIteratorInit(&it, &list->head);
    while (HdfSListIteratorHasNext(&it)) {
        matchEntity = (struct HdfOrderedListEntity *)HdfSListIteratorNext(&it);
        if (newEntity->key > matchEntity->key) {
            HdfSListIteratorInsert(&it, &newEntity->node);
            break;
        }
    }
FINISHED:
    OsalMutexUnlock(&list->mutex);
    OsalSemPost(&list->sem);
}

struct HdfOrderedListEntity *HdfOrderedListGet(struct HdfOrderedList *list)
{
    struct HdfOrderedListEntity *entity;
    OsalMutexLock(&list->mutex);
    entity = (struct HdfOrderedListEntity *)HdfSListPeek(&list->head);
    OsalMutexUnlock(&list->mutex);
    return entity;
}

struct HdfOrderedListEntity *HdfOrderedListTake(struct HdfOrderedList *list)
{
    struct HdfOrderedListEntity *entity = HdfOrderedListGet(list);
    while (entity == NULL) {
        OsalSemWait(&list->sem, OSAL_WAIT_FOREVER);
        entity = HdfOrderedListGet(list);
    }
    return entity;
}

long HdfOrderedListPeekKey(struct HdfOrderedList *list)
{
    long orderedKey = 0;
    struct HdfOrderedListEntity *firstEntity;
    OsalMutexLock(&list->mutex);
    firstEntity = (struct HdfOrderedListEntity *)HdfSListPeek(&list->head);
    if (firstEntity != NULL) {
        orderedKey = firstEntity->key;
    }
    OsalMutexUnlock(&list->mutex);
    return orderedKey;
}

struct HdfOrderedListEntity *HdfOrderedListFetch(
    struct HdfOrderedList *list, long matchKey, HdfOrderedListComparer comparer)
{
    struct HdfSListIterator it;
    struct HdfOrderedListEntity *matchEntity = NULL;
    if (comparer == NULL) {
        return matchEntity;
    }
    OsalMutexLock(&list->mutex);
    HdfSListIteratorInit(&it, &list->head);
    while (HdfSListIteratorHasNext(&it)) {
        struct OrderedListEntity *searchEntity =
                (struct OrderedListEntity *)HdfSListIteratorNext(&it);
        if (comparer(matchKey, searchEntity)) {
            HdfSListIteratorRemove(&it);
            matchEntity = searchEntity;
            break;
        }
    }
    OsalMutexUnlock(&list->mutex);
    return matchEntity;
}

void HdfOrderedListFlush(struct HdfOrderedList *list, HdfOrderedListEntityDeleter deleter)
{
    OsalMutexLock(&list->mutex);
    HdfSListFlush(&list->head, (HdfSListDeleter)deleter);
    OsalMutexUnlock(&list->mutex);
    OsalSemPost(&list->sem);
}

int HdfOrderedListWait(struct HdfOrderedList *list, long timeout)
{
    OsalSemWait(&list->sem, timeout);
}
