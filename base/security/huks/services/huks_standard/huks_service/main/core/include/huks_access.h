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

#ifndef HUKS_ACCESS_H
#define HUKS_ACCESS_H

#include "hks_param.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HuksAccessModuleInit(void);

int32_t HuksAccessRefresh(void);

int32_t HuksAccessGenerateKey(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    const struct HksBlob *keyIn, struct HksBlob *keyOut);

int32_t HuksAccessImportKey(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HuksAccessImportWrappedKey(const struct HksBlob *wrappingKeyAlias, const struct HksBlob *key,
    const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut);

int32_t HuksAccessExportPublicKey(const struct HksBlob *key, const struct HksParamSet *paramSet,
    struct HksBlob *keyOut);

int32_t HuksAccessInit(const struct  HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *handle);

int32_t HuksAccessUpdate(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData);

int32_t HuksAccessFinish(const struct HksBlob *handle, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData);

int32_t HuksAccessAbort(const struct HksBlob *handle, const struct HksParamSet *paramSet);

int32_t HuksAccessGetKeyProperties(const struct HksParamSet *paramSet, const struct HksBlob *key);

int32_t HuksAccessGetAbility(int funcType);

int32_t HuksAccessGetHardwareInfo(void);

int32_t HuksAccessCalcMacHeader(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac);

int32_t HuksAccessUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo,
    struct HksBlob *keyOut);

int32_t HuksAccessGenerateRandom(const struct HksParamSet *paramSet, struct HksBlob *random);

int32_t HuksAccessCalcHeaderMac(const struct HksParamSet *paramSet, const struct HksBlob *salt,
    const struct HksBlob *srcData, struct HksBlob *mac);

int32_t HuksAccessUpgradeKeyInfo(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo,
    struct HksBlob *keyOut);

int32_t HuksAccessAttestKey(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *certChain);

int32_t HuksAccessSign(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature);

int32_t HuksAccessVerify(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature);

int32_t HuksAccessEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText);

int32_t HuksAccessDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText);

int32_t HuksAccessAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

int32_t HuksAccessDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
    struct HksBlob *derivedKey);

int32_t HuksAccessMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac);

#ifdef __cplusplus
}
#endif

#endif /* HUKS_ACCESS_H */