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

#include "hks_openssl_ed25519tox25519.h"

#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include "hks_log.h"
#include "hks_openssl_engine.h"

#define CURVE25519_KEY_LEN 32
#define CURVE25519_KEY_BITS 256
#define HKS_OPENSSL_ERROR_LEN 128

#define P_BITS 256 // 255 significant bits + 1 for carry
#define P_BYTES 32 // 32 bytes
#define ED25519_FIX_KEY_BUFFER_SIZE 64

// RFC standered implement
// The prime number: 2^255 - 19
static const unsigned char g_pBytes[P_BYTES] = {
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 0:p */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xed
};

// A non-zero element in the finite field
static const unsigned char g_dBytes[P_BYTES] = {
    0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75, 0xab, 0xd8, 0x41, 0x41, 0x4d, 0x0a, 0x70, 0x00,
    0x98, 0xe8, 0x79, 0x77, 0x79, 0x40, 0xc7, 0x8c, 0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52
};

// The large number of (-2 mod p)
static const unsigned char g_negativeTwoModPBytes[P_BYTES] = {
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xeb
};

// The large number of (-1 mod p)
static const unsigned char g_negativeOneModPBytes[P_BYTES] = {
    0xec, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f
};

// The large number of (-1 / 2)
static const unsigned char g_negativeOneDivideTwoBytes[P_BYTES] = {
    0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f
};

static int32_t ConvertStringToInt(const uint8_t *param, uint32_t paraLen, BIGNUM **bigNum)
{
    *bigNum = BN_bin2bn(param, paraLen, NULL);
    if (*bigNum == NULL) {
        HKS_LOG_E("failed to translate octet string into big integer!");
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    return HKS_SUCCESS;
}

static void FreeBigInt(BIGNUM **bigInt)
{
    BN_free(*bigInt);
    *bigInt = NULL;
}

static void Curve25519Destroy(struct Curve25519Structure *curve25519)
{
    if (curve25519->p != NULL) {
        FreeBigInt(&curve25519->p);
    }

    if (curve25519->d != NULL) {
        FreeBigInt(&curve25519->d);
    }

    if (curve25519->negativeTwo != NULL) {
        FreeBigInt(&curve25519->negativeTwo);
    }

    if (curve25519->negativeOne != NULL) {
        FreeBigInt(&curve25519->negativeOne);
    }

    if (curve25519->negativeOneDivideTwo != NULL) {
        FreeBigInt(&curve25519->negativeOneDivideTwo);
    }

    if (curve25519->ed25519Pubkey != NULL) {
        BN_clear(curve25519->ed25519Pubkey);
        FreeBigInt(&curve25519->ed25519Pubkey);
    }
}

static void SwapEndianThirtyTwoByte(uint8_t *pubkey, int len, bool isBigEndian)
{
    if (isBigEndian) {
        for (int i = 0; i < len / 2; ++i) { // 2: get the middle position of the string
            uint8_t tmp = pubkey[i];
            pubkey[i] = pubkey[len - i - 1];
            pubkey[len - i - 1] = tmp;
        }
    }
}

static int32_t CovertData(struct Curve25519Structure *curve25519, uint8_t *pubkey, int len)
{
    int32_t ret;
    do {
        ret = ConvertStringToInt(g_pBytes, P_BYTES, &curve25519->p);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Convert to g_pBytes big number failed!");
            break;
        }
        ret = ConvertStringToInt(g_dBytes, P_BYTES, &curve25519->d);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Convert to g_dBytes big number failed!");
            break;
        }
        ret = ConvertStringToInt(g_negativeTwoModPBytes, P_BYTES, &curve25519->negativeTwo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Convert to g_negativeTwoModPBytes big number failed!");
            break;
        }
        ret = ConvertStringToInt(g_negativeOneModPBytes, P_BYTES, &curve25519->negativeOne);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Convert to g_negativeOneModPBytes big number failed!");
            break;
        }
        ret = ConvertStringToInt(g_negativeOneDivideTwoBytes, P_BYTES, &curve25519->negativeOneDivideTwo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Convert to g_negativeOneDivideTwoBytes big number failed!");
            break;
        }
        ret = ConvertStringToInt(pubkey, len, &curve25519->ed25519Pubkey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Convert to ed25519 big number failed!");
            break;
        }
    } while (0);

    if (ret != HKS_SUCCESS)
        Curve25519Destroy(curve25519);

    return ret;
}

static int32_t Curve25519Initialize(struct Curve25519Structure *curve25519,
    const uint8_t *source, uint32_t sourceLen, bool isBigEndian)
{
    if (sourceLen != P_BYTES) {
        HKS_LOG_E("invalid param input");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int32_t ret;
    uint8_t pubKey[P_BYTES] = {0};
    if (memcpy_s(pubKey, P_BYTES - 1, source, sourceLen - 1) != EOK) { // the 0-30 bit assignment
        HKS_LOG_E("copy source data to pubKey failed!");
        return HKS_ERROR_BAD_STATE;
    }
    pubKey[P_BYTES - 1] = source[P_BYTES - 1] & 0x7f; // the last bit assignment
    SwapEndianThirtyTwoByte(pubKey, sizeof(pubKey), isBigEndian);

    (void)memset_s(curve25519, sizeof(struct Curve25519Structure), 0, sizeof(struct Curve25519Structure));
    ret = CovertData(curve25519, pubKey, sizeof(pubKey));
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("convert binary data to big num failed!");
        return ret;
    }
    return ret;
}

