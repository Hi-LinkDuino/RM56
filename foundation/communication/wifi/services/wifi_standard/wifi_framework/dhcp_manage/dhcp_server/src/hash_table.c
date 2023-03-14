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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "securec.h"

#define HASH_TRUE 1
#define HASH_FALSE 0

#define HASH_CAP_LOAD_FACTOR 5
#define HASH_CAP_GROWTH_FACTOR 10
#define HASH_CALC_CODE_CALC 5

const int HASH_DEFAULT_VALUE = 5381;
const int HASH_CALC_MODE_HASH = 1;

size_t HashCode(uintptr_t key, size_t keySize);
int HashCompare(uintptr_t firstKey, uintptr_t secondKey, size_t keySize);
size_t Hash(const HashTable *table, uintptr_t key);
int HashEquals(const HashTable *table, uintptr_t firstKey, uintptr_t secondKey);
int HashShouldGrow(HashTable *table);
int HashShouldShrink(HashTable *table);
int CapExtend(HashTable *table, size_t miniCapacity);
HashNode *HashCreateNode(HashTable *table, uintptr_t key, uintptr_t value, HashNode *next);
int HashPushFront(HashTable *table, size_t index, uintptr_t key, uintptr_t value);
void DestroyHashNode(HashNode *node);
int HashAdjustCapacity(HashTable *table);
int HashAllocate(HashTable *table, size_t capacity);
void HashRehash(HashTable *table, HashNode **old, size_t oldCapacity);

int CreateHashTable(HashTable *table, size_t keySize, size_t valueSize, size_t capacity)
{
    if (table == 0) {
        return HASH_ERROR;
    }

    if (capacity < HASH_MINI_CAPACITY) {
        capacity = HASH_MINI_CAPACITY;
    }

    if (HashAllocate(table, capacity) == HASH_ERROR) {
        return HASH_ERROR;
    }

    table->keySize = keySize;
    table->valueSize = valueSize;
    table->hash = HashCode;
    table->compare = HashCompare;
    table->size = 0;
    return HASH_SUCCESS;
}

int DestroyHashTable(HashTable *table)
{
    if (!Initialized(table)) {
        return HASH_ERROR;
    }
    for (size_t current = 0; current < table->capacity; ++current) {
        HashNode *node = table->nodes[current];
        while (node) {
            HashNode *next = node->next;
            DestroyHashNode(node);
            node = next;
        }
    }
    free(table->nodes);
    table->nodes = NULL;
    return HASH_SUCCESS;
}

int Insert(HashTable *table, uintptr_t key, uintptr_t value)
{
    if (!Initialized(table)) {
        return HASH_ERROR;
    }
    if (key == 0) {
        return HASH_ERROR;
    }
    if (HashShouldGrow(table)) {
        HashAdjustCapacity(table);
    }
    size_t index = Hash(table, key);
    int ret = HASH_INSERTED;
    for (HashNode *node = table->nodes[index]; node; node = node->next) {
        if (HashEquals(table, key, node->key)) {
            ret = HASH_UPDATED;
        }
        if (ret == HASH_UPDATED && memcpy_s((void *)node->value, table->valueSize,
            (void *)value, table->valueSize) != EOK) {
            return HASH_ERROR;
        }
        if (ret == HASH_UPDATED) {
            return HASH_UPDATED;
        }
    }
    if (HashPushFront(table, index, key, value) == HASH_ERROR) {
        return HASH_ERROR;
    }
    ++table->size;
    return ret;
}

int ContainsKey(const HashTable *table, uintptr_t key)
{
    if (!Initialized(table)) {
        return HASH_FALSE;
    }
    if (key == 0) {
        return HASH_FALSE;
    }

    size_t index = Hash(table, key);
    for (HashNode *node = table->nodes[index]; node; node = node->next) {
        if (HashEquals(table, key, node->key)) {
            return HASH_TRUE;
        }
    }
    return HASH_FALSE;
}

uintptr_t At(HashTable *table, uintptr_t key)
{
    if (table == 0) {
        return 0;
    }
    if (key == 0) {
        return 0;
    }

    size_t index = Hash(table, key);
    for (HashNode *node = table->nodes[index]; node; node = node->next) {
        if (HashEquals(table, key, node->key)) {
            return node->value;
        }
    }
    return 0;
}

int Remove(HashTable *table, uintptr_t key)
{
    if (table == 0) {
        return HASH_ERROR;
    }
    if (key == 0) {
        return HASH_ERROR;
    }

    size_t index = Hash(table, key);
    HashNode *node = table->nodes[index];

    for (HashNode *previous = NULL; node; previous = node, node = node->next) {
        if (HashEquals(table, key, node->key)) {
            if (previous) {
                previous->next = node->next;
            } else {
                table->nodes[index] = node->next;
            }

            DestroyHashNode(node);
            --table->size;
            int shouldShrink = 0;
            if (HashShouldShrink(table)) {
                shouldShrink = 1;
            }
            if (shouldShrink && HashAdjustCapacity(table) == HASH_ERROR) {
                return HASH_ERROR;
            }
            return HASH_SUCCESS;
        }
    }
    return HASH_KEY_NOT_FOUND;
}

int ClearAll(HashTable *table)
{
    if (table == 0 || table->nodes == 0) {
        return HASH_ERROR;
    }
    if (table->size == 0) {
        return HASH_SUCCESS;
    }
    if (DestroyHashTable(table) == HASH_ERROR || HashAllocate(table, HASH_MINI_CAPACITY) == HASH_ERROR) {
        return HASH_ERROR;
    }
    table->size = 0;
    return HASH_SUCCESS;
}

