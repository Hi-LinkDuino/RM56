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

#include "hks_ecdh_agree_test_common.h"

#include <gtest/gtest.h>

int32_t Unittest::EcdhAgree::HksEcdhAgreeFinish(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet, const struct HksParamSet *finishParamSet, struct HksBlob *outData)
{
    struct HksBlob inData = {
        Unittest::EcdhAgree::g_inData.length(),
        (uint8_t *)Unittest::EcdhAgree::g_inData.c_str()
    };

    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    int32_t ret = HksInit(keyAlias, initParamSet, &handle);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    uint8_t outDataU[Unittest::EcdhAgree::ECDH_COMMON_SIZE] = {0};
    struct HksBlob outDataUpdate = { Unittest::EcdhAgree::ECDH_COMMON_SIZE, outDataU };
    ret = HksUpdate(&handle, initParamSet, publicKey, &outDataUpdate);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    ret = HksFinish(&handle, finishParamSet, &inData, outData);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t Unittest::EcdhAgree::HksEcdhAgreeAbort(const struct HksBlob *keyAlias, const struct HksBlob *publicKey,
    const struct HksParamSet *initParamSet)
{
    uint8_t handleU[sizeof(uint64_t)] = {0};
    struct HksBlob handle = { sizeof(uint64_t), handleU };
    int32_t ret = HksInit(keyAlias, initParamSet, &handle);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Init failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    uint8_t outDataU[Unittest::EcdhAgree::ECDH_COMMON_SIZE] = {0};
    struct HksBlob outDataUpdate = { Unittest::EcdhAgree::ECDH_COMMON_SIZE, outDataU };
    ret = HksUpdate(&handle, initParamSet, publicKey, &outDataUpdate);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Update failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }

    ret = HksAbort(&handle, initParamSet);
    EXPECT_EQ(ret, HKS_SUCCESS) << "Finish failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

int32_t Unittest::EcdhAgree::HksEcdhAgreeExport(const struct HksBlob *keyAlias1, const struct HksBlob *keyAlias2,
    struct HksBlob *publicKey1, struct HksBlob *publicKey2, const struct HksParamSet *genParamSet)
{
    int32_t ret = HksExportPublicKey(keyAlias1, genParamSet, publicKey1);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey01 failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    ret = HksExportPublicKey(keyAlias2, genParamSet, publicKey2);
    EXPECT_EQ(ret, HKS_SUCCESS) << "ExportPublicKey02 failed.";
    if (ret != HKS_SUCCESS) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

void Unittest::EcdhAgree::HksEcdhAgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3)
{
    HksFreeParamSet(&paramSet1);
    HksFreeParamSet(&paramSet2);
    HksFreeParamSet(&paramSet3);
}

void Unittest::EcdhAgree::HksEcdhAgreeFreeParamSet(struct HksParamSet *paramSet1, struct HksParamSet *paramSet2,
    struct HksParamSet *paramSet3, struct HksParamSet *paramSet4, struct HksParamSet *paramSet5)
{
    HksFreeParamSet(&paramSet1);
    HksFreeParamSet(&paramSet2);
    HksFreeParamSet(&paramSet3);
    HksFreeParamSet(&paramSet4);
    HksFreeParamSet(&paramSet5);
}

void Unittest::EcdhAgree::HksEcdhAgreeFreeBlob(struct HksBlob *blob1, struct HksBlob *blob2)
{
    HksFree(blob1->data);
    HksFree(blob2->data);
}

void Unittest::EcdhAgree::HksEcdhAgreeFreeBlob(struct HksBlob *blob1, struct HksBlob *blob2,
    struct HksBlob *blob3, struct HksBlob *blob4)
{
    HksFree(blob1->data);
    HksFree(blob2->data);
    HksFree(blob3->data);
    HksFree(blob4->data);
}