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
const uint32_t IV_SIZE = 20;
const uint32_t ERROR_KEY_SIZE = 2000;
class HksCryptoHalApiMbedtls : public HksCryptoHalCommon, public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void HksCryptoHalApiMbedtls::SetUpTestCase(void)
{
}

void HksCryptoHalApiMbedtls::TearDownTestCase(void)
{
}

void HksCryptoHalApiMbedtls::SetUp()
{
    EXPECT_EQ(HksCryptoAbilityInit(), 0);
}

void HksCryptoHalApiMbedtls::TearDown()
{
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_001
 * @tc.name      : HksCryptoHalApiMbedtls_001
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate key -- key is NULL.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_001, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_AES,
        .keyLen = HKS_AES_KEY_SIZE_128,
        .algParam = nullptr,
    };

    ret = HksCryptoHalGenerateKey(&spec, NULL);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_002
 * @tc.name      : HksCryptoHalApiMbedtls_002
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate key -- algType is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_002, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = 0xffff,
        .keyLen = 0,
        .algParam = nullptr,
    };

    HksBlob key = {};

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_003
 * @tc.name      : HksCryptoHalApiMbedtls_003
 * @tc.desc      : Using HksCryptoHalGenerateKey Generate key -- algType is AES keyLen is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_003, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_AES,
        .keyLen = 0,
        .algParam = nullptr,
    };

    HksBlob key = {};

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_004
 * @tc.name      : HksCryptoHalApiMbedtls_004
 * @tc.desc      : Using HksCryptoHalEncrypt -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_004, Function | SmallTest | Level0)
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
 * @tc.number    : HksCryptoHalApiMbedtls_005
 * @tc.name      : HksCryptoHalApiMbedtls_005
 * @tc.desc      : Using HksCryptoHalDecrypt -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_005, Function | SmallTest | Level0)
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
 * @tc.number    : HksCryptoHalApiMbedtls_006
 * @tc.name      : HksCryptoHalApiMbedtls_006
 * @tc.desc      : Using HksCryptoHalEncrypt -- AES encrypt parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_006, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob key = { .size = 1, .data = buff };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };
    HksBlob tagAead = { .size = 1, .data = buff };

    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_KEY_SIZE, ret);

    key.size = sizeof(buff);
    ret = HksCryptoHalEncrypt(&key, &spec, &message, &cipherText, &tagAead);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_007
 * @tc.name      : HksCryptoHalApiMbedtls_007
 * @tc.desc      : Using HksCryptoHalDecrypt -- AES decrypt parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_007, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob key = { .size = 1, .data = buff };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = 0xffff };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };

    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_KEY_SIZE, ret);

    key.size = sizeof(buff);
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_008
 * @tc.name      : HksCryptoHalApiMbedtls_008
 * @tc.desc      : Using HksCryptoHalDecrypt -- AES decrypt padding is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_008, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = HKS_MODE_CBC, .padding = HKS_PADDING_PSS };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };

    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_PADDING, ret);

    spec.mode = HKS_MODE_ECB;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_PADDING, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_009
 * @tc.name      : HksCryptoHalApiMbedtls_009
 * @tc.desc      : Using HksCryptoHalDecrypt -- AES decrypt CBC mode nopadding with large iv.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_009, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    uint8_t iv[IV_SIZE] = {0};
    HksBlob key = { .size = sizeof(buff), .data = buff };
    HksCipherParam cipherParam = { .iv = { .size = sizeof(iv), .data = iv } };
    HksUsageSpec spec = { .algType = HKS_ALG_AES, .mode = HKS_MODE_CBC, .padding = HKS_PADDING_NONE };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };

    spec.algParam = &cipherParam;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_IV, ret);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_010
 * @tc.name      : HksCryptoHalApiMbedtls_010
 * @tc.desc      : Using HksCryptoHalDecrypt -- RSA decrypt key is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_010, Function | SmallTest | Level0)
{
    int32_t ret;

    uint8_t buff[HKS_KEY_BYTES(HKS_AES_KEY_SIZE_128)] = {0};
    uint8_t iv[IV_SIZE] = {0};
    HksBlob key;
    HksCipherParam cipherParam = { .iv = { .size = sizeof(iv), .data = iv } };
    HksUsageSpec spec = { .algType = HKS_ALG_RSA, .mode = HKS_MODE_ECB, .padding = HKS_PADDING_NONE };
    HksBlob message = { .size = 1, .data = buff };
    HksBlob cipherText = { .size = 1, .data = buff };
    spec.algParam = &cipherParam;

    uint32_t keyLen = sizeof(KeyMaterialRsa) + HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096) +
                      HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096) +
                      (HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096) + HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096));
    key = { .size = keyLen, .data = (uint8_t *)HksMalloc(keyLen) };
    ASSERT_NE(key.data, nullptr);
    KeyMaterialRsa *keyMaterial = (KeyMaterialRsa *)key.data;
    keyMaterial->keyAlg = HKS_ALG_RSA;
    keyMaterial->keySize = HKS_RSA_KEY_SIZE_4096;
    keyMaterial->nSize = HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096);
    keyMaterial->eSize = HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096);
    keyMaterial->dSize = HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096) + HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096);

    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_ARGUMENT, ret);

    key.size -= HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096);
    keyMaterial->dSize = HKS_KEY_BYTES(HKS_RSA_KEY_SIZE_4096);
    keyMaterial->keySize = ERROR_KEY_SIZE;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_KEY_SIZE, ret);

    key.size = sizeof(KeyMaterialRsa);
    keyMaterial->keySize = HKS_RSA_KEY_SIZE_4096;
    ret = HksCryptoHalDecrypt(&key, &spec, &message, &cipherText);
    EXPECT_EQ(HKS_ERROR_INVALID_KEY_INFO, ret);
    HKS_FREE_BLOB(key);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_011
 * @tc.name      : HksCryptoHalApiMbedtls_011
 * @tc.desc      : Using HksMbedtlsGetRsaPubKey -- RSA in/out key is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_011, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_RSA,
        .keyLen = HKS_RSA_KEY_SIZE_2048,
    };
    HksBlob key = { .size = 0, .data = NULL };

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_SUCCESS);

    KeyMaterialRsa *keyMaterial = (KeyMaterialRsa *)key.data;

    uint32_t keyOutLen = sizeof(KeyMaterialRsa) + keyMaterial->nSize + keyMaterial->eSize;
    HksBlob keyOut = { .size = sizeof(KeyMaterialRsa), .data = (uint8_t *)HksMalloc(keyOutLen) };
    ASSERT_NE(keyOut.data, nullptr);

    keyMaterial->keySize = ERROR_KEY_SIZE;
    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_KEY_SIZE);

    keyMaterial->keySize = HKS_RSA_KEY_SIZE_2048;
    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_BUFFER_TOO_SMALL);
    HKS_FREE_BLOB(keyOut);
    HKS_FREE_BLOB(key);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_012
 * @tc.name      : HksCryptoHalApiMbedtls_012
 * @tc.desc      : Using HksMbedtlsEccGenerateKey -- key size is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_012, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
    };
    HksBlob key = { .size = 0, .data = NULL };

    spec.keyLen += 1;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_KEY_SIZE);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_013
 * @tc.name      : HksCryptoHalApiMbedtls_013
 * @tc.desc      : Using HksMbedtlsGetEccPubKey -- Ecc in/out key is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_013, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_ECC,
        .keyLen = HKS_ECC_KEY_SIZE_256,
    };
    HksBlob key = { .size = 0, .data = NULL };

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_SUCCESS);

    KeyMaterialEcc *keyMaterial = (KeyMaterialEcc *)key.data;

    uint32_t keyOutLen = sizeof(KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize;
    HksBlob keyOut = { .size = sizeof(KeyMaterialEcc), .data = (uint8_t *)HksMalloc(keyOutLen) };
    ASSERT_NE(keyOut.data, nullptr);

    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_BUFFER_TOO_SMALL);

    key.size -= 1;
    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_KEY_INFO);
    key.size += 1;

    keyMaterial->zSize = 0xFFFF;
    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_ARGUMENT);

    keyMaterial->keySize += 1;
    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_KEY_SIZE);

    HKS_FREE_BLOB(keyOut);
    HKS_FREE_BLOB(key);
}

