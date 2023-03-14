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
#undef HKS_SUPPORT_X25519_C
#endif /* _CUT_AUTHENTICATE_ */

#ifdef HKS_SUPPORT_X25519_C

#include "hks_mbedtls_x25519.h"

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>

#ifdef HKS_SUPPORT_ED25519_TO_X25519
#include "hks_crypto_adapter.h"
#endif

#include "hks_log.h"
#include "hks_mbedtls_common.h"
#include "hks_mem.h"

#define HKS_X25519_KEY_BITS 256
#define HKS_X25519_KEY_BYTES 32
#define HASH512_OUT_SIZE 64

struct HksCurveConstPara {
    mbedtls_mpi mpiP;
    mbedtls_mpi mpiOne;
    mbedtls_mpi mpiD;
    mbedtls_mpi mpiK;
    mbedtls_mpi mpiCapitalA;
    mbedtls_mpi mpiMinusA;
    mbedtls_mpi mpiU;
    mbedtls_mpi mpiQ;
};

#ifdef HKS_SUPPORT_ED25519_TO_X25519
#define CURVE25519_KEY_BYTE_SIZE HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256)
static const uint8_t g_hksCurveParamP[CURVE25519_KEY_BYTE_SIZE] = {
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xed
};

/* RFC 8032, one = 1 */
static const uint8_t g_hksCurveParamOne[CURVE25519_KEY_BYTE_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

/* RFC 8032, A non-zero element in the finite field GF(p), not equal to 1 */
static const uint8_t g_hksCurveParamD[CURVE25519_KEY_BYTE_SIZE] = {
    0xa3, 0x78, 0x59, 0x13, 0xca, 0x4d, 0xeb, 0x75, 0xab, 0xd8, 0x41, 0x41, 0x4d, 0x0a, 0x70, 0x00,
    0x98, 0xe8, 0x79, 0x77, 0x79, 0x40, 0xc7, 0x8c, 0x73, 0xfe, 0x6f, 0x2b, 0xee, 0x6c, 0x03, 0x52
};

/* RFC 8032, k = (p - 1) / 2 */
static const uint8_t g_hksCurveParamK[CURVE25519_KEY_BYTE_SIZE] = {
    0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f
};

/* RFC 8032, A = 486662 */
static const uint8_t g_hksCurveParamCapitalA[CURVE25519_KEY_BYTE_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x6d, 0x06
};

/* RFC 8032, -A = -486662 */
static const uint8_t g_hksCurveParamMinusA[CURVE25519_KEY_BYTE_SIZE] = {
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x92, 0xe7
};

/* RFC 8032, u = 2 */
static const uint8_t g_hksCurveParamU[CURVE25519_KEY_BYTE_SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
};

/* RFC 8032, q = endian_swap(k) */
static const uint8_t g_hksCurveParamQ[CURVE25519_KEY_BYTE_SIZE] = {
    0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf6
};
#endif

static int32_t X25519CheckKeyMaterialPubPriSize(const struct KeyMaterial25519 *keyMaterial)
{
    if ((keyMaterial->pubKeySize > HKS_X25519_KEY_BYTES) ||
        (keyMaterial->priKeySize > HKS_X25519_KEY_BYTES)) {
        HKS_LOG_E("Invalid x25519 keyMaterial! pubKeySize = 0x%X, priKeySize = 0x%X",
            keyMaterial->pubKeySize, keyMaterial->priKeySize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

static int32_t EndianSwap(uint8_t *pData, const int32_t length)
{
    if ((pData == NULL) || (length <= 0)) {
        HKS_LOG_E("Endian swap invalid input!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    const int32_t end = length - 1;
    const int32_t count = length / 2; /* 2: count is half of length */

    for (int32_t i = 0; i < count; ++i) {
        uint8_t tmp = pData[i];
        pData[i] = pData[end - i];
        pData[end - i] = tmp;
    }

    return HKS_SUCCESS;
}

static int32_t X25519CheckKeySize(const struct HksBlob *key, const struct KeyMaterial25519 *keyMaterial, bool isPubKey)
{
    if (key->size < (sizeof(struct KeyMaterial25519) + keyMaterial->pubKeySize +
        (isPubKey ? 0 : keyMaterial->priKeySize))) {
        HKS_LOG_E("X25519 key size too small! key size = 0x%X", key->size);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    return HKS_SUCCESS;
}

static int32_t X25519CheckKeyMaterialSize(const struct HksBlob *priKey, const struct HksBlob *pubKey)
{
    const struct KeyMaterial25519 *priKm = (struct KeyMaterial25519 *)(priKey->data);
    const struct KeyMaterial25519 *pubKm = (struct KeyMaterial25519 *)(pubKey->data);

    /* x25519's keySize is fixed at 256, so don't need to check */
    int32_t ret = X25519CheckKeyMaterialPubPriSize(priKm);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("invalid privateKey size");
        return ret;
    }
    ret = X25519CheckKeyMaterialPubPriSize(pubKm);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("invalid publicKey size");
        return ret;
    }

    ret = X25519CheckKeySize(priKey, priKm, false);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return X25519CheckKeySize(pubKey, pubKm, true);
}

#ifdef HKS_SUPPORT_X25519_GENERATE_KEY
static int32_t X25519SaveKeyMaterial(const mbedtls_ecp_point *pub,
    const mbedtls_mpi *pri, struct HksBlob *key)
{
    const uint32_t keyByteLen = HKS_X25519_KEY_BITS / HKS_BITS_PER_BYTE;
    const uint32_t rawMaterialLen = sizeof(struct KeyMaterial25519) + keyByteLen + keyByteLen;
    uint8_t *rawMaterial = (uint8_t *)HksMalloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);

    struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_X25519;
    keyMaterial->keySize = HKS_X25519_KEY_BITS;
    keyMaterial->pubKeySize = keyByteLen;
    keyMaterial->priKeySize = keyByteLen;

    /* mbedtls mbedtls_ecp_point.X is x25519's public key */
    uint32_t offset = sizeof(struct KeyMaterial25519);
    int32_t ret = mbedtls_mpi_write_binary(&(pub->X), rawMaterial + offset, keyMaterial->pubKeySize);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls mpi write to x25519 public key failed! mbedtls ret = 0x%X", ret);
        (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
        HKS_FREE_PTR(rawMaterial);
        return ret;
    }

    ret = EndianSwap(rawMaterial + offset, keyMaterial->pubKeySize);
    if (ret != HKS_SUCCESS) {
        (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
        HKS_FREE_PTR(rawMaterial);
        return ret;
    }

    offset = offset + keyMaterial->pubKeySize;
    ret = mbedtls_mpi_write_binary(pri, rawMaterial + offset, keyMaterial->priKeySize);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls mpi write to x25519 private key failed! mbedtls ret = 0x%X", ret);
        (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
        HKS_FREE_PTR(rawMaterial);
        return ret;
    }

    key->data = rawMaterial;
    key->size = rawMaterialLen;

    return HKS_SUCCESS;
}

int32_t HksMbedtlsX25519GenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    (void)spec;
    mbedtls_ecp_group grp;
    mbedtls_ecp_point pub;
    mbedtls_mpi pri;

    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_entropy_context entropy;
    int32_t ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ecp_group_init(&grp);
    mbedtls_ecp_point_init(&pub);
    mbedtls_mpi_init(&pri);

    do {
        ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_CURVE25519);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls load x25519 group failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = mbedtls_ecdh_gen_public(&grp, &pri, &pub, mbedtls_ctr_drbg_random, &ctrDrbg);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls generate x25519 key failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = X25519SaveKeyMaterial(&pub, &pri, key);
    } while (0);

    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&pub);
    mbedtls_mpi_free(&pri);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}
#endif /* HKS_SUPPORT_X25519_GENERATE_KEY */

#ifdef HKS_SUPPORT_X25519_AGREE_KEY
static int32_t X25519KeyMaterialToPub(const struct HksBlob *pubKey, mbedtls_ecp_point *pub)
{
    const struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)(pubKey->data);
    uint32_t offset = sizeof(struct KeyMaterial25519);

    uint8_t *tmpPubKey = (uint8_t *)HksMalloc(keyMaterial->pubKeySize);
    if (tmpPubKey == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    int32_t ret;
    do {
        if (memcpy_s(tmpPubKey, keyMaterial->pubKeySize, pubKey->data + offset, keyMaterial->pubKeySize) != EOK) {
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        ret = EndianSwap(tmpPubKey, keyMaterial->pubKeySize);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_read_binary(&(pub->X), tmpPubKey, keyMaterial->pubKeySize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi read from x25519 public key failed! mbedtls ret = 0x%X", ret);
            break;
        }

        /* set initial coordinates. Z = 1, X and Y are its standard(affine) coordinates. */
        ret = mbedtls_mpi_lset(&(pub->Z), 1);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi set Z = 1 failed! mbedtls ret = 0x%X", ret);
        }
    } while (0);

    HKS_FREE_PTR(tmpPubKey);
    return ret;
}

static int32_t X25519KeyMaterialToPri(const struct HksBlob *nativeKey, mbedtls_mpi *pri)
{
    const struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)(nativeKey->data);

    uint32_t offset = sizeof(struct KeyMaterial25519) + keyMaterial->pubKeySize;
    int32_t ret = mbedtls_mpi_read_binary(pri, nativeKey->data + offset, keyMaterial->priKeySize);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Mbedtls mpi read from x25519 private key failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsX25519KeyAgreement(const struct HksBlob *nativeKey,
    const struct HksBlob *pubKey, const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    (void)spec;
    int32_t ret = X25519CheckKeyMaterialSize(nativeKey, pubKey);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ecdh_context ctx;
    mbedtls_ecdh_init(&ctx);

    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_entropy_context entropy;
    ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        mbedtls_ecdh_free(&ctx);
        return ret;
    }

    do {
        ret = mbedtls_ecp_group_load(&(ctx.grp), MBEDTLS_ECP_DP_CURVE25519);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls load x25519 group failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = X25519KeyMaterialToPub(pubKey, &(ctx.Qp));
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = X25519KeyMaterialToPri(nativeKey, &(ctx.d));
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = mbedtls_ecdh_compute_shared(&(ctx.grp), &(ctx.z), &(ctx.Qp), &(ctx.d), mbedtls_ctr_drbg_random, &ctrDrbg);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls x25519 shared key failed! mbedtls ret = 0x%X", ret);
            break;
        }

        ret = mbedtls_mpi_write_binary(&(ctx.z), sharedKey->data, HKS_X25519_KEY_BYTES);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls mpi write to shared key failed! mbedtls ret = 0x%X", ret);
            (void)memset_s(sharedKey->data, sharedKey->size, 0, sharedKey->size);
            break;
        }
        sharedKey->size = HKS_X25519_KEY_BYTES;

        ret = EndianSwap(sharedKey->data, sharedKey->size);
    } while (0);

    mbedtls_ecdh_free(&ctx);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    return ret;
}
#endif /* HKS_SUPPORT_X25519_AGREE_KEY */

