/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <string>

#include "hks_storage_file_lock.h"

using namespace testing::ext;

namespace {
namespace {
constexpr size_t MAX_TEST_COUNT = 1024;
}

class HksStorageFileLockTest : public testing::Test {};

HWTEST_F(HksStorageFileLockTest, HksStorageFileLockTest_00100, Function | SmallTest | Level1)
{
    std::string path = "/test/test";
    HksStorageFileLock *lock = HksStorageFileLockCreate(&path[0]);
    EXPECT_NE(lock, nullptr);
    int32_t result = HksStorageFileLockRead(lock);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileUnlockRead(lock);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileLockWrite(lock);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileUnlockWrite(lock);
    EXPECT_EQ(result, (uint32_t)0);
    HksStorageFileLockRelease(lock);
}

HWTEST_F(HksStorageFileLockTest, HksStorageFileLockTest_00200, Function | SmallTest | Level1)
{
    std::string pathBase = "/test/test";
    std::vector<HksStorageFileLock *> locks;
    for (size_t i = 0; i < MAX_TEST_COUNT; i++) {
        std::string path = pathBase + std::to_string(i);
        HksStorageFileLock *lock = HksStorageFileLockCreate(&path[0]);
        if (lock != nullptr) {
            locks.push_back(lock);
        }
    }
    EXPECT_EQ(locks.size(), MAX_TEST_COUNT);
    for (auto lock : locks) {
        HksStorageFileLockRelease(lock);
    }
}

HWTEST_F(HksStorageFileLockTest, HksStorageFileLockTest_00300, Function | SmallTest | Level1)
{
    std::string path = "/test/test";
    std::vector<HksStorageFileLock *> locks;
    for (size_t i = 0; i < MAX_TEST_COUNT; i++) {
        HksStorageFileLock *lock = HksStorageFileLockCreate(&path[0]);
        if (lock != nullptr) {
            locks.push_back(lock);
        }
    }
    EXPECT_EQ(locks.size(), MAX_TEST_COUNT);

    HksStorageFileLock *first = locks[0];
    for (auto lock : locks) {
        EXPECT_EQ(first, lock);
    }

    for (auto lock : locks) {
        HksStorageFileLockRelease(lock);
    }
}

HWTEST_F(HksStorageFileLockTest, HksStorageFileLockTest_00400, Function | SmallTest | Level1)
{
    std::string path = "/test/test";
    HksStorageFileLock *lock1 = HksStorageFileLockCreate(&path[0]);
    EXPECT_NE(lock1, nullptr);
    HksStorageFileLock *lock2 = HksStorageFileLockCreate(&path[0]);
    EXPECT_EQ(lock1, lock2);

    int32_t result = HksStorageFileLockRead(lock1);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileUnlockRead(lock1);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileLockRead(lock2);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileUnlockRead(lock2);
    EXPECT_EQ(result, (uint32_t)0);

    result = HksStorageFileLockWrite(lock1);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileUnlockWrite(lock1);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileLockWrite(lock2);
    EXPECT_EQ(result, (uint32_t)0);
    result = HksStorageFileUnlockWrite(lock2);
    EXPECT_EQ(result, (uint32_t)0);

    HksStorageFileLockRelease(lock1);
    HksStorageFileLockRelease(lock2);
}
}  // namespace