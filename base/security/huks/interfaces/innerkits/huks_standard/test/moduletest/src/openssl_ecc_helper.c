/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "openssl_ecc_helper.h"

#include <stdlib.h>

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <securec.h>

#include "hks_crypto_hal.h"
#include "hks_mem.h"

static int32_t GetCurveId(uint32_t keyLen, int *nid)
{
    switch (keyLen) {
        case HKS_ECC_KEY_SIZE_224:
            *nid = NID_secp224r1;
            break;
        case HKS_ECC_KEY_SIZE_256:
            *nid = NID_X9_62_prime256v1;
            break;
        case HKS_ECC_KEY_SIZE_384:
            *nid = NID_secp384r1;
            break;
        case HKS_ECC_KEY_SIZE_521:
            *nid = NID_secp521r1;
            break;
        default:
            return ECC_FAILED;
    }

    return ECC_SUCCESS;
}

const EVP_MD *GetOpensslAlg(uint32_t alg)
{
    switch (alg) {
        case HKS_DIGEST_MD5:
            return EVP_md5();
        case HKS_DIGEST_SHA1:
            return EVP_sha1();
        case HKS_DIGEST_SHA224:
            return EVP_sha224();
        case HKS_DIGEST_SHA256:
            return EVP_sha256();
        case HKS_DIGEST_SHA384:
            return EVP_sha384();
        case HKS_DIGEST_SHA512:
            return EVP_sha512();
        default:
            return NULL;
    }
}

static int32_t TransEccKeyToKeyBlob(
    const struct KeyMaterialEcc *keyMaterial, BIGNUM *pubX, BIGNUM *pubY, const BIGNUM *priv, uint8_t *rawMaterial)
{
    uint32_t offset = sizeof(struct KeyMaterialEcc);
    if (BN_bn2binpad(pubX, rawMaterial + offset, keyMaterial->xSize) <= 0) {
        return ECC_FAILED;
    }
    offset += keyMaterial->xSize;

    if (BN_bn2binpad(pubY, rawMaterial + offset, keyMaterial->ySize) <= 0) {
        return ECC_FAILED;
    }
    offset += keyMaterial->ySize;

    if (BN_bn2binpad(priv, rawMaterial + offset, keyMaterial->zSize) <= 0) {
        return ECC_FAILED;
    }
    return ECC_SUCCESS;
}

static int32_t EccSaveKeyMaterial(const EC_KEY *eccKey, const uint32_t keyLen, uint8_t **output, uint32_t *outputSize)
{
    uint32_t rawMaterialLen = sizeof(struct KeyMaterialEcc) + HKS_KEY_BYTES(keyLen) * ECC_KEYPAIR_CNT;
    uint8_t *rawMaterial = (uint8_t *)malloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return ECC_FAILED;
    }
    struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_ECC;
    keyMaterial->keySize = keyLen;
    keyMaterial->xSize = HKS_KEY_BYTES(keyLen);
    keyMaterial->ySize = HKS_KEY_BYTES(keyLen);
    keyMaterial->zSize = HKS_KEY_BYTES(keyLen);
    BIGNUM *pubX = BN_new();
    BIGNUM *pubY = BN_new();

    int32_t result = ECC_FAILED;
    do {
        if ((pubX == NULL) || (pubY == NULL)) {
            break;
        }

        const EC_GROUP *ecGroup = EC_KEY_get0_group(eccKey);
        if (ecGroup == NULL) {
            break;
        }

        if (EC_POINT_get_affine_coordinates_GFp(ecGroup, EC_KEY_get0_public_key(eccKey), pubX, pubY, NULL) != 1) {
            break;
        }

        const BIGNUM *priv = EC_KEY_get0_private_key(eccKey);
        if (priv == NULL) {
            break;
        }

        if (TransEccKeyToKeyBlob(keyMaterial, pubX, pubY, priv, rawMaterial) == ECC_FAILED) {
            break;
        }

        *outputSize = rawMaterialLen;

        if (memcpy_s(*output, *outputSize, rawMaterial, rawMaterialLen) != 0) {
            break;
        }

        result = ECC_SUCCESS;
    } while (0);

    if (pubX != NULL) {
        BN_free(pubX);
    }
    if (pubX != NULL) {
        BN_free(pubY);
    }
    free(rawMaterial);
    return result;
}

