/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform/include/list.h"
#include <stdlib.h>
#include "platform/include/dl_list.h"
#include "platform/include/platform_def.h"

typedef struct ListNode {
    DL_LIST node;
    List *list;  // Used in func ListGetNextNode, might de deleted in future version.
    void *data;
} ListNodeInternal;

typedef struct List {
    DL_LIST dlList;
    int len;
    FreeDataCb cb;
} ListInternal;

static ListNode *ListGetFirstNodeInt(const List *list)
{
    ASSERT(list);
    if (list->len == 0) {
        return NULL;
    }

    return DL_LIST_ENTRY(list->dlList.pstNext, ListNode, node);
}

List *ListCreate(FreeDataCb cb)
{
    List *newList = (List *)calloc(1, (sizeof(List)));
    if (newList != NULL) {
        DL_ListInit(&newList->dlList);
        newList->cb = cb;
    }
    return newList;
}

NO_SANITIZE("cfi") static ListNode *ListFreeNode(List *list, ListNode *node)
{
    ASSERT(list);
    ASSERT(node);
    ListNode *next = ListGetNextNode(node);
    DL_ListDelete(&node->node);
    if (list->cb) {
        list->cb(node->data);
    }
    free(node);
    list->len--;

    return next;
}

void ListClear(List *list)
{
    ASSERT(list);
    if (list->len > 0) {
        for (ListNode *node = ListGetFirstNode(list); node;) {
            node = ListFreeNode(list, node);
        }
    }
    list->len = 0;
}

void ListDelete(List *list)
{
    if (list == NULL) {
        return;
    }

    ListClear(list);
    free(list);
}

int32_t ListGetSize(const List *list)
{
    ASSERT(list);
    return list->len;
}

void ListAddFirst(List *list, void *data)
{
    ASSERT(list);
    ASSERT(data);
    ListNode *node = (ListNode *)calloc(1, sizeof(ListNode));
    if (node != NULL) {
        node->data = data;
        node->list = list;
        DL_ListHeadInsert(&list->dlList, &node->node);
        list->len++;
    }
}

void ListAddLast(List *list, void *data)
{
    ASSERT(list);
    ASSERT(data);
    ListNode *node = (ListNode *)calloc(1, sizeof(ListNode));
    if (node != NULL) {
        node->data = data;
        node->list = list;
        DL_ListTailInsert(&list->dlList, &node->node);
        list->len++;
    }
}

void *ListForEachData(const List *list, const ListCmpFunc cmp, void *parameter)
{
    ASSERT(list);
    ASSERT(cmp);
    if (list->len == 0) {
        return NULL;
    }

    for (ListNode *node = ListGetFirstNodeInt(list); node;) {
        ListNode *next = ListGetNextNode(node);
        if (cmp(node->data, parameter)) {
            return node->data;
        }
        node = next;
    }
    return NULL;
}

ListNode *ListGetFirstNode(const List *list)
{
    ASSERT(list);
    if (list->len == 0) {
        return NULL;
    }

    return DL_LIST_ENTRY(list->dlList.pstNext, ListNode, node);
}

ListNode *ListGetLastNode(const List *list)
{
    ASSERT(list);
    if (list->len == 0) {
        return NULL;
    }
    return DL_LIST_ENTRY(list->dlList.pstPrev, ListNode, node);
}

ListNode *ListGetNextNode(const ListNode *listNode)
{
    ASSERT(listNode);
    if (listNode == ListGetLastNode(listNode->list)) {
        return NULL;
    }
    return DL_LIST_ENTRY(listNode->node.pstNext, ListNode, node);
}

void *ListGetNodeData(const ListNode *node)
{
    ASSERT(node);
    return node->data;
}

bool ListRemoveNode(List *list, void *data)
{
    ASSERT(list);
    ASSERT(data);
    if (list->len == 0) {
        return false;
    }
    for (ListNode *node = ListGetFirstNode(list); node; node = ListGetNextNode(node)) {
        if (node->data == data) {
            ListFreeNode(list, node);
            return true;
        }
    }
    return false;
}

bool ListRemoveFirst(List *list)
{
    ASSERT(list);
    if (list->len == 0) {
        return false;
    }

    ListNode *first = ListGetFirstNode(list);
    if (!ListRemoveNode(list, first->data)) {
        return false;
    }
    return true;
}

bool ListRemoveLast(List *list)
{
    ASSERT(list);
    if (list->len == 0) {
        return false;
    }

    ListNode *last = ListGetLastNode(list);
    if (!ListRemoveNode(list, last->data)) {
        return false;
    }
    return true;
}

bool ListIsEmpty(const List* list)
{
    ASSERT(list);
    return (list->len == 0);
}
