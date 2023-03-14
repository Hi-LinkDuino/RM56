/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_slist.h"
#include "hdf_log.h"

#define HDF_LOG_TAG utils_slist

void HdfSListInit(struct HdfSList *list)
{
    if (list != NULL) {
        list->root = NULL;
    }
}

bool HdfSListIsEmpty(struct HdfSList *list)
{
    return ((list == NULL) || (list->root == NULL));
}

struct HdfSListNode *HdfSListSearch(struct HdfSList *list, uint32_t keyValue, HdfSListSearchComparer comparer)
{
    struct HdfSListIterator it;
    if (comparer == NULL) {
        return NULL;
    }
    HdfSListIteratorInit(&it, list);
    while (HdfSListIteratorHasNext(&it)) {
        struct HdfSListNode *listNode = HdfSListIteratorNext(&it);
        if (comparer(listNode, keyValue))  {
            return listNode;
        }
    }
    return NULL;
}

struct HdfSListNode *HdfSListGetLast(struct HdfSList *list)
{
    struct HdfSListNode *last = NULL;
    struct HdfSListNode *iterator = NULL;
    if (list == NULL) {
        return NULL;
    }

    for (iterator = list->root; iterator != NULL; iterator = iterator->next) {
        last = iterator;
    }
    return last;
}

void HdfSListAdd(struct HdfSList *list, struct HdfSListNode *link)
{
    if (list == NULL || link == NULL) {
        return;
    }
    link->next = list->root;
    list->root = link;
}

void HdfSListAddTail(struct HdfSList *list, struct HdfSListNode *link)
{
    struct HdfSListNode *iterator = NULL;
    if (list == NULL || link == NULL) {
        return;
    }

    for (iterator = (struct HdfSListNode *)list; iterator->next; iterator = iterator->next) {
        if (iterator->next == link) {
            return;
        }
    }

    link->next = NULL;
    iterator->next = link;
}

bool HdfSListAddOrder(struct HdfSList *list, struct HdfSListNode *link, HdfSListAddComparer comparer)
{
    struct HdfSListNode *iterator = NULL;
    if (list == NULL || link == NULL || comparer == NULL) {
        HDF_LOGE("input is invalid");
        return false;
    }
    for (iterator = list->root; iterator; iterator = iterator->next) {
        if (iterator == link) {
            HDF_LOGE("link is already in list");
            return false;
        }
    }
    if (comparer(link, list->root)) {
        link->next = list->root;
        list->root = link;
        return true;
    }
    for (iterator = list->root; iterator && iterator->next; iterator = iterator->next) {
        if (comparer(iterator, link) && comparer(link, iterator->next)) {
            link->next = iterator->next;
            iterator->next = link;
            return true;
        }
    }
    if ((list->root == NULL) || (iterator == NULL)) {
        link->next = NULL;
        list->root = link;
    } else {
        link->next = NULL;
        iterator->next = link;
    }
    return true;
}

void HdfSListRemove(struct HdfSList *list, struct HdfSListNode *link)
{
    struct HdfSListNode *iterator = NULL;
    if (list == NULL || link == NULL) {
        return;
    }

    for (iterator = (struct HdfSListNode *)list; iterator; iterator = iterator->next) {
        if (iterator->next == link) {
            iterator->next = link->next;
            return;
        }
    }
}

void HdfSListFlush(struct HdfSList *list, HdfSListDeleter deleter)
{
    struct HdfSListIterator iterator;
    if (list == NULL) {
        return;
    }

    HdfSListIteratorInit(&iterator, list);
    while (HdfSListIteratorHasNext(&iterator)) {
        void *listNode = (void *)HdfSListIteratorNext(&iterator);
        HdfSListIteratorRemove(&iterator);
        if (deleter != NULL) {
            deleter(listNode);
        }
    }
}

int HdfSListCount(struct HdfSList *list)
{
    struct HdfSListNode *iterator = NULL;
    int counter = 0;
    if (list == NULL) {
        return counter;
    }

    for (iterator = (struct HdfSListNode *)list; iterator->next; iterator = iterator->next) {
        counter++;
    }
    return counter;
}

struct HdfSListNode *HdfSListPeek(struct HdfSList *list)
{
    if (list == NULL) {
        return NULL;
    }

    return list->root;
}

struct HdfSListNode *HdfSListNext(struct HdfSListNode *link)
{
    if (link == NULL) {
        return NULL;
    }

    return link->next;
}

struct HdfSListNode *HdfSListPop(struct HdfSList *list)
{
    struct HdfSListNode *first = NULL;
    if (list == NULL || list->root == NULL) {
        return NULL;
    }
    first = list->root;
    list->root = first->next;
    return first;
}

void HdfSListIteratorInit(struct HdfSListIterator *iterator, struct HdfSList *list)
{
    if (iterator == NULL || list == NULL) {
        if (iterator != NULL) {
            iterator->stepOnNext = 0;
            iterator->prev = NULL;
            iterator->curr = NULL;
        }
        return;
    }
    iterator->stepOnNext = 0;
    iterator->prev = (struct HdfSListNode *)list;
    iterator->curr = list->root;
}

bool HdfSListIteratorHasNext(struct HdfSListIterator *iterator)
{
    if ((iterator == NULL) || (iterator->curr == NULL) || (iterator->prev == NULL)) {
        return false;
    }

    if (!iterator->stepOnNext) {
        return iterator->curr != NULL;
    }

    if (iterator->prev->next != iterator->curr) {
        // current item has been removed
        return iterator->prev->next != NULL;
    }

    // current items has not been removed
    return iterator->curr->next != NULL;
}

struct HdfSListNode *HdfSListIteratorNext(struct HdfSListIterator *iterator)
{
    if (iterator == NULL || iterator->curr == NULL || iterator->prev == NULL) {
        return NULL;
    }

    if (iterator->stepOnNext) {
        if (iterator->prev->next == iterator->curr) {
            iterator->prev = iterator->curr;
            iterator->curr = iterator->curr->next;
        } else {
            // curr was removed from the list, set it again but don't advance prev
            iterator->curr = iterator->prev->next;
        }
    } else {
        iterator->stepOnNext = 1;
    }

    return iterator->curr;
}

void HdfSListIteratorRemove(struct HdfSListIterator *iterator)
{
    if (iterator == NULL || iterator->curr == NULL || iterator->prev == NULL) {
        return;
    }

    iterator->curr = iterator->curr->next;
    iterator->prev->next = iterator->curr;
    iterator->stepOnNext = 0;
}

void HdfSListIteratorInsert(struct HdfSListIterator *iterator, struct HdfSListNode *link)
{
    if (iterator == NULL || link == NULL) {
        return;
    }

    link->next = iterator->curr;
    iterator->prev->next = link;
    iterator->prev = link;
}