int32_t EccGenerateKey(const int keyLen, struct HksBlob *key)
{
    int curveId;
    if (GetCurveId(keyLen, &curveId) != ECC_SUCCESS) {
        return ECC_FAILED;
    }

    EC_KEY *eccKey = EC_KEY_new_by_curve_name(curveId);
    if (eccKey == NULL) {
        return ECC_FAILED;
    }

    if (EC_KEY_generate_key(eccKey) <= 0) {
        EC_KEY_free(eccKey);
        return ECC_FAILED;
    }

    if (EccSaveKeyMaterial(eccKey, keyLen, &key->data, &key->size) != ECC_SUCCESS) {
        EC_KEY_free(eccKey);
        return ECC_FAILED;
    }

    EC_KEY_free(eccKey);
    return ECC_SUCCESS;
}

static int GetEccModules(
    const uint8_t *key, uint32_t *keySize, uint32_t *publicXSize, uint32_t *publicYSize, uint32_t *privateXSize)
{
    struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)key;
    *keySize = keyMaterial->keySize;
    *publicXSize = keyMaterial->xSize;
    *publicYSize = keyMaterial->ySize;
    *privateXSize = keyMaterial->zSize;

    return 0;
}

static int32_t EccInitPublicKey(EC_KEY *eccKey, const uint8_t *keyPair, uint32_t xSize, uint32_t ySize)
{
    const EC_GROUP *ecGroup = EC_KEY_get0_group(eccKey);
    if (ecGroup == NULL) {
        return ECC_FAILED;
    }

    int32_t ret = ECC_FAILED;
    uint32_t offset = sizeof(struct KeyMaterialEcc);
    EC_POINT *pub = EC_POINT_new(ecGroup);
    BIGNUM *pubX = BN_bin2bn(keyPair + offset, xSize, NULL);
    offset += xSize;
    BIGNUM *pubY = BN_bin2bn(keyPair + offset, ySize, NULL);
    do {
        if ((pubX == NULL) || (pubY == NULL) || (pub == NULL)) {
            break;
        }

        if (EC_POINT_set_affine_coordinates_GFp(ecGroup, pub, pubX, pubY, NULL) <= 0) {
            break;
        }

        if (EC_KEY_set_public_key(eccKey, pub) <= 0) {
            break;
        }
        ret = ECC_SUCCESS;
    } while (0);

    if (pubX != NULL) {
        BN_free(pubX);
        pubX = NULL;
    }

    if (pubY != NULL) {
        BN_free(pubY);
        pubY = NULL;
    }

    if (pub != NULL) {
        EC_POINT_free(pub);
        pub = NULL;
    }
    return ret;
}

static EC_KEY *EccInitKey(const struct HksBlob *keyBlob, bool sign)
{
    /* get ecc pubX,pubY,pri */
    uint8_t *keyPair = keyBlob->data;
    uint32_t publicXSize;
    uint32_t publicYSize;
    uint32_t privateSize;
    uint32_t keySize;

    if (GetEccModules(keyPair, &keySize, &publicXSize, &publicYSize, &privateSize) != 0) {
        return NULL;
    }

    int nid;
    if (GetCurveId(keySize, &nid) != ECC_SUCCESS) {
        return NULL;
    }

    EC_KEY *eccKey = EC_KEY_new_by_curve_name(nid);
    if (eccKey == NULL) {
        return NULL;
    }

    if (EccInitPublicKey(eccKey, keyPair, publicXSize, publicYSize) != ECC_SUCCESS) {
        EC_KEY_free(eccKey);
        return NULL;
    }

    if (sign) {
        BIGNUM *pri = BN_bin2bn(keyPair + sizeof(struct KeyMaterialEcc) + publicXSize + publicYSize, privateSize, NULL);
        if (pri == NULL) {
            EC_KEY_free(eccKey);
            return NULL;
        }
        if (EC_KEY_set_private_key(eccKey, pri) != 1) {
            BN_free(pri);
            EC_KEY_free(eccKey);
            return NULL;
        }
        BN_clear_free(pri);
    }

    return eccKey;
}

