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

#include "hks_cross_test_common.h"

#include <gtest/gtest.h>

int32_t Unittest::CrossTest::HksCrossTestGenerateKey(const struct HksBlob *keyAlias, struct HksParam *genParams,
    const uint32_t paramCnt, struct HksParamSet *genParamSet)
{
    if (InitParamSet(&genParamSet, genParams, paramCnt) != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HksGenerateKey(keyAlias, genParamSet, nullptr);
}

int32_t Unittest::CrossTest::HksCrossTestImportKey(const struct HksBlob *keyAlias, struct HksParamSet *firstParamSet,
    struct HksParamSet *secondParamSet, struct HksBlob *newKeyAlias)
{
    uint8_t tmpPublicKey[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob publicKey = { Unittest::CrossTest::CROSS_COMMON_SIZE, (uint8_t *)tmpPublicKey };
    if (HksExportPublicKey(keyAlias, firstParamSet, &publicKey) != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HksImportKey(newKeyAlias, secondParamSet, &publicKey);
}

int32_t Unittest::CrossTest::HksCrossTestAesEncrypt(const struct HksBlob *keyAlias,
    const struct HksParamSet *encryptParamSet, const struct HksBlob *inData, struct HksBlob *cipherText)
{
    uint8_t tmpHandle[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), tmpHandle };
    int32_t ret = HksInit(keyAlias, encryptParamSet, &handle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("AesEncry Init failed.");
        return HKS_FAILURE;
    }

    ret = TestUpdateLoopFinish(&handle, encryptParamSet, inData, cipherText);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("AesEncry TestUpdateLoopFinish failed.");
        return HKS_FAILURE;
    }

    ret = HksMemCmp(inData->data, cipherText->data, cipherText->size);
    if (ret == HKS_SUCCESS) {
        HKS_LOG_E("AesEncry inData equals cipherText.");
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestAesDecrypt(const struct HksBlob *keyAlias,
    const struct HksParamSet *decryptParamSet, const struct HksBlob *cipherTest, struct HksBlob *plainText)
{
    uint8_t tmpHandle[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), tmpHandle };
    int32_t ret = HksInit(keyAlias, decryptParamSet, &handle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("AesDecry Init failed.");
        return HKS_FAILURE;
    }

    ret = TestUpdateLoopFinish(&handle, decryptParamSet, cipherTest, plainText);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("AesDecry TestUpdateLoopFinish failed.");
        return HKS_FAILURE;
    }

    ret = HksMemCmp((uint8_t *)g_inData.c_str(), plainText->data, plainText->size);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("AesEncry inData not equals plainText.");
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestRsaEncrypt(const struct HksBlob *keyAlias,
    const struct HksParamSet *encryptParamSet, const struct HksBlob *inData, struct HksBlob *cipherText)
{
    uint8_t tmpHandle[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), tmpHandle };
    int32_t ret = HksInit(keyAlias, encryptParamSet, &handle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("RsaEncry Init failed.");
        return HKS_FAILURE;
    }

    ret = TestUpdateFinish(&handle, encryptParamSet, inData, cipherText);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("RsaEncry TestUpdateFinish failed.");
        return HKS_FAILURE;
    }

    ret = HksMemCmp(inData->data, cipherText->data, cipherText->size);
    if (ret == HKS_SUCCESS) {
        HKS_LOG_E("RsaEncry inData equals cipherText.");
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestRsaDecrypt(const struct HksBlob *keyAlias,
    const struct HksParamSet *decryptParamSet, const struct HksBlob *cipherTest, struct HksBlob *plainText)
{
    uint8_t tmpHandle[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), tmpHandle };
    int32_t ret = HksInit(keyAlias, decryptParamSet, &handle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("RsaDecry Init failed.");
        return HKS_FAILURE;
    }

    ret = TestUpdateFinish(&handle, decryptParamSet, cipherTest, plainText);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("RsaDecry TestUpdateFinish failed.");
        return HKS_FAILURE;
    }

    ret = HksMemCmp((uint8_t *)g_inData.c_str(), plainText->data, plainText->size);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("RsaDecry inData not equals plainText.");
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestSignVerify(const struct HksBlob *keyAlias,
    const struct HksParamSet *signVerifyParamSet, const struct HksBlob *inData, struct HksBlob *signedData)
{
    uint8_t tmpHandle[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), tmpHandle };
    int32_t ret = HksInit(keyAlias, signVerifyParamSet, &handle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Sign/verify Init failed.");
        return HKS_FAILURE;
    }

    ret = TestUpdateFinish(&handle, signVerifyParamSet, inData, signedData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Sign/verify TestUpdateFinish failed.");
        return HKS_FAILURE;
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestHmac(const struct HksBlob *keyAlias, const struct HksParamSet* hmacParamSet)
{
    struct HksBlob inData = {
        Unittest::CrossTest::g_inData.length(),
        (uint8_t *)Unittest::CrossTest::g_inData.c_str()
    };

    uint8_t tmpMac[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob mac = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpMac };
    int32_t ret = HksMac(keyAlias, hmacParamSet, &inData, &mac);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksMac failed.");
        return HKS_FAILURE;
    }

    uint8_t handle[sizeof(uint64_t)] = {0};
    struct HksBlob handleHMAC = { sizeof(uint64_t), handle };
    ret = HksInit(keyAlias, hmacParamSet, &handleHMAC);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksInit failed.");
        return HKS_FAILURE;
    }

    uint8_t out[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outData = { Unittest::CrossTest::CROSS_COMMON_SIZE, out };
    ret = TestUpdateFinish(&handleHMAC, hmacParamSet, &inData, &outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("TestUpdateFinish failed.");
        return HKS_FAILURE;
    }

    ret = HksMemCmp(mac.data, outData.data, outData.size);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("outData not equals macData.");
        return HKS_FAILURE;
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestAgreeGenerate(const struct HksBlob *keyAlias01,
    const struct HksBlob *keyAlias02, struct HksParamSet *genParamSet, const struct HksParam *genParams,
    const uint32_t genParamsCnt)
{
    int32_t ret = InitParamSet(&genParamSet, genParams, genParamsCnt);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("InitParamSet failed.");
        return HKS_FAILURE;
    }

    ret = HksGenerateKey(keyAlias01, genParamSet, nullptr);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksGenerateKey01 failed.");
        return HKS_FAILURE;
    }

    ret = HksGenerateKey(keyAlias02, genParamSet, nullptr);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksGenerateKey02 failed.");
        return HKS_FAILURE;
    }

    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestAgreeExport(const struct HksBlob *keyAlias01, const struct HksBlob *keyAlias02,
    struct HksBlob *publicKey01, struct HksBlob *publicKey02, const struct HksParamSet *genParamSet)
{
    int32_t ret = HksExportPublicKey(keyAlias01, genParamSet, publicKey01);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksExportPublicKey02 failed.");
        return HKS_FAILURE;
    }

    ret = HksExportPublicKey(keyAlias02, genParamSet, publicKey02);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksExportPublicKey02 failed.");
        return HKS_FAILURE;
    }

    return HKS_SUCCESS;
}

