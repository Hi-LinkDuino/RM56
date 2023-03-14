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

#ifndef HKS_DOUBLE_LIST_H
#define HKS_DOUBLE_LIST_H

struct DoubleList {
    struct DoubleList *prev;
    struct DoubleList *next;
};

#ifdef __cplusplus
extern "C" {
#endif

void InitializeDoubleList(struct DoubleList *node);

void AddNodeAfterDoubleListHead(struct DoubleList *head, struct DoubleList *node);

void AddNodeAtDoubleListTail(struct DoubleList *head, struct DoubleList *node);

void RemoveDoubleListNode(struct DoubleList *node);

#ifdef __cplusplus
}
#endif

/*
 * HKS_DLIST_ITER - iterate over double list of struct st, double list should be the first member of struct st
 * @st:        the struct in the double list
 * @head:      the head for double list.
 */
#define HKS_DLIST_ITER(st, head) \
    struct DoubleList *p = NULL; \
    for (p = (head)->next, (st) = (__typeof__(st))p; p != (head); p = p->next, (st) = (__typeof__(st))p)

#endif
