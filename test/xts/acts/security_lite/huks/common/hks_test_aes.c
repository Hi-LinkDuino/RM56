/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hks_test_aes.h"
#include "hks_api.h"
#include "hks_param.h"
#include "hks_test_common.h"
#include "hks_test_log.h"

#define TEST_PLAIN_TEST "This is a plain text! Hello world and thanks for watching AES^^"
#define TEST_AES_12 12
#define TEST_AES_16 16
#define TEST_AES_32 32
#define TEST_AES_128 128
#define TEST_AES_256 256

static uint8_t g_buffer[TEST_AES_256];
static uint32_t g_bufferSize = TEST_AES_256;

uint8_t g_nonce[TEST_AES_12] = "hahahahahah";
uint8_t g_aad[TEST_AES_16] = "bababababababab";
uint8_t g_iv[TEST_AES_16] = "aabaabaabaabaab";

static int32_t ConstructParamSetEncryptDecryptAesPre(uint32_t mode, uint32_t padding, bool isEncrypt,
    struct HksParamSet **paramSet)
{
    int32_t ret = HksInitParamSet(paramSet);
    if (ret != 0) {
        HKS_TEST_LOG_E("HksAddParamInit failed!\n");
        return ret;
    }

    do {
        struct HksParam algParam = {0};
        algParam.tag = HKS_TAG_ALGORITHM;
        algParam.uint32Param = HKS_ALG_AES;
        ret = HksAddParams(*paramSet, (const struct HksParam *)&algParam, 1);
        if (ret != 0) {
            HKS_TEST_LOG_E("HksAddParam algParam failed!\n");
            break;
        }

        struct HksParam modeParam = {0};
        modeParam.tag = HKS_TAG_BLOCK_MODE;
        modeParam.uint32Param = mode;
        ret = HksAddParams(*paramSet, (const struct HksParam *)&modeParam, 1);
        if (ret != 0) {
            HKS_TEST_LOG_E("HksAddParam algParam failed!\n");
            break;
        }

        struct HksParam padParam = {0};
        padParam.tag = HKS_TAG_PADDING;
        padParam.uint32Param = padding;
        ret = HksAddParams(*paramSet, (const struct HksParam *)&padParam, 1);
        if (ret != 0) {
            HKS_TEST_LOG_E("HksAddParam algParam failed!\n");
            break;
        }

        struct HksParam purposeParam = {0};
        purposeParam.tag = HKS_TAG_PURPOSE;
        if (isEncrypt) {
            purposeParam.uint32Param = HKS_KEY_PURPOSE_ENCRYPT;
        } else {
            purposeParam.uint32Param = HKS_KEY_PURPOSE_DECRYPT;
        }
        ret = HksAddParams(*paramSet, (const struct HksParam *)&purposeParam, 1);
        if (ret != 0) {
            HKS_TEST_LOG_E("HksAddParam algParam failed!\n");
            break;
        }
    } while (0);
    return ret;
}

static int32_t ConstructParamSetEncryptDecryptAesPost(uint32_t mode, struct HksParamSet **paramSet)
{
    int32_t ret;
    do {
        struct HksParam aadParam = {0};
        aadParam.tag = HKS_TAG_ASSOCIATED_DATA;
        aadParam.blob.data = g_aad;
        aadParam.blob.size = sizeof(g_aad);
        ret = HksAddParams(*paramSet, (const struct HksParam *)&aadParam, 1);
        if (ret != 0) {
            HKS_TEST_LOG_E("HksAddParam aadParam failed!\n");
            break;
        }

        if (mode == HKS_MODE_CBC) {
            struct HksParam ivParam = {0};
            ivParam.tag = HKS_TAG_IV;
            ivParam.blob.data = g_iv;
            ivParam.blob.size = sizeof(g_iv);
            ret = HksAddParams(*paramSet, (const struct HksParam *)&ivParam, 1);
            if (ret != 0) {
                HKS_TEST_LOG_E("HksAddParam ivParam failed!\n");
                break;
            }
        } else {
            struct HksParam nonceParam = {0};
            nonceParam.tag = HKS_TAG_NONCE;
            nonceParam.blob.data = g_nonce;
            nonceParam.blob.size = sizeof(g_nonce);
            ret = HksAddParams(*paramSet, (const struct HksParam *)&nonceParam, 1);
            if (ret != 0) {
                HKS_TEST_LOG_E("HksAddParam nonceParam failed!\n");
                break;
            }
        }

        ret = HksBuildParamSet(paramSet);
        if (ret != 0) {
            HKS_TEST_LOG_E("HksAddParamFinal failed!\n");
            break;
        }
    } while (0);

    return ret;
}

