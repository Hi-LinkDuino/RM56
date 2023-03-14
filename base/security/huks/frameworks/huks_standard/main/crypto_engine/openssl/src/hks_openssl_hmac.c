/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifdef HKS_SUPPORT_HMAC_C

#include "hks_openssl_hmac.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

#include "hks_common_check.h"
#include "hks_log.h"
#include "hks_openssl_common.h"
#include "hks_openssl_engine.h"
#include "hks_type_inner.h"
#include "hks_mem.h"
struct HksOpensslHmacCtx {
    uint32_t digestLen;
    void *append;
};

static int32_t HmacCheckBuffer(const struct HksBlob *key, const struct HksBlob *msg, const struct HksBlob *mac)
{
    if (HksOpensslCheckBlob(key) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid key point");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (HksOpensslCheckBlob(msg) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid msg");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (HksOpensslCheckBlob(mac) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid mac");
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

static int32_t HmacGenKeyCheckParam(const struct HksKeySpec *spec)
{
    if ((spec->keyLen == 0) || (spec->keyLen % BIT_NUM_OF_UINT8 != 0)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

#ifdef HKS_SUPPORT_HMAC_GENERATE_KEY
int32_t HksOpensslHmacGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    if (HmacGenKeyCheckParam(spec) != HKS_SUCCESS) {
        HKS_LOG_E("aes generate key invalid params!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HksOpensslGenerateRandomKey(spec->keyLen, key);
}
#endif /* HKS_SUPPORT_HMAC_GENERATE_KEY */

static int32_t HmacCheckParam(
    const struct HksBlob *key, uint32_t alg, const struct HksBlob *msg, const struct HksBlob *mac)
{
    if (HmacCheckBuffer(key, msg, mac) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid Buffer Info");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((alg != HKS_DIGEST_SHA1) && (alg != HKS_DIGEST_SHA224) && (alg != HKS_DIGEST_SHA256) &&
        (alg != HKS_DIGEST_SHA384) && (alg != HKS_DIGEST_SHA512)) {
        HKS_LOG_E("Invalid alg(0x%x)", alg);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint32_t digestLen;
    if (HksGetDigestLen(alg, &digestLen) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid alg(0x%x)", alg);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (mac->size < digestLen) {
        HKS_LOG_E("invalid mac->size(0x%x) for digestLen(0x%x)", mac->size, digestLen);
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    return HKS_SUCCESS;
}

#if defined(HKS_SUPPORT_HMAC_SHA1) || defined(HKS_SUPPORT_HMAC_SHA224) || defined(HKS_SUPPORT_HMAC_SHA256) || \
    defined(HKS_SUPPORT_HMAC_SHA384) || defined(HKS_SUPPORT_HMAC_SHA512)
int32_t HksOpensslHmac(const struct HksBlob *key, uint32_t digestAlg, const struct HksBlob *msg, struct HksBlob *mac)
{
    if (HmacCheckParam(key, digestAlg, msg, mac) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    const EVP_MD *opensslAlg = GetOpensslAlg(digestAlg);
    if (opensslAlg == NULL) {
        HKS_LOG_E("get openssl algorithm failed");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    uint8_t *hmacData = HMAC(opensslAlg, key->data, (int32_t)key->size, msg->data, msg->size, mac->data, &mac->size);
    if (hmacData == NULL) {
        HKS_LOG_E("hmac process failed.");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    return HKS_SUCCESS;
}

int32_t HksOpensslHmacInit(void **cryptoCtx, const struct HksBlob *key, uint32_t digestAlg)
{
    if (HksOpensslCheckBlob(key) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid key point");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((digestAlg != HKS_DIGEST_SHA1) && (digestAlg != HKS_DIGEST_SHA224) && (digestAlg != HKS_DIGEST_SHA256) &&
        (digestAlg != HKS_DIGEST_SHA384) && (digestAlg != HKS_DIGEST_SHA512)) {
        HKS_LOG_E("Invalid alg(0x%x)", digestAlg);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    const EVP_MD *opensslAlg = GetOpensslAlg(digestAlg);
    if (opensslAlg == NULL) {
        HKS_LOG_E("get openssl algorithm failed");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    uint32_t digestLen;
    if (HksGetDigestLen(digestAlg, &digestLen) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid alg(0x%x)", digestAlg);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    HMAC_CTX *tmpCtx = NULL;
    if ((tmpCtx = HMAC_CTX_new()) == NULL) {
        HKS_LOG_E("initialize HksOpensslHmacCtx failed");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    if (!HMAC_Init_ex(tmpCtx, key->data, (int32_t)key->size, opensslAlg, NULL)) {
        HKS_LOG_E("openssl hmac init failed.");
        HMAC_CTX_free(tmpCtx);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    struct HksOpensslHmacCtx *outCtx = (struct HksOpensslHmacCtx *)HksMalloc(
        sizeof(struct HksOpensslHmacCtx));

    if (outCtx == NULL) {
        HKS_LOG_E("initialize HksOpensslHmacCtx failed");
        HMAC_CTX_free(tmpCtx);
        return HKS_ERROR_MALLOC_FAIL;
    }

    outCtx->digestLen = digestLen;
    outCtx->append = (void *)tmpCtx;
    *cryptoCtx = (void *)outCtx;

    return HKS_SUCCESS;
}

int32_t HksOpensslHmacUpdate(void *cryptoCtx, const struct HksBlob *msg)
{
    if (HksOpensslCheckBlob(msg) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid key point");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct HksOpensslHmacCtx *hmacCtx = (struct HksOpensslHmacCtx *)cryptoCtx;
    if (hmacCtx == NULL) {
        HKS_LOG_E("hmacCtx invalid");
        return HKS_ERROR_NULL_POINTER;
    }

    HMAC_CTX *context = (HMAC_CTX *)hmacCtx->append;
    if (context == NULL) {
        HKS_LOG_E("context is null");
        return HKS_FAILURE;
    }

    int hmacData = HMAC_Update(context, msg->data, msg->size);
    if (!hmacData) {
        HKS_LOG_E("hmac update failed.");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    return HKS_SUCCESS;
}

int32_t HksOpensslHmacFinal(void **cryptoCtx, struct HksBlob *msg, struct HksBlob *mac)
{
    if ((cryptoCtx == NULL) || (*cryptoCtx == NULL)) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksOpensslHmacCtx *hmacCtx = (struct HksOpensslHmacCtx *)*cryptoCtx;
    if (hmacCtx == NULL) {
        HKS_LOG_E("hmacCtx invalid");
        HKS_FREE_PTR(*cryptoCtx);
        return HKS_ERROR_NULL_POINTER;
    }

    HMAC_CTX *context = (HMAC_CTX *)hmacCtx->append;
    if (context == NULL) {
        HKS_LOG_E("context is null");
        HKS_FREE_PTR(*cryptoCtx);
        return HKS_FAILURE;
    }

    if (msg == NULL || HksOpensslCheckBlob(mac) != HKS_SUCCESS) {
        HKS_LOG_E("Invalid msg or mac point");
        HksOpensslHmacHalFreeCtx(cryptoCtx);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int hmacData;
    if (msg->size != 0) {
        hmacData = HMAC_Update(context, msg->data, msg->size);
        if (!hmacData) {
            HKS_LOG_E("hmac final update failed.");
            HksOpensslHmacHalFreeCtx(cryptoCtx);
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }

    hmacData = HMAC_Final(context, mac->data, &mac->size);
    if (!hmacData) {
        HKS_LOG_E("hmac final failed.");
        HksOpensslHmacHalFreeCtx(cryptoCtx);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    HksOpensslHmacHalFreeCtx(cryptoCtx);
    return HKS_SUCCESS;
}

void HksOpensslHmacHalFreeCtx(void **cryptoCtx)
{
    if (cryptoCtx == NULL || *cryptoCtx == NULL) {
        HKS_LOG_E("Openssl hmac free ctx is null");
        return;
    }

    struct HksOpensslHmacCtx *opensslHmacCtx = (struct HksOpensslHmacCtx *)*cryptoCtx;
    if (opensslHmacCtx->append != NULL) {
        HMAC_CTX_free((HMAC_CTX *)opensslHmacCtx->append);
        opensslHmacCtx->append = NULL;
    }

    HKS_FREE_PTR(*cryptoCtx);
}
#endif /* HKS_SUPPORT_HMAC_SHA1 */
#endif /* HKS_SUPPORT_HMAC_C */