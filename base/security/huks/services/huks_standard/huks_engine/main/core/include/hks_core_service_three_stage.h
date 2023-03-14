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

#ifndef HKS_CORE_SERVICE_THREE_STAGE_H
#define HKS_CORE_SERVICE_THREE_STAGE_H

#include "hks_type_inner.h"
#include "hks_keynode.h"
#include "hks_crypto_hal.h"

#define MAX_HASH_SIZE   64
#define TEMP_DATA_SIZE 64
#define MAX_UPDATE_SIZE (64 * 1024)
#define MAX_TOTAL_SIZE  (1024 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksCoreSignVerifyThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreSignVerifyThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg);

int32_t HksCoreSignVerifyThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreSignVerifyThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreCryptoThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreCryptoThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreEncryptThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreCryptoThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreDecryptThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreDeriveThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreDeriveThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg);

int32_t HksCoreDeriveThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreDeriveThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreAgreeThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreAgreeThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg);

int32_t HksCoreAgreeThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreAgreeThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreMacThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

int32_t HksCoreMacThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg);

int32_t HksCoreMacThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);

int32_t HksCoreMacThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg);

#ifdef __cplusplus
}
#endif

#endif /* HKS_CORE_SERVICE_H */