static int32_t ConstructParamSetEncryptDecryptAes(uint32_t mode, uint32_t padding, bool isEncrypt,
    struct HksParamSet **paramSet)
{
    int32_t ret =  ConstructParamSetEncryptDecryptAesPre(mode, padding, isEncrypt, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    ret =  ConstructParamSetEncryptDecryptAesPost(mode, paramSet);
    HKS_TEST_ASSERT(ret == 0);
    return ret;
}

#define TEST_KEY_AUTH_ID "This is a test auth id for generate ed25519 key, keyauthIdvalid!"
void GenerateBaseKey(const struct HksBlob *alias)
{
    HKS_TEST_LOG_I("Test_Aes_gen_by_derive BASEKEY!\n");
    struct HksParamSet *paramSet = NULL;
    HKS_TEST_ASSERT(HksInitParamSet(&paramSet) == 0);

    struct HksBlob authId = { strlen(TEST_KEY_AUTH_ID), (uint8_t *)TEST_KEY_AUTH_ID };

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ED25519 },
        { .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = TEST_AES_256 },
        { .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_SIGN | HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = authId },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_AGREE }, /* no use */
    };

    HKS_TEST_ASSERT(HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0])) == 0);
    HKS_TEST_ASSERT(HksBuildParamSet(&paramSet) == 0);
    HKS_TEST_ASSERT(HksGenerateKey(alias, paramSet, NULL) == 0);

    HksFreeParamSet(&paramSet);
}

#define TEST_AES_KEY_BASE_NAME_1 "test_aes_key_by_derive_base_name_ed25519_1"
#define TEST_AES_KEY_BASE_NAME_2 "test_aes_key_by_derive_base_name_ed25519_2"
#define TEST_AES_KEY_BASE_NAME_1_PUBKEY "test_aes_key_by_derive_base_name_ed25519_1_pubkey"
#define TEST_AES_KEY_BASE_NAME_2_PUBKEY "test_aes_key_by_derive_base_name_ed25519_2_pubkey"

#define TEST_AES_KEY_AGREE_NAME1 "test_aes_key_by_agree_name_keyAlias1"
#define TEST_AES_KEY_AGREE_NAME2 "test_aes_key_by_agree_name_keyAlias2"

static void PlainPubKey(const struct HksBlob *baseKey, const struct HksBlob *peerPubKey,
    struct HksParamSet *paramSet)
{
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = TEST_AES_256 },
        { .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_AGREE },
        { .tag = HKS_TAG_AGREE_PRIVATE_KEY_ALIAS,
            .blob = *baseKey },
        { .tag = HKS_TAG_AGREE_PUBLIC_KEY,
            .blob = *peerPubKey },
        { .tag = HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS,
            .boolParam = false },
        { .tag = HKS_TAG_AGREE_ALG,
            .uint32Param = HKS_ALG_ED25519 },
    };

    HKS_TEST_ASSERT(HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0])) == 0);
}

static void SetKeyAliasTrue(const struct HksBlob *baseKey, const struct HksBlob *peerPubKey,
    struct HksParamSet *paramSet)
{
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = TEST_AES_256 },
        { .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_AGREE },
        { .tag = HKS_TAG_AGREE_PRIVATE_KEY_ALIAS,
            .blob = *baseKey },
        { .tag = HKS_TAG_AGREE_PUBLIC_KEY_IS_KEY_ALIAS,
            .boolParam = true },
        { .tag = HKS_TAG_AGREE_PUBLIC_KEY,
            .blob = *peerPubKey },
        { .tag = HKS_TAG_AGREE_ALG,
            .uint32Param = HKS_ALG_ED25519 },
    };

    HKS_TEST_ASSERT(HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0])) == 0);
}

static void SetKeyAliasWrong(const struct HksBlob *baseKey, const struct HksBlob *peerPubKey,
    struct HksParamSet *paramSet)
{
    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = TEST_AES_256 },
        { .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_BLOCK_MODE,
            .uint32Param = HKS_MODE_GCM },
        { .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_AGREE },
        { .tag = HKS_TAG_AGREE_PRIVATE_KEY_ALIAS,
            .blob = *baseKey },
        { .tag = HKS_TAG_AGREE_PUBLIC_KEY,
            .blob = *peerPubKey },
        { .tag = HKS_TAG_AGREE_ALG,
            .uint32Param = HKS_ALG_ED25519 },
    };

    HKS_TEST_ASSERT(HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0])) == 0);
}

