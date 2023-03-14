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

#ifndef HKS_TEST_API_PERFORMANCE_H
#define HKS_TEST_API_PERFORMANCE_H

#include "hks_type.h"

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksGenerateKeyRun(const struct HksBlob *keyAlias, const struct HksParamSet *paramSetIn,
    struct HksParamSet *paramSetOut, uint32_t performTimes);

int32_t HksEncryptRun(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *plainText, struct HksBlob *cipherText, uint32_t performTimes);

int32_t HksDecryptRun(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *cipherText, struct HksBlob *plainText, uint32_t performTimes);

int32_t HksMacRun(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac, uint32_t performTimes);

int32_t HksDeriveKeyRun(const struct HksParamSet *paramSet, const struct HksBlob *masterKey,
    struct HksBlob *derivedKey, uint32_t performTimes);

int32_t HksDeleteKeyRun(const struct HksBlob *keyAlias, uint32_t performTimes);

int32_t HksKeyExistRun(const struct HksBlob *keyAlias, uint32_t performTimes);

int32_t HksGenerateRandomRun(struct HksBlob *random, uint32_t performTimes);

int32_t HksAgreeKeyRun(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, struct HksBlob *agreedKey, uint32_t performTimes);

int32_t HksHashRun(const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *hash, uint32_t performTimes);

int32_t HksBnExpModRun(struct HksBlob *x, const struct HksBlob *a,
    const struct HksBlob *e, const struct HksBlob *n, uint32_t performTimes);

#ifdef __cplusplus
}
#endif

#endif /* HKS_TEST_API_PERFORMANCE_H */