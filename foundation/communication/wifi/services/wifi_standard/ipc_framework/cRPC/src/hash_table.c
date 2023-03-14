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

#include "hash_table.h"
#include "log.h"

#undef LOG_TAG
#define LOG_TAG "WifiRpcHashTable"

const int START_PRIME_NUM = 3;
const int START_CALC_PRIME_NUM = 2;

static int CalcHash(const HashTable *p, int element)
{
    return element % (p->slots);
}

/* Search for the first prime number greater than or equal to the integer i */
static int CalcNextPrime(int i)
{
    if (i <= START_PRIME_NUM) {
        return START_PRIME_NUM;
    }
    while (1) {
        int j = START_CALC_PRIME_NUM;
        int k = i / START_CALC_PRIME_NUM;
        for (; j <= k; ++j) {
            if (i % j == 0) {
                break;
            }
        }
        if (j > k) {
            return i;
        } else {
            ++i;
        }
    }
}

HashTable *InitHashTable(int slots)
{
    HashTable *p = (HashTable *)calloc(1, sizeof(HashTable));
    if (p == NULL) {
        return NULL;
    }
    p->slots = CalcNextPrime(slots);
    p->list = (ListNode **)calloc(p->slots, sizeof(ListNode *));
    if (p->list == NULL) {
        free(p);
        return NULL;
    }
    return p;
}

void DestroyHashTable(HashTable *p)
{
    int i = 0;
    int nCount = p->slots;
    for (; i < nCount; ++i) {
        ListNode *t = p->list[i];
        while (t != NULL) {
            ListNode *next = t->next;
            ReleaseContext(t->context);
            free(t);
            t = next;
        }
    }
    free(p->list);
    free(p);
    p = NULL;
}

static int RebuildHashTable(HashTable *p)
{
    if (p == NULL) {
        return -1;
    }
    int slot = (p->slots << 1); /* double size */
    slot = CalcNextPrime(slot);
    if (slot <= 0) {
        return -1;
    }
    ListNode **new_list = (ListNode **)calloc(slot, sizeof(ListNode *));
    if (new_list == NULL) {
        LOGE("rebuild hash table calloc failed!");
        return -1;
    }
    int orgSlot = p->slots;
    p->slots = slot;
    for (int i = 0; i < orgSlot; ++i) {
        ListNode *t = p->list[i];
        while (t != NULL) {
            ListNode *q = t->next;
            int pos = CalcHash(p, t->context->fd);
            t->next = new_list[pos];
            new_list[pos] = t;
            t = q;
        }
    }
    free(p->list);
    p->list = new_list;
    return 0;
}

Context *FindContext(HashTable *p, int fd)
{
    if (p == NULL) {
        return NULL;
    }

    int slot = CalcHash(p, fd);
    ListNode *t = p->list[slot];
    while (t != NULL) {
        if (t->context->fd == fd) {
            break;
        } else {
            t = t->next;
        }
    }
    return (t == NULL) ? NULL : t->context;
}

int InsertHashTable(HashTable *p, Context *context)
{
    if ((p == NULL) || (context == NULL)) {
        return -1;
    }

    int slot = CalcHash(p, context->fd);
    ListNode *t = p->list[slot];
    ListNode *q = t;
    while (t != NULL && t->context->fd != context->fd) {
        q = t;
        t = t->next;
    }
    if (t == NULL) {
        ListNode *n = (ListNode *)calloc(1, sizeof(ListNode));
        if (n == NULL) {
            LOGE("insert element calloc failed!");
            return -1;
        }
        n->context = context;
        n->next = NULL;
        if (q == NULL) {
            p->list[slot] = n;
        } else {
            q->next = n;
        }
        p->size += 1;
        int count = p->slots << 1;
        if (p->size > count) {
            return RebuildHashTable(p);
        }
    } else {
        return INSERT_HASH_DUPLICATE; /* fd is duplicate */
    }
    return 0;
}

void DeleteHashTable(HashTable *p, const Context *context)
{
    if ((p == NULL) || (context == NULL)) {
        return;
    }

    int slot = CalcHash(p, context->fd);
    ListNode *t = p->list[slot];
    ListNode *q = t;
    while (t != NULL && t->context->fd != context->fd) {
        q = t;
        t = t->next;
    }
    if (t != NULL) {
        if (t == p->list[slot]) {
            p->list[slot] = t->next;
        } else {
            q->next = t->next;
        }
        free(t);
        t = NULL;
        p->size -= 1;
    }
    return;
}
