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

#ifdef _CUT_AUTHENTICATE_
#undef HKS_SUPPORT_RSA_C
#endif

#ifdef HKS_SUPPORT_RSA_C

#include "hks_mbedtls_rsa.h"

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/rsa.h>

#include "hks_log.h"
#include "hks_mbedtls_common.h"
#include "hks_mem.h"

#define HKS_RSA_PUBLIC_EXPONENT 65537
#define HKS_RSA_KEYPAIR_CNT 3

static int32_t RsaCheckKeySize(const uint32_t keySize)
{
    switch (keySize) {
        case HKS_RSA_KEY_SIZE_512:
        case HKS_RSA_KEY_SIZE_768:
        case HKS_RSA_KEY_SIZE_1024:
        case HKS_RSA_KEY_SIZE_2048:
        case HKS_RSA_KEY_SIZE_3072:
        case HKS_RSA_KEY_SIZE_4096:
            break;
        default:
            HKS_LOG_E("Invalid rsa key size! keySize = 0x%X", keySize);
            return HKS_ERROR_INVALID_KEY_SIZE;
    }

    return HKS_SUCCESS;
}

static int32_t RsaKeyMaterialNedSizeCheck(const struct KeyMaterialRsa *keyMaterial)
{
    const uint32_t maxKeyByteLen = HKS_RSA_KEY_SIZE_4096 / HKS_BITS_PER_BYTE;
    if ((keyMaterial->nSize > maxKeyByteLen) || (keyMaterial->eSize > maxKeyByteLen) ||
        (keyMaterial->dSize > maxKeyByteLen)) {
        HKS_LOG_E("Invalid rsa keyMaterial! nSize = 0x%X, eSize = 0x%X, dSize = 0x%X",
            keyMaterial->nSize,
            keyMaterial->eSize,
            keyMaterial->dSize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

static int32_t RsaKeyCheck(const struct HksBlob *key)
{
    const struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)(key->data);

    int32_t ret = RsaCheckKeySize(keyMaterial->keySize);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = RsaKeyMaterialNedSizeCheck(keyMaterial);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    if (key->size < (sizeof(struct KeyMaterialRsa) + keyMaterial->nSize + keyMaterial->eSize + keyMaterial->dSize)) {
        HKS_LOG_E("Rsa key size too small! key size = 0x%X", key->size);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    return HKS_SUCCESS;
}

#ifdef HKS_SUPPORT_RSA_GENERATE_KEY
static int32_t RsaSaveKeyMaterial(const mbedtls_rsa_context *ctx, const uint32_t keySize, struct HksBlob *key)
{
    const uint32_t keyByteLen = keySize / HKS_BITS_PER_BYTE;
    const uint32_t rawMaterialLen = sizeof(struct KeyMaterialRsa) + keyByteLen * HKS_RSA_KEYPAIR_CNT;
    uint8_t *rawMaterial = (uint8_t *)HksMalloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);

    /* RSA key data internal struct: struct KeyMaterialRsa + nData + eData + dData */
    struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_RSA;
    keyMaterial->keySize = keySize;
    keyMaterial->nSize = keyByteLen;
    keyMaterial->eSize = keyByteLen;
    keyMaterial->dSize = keyByteLen;

    int32_t ret;
    do {
        uint32_t offset = sizeof(*keyMaterial);
        ret = mbedtls_mpi_write_binary(&(ctx->N), rawMaterial + offset, keyMaterial->nSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Rsa save keyMaterial mpi write N failed! mbedtls ret = 0x%X", ret);
            break;
        }

        offset = offset + keyMaterial->nSize;
        ret = mbedtls_mpi_write_binary(&(ctx->E), rawMaterial + offset, keyMaterial->eSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Rsa save keyMaterial mpi write E failed! mbedtls ret = 0x%X", ret);
            break;
        }

        offset = offset + keyMaterial->eSize;
        ret = mbedtls_mpi_write_binary(&(ctx->D), rawMaterial + offset, keyMaterial->dSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Rsa save keyMaterial mpi write D failed! mbedtls ret = 0x%X", ret);
            break;
        }

        key->data = rawMaterial;
        key->size = rawMaterialLen;
    } while (0);

    if (ret != HKS_MBEDTLS_SUCCESS) {
        (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
        HKS_FREE_PTR(rawMaterial);
        ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    return ret;
}

int32_t HksMbedtlsRsaGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    mbedtls_rsa_context ctx;
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
    ctx.padding = 0;
    ctx.hash_id = 0;

    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_entropy_context entropy;
    int32_t ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        mbedtls_rsa_free(&ctx);
        return ret;
    }

    do {
        ret = mbedtls_rsa_gen_key(&ctx, mbedtls_ctr_drbg_random, &ctrDrbg, spec->keyLen, HKS_RSA_PUBLIC_EXPONENT);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa generate key failed! mbedtls ret = 0x%X", ret);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }

        ret = RsaSaveKeyMaterial(&ctx, spec->keyLen, key);
    } while (0);

    mbedtls_rsa_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}
#endif /* HKS_SUPPORT_RSA_GENERATE_KEY */

#if defined(HKS_SUPPORT_RSA_CRYPT) || defined(HKS_SUPPORT_RSA_SIGN_VERIFY)
static int32_t RsaKeyMaterialToCtx(const struct HksBlob *key, const bool needPrivateExponent, mbedtls_rsa_context *ctx)
{
    const struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)(key->data);

    mbedtls_mpi n;
    mbedtls_mpi e;
    mbedtls_mpi d;

    mbedtls_mpi_init(&n);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&d);

    int32_t ret;
    do {
        uint32_t offset = sizeof(*keyMaterial);
        ret = mbedtls_mpi_read_binary(&n, key->data + offset, keyMaterial->nSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa keyMaterial to ctx read N failed! mbedtls ret = 0x%X", ret);
            break;
        }

        offset = offset + keyMaterial->nSize;
        ret = mbedtls_mpi_read_binary(&e, key->data + offset, keyMaterial->eSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa keyMaterial to ctx read E failed! mbedtls ret = 0x%X", ret);
            break;
        }

        if (needPrivateExponent) {
            offset = offset + keyMaterial->eSize;
            ret = mbedtls_mpi_read_binary(&d, key->data + offset, keyMaterial->dSize);
            if (ret != HKS_MBEDTLS_SUCCESS) {
                HKS_LOG_E("Mbedtls rsa keyMaterial to ctx read D failed! mbedtls ret = 0x%X", ret);
                break;
            }
        }

        ret = mbedtls_rsa_import(ctx, &n, NULL, NULL, (needPrivateExponent ? &d : NULL), &e);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa keyMaterial to ctx import failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = mbedtls_rsa_complete(ctx);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa keyMaterial to ctx complete failed! mbedtls ret = 0x%X", ret);
        }
    } while (0);

    mbedtls_mpi_free(&n);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&d);

    if (ret != HKS_MBEDTLS_SUCCESS) {
        ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    return ret;
}
#endif /* HKS_SUPPORT_RSA_CRYPT or HKS_SUPPORT_RSA_SIGN_VERIFY */

