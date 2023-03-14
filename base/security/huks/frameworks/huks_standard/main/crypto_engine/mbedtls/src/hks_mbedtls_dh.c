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

#ifdef HKS_SUPPORT_DH_C

#include "hks_mbedtls_dh.h"

#include <mbedtls/dhm.h>

#include "hks_log.h"
#include "hks_mbedtls_common.h"
#include "hks_mem.h"

#define HKS_DH_KEYPAIR_CNT 2

#if defined(HKS_SUPPORT_DH_GENERATE_KEY) || defined(HKS_SUPPORT_DH_AGREE_KEY)
static uint8_t g_ffdhe2048ParamG[] = MBEDTLS_DHM_RFC7919_FFDHE2048_G_BIN;
static uint8_t g_ffdhe2048ParamP[] = MBEDTLS_DHM_RFC7919_FFDHE2048_P_BIN;
static uint8_t g_ffdhe3072ParamG[] = MBEDTLS_DHM_RFC7919_FFDHE3072_G_BIN;
static uint8_t g_ffdhe3072ParamP[] = MBEDTLS_DHM_RFC7919_FFDHE3072_P_BIN;
static uint8_t g_ffdhe4096ParamG[] = MBEDTLS_DHM_RFC7919_FFDHE4096_G_BIN;
static uint8_t g_ffdhe4096ParamP[] = MBEDTLS_DHM_RFC7919_FFDHE4096_P_BIN;

static int32_t GetDhParam(uint32_t keySize, struct HksBlob *P, struct HksBlob *G)
{
    switch (keySize) {
        case HKS_DH_KEY_SIZE_2048:
            P->data = g_ffdhe2048ParamP;
            P->size = sizeof(g_ffdhe2048ParamP);
            G->data = g_ffdhe2048ParamG;
            G->size = sizeof(g_ffdhe2048ParamG);
            return HKS_SUCCESS;
        case HKS_DH_KEY_SIZE_3072:
            P->data = g_ffdhe3072ParamP;
            P->size = sizeof(g_ffdhe3072ParamP);
            G->data = g_ffdhe3072ParamG;
            G->size = sizeof(g_ffdhe3072ParamG);
            return HKS_SUCCESS;
        case HKS_DH_KEY_SIZE_4096:
            P->data = g_ffdhe4096ParamP;
            P->size = sizeof(g_ffdhe4096ParamP);
            G->data = g_ffdhe4096ParamG;
            G->size = sizeof(g_ffdhe4096ParamG);
            return HKS_SUCCESS;
        default:
            return HKS_ERROR_INVALID_KEY_SIZE;
    }
}
#endif

#ifdef HKS_SUPPORT_DH_GENERATE_KEY
static int32_t DhSaveKeyMaterial(const mbedtls_dhm_context *ctx, const uint32_t keySize, struct HksBlob *key)
{
    const uint32_t keyByteLen = HKS_KEY_BYTES(keySize);
    const uint32_t rawMaterialLen = sizeof(struct KeyMaterialDh) + keyByteLen * HKS_DH_KEYPAIR_CNT;
    uint8_t *rawMaterial = (uint8_t *)HksMalloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);

    struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_DH;
    keyMaterial->keySize = keySize;
    keyMaterial->pubKeySize = mbedtls_mpi_size(&ctx->GX);
    keyMaterial->priKeySize = mbedtls_mpi_size(&ctx->X);

    int32_t ret;
    do {
        uint32_t offset = sizeof(*keyMaterial);
        ret = mbedtls_mpi_write_binary(&(ctx->GX), rawMaterial + offset, keyMaterial->pubKeySize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("mbedtls_mpi_write_binary failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
            HksFree(rawMaterial);
            break;
        }

        offset = offset + keyMaterial->pubKeySize;
        ret = mbedtls_mpi_write_binary(&(ctx->X), rawMaterial + offset, keyMaterial->priKeySize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("mbedtls_mpi_write_binary failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
            HksFree(rawMaterial);
            break;
        }

        key->data = rawMaterial;
        key->size = rawMaterialLen;
    } while (0);

    return ret;
}

int32_t HksMbedtlsDhGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    mbedtls_dhm_context ctx;
    mbedtls_dhm_init(&ctx);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    int32_t ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        mbedtls_dhm_free(&ctx);
        return ret;
    }

    do {
        struct HksBlob paramP;
        struct HksBlob paramG;
        ret = GetDhParam(spec->keyLen, &paramP, &paramG);
        if (ret != HKS_SUCCESS) {
            break;
        }
        uint32_t keyLen = HKS_KEY_BYTES(spec->keyLen);

        if (mbedtls_mpi_read_binary(&ctx.P, paramP.data, paramP.size) != HKS_MBEDTLS_SUCCESS ||
            mbedtls_mpi_read_binary(&ctx.G, paramG.data, paramG.size) != HKS_MBEDTLS_SUCCESS) {
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }
        ctx.len = keyLen;

        uint8_t *output = (uint8_t *)HksMalloc(keyLen);
        if (output == NULL) {
            break;
        }
        ret = mbedtls_dhm_make_public(&ctx, keyLen, output, keyLen, mbedtls_ctr_drbg_random, &ctrDrbg);
        if (ret != HKS_SUCCESS) {
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
        } else {
            ret = DhSaveKeyMaterial(&ctx, spec->keyLen, key);
        }
        HksFree(output);
    } while (0);

    mbedtls_dhm_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);

    return ret;
}
#endif

