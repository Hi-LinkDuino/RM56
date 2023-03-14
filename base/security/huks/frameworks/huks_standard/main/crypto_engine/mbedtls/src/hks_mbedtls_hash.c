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
#undef HKS_SUPPORT_HASH_C
#endif /* _CUT_AUTHENTICATE_ */

#ifdef HKS_SUPPORT_HASH_C

#include "hks_mbedtls_hash.h"

#include <mbedtls/md5.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <mbedtls/sha512.h>

#include "hks_common_check.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_mbedtls_common.h"

struct HksMbedtlsHashCtx {
    uint8_t *append;
    uint32_t mAlg;
} HksMbedtlsHashCtx;

int32_t HksMbedtlsHashMd5Init(void **ctx, uint32_t alg);

int32_t HksMbedtlsHashMd5Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen);

/* 16 is the output length that mbedtls need */
int32_t HksMbedtlsHashMd5Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[16]);

int32_t HksMbedtlsHashSha1Init(void **ctx, uint32_t alg);

int32_t HksMbedtlsHashSha1Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen);

/* 20 is the output length that mbedtls need */
int32_t HksMbedtlsHashSha1Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[20]);

int32_t HksMbedtlsHashSha256Init(void **ctx, int is224, uint32_t alg);

int32_t HksMbedtlsHashSha256Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen);

/* 32 is the output length that mbedtls need */
int32_t HksMbedtlsHashSha256Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[32]);

int32_t HksMbedtlsHashSha512Init(void **ctx, int is384, uint32_t alg);

int32_t HksMbedtlsHashSha512Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen);

/* 64 is the output length that mbedtls need */
int32_t HksMbedtlsHashSha512Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[64]);