static void FreeLocalBigVar(struct Curve25519Var *var)
{
    if (var->a != NULL) {
        BN_free(var->a);
        var->a = NULL;
    }

    if (var->b != NULL) {
        BN_free(var->b);
        var->b = NULL;
    }

    if (var->c != NULL) {
        BN_free(var->c);
        var->c = NULL;
    }
}

static int32_t Curve25519LocalVar(struct Curve25519Var *var)
{
    int32_t ret = HKS_ERROR_BUFFER_TOO_SMALL;
    do {
        var->a = BN_new();
        if (var->a == NULL) {
            HKS_LOG_E("Big integer a is null!");
            ret = HKS_ERROR_BUFFER_TOO_SMALL;
            break;
        }
        var->b = BN_new();
        if (var->b == NULL) {
            HKS_LOG_E("Big integer b is null!");
            ret = HKS_ERROR_BUFFER_TOO_SMALL;
            break;
        }
        var->c = BN_new();
        if (var->c == NULL) {
            HKS_LOG_E("Big integer c is null!");
            ret = HKS_ERROR_BUFFER_TOO_SMALL;
            break;
        }
        ret = HKS_SUCCESS;
    } while (0);

    if (ret != HKS_SUCCESS)
        FreeLocalBigVar(var);

    return ret;
}

static int32_t CheckEd25519PubkeyPart(const struct Curve25519Structure *curve25519, struct Curve25519Var *var,
    BN_CTX *ctx, BIGNUM *tmpOne)
{
    if (BN_set_word(tmpOne, 1) <= 0) {
        return HKS_FAILURE;
    }
    if (BN_mod_add(var->b, var->b, tmpOne, curve25519->p, ctx) <= 0) {
        return HKS_FAILURE;
    }
    if (BN_mod_exp(var->c, var->b, curve25519->negativeTwo, curve25519->p, ctx) <= 0) {
        return HKS_FAILURE;
    }
    if (BN_mod_add(var->b, var->a, curve25519->negativeOne, curve25519->p, ctx) <= 0) {
        return HKS_FAILURE;
    }
    if (BN_mod_mul(var->a, var->b, var->c, curve25519->p, ctx) <= 0) {
        return HKS_FAILURE;
    }
    if (BN_mod_add(var->a, var->a, curve25519->p, curve25519->p, ctx) <= 0) {
        return HKS_FAILURE;
    }
    if (BN_mod_exp(var->b, var->a, curve25519->negativeOneDivideTwo, curve25519->p, ctx) <= 0) {
        return HKS_FAILURE;
    }
    return HKS_SUCCESS;
}

static int32_t CheckEd25519Pubkey(const struct Curve25519Structure *curve25519, struct Curve25519Var *var,
    uint8_t flag, BN_CTX *ctx)
{
    int32_t res = HKS_FAILURE;
    uint32_t result = (uint32_t)(BN_cmp(curve25519->ed25519Pubkey, curve25519->p) < 0);
    BIGNUM *tmpOne = BN_new();
    if (tmpOne == NULL) {
        return res;
    }
    do {
        if (BN_mod_sqr(var->a, curve25519->ed25519Pubkey, curve25519->p, ctx) <= 0) {
            break;
        }
        if (BN_mod_mul(var->b, var->a, curve25519->d, curve25519->p, ctx) <= 0) {
            break;
        }
        if (CheckEd25519PubkeyPart(curve25519, var, ctx, tmpOne) != HKS_SUCCESS) {
            break;
        }
        result &= (uint32_t)(BN_cmp(var->b, curve25519->negativeOneDivideTwo) < 0);
        if (BN_mod_sub(var->a, var->a, curve25519->p, curve25519->p, ctx) <= 0) {
            break;
        }
        if (BN_cmp(var->a, tmpOne) < 0) {
            result ^= (0x1 & ((uint32_t)flag >> 7)); // 7: Get the sign bit of the last byte of the ed25519 pubkey
        } else {
            result ^= 0x0;
        }
        if (result == 0) {
            break;
        }
        res = HKS_SUCCESS;
    } while (0);
    if (tmpOne != NULL) {
        BN_free(tmpOne);
    }
    return res;
}

static int32_t FillPubKeyByZero(uint8_t *pubKey, uint32_t *pubKeySize)
{
    if (*pubKeySize < P_BYTES) {
        uint8_t tmpKey[P_BYTES] = {0};
        int baseAddr = P_BYTES - *pubKeySize;
        if (memcpy_s(tmpKey + baseAddr, P_BYTES - baseAddr, pubKey, *pubKeySize) != EOK) {
            HKS_LOG_E("memcpy_s pubKey to buf failed");
            return HKS_FAILURE;
        }
        if (memcpy_s(pubKey, P_BYTES, tmpKey, P_BYTES) != EOK) {
            HKS_LOG_E("memcpy_s buf to pubKey failed");
            return HKS_FAILURE;
        }
        *pubKeySize = P_BYTES;
    }
    return HKS_SUCCESS;
}

