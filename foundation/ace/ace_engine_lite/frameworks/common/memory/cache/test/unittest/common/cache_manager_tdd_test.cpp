/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "cache_manager_tdd_test.h"

#include "ace_mem_base.h"

namespace OHOS {
namespace ACELite {
const CacheUnit CacheManagerTddTest::TEST_CONFIG_TABLE[] = {
    {USER_LOCALICATION, 16}, // localication key-value cache, lowest 16KB
};

void CacheManagerTddTest::SetUp()
{
    const size_t count = 1;
    CacheManager::GetInstance().SetConfigTable(TEST_CONFIG_TABLE, count);
}

/**
 * @tc.name: CacheDistributeTest001
 * @tc.desc: Verify the cache distribution process
 */
HWTEST_F(CacheManagerTddTest, CacheDistribute001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare cache buffer
     */
    const size_t totalSize = 20 * 1024; // 20KB
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * totalSize));
    if (buffer == nullptr) {
        return;
    }
    /**
     * @tc.steps: step2. trigger the cache setup
     */
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(buffer);
    CacheManager::GetInstance().SetupCacheMemInfo(startAddr, totalSize);
    /**
     * @tc.steps: step3. verify the result
     */
    uintptr_t localicationPos = CacheManager::GetInstance().GetCacheBufAddress(CacheUser::USER_LOCALICATION);
    size_t localicationBufSize = CacheManager::GetInstance().GetCacheBufLength(CacheUser::USER_LOCALICATION);
    const uintptr_t targetPos = startAddr + MAGIC_NUMBER_LENGTH;
    const size_t unit = 1024;
    const uint8_t localicationMin = 16;
    const size_t targetLength = localicationMin * unit;
    EXPECT_EQ(localicationPos, targetPos);
    EXPECT_EQ(localicationBufSize, targetLength);
    ace_free(buffer);
    buffer = nullptr;
}

/**
 * @tc.name: CacheDistributeTest002
 * @tc.desc: Verify the cache distribution process
 */
HWTEST_F(CacheManagerTddTest, CacheDistribute002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare cache buffer
     */
    const size_t totalSize = 20 * 1024; // 40KB
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * totalSize));
    if (buffer == nullptr) {
        return;
    }
    const uint8_t pageFileBufLength = 6;
    const uint8_t localicationBufLength = 8;
    const CacheUnit customerCacheUnitTable[] = {
        {USER_PAGE_FILE, pageFileBufLength},
        {USER_LOCALICATION, localicationBufLength},
    };
    /**
     * @tc.steps: step2. trigger the cache setup
     */
    const size_t count = 2;
    CacheManager::GetInstance().SetConfigTable(customerCacheUnitTable, count);
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(buffer);
    CacheManager::GetInstance().SetupCacheMemInfo(startAddr, totalSize);
    /**
     * @tc.steps: step3. verify the result
     */
    uintptr_t bufferPos = CacheManager::GetInstance().GetCacheBufAddress(CacheUser::USER_LOCALICATION);
    size_t bufSize = CacheManager::GetInstance().GetCacheBufLength(CacheUser::USER_LOCALICATION);
    const size_t unit = 1024;
    const uint8_t magicNumberCount = MAGIC_NUMBER_COUNT;
    uintptr_t targetPos =
        startAddr + (pageFileBufLength * unit) + (magicNumberCount * MAGIC_NUMBER_LENGTH) + MAGIC_NUMBER_LENGTH;
    const size_t targetLength = localicationBufLength * unit;
    EXPECT_EQ(bufferPos, targetPos);
    EXPECT_EQ(bufSize, targetLength);
    ace_free(buffer);
    buffer = nullptr;
}

/**
 * @tc.name: CacheDistributeTest003
 * @tc.desc: Verify the cache distribution process
 */
HWTEST_F(CacheManagerTddTest, CacheDistribute003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare cache buffer
     */
    const size_t totalSize = 2 * 1024; // 2KB
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * totalSize));
    if (buffer == nullptr) {
        return;
    }

    /**
     * @tc.steps: step2. trigger the cache setup
     */
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(buffer);
    CacheManager::GetInstance().SetupCacheMemInfo(startAddr, totalSize);
    /**
     * @tc.steps: step3. verify the result
     */
    uintptr_t localPos = CacheManager::GetInstance().GetCacheBufAddress(CacheUser::USER_LOCALICATION);
    size_t localBufSize = CacheManager::GetInstance().GetCacheBufLength(CacheUser::USER_LOCALICATION);
    EXPECT_EQ(localPos, 0);
    EXPECT_EQ(localBufSize, 0);
    ace_free(buffer);
    buffer = nullptr;
}

