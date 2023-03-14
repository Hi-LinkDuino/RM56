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

#include "hks_check_paramset.h"

#include "hks_base_check.h"
#include "hks_common_check.h"
#include "hks_log.h"

#ifdef _CUT_AUTHENTICATE_
#undef HKS_SUPPORT_HASH_C
#undef HKS_SUPPORT_RSA_C
#undef HKS_SUPPORT_ECC_C
#undef HKS_SUPPORT_X25519_C
#undef HKS_SUPPORT_ED25519_C
#undef HKS_SUPPORT_KDF_PBKDF2
#endif /* _CUT_AUTHENTICATE_ */

#define HKS_DEFAULT_PBKDF2_ITERATION 1000
#define HKS_DEFAULT_PBKDF2_SALT_SIZE 16

#ifndef _CUT_AUTHENTICATE_
static uint32_t g_genKeyAlg[] = {
#ifdef HKS_SUPPORT_RSA_C
    HKS_ALG_RSA,
#endif
#ifdef HKS_SUPPORT_AES_C
    HKS_ALG_AES,
#endif
#ifdef HKS_SUPPORT_ECC_C
    HKS_ALG_ECC,
#endif
#ifdef HKS_SUPPORT_HMAC_C
    HKS_ALG_HMAC,
#endif
#ifdef HKS_SUPPORT_X25519_C
    HKS_ALG_X25519,
#endif
#ifdef HKS_SUPPORT_ED25519_C
    HKS_ALG_ED25519,
#endif
#ifdef HKS_SUPPORT_DSA_C
    HKS_ALG_DSA,
#endif
#ifdef HKS_SUPPORT_DH_C
    HKS_ALG_DH,
#endif
#ifdef HKS_SUPPORT_ECDH_C
    HKS_ALG_ECDH,
#endif
};

static uint32_t g_importKeyAlg[] = {
#ifdef HKS_SUPPORT_RSA_C
    HKS_ALG_RSA,
#endif
#ifdef HKS_SUPPORT_AES_C
    HKS_ALG_AES,
#endif
#ifdef HKS_SUPPORT_ECC_C
    HKS_ALG_ECC,
#endif
#ifdef HKS_SUPPORT_X25519_C
    HKS_ALG_X25519,
#endif
#ifdef HKS_SUPPORT_ED25519_C
    HKS_ALG_ED25519,
#endif
#ifdef HKS_SUPPORT_DSA_C
    HKS_ALG_DSA,
#endif
};

static uint32_t g_cipherAlg[] = {
#ifdef HKS_SUPPORT_RSA_C
    HKS_ALG_RSA,
#endif
#ifdef HKS_SUPPORT_AES_C
    HKS_ALG_AES,
#endif
};
#ifdef HKS_SUPPORT_API_SIGN_VERIFY
static uint32_t g_signAlg[] = {
#ifdef HKS_SUPPORT_RSA_C
    HKS_ALG_RSA,
#endif
#ifdef HKS_SUPPORT_DSA_C
    HKS_ALG_DSA,
#endif
#ifdef HKS_SUPPORT_ECC_C
    HKS_ALG_ECC,
#endif
#ifdef HKS_SUPPORT_ED25519_C
    HKS_ALG_ED25519,
#endif
};
#endif

static uint32_t g_agreeAlg[] = {
#ifdef HKS_SUPPORT_ECDH_C
    HKS_ALG_ECDH,
#endif
#ifdef HKS_SUPPORT_X25519_C
    HKS_ALG_X25519,
#endif
#ifdef HKS_SUPPORT_DH_C
    HKS_ALG_DH,
#endif
};

static uint32_t g_agreeAlgLocal[] = {
#ifdef HKS_SUPPORT_ECDH_C
    HKS_ALG_ECDH,
#endif
#ifdef HKS_SUPPORT_X25519_C
    HKS_ALG_X25519,
#endif
#ifdef HKS_SUPPORT_DH_C
    HKS_ALG_DH,
#endif
};
#endif /* _CUT_AUTHENTICATE_ */

