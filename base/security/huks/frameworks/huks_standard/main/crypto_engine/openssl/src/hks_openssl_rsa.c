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

#ifdef HKS_SUPPORT_RSA_C

#include "hks_openssl_rsa.h"

#include <openssl/evp.h>
#include <openssl/rsa.h>

#include "hks_log.h"
#include "hks_mem.h"
#include "hks_openssl_engine.h"
#include "hks_type_inner.h"

static int32_t RsaGenKeyCheckParam(const struct HksKeySpec *spec)
{
    switch (spec->keyLen) {
        case HKS_RSA_KEY_SIZE_512:
        case HKS_RSA_KEY_SIZE_768:
        case HKS_RSA_KEY_SIZE_1024:
        case HKS_RSA_KEY_SIZE_2048:
        case HKS_RSA_KEY_SIZE_3072:
        case HKS_RSA_KEY_SIZE_4096:
            return HKS_SUCCESS;
        default:
            HKS_LOG_E("Invlid rsa key len %x!", spec->keyLen);
            return HKS_ERROR_INVALID_ARGUMENT;
    }
}

static int32_t RsaCheckKeyMaterial(const struct HksBlob *key)
{
    const struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)(key->data);
    if (keyMaterial->keyAlg != HKS_ALG_RSA) {
        return HKS_ERROR_INVALID_KEY_INFO;
    }
    if (key->size != sizeof(struct KeyMaterialRsa) + keyMaterial->nSize + keyMaterial->eSize + keyMaterial->dSize) {
        return HKS_ERROR_INVALID_KEY_INFO;
    }
    return HKS_SUCCESS;
}

int32_t InitRsaKeyBuf(const struct KeyMaterialRsa *keyMaterial, struct HksBlob *bufBlob)
{
    uint32_t maxSize;
    if (keyMaterial->nSize >= keyMaterial->eSize) {
        maxSize = keyMaterial->nSize;
    } else {
        maxSize = keyMaterial->eSize;
    }

    if (maxSize < keyMaterial->dSize) {
        maxSize = keyMaterial->dSize;
    }

    bufBlob->data = (uint8_t *)HksMalloc(maxSize);
    if (bufBlob->data == NULL) {
        HKS_LOG_E("HksMalloc failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }
    bufBlob->size = maxSize;
    return HKS_SUCCESS;
}

static RSA *InitRsaStruct(const struct HksBlob *key, const bool needPrivateExponent)
{
    const struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)(key->data);
    struct HksBlob bufBlob = { 0, NULL };
    int32_t ret = InitRsaKeyBuf(keyMaterial, &bufBlob);
    if (ret != HKS_SUCCESS) {
        return NULL;
    }

    bool copyFail = false;
    uint32_t offset = sizeof(*keyMaterial);
    if (memcpy_s(bufBlob.data, bufBlob.size, key->data + offset, keyMaterial->nSize) != EOK) {
        copyFail = true;
    }
    BIGNUM *n = BN_bin2bn(bufBlob.data, keyMaterial->nSize, NULL);
    offset += keyMaterial->nSize;
    if (memcpy_s(bufBlob.data, bufBlob.size, key->data + offset, keyMaterial->eSize) != EOK) {
        copyFail = true;
    }
    BIGNUM *e = BN_bin2bn(bufBlob.data, keyMaterial->eSize, NULL);
    offset += keyMaterial->eSize;
    BIGNUM *d = NULL;
    if (needPrivateExponent) {
        if (memcpy_s(bufBlob.data, bufBlob.size, key->data + offset, keyMaterial->dSize) != EOK) {
            copyFail = true;
        }
        d = BN_bin2bn(bufBlob.data, keyMaterial->dSize, NULL);
    }

    RSA *rsa = NULL;
    do {
        if (copyFail) {
            break;
        }

        rsa = RSA_new();
        if (rsa != NULL) {
            int32_t ret = RSA_set0_key(rsa, n, e, d);
            if (ret != HKS_OPENSSL_SUCCESS) {
                RSA_free(rsa);
                rsa = NULL;
                break;
            }
        }
    } while (0);

    if (rsa == NULL) {
        SELF_FREE_PTR(n, BN_free);
        SELF_FREE_PTR(e, BN_free);
        SELF_FREE_PTR(d, BN_free);
    }

    (void)memset_s(bufBlob.data, bufBlob.size, 0, HKS_KEY_BYTES(keyMaterial->keySize));
    HksFree(bufBlob.data);
    return rsa;
}

#ifdef HKS_SUPPORT_RSA_GENERATE_KEY
static int32_t RsaSaveKeyMaterial(const RSA *rsa, const uint32_t keySize, struct HksBlob *key)
{
    const uint32_t keyByteLen = keySize / HKS_BITS_PER_BYTE;
    const uint32_t rawMaterialLen = sizeof(struct KeyMaterialRsa) + keyByteLen * HKS_RSA_KEYPAIR_CNT;
    uint8_t *rawMaterial = (uint8_t *)HksMalloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);

    struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_RSA;
    keyMaterial->keySize = keySize;

    uint8_t tmp_buff[keyByteLen];
    if (memset_s(tmp_buff, keyByteLen, 0, keyByteLen) != EOK) {
        HksFree(rawMaterial);
        return HKS_ERROR_INVALID_OPERATION;
    }

    uint32_t offset = sizeof(*keyMaterial);
    keyMaterial->nSize = (uint32_t)BN_bn2bin(RSA_get0_n(rsa), tmp_buff);
    if (memcpy_s(rawMaterial + offset, keyByteLen, tmp_buff, keyMaterial->nSize) != EOK) {
        HksFree(rawMaterial);
        return HKS_ERROR_INVALID_OPERATION;
    }

    offset += keyMaterial->nSize;
    keyMaterial->eSize = (uint32_t)BN_bn2bin(RSA_get0_e(rsa), tmp_buff);
    if (memcpy_s(rawMaterial + offset, keyByteLen, tmp_buff, keyMaterial->eSize) != EOK) {
        HksFree(rawMaterial);
        return HKS_ERROR_INVALID_OPERATION;
    }

    offset += keyMaterial->eSize;
    keyMaterial->dSize = (uint32_t)BN_bn2bin(RSA_get0_d(rsa), tmp_buff);
    if (memcpy_s(rawMaterial + offset, keyByteLen, tmp_buff, keyMaterial->dSize) != EOK) {
        HksFree(rawMaterial);
        return HKS_ERROR_INVALID_OPERATION;
    }

    key->data = rawMaterial;
    key->size = sizeof(struct KeyMaterialRsa) + keyMaterial->nSize + keyMaterial->eSize + keyMaterial->dSize;

    return HKS_SUCCESS;
}

