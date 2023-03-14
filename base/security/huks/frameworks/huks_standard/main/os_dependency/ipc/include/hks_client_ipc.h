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

#ifndef HKS_CLIENT_IPC_H
#define HKS_CLIENT_IPC_H

#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksClientInitialize(void);

int32_t HksClientRefreshKeyInfo(void);

int32_t HksClientGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    struct HksParamSet *paramSetOut);

int32_t HksClientImportKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key);

int32_t HksClientExportPublicKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksBlob *key);

int32_t HksClientDeleteKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet);

int32_t HksClientGetKeyParamSet(const struct HksBlob *keyAlias, struct HksParamSet *paramSet);

int32_t HksClientKeyExist(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet);

int32_t HksClientGenerateRandom(struct HksBlob *random, const struct HksParamSet *paramSet);

int32_t HksClientSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature);

int32_t HksClientVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature);

int32_t HksClientEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText);

int32_t HksClientDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText);

int32_t HksClientAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob  *privateKey,
    const struct HksBlob  *peerPublicKey, struct HksBlob *agreedKey);

int32_t HksClientDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    struct HksBlob *derivedKey);

int32_t HksClientMac(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *mac);

int32_t HksClientGetKeyInfoList(struct HksKeyInfo *keyInfoList, uint32_t *listCount);

int32_t HksClientAttestKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain);

int32_t HksClientGetCertificateChain(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    struct HksCertChain *certChain);

int32_t HksClientWrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksParamSet *paramSet, struct HksBlob *wrappedData);

int32_t HksClientUnwrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksBlob *wrappedData, const struct HksParamSet *paramSet);

int32_t HksClientInit(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet, struct HksBlob *handle);

int32_t HksClientUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksClientFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksClientAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet);

#ifdef __cplusplus
}
#endif

#endif /* HKS_CLIENT_IPC_H */
