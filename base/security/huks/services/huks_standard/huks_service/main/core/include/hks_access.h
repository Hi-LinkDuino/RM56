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

#ifndef HKS_ACCESS_H
#define HKS_ACCESS_H

#include "hks_type_inner.h"
#include "hks_cmd_id.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksAccessGenerateKey(const struct HksBlob *keyBlob, const struct HksParamSet *paramSetIn,
    const struct HksBlob *keyIn, struct HksBlob *keyOut);

int32_t HksAccessProcessInit(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    uint64_t *operationHandle);

int32_t HksAccessProcessMultiUpdate(uint32_t cmdId, uint64_t operationHandle, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksAccessProcessFinal(uint32_t cmdId, uint64_t operationHandle, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksAccessSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature);

int32_t HksAccessVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature);

int32_t HksAccessEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText);

int32_t HksAccessDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText);

int32_t HksAccessCheckKeyValidity(const struct HksParamSet *paramSet, const struct HksBlob *key);

int32_t HksAccessGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random);

int32_t HksAccessImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HksAccessExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *keyOut);

int32_t HksAccessAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

#ifdef HKS_SUPPORT_API_ATTEST_KEY
int32_t HksAccessAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *certChain);
#endif

int32_t HksAccessDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    struct HksBlob *derivedKey);

int32_t HksAccessMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac);

int32_t HksAccessInitialize(void);

int32_t HksAccessRefresh(void);

int32_t HksAccessUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo, struct HksBlob *keyOut);

int32_t HksAccessCalcHeaderMac(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac);

#ifdef __cplusplus
}
#endif

#endif /* HKS_ACCESS_H */
