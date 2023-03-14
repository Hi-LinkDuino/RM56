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

#include "hks_crypto_adapter.h"

#include "hks_common_check.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"

void HksFillKeySpec(const struct HksParamSet *paramSet, struct HksKeySpec *spec)
{
    for (uint32_t i = 0; i < paramSet->paramsCnt; i++) {
        switch (paramSet->params[i].tag) {
            case HKS_TAG_ALGORITHM:
                spec->algType = paramSet->params[i].uint32Param;
                break;
            case HKS_TAG_KEY_SIZE:
                spec->keyLen = paramSet->params[i].uint32Param;
                break;
            default:
                break;
        }
    }
}

void HksFillUsageSpec(const struct HksParamSet *paramSet, struct HksUsageSpec *usageSpec)
{
    for (uint32_t i = 0; i < paramSet->paramsCnt; i++) {
        switch (paramSet->params[i].tag) {
            case HKS_TAG_ALGORITHM:
                usageSpec->algType = paramSet->params[i].uint32Param;
                break;
            case HKS_TAG_PADDING:
                usageSpec->padding = paramSet->params[i].uint32Param;
                break;
            case HKS_TAG_DIGEST:
                usageSpec->digest = paramSet->params[i].uint32Param;
                break;
            case HKS_TAG_BLOCK_MODE:
                usageSpec->mode = paramSet->params[i].uint32Param;
                break;
            case HKS_TAG_PURPOSE:
                usageSpec->purpose = paramSet->params[i].uint32Param;
                break;
            default:
                break;
        }
    }
    usageSpec->algParam = NULL;
}

void HksFreeUsageSpec(struct HksUsageSpec **usageSpec)
{
    if ((usageSpec == NULL) || (*usageSpec == NULL)) {
        return;
    }

    if ((*usageSpec)->algParam != NULL) {
        HKS_FREE_PTR((*usageSpec)->algParam);
    }
    HKS_FREE_PTR(*usageSpec);
    *usageSpec = NULL;
}

void HksFillKeyDerivationParam(const struct HksParamSet *paramSet, struct HksKeyDerivationParam *param)
{
    for (uint32_t i = 0; i < paramSet->paramsCnt; i++) {
        switch (paramSet->params[i].tag) {
            case HKS_TAG_DIGEST:
                param->digestAlg = paramSet->params[i].uint32Param;
                break;
            case HKS_TAG_SALT:
                param->salt = paramSet->params[i].blob;
                break;
            case HKS_TAG_INFO:
                param->info = paramSet->params[i].blob;
                break;
            case HKS_TAG_ITERATION:
                param->iterations = paramSet->params[i].uint32Param;
                break;
            default:
                break;
        }
    }
}

int32_t HksFillAeadParam(
    const struct HksParamSet *paramSet, struct HksBlob *inputText, struct HksUsageSpec *usageSpec, bool isEncrypt)
{
    struct HksParam *nonceParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_NONCE, &nonceParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksFillAeadParam get nonce param failed!");
        return ret;
    }

    struct HksParam *aadParam = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_ASSOCIATED_DATA, &aadParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksFillAeadParam get aad param failed!");
        return ret;
    }

    struct HksParam tagParam;
    if (!isEncrypt) {
        if (inputText->size <= HKS_AE_TAG_LEN) {
            HKS_LOG_E("too small inputText size");
            return HKS_ERROR_INVALID_ARGUMENT;
        }
        inputText->size -= HKS_AE_TAG_LEN;

        tagParam.blob.size = HKS_AE_TAG_LEN;
        tagParam.blob.data = inputText->data + inputText->size;
    }

    struct HksAeadParam *aeadParam = (struct HksAeadParam *)HksMalloc(sizeof(struct HksAeadParam));
    if (aeadParam == NULL) {
        HKS_LOG_E("aeadParam malloc failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (!isEncrypt) {
        aeadParam->tagDec = tagParam.blob;
    } else {
        aeadParam->tagLenEnc = HKS_AE_TAG_LEN;
    }

    aeadParam->nonce = nonceParam->blob;
    aeadParam->aad = aadParam->blob;
    aeadParam->payloadLen = 0;
    usageSpec->algParam = aeadParam;
    return HKS_SUCCESS;
}

