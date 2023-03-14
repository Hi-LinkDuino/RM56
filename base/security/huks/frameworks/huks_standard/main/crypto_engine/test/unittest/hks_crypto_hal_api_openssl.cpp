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
const uint32_t KEY_LEN_256 = 256;
class HksCryptoHalApiOpenssl : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void HksCryptoHalApiOpenssl::SetUpTestCase(void)
{
}

void HksCryptoHalApiOpenssl::TearDownTestCase(void)
{
}

void HksCryptoHalApiOpenssl::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalApiOpenssl::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_001
 * @tc.name      : HksCryptoHalApiOpenssl_001
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate key -- key is NULL.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_001, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_AES,
        .keyLen = HKS_AES_KEY_SIZE_128,
    };

    ret = HksCryptoHalGenerateKey(&spec, NULL);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_002
 * @tc.name      : HksCryptoHalApiOpenssl_002
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate key -- algType is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_002, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = 0xffff,
        .keyLen = 0,
    };

    HksBlob key = {};

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_003
 * @tc.name      : HksCryptoHalApiOpenssl_003
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate key -- keyLen is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_003, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_AES,
        .keyLen = 1,
    };

    HksBlob key = {};

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.algType = HKS_ALG_RSA;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.algType = HKS_ALG_ECC;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.algType = HKS_ALG_DSA;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.algType = HKS_ALG_HMAC;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

#ifdef HKS_SUPPORT_DH_C
    spec.algType = HKS_ALG_DH;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_KEY_SIZE, ret);