#ifdef HKS_SUPPORT_X25519_GET_PUBLIC_KEY
static int32_t GetX25519PubKeyCheckParams(const struct HksBlob *keyIn, const struct HksBlob *keyOut)
{
    const struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)(keyIn->data);
    int32_t ret = X25519CheckKeyMaterialPubPriSize(keyMaterial);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = X25519CheckKeySize(keyIn, keyMaterial, false);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* check keyOut */
    if (keyOut->size < (sizeof(struct KeyMaterial25519) + keyMaterial->pubKeySize)) {
        HKS_LOG_E("X25519 public keyOut size too small! keyOut size = 0x%X", keyOut->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsGetX25519PubKey(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    int32_t ret = GetX25519PubKeyCheckParams(keyIn, keyOut);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    const struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)(keyIn->data);
    const uint32_t outLen = sizeof(struct KeyMaterial25519) + keyMaterial->pubKeySize;
    if (memcpy_s(keyOut->data, keyOut->size, (void *)keyMaterial, outLen) != EOK) {
        HKS_LOG_E("Memcpy x25519 pub key failed!");
        return HKS_ERROR_BAD_STATE;
    }
    ((struct KeyMaterial25519 *)(keyOut->data))->priKeySize = 0;
    keyOut->size = outLen;

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_X25519_GET_PUBLIC_KEY */

#if defined(HKS_SUPPORT_ED25519_TO_X25519)
static void HksCurveInitConstPara(struct HksCurveConstPara *para)
{
    mbedtls_mpi_init(&(para->mpiP));
    mbedtls_mpi_init(&(para->mpiOne));
    mbedtls_mpi_init(&(para->mpiD));
    mbedtls_mpi_init(&(para->mpiK));
    mbedtls_mpi_init(&(para->mpiCapitalA));
    mbedtls_mpi_init(&(para->mpiMinusA));
    mbedtls_mpi_init(&(para->mpiU));
    mbedtls_mpi_init(&(para->mpiQ));
}

static void HksCurveFreeConstPara(struct HksCurveConstPara *para)
{
    mbedtls_mpi_free(&(para->mpiP));
    mbedtls_mpi_free(&(para->mpiOne));
    mbedtls_mpi_free(&(para->mpiD));
    mbedtls_mpi_free(&(para->mpiK));
    mbedtls_mpi_free(&(para->mpiCapitalA));
    mbedtls_mpi_free(&(para->mpiMinusA));
    mbedtls_mpi_free(&(para->mpiU));
    mbedtls_mpi_free(&(para->mpiQ));
}

static int32_t HksCurveSetConstPara(struct HksCurveConstPara *para)
{
    int32_t ret = mbedtls_mpi_read_binary(&(para->mpiP), g_hksCurveParamP, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiOne), g_hksCurveParamOne, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiD), g_hksCurveParamD, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiK), g_hksCurveParamK, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiCapitalA), g_hksCurveParamCapitalA, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiMinusA), g_hksCurveParamMinusA, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiU), g_hksCurveParamU, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    ret = mbedtls_mpi_read_binary(&(para->mpiQ), g_hksCurveParamQ, CURVE25519_KEY_BYTE_SIZE);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        return ret;
    }

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_ED25519_TO_X25519 */