int32_t HksFillIvParam(const struct HksParamSet *paramSet, struct HksUsageSpec *usageSpec)
{
    struct HksParam *ivParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_IV, &ivParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher get iv param failed!");
        return ret;
    }

    struct HksCipherParam *param = (struct HksCipherParam *)HksMalloc(sizeof(struct HksCipherParam));
    if (param == NULL) {
        HKS_LOG_E("param malloc failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }

    param->iv = ivParam->blob;
    usageSpec->algParam = param;
    return HKS_SUCCESS;
}

int32_t HksBuildCipherUsageSpec(
    const struct HksParamSet *paramSet, bool isEncrypt, struct HksBlob *inputText, struct HksUsageSpec **outUsageSpec)
{
    bool isAes = false;
    bool isAeMode = false;
    int32_t ret = HksCheckAesAeMode(paramSet, &isAes, &isAeMode);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get aeMode failed!");
        return ret;
    }

    struct HksUsageSpec *usageSpec = (struct HksUsageSpec *)HksMalloc(sizeof(struct HksUsageSpec));
    if (usageSpec == NULL) {
        HKS_LOG_E("cipher usageSpec malloc failed!");
        return HKS_ERROR_MALLOC_FAIL;
    }

    HksFillUsageSpec(paramSet, usageSpec);

    if (usageSpec->algType == HKS_ALG_RSA && usageSpec->digest == HKS_DIGEST_NONE) {
        usageSpec->digest = HKS_DIGEST_SHA1;
    }

    if (!isAes) {
        *outUsageSpec = usageSpec;
        return HKS_SUCCESS;
    }

    if (isAeMode) {
        ret = HksFillAeadParam(paramSet, inputText, usageSpec, isEncrypt);
    } else {
        ret = HksFillIvParam(paramSet, usageSpec);
    }

    if (ret != HKS_SUCCESS) {
        HksFreeUsageSpec(&usageSpec);
        HKS_LOG_E("fill[%x] param failed!", isAeMode);
        return ret;
    }

    *outUsageSpec = usageSpec;
    return HKS_SUCCESS;
}

int32_t HksGetEncryptAeTag(
    const struct HksParamSet *paramSet, const struct HksBlob *inData, struct HksBlob *outData, struct HksBlob *tagAead)
{
    bool isAes = false;
    bool isAeMode = false;
    int32_t ret = HksCheckAesAeMode(paramSet, &isAes, &isAeMode);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get aeMode failed!");
        return ret;
    }

    if ((!isAes) || (!isAeMode)) {
        tagAead->data = NULL;
        tagAead->size = 0;
        return HKS_SUCCESS;
    }

    if (outData->size < (inData->size + HKS_AE_TAG_LEN)) {
        HKS_LOG_E("too small out buf!");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    tagAead->data = outData->data + inData->size;
    tagAead->size = HKS_AE_TAG_LEN;
    return HKS_SUCCESS;
}

#ifndef _CUT_AUTHENTICATE_
static int32_t SetCurve25519KeyMaterial(bool isPubKey, const struct HksBlob *keyIn, struct HksBlob *keyOut)
{
    struct KeyMaterial25519 curve25519Km = {0};
    curve25519Km.keySize = HKS_CURVE25519_KEY_SIZE_256;
    curve25519Km.reserved = 0;

    uint32_t offset = sizeof(struct KeyMaterial25519);
    if (isPubKey) {
        curve25519Km.pubKeySize = keyIn->size;
        curve25519Km.priKeySize = 0;
    } else {
        curve25519Km.pubKeySize = 0;
        curve25519Km.priKeySize = keyIn->size;
    }

    keyOut->size = sizeof(struct KeyMaterial25519) + curve25519Km.pubKeySize + curve25519Km.priKeySize;
    keyOut->data = (uint8_t *)HksMalloc(keyOut->size);
    if (keyOut->data == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (memcpy_s(keyOut->data, keyOut->size, &curve25519Km, sizeof(struct KeyMaterial25519)) != EOK) {
        HKS_FREE_PTR(keyOut->data);
        return HKS_ERROR_BAD_STATE;
    }

    if (memcpy_s(keyOut->data + offset, keyOut->size - offset, keyIn->data, keyIn->size) != EOK) {
        HKS_FREE_PTR(keyOut->data);
        return HKS_ERROR_BAD_STATE;
    }

    return HKS_SUCCESS;
}

static int32_t CheckCurve25519KeySize(const struct HksBlob *keyIn)
{
    if (keyIn->size < sizeof(struct KeyMaterial25519)) {
        HKS_LOG_E("keyIn buffer too small");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)keyIn->data;

    /* input pubKeySize and priKeySize of keyMaterial have been guaranteed that the addition will not overflow */
    if (keyIn->size < (sizeof(struct KeyMaterial25519) + keyMaterial->pubKeySize + keyMaterial->priKeySize)) {
        HKS_LOG_E("keyIn is not a valid key material");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

static int32_t CheckFormatCurve25519Key(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    int32_t ret = CheckCurve25519KeySize(keyIn);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)keyIn->data;
    uint32_t offset = sizeof(struct HksParamSet) + (sizeof(struct HksParam) << 1);
    if (paramSetOut->paramSetSize < (offset + keyMaterial->pubKeySize + keyMaterial->priKeySize)) {
        HKS_LOG_E("pubKey or priKey buffer too small");
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    return HKS_SUCCESS;
}

static int32_t BuildParamSetOut(const struct HksParam *params, uint32_t paramCnt, struct HksParamSet *paramSetOut)
{
    int32_t ret;
    struct HksParamSet *tmpParamSetOut = NULL;

    ret = HksInitParamSet(&tmpParamSetOut);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("init paramSet failed");
        return ret;
    }

    ret = HksAddParams(tmpParamSetOut, params, paramCnt);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("add params failed");
        HksFreeParamSet(&tmpParamSetOut);
        return ret;
    }

    ret = HksBuildParamSet(&tmpParamSetOut);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("build paramSet failed");
        HksFreeParamSet(&tmpParamSetOut);
        return ret;
    }

    if (memcpy_s(paramSetOut, paramSetOut->paramSetSize, tmpParamSetOut, tmpParamSetOut->paramSetSize) != EOK) {
        HksFreeParamSet(&tmpParamSetOut);
        HKS_LOG_E("memcpy paramSet out failed, paramSetOut size = %u", paramSetOut->paramSetSize);
        return HKS_ERROR_BAD_STATE;
    }

    paramSetOut->paramSetSize = tmpParamSetOut->paramSetSize;
    HksFreeParamSet(&tmpParamSetOut);
    return HksFreshParamSet(paramSetOut, false);
}

