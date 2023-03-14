/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hks_double_list.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

void InitializeDoubleList(struct DoubleList *node)
{
    if (node == NULL) {
        return;
    }

    node->prev = node;
    node->next = node;
}

void AddNodeAfterDoubleListHead(struct DoubleList *head, struct DoubleList *node)
{
    if ((head == NULL) || (node == NULL)) {
        return;
    }

    if (head->next == NULL) {
        head->next = head;
    }

    head->next->prev = node;
    node->next = head->next;
    node->prev = head;
    head->next = node;
}

void AddNodeAtDoubleListTail(struct DoubleList *head, struct DoubleList *node)
{
    if ((head == NULL) || (node == NULL)) {
        return;
    }

    if (head->prev == NULL) {
        head->prev = head;
    }

    head->prev->next = node;
    node->next = head;
    node->prev = head->prev;
    head->prev = node;
}

void RemoveDoubleListNode(struct DoubleList *node)
{
    if (node == NULL) {
        return;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    if (node->prev != NULL) {
        node->prev->next = node->next;
    }

    node->prev = NULL;
    node->next = NULL;
}

