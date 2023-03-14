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

#ifndef HKS_OPENSSL_CURVE25519_H
#define HKS_OPENSSL_CURVE25519_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CURVE25519_KEY_LEN 32
#define CURVE25519_KEY_BITS 256

int32_t HksOpensslCurve25519GenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);

int32_t HksOpensslX25519AgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey);

int32_t HksOpensslEd25519AgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey);

int32_t HksOpensslEd25519Sign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature);

int32_t HksOpensslEd25519Verify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature);

int32_t HksOpensslGetEd25519PubKey(const struct HksBlob *input, struct HksBlob *output);

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_CURVE25519_H */
