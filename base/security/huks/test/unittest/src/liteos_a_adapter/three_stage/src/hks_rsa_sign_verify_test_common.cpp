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

#include "hks_rsa_sign_verify_test_common.h"

#include <gtest/gtest.h>

int32_t Unittest::RsaSignVerify::HksRsaSignVerifyTestNormalCase(struct HksBlob keyAlias,
    struct HksParamSet *genParamSet, struct HksParamSet *signParamSet, struct HksParamSet *verifyParamSet)
{
    struct HksBlob inData = {
        Unittest::RsaSignVerify::g_inData.length(),
        (uint8_t *)Unittest::RsaSignVerify::g_inData.c_str()
    };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(&keyAlias, signParamSet, &handleSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleSign, signParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    uint8_t outDataS[Unittest::RsaSignVerify::RSA_COMMON_SIZE] = {0};
    struct HksBlob outDataSign = { Unittest::RsaSignVerify::RSA_COMMON_SIZE, outDataS };
    ret = HksFinish(&handleSign, signParamSet, &inData, &outDataSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";

    /* 3. Export Public Key */
    uint8_t pubKey[HKS_RSA_KEY_SIZE_1024] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_1024, pubKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey failed.";

    /* 4. Import Key */
    char newKey[] = "RSA_Sign_Verify_Import_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(newKey), .data = (uint8_t *)newKey };
    ret = HksImportKey(&newKeyAlias, verifyParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ImportKey failed";

    /* 5. Verify Three Stage */
    // Init
    uint8_t handleV[sizeof(uint64_t)] = {0};
    struct HksBlob handleVerify = { sizeof(uint64_t), handleV };
    ret = HksInit(&newKeyAlias, verifyParamSet, &handleVerify);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleVerify, verifyParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    ret = HksFinish(&handleVerify, verifyParamSet, &inData, &outDataSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";

    /* 6. Delete New Key */
    int32_t deleteRet = HksDeleteKey(&newKeyAlias, verifyParamSet);
    EXPECT_EQ(deleteRet, HKS_SUCCESS) << "Delete ImportKey failed.";

    return ret;
}

int32_t Unittest::RsaSignVerify::HksRSASignVerifyTestAbnormalCase(struct HksBlob keyAlias,
    struct HksParamSet *genParamSet, struct HksParamSet *signParamSet, struct HksParamSet *verifyParamSet)
{
    struct HksBlob inData = {
        Unittest::RsaSignVerify::g_inData.length(),
        (uint8_t *)Unittest::RsaSignVerify::g_inData.c_str()
    };
    int32_t ret = HKS_FAILURE;

    /* 1. Generate Key */
    // Generate Key
    ret = HksGenerateKey(&keyAlias, genParamSet, nullptr);
    EXPECT_EQ(ret, HKS_SUCCESS) << "GenerateKey failed.";

    /* 2. Sign Three Stage */
    // Init
    uint8_t handleS[sizeof(uint64_t)] = {0};
    struct HksBlob handleSign = { sizeof(uint64_t), handleS };
    ret = HksInit(&keyAlias, signParamSet, &handleSign);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleSign, signParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    uint8_t outDataS[Unittest::RsaSignVerify::RSA_COMMON_SIZE] = {0};
    struct HksBlob outDataSign = { Unittest::RsaSignVerify::RSA_COMMON_SIZE, outDataS };
    ret = HksFinish(&handleSign, signParamSet, &inData, &outDataSign);
    EXPECT_NE(ret, HKS_SUCCESS) << "Finish should failed.";
    int32_t abortRet = HksAbort(&handleSign, signParamSet);
    EXPECT_EQ(abortRet, HKS_SUCCESS) << "Abort failed.";

    /* 3. Export Public Key */
    uint8_t pubKey[HKS_RSA_KEY_SIZE_1024] = {0};
    struct HksBlob publicKey = { HKS_RSA_KEY_SIZE_1024, pubKey };
    ret = HksExportPublicKey(&keyAlias, genParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey failed.";

    /* 4. Import Key */
    char newKey[] = "RSA_Sign_Verify_Import_KeyAlias";
    struct HksBlob newKeyAlias = { .size = strlen(newKey), .data = (uint8_t *)newKey };
    ret = HksImportKey(&newKeyAlias, verifyParamSet, &publicKey);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ImportKey failed";

    /* 5. Verify Three Stage */
    // Init
    uint8_t handleV[sizeof(uint64_t)] = {0};
    struct HksBlob handleVerify = { sizeof(uint64_t), handleV };
    ret = HksInit(&newKeyAlias, verifyParamSet, &handleVerify);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    // Update loop
    ret = HksTestUpdate(&handleVerify, verifyParamSet, &inData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    // Finish
    ret = HksFinish(&handleVerify, verifyParamSet, &inData, &outDataSign);
    EXPECT_NE(ret, HKS_SUCCESS) << "Finish should failed.";
    abortRet = HksAbort(&handleVerify, verifyParamSet);
    EXPECT_EQ(abortRet, HKS_SUCCESS) << "Abort failed.";

    /* 6. Delete New Key */
    int32_t deleteRet = HksDeleteKey(&newKeyAlias, verifyParamSet);
    EXPECT_EQ(deleteRet, HKS_SUCCESS) << "Delete ImportKey failed.";

    return ret;
}