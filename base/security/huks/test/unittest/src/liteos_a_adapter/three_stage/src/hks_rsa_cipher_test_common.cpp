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

#include "hks_rsa_cipher_test_common.h"

#include <gtest/gtest.h>

int32_t Unittest::RsaCipher::HksRsaCipherTestCase(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet, const struct HksBlob *inData)
{
    /* 1. Generate Key */
    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Export Public Key */
    uint8_t tmpPublicKey[HKS_RSA_KEY_SIZE_1024] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_1024, (uint8_t *)tmpPublicKey };
    ret = HksExportPublicKey(keyAlias, genParamSet, &publicKey);

    /* 3. Import Key */
    char tmpKey[] = "RSA_Encrypt_Decrypt_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(tmpKey), .data = (uint8_t *)tmpKey };
    ret = HksImportKey(&newKeyAlias, encryptParamSet, &publicKey);

    /* 4. Encrypt Three Stage */
    // Init
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    ret = HksInit(&newKeyAlias, encryptParamSet, &handleEncrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t cipher[Unittest::RsaCipher::RSA_COMMON_SIZE] = {0};
    struct HksBlob cipherText = { Unittest::RsaCipher::RSA_COMMON_SIZE, cipher };
    ret = TestUpdateFinish(&handleEncrypt, encryptParamSet, inData, &cipherText);
    EXPECT_NE(memcmp(inData->data, cipherText.data, inData->size), HKS_SUCCESS) << "cipherText equals inData";
    if (ret != HKS_SUCCESS) {
        int32_t abortRet = HksAbort(&handleEncrypt, encryptParamSet);
        EXPECT_EQ(abortRet, HKS_SUCCESS) << "Abort failed.";
        HksDeleteKey(keyAlias, genParamSet);
        HksDeleteKey(&newKeyAlias, encryptParamSet);
        return ret;
    }

    /* 5. Decrypt Three Stage */
    // Init
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    ret = HksInit(keyAlias, decryptParamSet, &handleDecrypt);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";

    // Update & Finish
    uint8_t plain[Unittest::RsaCipher::RSA_COMMON_SIZE] = {0};
    struct HksBlob plainText = { Unittest::RsaCipher::RSA_COMMON_SIZE, plain };
    ret = TestUpdateFinish(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    EXPECT_EQ(memcmp(inData->data, plainText.data, inData->size), HKS_SUCCESS) << "plainText not equals inData";
    if (ret != HKS_SUCCESS) {
        int32_t abortRet = HksAbort(&handleDecrypt, decryptParamSet);
        EXPECT_EQ(abortRet, HKS_SUCCESS) << "Abort failed.";
        HksDeleteKey(keyAlias, genParamSet);
        HksDeleteKey(&newKeyAlias, encryptParamSet);
        return ret;
    }

    /* 6. Delete Key */
    ret = HksDeleteKey(keyAlias, genParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "DeleteKey failed.";
    ret = HksDeleteKey(&newKeyAlias, encryptParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Delete ImportKey failed.";
    return ret;
}