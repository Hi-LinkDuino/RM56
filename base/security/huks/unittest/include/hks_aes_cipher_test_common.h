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

#ifndef HKS_AES_CIPHER_TEST_COMMON_H
#define HKS_AES_CIPHER_TEST_COMMON_H

#include <string>
#include "hks_test_common.h"
namespace Unittest::AesCipher {
static const uint32_t AES_COMMON_SIZE = 1024;
static const uint32_t IV_SIZE = 16;
static const uint32_t AAD_SIZE = 16;
static const uint32_t NONCE_SIZE = 12;
static const uint32_t AEAD_SIZE = 16;

static uint8_t IV[IV_SIZE] = {0};
static uint8_t AAD[AAD_SIZE] = {0};
static uint8_t NONCE[NONCE_SIZE] = {0};
static uint8_t AEAD[AEAD_SIZE] = {0};

static const std::string g_inData = "Hks_AES_Cipher_Test_000000000000000000000000000000000000000000000000000000000000"
                                    "00000000000000000000000000000000000000000000000000000000000000000000000000000000"
                                    "0000000000000000000000000000000000000000000000000000000000000000000000000_string";

int32_t HksAesCipherTestEncrypt(const struct HksBlob *keyAlias, const struct HksParamSet *encryptParamSet,
    const struct HksBlob *inData, struct HksBlob *cipherText);
int32_t HksAesCipherTestDecrypt(const struct HksBlob *keyAlias, const struct HksParamSet *decryptParamSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText, const struct HksBlob *inData);
int32_t HksAesCipherTestCaseOther(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet);
int32_t HksAESCipherTestCaseCcm(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet);
int32_t HksAesCipherTestCaseGcm(const struct HksBlob *keyAlias, struct HksParamSet *genParamSet,
    struct HksParamSet *encryptParamSet, struct HksParamSet *decryptParamSet, struct HksParamSet *decrypt1ParamSet);
} // namespace Unittest::AesCipher
#endif // HKS_AES_CIPHER_TEST_COMMON_H
