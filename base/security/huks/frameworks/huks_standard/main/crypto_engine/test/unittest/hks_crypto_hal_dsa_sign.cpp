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
const uint32_t DSA_SIZE_256 = 256;
const uint32_t SIGNATURE_SIZE = 1024;
const uint32_t PUB_KEY_SIZE = 1024;
const HksUsageSpec HKS_CRYPTO_HAL_DSA_SIGN_001_SPEC = {
    .algType = HKS_ALG_DSA,
    .mode = HKS_MODE_ECB,
    .padding = HKS_PADDING_NONE,
    .digest = HKS_DIGEST_SHA1,
    .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
};

const HksUsageSpec HKS_CRYPTO_HAL_DSA_SIGN_002_SPEC = {
    .algType = HKS_ALG_DSA,
    .mode = HKS_MODE_ECB,
    .padding = HKS_PADDING_NONE,
    .digest = HKS_DIGEST_SHA224,
    .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
};

const HksUsageSpec HKS_CRYPTO_HAL_DSA_SIGN_003_SPEC = {
    .algType = HKS_ALG_DSA,
    .mode = HKS_MODE_ECB,
    .padding = HKS_PADDING_NONE,
    .digest = HKS_DIGEST_SHA256,
    .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
};

const HksUsageSpec HKS_CRYPTO_HAL_DSA_SIGN_004_SPEC = {
    .algType = HKS_ALG_DSA,
    .mode = HKS_MODE_ECB,
    .padding = HKS_PADDING_NONE,
    .digest = HKS_DIGEST_SHA384,
    .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
};

const HksUsageSpec HKS_CRYPTO_HAL_DSA_SIGN_005_SPEC = {
    .algType = HKS_ALG_DSA,
    .mode = HKS_MODE_ECB,
    .padding = HKS_PADDING_NONE,
    .digest = HKS_DIGEST_SHA512,
    .purpose = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY,
};
}  // namespace

class HksCryptoHalDsaSign : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
protected:
    void RunTestCase(const HksUsageSpec &hksUsageSpec) const
    {
        HksKeySpec spec = {
            .algType = HKS_ALG_DSA,
            .keyLen = DSA_SIZE_256,
            .algParam = nullptr,
        };

        HksBlob key = { .size = 0, .data = nullptr };

#if defined(_USE_MBEDTLS_)
        EXPECT_EQ(HksCryptoHalGenerateKey(&spec, &key), HKS_ERROR_NOT_SUPPORTED);
        return;
#endif
#if defined(_USE_OPENSSL_)
        EXPECT_EQ(HksCryptoHalGenerateKey(&spec, &key), HKS_SUCCESS);
#endif

        const char *hexData = "00112233445566778899aabbccddeeff";
        uint32_t dataLen = strlen(hexData) / HKS_COUNT_OF_HALF;

        HksBlob message = { .size = dataLen, .data = (uint8_t *)HksMalloc(dataLen) };
        ASSERT_NE(message.data, nullptr);
        for (uint32_t ii = 0; ii < dataLen; ii++) {
            message.data[ii] = ReadHex((const uint8_t *)&hexData[HKS_COUNT_OF_HALF * ii]);
        }

        uint8_t hashData[HKS_HMAC_DIGEST_SHA512_LEN] = {0};
        struct HksBlob hash = { HKS_HMAC_DIGEST_SHA512_LEN, hashData };
        EXPECT_EQ(HksCryptoHalHash(hksUsageSpec.digest, &message, &hash), HKS_SUCCESS);

        struct HksBlob signature = { .size = SIGNATURE_SIZE, .data = (uint8_t *)HksMalloc(SIGNATURE_SIZE) };
        ASSERT_NE(signature.data, nullptr);

        EXPECT_EQ(HksCryptoHalSign(&key, &hksUsageSpec, &hash, &signature), HKS_SUCCESS);

        struct HksBlob pubKey = { .size = PUB_KEY_SIZE, .data = (uint8_t *)HksMalloc(PUB_KEY_SIZE) };
        ASSERT_NE(pubKey.data, nullptr);

        EXPECT_EQ(HksCryptoHalGetPubKey(&key, &pubKey), HKS_SUCCESS);

        EXPECT_EQ(HksCryptoHalVerify(&pubKey, &hksUsageSpec, &hash, &signature), HKS_SUCCESS);

        HksFree(key.data);
        HksFree(message.data);
        HksFree(signature.data);
        HksFree(pubKey.data);
    }
};

void HksCryptoHalDsaSign::SetUpTestCase(void)
{
}

void HksCryptoHalDsaSign::TearDownTestCase(void)
{
}

void HksCryptoHalDsaSign::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalDsaSign::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalDsaSign_001
 * @tc.name      : HksCryptoHalDsaSign_001
 * @tc.desc      : Using HksCryptoHalSign Sign DSA-SHA1 key.
 */
HWTEST_F(HksCryptoHalDsaSign, HksCryptoHalDsaSign_001, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DSA_SIGN_001_SPEC);
}

/**
 * @tc.number    : HksCryptoHalDsaSign_002
 * @tc.name      : HksCryptoHalDsaSign_002
 * @tc.desc      : Using HksCryptoHalSign Sign DSA-SHA224 key.
 */
HWTEST_F(HksCryptoHalDsaSign, HksCryptoHalDsaSign_002, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DSA_SIGN_002_SPEC);
}

/**
 * @tc.number    : HksCryptoHalDsaSign_003
 * @tc.name      : HksCryptoHalDsaSign_003
 * @tc.desc      : Using HksCryptoHalSign Sign DSA-SHA256 key.
 */
HWTEST_F(HksCryptoHalDsaSign, HksCryptoHalDsaSign_003, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DSA_SIGN_003_SPEC);
}

/**
 * @tc.number    : HksCryptoHalDsaSign_004
 * @tc.name      : HksCryptoHalDsaSign_004
 * @tc.desc      : Using HksCryptoHalSign Sign DSA-SHA384 key.
 */
HWTEST_F(HksCryptoHalDsaSign, HksCryptoHalDsaSign_004, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DSA_SIGN_004_SPEC);
}

/**
 * @tc.number    : HksCryptoHalDsaSign_005
 * @tc.name      : HksCryptoHalDsaSign_005
 * @tc.desc      : Using HksCryptoHalSign Sign DSA-SHA512 key.
 */
HWTEST_F(HksCryptoHalDsaSign, HksCryptoHalDsaSign_005, Function | SmallTest | Level0)
{
    RunTestCase(HKS_CRYPTO_HAL_DSA_SIGN_005_SPEC);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS