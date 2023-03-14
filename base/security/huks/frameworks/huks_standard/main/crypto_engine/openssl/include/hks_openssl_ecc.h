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

#ifndef HKS_OPENSSL_ECC_H
#define HKS_OPENSSL_ECC_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ECC_KEYPAIR_CNT 3

#if defined(HKS_SUPPORT_ECC_GENERATE_KEY)
int32_t HksOpensslEccGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif
#if defined(HKS_SUPPORT_ECC_GET_PUBLIC_KEY)
int32_t HksOpensslGetEccPubKey(const struct HksBlob *input, struct HksBlob *output);
#endif

#ifdef HKS_SUPPORT_ECDH_AGREE_KEY
int32_t HksOpensslEcdhAgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey);
#endif /* HKS_SUPPORT_ECDH_AGREE_KEY */

#ifdef HKS_SUPPORT_ECDSA_SIGN_VERIFY
int32_t HksOpensslEcdsaSign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature);

int32_t HksOpensslEcdsaVerify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature);
#endif /* HKS_SUPPORT_ECDSA_SIGN_VERIFY */

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_ECC_H */
