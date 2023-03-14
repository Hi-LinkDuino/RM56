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

#include "openssl_dsa_helper.h"

#include <openssl/bn.h>
#include <openssl/dsa.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <securec.h>

#include "hks_crypto_hal.h"
#include "hks_common_check.h"

#define OPENSSL_KEY_BLOCK 8
#define OPENSSL_DSA_MIN_KEY_LEN 64
#define OPENSSL_DSA_KEY_LEN_DIVID (2048 / HKS_BITS_PER_BYTE)

EVP_PKEY *GenerateDsaKey(const uint32_t keySize)
{
    DSA *dsa = DSA_new();
    if (dsa == NULL) {
        return NULL;
    }
    if (DSA_generate_parameters_ex(dsa, keySize, NULL, 0, NULL, NULL, NULL) != 1) {
        DSA_free(dsa);
        return NULL;
    }
    if (DSA_generate_key(dsa) != 1) {
        DSA_free(dsa);
        return NULL;
    }

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        DSA_free(dsa);
        return NULL;
    }

    if (EVP_PKEY_assign_DSA(pkey, dsa) != 1) {
        EVP_PKEY_free(pkey);
        DSA_free(dsa);
        return NULL;
    }

    return pkey;
}

static BIGNUM *BinToBN(uint8_t *data, uint32_t length)
{
    uint8_t buff[length];
    if (memcpy_s(buff, sizeof(buff), data, length) != 0) {
        return NULL;
    }
    return BN_bin2bn(buff, length, NULL);
}

static void FreeBN(BIGNUM *bn)
{
    if (bn != NULL) {
        BN_free(bn);
    }
}

static DSA *InitDsa(struct HksBlob *key, const bool needPrivateExponent)
{
    const struct KeyMaterialDsa *keyMaterial = (struct KeyMaterialDsa *)(key->data);

    uint32_t offset = sizeof(struct KeyMaterialDsa);
    BIGNUM *x = NULL;
    BIGNUM *y = NULL;
    BIGNUM *p = NULL;
    BIGNUM *q = NULL;
    BIGNUM *g = NULL;

    if (needPrivateExponent) {
        x = BinToBN(key->data + offset, keyMaterial->xSize);
        offset += keyMaterial->xSize;
    }

    y = BinToBN(key->data + offset, keyMaterial->ySize);
    offset += keyMaterial->ySize;

    p = BinToBN(key->data + offset, keyMaterial->pSize);
    offset += keyMaterial->pSize;

    q = BinToBN(key->data + offset, keyMaterial->qSize);
    offset += keyMaterial->qSize;

    g = BinToBN(key->data + offset, keyMaterial->gSize);

    DSA *dsa = NULL;
    do {
        dsa = DSA_new();
        if (dsa == NULL) {
            break;
        }

        if (DSA_set0_key(dsa, y, x) != 1) {
            DSA_free(dsa);
            dsa = NULL;
            break;
        }

        if (DSA_set0_pqg(dsa, p, q, g) != 1) {
            DSA_free(dsa);
            dsa = NULL;
            break;
        }
    } while (0);

    if (dsa == NULL) {
        FreeBN(x);
        FreeBN(y);
        FreeBN(p);
        FreeBN(q);
        FreeBN(g);
    }

    return dsa;
}

static const EVP_MD *GetOpensslDigestType(enum HksKeyDigest digestType)
{
    switch (digestType) {
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

int32_t OpensslSignDsa(
    const struct HksBlob *plainText, struct HksBlob *signData, struct HksBlob *key, enum HksKeyDigest digestType)
{
    DSA *dsa = InitDsa(key, true);
    if (dsa == NULL) {
        return DSA_FAILED;
    }

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        DSA_free(dsa);
        return DSA_FAILED;
    }

    if (EVP_PKEY_assign_DSA(pkey, dsa) != 1) {
        EVP_PKEY_free(pkey);
        DSA_free(dsa);
        return DSA_FAILED;
    }

    EVP_MD_CTX *mctx = EVP_MD_CTX_new();
    if (mctx == NULL) {
        EVP_PKEY_free(pkey);
        return DSA_FAILED;
    }

    const EVP_MD *md = GetOpensslDigestType(digestType);
    if (EVP_DigestSignInit(mctx, NULL, md, NULL, pkey) != 1) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mctx);
        return DSA_FAILED;
    }

    if (EVP_DigestSignUpdate(mctx, plainText->data, plainText->size) != 1) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mctx);
        return DSA_FAILED;
    }

    size_t signLen = signData->size;
    if (EVP_DigestSignFinal(mctx, signData->data, &signLen) != 1) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mctx);
        return DSA_FAILED;
    }

    signData->size = signLen;

    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(mctx);

    return DSA_SUCCESS;
}

