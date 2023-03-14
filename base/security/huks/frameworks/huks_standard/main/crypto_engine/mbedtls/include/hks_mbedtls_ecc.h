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

#ifndef HKS_MBEDTLS_ECC_H
#define HKS_MBEDTLS_ECC_H

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include <mbedtls/bignum.h>
#include <mbedtls/ecp.h>

#include "hks_crypto_hal.h"
#include "hks_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t GetEccGroupId(const uint32_t keyLen, mbedtls_ecp_group_id *grpId);

int32_t EccKeyCheck(const struct HksBlob *key);

#if defined(HKS_SUPPORT_ECDH_C) || defined(HKS_SUPPORT_ECDSA_C)
int32_t HksMbedtlsEccGetKeyCurveNist(const struct KeyMaterialEcc *keyMaterial, mbedtls_ecp_group_id *curve);

int32_t HksEccKeyMaterialToPub(const struct HksBlob *key, mbedtls_ecp_point *pub);

int32_t HksEccKeyMaterialToPri(const struct HksBlob *key, mbedtls_mpi *pri);
#endif

#ifdef HKS_SUPPORT_ECC_GENERATE_KEY
int32_t HksMbedtlsEccGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key);
#endif

#ifdef HKS_SUPPORT_ECC_GET_PUBLIC_KEY
int32_t HksMbedtlsGetEccPubKey(const struct HksBlob *keyIn, struct HksBlob *keyOut);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HKS_MBEDTLS_ECC_H */