#ifdef HKS_SUPPORT_DH_GET_PUBLIC_KEY
int32_t HksMbedtlsGetDhPubKey(const struct HksBlob *input, struct HksBlob *output)
{
    struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)input->data;
    if (input->size < sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (output->size < sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(output->data, output->size, input->data, sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize) !=
        EOK) {
        return HKS_ERROR_INVALID_OPERATION;
    }
    ((struct KeyMaterialDh *)output->data)->priKeySize = 0;
    ((struct KeyMaterialDh *)output->data)->reserved = 0;
    output->size = sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize;

    return HKS_SUCCESS;
}
#endif

#ifdef HKS_SUPPORT_DH_AGREE_KEY
static int32_t DhKeyMaterialToCtx(const struct HksBlob *key, const bool needPrivateExponent, mbedtls_dhm_context *ctx)
{
    const struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)(key->data);

    int32_t ret;
    do {
        struct HksBlob paramP;
        struct HksBlob paramG;
        ret = GetDhParam(keyMaterial->keySize, &paramP, &paramG);
        if (ret != HKS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_read_binary(&ctx->P, paramP.data, paramP.size) != HKS_MBEDTLS_SUCCESS ||
            mbedtls_mpi_read_binary(&ctx->G, paramG.data, paramG.size) != HKS_MBEDTLS_SUCCESS) {
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }
        ctx->len = HKS_KEY_BYTES(keyMaterial->keySize);

        uint32_t offset = sizeof(struct KeyMaterialDh);
        ret = mbedtls_mpi_read_binary(&ctx->GX, key->data + offset, keyMaterial->pubKeySize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            HKS_LOG_E("mbedtls_mpi_read_binary failed! mbedtls ret = 0x%X", ret);
            break;
        }

        if (needPrivateExponent) {
            offset = offset + keyMaterial->pubKeySize;
            ret = mbedtls_mpi_read_binary(&ctx->X, key->data + offset, keyMaterial->priKeySize);
            if (ret != HKS_MBEDTLS_SUCCESS) {
                ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
                HKS_LOG_E("mbedtls_mpi_read_binary failed! mbedtls ret = 0x%X", ret);
                break;
            }
        }

        ret = HKS_SUCCESS;
    } while (0);

    return ret;
}

int32_t HksMbedtlsDhAgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    if (HKS_KEY_BYTES(spec->keyLen) > sharedKey->size) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    struct KeyMaterialDh *pubKeyMaterial = (struct KeyMaterialDh *)pubKey->data;

    mbedtls_dhm_context ctx;
    mbedtls_dhm_init(&ctx);

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    int32_t ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        mbedtls_dhm_free(&ctx);
        return ret;
    }

    do {
        ret = DhKeyMaterialToCtx(nativeKey, true, &ctx);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = mbedtls_dhm_read_public(&ctx, pubKey->data + sizeof(struct KeyMaterialDh), pubKeyMaterial->pubKeySize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("mbedtls_dhm_read_public failed! mbedtls ret = 0x%X", ret);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }

        size_t keyLen;
        ret =
            mbedtls_dhm_calc_secret(&ctx, sharedKey->data, sharedKey->size, &keyLen, mbedtls_ctr_drbg_random, &ctrDrbg);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("mbedtls_dhm_calc_secret failed! mbedtls ret = 0x%X", ret);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }
        sharedKey->size = keyLen;
        ret = HKS_SUCCESS;
    } while (0);

    mbedtls_dhm_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);

    return ret;
}
#endif

#endif