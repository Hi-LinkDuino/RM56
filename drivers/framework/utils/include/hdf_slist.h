/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_SINGLE_LIST_H
#define HDF_SINGLE_LIST_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfSListNode {
    struct HdfSListNode *next; // next element in list, or NULL
};

struct HdfSList {
    struct HdfSListNode *root;
};

struct HdfSListIterator {
    int stepOnNext;
    struct HdfSListNode *prev; // points to the item before the current one
    struct HdfSListNode *curr; // points to the current item (to detect item removal)
};

typedef void (*HdfSListDeleter)(struct HdfSListNode *);

typedef bool (*HdfSListSearchComparer)(struct HdfSListNode *, uint32_t);

typedef bool (*HdfSListAddComparer)(struct HdfSListNode *, struct HdfSListNode *);

/*
 * @brief Init the list
 *
 * @param[in] list the operation list.
 *
 * @return None
 */
void HdfSListInit(struct HdfSList *list);

/*
 * @brief search to see whether specific element is attach into the list.
 *
 * @param[in] list the operation list.
 * @param[in] searchKey search key of list node.
 * @param[in] comparer comparer of list node.
 *
 * @return struct HdfSListNode of search result
 *  -# NULL if not found.
 */
struct HdfSListNode *HdfSListSearch(struct HdfSList *list, uint32_t searchKey, HdfSListSearchComparer comparer);

/*
 * @brief tests if list is empty
 *
 * @param[in] list the operation list.
 *
 * @return bool result of queue status.
 */
bool HdfSListIsEmpty(struct HdfSList *list);

/*
 * @brief find the last item in the list
 *
 * @param[in] list the operation list.
 *
 * @return last link of list.
 */
struct HdfSListNode *HdfSListGetLast(struct HdfSList *list);

/*
 * @brief add item to the head of the list
 *
 * @param[in] list the operation list.
 * @param[in] link the new element to add.
 *
 * @return None
 */
void HdfSListAdd(struct HdfSList *list, struct HdfSListNode *link);

/*
 * @brief add item to list as last element
 *
 * @param[in] list the operation list.
 * @param[in] link the new element to add.
 *
 * @return None
 */
void HdfSListAddTail(struct HdfSList *list, struct HdfSListNode *link);

/*
 * @brief add item to list as ordered
 *
 * @param[in] list the operation list.
 * @param[in] link the new element to add.
 * @param[in] comparer comparer of list node.
 *
 * @return bool result of queue status.
 */
bool HdfSListAddOrder(struct HdfSList *list, struct HdfSListNode *link, HdfSListAddComparer comparer);

/*
 * @brief remove item from list
 *
 * @param[in] list the operation list.
 * @param[in] link the element to remove.
 *
 */
void HdfSListRemove(struct HdfSList *list, struct HdfSListNode *link);

/*
 * @brief flush the list and free memory
 *
 * @param[in] list the operation list.
 * @param[in] deleter the function of free memory.
 *
 * @return None
 */
void HdfSListFlush(struct HdfSList *list, HdfSListDeleter deleter);

/*
 * @brief calculate the element count in the list.
 *
 * @param[in] list the list to count.
 *
 * @return the count of list.
 */
int HdfSListCount(struct HdfSList *list);

/*
 * @brief get first element
 *
 * @param[in] list the operation list.
 *
 * @return the first element in the list.
 */
struct HdfSListNode *HdfSListPeek(struct HdfSList *list);

/*
 * @brief get next element of link;
 *
 * @param[in] link the operation link.
 *
 * @return the next element of the link pass in
 */
struct HdfSListNode *HdfSListNext(struct HdfSListNode *link);

/*
 * @brief get and remove first element
 *
 * @param[in] list the operation list.
 *
 * @return the first element in the list.
 */
struct HdfSListNode *HdfSListPop(struct HdfSList *list);

/*
 * @brief initialize iterator
 *
 * @param[in] iterator the point of iterator.
 * @param[in] list the point of operation list.
 *
 * @return None
 */
void HdfSListIteratorInit(struct HdfSListIterator *iterator, struct HdfSList *list);

/*
 * @brief check whether list has next node.
 *
 * @param[in] iterator the point of iterator.
 *
 * @return the result of check next.
 */
bool HdfSListIteratorHasNext(struct HdfSListIterator *iterator);

/*
 * @brief get next link in the list and move iterator to next.
 *
 * @param[in] iterator the point of iterator.
 *
 * @return point to next element of it.
 */
struct HdfSListNode *HdfSListIteratorNext(struct HdfSListIterator *iterator);

/*
 * @brief remove current node that iterator point to and move iterator to next node
 *
 * @param[in] iterator the point of iterator.
 *
 * @return None
 */
void HdfSListIteratorRemove(struct HdfSListIterator *iterator);

/*
 * @brief insert new node before the node that iterator point to.and hold current iterator.
 *
 * @param[in] iterator the point of iterator.
 * @param[in] link the point of operation list.
 *
 * @return None
 */
void HdfSListIteratorInsert(struct HdfSListIterator *iterator, struct HdfSListNode *link);

#define OFFSET_OF(type, mem) ((size_t) &((type *)0)->mem)

#define HDF_SLIST_CONTAINER_OF(mtype, ptr, type, mem) ( { \
        const mtype*__mptr = (ptr);                       \
        (type *)((char *)__mptr - OFFSET_OF(type, mem));  \
})

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_SINGLE_LIST_H */