/**
 * @tc.name: CacheDistributeTest004
 * @tc.desc: Verify the cache distribution process
 */
HWTEST_F(CacheManagerTddTest, CacheDistribute004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare cache buffer
     */
    const size_t totalSize = 20 * 1024; // 20KB
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * totalSize));
    if (buffer == nullptr) {
        return;
    }

    /**
     * @tc.steps: step2. trigger the cache setup
     */
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(buffer);
    CacheManager::GetInstance().SetupCacheMemInfo(startAddr, totalSize);
    /**
     * @tc.steps: step3. verify the result
     */
    uintptr_t bufStartPos = CacheManager::GetInstance().GetCacheBufAddress(CacheUser::USER_LOCALICATION);
    size_t bufLength = CacheManager::GetInstance().GetCacheBufLength(CacheUser::USER_LOCALICATION);
    uint32_t headMagicNumber = *reinterpret_cast<uint32_t *>(bufStartPos - MAGIC_NUMBER_LENGTH);
    EXPECT_EQ(headMagicNumber, CACHE_MEM_MAGIC_NUMBER);
    uint32_t tailMagicNumber = *reinterpret_cast<uint32_t *>(bufStartPos + bufLength);
    EXPECT_EQ(tailMagicNumber, CACHE_MEM_MAGIC_NUMBER);
    ace_free(buffer);
    buffer = nullptr;
}

/**
 * @tc.name: CacheDistributeTest005
 * @tc.desc: Verify the cache distribution process and overflow checking
 */
HWTEST_F(CacheManagerTddTest, CacheDistribute005, TestSize.Level0)
{
    /**
     * @tc.steps: step1. prepare cache buffer
     */
    const size_t totalSize = 20 * 1024; // 20KB
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * totalSize));
    if (buffer == nullptr) {
        return;
    }

    /**
     * @tc.steps: step2. trigger the cache setup
     */
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(buffer);
    CacheManager::GetInstance().SetupCacheMemInfo(startAddr, totalSize);
    size_t bufLength = CacheManager::GetInstance().GetCacheBufLength(CacheUser::USER_LOCALICATION);
    if (bufLength < MAGIC_NUMBER_LENGTH) {
        ace_free(buffer);
        buffer = nullptr;
        return;
    }
    /**
     * @tc.steps: step3. overflow the cache head and tail
     */
    buffer[0] = 0x11;
    size_t tailPos = bufLength - MAGIC_NUMBER_LENGTH;
    buffer[tailPos] = 0x22;
    EXPECT_TRUE(CacheManager::GetInstance().IsCacheOverflow(CacheUser::USER_LOCALICATION));
    EXPECT_FALSE(CacheManager::GetInstance().IsWholeCacheHealthy());
    ace_free(buffer);
    buffer = nullptr;
}

/**
 * @tc.name: CacheDistributeTest006
 * @tc.desc: Verify the cache distribution process, considering the magic number length
 */
HWTEST_F(CacheManagerTddTest, CacheDistribute006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. prepare cache buffer
     */
    const size_t totalSize = 16 * 1024; // 16KB, the total length equals to the requirement
    char *buffer = static_cast<char *>(ace_malloc(sizeof(char) * totalSize));
    if (buffer == nullptr) {
        return;
    }

    /**
     * @tc.steps: step2. trigger the cache setup, will failed, as no extra space for the magic numbers
     */
    uintptr_t startAddr = reinterpret_cast<uintptr_t>(buffer);
    CacheManager::GetInstance().SetupCacheMemInfo(startAddr, totalSize);

    /**
     * @tc.steps: step3. check the result, the distributing will fail
     */
    uintptr_t bufStartPos = CacheManager::GetInstance().GetCacheBufAddress(CacheUser::USER_LOCALICATION);
    size_t bufLength = CacheManager::GetInstance().GetCacheBufLength(CacheUser::USER_LOCALICATION);
    EXPECT_EQ(bufStartPos, 0);
    EXPECT_EQ(bufLength, 0);
    ace_free(buffer);
    buffer = nullptr;
}

/**
 * used for debugging TDD implementation on simulator
 */
void CacheManagerTddTest::RunTests()
{
#ifndef TDD_ASSERTIONS
    CacheDistribute001();
    CacheDistribute002();
    CacheDistribute003();
    CacheDistribute004();
    CacheDistribute005();
    CacheDistribute006();
#endif // TDD_ASSERTIONS
}
} // namespace ACELite
} // namespace OHOS
