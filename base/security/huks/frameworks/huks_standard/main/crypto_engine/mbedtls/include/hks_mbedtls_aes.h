/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#ifndef HKS_MBEDTLS_AES_H
#define HKS_MBEDTLS_AES_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "hks_crypto_hal.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HKS_SUPPORT_AES_C
#ifdef HKS_SUPPORT_AES_GENERATE_KEY
int32_t HksMbedtlsAesGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif /* HKS_SUPPORT_AES_GENERATE_KEY */

int32_t HksMbedtlsAesEncrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText, struct HksBlob *tagAead);

int32_t HksMbedtlsAesDecrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText);

int32_t HksMbedtlsAesCryptoInit(void **cryptoCtx, const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const bool encrypt);

int32_t HksMbedtlsAesCryptoUpdate(void *cryptoCtx, const struct HksBlob *message, struct HksBlob *cipherText,
    const bool encrypt);

int32_t HksMbedtlsAesCryptoFinal(void **cryptoCtx, const struct HksBlob *message, struct HksBlob *cipherText,
    struct HksBlob *tagAead, const bool encrypt);

void HksMbedtlsAesHalFreeCtx(void **cryptoCtx);
#endif /* HKS_SUPPORT_AES_C */

#ifdef __cplusplus
}
#endif

#endif /* HKS_MBEDTLS_AES_H */
