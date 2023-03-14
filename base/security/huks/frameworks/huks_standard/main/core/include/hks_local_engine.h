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

#ifndef HKS_LOCAL_ENGINE_H
#define HKS_LOCAL_ENGINE_H

#include "hks_cmd_id.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksLocalMac(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac);

int32_t HksLocalHash(const struct HksParamSet *paramSet, const struct HksBlob *srcData, struct HksBlob *hash);

int32_t HksLocalBnExpMod(struct HksBlob *x, const struct HksBlob *a, const struct HksBlob *e, const struct HksBlob *n);

int32_t HksLocalGenerateKey(const struct HksParamSet *paramSetIn, struct HksParamSet *paramSetOut);

int32_t HksLocalEncrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText);

int32_t HksLocalDecrypt(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText);

int32_t HksLocalAgreeKey(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey);

int32_t HksLocalDeriveKey(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    struct HksBlob *derivedKey);

int32_t HksLocalSign(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    struct HksBlob *signature);

int32_t HksLocalVerify(const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *srcData,
    const struct HksBlob *signature);

#ifdef __cplusplus
}
#endif

#endif /* HKS_LOCAL_ENGINE_H */