static uint32_t g_deriveAlg[] = {
#ifdef HKS_SUPPORT_KDF_HKDF
    HKS_ALG_HKDF,
#endif
#ifdef HKS_SUPPORT_KDF_PBKDF2
    HKS_ALG_PBKDF2,
#endif
};

static uint32_t g_deriveAlgLocal[] = {
#ifdef HKS_SUPPORT_KDF_HKDF
    HKS_ALG_HKDF,
#endif
};

static uint32_t g_digest[] = {
    HKS_DIGEST_SHA1,
    HKS_DIGEST_SHA224,
    HKS_DIGEST_SHA256,
    HKS_DIGEST_SHA384,
    HKS_DIGEST_SHA512
};
static uint32_t g_macDigest[] = {
    HKS_DIGEST_SHA1,
    HKS_DIGEST_SHA224,
    HKS_DIGEST_SHA256,
    HKS_DIGEST_SHA384,
    HKS_DIGEST_SHA512,
};
#ifdef HKS_SUPPORT_AES_C
static uint32_t g_aesKeySizeLocal[] = {
    HKS_AES_KEY_SIZE_128,
    HKS_AES_KEY_SIZE_192,
    HKS_AES_KEY_SIZE_256,
};
#endif
#ifdef HKS_SUPPORT_RSA_C
static uint32_t g_rsaKeySizeLocal[] = {
    HKS_RSA_KEY_SIZE_512,
    HKS_RSA_KEY_SIZE_768,
    HKS_RSA_KEY_SIZE_1024,
    HKS_RSA_KEY_SIZE_2048,
    HKS_RSA_KEY_SIZE_3072,
    HKS_RSA_KEY_SIZE_4096,
};
#endif

static uint32_t g_cipherAlgLocal[] = {
#ifdef HKS_SUPPORT_AES_C
    HKS_ALG_AES,
#endif
#ifdef HKS_SUPPORT_RSA_C
    HKS_ALG_RSA,
#endif
};

static int32_t CheckAndGetAlgorithm(
    const struct HksParamSet *paramSet, const uint32_t *expectAlg, uint32_t expectCnt, uint32_t *alg)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param 0x%x failed!", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    ret = HksCheckValue(algParam->uint32Param, expectAlg, expectCnt);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("alg value %u not expected", algParam->uint32Param);
        return HKS_ERROR_INVALID_ALGORITHM;
    }

    *alg = algParam->uint32Param;
    return ret;
}

static int32_t CheckAndGetDigest(
    const struct HksParamSet *paramSet, const uint32_t *expectDigest, uint32_t expectCnt, uint32_t *digest)
{
    struct HksParam *digestParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_DIGEST, &digestParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed!", HKS_TAG_DIGEST);
        return HKS_ERROR_CHECK_GET_DIGEST_FAIL;
    }

    ret = HksCheckValue(digestParam->uint32Param, expectDigest, expectCnt);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("digest value %u not expected", digestParam->uint32Param);
        return HKS_ERROR_INVALID_DIGEST;
    }

    *digest = digestParam->uint32Param;
    return ret;
}

#ifndef _CUT_AUTHENTICATE_
static int32_t CheckGenKeyParamsByAlg(uint32_t alg, const struct HksParamSet *paramSet, struct ParamsValues *params)
{
    int32_t ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_GEN_KEY, paramSet, params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get input params by algorithm failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckFixedParams(alg, HKS_CHECK_TYPE_GEN_KEY, params);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    return HksCheckGenKeyMutableParams(alg, params);
}

static int32_t CheckGenKeyMacDeriveParams(
    uint32_t alg, uint32_t inputPurpose, const struct HksParamSet *paramSet, struct ParamsValues *params)
{
    if (alg != HKS_ALG_AES && alg != HKS_ALG_HMAC) {
        HKS_LOG_E("check mac or derive, not aes alg, alg: %u", alg);
        return HKS_ERROR_INVALID_PURPOSE;
    }

    int32_t ret = HksCheckGenKeyPurpose(alg, inputPurpose);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check purpose invalid, purpose 0x%x", inputPurpose);
        return ret;
    }

    if (inputPurpose == HKS_KEY_PURPOSE_MAC) {
        ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_GEN_MAC_KEY, paramSet, params);
    } else {
        ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_GEN_DERIVE_KEY, paramSet, params);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get input params by algorithm failed, ret = %d", ret);
        return ret;
    }

    if (inputPurpose == HKS_KEY_PURPOSE_MAC) {
        ret = HksCheckFixedParams(alg, HKS_CHECK_TYPE_GEN_MAC_KEY, params);
    } else {
        ret = HksCheckFixedParams(alg, HKS_CHECK_TYPE_GEN_DERIVE_KEY, params);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check fixed params failed, ret = %d", ret);
    }

    return ret;
}

