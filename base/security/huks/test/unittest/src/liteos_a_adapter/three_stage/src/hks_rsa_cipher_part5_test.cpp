/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hks_rsa_cipher_part5_test.h"
#include "hks_rsa_cipher_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
namespace Unittest::RsaCipher {
class HksRsaCipherPart5Test : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void HksRsaCipherPart5Test::SetUpTestCase(void)
{
}

void HksRsaCipherPart5Test::TearDownTestCase(void)
{
}

void HksRsaCipherPart5Test::SetUp()
{
    EXPECT_EQ(HksInitialize(), 0);
}

void HksRsaCipherPart5Test::TearDown()
{
}

static struct HksParam g_genParams041[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_OAEP
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_ECB
    }
};
static struct HksParam g_encryptParams041[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_ENCRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_OAEP
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_ECB
    }
};
static struct HksParam g_decryptParams041[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_OAEP
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA384
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_ECB
    }
};

static struct HksParam g_genParams042[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_OAEP
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_ECB
    }
};
static struct HksParam g_encryptParams042[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_ENCRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_OAEP
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_ECB
    }
};
static struct HksParam g_decryptParams042[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_RSA
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_RSA_KEY_SIZE_4096
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_OAEP
    }, {
        .tag = HKS_TAG_DIGEST,
        .uint32Param = HKS_DIGEST_SHA512
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_ECB
    }
};

