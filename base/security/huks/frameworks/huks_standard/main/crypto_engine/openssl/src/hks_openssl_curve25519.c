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

#include "hks_openssl_curve25519.h"

#include <openssl/evp.h>

#include "hks_crypto_adapter.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_openssl_ed25519tox25519.h"
#include "hks_openssl_engine.h"

static int32_t SaveCurve25519KeyMaterial(uint32_t algType, const EVP_PKEY *pKey, struct HksBlob *keyOut)
{
    uint32_t totalSize = sizeof(struct KeyMaterial25519) + (CURVE25519_KEY_LEN << 1);
    uint8_t *buffer = (uint8_t *)HksMalloc(totalSize);
    if (buffer == NULL) {
        HKS_LOG_E("malloc size %u failed", totalSize);
        return HKS_ERROR_MALLOC_FAIL;
    }

    size_t tmpPubKeyLen = CURVE25519_KEY_LEN;
    size_t tmpPriKeyLen = CURVE25519_KEY_LEN;
    uint32_t offset = sizeof(struct KeyMaterial25519);

    if (EVP_PKEY_get_raw_public_key(pKey, buffer + offset, &tmpPubKeyLen) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        HksFree(buffer);
        return HKS_ERROR_BAD_STATE;
    }
    uint32_t pubKeyLen = (uint32_t)tmpPubKeyLen;

    offset += pubKeyLen;
    if (EVP_PKEY_get_raw_private_key(pKey, buffer + offset, &tmpPriKeyLen) != HKS_OPENSSL_SUCCESS) {
        HksLogOpensslError();
        HksFree(buffer);
        return HKS_ERROR_BAD_STATE;
    }
    uint32_t priKeyLen = (uint32_t)tmpPriKeyLen;

    struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)buffer;
    keyMaterial->keyAlg = algType;
    keyMaterial->keySize = CURVE25519_KEY_BITS;
    keyMaterial->pubKeySize = pubKeyLen;
    keyMaterial->priKeySize = priKeyLen;

    keyOut->data = buffer;
    keyOut->size = totalSize;
    return HKS_SUCCESS;
}

static int GetCurve25519Id(uint32_t algType)
{
    if (algType == HKS_ALG_X25519) {
        return EVP_PKEY_X25519;
    }
    return EVP_PKEY_ED25519;
}

int32_t HksOpensslCurve25519GenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    EVP_PKEY_CTX *pctx = NULL;
    EVP_PKEY *pkey = NULL;

    int32_t ret;
    do {
        if ((pctx = EVP_PKEY_CTX_new_id(GetCurve25519Id(spec->algType), NULL)) == NULL) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        if (EVP_PKEY_keygen_init(pctx) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        if (EVP_PKEY_keygen(pctx, &pkey) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        ret = SaveCurve25519KeyMaterial(spec->algType, pkey, key);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("save curve25519 key material failed");
        }
    } while (0);

    if (pctx != NULL) {
        EVP_PKEY_CTX_free(pctx);
    }
    if (pkey != NULL) {
        EVP_PKEY_free(pkey);
    }
    return ret;
}

static int32_t ImportX25519EvpKey(EVP_PKEY **ours, EVP_PKEY **theirs, const struct HksBlob *nativeKey,
    const struct HksBlob *pubKey)
{
    struct KeyMaterial25519 *km = (struct KeyMaterial25519 *)nativeKey->data;
    uint32_t offset = sizeof(struct KeyMaterial25519) + km->pubKeySize;

    *ours = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, NULL, nativeKey->data + offset, km->priKeySize);
    if (*ours == NULL) {
        HKS_LOG_E("invalid private key");
        HksLogOpensslError();
        return HKS_ERROR_BAD_STATE;
    }

    struct KeyMaterial25519 *kmPub = (struct KeyMaterial25519 *)pubKey->data;
    offset = sizeof(struct KeyMaterial25519);

    *theirs = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, NULL, pubKey->data + offset, kmPub->pubKeySize);
    if (*theirs == NULL) {
        HKS_LOG_E("invalid public key");
        HksLogOpensslError();
        EVP_PKEY_free(*ours);
        *ours = NULL;
        return HKS_ERROR_BAD_STATE;
    }

    return HKS_SUCCESS;
}

int32_t HksOpensslX25519AgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    (void)spec;
    EVP_PKEY *ours = NULL;
    EVP_PKEY *theirs = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    size_t tmpSharedKeySize = (size_t)sharedKey->size;

    int32_t ret = ImportX25519EvpKey(&ours, &theirs, nativeKey, pubKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("import x25519 evp key failed");
        return ret;
    }
    do {
        ctx = EVP_PKEY_CTX_new(ours, NULL);
        if (ctx == NULL) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
            break;
        }
        if (EVP_PKEY_derive_init(ctx) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
            break;
        }
        if (EVP_PKEY_derive_set_peer(ctx, theirs) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
            break;
        }
        if (EVP_PKEY_derive(ctx, sharedKey->data, &tmpSharedKeySize) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            ret = HKS_ERROR_BAD_STATE;
        }
        sharedKey->size = (uint32_t)tmpSharedKeySize;
    } while (0);
    EVP_PKEY_free(theirs);
    EVP_PKEY_free(ours);
    if (ctx != NULL) {
        EVP_PKEY_CTX_free(ctx);
    }
    return ret;
}