static int32_t CoreCheckGenKeyParams(const struct HksParamSet *paramSet, struct ParamsValues *params)
{
    uint32_t alg;
    int32_t ret = CheckAndGetAlgorithm(paramSet, g_genKeyAlg, HKS_ARRAY_SIZE(g_genKeyAlg), &alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check and get alg failed");
        return ret;
    }

    struct HksParam *purposeParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_PURPOSE, &purposeParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_PURPOSE);
        return HKS_ERROR_CHECK_GET_PURPOSE_FAIL;
    }

    if (((purposeParam->uint32Param & HKS_KEY_PURPOSE_DERIVE) != 0) ||
        ((purposeParam->uint32Param & HKS_KEY_PURPOSE_MAC) != 0)) {
        return CheckGenKeyMacDeriveParams(alg, purposeParam->uint32Param, paramSet, params);
    }

    return CheckGenKeyParamsByAlg(alg, paramSet, params);
}

static int32_t CheckImportKeySize(uint32_t alg, const struct ParamsValues *params, const struct HksBlob *key)
{
    int32_t ret = HKS_SUCCESS;
    switch (alg) {
        case HKS_ALG_AES:
            if (key->size != HKS_KEY_BYTES(params->keyLen.value)) {
                ret = HKS_ERROR_INVALID_KEY_INFO;
            }
            break;
        case HKS_ALG_ED25519:
        case HKS_ALG_X25519:
        case HKS_ALG_RSA:
        case HKS_ALG_ECC:
            if (key->size < sizeof(struct HksPubKeyInfo)) {
                ret = HKS_ERROR_INVALID_KEY_INFO;
            }
            struct HksPubKeyInfo *keyMaterial = (struct HksPubKeyInfo *)(key->data);
            if ((keyMaterial->keyAlg != alg) || (keyMaterial->keySize != params->keyLen.value)) {
                ret = HKS_ERROR_INVALID_KEY_INFO;
            }
            break;
#ifdef HKS_SUPPORT_DSA_C
        case HKS_ALG_DSA:
            break;
#endif
        default:
            return HKS_ERROR_INVALID_ALGORITHM;
    }
    return ret;
}

#ifdef HKS_SUPPORT_API_SIGN_VERIFY
static int32_t CheckSignVerifyParamsByAlg(uint32_t cmdId, uint32_t alg, const struct ParamsValues *inputParams)
{
    int32_t ret = HksCheckFixedParams(alg, HKS_CHECK_TYPE_USE_KEY, inputParams);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check sign or verify fixed params failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckSignVerifyMutableParams(cmdId, alg, inputParams);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check sign or verify mutable params failed, ret = %d", ret);
    }

    return ret;
}
#endif
#endif /* _CUT_AUTHENTICATE_ */

static int32_t CheckCipherParamsByAlg(
    uint32_t cmdId, uint32_t alg, const struct HksParamSet *paramSet, const struct ParamsValues *inputParams)
{
    int32_t ret = HksCheckFixedParams(alg, HKS_CHECK_TYPE_USE_KEY, inputParams);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher check fixed params failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckCipherMutableParams(cmdId, alg, inputParams);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher check mutable params failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckCipherMaterialParams(alg, inputParams, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher check material params failed, ret = %d", ret);
    }

    return ret;
}