/**
 * @tc.name: HksRsaCipherPart5Test.HksRsaCipherPart5Test041
 * @tc.desc: alg-RSA pur-ENCRYPT-DECRYPT size-4096 pad-OAEP dig-SHA384 mode-ECB.
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaCipherPart5Test, HksRsaCipherPart5Test041, TestSize.Level1)
{
    char tmpKeyAlias[] = "HksRSACipherKeyAliasTest041";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksBlob inData = { Unittest::RsaCipher::g_inData_32.length(),
                              (uint8_t *)Unittest::RsaCipher::g_inData_32.c_str() };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams041, sizeof(g_genParams041)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";

    struct HksParamSet *encryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_encryptParams041, sizeof(g_encryptParams041)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(encrypt) failed.";

    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&decryptParamSet, g_decryptParams041, sizeof(g_decryptParams041)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(decrypt) failed.";

    ret = HksRsaCipherTestCase(&keyAlias, genParamSet, encryptParamSet, decryptParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

/**
 * @tc.name: HksRsaCipherPart5Test.HksRsaCipherPart5Test042
 * @tc.desc: alg-RSA pur-ENCRYPT-DECRYPT size-4096 pad-OAEP dig-SHA512 mode-ECB.
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaCipherPart5Test, HksRsaCipherPart5Test042, TestSize.Level1)
{
    char tmpKeyAlias[] = "HksRSACipherKeyAliasTest042";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksBlob inData = { Unittest::RsaCipher::g_inData_32.length(),
                              (uint8_t *)Unittest::RsaCipher::g_inData_32.c_str() };

    struct HksParamSet *genParamSet = nullptr;
    int32_t ret = InitParamSet(&genParamSet, g_genParams042, sizeof(g_genParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";

    struct HksParamSet *encryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_encryptParams042, sizeof(g_encryptParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(encrypt) failed.";

    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&decryptParamSet, g_decryptParams042, sizeof(g_decryptParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(decrypt) failed.";

    ret = HksRsaCipherTestCase(&keyAlias, genParamSet, encryptParamSet, decryptParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "this case failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

/**
 * @tc.name: HksRsaCipherPart5Test.HksRsaCipherPart5Test043
 * @tc.desc: alg-RSA pur-ENCRYPT-DECRYPT size-4096 pad-OAEP dig-SHA512 mode-ECB. Decrypt-Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaCipherPart5Test, HksRsaCipherPart5Test043, TestSize.Level1)
{
    char tmpKeyAlias[] = "HksRSACipherKeyAliasTest043";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksBlob inData = { Unittest::RsaCipher::g_inData_32.length(),
                              (uint8_t *)Unittest::RsaCipher::g_inData_32.c_str() };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams042, sizeof(g_genParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Export Public Key */
    uint8_t tmpPublicKey[HKS_RSA_KEY_SIZE_4096] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_4096, (uint8_t *)tmpPublicKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);

    /* 3. Encrypt Three Stage */
    struct HksParamSet *encryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_encryptParams042, sizeof(g_encryptParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* Import Key */
    char tmpKey[] = "RSA_Encrypt_Decrypt_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksImportKey(&newKeyAlias, encryptParamSet, &publicKey);

    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(&newKeyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update & Finish
    uint8_t cipher[Unittest::RsaCipher::RSA_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::RsaCipher::RSA_COMMON_SIZE, cipher };
    ret = TestUpdateFinish(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    EXPECT_NE(memcmp(inData.data, cipherText.data, inData.size), HKS_SUCCESS) << "cipherText equals inData";

    /* 4. Decrypt Three Stage */
    struct HksParamSet *decryptParamSet = nullptr;
    ret = InitParamSet(&decryptParamSet, g_decryptParams042, sizeof(g_decryptParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";
    // Init
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    ret = HksInit(&keyAlias, decryptParamSet, &handleDecrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleDecrypt, decryptParamSet, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Abort
    ret = HksAbort(&handleDecrypt, decryptParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Abort failed.";

    /* 5. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    ret = HksDeleteKey(&newKeyAlias, encryptParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete ImportKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    HksFreeParamSet(&decryptParamSet);
}

/**
 * @tc.name: HksRsaCipherPart5Test.HksRsaCipherPart5Test044
 * @tc.desc: alg-RSA pur-ENCRYPT-DECRYPT size-4096 pad-OAEP dig-SHA512 mode-ECB. Encrypt-Abort
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaCipherPart5Test, HksRsaCipherPart5Test044, TestSize.Level1)
{
    char tmpKeyAlias[] = "HksRSACipherKeyAliasTest044";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    struct HksBlob inData = { Unittest::RsaCipher::g_inData_32.length(),
                              (uint8_t *)Unittest::RsaCipher::g_inData_32.c_str() };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams042, sizeof(g_genParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Export Public Key */
    uint8_t tmpPublicKey[HKS_RSA_KEY_SIZE_4096] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_4096, (uint8_t *)tmpPublicKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);

    /* 3. Encrypt Three Stage */
    struct HksParamSet *encryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_encryptParams042, sizeof(g_encryptParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* Import Key */
    char tmpKey[] = "RSA_Encrypt_Decrypt_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksImportKey(&newKeyAlias, encryptParamSet, &publicKey);
    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(&newKeyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleEncrypt, encryptParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Abort
    ret = HksAbort(&handleEncrypt, encryptParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Abort failed.";

    /* 4. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    ret = HksDeleteKey(&newKeyAlias, encryptParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete ImportKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
}

/**
 * @tc.name: HksRsaCipherPart5Test.HksRsaCipherPart5Test045
 * @tc.desc: alg-RSA pur-ENCRYPT-DECRYPT size-4096 pad-OAEP dig-SHA512 mode-ECB. abnormal
 * @tc.type: FUNC
 */
HWTEST_F(HksRsaCipherPart5Test, HksRsaCipherPart5Test045, TestSize.Level1)
{
    char tmpKeyAlias[] = "HksRSACipherKeyAliasTest045";
    struct HksBlob keyAlias = { strlen(tmpKeyAlias), (uint8_t *)tmpKeyAlias };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    struct HksParamSet *genParamSet = nullptr;
    ret = InitParamSet(&genParamSet, g_genParams042, sizeof(g_genParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet(gen) failed.";
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Export Public Key */
    uint8_t tmpPublicKey[HKS_RSA_KEY_SIZE_4096] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_4096, (uint8_t *)tmpPublicKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);

    /* 3. Encrypt Three Stage */
    struct HksParamSet *encryptParamSet = nullptr;
    ret = InitParamSet(&encryptParamSet, g_encryptParams042, sizeof(g_encryptParams042)/sizeof(HksParam));
    EXPECT_EQ(ret, HKS_SUCCESS) << "InitParamSet failed.";

    /* Import Key */
    char tmpKey[] = "RSA_Encrypt_Decrypt_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksImportKey(&newKeyAlias, encryptParamSet, &publicKey);
    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(NULL, encryptParamSet, &handleEncrypt);
    EXPECT_NE(ret, HKS_SUCCESS) << "Init should failed.";

    /* 4. Delete Key */
    ret = HksDeleteKey(&keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    ret = HksDeleteKey(&newKeyAlias, encryptParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete ImportKey failed.";

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
}
} // namespace Unittest::RsaCipher