static void GenerateAesAgreeKey(const struct HksBlob *alias, const struct HksBlob *baseKey,
    const struct HksBlob *peerPubKey, bool isPlainPubKey, bool isSetKeyAliasTrue)
{
    HKS_TEST_LOG_I("Test_Aes_gen_by_agree key!\n");
    struct HksParamSet *paramSet = NULL;
    HKS_TEST_ASSERT(HksInitParamSet(&paramSet) == 0);

    if (isPlainPubKey) {
        PlainPubKey(baseKey, peerPubKey, paramSet);
    } else {
        if (isSetKeyAliasTrue) {
            SetKeyAliasTrue(baseKey, peerPubKey, paramSet);
        } else {
            SetKeyAliasWrong(baseKey, peerPubKey, paramSet);
        }
    }
    HKS_TEST_ASSERT(HksBuildParamSet(&paramSet) == 0);
    HKS_TEST_ASSERT(HksGenerateKey(alias, paramSet, NULL) == 0);
    HKS_TEST_LOG_I("End Test_Aes_gen_by_agree key!\n");

    HksFreeParamSet(&paramSet);
}

static void ExportPubKey(const struct HksBlob *alias, struct HksBlob *pubKey)
{
    HKS_TEST_ASSERT(HksExportPublicKey(alias, NULL, pubKey) == 0);
}

static void ImportPubKey(const struct HksBlob *alias, const struct HksBlob *pubKey)
{
    HKS_TEST_LOG_I("Test_Import pubKey!\n");
    struct HksParamSet *paramSet = NULL;
    HKS_TEST_ASSERT(HksInitParamSet(&paramSet) == 0);

    struct HksBlob authId = { strlen(TEST_KEY_AUTH_ID), (uint8_t *)TEST_KEY_AUTH_ID };

    struct HksParam tmpParams[] = {
        { .tag = HKS_TAG_ALGORITHM,
            .uint32Param = HKS_ALG_ED25519 },
        { .tag = HKS_TAG_KEY_SIZE,
            .uint32Param = TEST_AES_256 },
        { .tag = HKS_TAG_PURPOSE,
            .uint32Param = HKS_KEY_PURPOSE_VERIFY },
        { .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA512 },
        { .tag = HKS_TAG_PADDING,
            .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_KEY_AUTH_ID,
            .blob = authId },
        { .tag = HKS_TAG_KEY_GENERATE_TYPE,
            .uint32Param = HKS_KEY_GENERATE_TYPE_DEFAULT }, /* no use */
    };

    HKS_TEST_ASSERT(HksAddParams(paramSet, tmpParams, sizeof(tmpParams) / sizeof(tmpParams[0])) == 0);
    HKS_TEST_ASSERT(HksBuildParamSet(&paramSet) == 0);
    HKS_TEST_ASSERT(HksImportKey(alias, paramSet, pubKey) == 0);

    HksFreeParamSet(&paramSet);
}