static int32_t BnOperationOfPubKeyConversion(const struct HksBlob *keyIn, struct HksBlob *keyOut,
    struct Curve25519Var *var, BIGNUM *numberOne, BN_CTX *ctx)
{
    uint32_t tmpSize = keyOut->size;
    uint8_t tmpKey[P_BYTES] = {0};
    struct Curve25519Structure curve25519 = {0};
    int32_t ret = Curve25519Initialize(&curve25519, keyIn->data, keyIn->size, true);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    ret = HKS_FAILURE;
    do {
        if (CheckEd25519Pubkey(&curve25519, var, keyIn->data[keyIn->size - 1], ctx) != HKS_SUCCESS) {
            break;
        }
        if (BN_set_word(numberOne, 1) <= 0) {
            break;
        }
        if (BN_sub(var->a, curve25519.p, curve25519.ed25519Pubkey) <= 0) {
            break;
        }
        if (BN_add(var->b, var->a, numberOne) <= 0) {
            break;
        }
        if (BN_mod_exp(var->a, var->b, curve25519.negativeTwo, curve25519.p, ctx) <= 0) {
            break;
        }
        if (BN_add(var->b, curve25519.ed25519Pubkey, numberOne) <= 0) {
            break;
        }
        if (BN_mod_mul(var->c, var->a, var->b, curve25519.p, ctx) <= 0) {
            break;
        }
        if (BN_bn2bin(var->c, tmpKey) <= 0) {
            break;
        }
        tmpSize = (uint32_t)BN_num_bytes(var->c);
        if (FillPubKeyByZero(tmpKey, &tmpSize) != HKS_SUCCESS) {
            break;
        }
        SwapEndianThirtyTwoByte(tmpKey, tmpSize, true);
        if (memcpy_s(keyOut->data, keyOut->size, tmpKey, tmpSize) != EOK) {
            break;
        }
        keyOut->size = tmpSize;
        ret = HKS_SUCCESS;
    } while (0);
    Curve25519Destroy(&curve25519);
    (void)memset_s(tmpKey, P_BYTES, 0, P_BYTES);
    return ret;
}

int32_t ConvertPubkeyX25519FromED25519(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    BN_CTX *ctx = BN_CTX_new();
    if (ctx == NULL) {
        return HKS_FAILURE;
    }
    struct Curve25519Var var = { NULL, NULL, NULL };
    int32_t ret = Curve25519LocalVar(&var);
    if (ret != HKS_SUCCESS) {
        BN_CTX_free(ctx);
        return ret;
    }
    BIGNUM *numberOne = BN_new();
    ret = BnOperationOfPubKeyConversion(keyIn, keyOut, &var, numberOne, ctx);

    BN_CTX_free(ctx);
    BN_free(numberOne);
    FreeLocalBigVar(&var);
    return ret;
}

int32_t ConvertPrivX25519FromED25519(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    uint32_t tmpSize = sizeof(struct KeyMaterial25519) + CURVE25519_KEY_LEN; // 2: pub + pri
    struct KeyMaterial25519 *keyMaterialOut = (struct KeyMaterial25519 *)keyOut->data;
    keyMaterialOut->keySize = tmpSize;
    keyMaterialOut->pubKeySize = 0;
    keyMaterialOut->priKeySize = CURVE25519_KEY_LEN;
    uint32_t offset = sizeof(struct KeyMaterial25519);

    uint8_t *input = keyIn->data + tmpSize;
    const uint32_t inputLen = CURVE25519_KEY_LEN; // Get 32 bytes private key data as the hash input
    uint8_t digest[ED25519_FIX_KEY_BUFFER_SIZE] = {0};
    uint32_t digestLen = ED25519_FIX_KEY_BUFFER_SIZE;

    if (EVP_Digest(input, inputLen, digest, &digestLen, EVP_sha512(), NULL) <= 0) {
        HksLogOpensslError();
        return HKS_FAILURE;
    }

    if (memcpy_s(keyOut->data + offset, keyMaterialOut->priKeySize, digest,
        digestLen / 2) != EOK) { // 2 : used to calculate half of the digest length
        (void)memset_s(digest, digestLen, 0, digestLen);
        HKS_LOG_E("copy digest data to output key failed");
        return HKS_FAILURE;
    }

    // 248 127 64 are the constant value of x25 to ed25 algorithm
    keyOut->size = keyMaterialOut->keySize;
    keyOut->data[offset] &= 248; // 248: RFC 8032
    keyOut->data[keyMaterialOut->keySize - 1] &= 127; // 127: RFC 8032
    keyOut->data[keyMaterialOut->keySize - 1] |= 64;  // 64: RFC 8032
    (void)memset_s(digest, digestLen, 0, digestLen);
    return HKS_SUCCESS;
}