int32_t OpensslVerifyDsa(
    const struct HksBlob *plainText, struct HksBlob *signData, struct HksBlob *key, enum HksKeyDigest digestType)
{
    DSA *dsa = InitDsa(key, false);
    if (dsa == NULL) {
        return DSA_FAILED;
    }

    EVP_PKEY *pkey = EVP_PKEY_new();
    if (pkey == NULL) {
        DSA_free(dsa);
        return DSA_FAILED;
    }

    if (EVP_PKEY_assign_DSA(pkey, dsa) != 1) {
        EVP_PKEY_free(pkey);
        DSA_free(dsa);
        return DSA_FAILED;
    }

    EVP_MD_CTX *mctx = EVP_MD_CTX_new();
    if (mctx == NULL) {
        EVP_PKEY_free(pkey);
        return DSA_FAILED;
    }

    const EVP_MD *md = GetOpensslDigestType(digestType);
    if (EVP_DigestVerifyInit(mctx, NULL, md, NULL, pkey) != 1) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mctx);
        return DSA_FAILED;
    }

    if (EVP_DigestVerifyUpdate(mctx, plainText->data, plainText->size) != 1) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mctx);
        return DSA_FAILED;
    }

    if (EVP_DigestVerifyFinal(mctx, signData->data, signData->size) != 1) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mctx);
        return DSA_FAILED;
    }

    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(mctx);

    return DSA_SUCCESS;
}

int32_t X509ToDsaPublicKey(struct HksBlob *x509Key, struct HksBlob *publicKey)
{
    uint8_t *data = x509Key->data;

    EVP_PKEY *pkey = d2i_PUBKEY(NULL, (const unsigned char **)&data, x509Key->size);
    if (pkey == NULL) {
        return DSA_FAILED;
    }

    DSA *dsa = EVP_PKEY_get0_DSA(pkey);
    if (dsa == NULL) {
        EVP_PKEY_free(pkey);
        return DSA_FAILED;
    }
    uint32_t ySize = (uint32_t)BN_num_bytes(DSA_get0_pub_key(dsa));
    uint32_t pSize = (uint32_t)BN_num_bytes(DSA_get0_p(dsa));
    uint32_t qSize = (uint32_t)BN_num_bytes(DSA_get0_q(dsa));
    uint32_t gSize = (uint32_t)BN_num_bytes(DSA_get0_g(dsa));
    if ((ySize == 0) || (pSize == 0) || (qSize == 0) || (gSize == 0)) {
        EVP_PKEY_free(pkey);
        return DSA_FAILED;
    }
    if ((ySize > UINT32_MAX - HKS_BITS_PER_BYTE) ||
        ((ySize + HKS_BITS_PER_BYTE - 1) / HKS_BITS_PER_BYTE > UINT32_MAX / (HKS_BITS_PER_BYTE * HKS_BITS_PER_BYTE))) {
        EVP_PKEY_free(pkey);
        return DSA_FAILED;
    }
    struct KeyMaterialDsa *keyMaterial = (struct KeyMaterialDsa *)publicKey->data;
    keyMaterial->keyAlg = HKS_ALG_DSA;
    keyMaterial->keySize = (ySize + HKS_BITS_PER_BYTE - 1) / HKS_BITS_PER_BYTE * HKS_BITS_PER_BYTE * HKS_BITS_PER_BYTE;
    keyMaterial->xSize = 0;
    keyMaterial->ySize = ySize;
    keyMaterial->pSize = pSize;
    keyMaterial->qSize = qSize;
    keyMaterial->gSize = gSize;

    int32_t result = DSA_FAILED;
    uint32_t offset = sizeof(struct KeyMaterialDsa) + keyMaterial->xSize;
    do {
        if ((BN_bn2bin(DSA_get0_pub_key(dsa), publicKey->data + offset) == 0)) {
            break;
        }
        if (BN_bn2bin(DSA_get0_p(dsa), publicKey->data + offset + ySize) == 0) {
            break;
        }
        if (BN_bn2bin(DSA_get0_q(dsa), publicKey->data + offset + ySize + pSize) == 0) {
            break;
        }
        if (BN_bn2bin(DSA_get0_g(dsa), publicKey->data + offset + ySize + pSize + qSize) == 0) {
            break;
        }
        result = DSA_SUCCESS;
    } while (0);

    EVP_PKEY_free(pkey);
    return result;
}