#define TEST_AES_KEY_BASE_NAME "test_aes_key_by_derive_base_name_ed25519"
#define TEST_AES_KEY_DERIVE_NAME1 "test_aes_key_by_derive_name_keyAlias1"
#define TEST_AES_KEY_DERIVE_NAME2 "test_aes_key_by_derive_name_keyAlias2"
int32_t TestAes256ByAgree()
{
    HKS_TEST_LOG_I("enter");
    /* generate ed25519 key */
    struct HksBlob baseKeyAlias1 = { strlen(TEST_AES_KEY_BASE_NAME_1), (uint8_t *)TEST_AES_KEY_BASE_NAME_1 };
    GenerateBaseKey(&baseKeyAlias1);

    struct HksBlob baseKeyAlias2 = { strlen(TEST_AES_KEY_BASE_NAME_2), (uint8_t *)TEST_AES_KEY_BASE_NAME_2 };
    GenerateBaseKey(&baseKeyAlias2);

    uint8_t pubKey1[TEST_AES_128] = {0};
    uint8_t pubKey2[TEST_AES_128] = {0};
    struct HksBlob pubKeyBlob1 = { TEST_AES_128, pubKey1 };
    struct HksBlob pubKeyBlob2 = { TEST_AES_128, pubKey2 };
    ExportPubKey(&baseKeyAlias1, &pubKeyBlob1);
    ExportPubKey(&baseKeyAlias2, &pubKeyBlob2);

    struct HksBlob pubKeyAlias1 = {
        strlen(TEST_AES_KEY_BASE_NAME_1_PUBKEY), (uint8_t *)TEST_AES_KEY_BASE_NAME_1_PUBKEY
    };
    struct HksBlob pubKeyAlias2 = {
        strlen(TEST_AES_KEY_BASE_NAME_2_PUBKEY), (uint8_t *)TEST_AES_KEY_BASE_NAME_2_PUBKEY
    };
    ImportPubKey(&pubKeyAlias1, &pubKeyBlob1);
    ImportPubKey(&pubKeyAlias2, &pubKeyBlob2);

    /* generate aes key 1 */
    struct HksBlob aesKeyAlias1 = { strlen(TEST_AES_KEY_AGREE_NAME1), (uint8_t *)TEST_AES_KEY_AGREE_NAME1 };
    GenerateAesAgreeKey(&aesKeyAlias1, &baseKeyAlias1, &pubKeyAlias2, false, false);

    /* generate aes key 2 */
    struct HksBlob aesKeyAlias2 = { strlen(TEST_AES_KEY_DERIVE_NAME2), (uint8_t *)TEST_AES_KEY_DERIVE_NAME2 };
    GenerateAesAgreeKey(&aesKeyAlias2, &baseKeyAlias2, &pubKeyAlias1, false, false);

    /* encrypt by aes key 1 */
    struct HksParamSet *paramSet = NULL;
    int32_t ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, true, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob plainText1 = { strlen(TEST_PLAIN_TEST) + 1, (uint8_t*)TEST_PLAIN_TEST };
    struct HksBlob cipherText1 = { TEST_AES_256, g_buffer };
    (void)memset_s(cipherText1.data, cipherText1.size, 0, cipherText1.size);
    HKS_TEST_ASSERT(HksEncrypt(&aesKeyAlias1, paramSet, &plainText1, &cipherText1) == 0);
    g_bufferSize = cipherText1.size;

    HksFreeParamSet(&paramSet);

    /* decrypt by aes key 2 */
    ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, false, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob cipherText = { g_bufferSize, g_buffer };
    uint8_t tmp[TEST_AES_256] = {0};
    struct HksBlob plainText = { TEST_AES_256, tmp };
    ret = HksDecrypt(&aesKeyAlias2, paramSet, &cipherText, &plainText);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_LOG_I("ConstructParamSetEncryptDecryptAes plainText: %s", plainText.data);
    HksFreeParamSet(&paramSet);

    HksDeleteKey(&baseKeyAlias1, NULL);
    HksDeleteKey(&baseKeyAlias2, NULL);
    HksDeleteKey(&pubKeyAlias1, NULL);
    HksDeleteKey(&pubKeyAlias2, NULL);
    HksDeleteKey(&aesKeyAlias1, NULL);
    HksDeleteKey(&aesKeyAlias2, NULL);
    HKS_TEST_LOG_I("end");
    return ret;
}

