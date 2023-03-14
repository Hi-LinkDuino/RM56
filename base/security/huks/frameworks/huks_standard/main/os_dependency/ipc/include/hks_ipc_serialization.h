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

#include "hks_type_inner.h"

#define MAX_IPC_BUF_SIZE    0x10000   /* Maximun IPC message buffer size. */
#define MAX_IPC_RSV_SIZE    0x400     /* Reserve IPC message buffer size */
#define MAX_PROCESS_SIZE    (MAX_IPC_BUF_SIZE - MAX_IPC_RSV_SIZE)

#ifdef __cplusplus
extern "C" {
#endif

int32_t CopyUint32ToBuffer(uint32_t value, const struct HksBlob *destBlob, uint32_t *destOffset);

int32_t HksGenerateKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSetIn, const struct HksBlob *keyOut);

int32_t HksImportKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *key);

int32_t HksExportPublicKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksBlob *key);

int32_t HksGetKeyParamSetPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksBlob *keyOut);

int32_t HksOnceParamPack(struct HksBlob *destData, const struct HksBlob *key, const struct HksParamSet *paramSet,
    uint32_t *offset);

int32_t HksOnceDataPack(struct HksBlob *destData, const struct HksBlob *inputData, const struct HksBlob *rsvData,
    const struct HksBlob *outputData, uint32_t *offset);

int32_t HksAgreeKeyPack(struct HksBlob *destData, const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, const struct HksBlob *agreedKey);

int32_t HksDeriveKeyPack(struct HksBlob *destData, const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    const struct HksBlob *derivedKey);

int32_t HksGetKeyInfoListPack(struct HksBlob *destData, uint32_t listCount, const struct HksKeyInfo *keyInfoList);

int32_t HksGetKeyInfoListUnpackFromService(const struct HksBlob *srcData, uint32_t *listCount,
    struct HksKeyInfo *keyInfoList);

int32_t HksCertificateChainPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *certChainBlob);

int32_t HksCertificateChainUnpackFromService(const struct HksBlob *srcData, bool needEncode,
    struct HksCertChain *certChain);

int32_t HksWrapKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias, const struct HksBlob *targetKeyAlias,
    const struct HksParamSet *paramSet, const struct HksBlob *wrappedData);

int32_t HksUnwrapKeyPack(struct HksBlob *destData, const struct HksBlob *keyAlias,
    const struct HksBlob *targetKeyAlias, const struct HksBlob *wrappedData, const struct HksParamSet *paramSet);

int32_t HksSignWithDeviceKeyPack(struct HksBlob *destData, uint32_t keyId, const struct HksParamSet *paramSet,
    const struct HksBlob *unsignedData, const struct HksBlob *signature);

int32_t HksParamsToParamSet(const struct HksParam *params, uint32_t cnt, struct HksParamSet **outParamSet);

#ifdef __cplusplus
}
#endif

#endif /* HKS_IPC_SERIALIZATION_H */