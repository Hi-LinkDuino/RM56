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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#include "hks_mbedtls_common.h"

#include <mbedtls/entropy.h>
#include <mbedtls/md.h>

#include "hks_log.h"

#ifndef _HARDWARE_ROOT_KEY_
#include "hks_rkc.h"
#endif

/* the custom data of random seed */
const unsigned char g_hksRandomSeedCustom[] = {
    /* H     K     S */
    0x48, 0x4B, 0x53
};

int32_t HksToMbedtlsDigestAlg(const uint32_t hksAlg, uint32_t *mbedtlsAlg)
{
    switch (hksAlg) {
        case HKS_DIGEST_MD5:
            *mbedtlsAlg = MBEDTLS_MD_MD5;
            break;
        case HKS_DIGEST_SHA1:
            *mbedtlsAlg = MBEDTLS_MD_SHA1;
            break;
        case HKS_DIGEST_SHA224:
            *mbedtlsAlg = MBEDTLS_MD_SHA224;
            break;
        case HKS_DIGEST_SHA256:
            *mbedtlsAlg = MBEDTLS_MD_SHA256;
            break;
        case HKS_DIGEST_SHA384:
            *mbedtlsAlg = MBEDTLS_MD_SHA384;
            break;
        case HKS_DIGEST_SHA512:
            *mbedtlsAlg = MBEDTLS_MD_SHA512;
            break;
        default:
            HKS_LOG_E("Unsupported digest algorithm! digestAlg: 0x%X", hksAlg);
            return HKS_ERROR_INVALID_DIGEST;
    }
    return HKS_SUCCESS;
}

int32_t HksCtrDrbgSeed(mbedtls_ctr_drbg_context *ctrDrbg, mbedtls_entropy_context *entropy)
{
    mbedtls_ctr_drbg_init(ctrDrbg);
    mbedtls_entropy_init(entropy);

    /* use the g_hksRandomSeedCustom without string terminator */
    int32_t ret = mbedtls_ctr_drbg_seed(ctrDrbg, mbedtls_entropy_func,
        entropy, g_hksRandomSeedCustom, sizeof(g_hksRandomSeedCustom));
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Ctr drbg seed failed! mbedtls ret = 0x%X", ret);
        mbedtls_ctr_drbg_free(ctrDrbg);
        mbedtls_entropy_free(entropy);
        return ret;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsFillRandom(struct HksBlob *randomData)
{
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    int32_t ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    do {
        ret = mbedtls_ctr_drbg_random(&ctrDrbg, randomData->data, randomData->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls random failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(randomData->data, randomData->size, 0, randomData->size);
        }
    } while (0);

    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int32_t HksMbedtlsGetMainKey(const struct HksBlob *message, struct HksBlob *mainKey)
{
    (void)message;

#ifndef _HARDWARE_ROOT_KEY_
    return HksRkcGetMainKey(mainKey);
#else
    /*
    * Currently, root key is implemented using stubs.
    * Product adaptation needs to be performed based on hardware capabilities.
    */
    uint8_t stubBuf[] = {
        0x0c, 0xb4, 0x29, 0x39, 0xb7, 0x46, 0xa6, 0x4b,
        0xdd, 0xf3, 0x75, 0x4c, 0xe0, 0x73, 0x91, 0x51,
        0xc4, 0x88, 0xbe, 0xa4, 0xe1, 0x87, 0xb5, 0x42,
        0x06, 0x27, 0x08, 0x21, 0xe2, 0x8f, 0x9b, 0xc1,
    };

    if (memcpy_s(mainKey->data, mainKey->size, stubBuf, sizeof(stubBuf)) != EOK) {
        HKS_LOG_E("memcpy failed, get stub main key failed");
        return HKS_ERROR_BAD_STATE;
    }
    return HKS_SUCCESS;
#endif
}