int32_t TestAes256ByAgree1()
{
    HKS_TEST_LOG_I("enter");

    /* generate ed25519 key */
    struct HksBlob baseKeyAlias1 = { strlen(TEST_AES_KEY_BASE_NAME_1), (uint8_t *)TEST_AES_KEY_BASE_NAME_1 };
    GenerateBaseKey(&baseKeyAlias1);

    struct HksBlob baseKeyAlias2 = { strlen(TEST_AES_KEY_BASE_NAME_2), (uint8_t *)TEST_AES_KEY_BASE_NAME_2 };
    GenerateBaseKey(&baseKeyAlias2);

    uint8_t pubKey1[TEST_AES_128] = {0};
    uint8_t pubKey2[TEST_AES_128] = {0};
    struct HksBlob pubKeyBlob1 = { TEST_AES_128, pubKey1 };
    struct HksBlob pubKeyBlob2 = { TEST_AES_128, pubKey2 };
    ExportPubKey(&baseKeyAlias1, &pubKeyBlob1);
    ExportPubKey(&baseKeyAlias2, &pubKeyBlob2);

    struct HksBlob pubKeyAlias1 = {
        strlen(TEST_AES_KEY_BASE_NAME_1_PUBKEY), (uint8_t *)TEST_AES_KEY_BASE_NAME_1_PUBKEY
    };
    struct HksBlob pubKeyAlias2 = {
        strlen(TEST_AES_KEY_BASE_NAME_2_PUBKEY), (uint8_t *)TEST_AES_KEY_BASE_NAME_2_PUBKEY
    };
    ImportPubKey(&pubKeyAlias1, &pubKeyBlob1);
    ImportPubKey(&pubKeyAlias2, &pubKeyBlob2);

    /* generate aes key 1 */
    struct HksBlob aesKeyAlias1 = { strlen(TEST_AES_KEY_AGREE_NAME1), (uint8_t *)TEST_AES_KEY_AGREE_NAME1 };
    GenerateAesAgreeKey(&aesKeyAlias1, &baseKeyAlias1, &pubKeyAlias2, false, true);

    /* generate aes key 2 */
    struct HksBlob aesKeyAlias2 = { strlen(TEST_AES_KEY_DERIVE_NAME2), (uint8_t *)TEST_AES_KEY_DERIVE_NAME2 };
    GenerateAesAgreeKey(&aesKeyAlias2, &baseKeyAlias2, &pubKeyAlias1, false, true);

    /* encrypt by aes key 1 */
    struct HksParamSet *paramSet = NULL;
    int32_t ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, true, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob plainText1 = { strlen(TEST_PLAIN_TEST) + 1, (uint8_t*)TEST_PLAIN_TEST };
    struct HksBlob cipherText1 = { TEST_AES_256, g_buffer };
    (void)memset_s(cipherText1.data, cipherText1.size, 0, cipherText1.size);
    HKS_TEST_ASSERT(HksEncrypt(&aesKeyAlias1, paramSet, &plainText1, &cipherText1) == 0);
    g_bufferSize = cipherText1.size;

    HksFreeParamSet(&paramSet);

    /* decrypt by aes key 2 */
    ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, false, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob cipherText = { g_bufferSize, g_buffer };
    uint8_t tmp[TEST_AES_256] = {0};
    struct HksBlob plainText = { TEST_AES_256, tmp };
    ret = HksDecrypt(&aesKeyAlias2, paramSet, &cipherText, &plainText);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_LOG_I("ConstructParamSetEncryptDecryptAes plainText: %s", plainText.data);
    HksFreeParamSet(&paramSet);

    HksDeleteKey(&baseKeyAlias1, NULL);
    HksDeleteKey(&baseKeyAlias2, NULL);
    HksDeleteKey(&pubKeyAlias1, NULL);
    HksDeleteKey(&pubKeyAlias2, NULL);
    HksDeleteKey(&aesKeyAlias1, NULL);
    HksDeleteKey(&aesKeyAlias2, NULL);
    HKS_TEST_LOG_I("end");
    return ret;
}

