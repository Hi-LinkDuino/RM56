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

#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stdbool.h>

#ifndef NO_SANITIZE
#ifdef __has_attribute
#if __has_attribute(no_sanitize)
#define NO_SANITIZE(type) __attribute__((no_sanitize(type)))
#endif
#endif
#endif

#ifndef NO_SANITIZE
#define NO_SANITIZE(type)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct List List;
typedef struct ListNode ListNode;
typedef void (*FreeDataCb)(void *data);
typedef bool (*ListCmpFunc)(void *nodeData, void *parameter);

/**
 * @brief Perform instantiation of the list.
 *
 * @param cb list free data callback.
 * @return List pointer.
 * @since 6
 */
List *ListCreate(FreeDataCb cb);

/**
 * @brief Clear all nodes in list.
 *
 * @param list List pointer.
 * @since 6
 */
void ListClear(List *list);

/**
 * @brief Delete the given list.
 *
 * @param list List pointer.
 * @since 6
 */
void ListDelete(List *list);

/**
 * @brief Return the length of the list.
 *
 * @param list List pointer.
 * @return List size.
 * @since 6
 */
int32_t ListGetSize(const List *list);

/**
 * @brief Append node to the beginning of the list with the given data.
 *
 * @param list List pointer.
 * @param data Node data.
 * @since 6
 */
void ListAddFirst(List *list, void *data);

/**
 * @brief Append node to the end of the list with the given data.
 *
 * @param list List pointer.
 * @param data Node data.
 * @since 6
 */
void ListAddLast(List *list, void *data);

/**
 * @brief Execute compare-function for each node in the list, until compare-function return true.
 *
 * @param list List pointer.
 * @param cmp Compare function.
 * @param parameter Compare parameter.
 * @return If ListCmpFunc return true, return Corresponding node's data.
 *               Else if ListCmpFunc always return false all the time, return NULL.
 * @since 6
 */
void *ListForEachData(const List *list, const ListCmpFunc cmp, void *parameter);

/**
 * @brief Return the head node of the list. If the list is empty, return NULL.
 *
 * @param list List pointer.
 * @return ListNode pointer.
 * @since 6
 */
ListNode *ListGetFirstNode(const List *list);

/**
 * @brief Return the tail node of the list. If the list is empty, return NULL.
 *
 * @param list List pointer.
 * @return ListNode pointer.
 * @since 6
 */
ListNode *ListGetLastNode(const List *list);

/**
 * @brief Return the next node of listNode. If next node is null return NULL.
 *
 * @param[in] listNode ListNode pointer.
 * @return ListNode* Next listNode pointer.
 * @since 6
 */
ListNode *ListGetNextNode(const ListNode *listNode);

/**
 * @brief Return data stored in the given node.
 *
 * @param node ListNode pointer.
 * @return void* ListNode data.
 * @since 6
 */
void *ListGetNodeData(const ListNode *node);

/**
 * @brief Delete the node with the given data.
 *
 * @param list List pointer.
 * @param data Data pointer.
 * @return true: Success remove data from list. false: Failed remove data from list.
 * @since 6
 */
bool ListRemoveNode(List *list, void *data);

/**
 * @brief Delete the first node of the list.
 *
 * @param list List pointer.
 * @return true: Success remove list first node. false: Failed remove list first node.
 * @since 6
 */
bool ListRemoveFirst(List *list);

/**
 * @brief Delete the last node of the list.
 *
 * @param list List pointer.
 * @return true: Success remove list last node. false: Failed remove list last node.
 * @since 6
 */
bool ListRemoveLast(List *list);

/**
 * @brief Check if the list is empty.
 *
 * @param list List pointer.
 * @return true: The list is empty. false: The list is not empty.
 * @since 6
 */
bool ListIsEmpty(const List* list);

#ifdef __cplusplus
}
#endif

#endif  // LIST_H
