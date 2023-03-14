/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef HKS_MBEDTLS_DH_H
#define HKS_MBEDTLS_DH_H

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

#ifdef HKS_SUPPORT_DH_C
#ifdef HKS_SUPPORT_DH_GENERATE_KEY
int32_t HksMbedtlsDhGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif /* HKS_SUPPORT_DH_GENERATE_KEY */

#ifdef HKS_SUPPORT_DH_GET_PUBLIC_KEY
int32_t HksMbedtlsGetDhPubKey(const struct HksBlob *input, struct HksBlob *output);
#endif /* HKS_SUPPORT_DH_GET_PUBLIC_KEY */

#ifdef HKS_SUPPORT_DH_AGREE_KEY
int32_t HksMbedtlsDhAgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey);
#endif /* HKS_SUPPORT_DH_AGREE_KEY */
#endif /* HKS_SUPPORT_DH_C */

#ifdef __cplusplus
}
#endif

#endif /* HKS_MBEDTLS_DH_H */