#ifdef HKS_SUPPORT_RSA_CRYPT
static int32_t HksToMbedtlsPadding(uint32_t hksPadding, int32_t *padding)
{
    switch (hksPadding) {
        case HKS_PADDING_PKCS1_V1_5:
            *padding = MBEDTLS_RSA_PKCS_V15;
            break;
        case HKS_PADDING_OAEP:
            *padding = MBEDTLS_RSA_PKCS_V21;
            break;
        default:
            return HKS_ERROR_NOT_SUPPORTED;
    }
    return HKS_SUCCESS;
}

static int32_t HksMbedtlsRsaCrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const bool encrypt, struct HksBlob *cipherText)
{
    int32_t ret = RsaKeyCheck(key);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    int32_t padding;
    ret = HksToMbedtlsPadding(usageSpec->padding, &padding);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    uint32_t mbedtlsAlg;
    if (padding == MBEDTLS_RSA_PKCS_V21) {
        ret = HksToMbedtlsDigestAlg(usageSpec->digest, &mbedtlsAlg);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }

    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_entropy_context entropy;
    ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_rsa_context ctx;
    mbedtls_rsa_init(&ctx, padding, mbedtlsAlg); /* only support oaep padding */

    do {
        ret = RsaKeyMaterialToCtx(key, !encrypt, &ctx); /* encrypt don't need private exponent (d) */
        if (ret != HKS_SUCCESS) {
            break;
        }

        size_t outlen;
        if (encrypt) {
            ret = mbedtls_rsa_pkcs1_encrypt(&ctx, mbedtls_ctr_drbg_random,
                &ctrDrbg, MBEDTLS_RSA_PUBLIC, (size_t)message->size, message->data, cipherText->data);
            outlen = mbedtls_rsa_get_len(&ctx);
        } else {
            ret = mbedtls_rsa_pkcs1_decrypt(&ctx, mbedtls_ctr_drbg_random, &ctrDrbg, MBEDTLS_RSA_PRIVATE,
                &outlen, message->data, cipherText->data, (size_t)cipherText->size);
        }
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa crypt failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(cipherText->data, cipherText->size, 0, cipherText->size);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            break;
        }
        cipherText->size = (uint32_t)outlen;
    } while (0);

    mbedtls_rsa_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int32_t HksMbedtlsRsaEncrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText, struct HksBlob *tagAead)
{
    (void)tagAead;
    return HksMbedtlsRsaCrypt(key, usageSpec, message, true, cipherText);
}

int32_t HksMbedtlsRsaDecrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText)
{
    return HksMbedtlsRsaCrypt(key, usageSpec, message, false, cipherText);
}
#endif /* HKS_SUPPORT_RSA_CRYPT */

#ifdef HKS_SUPPORT_RSA_SIGN_VERIFY
static int32_t HksToMbedtlsSignPadding(uint32_t hksPadding, int32_t *padding)
{
    switch (hksPadding) {
        case HKS_PADDING_PKCS1_V1_5:
            *padding = MBEDTLS_RSA_PKCS_V15;
            break;
        case HKS_PADDING_PSS:
            *padding = MBEDTLS_RSA_PKCS_V21;
            break;
        default:
            return HKS_ERROR_NOT_SUPPORTED;
    }
    return HKS_SUCCESS;
}

static int32_t HksMbedtlsRsaSignVerify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const bool sign, struct HksBlob *signature)
{
    uint32_t mbedtlsAlg;
    uint32_t digest = (usageSpec->digest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : usageSpec->digest;
    int32_t ret = HksToMbedtlsDigestAlg(digest, &mbedtlsAlg);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    int32_t padding;
    ret = HksToMbedtlsSignPadding(usageSpec->padding, &padding);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_entropy_context entropy;
    ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_rsa_context ctx;
    mbedtls_rsa_init(&ctx, padding, mbedtlsAlg);

    do {
        ret = RsaKeyMaterialToCtx(key, sign, &ctx); /* sign need private exponent (d) */
        if (ret != HKS_SUCCESS) {
            break;
        }

        if (sign) {
            ret = mbedtls_rsa_pkcs1_sign(&ctx, mbedtls_ctr_drbg_random, &ctrDrbg, MBEDTLS_RSA_PRIVATE,
                mbedtlsAlg, message->size, message->data, signature->data);
        } else {
            ret = mbedtls_rsa_pkcs1_verify(&ctx, mbedtls_ctr_drbg_random, &ctrDrbg, MBEDTLS_RSA_PUBLIC,
                mbedtlsAlg, message->size, message->data, signature->data);
        }
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls rsa sign/verify failed! mbedtls ret = 0x%X", ret);
            ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
            (void)memset_s(signature->data, signature->size, 0, signature->size);
        }
    } while (0);

    if (sign) {
        signature->size = mbedtls_rsa_get_len(&ctx);
    }

    mbedtls_rsa_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int32_t HksMbedtlsRsaSign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature)
{
    int32_t ret = RsaKeyCheck(key);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksMbedtlsRsaSignVerify(key, usageSpec, message, true, signature); /* true: is sign */
}

int32_t HksMbedtlsRsaVerify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature)
{
    int32_t ret = RsaKeyCheck(key);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksMbedtlsRsaSignVerify(key, usageSpec, message, false, (struct HksBlob *)signature); /* false: is verify */
}
#endif /* HKS_SUPPORT_RSA_SIGN_VERIFY */

#ifdef HKS_SUPPORT_RSA_GET_PUBLIC_KEY
static int32_t GetRsaPubKeyCheckParams(const struct HksBlob *keyIn, const struct HksBlob *keyOut)
{
    int32_t ret = RsaKeyCheck(keyIn);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* check keyOut size */
    const struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)(keyIn->data);
    if (keyOut->size < (sizeof(struct HksPubKeyInfo) + keyMaterial->nSize + keyMaterial->eSize)) {
        HKS_LOG_E("Rsa public keyOut size too small! keyOut size = 0x%X", keyOut->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsGetRsaPubKey(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    int32_t ret = GetRsaPubKeyCheckParams(keyIn, keyOut);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* n + e, so need size is: sizeof(struct HksPubKeyInfo) + nSize + eSize */
    const struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)(keyIn->data);
    const uint32_t outLen = sizeof(struct HksPubKeyInfo) + keyMaterial->nSize + keyMaterial->eSize;
    if (memcpy_s(keyOut->data, keyOut->size, (void *)keyMaterial, outLen) != EOK) {
        HKS_LOG_E("Memcpy rsa pub key failed!");
        (void)memset_s(keyOut->data, keyOut->size, 0, keyOut->size);
        return HKS_ERROR_BAD_STATE;
    }
    ((struct KeyMaterialRsa *)(keyOut->data))->dSize = 0;
    keyOut->size = outLen;

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_RSA_GET_PUBLIC_KEY */
#endif /* HKS_SUPPORT_RSA_C */