#ifndef _CUT_AUTHENTICATE_
#ifdef HKS_SUPPORT_KDF_PBKDF2
static int32_t CheckPbkdf2DeriveKeyParams(const struct HksParamSet *paramSet)
{
    struct HksParam *iterationParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ITERATION, &iterationParam);
    if (ret != HKS_SUCCESS) {
        return HKS_ERROR_CHECK_GET_ITERATION_FAIL;
    }

    if (iterationParam->uint32Param < HKS_DEFAULT_PBKDF2_ITERATION) {
        return HKS_ERROR_INVALID_ITERATION;
    }

    struct HksParam *saltParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_SALT, &saltParam);
    if (ret != HKS_SUCCESS) {
        return HKS_ERROR_CHECK_GET_SALT_FAIL;
    }

    if ((CheckBlob(&saltParam->blob) != HKS_SUCCESS) || (saltParam->blob.size < HKS_DEFAULT_PBKDF2_SALT_SIZE)) {
        return HKS_ERROR_INVALID_SALT;
    }

    return HKS_SUCCESS;
}
#endif

int32_t HksCoreCheckGenKeyParams(const struct HksBlob *keyAlias, const struct HksParamSet *paramSet,
    const struct HksBlob *keyIn, const struct HksBlob *keyOut)
{
    (void)keyAlias;
    (void)keyIn;
    (void)keyOut;
    struct ParamsValues params;
    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    return CoreCheckGenKeyParams(paramSet, &params);
}

int32_t HksCoreCheckImportKeyParams(const struct HksBlob *keyAlias, const struct HksBlob *key,
    const struct HksParamSet *paramSet, const struct HksBlob *keyOut)
{
    (void)keyAlias;
    (void)keyOut;
    /* import key paramset is subset of generate key paramset */
    struct ParamsValues params;
    (void)memset_s(&params, sizeof(params), 0, sizeof(params));
    int32_t ret = CoreCheckGenKeyParams(paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("CheckImportKeyParams failed");
        return ret;
    }

    uint32_t alg;
    ret = CheckAndGetAlgorithm(paramSet, g_importKeyAlg, HKS_ARRAY_SIZE(g_importKeyAlg), &alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("import key check and get alg failed");
        return ret;
    }

    /* check keySize */
    ret = CheckImportKeySize(alg, &params, key);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("import key check key size invalid");
        return ret;
    }

    /* check mutable params */
    return CheckImportMutableParams(alg, &params);
}

int32_t HksCoreCheckSignVerifyParams(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
#ifdef HKS_SUPPORT_API_SIGN_VERIFY
    (void)srcData;
    uint32_t alg;
    int32_t ret = CheckAndGetAlgorithm(paramSet, g_signAlg, HKS_ARRAY_SIZE(g_signAlg), &alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check and get alg failed");
        return ret;
    }

    struct ParamsValues params;
    (void)memset_s(&params, sizeof(params), 0, sizeof(params));

    ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_USE_KEY, paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("sign or verify get input params failed, ret = %d", ret);
        return ret;
    }

    ret = CheckSignVerifyParamsByAlg(cmdId, alg, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("sign or verify check params failed, ret = %d", ret);
        return ret;
    }

    uint32_t keySize = 0;
    ret = HksGetKeySize(alg, key, &keySize);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keySize failed!");
        return ret;
    }

    ret = HksCheckSignature(cmdId, alg, keySize, signature);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check signature failed, ret = %d", ret);
    }

    return ret;
#else
    (void)cmdId;
    (void)key;
    (void)paramSet;
    (void)srcData;
    (void)signature;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

int32_t HksLocalCheckSignVerifyParams(uint32_t cmdId, uint32_t keySize, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *signature)
{
#ifdef HKS_SUPPORT_API_SIGN_VERIFY
    (void)srcData;
    uint32_t alg;
    int32_t ret = CheckAndGetAlgorithm(paramSet, g_signAlg, HKS_ARRAY_SIZE(g_signAlg), &alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check and get alg failed");
        return ret;
    }

    struct ParamsValues params;
    (void)memset_s(&params, sizeof(params), 0, sizeof(params));

    ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_USE_KEY, paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("sign or verify get input params failed, ret = %d", ret);
        return ret;
    }

    ret = CheckSignVerifyParamsByAlg(cmdId, alg, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("sign or verify check params failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckSignature(cmdId, alg, keySize, signature);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check signature failed, ret = %d", ret);
    }

    return ret;