int Empty(const HashTable *table)
{
    if (table == 0) {
        return HASH_ERROR;
    }
    if (table->size == 0) {
        return HASH_SUCCESS;
    }
    return HASH_NOT_EMPTY;
}

int Initialized(const HashTable *table)
{
    if (table && table->capacity && table->keySize && table->valueSize && table->hash && table->compare) {
        return HASH_TRUE;
    }
    return HASH_FALSE;
}

int CapExtend(HashTable *table, size_t miniCapacity)
{
    if (!Initialized(table)) {
        return HASH_ERROR;
    }

    if (miniCapacity > table->threshold) {
        return Resize(table, miniCapacity / HASH_CAP_LOAD_FACTOR);
    }
    return HASH_SUCCESS;
}

int HashCompare(uintptr_t firstKey, uintptr_t secondKey, size_t keySize)
{
    if (memcmp((void *)firstKey, (void *)secondKey, keySize) == 0) {
        return 0;
    }
    return 1;
}

size_t HashCode(uintptr_t rawKey, size_t keySize)
{
    size_t hash = HASH_DEFAULT_VALUE;
    const char *key = (char *)rawKey;

    for (size_t byte = 0; byte < keySize; ++byte) {
        hash = ((hash << HASH_CALC_CODE_CALC) + hash) ^ key[byte];
    }
    return hash;
}

size_t Hash(const HashTable *table, uintptr_t key)
{
    if (HASH_CALC_MODE_HASH) {
        return table->hash(key, table->keySize) % table->capacity;
    }
    return table->hash(key, table->keySize) & table->capacity;
}

int HashEquals(const HashTable *table, uintptr_t firstKey, uintptr_t secondKey)
{
    if (table->compare(firstKey, secondKey, table->keySize) == 0) {
        return HASH_TRUE;
    }
    return HASH_FALSE;
}

int HashShouldGrow(HashTable *table)
{
    if (table->size == table->capacity) {
        return HASH_TRUE;
    }
    return HASH_FALSE;
}

int HashShouldShrink(HashTable *table)
{
    static size_t capSszie = sizeof(size_t);
    if (table->size == table->capacity / capSszie) {
        return HASH_TRUE;
    }
    return HASH_FALSE;
}

HashNode *HashCreateNode(HashTable *table, uintptr_t key, uintptr_t value, HashNode *next)
{
    HashNode *node = (HashNode *)calloc(1, sizeof(HashNode));
    if (node == 0) {
        return 0;
    }
    int flag = 0;
    do {
        node->key = (uintptr_t)calloc(table->keySize, sizeof(char));
        node->value = (uintptr_t)calloc(table->valueSize, sizeof(char));
        if (node->key == 0 || node->value == 0) {
            break;
        }
        if (memcpy_s((void *)node->key, table->keySize, (void *)key, table->keySize) != EOK ||
            memcpy_s((void *)node->value, table->valueSize, (void *)value, table->valueSize) != EOK) {
            break;
        }
        flag += 1;
    } while (0);
    if (flag == 0) {
        free((void *)node->key);
        free((void *)node->value);
        free(node);
        node = NULL;
        return 0;
    }
    node->next = next;
    return node;
}

int HashPushFront(HashTable *table, size_t index, uintptr_t key, uintptr_t value)
{
    table->nodes[index] = HashCreateNode(table, key, value, table->nodes[index]);
    if (table->nodes[index] != NULL) {
        return HASH_SUCCESS;
    }
    return HASH_ERROR;
}

void DestroyHashNode(HashNode *node)
{
    free((void *)node->key);
    free((void *)node->value);
    free(node);
    node = NULL;
}

int HashAdjustCapacity(HashTable *table)
{
    return Resize(table, table->size * HASH_CAP_GROWTH_FACTOR);
}

int HashAllocate(HashTable *table, size_t capacity)
{
    if (capacity <= 0) {
        return HASH_ERROR;
    }
    if ((table->nodes = calloc(capacity, sizeof(HashNode *))) == 0) {
        return HASH_ERROR;
    }
    table->capacity = capacity;
    table->threshold = capacity * HASH_CAP_LOAD_FACTOR;
    return HASH_SUCCESS;
}

int Resize(HashTable *table, size_t newSize)
{
    HashNode **old;
    size_t oldSize;
    size_t readySize = newSize;
    if (readySize < HASH_MINI_CAPACITY) {
        if (table->capacity > HASH_MINI_CAPACITY) {
            readySize = HASH_MINI_CAPACITY;
        } else {
            return HASH_SUCCESS;
        }
    }

    old = table->nodes;
    oldSize = table->capacity;
    if (HashAllocate(table, readySize) == HASH_ERROR) {
        return HASH_ERROR;
    }
    HashRehash(table, old, oldSize);
    free(old);
    old = NULL;
    return HASH_SUCCESS;
}

void HashRehash(HashTable *table, HashNode **old, size_t oldCapacity)
{
    for (size_t current = 0; current < oldCapacity; ++current) {
        HashNode *node = old[current];
        while (node != 0) {
            HashNode *next = node->next;
            size_t newPos = Hash(table, node->key);
            node->next = table->nodes[newPos];
            table->nodes[newPos] = node;
            node = next;
        }
    }
}