#ifdef HKS_SUPPORT_DH_C
/**
 * @tc.number    : HksCryptoHalApiMbedtls_014
 * @tc.name      : HksCryptoHalApiMbedtls_014
 * @tc.desc      : Using HksMbedtlsDhGenerateKey -- key size is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_014, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_2048,
    };
    HksBlob key = { .size = 0, .data = NULL };

    spec.keyLen += 1;
    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_KEY_SIZE);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_015
 * @tc.name      : HksCryptoHalApiMbedtls_015
 * @tc.desc      : Using HksMbedtlsGetDhPubKey -- Ecc in/out key is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_015, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_2048,
    };
    HksBlob key = { .size = 0, .data = NULL };

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_SUCCESS);

    KeyMaterialDh *keyMaterial = (KeyMaterialDh *)key.data;

    uint32_t keyOutLen = sizeof(KeyMaterialDh) + keyMaterial->pubKeySize;
    HksBlob keyOut = { .size = sizeof(KeyMaterialDh), .data = (uint8_t *)HksMalloc(keyOutLen) };
    ASSERT_NE(keyOut.data, nullptr);

    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_BUFFER_TOO_SMALL);

    key.size = keyOut.size;
    ret = HksCryptoHalGetPubKey(&key, &keyOut);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_ARGUMENT);
    HKS_FREE_BLOB(keyOut);
    HKS_FREE_BLOB(key);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_016
 * @tc.name      : HksCryptoHalApiMbedtls_016
 * @tc.desc      : Using HksMbedtlsDhAgreeKey -- Ecc in/out key is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_016, Function | SmallTest | Level0)
{
    int32_t ret;

    HksKeySpec spec = {
        .algType = HKS_ALG_DH,
        .keyLen = HKS_DH_KEY_SIZE_2048,
    };
    HksBlob key = { .size = 0, .data = NULL };

    ret = HksCryptoHalGenerateKey(&spec, &key);
    ASSERT_EQ(ret, HKS_SUCCESS);

    KeyMaterialDh *keyMaterial = (KeyMaterialDh *)key.data;

    uint32_t keyOutLen = HKS_KEY_BYTES(spec.keyLen);
    HksBlob sharedKey = { .size = keyOutLen, .data = (uint8_t *)HksMalloc(keyOutLen) };
    ASSERT_NE(sharedKey.data, nullptr);

    keyMaterial->keySize -= 1;
    ret = HksCryptoHalAgreeKey(&key, &key, &spec, &sharedKey);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_KEY_SIZE);

    sharedKey.size -= 1;
    ret = HksCryptoHalAgreeKey(&key, &key, &spec, &sharedKey);
    ASSERT_EQ(ret, HKS_ERROR_BUFFER_TOO_SMALL);
    HKS_FREE_BLOB(sharedKey);
    HKS_FREE_BLOB(key);
}
#endif

/**
 * @tc.number    : HksCryptoHalApiMbedtls_017
 * @tc.name      : HksCryptoHalApiMbedtls_017
 * @tc.desc      : Using HksCryptoHalHmacInit -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_017, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};
    void *hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);

    ASSERT_EQ(HksCryptoHalHmacInit(&key, NULL, &hmactestctx), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    ASSERT_EQ(HksCryptoHalHmacInit(&key, NULL, &hmactestctx), HKS_ERROR_INVALID_DIGEST);

    HksFree(hmactestctx);
    hmactestctx = nullptr;
    ASSERT_EQ(HksCryptoHalHmacInit(&key, HKS_DIGEST_SHA512, NULL), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_018
 * @tc.name      : HksCryptoHalApiMbedtls_018
 * @tc.desc      : Using HksCryptoHalHmacUpdate -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_018, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksBlob message = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};
    void *hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);

    ASSERT_EQ(HksCryptoHalHmacUpdate(&message, hmactestctx), HKS_ERROR_INVALID_ARGUMENT);
    HksFree(hmactestctx);

    key = { .size = 0, .data = nullptr };
    HksKeySpec spec = {.algType = HKS_ALG_HMAC, .keyLen = 256, .algParam = nullptr};
    ASSERT_EQ(HksCryptoHalGenerateKey(&spec, &key), HKS_SUCCESS);

    hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);
    ASSERT_EQ(HksCryptoHalHmacInit(&key, HKS_DIGEST_SHA512, &hmactestctx), HKS_SUCCESS);
    ASSERT_EQ(HksCryptoHalHmacUpdate(&message, hmactestctx), HKS_ERROR_INVALID_ARGUMENT);
    HksFree(hmactestctx);

    message = { .size = 1, .data = buff };
    ASSERT_EQ(HksCryptoHalHmacUpdate(&message, NULL), HKS_ERROR_INVALID_ARGUMENT);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_019
 * @tc.name      : HksCryptoHalApiMbedtls_019
 * @tc.desc      : Using HksCryptoHalHmacFinal -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_019, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksKeySpec spec = {.algType = HKS_ALG_HMAC, .keyLen = 256, .algParam = nullptr};
    ASSERT_EQ(HksCryptoHalGenerateKey(&spec, &key), HKS_SUCCESS);
    const char *hexData = "00112233445566778899aabbccddeeff";
    uint32_t dataLen = strlen(hexData) / HKS_COUNT_OF_HALF;

    HksBlob message = { .size = dataLen, .data = (uint8_t *)HksMalloc(dataLen) };
    ASSERT_NE(message.data, nullptr);
    for (uint32_t ii = 0; ii < dataLen; ii++) {
        message.data[ii] = ReadHex((const uint8_t *)&hexData[HKS_COUNT_OF_HALF * ii]);
    }

    uint8_t buff[1] = {0};
    HksBlob signature = { .size = 1, .data = buff };
    void *hmactestctx = HksMalloc(HKS_CONTEXT_DATA_MAX);
    ASSERT_NE(hmactestctx, nullptr);
    ASSERT_EQ(HksCryptoHalHmacInit(&key, HKS_DIGEST_SHA1, &hmactestctx), HKS_SUCCESS);
    ASSERT_EQ(HksCryptoHalHmacUpdate(&message, hmactestctx), HKS_SUCCESS);
    ASSERT_EQ(HksCryptoHalHmacFinal(NULL, &hmactestctx, &signature), HKS_ERROR_INVALID_ARGUMENT);

    ASSERT_EQ(HksCryptoHalHmacFinal(&message, NULL, &signature), HKS_ERROR_INVALID_ARGUMENT);

    signature = { .size = 0, .data = nullptr };
    ASSERT_EQ(HksCryptoHalHmacFinal(&message, &hmactestctx, &signature), HKS_ERROR_INVALID_ARGUMENT);
    HksFree(hmactestctx);
    HksFree(message.data);
}

/**
 * @tc.number    : HksCryptoHalApiMbedtls_020
 * @tc.name      : HksCryptoHalApiMbedtls_020
 * @tc.desc      : Using HksCryptoHalHmac -- parameter is invalid.
 */
HWTEST_F(HksCryptoHalApiMbedtls, HksCryptoHalApiMbedtls_020, Function | SmallTest | Level0)
{
    HksBlob key = { .size = 0, .data = nullptr };
    HksBlob message = { .size = 0, .data = nullptr };
    HksBlob signature = { .size = 0, .data = nullptr };
    uint8_t buff[1] = {0};

    ASSERT_EQ(HksCryptoHalHmac(&key, NULL, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    key = { .size = 1, .data = buff };
    ASSERT_EQ(HksCryptoHalHmac(&key, NULL, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    message = { .size = 1, .data = buff };
    ASSERT_EQ(HksCryptoHalHmac(&key, NULL, &message, &signature), HKS_ERROR_INVALID_ARGUMENT);

    signature = { .size = 1, .data = buff };
    ASSERT_EQ(HksCryptoHalHmac(&key, NULL, &message, &signature), HKS_ERROR_INVALID_DIGEST);
}
}  // namespace UnitTest
}  // namespace Huks
}  // namespace Security
}  // namespace OHOS