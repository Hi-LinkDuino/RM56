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

#ifndef HKS_CORE_SERVICE_H
#define HKS_CORE_SERVICE_H

#include "hks_crypto_hal.h"
#include "hks_type_inner.h"

#include "hks_core_service_three_stage.h"
#include "hks_keynode.h"
#include "hks_mutex.h"

#define MAX_HASH_SIZE 64

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksCoreGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *keyIn, struct HksBlob *keyOut);

int32_t HksCoreSign(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *signature);

int32_t HksCoreVerify(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    const struct HksBlob *signature);

int32_t HksCoreEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *plainText,
    struct HksBlob *cipherText);

int32_t HksCoreDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *cipherText,
    struct HksBlob *plainText);

int32_t HksCheckKeyValidity(const struct HksParamSet *paramSet, const struct HksBlob *key);

int32_t HksCoreGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random);

int32_t HksCoreImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HksCoreExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HksCoreAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

int32_t HksCoreDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey, struct HksBlob *derivedKey);

int32_t HksCoreMac(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *mac);

int32_t HksCoreRefreshKeyInfo(void);

int32_t HksCoreUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo, struct HksBlob *keyOut);

int32_t HksCoreCalcMacHeader(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac);

int32_t HksCoreModuleInit(void);

int32_t HksCoreRefresh(void);

int32_t HksCoreImportWrappedKey(const struct HksBlob *wrappingKeyAlias, const struct HksBlob *key,
    const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HksCoreInit(const struct  HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *handle);

int32_t HksCoreUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksCoreFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet, const struct HksBlob *inData,
    struct HksBlob *outData);

int32_t HksCoreAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet);

int32_t HksCoreGetKeyProperties(const struct HksParamSet *paramSet, const struct HksBlob *key);

int32_t HksCoreAttestKey(const struct HksBlob *key, const  struct HksParamSet *paramSet, struct HksBlob *certChain);

int32_t HksCoreGetAbility(int32_t funcType);

int32_t HksCoreGetHardwareInfo(void);

HksMutex *HksCoreGetHuksMutex(void);

struct HksCoreInitHandler {
    enum HksKeyPurpose pur;
    int32_t (*handler)(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
        uint32_t alg);
};

struct HksCoreUpdateHandler {
    enum HksKeyPurpose pur;
    int32_t (*handler)(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
        const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg);
};

struct HksCoreFinishHandler {
    enum HksKeyPurpose pur;
    int32_t (*handler)(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
        const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg);
};

struct HksCoreAbortHandler {
    enum HksKeyPurpose pur;
    int32_t (*handler)(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet, uint32_t alg);
};

#ifdef __cplusplus
}
#endif

#endif /* HKS_CORE_SERVICE_H */
