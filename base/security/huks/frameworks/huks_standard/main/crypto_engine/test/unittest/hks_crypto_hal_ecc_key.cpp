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

const TestCaseParams HKS_CRYPTO_HAL_ECC_KEY_001_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_224,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_ECC_C) && defined(HKS_SUPPORT_ECC_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_ECC_KEY_002_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_ECC_C) && defined(HKS_SUPPORT_ECC_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_ECC_KEY_003_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_ECC_C) && defined(HKS_SUPPORT_ECC_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};

const TestCaseParams HKS_CRYPTO_HAL_ECC_KEY_004_PARAMS = {
    .spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_521,
        .algParam = nullptr,
    },
#if defined(HKS_SUPPORT_ECC_C) && defined(HKS_SUPPORT_ECC_GENERATE_KEY)
    .generateKeyResult = HKS_SUCCESS,
#else
    .generateKeyResult = HKS_ERROR_NOT_SUPPORTED,
#endif
};
}  // namespace

class HksCryptoHalEccKey : public HksCryptoHalCommon, public testing::Test {
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

void HksCryptoHalEccKey::SetUpTestCase(void)
{
}

void HksCryptoHalEccKey::TearDownTestCase(void)
{
}

void HksCryptoHalEccKey::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalEccKey::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalEccKey_001
 * @tc.name      : HksCryptoHalEccKey_001
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate ECC-224bit key.
 */
HWTEST_F(HksCryptoHalEccKey, HksCryptoHalEccKey_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECC_KEY_001_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEccKey_002
 * @tc.name      : HksCryptoHalEccKey_002
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate ECC-256bit key.
 */
HWTEST_F(HksCryptoHalEccKey, HksCryptoHalEccKey_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECC_KEY_002_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEccKey_003
 * @tc.name      : HksCryptoHalEccKey_003
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate ECC-384bit key.
 */
HWTEST_F(HksCryptoHalEccKey, HksCryptoHalEccKey_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECC_KEY_003_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEccKey_004
 * @tc.name      : HksCryptoHalEccKey_004
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate ECC-521bit key.
 */
HWTEST_F(HksCryptoHalEccKey, HksCryptoHalEccKey_004, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_ECC_KEY_004_PARAMS);
}

/**
 * @tc.number    : HksCryptoHalEccKey_005
 * @tc.name      : HksCryptoHalEccKey_005
 * @tc.desc      : Generate key and export public key with ECC.
 */
HWTEST_F(HksCryptoHalEccKey, HksCryptoHalEccKey_005, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_384,
    };

    HksBlob key = { .size = 0, .data = NULL };

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_SUCCESS);

    KeyMaterialEcc *keyMaterial = (KeyMaterialEcc *)key.data;
    ASSERT_NE(keyMaterial, nullptr);

    uint32_t keyOutLen = sizeof(KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize;
    HksBlob keyOut = { .size = keyOutLen, .data = (uint8_t *)HksMalloc(keyOutLen) };
    ASSERT_NE(keyOut.data, nullptr);

    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_SUCCESS);
    HKS_FREE_BLOB(key);
    HKS_FREE_BLOB(keyOut);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS