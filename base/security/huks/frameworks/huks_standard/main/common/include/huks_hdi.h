/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HUKS_HDI_H
#define HUKS_HDI_H

#include "hks_param.h"
#include "hks_type.h"

struct HuksHdi {
    int32_t (*HuksHdiModuleInit)(void);

    int32_t (*HuksHdiRefresh)(void);

    int32_t (*HuksHdiGenerateKey)(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
        const struct HksBlob *keyIn, struct HksBlob *keyOut);

    int32_t (*HuksHdiImportKey)(const struct HksBlob *keyAlias, const struct HksBlob *key,
        const struct HksParamSet *paramSet, struct HksBlob *keyOut);

    int32_t (*HuksHdiImportWrappedKey)(const struct HksBlob *wrappingKeyAlias, const struct HksBlob *key,
        const struct HksBlob *wrappedKeyData, const struct HksParamSet *paramSet, struct HksBlob *keyOut);

    int32_t (*HuksHdiExportPublicKey)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        struct HksBlob *keyOut);

    int32_t (*HuksHdiInit)(const struct HksBlob *key, const struct HksParamSet *paramSet, struct HksBlob *handle);

    int32_t (*HuksHdiUpdate)(const struct HksBlob *handle, const struct HksParamSet *paramSet,
        const struct HksBlob *inData, struct HksBlob *outData);

    int32_t (*HuksHdiFinish)(const struct HksBlob *handle, const struct HksParamSet *paramSet,
        const struct HksBlob *inData, struct HksBlob *outData);

    int32_t (*HuksHdiAbort)(const struct HksBlob *handle, const struct HksParamSet *paramSet);

    int32_t (*HuksHdiGetKeyProperties)(const struct HksParamSet *paramSet, const struct HksBlob *key);

    int32_t (*HuksHdiAttestKey)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        struct HksBlob *certChain);

    int32_t (*HuksHdiGetAbility)(int32_t funcType);

    int32_t (*HuksHdiGetHardwareInfo)(void);

    int32_t (*HuksHdiCalcMacHeader)(const struct HksParamSet *paramSet, const struct HksBlob *salt,
        const struct HksBlob *srcData, struct HksBlob *mac);

    int32_t (*HuksHdiUpgradeKeyInfo)(const struct HksBlob *keyAlias, const struct HksBlob *keyInfo,
        struct HksBlob *keyOut);

    int32_t (*HuksHdiGenerateRandom)(const struct HksParamSet *paramSet, struct HksBlob *random);

    int32_t (*HuksHdiSign)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        const struct HksBlob *srcData, struct HksBlob *signature);

    int32_t (*HuksHdiVerify)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        const struct HksBlob *srcData, const struct HksBlob *signature);

    int32_t (*HuksHdiEncrypt)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        const struct HksBlob *plainText, struct HksBlob *cipherText);

    int32_t (*HuksHdiDecrypt)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        const struct HksBlob *cipherText, struct HksBlob *plainText);

    int32_t (*HuksHdiAgreeKey)(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
        const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

    int32_t (*HuksHdiDeriveKey)(const struct HksParamSet *paramSet, const struct HksBlob *kdfKey,
        struct HksBlob *derivedKey);

    int32_t (*HuksHdiMac)(const struct HksBlob *key, const struct HksParamSet *paramSet,
        const struct HksBlob *srcData, struct HksBlob *mac);
};

#endif /* HUKS_HDI_H */