int32_t HksOpensslRsaGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    if (RsaGenKeyCheckParam(spec) != HKS_SUCCESS) {
        HKS_LOG_E("rsa generate key invalid params!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    RSA *rsa = RSA_new();
    BIGNUM *e = BN_new();
    if (rsa == NULL || e == NULL) {
        SELF_FREE_PTR(rsa, RSA_free);
        SELF_FREE_PTR(e, BN_free);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    if (BN_set_word(e, RSA_F4) != HKS_OPENSSL_SUCCESS) {
        SELF_FREE_PTR(rsa, RSA_free);
        SELF_FREE_PTR(e, BN_free);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    if (RSA_generate_key_ex(rsa, spec->keyLen, e, NULL) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        BN_free(e);
        RSA_free(rsa);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    BN_free(e);

    int32_t ret = RsaSaveKeyMaterial(rsa, spec->keyLen, key);

    RSA_free(rsa);

    return ret;
}
#endif /* HKS_SUPPORT_RSA_GENERATE_KEY */

#ifdef HKS_SUPPORT_RSA_GET_PUBLIC_KEY
int32_t HksOpensslGetRsaPubKey(const struct HksBlob *input, struct HksBlob *output)
{
    struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)input->data;
    output->size = sizeof(struct KeyMaterialRsa) + keyMaterial->nSize + keyMaterial->eSize;

    struct KeyMaterialRsa *publickeyMaterial = (struct KeyMaterialRsa *)output->data;
    publickeyMaterial->keyAlg = keyMaterial->keyAlg;
    publickeyMaterial->keySize = keyMaterial->keySize;
    publickeyMaterial->nSize = keyMaterial->nSize;
    publickeyMaterial->eSize = keyMaterial->eSize;
    publickeyMaterial->dSize = 0;

    if (memcpy_s(output->data + sizeof(struct KeyMaterialRsa),
        output->size - sizeof(struct KeyMaterialRsa),
        input->data + sizeof(struct KeyMaterialRsa),
        keyMaterial->nSize + keyMaterial->eSize) != EOK) {
        return HKS_ERROR_INVALID_OPERATION;
    }

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_RSA_GET_PUBLIC_KEY */

#ifdef HKS_SUPPORT_RSA_CRYPT
static int32_t GetRsaCryptPadding(uint32_t padding, uint32_t *rsaPadding)
{
    switch (padding) {
#ifdef HKS_SUPPORT_RSA_ECB_NOPADDING
        case HKS_PADDING_NONE:
            *rsaPadding = RSA_NO_PADDING;
            return HKS_SUCCESS;
#endif
#ifdef HKS_SUPPORT_RSA_ECB_PKCS1PADDING
        case HKS_PADDING_PKCS1_V1_5:
            *rsaPadding = RSA_PKCS1_PADDING;
            return HKS_SUCCESS;
#endif
#if defined(HKS_SUPPORT_RSA_ECB_OEAPPADDING) || defined(HKS_SUPPORT_RSA_ECB_OAEPPADDING_SHA1MGF1) ||              \
    defined(HKS_SUPPORT_RSA_ECB_OAEPPADDING_SHA224MGF1) || defined(HKS_SUPPORT_RSA_ECB_OAEPPADDING_SHA256MGF1) || \
    defined(HKS_SUPPORT_RSA_ECB_OAEPPADDING_SHA384MGF1) || defined(HKS_SUPPORT_RSA_ECB_OAEPPADDING_SHA512MGF1)
        case HKS_PADDING_OAEP:
            *rsaPadding = RSA_PKCS1_OAEP_PADDING;
            return HKS_SUCCESS;
#endif
        default:
            return HKS_FAILURE;
    }
}

static EVP_PKEY_CTX *InitEvpPkeyCtx(const struct HksBlob *key, bool encrypt)
{
    int32_t ret = RsaCheckKeyMaterial(key);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check key material failed");
        return NULL;
    }

    RSA *rsa = InitRsaStruct(key, !encrypt);
    if (rsa == NULL) {
        HKS_LOG_E("initialize rsa key failed");
        return NULL;
    }

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        RSA_free(rsa);
        HksLogOpensslError();
        return NULL;
    }

    ret = EVP_PKEY_assign_RSA(pkey, rsa);
    if (ret != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        RSA_free(rsa);
        EVP_PKEY_free(pkey);
        return NULL;
    }

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (ctx == NULL) {
        HksLogOpensslError();
        EVP_PKEY_free(pkey);
        return NULL;
    }

    if (encrypt) {
        ret = EVP_PKEY_encrypt_init(ctx);
    } else {
        ret = EVP_PKEY_decrypt_init(ctx);
    }
    if (ret != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_free(pkey);
    return ctx;
}

static int32_t HksOpensslRsaCryptInit(EVP_PKEY_CTX *ctx, const struct HksUsageSpec *usageSpec)
{
    int32_t ret;
    uint32_t padding = 0;
    if (GetRsaCryptPadding(usageSpec->padding, &padding) != HKS_SUCCESS) {
        HKS_LOG_E("Unsupport padding.");
        return HKS_FAILURE;
    }

    ret = EVP_PKEY_CTX_set_rsa_padding(ctx, padding);
    if (ret <= 0) {
        HksLogOpensslError();
        return HKS_FAILURE;
    }

    if (usageSpec->padding == HKS_PADDING_OAEP) {
        const EVP_MD *md = GetOpensslAlg(usageSpec->digest);
        if (md == NULL || EVP_PKEY_CTX_set_rsa_oaep_md(ctx, md) <= 0 || EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, md) <= 0) {
            HksLogOpensslError();
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }
    return HKS_SUCCESS;
}

static int32_t HksOpensslRsaCrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const bool encrypt, struct HksBlob *cipherText)
{
    int32_t ret;
    EVP_PKEY_CTX *ctx = InitEvpPkeyCtx(key, encrypt);
    if (ctx == NULL) {
        HksLogOpensslError();
        return HKS_FAILURE;
    }

    if (HksOpensslRsaCryptInit(ctx, usageSpec) != HKS_SUCCESS) {
        EVP_PKEY_CTX_free(ctx);
        return HKS_FAILURE;
    }

    size_t outLen;
    if (encrypt) {
        ret = EVP_PKEY_encrypt(ctx, NULL, &outLen, message->data, message->size);
    } else {
        ret = EVP_PKEY_decrypt(ctx, NULL, &outLen, message->data, message->size);
    }
    if (ret != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }

    if (outLen > cipherText->size) {
        HksLogOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if (encrypt) {
        ret = EVP_PKEY_encrypt(ctx, cipherText->data, &outLen, message->data, message->size);
    } else {
        ret = EVP_PKEY_decrypt(ctx, cipherText->data, &outLen, message->data, message->size);
    }
    if (ret != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    cipherText->size = outLen;

    EVP_PKEY_CTX_free(ctx);
    return HKS_SUCCESS;
}

int32_t HksOpensslRsaEncrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText, struct HksBlob *tagAead)
{
    (void)tagAead;
    return HksOpensslRsaCrypt(key, usageSpec, message, true, cipherText);
}

int32_t HksOpensslRsaDecrypt(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *cipherText)
{
    return HksOpensslRsaCrypt(key, usageSpec, message, false, cipherText);
}
#endif /* HKS_SUPPORT_RSA_CRYPT */

#ifdef HKS_SUPPORT_RSA_SIGN_VERIFY
static int32_t GetRsaSignPadding(uint32_t padding, uint32_t *rsaPadding)
{
    switch (padding) {
        case HKS_PADDING_PKCS1_V1_5:
            *rsaPadding = RSA_PKCS1_PADDING;
            return HKS_SUCCESS;
        case HKS_PADDING_PSS:
            *rsaPadding = RSA_PKCS1_PSS_PADDING;
            return HKS_SUCCESS;
        default:
            return HKS_FAILURE;
    }
}

static int32_t SetRsaPadding(EVP_PKEY_CTX *ctx, const struct HksUsageSpec *usageSpec)
{
    uint32_t opensslPadding = 0;
    int32_t ret = GetRsaSignPadding(usageSpec->padding, &opensslPadding);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Unsupport padding.");
        return HKS_ERROR_INVALID_PADDING;
    }

    if (usageSpec->padding == HKS_PADDING_PSS) {
        if (EVP_PKEY_CTX_set_rsa_padding(ctx, opensslPadding) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            return HKS_ERROR_CRYPTO_ENGINE_ERROR;
        }
    }
    return HKS_SUCCESS;
}

static EVP_PKEY *InitRsaEvpKey(const struct HksBlob *key, bool signing)
{
    RSA *rsa = InitRsaStruct(key, signing);
    if (rsa == NULL) {
        HKS_LOG_E("initialize rsa key failed");
        return NULL;
    }

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        HKS_LOG_E("evp pkey new failed");
        SELF_FREE_PTR(rsa, RSA_free);
        return NULL;
    }

    if (EVP_PKEY_assign_RSA(pkey, rsa) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        SELF_FREE_PTR(rsa, RSA_free);
        SELF_FREE_PTR(pkey, EVP_PKEY_free);
        return NULL;
    }

    return pkey;
}

static EVP_PKEY_CTX *InitRsaCtx(const struct HksBlob *key, const struct HksUsageSpec *usageSpec, bool signing)
{
    uint32_t digest = (usageSpec->digest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : usageSpec->digest;

    const EVP_MD *opensslAlg = GetOpensslAlg(digest);
    if (opensslAlg == NULL) {
        HKS_LOG_E("get openssl algorithm fail");
        return NULL;
    }

    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    int32_t ret = HKS_FAILURE;
    do {
        pkey = InitRsaEvpKey(key, signing);
        if (pkey == NULL) {
            break;
        }

        ctx = EVP_PKEY_CTX_new(pkey, NULL);
        if (ctx == NULL) {
            break;
        }

        if (signing) {
            ret = EVP_PKEY_sign_init(ctx);
        } else {
            ret = EVP_PKEY_verify_init(ctx);
        }
        if (ret != HKS_OPENSSL_SUCCESS) {
            break;
        }

        ret = HKS_FAILURE;
        if (SetRsaPadding(ctx, usageSpec) != HKS_SUCCESS) {
            break;
        }
        if (EVP_PKEY_CTX_set_signature_md(ctx, opensslAlg) != HKS_OPENSSL_SUCCESS) {
            break;
        }
        ret = HKS_SUCCESS;
    } while (0);

    SELF_FREE_PTR(pkey, EVP_PKEY_free);
    if (ret != HKS_SUCCESS) {
        HksLogOpensslError();
        SELF_FREE_PTR(ctx, EVP_PKEY_CTX_free);
    }

    return ctx;
}

int32_t HksOpensslRsaSign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature)
{
    EVP_PKEY_CTX *ctx = InitRsaCtx(key, usageSpec, true);
    if (ctx == NULL) {
        HKS_LOG_E("initialize rsa context failed");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    size_t sigSize = (size_t)signature->size;
    if (EVP_PKEY_sign(ctx, signature->data, &sigSize, message->data, message->size) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    signature->size = (uint32_t)sigSize;
    EVP_PKEY_CTX_free(ctx);
    return HKS_SUCCESS;
}

int32_t HksOpensslRsaVerify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature)
{
    EVP_PKEY_CTX *ctx = InitRsaCtx(key, usageSpec, false);
    if (ctx == NULL) {
        HKS_LOG_E("initialize rsa context failed");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    if (EVP_PKEY_verify(ctx, signature->data, signature->size, message->data, message->size) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HKS_ERROR_CRYPTO_ENGINE_ERROR;
    }
    EVP_PKEY_CTX_free(ctx);
    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_RSA_SIGN_VERIFY */
#endif /* HKS_SUPPORT_RSA_C */