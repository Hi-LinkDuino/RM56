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

#ifndef HKS_TEEC_H
#define HKS_TEEC_H

#include <tee_client_type.h>
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksTeeOpen(void);

int32_t HksTeeGenerateKey(const struct HksBlob *keyBlob, const struct HksParamSet *paramSetIn,
    struct HksBlob *keyOut);

int32_t HksTeeProcessInit(uint32_t msgId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    uint64_t *operationHandle);

int32_t HksTeeProcessMultiUpdate(uint32_t msgId, uint64_t operationHandle, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksTeeSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature);

int32_t HksTeeVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature);

int32_t HksTeeEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText);

int32_t HksTeeDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText);

int32_t HksTeeCheckKeyLegality(const struct HksParamSet *paramSet, const struct HksBlob *key);

int32_t HksTeeGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random);

int32_t HksTeeImportKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HksTeeExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HksTeeAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

int32_t HksTeeDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey, struct HksBlob *derivedKey);

int32_t HksTeeWrapKey(const struct HksBlob *keyAlias, const struct HksBlob *targetAlias,
    const struct HksParamSet *paramSet, struct HksBlob *wrappedData);

int32_t HksTeeUnwrapKey(const struct HksBlob *keyAlias, const struct HksBlob *wrappedData,
    const struct HksParamSet *paramSet, struct HksBlob *output);

int32_t HksTeeMac(const struct HksBlob *keyBlob, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac);

int32_t HksTeeAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *certChain);

int32_t ProvisionVerify(TEEC_Operation *operation, const struct HksBlob *challengeIn, uint32_t certCount,
    const struct HksBlob *verify, bool isVerify);

int32_t HksTeeExportTrustCerts(struct HksBlob *certChain);

int32_t HksTeeImportTrustCerts(const struct HksBlob *certChain);

int32_t HcmTeeIsDeviceKeyExist(void);

int32_t HksTeeProvision(const struct HksBlob *keybox, struct HksBlob *challenge,
    const struct HksBlob *challengeIn, struct HksBlob *signature, struct HksBlob *certData);

int32_t HksTeeProvisionVerify(const struct HksBlob *verify, struct HksBlob *challenge,
    const struct HksBlob *challengeIn, struct HksBlob *signature);

#ifdef __cplusplus
}
#endif

#endif