#else
    (void)cmdId;
    (void)keySize;
    (void)paramSet;
    (void)srcData;
    (void)signature;
    return HKS_ERROR_NOT_SUPPORTED;
#endif
}

int32_t HksCoreCheckAgreeKeyParams(const struct HksParamSet *paramSet, const struct HksBlob *privateKey,
    const struct HksBlob *peerPublicKey, const struct HksBlob *agreedKey, bool isLocalCheck)
{
    uint32_t alg;
    int32_t ret;

    if (isLocalCheck) {
        ret = CheckAndGetAlgorithm(paramSet, g_agreeAlgLocal, HKS_ARRAY_SIZE(g_agreeAlgLocal), &alg);
    } else {
        ret = CheckAndGetAlgorithm(paramSet, g_agreeAlg, HKS_ARRAY_SIZE(g_agreeAlg), &alg);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check alg failed");
        return ret;
    }

    uint32_t keySize = 0;
    if (isLocalCheck) {
        if (alg == HKS_ALG_ED25519) {
            if ((privateKey->size != HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256)) ||
                (peerPublicKey->size != HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256))) {
                return HKS_ERROR_INVALID_KEY_SIZE;
            }
        }

        if (alg == HKS_ALG_DH || alg == HKS_ALG_ECC || alg == HKS_ALG_ECDH) {
            keySize = ((struct HksKeyMaterialHeader *)privateKey->data)->keySize;
        } else if (alg == HKS_ALG_ED25519) {
            keySize = privateKey->size * HKS_BITS_PER_BYTE;
        }
    } else {
        ret = HksGetKeySize(alg, privateKey, &keySize);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get key size failed");
            return ret;
        }
    }

    uint32_t size = keySize / HKS_BITS_PER_BYTE + keySize % HKS_BITS_PER_BYTE;
    if (agreedKey->size < size) {
        HKS_LOG_E("agreeKey buffer too small, size %u", agreedKey->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return HKS_SUCCESS;
}

int32_t HksCoreCheckCipherParams(uint32_t cmdId, const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, const struct HksBlob *outData)
{
    uint32_t alg;
    int32_t ret = CheckAndGetAlgorithm(paramSet, g_cipherAlg, HKS_ARRAY_SIZE(g_cipherAlg), &alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check alg failed");
        return ret;
    }

    struct ParamsValues params;
    (void)memset_s(&params, sizeof(params), 0, sizeof(params));

    ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_USE_KEY, paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher get input params failed, ret = %d", ret);
        return ret;
    }

    if (alg == HKS_ALG_RSA) {
        ret = HksGetKeySize(alg, key, &params.keyLen.value);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("rsa cipher get key size failed");
            return ret;
        }
    }

    ret = CheckCipherParamsByAlg(cmdId, alg, paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher check params failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckCihperData(cmdId, alg, &params, inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher check input or output data failed, ret = %d", ret);
    }

    return ret;
}
#endif /* _CUT_AUTHENTICATE_ */

int32_t HksLocalCheckCipherParams(uint32_t cmdId, uint32_t keySize, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, const struct HksBlob *outData)
{
    uint32_t alg;
    int32_t ret = CheckAndGetAlgorithm(paramSet, g_cipherAlgLocal, HKS_ARRAY_SIZE(g_cipherAlgLocal), &alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check alg failed");
        return ret;
    }

    if (alg == HKS_ALG_AES) {
#ifdef HKS_SUPPORT_AES_C
        ret = HksCheckValue(keySize, g_aesKeySizeLocal, HKS_ARRAY_SIZE(g_aesKeySizeLocal));
#else
        ret = HKS_ERROR_NOT_SUPPORTED;
#endif
    } else if (alg == HKS_ALG_RSA) {
#ifdef HKS_SUPPORT_RSA_C
        ret = HksCheckValue(keySize, g_rsaKeySizeLocal, HKS_ARRAY_SIZE(g_rsaKeySizeLocal));
#else
        ret = HKS_ERROR_NOT_SUPPORTED;
#endif
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("keySize value %u not expected", keySize);
        return HKS_ERROR_INVALID_KEY_SIZE;
    }

    struct ParamsValues params;
    (void)memset_s(&params, sizeof(params), 0, sizeof(params));

    ret = HksGetInputParmasByAlg(alg, HKS_CHECK_TYPE_USE_KEY, paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("local cipher get input params failed, ret = %d", ret);
        return ret;
    }

    ret = CheckCipherParamsByAlg(cmdId, alg, paramSet, &params);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("local cipher check params failed, ret = %d", ret);
        return ret;
    }

    ret = HksCheckCihperData(cmdId, alg, &params, inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("local cipher check input or output data failed, ret = %d", ret);
    }

    return ret;
}

