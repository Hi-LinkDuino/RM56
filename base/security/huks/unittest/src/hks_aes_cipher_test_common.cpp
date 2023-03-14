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

int32_t Unittest::AesCipher::HksAesCipherTestEncrypt(const struct HksBlob *keyAlias,
    const struct HksParamSet *encryptParamSet, const struct HksBlob *inData, struct HksBlob *cipherText)
{
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    int32_t ret = HksInit(keyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = TestUpdateLoopFinish(&handleEncrypt, encryptParamSet, inData, cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateLoopFinish failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    EXPECT_NE(HksMemCmp(inData->data, cipherText->data, inData->size), HKS_SUCCESS) << "cipherText equals inData";

    uint8_t tmpOut[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob outData = { Unittest::AesCipher::AES_COMMON_SIZE, tmpOut };
    ret = HksEncrypt(keyAlias, encryptParamSet, inData, &outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksEncrypt failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    EXPECT_EQ(HksMemCmp(outData.data, cipherText->data, outData.size), HKS_SUCCESS) << "cipherText not equals outData";

    return HKS_SUCCESS;
}

int32_t Unittest::AesCipher::HksAesCipherTestDecrypt(const struct HksBlob *keyAlias,
    const struct HksParamSet *decryptParamSet, const struct HksBlob *cipherText, struct HksBlob *plainText,
    const struct HksBlob *inData)
{
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    int32_t ret = HksInit(keyAlias, decryptParamSet, &handleDecrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = TestUpdateLoopFinish(&handleDecrypt, decryptParamSet, cipherText, plainText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "TestUpdateLoopFinish failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    EXPECT_EQ(HksMemCmp(inData->data, plainText->data, inData->size), HKS_SUCCESS) << "plainText not equals inData";

    uint8_t tmpOut[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob outData = { Unittest::AesCipher::AES_COMMON_SIZE, tmpOut };
    ret = HksDecrypt(keyAlias, decryptParamSet, cipherText, &outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksDecrypt failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    EXPECT_EQ(HksMemCmp(outData.data, plainText->data, outData.size), HKS_SUCCESS) << "plainText not equals outData";

    return HKS_SUCCESS;
}

int32_t Unittest::AesCipher::HksAesCipherTestCaseOther(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet)
{
    char tmpInData[] = "AES_ECB_INDATA_1";
    struct HksBlob inData = {
        Unittest::AesCipher::g_inData.length(),
        (uint8_t *)Unittest::AesCipher::g_inData.c_str()
    };

    struct HksParam *modeParam = nullptr;
    HksGetParam(genParamSet, HKS_TAG_BLOCK_MODE, &modeParam);
    if (modeParam->uint32Param == HKS_MODE_ECB) {
        inData.size = strlen(tmpInData);
        inData.data = (uint8_t *)tmpInData;
    }

    /* 1. Generate Key */
    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. Encrypt */
    uint8_t cipher[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::AesCipher::AES_COMMON_SIZE, cipher };
    ret = HksAesCipherTestEncrypt(keyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAesCipherTestEncrypt failed.";

    /* 3. Decrypt Three Stage */
    uint8_t plain[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::AesCipher::AES_COMMON_SIZE, plain };
    ret = HksAesCipherTestDecrypt(keyAlias, decryptParamSet, &cipherText, &plainText, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAesCipherTestDecrypt failed.";

    /* 3. Delete Key */
    EXPECT_EQ(HksDeleteKey(keyAlias, genParamSet), HKS_SUCCESS) << "DeleteKey failed.";
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
    uint8_t cipher[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::AesCipher::AES_COMMON_SIZE, cipher };
    ret = HksAesCipherTestEncrypt(keyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAesCipherTestEncrypt failed.";
    cipherText.size -= Unittest::AesCipher::AEAD_SIZE;

    /* 3. Decrypt Three Stage */
    uint8_t plain[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::AesCipher::AES_COMMON_SIZE, plain };
    ret = HksAesCipherTestDecrypt(keyAlias, decryptParamSet, &cipherText, &plainText, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAesCipherTestDecrypt failed.";

    /* 3. Delete Key */
    EXPECT_EQ(HksDeleteKey(keyAlias, genParamSet), HKS_SUCCESS) << "DeleteKey failed.";
    return ret;
}
#endif

int32_t Unittest::AesCipher::HksAesCipherTestCaseGcm(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet, struct HksParamSet *decrypt1ParamSet)
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
    uint8_t cipher[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::AesCipher::AES_COMMON_SIZE, cipher };
    ret = HksAesCipherTestEncrypt(keyAlias, encryptParamSet, &inData, &cipherText);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksAesCipherTestEncrypt failed.";

    uint8_t plain1[Unittest::AesCipher::AES_COMMON_SIZE] = {0};
    struct HksBlob plainText1 = { Unittest::AesCipher::AES_COMMON_SIZE, plain1 };
    ret = HksDecrypt(keyAlias, decrypt1ParamSet, &cipherText, &plainText1);
    EXPECT_EQ(ret, HKS_SUCCESS) << "HksDecrypt failed.";

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
    EXPECT_EQ(HksMemCmp(inData.data, plainText.data, inData.size), HKS_SUCCESS) << "plainText not equals inData";
    EXPECT_EQ(HksMemCmp(plainText1.data, plainText.data, plainText1.size), HKS_SUCCESS) << "plainText != plainText1";

    /* 3. Delete Key */
    EXPECT_EQ(HksDeleteKey(keyAlias, genParamSet), HKS_SUCCESS) << "DeleteKey failed.";
    return ret;
}