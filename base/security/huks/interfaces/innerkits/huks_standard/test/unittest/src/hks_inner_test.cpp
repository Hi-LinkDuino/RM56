/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "hks_base_check.h"
#include "hks_check_paramset.h"
#include "hks_client_service_adapter.h"
#include "hks_cmd_id.h"
#include "hks_common_check.h"
#include "hks_crypto_adapter.h"
#include "hks_local_engine.h"
#include "hks_mem.h"

using namespace testing::ext;
namespace {
class HksInnerTest : public testing::Test {};

/**
 * @tc.name: HksInnerTest.HksInnerTest001
 * @tc.desc: CheckPurposeValid HMAC & DSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest001, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    ret = HksCheckGenKeyPurpose(HKS_ALG_HMAC, HKS_KEY_PURPOSE_MAC);
    ASSERT_EQ(ret, HKS_SUCCESS);
    ret = HksCheckGenKeyPurpose(HKS_ALG_DSA, HKS_KEY_PURPOSE_SIGN);
    ASSERT_EQ(ret, HKS_SUCCESS);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest002
 * @tc.desc: InitInputParamsByAlg HMAC & DSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest002, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    ret = HksGetInputParmasByAlg(HKS_ALG_HMAC, HKS_CHECK_TYPE_GEN_KEY, NULL, NULL);
    ret = HksGetInputParmasByAlg(HKS_ALG_DSA, HKS_CHECK_TYPE_GEN_KEY, NULL, NULL);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest003
 * @tc.desc: GetExpectParams HMAC & DSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest003, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    ParamsValues values = {0, 0, 0, 0, 0};
    ret = HksCheckFixedParams(HKS_ALG_HMAC, HKS_CHECK_TYPE_GEN_KEY, &values);
    ret = HksCheckFixedParams(HKS_ALG_DSA, HKS_CHECK_TYPE_GEN_KEY, &values);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest004
 * @tc.desc: CheckAesPadding CTR & ECB mode
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest004, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    ParamsValues values = {
        {0},
        {.needCheck = true, .value = HKS_PADDING_NONE},
        {.needCheck = true, .value = HKS_KEY_PURPOSE_ENCRYPT},
        {0},
        {.needCheck = true, .value = HKS_MODE_CTR},
    };
    ret = HksCheckGenKeyMutableParams(HKS_ALG_AES, &values);
    values.mode = {.needCheck = true, .value = HKS_MODE_ECB};
    ret = HksCheckGenKeyMutableParams(HKS_ALG_AES, &values);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest005
 * @tc.desc: CheckImportMutableParams CTR & ECB mode
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest005, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    ret = CheckImportMutableParams(HKS_ALG_AES, NULL);

    ParamsValues values = {
        {0},
        {.needCheck = true, .value = HKS_PADDING_NONE},
        {.needCheck = true, .value = HKS_KEY_PURPOSE_ENCRYPT},
        {0},
        {0},
    };
    ret = CheckImportMutableParams(HKS_ALG_ECC, &values);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_PURPOSE);

    ret = CheckImportMutableParams(HKS_ALG_RSA, &values);

    values.purpose = {.needCheck = true, .value = HKS_KEY_PURPOSE_VERIFY};
    ret = CheckImportMutableParams(HKS_ALG_RSA, &values);

    values.purpose = {.needCheck = true, .value = HKS_KEY_PURPOSE_SIGN};
    ret = CheckImportMutableParams(HKS_ALG_RSA, &values);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_PURPOSE);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest006
 * @tc.desc: CheckImportKeySize DSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest006, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;

    ParamsValues values = {
        {0},
        {.needCheck = true, .value = HKS_PADDING_NONE},
        {.needCheck = true, .value = HKS_KEY_PURPOSE_ENCRYPT},
        {0},
        {0},
    };
    HksBlob keyAlias = {0};
    HksBlob key = {0};
    HksParamSet *paramSet = nullptr;
    HksBlob keyOut = {0};

    ret = HksInitParamSet(&paramSet);
    ASSERT_EQ(ret, HKS_SUCCESS);

    HksParam algrithom;
    algrithom.tag = HKS_TAG_ALGORITHM;
    algrithom.uint32Param = HKS_ALG_DSA;
    HksAddParams(paramSet, &algrithom, 1);

    HksParam keySize;
    keySize.tag = HKS_TAG_KEY_SIZE;
    keySize.uint32Param = 256;
    HksAddParams(paramSet, &keySize, 1);

    HksParam purpose;
    purpose.tag = HKS_TAG_PURPOSE;
    purpose.uint32Param = HKS_KEY_PURPOSE_SIGN;
    HksAddParams(paramSet, &purpose, 1);

    HksParam digest;
    digest.tag = HKS_TAG_DIGEST;
    digest.uint32Param = HKS_DIGEST_SHA256;
    HksAddParams(paramSet, &digest, 1);

    ret = HksCoreCheckImportKeyParams(&keyAlias, &key, paramSet, &keyOut);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest007
 * @tc.desc: CheckImportKeySize DSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest007, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;

    ParamsValues values = {
        {0},
        {.needCheck = true, .value = HKS_PADDING_NONE},
        {.needCheck = true, .value = HKS_KEY_PURPOSE_ENCRYPT},
        {0},
        {0},
    };
    HksBlob keyAlias = {0};
    HksBlob key = {0};
    HksParamSet *paramSet = nullptr;
    HksBlob keyOut = {0};

    ret = HksInitParamSet(&paramSet);
    ASSERT_EQ(ret, HKS_SUCCESS);

    HksParam algrithom;
    algrithom.tag = HKS_TAG_ALGORITHM;
    algrithom.uint32Param = HKS_ALG_DSA;
    HksAddParams(paramSet, &algrithom, 1);

    HksParam keySize;
    keySize.tag = HKS_TAG_KEY_SIZE;
    keySize.uint32Param = 256;
    HksAddParams(paramSet, &keySize, 1);

    HksParam purpose;
    purpose.tag = HKS_TAG_PURPOSE;
    purpose.uint32Param = HKS_KEY_PURPOSE_SIGN;
    HksAddParams(paramSet, &purpose, 1);

    HksParam digest;
    digest.tag = HKS_TAG_DIGEST;
    digest.uint32Param = HKS_DIGEST_SHA256;
    HksAddParams(paramSet, &digest, 1);

    HksBuildParamSet(&paramSet);

    ret = HksCoreCheckImportKeyParams(&keyAlias, &key, paramSet, &keyOut);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest008
 * @tc.desc: HksLocalCheckCipherParams AES RSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest008, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;

    HksParamSet *paramSet = nullptr;

    HksParam algrithom;
    algrithom.tag = HKS_TAG_ALGORITHM;
    algrithom.uint32Param = HKS_ALG_AES;

    ret = HksInitParamSet(&paramSet);
    ASSERT_EQ(ret, HKS_SUCCESS);
    HksAddParams(paramSet, &algrithom, 1);
    HksBuildParamSet(&paramSet);

    ret = HksLocalCheckCipherParams(HKS_CMD_ID_ENCRYPT, HKS_AES_KEY_SIZE_512, paramSet, nullptr, nullptr);
    HksFreeParamSet(&paramSet);

    ret = HksInitParamSet(&paramSet);
    ASSERT_EQ(ret, HKS_SUCCESS);
    algrithom.uint32Param = HKS_ALG_RSA;
    HksAddParams(paramSet, &algrithom, 1);
    HksBuildParamSet(&paramSet);

    ret = HksLocalCheckCipherParams(HKS_CMD_ID_ENCRYPT, HKS_RSA_KEY_SIZE_1024, paramSet, nullptr, nullptr);
    ret = HksLocalCheckCipherParams(HKS_CMD_ID_ENCRYPT, 0, paramSet, nullptr, nullptr);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest009
 * @tc.desc: HksGetDigestLen sha1 sha224 digest
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest009, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    uint32_t len;
    ret = HksGetDigestLen(HKS_DIGEST_SHA1, &len);
    ret = HksGetDigestLen(HKS_DIGEST_SHA224, &len);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest010
 * @tc.desc: HksFormatKeyFromMaterial AES RSA HMAC DSA ECC algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest010, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    uint32_t len;
    HksBlob keyMaterial = {.size = 0, .data = nullptr};
    ret = HksFormatKeyFromMaterial(HKS_ALG_AES, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_RSA, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_HMAC, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_DSA, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_ECC, &keyMaterial, nullptr);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest011
 * @tc.desc: HksFormatKeyFromMaterial AES RSA HMAC DSA ECC algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest011, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;
    uint32_t len;
    HksBlob keyMaterial = {.size = 0, .data = nullptr};
    ret = HksFormatKeyFromMaterial(HKS_ALG_AES, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_RSA, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_HMAC, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_DSA, &keyMaterial, nullptr);
    ret = HksFormatKeyFromMaterial(HKS_ALG_ECC, &keyMaterial, nullptr);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest012
 * @tc.desc: CheckLocalGenerateKeyParams function
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest012, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;

    HksParamSet *inParamSet = nullptr;
    HksParamSet *outParamSet = nullptr;

    ret = HksInitParamSet(&inParamSet);
    ASSERT_EQ(ret, HKS_SUCCESS);
    ret = HksInitParamSet(&outParamSet);
    ASSERT_EQ(ret, HKS_SUCCESS);

    HksParam keySize;
    keySize.tag = HKS_TAG_KEY_SIZE;
    keySize.uint32Param = 256;
    HksAddParams(inParamSet, &keySize, 1);

    HksParam purpose;
    purpose.tag = HKS_TAG_PURPOSE;
    purpose.uint32Param = HKS_KEY_PURPOSE_SIGN;
    HksAddParams(inParamSet, &purpose, 1);

    HksParam digest;
    digest.tag = HKS_TAG_DIGEST;
    digest.uint32Param = HKS_DIGEST_SHA256;
    HksAddParams(inParamSet, &digest, 1);

    ret = HksLocalGenerateKey(inParamSet, nullptr);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_ARGUMENT);
    ret = HksLocalGenerateKey(inParamSet, outParamSet);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_ARGUMENT);

    HksParam algrithom;
    algrithom.tag = HKS_TAG_ALGORITHM;
    algrithom.uint32Param = HKS_ALG_DSA;
    HksAddParams(inParamSet, &algrithom, 1);

    ret = HksLocalGenerateKey(inParamSet, outParamSet);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest013
 * @tc.desc: CheckLocalCipherParams AES RSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest013, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;

    HksParamSet *inParamSet = nullptr;
    KeyMaterialRsa keyMaterial = {.keyAlg = HKS_ALG_RSA, .keySize = HKS_RSA_KEY_SIZE_512};
    HksBlob blobArg = {.size = sizeof(KeyMaterialRsa), .data = (uint8_t *)&keyMaterial};
    HksParam algrithom;
    algrithom.tag = HKS_TAG_ALGORITHM;
    algrithom.uint32Param = HKS_ALG_DSA;

    ret = HksInitParamSet(&inParamSet);
    ASSERT_EQ(ret, HKS_SUCCESS);
    HksAddParams(inParamSet, &algrithom, 1);
    ret = HksLocalEncrypt(&blobArg, inParamSet, &blobArg, &blobArg);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_ALGORITHM);
    HksFreeParamSet(&inParamSet);

    ret = HksInitParamSet(&inParamSet);
    ASSERT_EQ(ret, HKS_SUCCESS);
    algrithom.uint32Param = HKS_ALG_AES;
    HksAddParams(inParamSet, &algrithom, 1);
    ret = HksLocalEncrypt(&blobArg, inParamSet, &blobArg, &blobArg);
    HksFreeParamSet(&inParamSet);

    ret = HksInitParamSet(&inParamSet);
    ASSERT_EQ(ret, HKS_SUCCESS);
    algrithom.uint32Param = HKS_ALG_RSA;
    HksAddParams(inParamSet, &algrithom, 1);
    ret = HksLocalEncrypt(&blobArg, inParamSet, &blobArg, &blobArg);
    HksFreeParamSet(&inParamSet);
}

/**
 * @tc.name: HksInnerTest.HksInnerTest014
 * @tc.desc: TranslateToX509PublicKey and TranslateFromX509PublicKey DSA algrithom
 * @tc.type: FUNC
 */
HWTEST_F(HksInnerTest, HksInnerTest014, Function | SmallTest | Level0)
{
    int32_t ret = HKS_SUCCESS;

    HksBlob publicKey = {.size = sizeof(KeyMaterialDsa) + 5, .data = (uint8_t *)HksMalloc(sizeof(KeyMaterialDsa) + 5)};
    HksBlob x509Key = {.size = 0, .data = nullptr};
    KeyMaterialDsa *keyMaterial = (KeyMaterialDsa *)publicKey.data;
    (void)memset_s(publicKey.data, publicKey.size, 0x01, publicKey.size);
    keyMaterial->keyAlg = HKS_ALG_DSA;
    keyMaterial->keySize = 256;

    publicKey.size = 20;
    ret = TranslateToX509PublicKey(&publicKey, &x509Key);
    ASSERT_EQ(ret, HKS_ERROR_INVALID_ARGUMENT);

    publicKey.size = sizeof(KeyMaterialDsa) + 5;
    keyMaterial->xSize = 1;
    keyMaterial->ySize = 1;
    keyMaterial->pSize = 1;
    keyMaterial->qSize = 1;
    keyMaterial->gSize = 1;
    ret = TranslateToX509PublicKey(&publicKey, &x509Key);

    ret = TranslateFromX509PublicKey(&x509Key, &publicKey);
}
}  // namespace