#ifdef HKS_SUPPORT_ED25519_TO_X25519
struct HksEd25519ToX25519Blob {
    struct HksBlob rawNativeKey;
    struct HksBlob rawPubKey;
    struct HksBlob rawX25519NativeKey;
    struct HksBlob rawX25519PubKey;
    struct HksBlob kmX25519NativeKey;
    struct HksBlob kmX25519PubKey;
};

static int32_t HksCurvePskCalcA(const mbedtls_mpi *edwardsY,
    const struct HksCurveConstPara *curvePara, mbedtls_mpi *mpiA)
{
    mbedtls_mpi mpiE;
    mbedtls_mpi swap;

    mbedtls_mpi_init(&mpiE);
    mbedtls_mpi_init(&swap);

    int32_t ret = HKS_ERROR_CRYPTO_ENGINE_ERROR;
    do {
        /* mpiA := (y ^ 2 - 1) / (1 + d * y ^ 2) */
        if (mbedtls_mpi_mul_mpi(&mpiE, edwardsY, edwardsY) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_mod_mpi(&mpiE, &mpiE, &(curvePara->mpiP)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_mul_mpi(&swap, &(curvePara->mpiD), &mpiE) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_add_mpi(&swap, &swap, &(curvePara->mpiOne)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_mod_mpi(&swap, &swap, &(curvePara->mpiP)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_inv_mod(&swap, &swap, &(curvePara->mpiP)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_sub_mpi(&mpiE, &mpiE, &(curvePara->mpiOne)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_mod_mpi(&mpiE, &mpiE, &(curvePara->mpiP)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_mul_mpi(&mpiE, &mpiE, &swap) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_mod_mpi(&mpiE, &mpiE, &(curvePara->mpiP)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        if (mbedtls_mpi_add_mpi(&mpiE, &mpiE, &(curvePara->mpiP)) != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        /* Sliding-window exponentiation: mpiA = e^k mod p */
        ret = mbedtls_mpi_exp_mod(mpiA, &mpiE, &(curvePara->mpiK), &(curvePara->mpiP), NULL);
    } while (0);

    mbedtls_mpi_free(&mpiE);
    mbedtls_mpi_free(&swap);

    return ret;
}

static int32_t HksCurvePskCheckA(mbedtls_mpi *mpiA, const uint8_t *ed25519PublicKey,
    size_t len, const struct HksCurveConstPara *curvePara)
{
    /* Check, whether a is a square modulo p (including a = 0) */
    int32_t ret = mbedtls_mpi_cmp_mpi(&(curvePara->mpiOne), mpiA);
    if (ret > 0) {
        HKS_LOG_E("a is not a square modulo p");
        return HKS_ERROR_INVALID_PUBLIC_KEY;
    }

    /* If a == p, the parity bit must be 0 */
    ret = mbedtls_mpi_cmp_mpi(mpiA, &(curvePara->mpiP));
    if (ret == 0) {
        uint32_t value = ed25519PublicKey[len - 1] >> 7; /* 7: RFC8032 */
        /* The previous check has guaranteed that a must be greater than or equal to one */
        value = (value & (uint32_t)mbedtls_mpi_sub_mpi(mpiA, mpiA, &(curvePara->mpiOne)));
        if (value != 0) {
            HKS_LOG_E("the parity bit not zero");
            return HKS_ERROR_INVALID_PUBLIC_KEY;
        }
    }

    return HKS_SUCCESS;
}

static int32_t HksCurvePskCheckPoint(const mbedtls_mpi *edwardsY, const uint8_t *ed25519PublicKey,
    size_t len, const struct HksCurveConstPara *curvePara)
{
    mbedtls_mpi mpiA;
    mbedtls_mpi_init(&mpiA);

    int32_t ret;
    do {
        /* if y >= p, return error */
        ret = mbedtls_mpi_cmp_mpi(edwardsY, &(curvePara->mpiP));
        if (ret >= 0) {
            HKS_LOG_E("edwardsY is greater than or equal to p, ret is %d", ret);
            ret = HKS_ERROR_INVALID_PUBLIC_KEY;
            break;
        }

        /* if y == 1, return error */
        ret = mbedtls_mpi_cmp_mpi(edwardsY, &(curvePara->mpiOne));
        if (ret == 0) {
            HKS_LOG_E("edwards_y equals one");
            ret = HKS_ERROR_INVALID_PUBLIC_KEY;
            break;
        }

        /* a := (y ^ 2 - 1) / (1 + d * y ^ 2) */
        ret = HksCurvePskCalcA(edwardsY, curvePara, &mpiA);
        if (ret != HKS_SUCCESS)
            break;

        ret = HksCurvePskCheckA(&mpiA, ed25519PublicKey, len, curvePara);
    } while (0);

    mbedtls_mpi_free(&mpiA);
    return ret;
}

static int32_t HksCurvePskCalcC(const mbedtls_mpi *edwardsY, struct HksBlob *x25519Key,
    const struct HksCurveConstPara *curvePara)
{
    mbedtls_mpi curveX;
    mbedtls_mpi result;

    mbedtls_mpi_init(&curveX);
    mbedtls_mpi_init(&result);

    int32_t ret;
    do {
        /* If y != 1, c := (1 + y) / (1 - y), otherwise c := 0 */
        ret = mbedtls_mpi_sub_mpi(&result, &(curvePara->mpiOne), edwardsY);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_mod_mpi(&result, &result, &(curvePara->mpiP));
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_inv_mod(&result, &result, &(curvePara->mpiP));
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_add_mpi(&curveX, &(curvePara->mpiOne), edwardsY);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_mul_mpi(&result, &curveX, &result);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_mod_mpi(&result, &result, &(curvePara->mpiP));
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_write_binary(&result, x25519Key->data, x25519Key->size);
    } while (0);

    mbedtls_mpi_free(&curveX);
    mbedtls_mpi_free(&result);

    return ret;
}

static int32_t ConvertPublicKey(const struct HksBlob *publicKeyIn, struct HksBlob *x25519PublicKey)
{
    /* RFC 8032 */
    publicKeyIn->data[publicKeyIn->size - 1] = publicKeyIn->data[publicKeyIn->size - 1] & 0x7f;
    int32_t ret = EndianSwap(publicKeyIn->data, publicKeyIn->size);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_mpi edwardsY;
    struct HksCurveConstPara curvePara;

    mbedtls_mpi_init(&edwardsY);
    HksCurveInitConstPara(&curvePara);

    do {
        ret = HksCurveSetConstPara(&curvePara);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = mbedtls_mpi_read_binary(&edwardsY, publicKeyIn->data, publicKeyIn->size);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            break;
        }

        ret = HksCurvePskCheckPoint(&edwardsY, publicKeyIn->data, publicKeyIn->size, &curvePara);
        if (ret != HKS_SUCCESS) {
            break;
        }

        x25519PublicKey->size = CURVE25519_KEY_BYTE_SIZE;
        x25519PublicKey->data = (uint8_t *)HksMalloc(x25519PublicKey->size);
        if (x25519PublicKey->data == NULL) {
            HKS_LOG_E("malloc failed");
            ret = HKS_ERROR_MALLOC_FAIL;
            break;
        }

        ret = HksCurvePskCalcC(&edwardsY, x25519PublicKey, &curvePara);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = EndianSwap(x25519PublicKey->data, x25519PublicKey->size);
    } while (0);

    HksCurveFreeConstPara(&curvePara);
    mbedtls_mpi_free(&edwardsY);
    if ((ret != HKS_SUCCESS) && (x25519PublicKey->data != NULL)) {
        (void)memset_s(x25519PublicKey->data, x25519PublicKey->size, 0, x25519PublicKey->size);
        HKS_FREE_BLOB(*x25519PublicKey);
    }

    return ret;
}

static int32_t ConvertPrivateKey(const struct HksBlob *privateKeyIn, struct HksBlob *x25519PrivateKey)
{
    uint8_t hash512Data[HASH512_OUT_SIZE] = {0};
    struct HksBlob hash512 = { HASH512_OUT_SIZE, hash512Data };

    /* Get 32-bit private key data as the hash input parameter (little endian) */
    int32_t ret = HksCryptoHalHash(HKS_DIGEST_SHA512, privateKeyIn, &hash512);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("convert private key calc hash failed");
        return ret;
    }

    x25519PrivateKey->size = CURVE25519_KEY_BYTE_SIZE;
    x25519PrivateKey->data = (uint8_t *)HksMalloc(x25519PrivateKey->size);
    if (x25519PrivateKey->data == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    do {
        /* Get the first 32 bytes of the hash value (little endian) */
        if (memcpy_s(x25519PrivateKey->data, x25519PrivateKey->size, hash512.data, CURVE25519_KEY_BYTE_SIZE) != EOK) {
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        x25519PrivateKey->data[0] &= 248;  /* 248: RFC 8032 */
        x25519PrivateKey->data[x25519PrivateKey->size - 1] &= 127; /* 127: RFC 8032 */
        x25519PrivateKey->data[x25519PrivateKey->size - 1] |= 64; /* 64: RFC 8032 */

        ret = EndianSwap(x25519PrivateKey->data, x25519PrivateKey->size);
    } while (0);

    if ((ret != HKS_SUCCESS) && (x25519PrivateKey->data != NULL)) {
        (void)memset_s(x25519PrivateKey->data, x25519PrivateKey->size, 0, x25519PrivateKey->size);
        HKS_FREE_BLOB(*x25519PrivateKey);
    }
    return ret;
}

int32_t MbedtlsEd25519KeyToX25519(const struct HksBlob *ed25519NativeKey,
    const struct HksBlob *ed25519PubKey, struct HksBlob *x25519NativeKey, struct HksBlob *x25519PubKey)
{
    int32_t ret = ConvertPublicKey(ed25519PubKey, x25519PubKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("convert ed25519 public key to x25519 public key failed! ret = %d", ret);
        return ret;
    }

    ret = ConvertPrivateKey(ed25519NativeKey, x25519NativeKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("convert ed25519 private key to x25519 private key failed! ret = %d", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

void HksFreeKeyInfo(struct HksEd25519ToX25519Blob *key)
{
    if (key->rawNativeKey.data != NULL) {
        (void)memset_s(key->rawNativeKey.data, key->rawNativeKey.size, 0, key->rawNativeKey.size);
        HKS_FREE_PTR(key->rawNativeKey.data);
    }
    if (key->rawPubKey.data != NULL) {
        (void)memset_s(key->rawPubKey.data, key->rawPubKey.size, 0, key->rawPubKey.size);
        HKS_FREE_PTR(key->rawPubKey.data);
    }
    if (key->rawX25519NativeKey.data != NULL) {
        (void)memset_s(key->rawX25519NativeKey.data, key->rawX25519NativeKey.size, 0, key->rawX25519NativeKey.size);
        HKS_FREE_PTR(key->rawX25519NativeKey.data);
    }
    if (key->rawX25519PubKey.data != NULL) {
        (void)memset_s(key->rawX25519PubKey.data, key->rawX25519PubKey.size, 0, key->rawX25519PubKey.size);
        HKS_FREE_PTR(key->rawX25519PubKey.data);
    }
    if (key->kmX25519NativeKey.data != NULL) {
        (void)memset_s(key->kmX25519NativeKey.data, key->kmX25519NativeKey.size, 0, key->kmX25519NativeKey.size);
        HKS_FREE_PTR(key->kmX25519NativeKey.data);
    }
    if (key->kmX25519PubKey.data != NULL) {
        (void)memset_s(key->kmX25519PubKey.data, key->kmX25519PubKey.size, 0, key->kmX25519PubKey.size);
        HKS_FREE_PTR(key->kmX25519PubKey.data);
    }
}

int32_t HksMbedtlsEd25519KeyAgreement(const struct HksBlob *nativeKey,
    const struct HksBlob *pubKey, const struct HksKeySpec *spec, struct HksBlob *sharedKey)
{
    (void)spec;
    int32_t ret = X25519CheckKeyMaterialSize(nativeKey, pubKey);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct HksEd25519ToX25519Blob key;
    (void)memset_s(&key, sizeof(key), 0, sizeof(key));

    do {
        ret = HksGetKeyFromMaterial(HKS_ALG_ED25519, false, nativeKey, &(key.rawNativeKey));
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get private key from material failed, ret = 0x%X", ret);
            break;
        }
        ret = HksGetKeyFromMaterial(HKS_ALG_ED25519, true, pubKey, &(key.rawPubKey));
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get public key from material failed, ret = 0x%X", ret);
            break;
        }

        ret = MbedtlsEd25519KeyToX25519(&(key.rawNativeKey), &(key.rawPubKey),
            &(key.rawX25519NativeKey), &(key.rawX25519PubKey));
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("covert from ed25519 to x25519 failed, ret = 0x%X", ret);
            break;
        }

        ret = HksSetKeyToMaterial(HKS_ALG_X25519, false, &(key.rawX25519NativeKey), &(key.kmX25519NativeKey));
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("set private key to material failed, ret = 0x%X", ret);
            break;
        }
        ret = HksSetKeyToMaterial(HKS_ALG_X25519, true, &(key.rawX25519PubKey), &(key.kmX25519PubKey));
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("set publicy key to material failed, ret = 0x%X", ret);
            break;
        }

        ret = HksMbedtlsX25519KeyAgreement(&(key.kmX25519NativeKey), &(key.kmX25519PubKey), NULL, sharedKey);
    } while (0);

    HksFreeKeyInfo(&key);
    return ret;
}
#endif

#endif /* HKS_SUPPORT_X25519_C */
