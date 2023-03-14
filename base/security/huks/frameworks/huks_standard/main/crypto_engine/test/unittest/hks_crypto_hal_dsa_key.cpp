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
class HksCryptoHalDsaKey : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

const uint32_t DSA_SIZE_256 = 256;
const uint32_t DSA_SIZE_512 = 512;
void HksCryptoHalDsaKey::SetUpTestCase(void)
{
}

void HksCryptoHalDsaKey::TearDownTestCase(void)
{
}

void HksCryptoHalDsaKey::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalDsaKey::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalDsaKey_001
 * @tc.name      : HksCryptoHalDsaKey_001
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate DSA-256bit key.
 */
HWTEST_F(HksCryptoHalDsaKey, HksCryptoHalDsaKey_001, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_DSA,
        .keyLen = DSA_SIZE_256,
        .algParam = nullptr,
    };

    HksBlob key = { .size = 0, .data = nullptr };

    ret = HksCryptoHalGenerateKey(&spec, &key);
#if defined(HKS_SUPPORT_DSA_C) && defined(HKS_SUPPORT_DSA_GENERATE_KEY)
#if defined(_USE_OPENSSL_)
    ASSERT_EQ(HKS_SUCCESS, ret);
    ASSERT_NE((uint32_t)0, key.size);
    ASSERT_NE(nullptr, key.data);
    HksFree(key.data);
#endif
#if defined(_USE_MBEDTLS_)
    ASSERT_EQ(HKS_ERROR_NOT_SUPPORTED, ret);
    HksFree(key.data);
#endif
#else
    ASSERT_EQ(HKS_ERROR_NOT_SUPPORTED, ret);
#endif
}

/**
 * @tc.number    : HksCryptoHalDsaKey_002
 * @tc.name      : HksCryptoHalDsaKey_002
 * @tc.desc      : Generate key and export public key with DSA.
 */
HWTEST_F(HksCryptoHalDsaKey, HksCryptoHalDsaKey_002, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_DSA,
        .keyLen = DSA_SIZE_512,
    };

    HksBlob key = { .size = 0, .data = NULL };

    ret = HksCryptoHalGenerateKey(&spec, &key);
#ifdef _USE_OPENSSL_
    ASSERT_EQ(ret, HKS_SUCCESS);
#endif
#ifdef _USE_MBEDTLS_
    ASSERT_EQ(ret, HKS_ERROR_NOT_SUPPORTED);
    return;
#endif

    KeyMaterialDsa *keyMaterial = (KeyMaterialDsa *)key.data;
    ASSERT_NE(keyMaterial, nullptr);

    uint32_t keyOutLen =
        sizeof(KeyMaterialDsa) + keyMaterial->ySize + keyMaterial->pSize + keyMaterial->qSize + keyMaterial->gSize;
    HksBlob keyOut = { .size = keyOutLen, .data = (uint8_t *)HksMalloc(keyOutLen) };
    ASSERT_NE(keyOut.data, nullptr);

    ret = HksCryptoHalGetPubKey(&key, &keyOut);
#ifdef _USE_OPENSSL_
    ASSERT_EQ(ret, HKS_SUCCESS);
#endif
#ifdef _USE_MBEDTLS_
    ASSERT_EQ(ret, HKS_ERROR_NOT_SUPPORTED);
#endif
    HKS_FREE_BLOB(key);
    HKS_FREE_BLOB(keyOut);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS