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

#ifndef HKS_CRYPTO_CHECK_H
#define HKS_CRYPTO_CHECK_H

#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksCheckIpcGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn);

int32_t HksCheckIpcImportKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key);

int32_t HksCheckIpcExportPublicKey(const struct HksBlob *keyAlias, const struct HksBlob *key);

int32_t HksCheckIpcGetKeyParamSet(const struct HksBlob *keyAlias, struct HksParamSet *paramSet);

int32_t HksCheckIpcAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, const struct HksBlob *agreedKey);

int32_t HksCheckIpcDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    const struct HksBlob *derivedKey);

int32_t HksCheckIpcGetKeyInfoList(const struct HksKeyInfo *keyInfoList, uint32_t listCount);

int32_t HksCheckIpcCertificateChain(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksCertChain *certChain);

int32_t HksCheckIpcWrapUnwrapKey(bool isWrapKey, const struct HksBlob *keyAlias,
    const struct HksBlob *targetKeyAlias, const struct HksParamSet *paramSet, const struct HksBlob *wrappedData);

#ifdef __cplusplus
}
#endif

#endif