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

#ifndef HKS_MBEDTLS_COMMON_H
#define HKS_MBEDTLS_COMMON_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

#include "hks_type_inner.h"

#define HKS_MBEDTLS_SUCCESS ((int32_t)0)

#ifdef __cplusplus
extern "C" {
#endif

int32_t HksToMbedtlsDigestAlg(const uint32_t hksAlg, uint32_t *mbedtlsAlg);

int32_t HksCtrDrbgSeed(mbedtls_ctr_drbg_context *ctrDrbg, mbedtls_entropy_context *entropy);

int32_t HksMbedtlsFillRandom(struct HksBlob *randomData);

int32_t HksMbedtlsGetMainKey(const struct HksBlob *message, struct HksBlob *mainKey);

#ifdef __cplusplus
}
#endif

#endif /* HKS_MBEDTLS_COMMON_H */