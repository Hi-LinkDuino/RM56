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

#ifndef HKS_OPENSSL_HMAC_H
#define HKS_OPENSSL_HMAC_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HKS_DIGEST_SHA1_LEN 20
#define HKS_DIGEST_SHA224_LEN 28
#define HKS_DIGEST_SHA256_LEN 32
#define HKS_DIGEST_SHA384_LEN 48
#define HKS_DIGEST_SHA512_LEN 64

#ifdef HKS_SUPPORT_HMAC_C
#ifdef HKS_SUPPORT_HMAC_GENERATE_KEY
int32_t HksOpensslHmacGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif /* HKS_SUPPORT_HMAC_GENERATE_KEY */

#if defined(HKS_SUPPORT_HMAC_SHA1) || defined(HKS_SUPPORT_HMAC_SHA224) || defined(HKS_SUPPORT_HMAC_SHA256) || \
    defined(HKS_SUPPORT_HMAC_SHA384) || defined(HKS_SUPPORT_HMAC_SHA512)
int32_t HksOpensslHmac(const struct HksBlob *key, uint32_t digestAlg, const struct HksBlob *msg, struct HksBlob *mac);

int32_t HksOpensslHmacInit(void **cryptoCtx, const struct HksBlob *key, uint32_t digestAlg);

int32_t HksOpensslHmacUpdate(void *cryptoCtx, const struct HksBlob *msg);

int32_t HksOpensslHmacFinal(void **cryptoCtx, struct HksBlob *msg, struct HksBlob *mac);

void HksOpensslHmacHalFreeCtx(void **cryptoCtx);
#endif /* HKS_SUPPORT_HMAC_SHA1 */
#endif /* HKS_SUPPORT_HMAC_C */

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_HMAC_H */