static EVP_MD_CTX *InitEccMdCtx(const struct HksBlob *mainKey, uint32_t digest, bool sign)
{
    const EVP_MD *md = GetOpensslAlg(digest);

    EC_KEY *eccKey = EccInitKey(mainKey, sign);
    if (eccKey == NULL) {
        return NULL;
    }

    EVP_PKEY *key = EVP_PKEY_new();
    if (key == NULL) {
        EC_KEY_free(eccKey);
        return NULL;
    }

    if (EVP_PKEY_assign_EC_KEY(key, eccKey) != 1) {
        EC_KEY_free(eccKey);
        EVP_PKEY_free(key);
        return NULL;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        EVP_PKEY_free(key);
        return NULL;
    }

    if (sign) {
        int32_t ret = EVP_DigestSignInit(ctx, NULL, md, NULL, key);
        EVP_PKEY_free(key);
        if (ret != 1) {
            EVP_MD_CTX_free(ctx);
            return NULL;
        }
    } else {
        int ret = EVP_DigestVerifyInit(ctx, NULL, md, NULL, key);
        EVP_PKEY_free(key);
        if (ret != 1) {
            EVP_MD_CTX_free(ctx);
            return NULL;
        }
    }
    return ctx;
}

int32_t EcdsaSign(const struct HksBlob *key, int digest, const struct HksBlob *message, struct HksBlob *signature)
{
    EVP_MD_CTX *ctx = InitEccMdCtx(key, digest, true);
    if (ctx == NULL) {
        return ECC_FAILED;
    }

    if (EVP_DigestSignUpdate(ctx, message->data, message->size) != 1) {
        EVP_MD_CTX_free(ctx);
        return ECC_FAILED;
    }
    size_t req = 0;

    if (EVP_DigestSignFinal(ctx, NULL, &req) != 1) {
        EVP_MD_CTX_free(ctx);
        return ECC_FAILED;
    }

    if (EVP_DigestSignFinal(ctx, signature->data, &req) != 1) {
        EVP_MD_CTX_free(ctx);
        return ECC_FAILED;
    }
    signature->size = req;

    EVP_MD_CTX_free(ctx);
    return ECC_SUCCESS;
}

int32_t EcdsaVerify(
    const struct HksBlob *key, int digest, const struct HksBlob *message, const struct HksBlob *signature)
{
    EVP_MD_CTX *ctx = InitEccMdCtx(key, digest, false);
    if (ctx == NULL) {
        return ECC_FAILED;
    }

    if (EVP_DigestVerifyUpdate(ctx, message->data, message->size) != 1) {
        EVP_MD_CTX_free(ctx);
        return ECC_FAILED;
    }

    if (EVP_DigestVerifyFinal(ctx, signature->data, signature->size) != 1) {
        EVP_MD_CTX_free(ctx);
        return ECC_FAILED;
    }

    EVP_MD_CTX_free(ctx);
    return ECC_SUCCESS;
}

int32_t GetEccPubKey(const struct HksBlob *input, struct HksBlob *output)
{
    if (input == NULL || input->data == NULL || input->size < sizeof(struct KeyMaterialEcc) || output == NULL ||
        output->data == NULL || output->size < sizeof(struct KeyMaterialEcc)) {
        return ECC_FAILED;
    }
    struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)input->data;

    output->size = sizeof(struct KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize;

    struct KeyMaterialEcc *publickeyMaterial = (struct KeyMaterialEcc *)output->data;
    publickeyMaterial->keyAlg = keyMaterial->keyAlg;
    publickeyMaterial->keySize = keyMaterial->keySize;
    publickeyMaterial->xSize = keyMaterial->xSize;
    publickeyMaterial->ySize = keyMaterial->ySize;
    publickeyMaterial->zSize = 0;

    if (memcpy_s(output->data + sizeof(struct KeyMaterialEcc),
        output->size - sizeof(struct KeyMaterialEcc),
        input->data + sizeof(struct KeyMaterialEcc),
        keyMaterial->xSize + keyMaterial->ySize) != 0) {
        return ECC_FAILED;
    }

    return ECC_SUCCESS;
}