int32_t HksCoreCheckDeriveKeyParams(const struct HksParamSet *paramSet, const struct HksBlob *mainKey,
    const struct HksBlob *derivedKey, bool isLocalCheck)
{
    (void)mainKey;
    (void)derivedKey;
    uint32_t alg;
    int32_t ret;
    if (isLocalCheck) {
        ret = CheckAndGetAlgorithm(paramSet, g_deriveAlgLocal, HKS_ARRAY_SIZE(g_deriveAlgLocal), &alg);
    } else {
        ret = CheckAndGetAlgorithm(paramSet, g_deriveAlg, HKS_ARRAY_SIZE(g_deriveAlg), &alg);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check alg failed");
        return ret;
    }

    struct HksParam *purposeParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_PURPOSE, &purposeParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_PURPOSE);
        return HKS_ERROR_CHECK_GET_PURPOSE_FAIL;
    }

    if (purposeParam->uint32Param != HKS_KEY_PURPOSE_DERIVE) {
        return HKS_ERROR_INVALID_PURPOSE;
    }

    /* according to RFC5869, HKDF no need check salt and info */
    uint32_t digest;
    ret = CheckAndGetDigest(paramSet, g_digest, HKS_ARRAY_SIZE(g_digest), &digest);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check digest failed");
        return ret;
    }

#ifdef HKS_SUPPORT_KDF_PBKDF2
    if (alg == HKS_ALG_PBKDF2) {
        return CheckPbkdf2DeriveKeyParams(paramSet);
    }
#endif

    return HKS_SUCCESS;
}

static int32_t CheckMacPurpose(const struct HksParamSet *paramSet)
{
    struct HksParam *purposeParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_PURPOSE, &purposeParam);
    if (ret != HKS_SUCCESS) {
        return HKS_ERROR_CHECK_GET_PURPOSE_FAIL;
    }

    if (purposeParam->uint32Param != HKS_KEY_PURPOSE_MAC) {
        return HKS_ERROR_INVALID_PURPOSE;
    }

    return HKS_SUCCESS;
}

static int32_t CheckMacOutput(
    const struct HksBlob *key, const struct HksParamSet *paramSet, const struct HksBlob *mac, bool isLocalCheck)
{
    uint32_t digest;
    int32_t ret;
    if (isLocalCheck) {
        ret = CheckAndGetDigest(paramSet, g_digest, HKS_ARRAY_SIZE(g_digest), &digest);
    } else {
        ret = CheckAndGetDigest(paramSet, g_macDigest, HKS_ARRAY_SIZE(g_macDigest), &digest);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check digest failed");
        return ret;
    }

    uint32_t digestLen;
    ret = HksGetDigestLen(digest, &digestLen);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get digest length failed, ret = %d", ret);
        return ret;
    }

    if (mac->size < digestLen) {
        HKS_LOG_E("mac buffer too small, size %u", mac->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if ((isLocalCheck) && (key->size < digestLen)) { /* the unit of local engine input key size is byte */
        HKS_LOG_E("key size too small, size = %u", key->size);
        return HKS_ERROR_INVALID_KEY_SIZE;
    }

    return HKS_SUCCESS;
}

int32_t HksCoreCheckMacParams(const struct HksBlob *key, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, const struct HksBlob *mac, bool isLocalCheck)
{
    (void)srcData;
    int32_t ret = CheckMacPurpose(paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check Mac purpose failed");
        return ret;
    }

    return CheckMacOutput(key, paramSet, mac, isLocalCheck);
}
