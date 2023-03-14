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

#include <gtest/gtest.h>
#include <stdint.h>
#include <stdbool.h>
#include "hash_table.h"

using namespace testing::ext;

class HashTableTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        if (CreateHashTable(&table, sizeof(uint32_t),  sizeof(uint32_t), HASH_MINI_CAPACITY) != HASH_SUCCESS) {
            printf("failed to create hash table.\n");
        }
    }
    virtual void TearDown()
    {
        if (Initialized(&table)) {
            DestroyHashTable(&table);
        }
    }
public:
    HashTable table;
};


HWTEST_F(HashTableTest, InitializedTest, TestSize.Level1)
{
    HashTable testTable = {0};
    EXPECT_EQ(0, Initialized(&testTable));
    EXPECT_EQ(0, Initialized(0));
    EXPECT_EQ(1, Initialized(&table));
}

HWTEST_F(HashTableTest, InsertTest, TestSize.Level1)
{
    uint32_t keyOffset = 100;
    uint32_t valOffset = 1000;
    size_t expectSize = 100;

    EXPECT_TRUE(Initialized(&table));
    int testRet = 1;
    for (size_t i = 0; i < expectSize; i++)
    {
        uint32_t key = keyOffset + i;
        uint32_t val = valOffset;
        if (Insert(&table, (uintptr_t)&key, (uintptr_t)&val) != HASH_INSERTED) {
            printf("failed to test insert.\n");
            testRet = 0;
        }
    }
    uint32_t updateKey = keyOffset;
    uint32_t updateVal = 1000;
    EXPECT_TRUE(Insert(&table, (uintptr_t)&updateKey, (uintptr_t)&updateVal) == HASH_UPDATED);
    EXPECT_TRUE(testRet);
    EXPECT_TRUE(expectSize == table.size);
    EXPECT_EQ(HASH_SUCCESS, ClearAll(&table));
    EXPECT_TRUE(table.size == 0);
}

HWTEST_F(HashTableTest, ContainsKeyTest, TestSize.Level1)
{
    uint32_t key1 = 1001;
    uint32_t val1 = 2001;
    uint32_t key2 = 1002;
    uint32_t val2 = 2002;
    uint32_t key3 = 1003;
    uint32_t val3 = 2003;

    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key1, (uintptr_t)&val1));
    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key2, (uintptr_t)&val2));
    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key3, (uintptr_t)&val3));
    EXPECT_EQ(HASH_UPDATED, Insert(&table, (uintptr_t)&key3, (uintptr_t)&val3));
    uint32_t testKey  = 1002;
    uint32_t testKey2 = 6002;
    EXPECT_EQ(1, ContainsKey(&table, (uintptr_t)&testKey));
    EXPECT_EQ(0, ContainsKey(&table, (uintptr_t)&testKey2));
    EXPECT_EQ(HASH_SUCCESS, ClearAll(&table));
    EXPECT_TRUE(table.size == 0);
}

HWTEST_F(HashTableTest, GetAtTest, TestSize.Level1)
{
    uint32_t key1 = 1001;
    uint32_t val1 = 2001;
    uint32_t key2 = 1002;
    uint32_t val2 = 2002;
    uint32_t key3 = 1003;
    uint32_t val3 = 2003;

    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key1, (uintptr_t)&val1));
    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key2, (uintptr_t)&val2));
    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key3, (uintptr_t)&val3));

    uint32_t testKey  = 1002;
    uint32_t expectVal = val2;
    uintptr_t valPtr = At(&table, (uintptr_t)&testKey);
    uint32_t rstVal = 0;
    if (valPtr) {
        rstVal = *(uint32_t*)valPtr;
    }
    EXPECT_EQ(expectVal, rstVal);
    EXPECT_EQ(HASH_SUCCESS, ClearAll(&table));
    EXPECT_TRUE(table.size == 0);
}

HWTEST_F(HashTableTest, RemoveTest, TestSize.Level1)
{
    uint32_t key1 = 1001;
    uint32_t val1 = 2001;
    uint32_t key2 = 1002;
    uint32_t val2 = 2002;
    uint32_t key3 = 1003;
    uint32_t val3 = 2003;

    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key1, (uintptr_t)&val1));
    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key2, (uintptr_t)&val2));
    EXPECT_EQ(HASH_INSERTED, Insert(&table, (uintptr_t)&key3, (uintptr_t)&val3));

    uint32_t testKey  = 1002;
    EXPECT_EQ(1, ContainsKey(&table, (uintptr_t)&testKey));
    EXPECT_EQ(HASH_SUCCESS, Remove(&table, (uintptr_t)&testKey));
    EXPECT_EQ(0, ContainsKey(&table, (uintptr_t)&testKey));
    EXPECT_EQ(HASH_SUCCESS, ClearAll(&table));
    EXPECT_TRUE(table.size == 0);
}

HWTEST_F(HashTableTest, EmptyTest, TestSize.Level1)
{
    EXPECT_EQ(HASH_ERROR, Empty(0));
    EXPECT_EQ(HASH_SUCCESS, Empty(&table));
    uint32_t key1 = 1001;
    uint32_t val1 = 2001;
    EXPECT_TRUE(Insert(&table, (uintptr_t)&key1, (uintptr_t)&val1) == HASH_INSERTED);
    EXPECT_EQ(HASH_NOT_EMPTY, Empty(&table));
}

HWTEST_F(HashTableTest, ResizeTest, TestSize.Level1)
{
    HashTable testTable;
    EXPECT_EQ(HASH_SUCCESS, CreateHashTable(&testTable, sizeof(uint32_t),
        sizeof(uint32_t), HASH_MINI_CAPACITY));
    EXPECT_TRUE(testTable.capacity == HASH_MINI_CAPACITY);
    EXPECT_EQ(HASH_SUCCESS, Resize(&testTable, HASH_MINI_CAPACITY * 2));
    EXPECT_TRUE(testTable.capacity == HASH_MINI_CAPACITY * 2);
    EXPECT_EQ(HASH_SUCCESS, DestroyHashTable(&testTable));
}

extern "C" int CapExtend(HashTable *table, size_t miniCapacity);
HWTEST_F(HashTableTest, CapExtendTest, TestSize.Level1)
{
    HashTable testTable;
    EXPECT_EQ(HASH_SUCCESS, CreateHashTable(&testTable, sizeof(uint32_t),
        sizeof(uint32_t), HASH_MINI_CAPACITY));
    EXPECT_TRUE(testTable.capacity == HASH_MINI_CAPACITY);
    EXPECT_EQ(HASH_SUCCESS, CapExtend(&testTable, HASH_MINI_CAPACITY * 5));
    EXPECT_TRUE(testTable.capacity == HASH_MINI_CAPACITY);
    EXPECT_EQ(HASH_SUCCESS, DestroyHashTable(&testTable));
}