bool DsaGetx509PubKey(EVP_PKEY *pkey, struct HksBlob *x509Key)
{
    uint8_t *tmp = NULL;
    uint32_t length = (uint32_t)i2d_PUBKEY(pkey, &tmp);
    if (x509Key->size < length) {
        OPENSSL_free(tmp);
        return false;
    }
    x509Key->size = length;
    if (tmp != NULL) {
        if (memcpy_s(x509Key->data, x509Key->size, tmp, length) != EOK) {
            OPENSSL_free(tmp);
            return false;
        }
        OPENSSL_free(tmp);
    } else {
        return false;
    }
    return true;
}

int32_t SaveDsaKeyToHksBlob(EVP_PKEY *pkey, const uint32_t keySize, struct HksBlob *key)
{
    DSA *dsa = EVP_PKEY_get0_DSA(pkey);
    if (dsa == NULL) {
        return DSA_FAILED;
    }

    uint32_t opensslKeyByteLen = HKS_KEY_BYTES(keySize);
    if (opensslKeyByteLen < OPENSSL_DSA_MIN_KEY_LEN) {
        opensslKeyByteLen = OPENSSL_DSA_MIN_KEY_LEN;
    }
    uint32_t keyByteLen = (opensslKeyByteLen + OPENSSL_KEY_BLOCK - 1) / OPENSSL_KEY_BLOCK * OPENSSL_KEY_BLOCK;

    struct KeyMaterialDsa *keyMaterial = (struct KeyMaterialDsa *)key->data;
    keyMaterial->keyAlg = HKS_ALG_DSA;
    keyMaterial->keySize = keyByteLen * HKS_BITS_PER_BYTE;
    keyMaterial->xSize = (keyByteLen > OPENSSL_DSA_KEY_LEN_DIVID) ? HKS_DIGEST_SHA256_LEN : HKS_DIGEST_SHA1_LEN;
    keyMaterial->ySize = keyByteLen;
    keyMaterial->pSize = keyByteLen;
    keyMaterial->qSize = (keyByteLen > OPENSSL_DSA_KEY_LEN_DIVID) ? HKS_DIGEST_SHA256_LEN : HKS_DIGEST_SHA1_LEN;
    keyMaterial->gSize = keyByteLen;

    const BIGNUM *x = DSA_get0_priv_key(dsa);
    const BIGNUM *y = DSA_get0_pub_key(dsa);
    const BIGNUM *p = DSA_get0_p(dsa);
    const BIGNUM *q = DSA_get0_q(dsa);
    const BIGNUM *g = DSA_get0_g(dsa);

    if (x == NULL || y == NULL || p == NULL || q == NULL || g == NULL) {
        return DSA_FAILED;
    }

    uint32_t offset = sizeof(struct KeyMaterialDsa);
    if (BN_bn2bin(x, key->data + offset + (keyMaterial->xSize - (uint32_t)BN_num_bytes(x))) <= 0) {
        return DSA_FAILED;
    }
    offset += keyMaterial->xSize;
    if (BN_bn2bin(y, key->data + offset + (keyMaterial->ySize - (uint32_t)BN_num_bytes(y))) <= 0) {
        return DSA_FAILED;
    }
    offset += keyMaterial->ySize;
    if (BN_bn2bin(p, key->data + offset + (keyMaterial->pSize - (uint32_t)BN_num_bytes(p))) <= 0) {
        return DSA_FAILED;
    }
    offset += keyMaterial->pSize;
    if (BN_bn2bin(q, key->data + offset + (keyMaterial->qSize - (uint32_t)BN_num_bytes(q))) <= 0) {
        return DSA_FAILED;
    }
    offset += keyMaterial->qSize;
    if (BN_bn2bin(g, key->data + offset + (keyMaterial->gSize - (uint32_t)BN_num_bytes(g))) <= 0) {
        return DSA_FAILED;
    }
    return DSA_SUCCESS;
}
