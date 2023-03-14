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

#ifndef HKS_MBEDTLS_HMAC_H
#define HKS_MBEDTLS_HMAC_H

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

#ifdef HKS_SUPPORT_HMAC_GENERATE_KEY
int32_t HksMbedtlsHmacGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif

int32_t HksMbedtlsHmac(const struct HksBlob *key,
    uint32_t digestAlg, const struct HksBlob *msg, struct HksBlob *mac);

int32_t HksMbedtlsHmacInit(void **cryptoCtx, const struct HksBlob *key, uint32_t digestAlg);

int32_t HksMbedtlsHmacUpdate(void *cryptoCtx, const struct HksBlob *msg);

int32_t HksMbedtlsHmacFinal(void **cryptoCtx, struct HksBlob *msg, struct HksBlob *mac);

void HksMbedtlsHmacHalFreeCtx(void **cryptoCtx);
#ifdef __cplusplus
}
#endif

#endif /* HKS_MBEDTLS_HMAC_H */