static int32_t EcKeyToPublicKey(EC_KEY *ecKey, struct HksBlob *eccPublicKey)
{
    BIGNUM *x = BN_new();
    BIGNUM *y = BN_new();
    int32_t ret;
    do {
        ret = ECC_FAILED;
        if (x == NULL || y == NULL) {
            break;
        }

        if (EC_POINT_get_affine_coordinates_GFp(EC_KEY_get0_group(ecKey), EC_KEY_get0_public_key(ecKey), x, y, NULL) ==
            0) {
            break;
        }

        uint32_t keyLen = (uint32_t)EC_GROUP_order_bits(EC_KEY_get0_group(ecKey));
        uint32_t xSize = HKS_KEY_BYTES(keyLen);
        uint32_t ySize = HKS_KEY_BYTES(keyLen);
        if ((keyLen == 0) || (keyLen > HKS_ECC_KEY_SIZE_521)) {
            break;
        }

        uint32_t totalSize = xSize + ySize + sizeof(struct HksPubKeyInfo);
        uint8_t *keyBuffer = HksMalloc(totalSize);
        if (keyBuffer == NULL) {
            break;
        }

        struct HksPubKeyInfo *pubKeyInfo = (struct HksPubKeyInfo *)keyBuffer;
        pubKeyInfo->keyAlg = HKS_ALG_ECC;
        pubKeyInfo->keySize = keyLen;
        pubKeyInfo->nOrXSize = xSize;
        pubKeyInfo->eOrYSize = ySize;
        if (BN_bn2binpad(x, keyBuffer + sizeof(struct HksPubKeyInfo), xSize) == 0 ||
            BN_bn2binpad(y, keyBuffer + sizeof(struct HksPubKeyInfo) + xSize, ySize) == 0) {
            HKS_FREE_PTR(keyBuffer);
            break;
        }

        ret = ECC_SUCCESS;
        eccPublicKey->size = totalSize;
        if (memcpy_s(eccPublicKey->data, eccPublicKey->size, keyBuffer, totalSize) != 0) {
            HKS_FREE_PTR(keyBuffer);
            break;
        }
        HKS_FREE_PTR(keyBuffer);
    } while (0);

    SELF_FREE_PTR(x, BN_free);
    SELF_FREE_PTR(y, BN_free);
    return ret;
}

int32_t X509ToHksBlob(const struct HksBlob *x509Key, struct HksBlob *publicKey)
{
    if (x509Key == NULL || x509Key->data == NULL || x509Key->size == 0 || publicKey == NULL) {
        return ECC_FAILED;
    }

    uint8_t *data = x509Key->data;

    EVP_PKEY *pkey = d2i_PUBKEY(NULL, (const unsigned char **)&data, x509Key->size);
    if (pkey == NULL) {
        return ECC_FAILED;
    }

    EC_KEY *ecKey = EVP_PKEY_get0_EC_KEY(pkey);
    if (ecKey == NULL) {
        EVP_PKEY_free(pkey);
        return ECC_FAILED;
    }

    if (EcKeyToPublicKey(ecKey, publicKey) != ECC_SUCCESS) {
        EVP_PKEY_free(pkey);
        return ECC_FAILED;
    }

    EVP_PKEY_free(pkey);
    return ECC_SUCCESS;
}