void Unittest::CrossTest::HksCrossTestAgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3, struct HksParamSet *paramSet4, struct HksParamSet *paramSet5)
{
    HksFreeParamSet(&paramSet1);
    HksFreeParamSet(&paramSet2);
    HksFreeParamSet(&paramSet3);
    HksFreeParamSet(&paramSet4);
    HksFreeParamSet(&paramSet5);
}

void Unittest::CrossTest::HksCrossTestAgreeDeleteKey(const struct HksBlob *keyAlias1, const struct HksBlob *keyAlias2)
{
    HksDeleteKey(keyAlias1, nullptr);
    HksDeleteKey(keyAlias2, nullptr);
}

int32_t Unittest::CrossTest::HksCrossTestAgree(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet, const struct HksParamSet *finishParamSet, struct HksBlob *outData)
{
    struct HksBlob inData = {
        Unittest::CrossTest::g_inData.length(),
        (uint8_t *)Unittest::CrossTest::g_inData.c_str()
    };

    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    int32_t ret = HksInit(keyAlias, initParamSet, &handle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksInit failed.");
        return HKS_FAILURE;
    }

    uint8_t outDataU[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outDataUpdate = { Unittest::CrossTest::CROSS_COMMON_SIZE, outDataU };
    ret = HksUpdate(&handle, initParamSet, publicKey, &outDataUpdate);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksUpdate failed.");
        return HKS_FAILURE;
    }

    ret = HksFinish(&handle, finishParamSet, &inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksFinish failed.");
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t Unittest::CrossTest::HksCrossTestDerive(const struct HksBlob *keyAlias, const struct HksParamSet *genParamSet,
    struct HksParamSet *initParamSet, struct HksParamSet *finishParamSet)
{
    struct HksBlob inData = {
        Unittest::CrossTest::g_inData.length(),
        (uint8_t *)Unittest::CrossTest::g_inData.c_str()
    };

    int32_t ret = HksGenerateKey(keyAlias, genParamSet, nullptr);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksGenerateKey failed.");
        return HKS_FAILURE;
    }

    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDerive = { sizeof(uint64_t), handleD };
    ret = HksInit(keyAlias, initParamSet, &handleDerive);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksInit failed.");
        return HKS_FAILURE;
    }

    uint8_t tmpOut[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outDataU = { Unittest::CrossTest::CROSS_COMMON_SIZE, tmpOut };
    ret = HksUpdate(&handleDerive, initParamSet, &inData, &outDataU);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksUpdate failed.");
        return HKS_FAILURE;
    }

    uint8_t outDataD[Unittest::CrossTest::CROSS_COMMON_SIZE] = {0};
    struct HksBlob outData = { Unittest::CrossTest::CROSS_COMMON_SIZE, outDataD };
    ret = HksFinish(&handleDerive, finishParamSet, &inData, &outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksFinish failed.");
        return HKS_FAILURE;
    }

    uint8_t tmpDerive[DERIVE_KEY_SIZE_32] = {0};
    struct HksBlob derivedKey = { DERIVE_KEY_SIZE_32, tmpDerive };
    ret = HksDeriveKey(initParamSet, keyAlias, &derivedKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksDeriveKey failed.");
        return HKS_FAILURE;
    }

    ret = HksMemCmp(derivedKey.data, outData.data, outData.size);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("outData not equals derivedKey.");
        return HKS_FAILURE;
    }

    return HKS_SUCCESS;
}