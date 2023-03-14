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

#ifndef HKS_CHECK_PARAMSET_H
#define HKS_CHECK_PARAMSET_H

#include "hks_param.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksCoreCheckGenKeyParams(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *keyIn, const struct HksBlob *keyOut);

int32_t HksCoreCheckImportKeyParams(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, const struct HksBlob *keyOut);

int32_t HksCoreCheckSignVerifyParams(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature);

int32_t HksCoreCheckCipherParams(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, const struct HksBlob *outData);

int32_t HksCoreCheckAgreeKeyParams(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, const struct HksBlob *agreedKey, bool isLocalCheck);

int32_t HksLocalCheckSignVerifyParams(uint32_t cmdId, uint32_t keySize, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature);

int32_t HksLocalCheckCipherParams(uint32_t cmdId, uint32_t keySize, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, const struct HksBlob *outData);

int32_t HksCoreCheckDeriveKeyParams(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    const struct HksBlob *derivedKey, bool isLocalCheck);

int32_t HksCoreCheckMacParams(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *mac, bool isLocalCheck);

#ifdef __cplusplus
}
#endif

#endif /* HKS_CHECK_PARAMSET_H */
