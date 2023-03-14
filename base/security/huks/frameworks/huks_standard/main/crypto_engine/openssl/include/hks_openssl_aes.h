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

#ifndef HKS_OPENSSL_AES_H
#define HKS_OPENSSL_AES_H

#include "hks_crypto_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HKS_SUPPORT_AES_C
#ifdef HKS_SUPPORT_AES_GENERATE_KEY
int32_t HksOpensslAesGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif /* HKS_SUPPORT_AES_GENERATE_KEY */

#if defined(HKS_SUPPORT_AES_CBC_NOPADDING) || defined(HKS_SUPPORT_AES_CBC_PKCS7) ||        \
    defined(HKS_SUPPORT_AES_ECB_PKCS7PADDING) || defined(HKS_SUPPORT_AES_CTR_NOPADDING) || \
    defined(HKS_SUPPORT_AES_ECB_NOPADDING) || defined(HKS_SUPPORT_AES_GCM)
int32_t HksOpensslAesEncryptInit(void **cryptoCtx, const struct HksBlob *key, const struct HksUsageSpec *usageSpec);

int32_t HksOpensslAesEncryptUpdate(void *cryptoCtx, const struct HksBlob *message, struct HksBlob *cipherText);

int32_t HksOpensslAesEncryptFinal(void **cryptoCtx, const struct HksBlob *message, struct HksBlob *cipherText,
    struct HksBlob *tagAead);

int32_t HksOpensslAesDecryptInit(void **cryptoCtx, const struct HksBlob *key, const struct HksUsageSpec *usageSpec);

int32_t HksOpensslAesDecryptUpdate(void *cryptoCtx, const struct HksBlob *message, struct HksBlob *plainText);

int32_t HksOpensslAesDecryptFinal(void **cryptoCtx, const struct HksBlob *message, struct HksBlob *cipherText,
    struct HksBlob *tagAead);

void HksOpensslAesHalFreeCtx(void **cryptoCtx);

int32_t HksOpensslAesEncrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText, struct HksBlob *tagAead);

int32_t HksOpensslAesDecrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText);
#endif /* HKS_SUPPORT_AES_CBC_NOPADDING */
#endif /* HKS_SUPPORT_AES_C */

#ifdef __cplusplus
}
#endif

#endif /* HKS_OPENSSL_AES_H */
