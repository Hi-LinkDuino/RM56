/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#undef HKS_SUPPORT_ECC_C
#endif /* _CUT_AUTHENTICATE_ */

#ifdef HKS_SUPPORT_ECC_C

#include "hks_mbedtls_ecc.h"

#include <mbedtls/bignum.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecp.h>
#include <mbedtls/entropy.h>

#include "hks_log.h"
#include "hks_mbedtls_common.h"
#include "hks_mem.h"

#define HKS_ECC_KEYPAIR_CNT 3

static int32_t HksMbedtlsEccCheckKeySize(const uint32_t keySize)
{
    if ((keySize != HKS_ECC_KEY_SIZE_224) && (keySize != HKS_ECC_KEY_SIZE_256) && (keySize != HKS_ECC_KEY_SIZE_384) &&
        (keySize != HKS_ECC_KEY_SIZE_521)) {
        HKS_LOG_E("Invalid ecc keySize! keySize = 0x%X", keySize);
        return HKS_ERROR_INVALID_KEY_SIZE;
    }

    return HKS_SUCCESS;
}

int32_t GetEccGroupId(const uint32_t keyLen, mbedtls_ecp_group_id *grpId)
{
    switch (keyLen) {
        case HKS_ECC_KEY_SIZE_224:
            *grpId = MBEDTLS_ECP_DP_SECP224R1;
            break;
        case HKS_ECC_KEY_SIZE_256:
            *grpId = MBEDTLS_ECP_DP_SECP256R1;
            break;
        case HKS_ECC_KEY_SIZE_384:
            *grpId = MBEDTLS_ECP_DP_SECP384R1;
            break;
        case HKS_ECC_KEY_SIZE_521:
            *grpId = MBEDTLS_ECP_DP_SECP521R1;
            break;
        default:
            HKS_LOG_E("Unsupported key length! keyLen: 0x%X", keyLen);
            return HKS_ERROR_INVALID_KEY_SIZE;
    }
    return HKS_SUCCESS;
}