static void FreeEd25519KeyData(uint8_t *priData, uint8_t *pubData, uint8_t *pubKmData)
{
    if (priData != NULL) {
        HksFree(priData);
    }

    if (pubData != NULL) {
        HksFree(pubData);
    }

    if (pubKmData != NULL) {
        HksFree(pubKmData);
    }
}

int32_t HksOpensslEd25519AgreeKey(const struct HksBlob *nativeKey, const struct HksBlob *pubKey,
    const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    (void)spec;
    uint32_t priSize = sizeof(struct KeyMaterial25519) + CURVE25519_KEY_LEN;
    uint8_t *priData = (uint8_t *)HksMalloc(priSize);
    struct HksBlob nKey = { priSize, priData };
    uint32_t pubSize = CURVE25519_KEY_LEN;
    uint8_t *pubData = (uint8_t *)HksMalloc(pubSize);
    struct HksBlob pKey = { pubSize, pubData };
    struct HksBlob pubKeyData = {0};
    uint32_t pubKmSize = sizeof(struct KeyMaterial25519) + CURVE25519_KEY_LEN;
    uint8_t *pubKmData = (uint8_t *)HksMalloc(pubKmSize);
    struct HksBlob pubKm = { pubKmSize, pubKmData };
    if (priData == NULL || pubData == NULL || pubKmData == NULL) {
        FreeEd25519KeyData(priData, pubData, pubKmData);
        return HKS_ERROR_MALLOC_FAIL;
    }
    int32_t ret;
    do {
        ret = ConvertPrivX25519FromED25519(nativeKey, &nKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("convert ED25519 private key to x25519 failed");
            break;
        }

        ret = HksGetKeyFromMaterial(HKS_ALG_X25519, true, pubKey, &pubKeyData);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get key data from key material failed");
            break;
        }
        ret = ConvertPubkeyX25519FromED25519(&pubKeyData, &pKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("convert ED25519 public key to x25519 failed");
            break;
        }
        ret = HksSetKeyToMaterial(HKS_ALG_X25519, true, &pKey, &pubKm);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("set mkey data to key material failed");
            break;
        }
        ret = HksOpensslX25519AgreeKey(&nKey, &pubKm, NULL, sharedKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("x25519 agree key failed");
            break;
        }
    } while (0);
    (void)memset_s(nKey.data, nKey.size, 0, nKey.size);
    FreeEd25519KeyData(nKey.data, pKey.data, pubKm.data);
    HksFree(pubKeyData.data);
    return ret;
}

int32_t HksOpensslEd25519Sign(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, struct HksBlob *signature)
{
    (void)usageSpec;
    struct KeyMaterial25519 *km = (struct KeyMaterial25519 *)key->data;
    uint32_t offset = sizeof(struct KeyMaterial25519) + km->pubKeySize;
    EVP_PKEY *edKey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, NULL, key->data + offset, km->priKeySize);
    if (edKey == NULL) {
        HksLogOpensslError();
        return HKS_ERROR_BAD_STATE;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        HksLogOpensslError();
        EVP_PKEY_free(edKey);
        return HKS_ERROR_BAD_STATE;
    }

    int32_t ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
    do {
        if (EVP_DigestSignInit(mdctx, NULL, NULL, NULL, edKey) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            break;
        }

        size_t req = 0;
        if (EVP_DigestSign(mdctx, NULL, &req, NULL, 0) <= 0) {
            HksLogOpensslError();
            break;
        }

        if (EVP_DigestSign(mdctx, signature->data, &req, message->data, message->size) <= 0) {
            HksLogOpensslError();
            break;
        }

        signature->size = req;
        ret = HKS_SUCCESS;
    } while (0);

    EVP_PKEY_free(edKey);
    EVP_MD_CTX_free(mdctx);
    return ret;
}

int32_t HksOpensslEd25519Verify(const struct HksBlob *key, const struct HksUsageSpec *usageSpec,
    const struct HksBlob *message, const struct HksBlob *signature)
{
    (void)usageSpec;
    struct KeyMaterial25519 *km = (struct KeyMaterial25519 *)key->data;
    EVP_PKEY *edKey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, NULL, key->data + sizeof(struct KeyMaterial25519),
        km->pubKeySize);
    if (edKey == NULL) {
        HksLogOpensslError();
        return HKS_ERROR_BAD_STATE;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        HksLogOpensslError();
        EVP_PKEY_free(edKey);
        return HKS_ERROR_BAD_STATE;
    }

    int32_t ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
    do {
        if (EVP_DigestVerifyInit(mdctx, NULL, NULL, NULL, edKey) != HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            break;
        }

        if (EVP_DigestVerify(mdctx, signature->data, signature->size, message->data, message->size) !=
            HKS_OPENSSL_SUCCESS) {
            HksLogOpensslError();
            break;
        }

        ret = HKS_SUCCESS;
    } while (0);

    EVP_PKEY_free(edKey);
    EVP_MD_CTX_free(mdctx);
    return ret;
}

int32_t HksOpensslGetEd25519PubKey(const struct HksBlob *input, struct HksBlob *output)
{
    struct KeyMaterial25519 *key = (struct KeyMaterial25519 *)input->data;
    uint32_t outLen = sizeof(struct KeyMaterial25519) + key->pubKeySize;
    if (memcpy_s(output->data, output->size, key, outLen) != EOK) {
        HKS_LOG_E("memcpy_s ed25519 pub key Fail!");
        return HKS_ERROR_BAD_STATE;
    }

    ((struct KeyMaterial25519 *)output->data)->priKeySize = 0;
    output->size = outLen;
    return HKS_SUCCESS;
}