static int32_t FormatCurve25519Key(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    int32_t ret = CheckFormatCurve25519Key(keyIn, paramSetOut);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("check curve 25519 key failed");
        return ret;
    }

    struct KeyMaterial25519 *keyMaterial = (struct KeyMaterial25519 *)keyIn->data;
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA,
            .blob = { keyMaterial->pubKeySize, keyIn->data + sizeof(struct KeyMaterial25519) },
        },
        {
            .tag = HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA,
            .blob = { keyMaterial->priKeySize,
                keyIn->data + sizeof(struct KeyMaterial25519) + keyMaterial->pubKeySize },
        },
    };

    return BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
}

int32_t GetCurve25519FromKeyMaterial(const bool isPubKey, const struct HksBlob *keyMaterial,
    struct HksBlob *keyOut)
{
    int32_t ret = CheckCurve25519KeySize(keyMaterial);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    const struct KeyMaterial25519 *km = (struct KeyMaterial25519 *)(keyMaterial->data);

    uint32_t size = (isPubKey ? km->pubKeySize : km->priKeySize);
    if (size == 0) {
        HKS_LOG_E("get key material size invalid, pubSize = %u, priSize = %u", km->pubKeySize, km->priKeySize);
        return HKS_ERROR_INVALID_KEY_INFO;
    }
    uint8_t *buffer = (uint8_t *)HksMalloc(size);
    if (buffer == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    uint32_t offset = sizeof(struct KeyMaterial25519);
    uint8_t *tmp = (isPubKey ? (keyMaterial->data + offset) : (keyMaterial->data + offset + km->pubKeySize));
    if (memcpy_s(buffer, size, tmp, size) != EOK) {
        (void)memset_s(buffer, size, 0, size);
        HKS_FREE_PTR(buffer);
        return HKS_ERROR_BAD_STATE;
    }

    keyOut->data = buffer;
    keyOut->size = size;
    return HKS_SUCCESS;
}

#ifdef HKS_SUPPORT_AES_C
static int32_t FormatAesKey(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { keyIn->size, keyIn->data },
        },
    };
    return BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
}
#endif