static int32_t EccKeyMaterialXyzSizeCheck(const struct KeyMaterialEcc *keyMaterial)
{
    const uint32_t maxKeyByteLen = HKS_KEY_BYTES(HKS_ECC_KEY_SIZE_521);
    if ((keyMaterial->xSize > maxKeyByteLen) ||
        (keyMaterial->ySize > maxKeyByteLen) || (keyMaterial->zSize > maxKeyByteLen)) {
        HKS_LOG_E("Invalid ecc keyMaterial! xSize = 0x%X, ySize = 0x%X, zSize = 0x%X",
            keyMaterial->xSize, keyMaterial->ySize, keyMaterial->zSize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

int32_t EccKeyCheck(const struct HksBlob *key)
{
    const struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)(key->data);
    int32_t ret = HksMbedtlsEccCheckKeySize(keyMaterial->keySize);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = EccKeyMaterialXyzSizeCheck(keyMaterial);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    if (key->size < (sizeof(struct KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize + keyMaterial->zSize)) {
        HKS_LOG_E("Ecc key size too small! key size = 0x%X", key->size);
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    return HKS_SUCCESS;
}

#if defined(HKS_SUPPORT_ECDH_C) || defined(HKS_SUPPORT_ECDSA_C)
int32_t HksMbedtlsEccGetKeyCurveNist(const struct KeyMaterialEcc *keyMaterial, mbedtls_ecp_group_id *curve)
{
    if ((keyMaterial->keyAlg != HKS_ALG_ECC) && (keyMaterial->keyAlg != HKS_ALG_ECDH)) {
        HKS_LOG_E("Invalid param key! key mode = 0x%X", keyMaterial->keyAlg);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return GetEccGroupId(keyMaterial->keySize, curve);
}

int32_t HksEccKeyMaterialToPub(const struct HksBlob *key, mbedtls_ecp_point *pub)
{
    const struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)(key->data);

    uint32_t offset = sizeof(*keyMaterial);
    int32_t ret = mbedtls_mpi_read_binary(&(pub->X), key->data + offset, keyMaterial->xSize);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Ecc keyMaterial to public key read X failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    offset = offset + keyMaterial->xSize;
    ret = mbedtls_mpi_read_binary(&(pub->Y), key->data + offset, keyMaterial->ySize);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Ecc keyMaterial to public key read Y failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    /* Z = 1, X and Y are its standard (affine) coordinates */
    ret = mbedtls_mpi_lset(&(pub->Z), 1);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Ecc keyMaterial to public key set Z failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

int32_t HksEccKeyMaterialToPri(const struct HksBlob *key, mbedtls_mpi *pri)
{
    const struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)(key->data);

    uint32_t offset = sizeof(*keyMaterial) + keyMaterial->xSize + keyMaterial->ySize;
    int32_t ret = mbedtls_mpi_read_binary(pri, key->data + offset, keyMaterial->zSize);
    if (ret != HKS_MBEDTLS_SUCCESS) {
        HKS_LOG_E("Ecc keyMaterial to private key read Z failed! mbedtls ret = 0x%X", ret);
        return ret;
    }

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_ECDH_C or HKS_SUPPORT_ECDSA_C */

#ifdef HKS_SUPPORT_ECC_GENERATE_KEY
static int32_t EccSaveKeyMaterial(const mbedtls_ecp_keypair *ecp,
    const uint32_t keySize, struct HksBlob *key)
{
    /* public exponent x and y, and private exponent, so need size is: key_size / 8 * 3 */
    const uint32_t keyByteLen = HKS_KEY_BYTES(keySize);
    const uint32_t rawMaterialLen = sizeof(struct KeyMaterialEcc) + keyByteLen * HKS_ECC_KEYPAIR_CNT;
    uint8_t *rawMaterial = (uint8_t *)HksMalloc(rawMaterialLen);
    if (rawMaterial == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);

    /* ECC key data internal struct: struct KeyMaterialEcc + pubXData + pubYData + priData */
    struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)rawMaterial;
    keyMaterial->keyAlg = HKS_ALG_ECC;
    keyMaterial->keySize = keySize;
    keyMaterial->xSize = keyByteLen;
    keyMaterial->ySize = keyByteLen;
    keyMaterial->zSize = keyByteLen;

    int32_t ret;
    do {
        uint32_t offset = sizeof(struct KeyMaterialEcc);
        ret = mbedtls_mpi_write_binary(&(ecp->Q.X), rawMaterial + offset, keyMaterial->xSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Save ecc keyMaterial write X failed! mbedtls ret = 0x%X", ret);
            break;
        }

        offset = offset + keyMaterial->xSize;
        ret = mbedtls_mpi_write_binary(&(ecp->Q.Y), rawMaterial + offset, keyMaterial->ySize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Save ecc keyMaterial write Y failed! mbedtls ret = 0x%X", ret);
            break;
        }

        offset = offset + keyMaterial->ySize;
        ret = mbedtls_mpi_write_binary(&(ecp->d), rawMaterial + offset, keyMaterial->zSize);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Save ecc keyMaterial write D failed! mbedtls ret = 0x%X", ret);
            break;
        }

        key->data = rawMaterial;
        key->size = rawMaterialLen;
    } while (0);

    if (ret != HKS_MBEDTLS_SUCCESS) {
        (void)memset_s(rawMaterial, rawMaterialLen, 0, rawMaterialLen);
        HKS_FREE_PTR(rawMaterial);
    }

    return ret;
}

int32_t HksMbedtlsEccGenerateKey(const struct HksKeySpec *spec, struct HksBlob *key)
{
    mbedtls_ecp_group_id grpId;
    int32_t ret = GetEccGroupId(spec->keyLen, &grpId);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    ret = HksCtrDrbgSeed(&ctrDrbg, &entropy);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    mbedtls_ecp_keypair ecp;
    mbedtls_ecp_keypair_init(&ecp);

    do {
        ret = mbedtls_ecp_gen_key(grpId, &ecp, mbedtls_ctr_drbg_random, &ctrDrbg);
        if (ret != HKS_MBEDTLS_SUCCESS) {
            HKS_LOG_E("Mbedtls ecc generate key failed! ret = 0x%X", ret);
            break;
        }

        ret = EccSaveKeyMaterial(&ecp, spec->keyLen, key);
    } while (0);

    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_ecp_keypair_free(&ecp);
    return ret;
}
#endif /* HKS_SUPPORT_ECC_GENERATE_KEY */

#ifdef HKS_SUPPORT_ECC_GET_PUBLIC_KEY
static int32_t GetEccPubKeyCheckParams(const struct HksBlob *keyIn, const struct HksBlob *keyOut)
{
    int32_t ret = EccKeyCheck(keyIn);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* check keyOut size */
    const struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)(keyIn->data);
    if (keyOut->size < (sizeof(struct KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize)) {
        HKS_LOG_E("Ecc public keyOut size too small! keyOut size = 0x%X", keyOut->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return HKS_SUCCESS;
}

int32_t HksMbedtlsGetEccPubKey(const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    int32_t ret = GetEccPubKeyCheckParams(keyIn, keyOut);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* x + y, so need size is: sizeof(struct HksPubKeyInfo) + xSize + ySize */
    const struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)(keyIn->data);
    uint32_t outLen = sizeof(struct KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize;
    if (memcpy_s(keyOut->data, keyOut->size, (void *)keyMaterial, outLen) != EOK) {
        HKS_LOG_E("Memcpy ecc public key fail!");
        (void)memset_s(keyOut->data, keyOut->size, 0, keyOut->size);
        return HKS_ERROR_BAD_STATE;
    }
    ((struct KeyMaterialEcc *)(keyOut->data))->zSize = 0;
    keyOut->size = outLen;

    return HKS_SUCCESS;
}
#endif /* HKS_SUPPORT_ECC_GET_PUBLIC_KEY */
#endif /* HKS_SUPPORT_ECC_C */