int32_t TestAes256ByAgree2()
{
    HKS_TEST_LOG_I("enter");
    /* generate ed25519 key */
    struct HksBlob baseKeyAlias1 = { strlen(TEST_AES_KEY_BASE_NAME_1), (uint8_t *)TEST_AES_KEY_BASE_NAME_1 };
    GenerateBaseKey(&baseKeyAlias1);

    struct HksBlob baseKeyAlias2 = { strlen(TEST_AES_KEY_BASE_NAME_2), (uint8_t *)TEST_AES_KEY_BASE_NAME_2 };
    GenerateBaseKey(&baseKeyAlias2);

    uint8_t pubKey1[TEST_AES_128] = {0};
    uint8_t pubKey2[TEST_AES_128] = {0};
    struct HksBlob pubKeyBlob1 = { TEST_AES_128, pubKey1 };
    struct HksBlob pubKeyBlob2 = { TEST_AES_128, pubKey2 };
    ExportPubKey(&baseKeyAlias1, &pubKeyBlob1);
    ExportPubKey(&baseKeyAlias2, &pubKeyBlob2);

    /* generate aes key 1 */
    struct HksBlob aesKeyAlias1 = { strlen(TEST_AES_KEY_AGREE_NAME1), (uint8_t *)TEST_AES_KEY_AGREE_NAME1 };
    GenerateAesAgreeKey(&aesKeyAlias1, &baseKeyAlias1, &pubKeyBlob2, true, false);

    /* generate aes key 2 */
    struct HksBlob aesKeyAlias2 = { strlen(TEST_AES_KEY_DERIVE_NAME2), (uint8_t *)TEST_AES_KEY_DERIVE_NAME2 };
    GenerateAesAgreeKey(&aesKeyAlias2, &baseKeyAlias2, &pubKeyBlob1, true, false);

    /* encrypt by aes key 1 */
    struct HksParamSet *paramSet = NULL;
    int32_t ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, true, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob plainText1 = { strlen(TEST_PLAIN_TEST) + 1, (uint8_t*)TEST_PLAIN_TEST };
    struct HksBlob cipherText1 = { TEST_AES_256, g_buffer };
    (void)memset_s(cipherText1.data, cipherText1.size, 0, cipherText1.size);
    HKS_TEST_ASSERT(HksEncrypt(&aesKeyAlias1, paramSet, &plainText1, &cipherText1) == 0);
    g_bufferSize = cipherText1.size;

    HksFreeParamSet(&paramSet);

    /* decrypt by aes key 2 */
    ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, false, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    struct HksBlob cipherText = { g_bufferSize, g_buffer };
    uint8_t tmp[TEST_AES_256] = {0};
    struct HksBlob plainText = { TEST_AES_256, tmp };
    ret = HksDecrypt(&aesKeyAlias2, paramSet, &cipherText, &plainText);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_LOG_I("ConstructParamSetEncryptDecryptAes plainText: %s", plainText.data);
    HksFreeParamSet(&paramSet);

    HksDeleteKey(&baseKeyAlias1, NULL);
    HksDeleteKey(&baseKeyAlias2, NULL);
    HksDeleteKey(&aesKeyAlias1, NULL);
    HksDeleteKey(&aesKeyAlias2, NULL);
    HKS_TEST_LOG_I("end");
    return ret;
}

int32_t TestAes256ByLocal()
{
    HKS_TEST_LOG_I("enter");

    /* generate aes key 1 */
    char testKey[TEST_AES_32];
    struct HksBlob keyBlob;

    for (int i = 0; i < TEST_AES_32; ++i) {
        testKey[i] = i + TEST_AES_32;
    }

    keyBlob.data = (uint8_t *)(testKey);
    keyBlob.size = sizeof(testKey);

    /* encrypt by aes key 1 */
    struct HksParamSet *paramSet = NULL;
    struct HksParam algParam = {
        .tag = HKS_TAG_IS_KEY_ALIAS,
        .uint32Param = 0
    };

    int32_t ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, true, &paramSet);
    HKS_TEST_ASSERT(ret == 0);

    ret = HksAddParams(paramSet, (const struct HksParam *)&algParam, 1);
    if (ret != 0) {
        HKS_TEST_LOG_E("HksAddParam algParam failed!\n");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    struct HksBlob plainText1 = { strlen(TEST_PLAIN_TEST) + 1, (uint8_t*)TEST_PLAIN_TEST };
    struct HksBlob cipherText1 = { TEST_AES_256, g_buffer };
    (void)memset_s(cipherText1.data, cipherText1.size, 0, cipherText1.size);
    HKS_TEST_ASSERT(HksEncrypt(&keyBlob, paramSet, &plainText1, &cipherText1) == 0);
    g_bufferSize = cipherText1.size;

    HksFreeParamSet(&paramSet);

    /* decrypt by aes key 2 */
    ret = ConstructParamSetEncryptDecryptAes(HKS_MODE_GCM, HKS_PADDING_NONE, false, &paramSet);
    HKS_TEST_ASSERT(ret == 0);
    algParam.tag = HKS_TAG_IS_KEY_ALIAS;
    algParam.uint32Param = 0;
    ret = HksAddParams(paramSet, (const struct HksParam *)&algParam, 1);
    if (ret != 0) {
        HKS_TEST_LOG_E("HksAddParam algParam failed!\n");
        HksFreeParamSet(&paramSet);
        return ret;
    }

    struct HksBlob cipherText = { g_bufferSize, g_buffer };
    uint8_t tmp[TEST_AES_256] = {0};
    struct HksBlob plainText = { TEST_AES_256, tmp };
    ret = HksDecrypt(&keyBlob, paramSet, &cipherText, &plainText);
    HKS_TEST_ASSERT(ret == 0);
    HKS_TEST_ASSERT(memcmp(plainText.data, plainText1.data, plainText.size) == 0);
    HksFreeParamSet(&paramSet);

    HKS_TEST_LOG_I("end");
    return ret;
}