int32_t HksBlobToX509(const struct HksBlob *key, struct HksBlob *x509Key)
{
    uint8_t *keyPair = key->data;
    uint32_t publicXSize;
    uint32_t publicYSize;
    uint32_t privateSize;
    uint32_t keySize;

    if (GetEccModules(keyPair, &keySize, &publicXSize, &publicYSize, &privateSize) != 0) {
        return ECC_FAILED;
    }

    int nid;
    if (GetCurveId(keySize, &nid) != ECC_SUCCESS) {
        return ECC_FAILED;
    }

    EC_KEY *eccKey = EC_KEY_new_by_curve_name(nid);
    if (eccKey == NULL) {
        return ECC_FAILED;
    }

    if (EccInitPublicKey(eccKey, keyPair, publicXSize, publicYSize) != ECC_SUCCESS) {
        EC_KEY_free(eccKey);
        return ECC_FAILED;
    }

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        EC_KEY_free(eccKey);
        return ECC_FAILED;
    }

    if (EVP_PKEY_assign_EC_KEY(pkey, eccKey) <= 0) {
        EC_KEY_free(eccKey);
        EVP_PKEY_free(pkey);
        return ECC_FAILED;
    }

    uint8_t *tmp = NULL;
    uint32_t length = (uint32_t)i2d_PUBKEY(pkey, &tmp);
    x509Key->size = length;
    if (tmp == NULL) {
        EVP_PKEY_free(pkey);
        return ECC_FAILED;
    }

    if (memcpy_s(x509Key->data, x509Key->size, tmp, length) != 0) {
        EVP_PKEY_free(pkey);
        OPENSSL_free(tmp);
        return ECC_FAILED;
    }

    EVP_PKEY_free(pkey);
    OPENSSL_free(tmp);
    return ECC_SUCCESS;
}

int32_t GetNativePkey(const struct HksBlob *nativeKey, EVP_PKEY *key)
{
    EC_KEY *eccKey = EccInitKey(nativeKey, true);
    if (eccKey == NULL) {
        return ECC_FAILED;
    }

    if (EVP_PKEY_assign_EC_KEY(key, eccKey) <= 0) {
        EC_KEY_free(eccKey);
        return ECC_FAILED;
    }
    return ECC_SUCCESS;
}

int32_t GetPeerKey(const struct HksBlob *pubKey, EVP_PKEY *key)
{
    EC_KEY *eccKey = EccInitKey(pubKey, false);
    if (eccKey == NULL) {
        return ECC_FAILED;
    }

    if (EVP_PKEY_assign_EC_KEY(key, eccKey) <= 0) {
        EC_KEY_free(eccKey);
        return ECC_FAILED;
    }
    return ECC_SUCCESS;
}

int32_t EcdhDerive(EVP_PKEY_CTX *ctx, EVP_PKEY *peerKey, struct HksBlob *sharedKey)
{
    if (EVP_PKEY_derive_init(ctx) != 1) {
        return ECC_FAILED;
    }
    if (EVP_PKEY_derive_set_peer(ctx, peerKey) != 1) {
        return ECC_FAILED;
    }
    size_t keyLen = 0;
    if (EVP_PKEY_derive(ctx, NULL, &keyLen) != 1) {
        return ECC_FAILED;
    }
    sharedKey->size = keyLen;

    if (sharedKey->data == NULL) {
        sharedKey->data = (uint8_t *)HksMalloc(keyLen);
        if (sharedKey->data == NULL) {
            return ECC_FAILED;
        }
    }
    if (EVP_PKEY_derive(ctx, sharedKey->data, &keyLen) != 1) {
        HksFree(sharedKey->data);
        return ECC_FAILED;
    }
    sharedKey->size = keyLen;

    return ECC_SUCCESS;
}

int32_t EcdhAgreeKey(
    const int keyLen, const struct HksBlob *nativeKey, const struct HksBlob *pubKey, struct HksBlob *sharedKey)
{
    int32_t ret = ECC_FAILED;
    EVP_PKEY *pKey = EVP_PKEY_new();
    EVP_PKEY *peerKey = EVP_PKEY_new();
    EVP_PKEY_CTX *ctx = NULL;

    do {
        if ((peerKey == NULL) || (pKey == NULL)) {
            break;
        }
        if (GetNativePkey(nativeKey, pKey) != ECC_SUCCESS) {
            break;
        }

        if (GetPeerKey(pubKey, peerKey) != ECC_SUCCESS) {
            break;
        }

        ctx = EVP_PKEY_CTX_new(pKey, NULL);
        if (ctx == NULL) {
            break;
        }

        if (EcdhDerive(ctx, peerKey, sharedKey) != ECC_SUCCESS) {
            break;
        }

        ret = ECC_SUCCESS;
    } while (0);

    if (ctx != NULL) {
        EVP_PKEY_CTX_free(ctx);
    }
    if (peerKey != NULL) {
        EVP_PKEY_free(peerKey);
    }
    if (pKey != NULL) {
        EVP_PKEY_free(pKey);
    }

    return ret;
}
