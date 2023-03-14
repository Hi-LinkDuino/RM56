/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HKS_IPC_SERIALIZATION_H
#define HKS_IPC_SERIALIZATION_H

#include "hks_param.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t GetBlobFromBuffer(struct HksBlob *blob, const struct HksBlob *srcBlob, uint32_t *srcOffset);

int32_t HksGenerateKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSetIn, struct HksBlob *keyOut);

int32_t HksImportKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias, struct HksParamSet **paramSet,
    struct HksBlob *key);

int32_t HksExportPublicKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias, struct HksBlob *key);

int32_t HksGetKeyParamSetUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSet);

int32_t HksSignUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *unsignedData, struct HksBlob *signature);

int32_t HksVerifyUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *unsignedData, struct HksBlob *signature);

int32_t HksEncryptDecryptUnpack(const struct HksBlob *srcData, struct HksBlob *key,
    struct HksParamSet **paramSet, struct HksBlob *inputText, struct HksBlob *outputText);

int32_t HksAgreeKeyUnpack(const struct HksBlob *srcData, struct HksParamSet **paramSet, struct HksBlob *privateKey,
    struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

int32_t HksDeriveKeyUnpack(const struct HksBlob *srcData, struct HksParamSet **paramSet, struct HksBlob *kdfKey,
    struct HksBlob *derivedKey);

int32_t HksHmacUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *inputData, struct HksBlob *mac);

int32_t HksGetKeyInfoListUnpack(const struct HksBlob *srcData, uint32_t *listCount, struct HksKeyInfo **keyInfoList);

int32_t HksGetKeyInfoListPackFromService(struct HksBlob *destData, uint32_t listCount,
    const struct HksKeyInfo *keyInfoList);

int32_t HksCertificateChainUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSet, struct HksBlob *certChainBlob);

int32_t HksWrapKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias, struct HksBlob *targetKeyAlias,
    struct HksParamSet **paramSet, struct HksBlob *wrappedData);

int32_t HksUnwrapKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksBlob *targetKeyAlias, struct HksBlob *wrappedData, struct HksParamSet **paramSet);

int32_t HksTrustCertsUnpack(const struct HksBlob *srcData, struct HksBlob *certChainBlob);

int32_t HksParamSetToParams(const struct HksParamSet *paramSet, struct HksParamOut *outParams, uint32_t cnt);

enum HksTagType GetTagType(enum HksTag tag);

#ifdef __cplusplus
}
#endif

#endif
