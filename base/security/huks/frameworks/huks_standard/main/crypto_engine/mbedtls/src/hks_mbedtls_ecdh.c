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

#ifdef _CUT_AUTHENTICATE_
#undef HKS_SUPPORT_ECDH_C
#endif /* _CUT_AUTHENTICATE_ */

#ifdef HKS_SUPPORT_ECDH_C

#include "hks_mbedtls_ecdh.h"

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>

#include "hks_log.h"
#include "hks_mbedtls_common.h"
#include "hks_mbedtls_ecc.h"

#ifdef HKS_SUPPORT_ECDH_AGREE_KEY
static int32_t EccKeyMaterialToCtx(const struct HksBlob *nativeKey,
    const struct HksBlob *pubKey, mbedtls_ecdh_context *ctx)
{
    int32_t ret = HksEccKeyMaterialToPub(pubKey, &(ctx->Qp));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Ecc keyMaterial to public key failed! ret = 0x%X", ret);
        return ret;
    }

    ret = HksEccKeyMaterialToPri(nativeKey, &(ctx->d));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Ecc keyMaterial to private key failed! ret = 0x%X", ret);
    }

    return ret;
}

int32_t HksMbedtlsEcdh(const struct HksBlob *nativeKey,
    const struct HksBlob *pubKey, const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    int32_t ret = EccKeyCheck(pubKey);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ecp_group_id mbedtlsCurveNist = MBEDTLS_ECP_DP_NONE;
    ret = HksMbedtlsEccGetKeyCurveNist((struct KeyMaterialEcc *)(nativeKey->data), &mbedtlsCurveNist);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_entropy_context entropy;
    ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ecdh_context ctx;
    mbedtls_ecdh_init(&ctx);

    do {
        ret = mbedtls_ecp_group_load(&(ctx.grp), mbedtlsCurveNist);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls ecdh load group failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = EccKeyMaterialToCtx(nativeKey, pubKey, &ctx);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = mbedtls_ecdh_compute_shared(&(ctx.grp), &(ctx.z), &(ctx.Qp), &(ctx.d), mbedtls_ctr_drbg_random, &ctrDrbg);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls ecdh shared key failed! mbedtls ret = 0x%X", ret);
            break;
        }

        const uint32_t keyByteLen = HKS_KEY_BYTES(spec->keyLen);
        ret = mbedtls_mpi_write_binary(&(ctx.z), sharedKey->data, keyByteLen);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls ecdh mpi write to sharedKey failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(sharedKey->data, sharedKey->size, 0, sharedKey->size);
            break;
        }
        sharedKey->size = keyByteLen;
    } while (0);

    mbedtls_ecdh_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}
#endif /* HKS_SUPPORT_ECDH_AGREE_KEY */
#endif /* HKS_SUPPORT_ECDH_C */
