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

#ifndef CRPC_HASH_TABLE_H
#define CRPC_HASH_TABLE_H

#include "common.h"
#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * HashTable
 * _______________
 * |             |
 * |   ListNode  |----> ListNode ----> ListNode
 * |_____________|
 * |             |
 * |   ListNode  |----> ListNode ----> ListNode
 * |_____________|
 * |             |
 * |   ListNode  |----> ListNode ----> ListNode
 * |_____________|
 * |             |
 * |   ListNode  |----> ListNode ----> ListNode
 * |_____________|
 */

typedef struct ListNode ListNode;
struct ListNode {
    struct Context *context;
    struct ListNode *next;
};

typedef struct HashTable HashTable;
struct HashTable {
    unsigned int slots;
    int size;
    ListNode **list;
};

/**
 * @Description Create a hash table
 *
 * @param slots - Number of buckets
 * @return HashTable* - pointer to the hash table or NULL if failed
 */
HashTable *InitHashTable(int slots);

/**
 * @Description Destroy HashTable
 *
 * @param p - HashTable object's pointer
 */
void DestroyHashTable(HashTable *p);

/**
 * @Description Find Context object by fd
 *
 * @param p - HashTable object's pointer
 * @param fd - Socket fd
 * @return Context* - return the context pointer
 */
Context *FindContext(HashTable *p, int fd);

/**
 * @Description Insert a Context to the HashTable
 *
 * @param p - HashTable object's pointer
 * @param context - Context to be added
 * @return int - 0 Success, -1 Failed, -2 Exist
 */
int InsertHashTable(HashTable *p, Context *context);

/**
 * @Description Delete a context from the HashTable
 *
 * @param p - HashTable object's pointer
 * @param context - Context to be deleted
 */
void DeleteHashTable(HashTable *p, const Context *context);

#ifdef __cplusplus
}
#endif
#endif