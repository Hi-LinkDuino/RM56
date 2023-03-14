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

#ifndef OHOS_HASH_TABLE_H
#define OHOS_HASH_TABLE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HASH_MINI_CAPACITY 8

enum HashTableResult {
    HASH_SUCCESS = 0,
    HASH_ERROR,
    HASH_UPDATED,
    HASH_INSERTED,
    HASH_KEY_NOT_FOUND,
    HASH_NOT_EMPTY,
};

typedef int (*HashComparison)(uintptr_t, uintptr_t, size_t);
typedef size_t (*HashType)(uintptr_t, size_t);
typedef struct HashNode {
    struct HashNode *next;
    uintptr_t key;
    uintptr_t value;
} HashNode;

typedef struct HashTable {
    size_t size;
    size_t threshold;
    size_t capacity;
    size_t keySize;
    size_t valueSize;
    HashComparison compare;
    HashType hash;
    HashNode **nodes;
} HashTable;

int CreateHashTable(HashTable *table, size_t keySize, size_t valueSize, size_t capacity);
int Initialized(const HashTable *table);
int DestroyHashTable(HashTable *table);
int Insert(HashTable *table, uintptr_t key, uintptr_t value);
int ContainsKey(const HashTable *table, uintptr_t key);
uintptr_t At(HashTable *table, uintptr_t key);
int Remove(HashTable *table, uintptr_t key);
int ClearAll(HashTable *table);
int Empty(const HashTable *table);
int Resize(HashTable *table, size_t newSize);

#ifdef __cplusplus
}
#endif
#endif