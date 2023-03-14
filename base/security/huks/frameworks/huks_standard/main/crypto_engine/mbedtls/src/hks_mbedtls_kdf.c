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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#ifdef HKS_SUPPORT_KDF_C

#include "hks_mbedtls_kdf.h"

#include <mbedtls/hkdf.h>
#include <mbedtls/md.h>
#include <mbedtls/pkcs5.h>

#include "hks_log.h"
#include "hks_mbedtls_common.h"
#include "hks_type_inner.h"

#ifdef _CUT_AUTHENTICATE
#undef HKS_SUPPORT_KDF_PBKDF2
#endif

#ifdef HKS_SUPPORT_KDF_PBKDF2
static int32_t DeriveKeyPbkdf2(const struct HksBlob *mainKey, const struct HksKeyDerivationParam *derParam,
    const mbedtls_md_info_t *info, struct HksBlob *derivedKey)
{
    mbedtls_md_context_t ctx;
    mbedtls_md_init(&ctx);

    int32_t ret;
    do {
        ret = mbedtls_md_setup(&ctx, info, 1); /* 1 for using HMAC */
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls md setup failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = mbedtls_pkcs5_pbkdf2_hmac(&ctx, mainKey->data, mainKey->size, derParam->salt.data,
            derParam->salt.size, derParam->iterations, derivedKey->size, derivedKey->data);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls pbkdf2 failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(derivedKey->data, derivedKey->size, 0, derivedKey->size);
        }
    } while (0);

    mbedtls_md_free(&ctx);
    return ret;
}
#endif /* HKS_SUPPORT_KDF_PBKDF2 */

#ifdef HKS_SUPPORT_KDF_HKDF
static int32_t DeriveKeyHkdf(const struct HksBlob *mainKey, const struct HksKeyDerivationParam *derParam,
    const mbedtls_md_info_t *info, struct HksBlob *derivedKey)
{
    int32_t ret = mbedtls_hkdf(info, derParam->salt.data, derParam->salt.size, mainKey->data, mainKey->size,
        derParam->info.data, derParam->info.size, derivedKey->data, derivedKey->size);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls hkdf failed! mbedtls ret = 0x%X", ret);
        (void)memset_s(derivedKey->data, derivedKey->size, 0, derivedKey->size);
    }

    return ret;
}
#endif /* HKS_SUPPORT_KDF_HKDF */

int32_t HksMbedtlsDeriveKey(const struct HksBlob *mainKey,
    const struct HksKeySpec *derivationSpec, struct HksBlob *derivedKey)
{
    const struct HksKeyDerivationParam *derParam = (struct HksKeyDerivationParam *)(derivationSpec->algParam);

    uint32_t mbedtlsAlg;
    int32_t ret = HksToMbedtlsDigestAlg(derParam->digestAlg, &mbedtlsAlg);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    const mbedtls_md_info_t *info = mbedtls_md_info_from_type(mbedtlsAlg);
    if (info == NULL) {
        HKS_LOG_E("Mbedtls get md info failed! mbedtls ret = 0x%X", ret);
        return HKS_FAILURE;
    }

    switch (derivationSpec->algType) {
#ifdef HKS_SUPPORT_KDF_PBKDF2
        case HKS_ALG_PBKDF2:
            return DeriveKeyPbkdf2(mainKey, derParam, info, derivedKey);
#endif
#ifdef HKS_SUPPORT_KDF_HKDF
        case HKS_ALG_HKDF:
            return DeriveKeyHkdf(mainKey, derParam, info, derivedKey);
#endif
        default:
            HKS_LOG_E("Unsupport derive key alg! mode = 0x%X", derivationSpec->algType);
            return HKS_ERROR_INVALID_ARGUMENT;
    }
}
#endif /* HKS_SUPPORT_KDF_C */