#endif
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_004
 * @tc.name      : HksCryptoHalApiOpenssl_004
 * @tc.desc      : Using HksCryptoHalEncrypt -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_004, Function | SmallTest | Level0)
{
    int32_t ret;

    HksBlob key = { .size = 0, .data = nullptr };
    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob message = { .size = 0, .data = nullptr };
    HksBlob cipherText = { .size = 0, .data = nullptr };
    HksBlob tagAead = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    ret = HksCryptoHalEncrypt(&key, nullptr, &message, &cipherText, &tagAead);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    key = { .size = 1, .data = buff };
    ret = HksCryptoHalEncrypt(&key, nullptr, &message, &cipherText, &tagAead);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    message = { .size = 1, .data = buff };
    ret = HksCryptoHalEncrypt(&key, nullptr, &message, &cipherText, &tagAead);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    cipherText = { .size = 1, .data = buff };
    ret = HksCryptoHalEncrypt(&key, nullptr, &message, &cipherText, &tagAead);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_005
 * @tc.name      : HksCryptoHalApiOpenssl_005
 * @tc.desc      : Using HksCryptoHalDecrypt -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_005, Function | SmallTest | Level0)
{
    int32_t ret;

    HksBlob key = { .size = 0, .data = nullptr };
    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob message = { .size = 0, .data = nullptr };
    HksBlob cipherText = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    ret = HksCryptoHalDecrypt(&key, nullptr, &message, &cipherText);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    key = { .size = 1, .data = buff };
    ret = HksCryptoHalDecrypt(&key, nullptr, &message, &cipherText);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    message = { .size = 1, .data = buff };
    ret = HksCryptoHalDecrypt(&key, nullptr, &message, &cipherText);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    cipherText = { .size = 1, .data = buff };
    ret = HksCryptoHalDecrypt(&key, nullptr, &message, &cipherText);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_006
 * @tc.name      : HksCryptoHalApiOpenssl_006
 * @tc.desc      : Using HksCryptoHalEncrypt -- AES encrypt parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_006, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob key = { .size = 1, .data = buff };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };
    HksBlob tagAead = { .size = 1, .data = buff };

    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.mode = HKS_MODE_CBC;
    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.mode = HKS_MODE_CTR;
    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.mode = HKS_MODE_ECB;
    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.mode = HKS_MODE_GCM;
    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_007
 * @tc.name      : HksCryptoHalApiOpenssl_007
 * @tc.desc      : Using HksCryptoHalDecrypt -- AES decrypt parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_007, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob key = { .size = 1, .data = buff };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };

    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    key.size = sizeof(buff);
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec.mode = HKS_MODE_GCM;
    key.size = 1;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_008
 * @tc.name      : HksCryptoHalApiOpenssl_008
 * @tc.desc      : Using HksCryptoHalDecrypt -- decrypt padding is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_008, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = HKS_MODE_CBC, .padding = HKS_PADDING_PSS };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };

    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_CRYPTO_ENGINE_ERROR, ret);

    spec.mode = HKS_MODE_ECB;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_CRYPTO_ENGINE_ERROR, ret);

    spec.algType = HKS_ALG_RSA;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_FAILURE, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_009
 * @tc.name      : HksCryptoHalApiOpenssl_009
 * @tc.desc      : Using HksCryptoHalSign -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_009, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob message = { .size = 0, .data = nullptr };
    HksBlob signature = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    EXPECT_EQ(HksCryptoHalSign(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalSign(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    message = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalSign(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    signature = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalSign(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    EXPECT_EQ(HksCryptoHalSign(&key, &spec, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_010
 * @tc.name      : HksCryptoHalApiOpenssl_010
 * @tc.desc      : Using HksCryptoHalVerify -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_010, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob message = { .size = 0, .data = nullptr };
    HksBlob signature = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    EXPECT_EQ(HksCryptoHalVerify(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalVerify(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    message = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalVerify(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    signature = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalVerify(&key, nullptr, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    EXPECT_EQ(HksCryptoHalVerify(&key, &spec, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_011
 * @tc.name      : HksCryptoHalApiOpenssl_011
 * @tc.desc      : RSA Using HksCryptoHalSign -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_011, Function | SmallTest | Level0)
{
    HksKeySpec spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_512,
    };

    uint8_t buff[HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_512)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec signSpec = { .algType = HKS_ALG_RSA, .padding = HKS_PADDING_PKCS1_V1_5 };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob signature = { .size = 1, .data = buff };

    EXPECT_EQ(HksCryptoHalSign(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);

    signSpec.padding = HKS_PADDING_PSS;
    EXPECT_EQ(HksCryptoHalSign(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);

    HksCryptoHalGenerateKey(&spec, &key);
    EXPECT_NE(HksCryptoHalSign(&key, &signSpec, &message, &signature), HKS_SUCCESS);
    HKS_FREE_BLOB(key);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_012
 * @tc.name      : HksCryptoHalApiOpenssl_012
 * @tc.desc      : RSA Using HksCryptoHalVerify -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_012, Function | SmallTest | Level0)
{
    uint8_t buff[HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_512)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec signSpec = { .algType = HKS_ALG_RSA };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob signature = { .size = 1, .data = buff };

    EXPECT_EQ(HksCryptoHalVerify(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);

    signSpec.padding = HKS_PADDING_PSS;
    EXPECT_EQ(HksCryptoHalVerify(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_013
 * @tc.name      : HksCryptoHalApiOpenssl_013
 * @tc.desc      : DSA Using HksCryptoHalSign -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_013, Function | SmallTest | Level0)
{
    HksKeySpec spec = {
        .algType = HKS_ALG_DSA,
        .keyLen = KEY_LEN_256,
    };

    uint8_t buff[HKS_KEY_BYTES(KEY_LEN_256)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec signSpec = { .algType = HKS_ALG_DSA };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob signature = { .size = 1, .data = buff };

    EXPECT_EQ(HksCryptoHalSign(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);

    HksCryptoHalGenerateKey(&spec, &key);
    EXPECT_NE(HksCryptoHalSign(&key, &signSpec, &message, &signature), HKS_SUCCESS);
    HKS_FREE_BLOB(key);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_014
 * @tc.name      : HksCryptoHalApiOpenssl_014
 * @tc.desc      : DSA Using HksCryptoHalVerify -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_014, Function | SmallTest | Level0)
{
    uint8_t buff[HKS_KEY_BYTES(KEY_LEN_256)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec signSpec = { .algType = HKS_ALG_DSA };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob signature = { .size = 1, .data = buff };

    EXPECT_EQ(HksCryptoHalVerify(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_015
 * @tc.name      : HksCryptoHalApiOpenssl_015
 * @tc.desc      : ECDSA Using HksCryptoHalSign -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_015, Function | SmallTest | Level0)
{
    uint8_t buff[HKS_KEY_BYTES(HKS_ECC_KEY_SIZE_256)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec signSpec = { .algType = HKS_ALG_ECC };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob signature = { .size = 1, .data = buff };

    EXPECT_EQ(HksCryptoHalSign(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_016
 * @tc.name      : HksCryptoHalApiOpenssl_016
 * @tc.desc      : ECDSA Using HksCryptoHalVerify -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_016, Function | SmallTest | Level0)
{
    HksKeySpec spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
    };

    uint8_t buff[HKS_KEY_BYTES(KEY_LEN_256)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec signSpec = { .algType = HKS_ALG_ECC };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob signature = { .size = 1, .data = buff };

    EXPECT_EQ(HksCryptoHalVerify(&key, &signSpec, &message, &signature), HKS_ERROR_INVALID_KEY_INFO);

    HksCryptoHalGenerateKey(&spec, &key);
    EXPECT_NE(HksCryptoHalVerify(&key, &signSpec, &message, &signature), HKS_SUCCESS);
    HKS_FREE_BLOB(key);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_017
 * @tc.name      : HksCryptoHalApiOpenssl_017
 * @tc.desc      : Using HksCryptoHalAgreeKey -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_017, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksBlob pubKey = { .size = 0, .data = nullptr };
    HksKeySpec spec = { .algType = 0xffff };
    HksBlob agreeKey = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, nullptr, &agreeKey), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, nullptr, &agreeKey), HKS_ERROR_INVALID_ARGUMENT);

    pubKey = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, nullptr, &agreeKey), HKS_ERROR_INVALID_ARGUMENT);

    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_ERROR_INVALID_ARGUMENT);

    agreeKey = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_018
 * @tc.name      : HksCryptoHalApiOpenssl_018
 * @tc.desc      : ECDH Using HksCryptoHalAgreeKey -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_018, Function | SmallTest | Level0)
{
    HksKeySpec keySpec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
    };

    uint8_t buff[HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_512)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksBlob pubKey = { .size = sizeof(buff), .data = buff };
    HksKeySpec spec = { .algType = HKS_ALG_ECDH };
    HksBlob agreeKey = { .size = sizeof(buff), .data = buff };

    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_ERROR_INVALID_ARGUMENT);

    spec.keyLen = HKS_ECC_KEY_SIZE_256;
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_FAILURE);

    HksCryptoHalGenerateKey(&keySpec, &key);
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_FAILURE);
    HKS_FREE_BLOB(key);
}

#ifdef HKS_SUPPORT_DH_C
/**
 * @tc.number    : HksCryptoHalApiOpenssl_019
 * @tc.name      : HksCryptoHalApiOpenssl_019
 * @tc.desc      : DH Using HksCryptoHalAgreeKey -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_019, Function | SmallTest | Level0)
{
    HksKeySpec keySpec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_2048,
    };

    uint8_t buff[HKS_KEY_BYTES(HKS_DH_KEY_SIZE_2048)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksBlob pubKey = { .size = sizeof(buff), .data = buff };
    HksKeySpec spec = { .algType = HKS_ALG_DH, .keyLen = HKS_DH_KEY_SIZE_4096 };
    HksBlob agreeKey = { .size = sizeof(buff), .data = buff };

    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_ERROR_INVALID_KEY_SIZE);

    spec.keyLen = 0;
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_ERROR_CRYPTO_ENGINE_ERROR);

    HksCryptoHalGenerateKey(&keySpec, &key);
    EXPECT_EQ(HksCryptoHalAgreeKey(&key, &pubKey, &spec, &agreeKey), HKS_ERROR_CRYPTO_ENGINE_ERROR);

    HKS_FREE_BLOB(key);
}
#endif

/**
 * @tc.number    : HksCryptoHalApiOpenssl_020
 * @tc.name      : HksCryptoHalApiOpenssl_020
 * @tc.desc      : Using HksCryptoHalAgreeKey -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_020, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksBlob message = { .size = 0, .data = nullptr };
    HksBlob signature = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    EXPECT_EQ(HksCryptoHalHmac(&key, 0, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalHmac(&key, 0, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    message = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalHmac(&key, 0, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    signature = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalHmac(&key, 0, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    EXPECT_EQ(HksCryptoHalHmac(&key, HKS_DIGEST_SHA512, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_021
 * @tc.name      : HksCryptoHalApiOpenssl_021
 * @tc.desc      : Using HksCryptoHalHmacInit -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_021, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};
    void *hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);

    EXPECT_EQ(HksCryptoHalHmacInit(&key, 0, &hmactestctx), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalHmacInit(&key, 0, &hmactestctx), HKS_ERROR_INVALID_ARGUMENT);

    EXPECT_EQ(HksCryptoHalHmacInit(&key, HKS_DIGEST_NONE, &hmactestctx), HKS_ERROR_INVALID_ARGUMENT);

    HksFree(hmactestctx);
    hmactestctx = nullptr;
    EXPECT_EQ(HksCryptoHalHmacInit(&key, HKS_DIGEST_SHA512, NULL), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_022
 * @tc.name      : HksCryptoHalApiOpenssl_022
 * @tc.desc      : Using HksCryptoHalHmacUpdate -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_022, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksBlob message = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};
    void *hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);

    EXPECT_EQ(HksCryptoHalHmacUpdate(&message, hmactestctx), HKS_ERROR_INVALID_ARGUMENT);
    HksFree(hmactestctx);

    key = { .size = 0, .data = nullptr };
    HksKeySpec spec = {.algType = HKS_ALG_HMAC, .keyLen = KEY_LEN_256, .algParam = nullptr};
    EXPECT_EQ(HksCryptoHalGenerateKey(&spec, &key), HKS_SUCCESS);

    hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);
    EXPECT_EQ(HksCryptoHalHmacInit(&key, HKS_DIGEST_SHA512, &hmactestctx), HKS_SUCCESS);
    EXPECT_EQ(HksCryptoHalHmacUpdate(&message, hmactestctx), HKS_ERROR_INVALID_ARGUMENT);
    HksFree(hmactestctx);

    message = { .size = 1, .data = buff };
    EXPECT_EQ(HksCryptoHalHmacUpdate(&message, NULL), HKS_ERROR_INVALID_ARGUMENT);
}


/**
 * @tc.number    : HksCryptoHalApiOpenssl_024
 * @tc.name      : HksCryptoHalApiOpenssl_024
 * @tc.desc      : Using HksCryptoHalEncryptInit -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_024, Function | SmallTest | Level0)
{
    int32_t ret;

    HksBlob key = { .size = 0, .data = nullptr };
    HksUsageSpec spec = { .algType = 0xffff };
    uint8_t buff[1] = {0};

    ret = HksCryptoHalEncryptInit(&key, nullptr, nullptr);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    key = { .size = 1, .data = buff };
    ret = HksCryptoHalEncryptInit(&key, nullptr, nullptr);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec = { .algType = HKS_ALG_AES, .mode = 0xffff };

    ret = HksCryptoHalEncryptInit(&key, &spec, nullptr);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    void *ctx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(ctx, nullptr);

    ret = HksCryptoHalEncryptInit(&key, &spec, &ctx);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
    HksFree(ctx);
    ctx = nullptr;
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_025
 * @tc.name      : HksCryptoHalApiOpenssl_025
 * @tc.desc      : Using HksCryptoHalEncryptUpdate -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_025, Function | SmallTest | Level0)
{
    int32_t ret;

    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob message = { .size = 0, .data = nullptr };
    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob out = { .size = 0, .data = nullptr };

    ret = HksCryptoHalEncryptUpdate(&message, nullptr, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    message = { .size = 1, .data = buff };
    ret = HksCryptoHalEncryptUpdate(&message, nullptr, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    void *ctx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(ctx, nullptr);
    ret = HksCryptoHalEncryptUpdate(&message, &ctx, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    out = { .size = 1, .data = buff };
    ret = HksCryptoHalEncryptUpdate(&message, &ctx, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    ret = HksCryptoHalEncryptUpdate(&message, &ctx, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
    HksFree(ctx);
    ctx = nullptr;
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_026
 * @tc.name      : HksCryptoHalApiOpenssl_026
 * @tc.desc      : Using HksCryptoHalEncryptFinal -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_026, Function | SmallTest | Level0)
{
    int32_t ret;

    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob cipherText = { .size = 0, .data = nullptr };
    HksBlob tagAead = { .size = 0, .data = nullptr };
    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob messageLast = { .size = 0, .data = nullptr };

    ret = HksCryptoHalEncryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    messageLast = { .size = 1, .data = buff };
    ret = HksCryptoHalEncryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    void *ctx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(ctx, nullptr);
    ret = HksCryptoHalEncryptFinal(&messageLast, &ctx, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    cipherText = { .size = 1, .data = buff };
    ret = HksCryptoHalEncryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    tagAead = { .size = 1, .data = buff };
    ret = HksCryptoHalEncryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    ret = HksCryptoHalEncryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_027
 * @tc.name      : HksCryptoHalApiOpenssl_027
 * @tc.desc      : Using HksCryptoHalDecryptInit -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_027, Function | SmallTest | Level0)
{
    int32_t ret;

    HksBlob key = { .size = 0, .data = nullptr };
    HksUsageSpec spec = { .algType = 0xffff };
    uint8_t buff[1] = {0};

    ret = HksCryptoHalDecryptInit(&key, nullptr, nullptr);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    key = { .size = 1, .data = buff };
    ret = HksCryptoHalDecryptInit(&key, nullptr, nullptr);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec = { .algType = HKS_ALG_AES, .mode = 0xffff };

    ret = HksCryptoHalDecryptInit(&key, &spec, nullptr);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    void *ctx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(ctx, nullptr);

    ret = HksCryptoHalDecryptInit(&key, &spec, &ctx);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
    HksFree(ctx);
    ctx = nullptr;
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_028
 * @tc.name      : HksCryptoHalApiOpenssl_028
 * @tc.desc      : Using HksCryptoHalDecryptUpdate -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_028, Function | SmallTest | Level0)
{
    int32_t ret;

    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob message = { .size = 0, .data = nullptr };
    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob out = { .size = 0, .data = nullptr };

    ret = HksCryptoHalDecryptUpdate(&message, nullptr, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    message = { .size = 1, .data = buff };
    ret = HksCryptoHalDecryptUpdate(&message, nullptr, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    void *ctx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(ctx, nullptr);
    ret = HksCryptoHalDecryptUpdate(&message, &ctx, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    out = { .size = 1, .data = buff };
    ret = HksCryptoHalDecryptUpdate(&message, &ctx, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    ret = HksCryptoHalDecryptUpdate(&message, &ctx, &out, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
    HksFree(ctx);
    ctx = nullptr;
}

/**
 * @tc.number    : HksCryptoHalApiOpenssl_029
 * @tc.name      : HksCryptoHalApiOpenssl_029
 * @tc.desc      : Using HksCryptoHalDecryptFinal -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiOpenssl, HksCryptoHalApiOpenssl_029, Function | SmallTest | Level0)
{
    int32_t ret;

    HksUsageSpec spec = { .algType = 0xffff };
    HksBlob cipherText = { .size = 0, .data = nullptr };
    HksBlob tagAead = { .size = 0, .data = nullptr };
    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob messageLast = { .size = 0, .data = nullptr };

    ret = HksCryptoHalDecryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    messageLast = { .size = 1, .data = buff };
    ret = HksCryptoHalDecryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    void *ctx = (void *)HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(ctx, nullptr);
    ret = HksCryptoHalDecryptFinal(&messageLast, &ctx, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    cipherText = { .size = 1, .data = buff };
    ret = HksCryptoHalDecryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    tagAead = { .size = 1, .data = buff };
    ret = HksCryptoHalDecryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    ret = HksCryptoHalDecryptFinal(&messageLast, nullptr, &cipherText, &tagAead, spec.algType);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
    HksFree(ctx);
    ctx = nullptr;
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS