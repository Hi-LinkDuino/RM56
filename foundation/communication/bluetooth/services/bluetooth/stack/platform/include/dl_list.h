/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef DL_LIST_H
#define DL_LIST_H

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Double linked list.
typedef struct DL_LIST {
    struct DL_LIST *pstPrev;  // previous node
    struct DL_LIST *pstNext;  // next node
} DL_LIST;

/**
 * @brief Perform instantiation of the DL_LIST.
 *
 * @param list DL_LIST pointer.
 * @since 6
 */
static inline void DL_ListInit(DL_LIST *list)
{
    list->pstNext = list;
    list->pstPrev = list;
}

/**
 * @brief Insert a DL_LIST node into DL_LIST.
 *
 * @param list DL_LIST pointer.
 * @param node DL_LIST node pointer.
 * @since 6
 */
static inline void DL_ListAdd(DL_LIST *list, DL_LIST *node)
{
    node->pstNext = list->pstNext;
    node->pstPrev = list;
    list->pstNext->pstPrev = node;
    list->pstNext = node;
}

/**
 * @brief Insert a DL_LIST node into DL_LIST tail.
 *
 * @param list DL_LIST pointer.
 * @param node DL_LIST node pointer.
 * @since 6
 */
static inline void DL_ListTailInsert(DL_LIST *list, DL_LIST *node)
{
    DL_ListAdd(list->pstPrev, node);
}

/**
 * @brief Insert a DL_LIST node into DL_LIST head.
 *
 * @param list DL_LIST pointer.
 * @param node DL_LIST node pointer.
 * @since 6
 */
static inline void DL_ListHeadInsert(DL_LIST *list, DL_LIST *node)
{
    DL_ListAdd(list, node);
}

/**
 * @brief Delete a DL_LIST node from DL_LIST.
 *
 * @param node DL_LIST node pointer.
 * @since 6
 */
static inline void DL_ListDelete(DL_LIST *node)
{
    node->pstNext->pstPrev = node->pstPrev;
    node->pstPrev->pstNext = node->pstNext;
    node->pstNext = NULL;
    node->pstPrev = NULL;
}

/**
 * @brief Whether a specific DL_LIST is empty.
 *
 * @param node DL_LIST node pointer.
 * @return DL_List is empty return true, else return false.
 * @since 6
 */
static inline bool DL_ListEmpty(DL_LIST *list)
{
    return (bool)(list->pstNext == list);
}

/**
 * @brief Member's offset relative to specific struct.
 *
 * @param type Struct name.
 * @param member Member name.
 * @return Offset value type is unsigned long.
 * @since 6
 */
#define OFF_SET_OF(type, member) ((unsigned long int)&((type *)0)->member)

/**
 * @brief Get pointer of Given type struct, which includes DL_LIST member.
 *
 * @param item DL_LIST address.
 * @param type Struct name.
 * @param member Member name.
 * @return Pointer of Given type struct.
 * @since 6
 */
#define DL_LIST_ENTRY(item, type, member) ((type *)(void *)((char *)(item)-OFF_SET_OF(type, member)))

#ifdef __cplusplus
}
#endif

#endif  // DL_LIST_H