#if defined(HKS_SUPPORT_RSA_C) && defined(HKS_SUPPORT_RSA_GENERATE_KEY)
static int32_t FormatRsaKey(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    if (keyIn->size < sizeof(struct KeyMaterialRsa)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct KeyMaterialRsa *keyMaterial = (struct KeyMaterialRsa *)keyIn->data;
    uint32_t publicKeySize = sizeof(struct KeyMaterialRsa) + keyMaterial->nSize + keyMaterial->eSize;
    if (keyIn->size < publicKeySize) {
        HKS_LOG_E("invalid key info.");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint8_t *publicKey = (uint8_t *)HksMalloc(publicKeySize);
    if (publicKey == NULL) {
        HKS_LOG_E("malloc public key failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(publicKey, publicKeySize, keyIn->data, publicKeySize);
    ((struct KeyMaterialRsa *)publicKey)->dSize = 0;

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA,
            .blob = { publicKeySize, publicKey },
        },
        {
            .tag = HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA,
            .blob = { keyIn->size, keyIn->data },
        },
    };
    int32_t ret = BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
    (void)memset_s(publicKey, publicKeySize, 0, publicKeySize);
    HksFree(publicKey);
    return ret;
}
#endif

#if defined(HKS_SUPPORT_HMAC_C) && defined(HKS_SUPPORT_HMAC_GENERATE_KEY)
static int32_t FormatHmacKey(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    struct HksParam params[] = {
        {
            .tag = HKS_TAG_SYMMETRIC_KEY_DATA,
            .blob = { keyIn->size, keyIn->data },
        },
    };
    return BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
}
#endif

#if defined(HKS_SUPPORT_DSA_C) && defined(HKS_SUPPORT_DSA_GENERATE_KEY)
static int32_t FormatDsaKey(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    if (keyIn->size < sizeof(struct KeyMaterialDsa)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct KeyMaterialDsa *keyMaterial = (struct KeyMaterialDsa *)keyIn->data;
    uint32_t publicKeySize = sizeof(struct KeyMaterialDsa) + keyMaterial->ySize + keyMaterial->pSize +
                             keyMaterial->qSize + keyMaterial->gSize;
    if (keyIn->size < publicKeySize) {
        HKS_LOG_E("invalid key info.");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint8_t *publicKey = (uint8_t *)HksMalloc(publicKeySize);
    if (publicKey == NULL) {
        HKS_LOG_E("malloc key failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (memcpy_s(publicKey, publicKeySize, keyIn->data, sizeof(struct KeyMaterialDsa)) != EOK) {
        HKS_FREE_PTR(publicKey);
        return HKS_ERROR_INVALID_OPERATION;
    }
    uint32_t inOffset = sizeof(struct KeyMaterialDsa);
    uint32_t outOffset = sizeof(struct KeyMaterialDsa) + keyMaterial->xSize;
    if (memcpy_s(publicKey + inOffset, publicKeySize - inOffset, keyIn->data + outOffset, publicKeySize - inOffset) !=
        EOK) {
        (void)memset_s(publicKey, publicKeySize, 0, publicKeySize);
        HKS_FREE_PTR(publicKey);
        return HKS_ERROR_INVALID_OPERATION;
    }
    ((struct KeyMaterialDsa *)publicKey)->xSize = 0;

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA,
            .blob = { publicKeySize, publicKey },
        },
        {
            .tag = HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA,
            .blob = { keyIn->size, keyIn->data },
        },
    };

    int32_t ret = BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
    (void)memset_s(publicKey, publicKeySize, 0, publicKeySize);
    HksFree(publicKey);
    return ret;
}
#endif

#if defined(HKS_SUPPORT_ECC_C) && defined(HKS_SUPPORT_ECC_GENERATE_KEY)
static int32_t FormatEccKey(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    if (keyIn->size < sizeof(struct KeyMaterialEcc)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct KeyMaterialEcc *keyMaterial = (struct KeyMaterialEcc *)keyIn->data;
    uint32_t publicKeySize = sizeof(struct KeyMaterialEcc) + keyMaterial->xSize + keyMaterial->ySize;
    if (keyIn->size < publicKeySize) {
        HKS_LOG_E("invalid key info.");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint8_t *publicKey = (uint8_t *)HksMalloc(publicKeySize);
    if (publicKey == NULL) {
        HKS_LOG_E("malloc public key failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(publicKey, publicKeySize, keyIn->data, publicKeySize);
    ((struct KeyMaterialEcc *)publicKey)->zSize = 0;

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA,
            .blob = { publicKeySize, publicKey },
        },
        {
            .tag = HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA,
            .blob = { keyIn->size, keyIn->data },
        },
    };
    int32_t ret = BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
    (void)memset_s(publicKey, publicKeySize, 0, publicKeySize);
    HksFree(publicKey);
    return ret;
}
#endif

#if defined(HKS_SUPPORT_DH_C) && defined(HKS_SUPPORT_DH_GENERATE_KEY)
static int32_t FormatDhKey(const struct HksBlob *keyIn, struct HksParamSet *paramSetOut)
{
    if (keyIn->size < sizeof(struct KeyMaterialDh)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct KeyMaterialDh *keyMaterial = (struct KeyMaterialDh *)keyIn->data;
    uint32_t publicKeySize = sizeof(struct KeyMaterialDh) + keyMaterial->pubKeySize;
    if (keyIn->size < publicKeySize) {
        HKS_LOG_E("invalid key info.");
        return HKS_ERROR_INVALID_KEY_INFO;
    }

    uint8_t *publicKey = (uint8_t *)HksMalloc(publicKeySize);
    if (publicKey == NULL) {
        HKS_LOG_E("malloc public key failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(publicKey, publicKeySize, keyIn->data, publicKeySize);
    ((struct KeyMaterialDh *)publicKey)->priKeySize = 0;

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ASYMMETRIC_PUBLIC_KEY_DATA,
            .blob = { publicKeySize, publicKey },
        },
        {
            .tag = HKS_TAG_ASYMMETRIC_PRIVATE_KEY_DATA,
            .blob = { keyIn->size, keyIn->data },
        },
    };
    int32_t ret = BuildParamSetOut(params, HKS_ARRAY_SIZE(params), paramSetOut);
    (void)memset_s(publicKey, publicKeySize, 0, publicKeySize);
    HksFree(publicKey);
    return ret;
}
#endif

int32_t HksSetKeyToMaterial(uint32_t alg, bool isPubKey, const struct HksBlob *key, struct HksBlob *keyMaterial)
{
    switch (alg) {
        case HKS_ALG_X25519:
        case HKS_ALG_ED25519:
            return SetCurve25519KeyMaterial(isPubKey, key, keyMaterial);
        case HKS_ALG_RSA:
        case HKS_ALG_DSA:
        case HKS_ALG_ECC:
        case HKS_ALG_ECDH:
        case HKS_ALG_DH:
            keyMaterial->size = key->size;
            keyMaterial->data = (uint8_t *)HksMalloc(keyMaterial->size);
            if (keyMaterial->data != NULL) {
                if (memcpy_s(keyMaterial->data, keyMaterial->size, key->data, key->size) != EOK) {
                    HKS_FREE_PTR(keyMaterial->data);
                    return HKS_ERROR_INVALID_OPERATION;
                }
                return HKS_SUCCESS;
            } else {
                return HKS_ERROR_MALLOC_FAIL;
            }
            break;
        default:
            HKS_LOG_E("alg not support");
            return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksGetKeyFromMaterial(uint32_t alg, bool isPubKey, const struct HksBlob *keyMaterial, struct HksBlob *key)
{
    switch (alg) {
        case HKS_ALG_X25519:
        case HKS_ALG_ED25519:
            return GetCurve25519FromKeyMaterial(isPubKey, keyMaterial, key);
        default:
            HKS_LOG_E("alg not support");
            return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksFormatKeyFromMaterial(uint32_t alg, const struct HksBlob *keyMaterial,
    struct HksParamSet *paramSetOut)
{
    switch (alg) {
        case HKS_ALG_X25519:
        case HKS_ALG_ED25519:
            return FormatCurve25519Key(keyMaterial, paramSetOut);
#if defined(HKS_SUPPORT_AES_C) && defined(HKS_SUPPORT_AES_GENERATE_KEY)
        case HKS_ALG_AES:
            return FormatAesKey(keyMaterial, paramSetOut);
#endif
#if defined(HKS_SUPPORT_RSA_C) && defined(HKS_SUPPORT_RSA_GENERATE_KEY)
        case HKS_ALG_RSA:
            return FormatRsaKey(keyMaterial, paramSetOut);
#endif
#if defined(HKS_SUPPORT_HMAC_C) && defined(HKS_SUPPORT_HMAC_GENERATE_KEY)
        case HKS_ALG_HMAC:
            return FormatHmacKey(keyMaterial, paramSetOut);
#endif
#if defined(HKS_SUPPORT_DSA_C) && defined(HKS_SUPPORT_DSA_GENERATE_KEY)
        case HKS_ALG_DSA:
            return FormatDsaKey(keyMaterial, paramSetOut);
#endif
#if defined(HKS_SUPPORT_ECC_C) && defined(HKS_SUPPORT_ECC_GENERATE_KEY)
        case HKS_ALG_ECC:
        case HKS_ALG_ECDH:
            return FormatEccKey(keyMaterial, paramSetOut);
#endif
#if defined(HKS_SUPPORT_DH_C) && defined(HKS_SUPPORT_DH_GENERATE_KEY)
        case HKS_ALG_DH:
            return FormatDhKey(keyMaterial, paramSetOut);
#endif
        default:
            HKS_LOG_E("alg not support");
            return HKS_ERROR_INVALID_ALGORITHM;
    }
}
#endif
