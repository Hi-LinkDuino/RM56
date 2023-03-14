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

#include "hks_aes_cipher_test_common.h"

#include <gtest/gtest.h>

int32_t Unittest::AesCipher::HksAesCipherTestCaseOther(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet)
{
    struct HksBlob inData = {
        Unittest::AesCipher::g_inData.length(),
        (uint8_t *)Unittest::AesCipher::g_inData.c_str()
    };

    /* 1. Generate Key */
    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. Encrypt Three Stage */
    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(keyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t cipher[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::AesCipher::AES_COMMON_SIZE, cipher };
    ret = TestUpdateLoopFinish(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateLoopFinish failed.";
    EXPECT_NE(memcmp(inData.data, cipherText.data, inData.size), HKS_SUCCESS) << "cipherText equals inData";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }

    /* 3. Decrypt Three Stage */
    // Init
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    ret = HksInit(keyAlias, decryptParamSet, &handleDecrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t plain[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::AesCipher::AES_COMMON_SIZE, plain };
    ret = TestUpdateLoopFinish(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateLoopFinish failed.";
    EXPECT_EQ(memcmp(inData.data, plainText.data, inData.size), HKS_SUCCESS) << "plainText not equals inData";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }

    /* 3. Delete Key */
    ret = HksDeleteKey(keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    return ret;
}

#if defined(_USE_OPENSSL_) && defined(_USE_MBEDTLS_)
int32_t Unittest::AesCipher::HksAESCipherTestCaseCcm(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet)
{
    struct HksBlob inData = {
        Unittest::AesCipher::g_inData.length(),
        (uint8_t *)Unittest::AesCipher::g_inData.c_str()
    };

    /* 1. Generate Key */
    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. Encrypt Three Stage */
    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(keyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t cipher[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::AesCipher::AES_COMMON_SIZE, cipher };
    ret = TestUpdateLoopFinish(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateFinish failed.";
    EXPECT_NE(memcmp(inData.data, cipherText.data, inData.size), HKS_SUCCESS) << "cipherText equals inData";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }
    cipherText.size -= Unittest::AesCipher::AEAD_SIZE;

    /* 3. Decrypt Three Stage */
    // Init
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    ret = HksInit(keyAlias, decryptParamSet, &handleDecrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t plain[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::AesCipher::AES_COMMON_SIZE, plain };
    ret = TestUpdateLoopFinish(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateFinish failed.";
    EXPECT_EQ(memcmp(inData.data, plainText.data, inData.size), HKS_SUCCESS) << "plainText not equals inData";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }

    /* 3. Delete Key */
    ret = HksDeleteKey(keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    return ret;
}
#endif

int32_t Unittest::AesCipher::HksAesCipherTestCaseGcm(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet)
{
    struct HksBlob inData = {
        Unittest::AesCipher::g_inData.length(),
        (uint8_t *)Unittest::AesCipher::g_inData.c_str()
    };

    /* 1. Generate Key */
    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. Encrypt Three Stage */
    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(keyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t cipher[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::AesCipher::AES_COMMON_SIZE, cipher };
    ret = TestUpdateLoopFinish(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateLoopFinish failed.";
    EXPECT_NE(memcmp(inData.data, cipherText.data, inData.size), HKS_SUCCESS) << "cipherText equals inData";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }

    cipherText.size -= Unittest::AesCipher::AEAD_SIZE;

    uint32_t i = 0;
    for (i = 0; i < decryptParamSet->paramsCnt; i++) {
        if (decryptParamSet->params[i].tag == HKS_TAG_AE_TAG) {
            uint8_t *tempPtr = cipherText.data;
            (void)memcpy_s(decryptParamSet->params[i].blob.data, Unittest::AesCipher::AEAD_SIZE,
                tempPtr + cipherText.size, Unittest::AesCipher::AEAD_SIZE);
            break;
        }
    }

    /* 3. Decrypt Three Stage */
    // Init
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    ret = HksInit(keyAlias, decryptParamSet, &handleDecrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t plain[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::AesCipher::AES_COMMON_SIZE, plain };
    ret = TestUpdateLoopFinish(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateLoopFinish failed.";
    EXPECT_EQ(memcmp(inData.data, plainText.data, inData.size), HKS_SUCCESS) << "plainText not equals inData";
    if (ret != HKS_SUCCESS) {
        HksDeleteKey(keyAlias, genParamSet);
        return ret;
    }

    /* 3. Delete Key */
    ret = HksDeleteKey(keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    return ret;
}