int32_t HksMbedtlsHashInit(void **cryptoCtx, uint32_t digestAlg)
{
    int32_t ret;
    switch (digestAlg) {
        case HKS_DIGEST_MD5:
            ret = HksMbedtlsHashMd5Init(cryptoCtx, digestAlg); /* 0 for MD5 */
            break;
        case HKS_DIGEST_SHA1:
            ret = HksMbedtlsHashSha1Init(cryptoCtx, digestAlg); /* 0 for SHA-1 */
            break;
        case HKS_DIGEST_SHA224:
            ret = HksMbedtlsHashSha256Init(cryptoCtx, 1, digestAlg); /* 0 for SHA-224 */
            break;
        case HKS_DIGEST_SHA256:
            ret = HksMbedtlsHashSha256Init(cryptoCtx, 0, digestAlg); /* 0 for SHA-256 */
            break;
        case HKS_DIGEST_SHA384:
            ret = HksMbedtlsHashSha512Init(cryptoCtx, 1, digestAlg); /* 1 for SHA-384 */
            break;
        case HKS_DIGEST_SHA512:
            ret = HksMbedtlsHashSha512Init(cryptoCtx, 0, digestAlg); /* 0 for SHA-512 */
            break;
        default:
            return HKS_ERROR_INVALID_DIGEST;
    }

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Mbedtls hash init failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashUpdate(void *cryptoCtx, const struct HksBlob *msg)
{
    int32_t ret;
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)cryptoCtx;

    switch (rsaCtx->mAlg) {
        case HKS_DIGEST_MD5:
            ret = HksMbedtlsHashMd5Update(rsaCtx, msg->data, msg->size); /* 0 for MD5 */
            break;
        case HKS_DIGEST_SHA1:
            ret = HksMbedtlsHashSha1Update(rsaCtx, msg->data, msg->size); /* 0 for SHA-1 */
            break;
        case HKS_DIGEST_SHA224:
        case HKS_DIGEST_SHA256:
            ret = HksMbedtlsHashSha256Update(rsaCtx, msg->data, msg->size);
            break;
        case HKS_DIGEST_SHA384:
        case HKS_DIGEST_SHA512:
            ret = HksMbedtlsHashSha512Update(rsaCtx, msg->data, msg->size);
            break;
        default:
            return HKS_ERROR_INVALID_DIGEST;
    }

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Mbedtls hash update failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashFinal(void **cryptoCtx, const struct HksBlob *msg, struct HksBlob *hash)
{
    int32_t ret;
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)*cryptoCtx;

    switch (rsaCtx->mAlg) {
        case HKS_DIGEST_MD5:
            ret = HksMbedtlsHashMd5Final(rsaCtx, msg, hash->data); /* 0 for MD5 */
            break;
        case HKS_DIGEST_SHA1:
            ret = HksMbedtlsHashSha1Final(rsaCtx, msg, hash->data); /* 0 for SHA-1 */
            break;
        case HKS_DIGEST_SHA224:
        case HKS_DIGEST_SHA256:
            ret = HksMbedtlsHashSha256Final(rsaCtx, msg, hash->data);
            break;
        case HKS_DIGEST_SHA384:
        case HKS_DIGEST_SHA512:
            ret = HksMbedtlsHashSha512Final(rsaCtx, msg, hash->data);
            break;
        default:
            return HKS_ERROR_INVALID_DIGEST;
    }

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Mbedtls hash final failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    ret = HksGetDigestLen(rsaCtx->mAlg, &(hash->size));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Get digest len failed!");
        return ret;
    }

    if (ret == HKS_SUCCESS) {
        HksFree(rsaCtx);
        rsaCtx = NULL;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashMd5Init(void **ctx, uint32_t alg)
{
    int32_t ret;
    mbedtls_md5_context *context = (mbedtls_md5_context *)HksMalloc(sizeof(mbedtls_md5_context));
    if (context == NULL) {
        HKS_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }

    mbedtls_md5_init(context);

    ret = mbedtls_md5_starts_ret(context);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash Md5 init starts_ret fail");
        mbedtls_md5_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    struct HksMbedtlsHashCtx *outCtx = (struct HksMbedtlsHashCtx *)HksMalloc(sizeof(HksMbedtlsHashCtx));
    if (outCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash Md5 init malloc fail");
        mbedtls_md5_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_MALLOC_FAIL;
    }

    outCtx->append = (void *)context;
    outCtx->mAlg = alg;
    *ctx = (void *)outCtx;
    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashMd5Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen)
{
    int32_t ret;
    mbedtls_md5_context *context = (mbedtls_md5_context*)ctx->append;

    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    ret = mbedtls_md5_update_ret(context, input, ilen);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash Md5 update fail");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    return HKS_SUCCESS;
}

/* 16 is the output length that mbedtls need */
int32_t HksMbedtlsHashMd5Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[16])
{
    int32_t ret;
    mbedtls_md5_context *context = (mbedtls_md5_context*)ctx->append;

    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    if (msg->size != 0) {
        ret = mbedtls_md5_update_ret(context, msg->data, msg->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls Hash Md5 finish last data fail");
            mbedtls_md5_free(context);
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }

    ret = mbedtls_md5_finish_ret(context, output);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash Md5 finish fail");
        mbedtls_md5_free(context);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    mbedtls_md5_free(context);
    HksFree(context);
    ctx->append = NULL;

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashSha1Init(void **ctx, uint32_t alg)
{
    int32_t ret;
    mbedtls_sha1_context *context = (mbedtls_sha1_context *)HksMalloc(sizeof(mbedtls_sha1_context));
    if (context == NULL) {
        HKS_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }
    mbedtls_sha1_init(context);

    ret = mbedtls_sha1_starts_ret(context);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash Sha1 init start_rat fail");
        mbedtls_sha1_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    struct HksMbedtlsHashCtx *outCtx = (struct HksMbedtlsHashCtx *)HksMalloc(sizeof(HksMbedtlsHashCtx));
    if (outCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash Sha1 init malloc fail");
        mbedtls_sha1_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_MALLOC_FAIL;
    }

    outCtx->append = (void *)context;
    outCtx->mAlg = alg;
    *ctx = (void *)outCtx;

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashSha1Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen)
{
    mbedtls_sha1_context *context = (mbedtls_sha1_context*)ctx->append;
    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    if (ilen == 0 || input == NULL) {
        HKS_LOG_E("Mbedtls Hash sha1 input param error");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int32_t ret = mbedtls_sha1_update_ret(context, input, ilen);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha1 update fail");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    return HKS_SUCCESS;
}

/* 20 is the output length that mbedtls need */
int32_t HksMbedtlsHashSha1Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[20])
{
    mbedtls_sha1_context *context = (mbedtls_sha1_context*)ctx->append;
    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    if ((unsigned char *)output == NULL) {
        HKS_LOG_E("Mbedtls Hash sha1 output param error");
        mbedtls_sha1_free(context);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int32_t ret;
    if (msg->size != 0) {
        ret = mbedtls_sha1_update_ret(context, msg->data, msg->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls Hash sha1 finish last data fail");
            mbedtls_sha1_free(context);
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }

    ret = mbedtls_sha1_finish_ret(context, output);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha1 finish fail");
        mbedtls_sha1_free(context);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    mbedtls_sha1_free(context);
    HksFree(context);
    ctx->append = NULL;

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashSha256Init(void **ctx, int is224, uint32_t alg)
{
    mbedtls_sha256_context *context = (mbedtls_sha256_context *)HksMalloc(sizeof(mbedtls_sha256_context));
    if (context == NULL) {
        HKS_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (is224 != 0 && is224 != 1) {
        HKS_LOG_E("Mbedtls Hash not sha224 & not sha256 ");
        mbedtls_sha256_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    mbedtls_sha256_init(context);

    int32_t ret = mbedtls_sha256_starts_ret(context, is224);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha256 init fail");
        mbedtls_sha256_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    struct HksMbedtlsHashCtx *outCtx = (struct HksMbedtlsHashCtx *)HksMalloc(sizeof(HksMbedtlsHashCtx));
    if (outCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash sha256 init fail_outCtx malloc fail");
        mbedtls_sha256_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_MALLOC_FAIL;
    }

    outCtx->append = (void *)context;
    outCtx->mAlg = alg;
    *ctx = (void *)outCtx;
    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashSha256Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen)
{
    mbedtls_sha256_context *context = (mbedtls_sha256_context*)ctx->append;
    if (context == NULL) {
        return HKS_FAILURE;
    }

    if (ilen == 0 || input == NULL) {
        return HKS_FAILURE;
    }

    int32_t ret = mbedtls_sha256_update_ret(context, input, ilen);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha256 update fail");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    return HKS_SUCCESS;
}

/* 32 is the output length that mbedtls need */
int32_t HksMbedtlsHashSha256Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[32])
{
    mbedtls_sha256_context *context = (mbedtls_sha256_context*)ctx->append;
    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    if ((unsigned char *)output == NULL) {
        HKS_LOG_E("Mbedtls Hash sha256 output param error");
        mbedtls_sha256_free(context);
        return HKS_FAILURE;
    }

    int32_t ret;
    if (msg->size != 0) {
        ret = mbedtls_sha256_update_ret(context, msg->data, msg->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls Hash sha256 update fail");
            mbedtls_sha256_free(context);
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }

    ret = mbedtls_sha256_finish_ret(context, output);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha256 finish fail");
        mbedtls_sha256_free(context);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    mbedtls_sha256_free(context);
    HksFree(context);
    ctx->append = NULL;

    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashSha512Init(void **ctx, int is384, uint32_t alg)
{
    mbedtls_sha512_context *context = (mbedtls_sha512_context *)HksMalloc(sizeof(mbedtls_sha512_context));
    if (context == NULL) {
        HKS_LOG_E("malloc fail");
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (is384 != 0 && is384 != 1) {
        HKS_LOG_E("Mbedtls Hash not sha384 & not sha512 ");
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_FAILURE;
    }

    mbedtls_sha512_init(context);

    int32_t ret = mbedtls_sha512_starts_ret(context, is384);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha512 init fail");
        mbedtls_sha512_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    struct HksMbedtlsHashCtx *outCtx = (struct HksMbedtlsHashCtx *)HksMalloc(sizeof(HksMbedtlsHashCtx));
    if (outCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash Md5 init fail_outCtx malloc fail");
        mbedtls_sha512_free(context);
        if (context != NULL) {
            HksFree(context);
        }
        return HKS_ERROR_MALLOC_FAIL;
    }

    outCtx->append = (void *)context;
    outCtx->mAlg = alg;
    *ctx = (void *)outCtx;
    return HKS_SUCCESS;
}

int32_t HksMbedtlsHashSha512Update(struct HksMbedtlsHashCtx *ctx, const unsigned char *input, size_t ilen)
{
    mbedtls_sha512_context *context = (mbedtls_sha512_context *)ctx->append;
    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    if (ilen == 0 || input == NULL) {
        HKS_LOG_E("Mbedtls Hash sha512 input param error");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int32_t ret = mbedtls_sha512_update_ret(context, input, ilen);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha512 update fail");
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    return HKS_SUCCESS;
}

/* 64 is the output length that mbedtls need */
int32_t HksMbedtlsHashSha512Final(struct HksMbedtlsHashCtx *ctx, const struct HksBlob *msg, unsigned char output[64])
{
    mbedtls_sha512_context *context = (mbedtls_sha512_context *)ctx->append;
    if (context == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    if ((unsigned char *)output == NULL) {
        HKS_LOG_E("Mbedtls Hash sha512 output param error");
        mbedtls_sha512_free(context);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int32_t ret;
    if (msg->size != 0) {
        ret = mbedtls_sha512_update_ret(context, msg->data, msg->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls Hash sha512 finish last data fail");
            mbedtls_sha512_free(context);
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }

    ret = mbedtls_sha512_finish_ret(context, output);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls Hash sha512 finish fail");
        mbedtls_sha512_free(context);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    mbedtls_sha512_free(context);
    HksFree(context);
    ctx->append = NULL;
    return HKS_SUCCESS;
}

void HksMbedtlsMd5HashFreeCtx(void **cryptoCtx)
{
    if (cryptoCtx == NULL || *cryptoCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash freeCtx param error");
        return;
    }
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)*cryptoCtx;
    mbedtls_md5_free((mbedtls_md5_context *)rsaCtx->append); /* 0 for MD5 */
    if (rsaCtx->append != NULL) {
        HksFree(rsaCtx->append);
        rsaCtx->append = NULL;
    }
}

void HksMbedtlsSHA1HashFreeCtx(void **cryptoCtx)
{
    if (cryptoCtx == NULL || *cryptoCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash freeCtx param error");
        return;
    }
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)*cryptoCtx;
    mbedtls_sha1_free((mbedtls_sha1_context *)rsaCtx->append); /* 0 for MD5 */
    if (rsaCtx->append != NULL) {
        HksFree(rsaCtx->append);
        rsaCtx->append = NULL;
    }
}

void HksMbedtlsSha224Sha256HashFreeCtx(void **cryptoCtx)
{
    if (cryptoCtx == NULL || *cryptoCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash freeCtx param error");
        return;
    }
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)*cryptoCtx;
    if (rsaCtx->append != NULL) {
        mbedtls_sha256_free((mbedtls_sha256_context *)rsaCtx->append);
        if (rsaCtx->append != NULL) {
            HksFree(rsaCtx->append);
            rsaCtx->append = NULL;
        }
    }
}

void HksMbedtlsSha384Sha512HashFreeCtx(void **cryptoCtx)
{
    if (cryptoCtx == NULL || *cryptoCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash freeCtx param error");
        return;
    }
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)*cryptoCtx;
    if (rsaCtx->append != NULL) {
        mbedtls_sha512_free((mbedtls_sha512_context *)rsaCtx->append);
        if (rsaCtx->append != NULL) {
            HksFree(rsaCtx->append);
            rsaCtx->append = NULL;
        }
    }
}

void HksMbedtlsHashFreeCtx(void **cryptoCtx)
{
    if (cryptoCtx == NULL || *cryptoCtx == NULL) {
        HKS_LOG_E("Mbedtls Hash freeCtx param error");
        return;
    }
    struct HksMbedtlsHashCtx *rsaCtx = (struct HksMbedtlsHashCtx *)*cryptoCtx;
    switch (rsaCtx->mAlg) {
        case HKS_DIGEST_MD5:
            HksMbedtlsMd5HashFreeCtx(cryptoCtx);
            break;
        case HKS_DIGEST_SHA1:
            HksMbedtlsSHA1HashFreeCtx(cryptoCtx);
            break;
        case HKS_DIGEST_SHA224:
        case HKS_DIGEST_SHA256:
            HksMbedtlsSha224Sha256HashFreeCtx(cryptoCtx);
            break;
        case HKS_DIGEST_SHA384:
        case HKS_DIGEST_SHA512:
            HksMbedtlsSha384Sha512HashFreeCtx(cryptoCtx);
            break;
        default:
            break;
    }

    if (*cryptoCtx != NULL) {
        HksFree(*cryptoCtx);
        *cryptoCtx = NULL;
    }
}

int32_t HksMbedtlsHash(uint32_t alg, const struct HksBlob *msg, struct HksBlob *hash)
{
    int32_t ret;
    switch (alg) {
#ifdef HKS_SUPPORT_HASH_MD5
        case HKS_DIGEST_MD5:
            ret = mbedtls_md5_ret(msg->data, msg->size, hash->data); /* 0 for MD5 */
            break;
#endif
#ifdef HKS_SUPPORT_HASH_SHA1
        case HKS_DIGEST_SHA1:
            ret = mbedtls_sha1_ret(msg->data, msg->size, hash->data); /* 0 for SHA-1 */
            break;
#endif
#ifdef HKS_SUPPORT_HASH_SHA224
        case HKS_DIGEST_SHA224:
            ret = mbedtls_sha256_ret(msg->data, msg->size, hash->data, 1); /* 0 for SHA-224 */
            break;
#endif
#ifdef HKS_SUPPORT_HASH_SHA256
        case HKS_DIGEST_SHA256:
            ret = mbedtls_sha256_ret(msg->data, msg->size, hash->data, 0); /* 0 for SHA-256 */
            break;
#endif
#ifdef HKS_SUPPORT_HASH_SHA384
        case HKS_DIGEST_SHA384:
            ret = mbedtls_sha512_ret(msg->data, msg->size, hash->data, 1); /* 1 for SHA-384 */
            break;
#endif
#ifdef HKS_SUPPORT_HASH_SHA512
        case HKS_DIGEST_SHA512:
            ret = mbedtls_sha512_ret(msg->data, msg->size, hash->data, 0); /* 0 for SHA-512 */
            break;
#endif
        default:
            return HKS_ERROR_INVALID_DIGEST;
    }

    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls hash failed! mbedtls ret = 0x%X", ret);
        return HKS_FAILURE;
    }

    ret = HksGetDigestLen(alg, &(hash->size));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Get digest len failed!");
    }

    return ret;
}
#endif /* HKS_SUPPORT_HASH_C */
