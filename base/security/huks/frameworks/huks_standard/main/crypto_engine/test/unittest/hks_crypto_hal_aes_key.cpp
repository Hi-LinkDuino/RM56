/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <iostream>

#include "hks_ability.h"
#include "hks_config.h"
#include "hks_crypto_hal.h"
#include "hks_crypto_hal_common.h"
#include "hks_mem.h"

using namespace testing::ext;
namespace OHOS {
namespace Security {
namespace Huks {
namespace UnitTest {
namespace {
struct TestCaseParams {
    HksKeySpec spec = {0};

    HksErrorCode generateKeyResult = HksErrorCode::HKS_SUCCESS;
};

const TestCaseParams HKS_CRYPTO_HAL_AES_KEY_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_AES,
        .keyLen = HKS_AES_KEY_SIZE_128,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_AES_C) && defined(HKS_SUPPORT_AES_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_AES_KEY_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_AES,
        .keyLen = HKS_AES_KEY_SIZE_192,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_AES_C) && defined(HKS_SUPPORT_AES_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_AES_KEY_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_AES,
        .keyLen = HKS_AES_KEY_SIZE_256,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_AES_C) && defined(HKS_SUPPORT_AES_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};
}  // namespace

class HksCryptoHalAesKey : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const TestCaseParams &testCaseParams) const
    {
        HksBlob key = { .size = 0, .data = nullptr };
        ASSERT_EQ(HksCryptoHalGenerateKey(&testCaseParams.spec, &key), testCaseParams.generateKeyResult);
        if (testCaseParams.generateKeyResult == HKS_SUCCESS) {
            ASSERT_NE((uint32_t)0, key.size);
            ASSERT_NE(nullptr, key.data);
            HksFree(key.data);
        }
    }
};

void HksCryptoHalAesKey::SetUpTestCase(void)
{
}

void HksCryptoHalAesKey::TearDownTestCase(void)
{
}

void HksCryptoHalAesKey::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalAesKey::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalAesKey_001
 * @tc.name      : HksCryptoHalAesKey_001
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate AES-128bit key.
 */
HWTEST_F(HksCryptoHalAesKey, HksCryptoHalAesKey_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_KEY_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesKey_002
 * @tc.name      : HksCryptoHalAesKey_002
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate AES-192bit key.
 */
HWTEST_F(HksCryptoHalAesKey, HksCryptoHalAesKey_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_KEY_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalAesKey_003
 * @tc.name      : HksCryptoHalAesKey_003
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate AES-256bit key.
 */
HWTEST_F(HksCryptoHalAesKey, HksCryptoHalAesKey_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_AES_KEY_003_PARAMS);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS