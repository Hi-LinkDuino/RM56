/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "hks_core_service_three_stage.h"

#include "hks_auth.h"
#include "hks_check_paramset.h"
#include "hks_client_service_adapter.h"
#include "hks_cmd_id.h"
#include "hks_common_check.h"
#include "hks_core_service.h"
#include "hks_crypto_adapter.h"
#include "hks_crypto_hal.h"
#include "hks_keyblob.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_type_inner.h"

#ifndef _HARDWARE_ROOT_KEY_
#include "hks_rkc.h"
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA
#include "hks_upgrade_key_info.h"
#endif

#ifndef _CUT_AUTHENTICATE_
#define CURVE25519_KEY_BYTE_SIZE HKS_KEY_BYTES(HKS_CURVE25519_KEY_SIZE_256)

#ifdef HKS_SUPPORT_ED25519_TO_X25519

#endif
#define HKS_RSA_OAEP_DIGEST_NUM     2
#define HKS_AES_CBC_BLOCK_SIZE      16
#define HKS_TEMP_SIZE               32
#define MAX_BUF_SIZE                (5 * 1024 * 1024)

static int32_t CheckRsaCipherData(bool isEncrypt, uint32_t keyLen, struct HksUsageSpec *usageSpec,
    const struct HksBlob *outData)
{
    uint32_t keySize = keyLen / HKS_BITS_PER_BYTE;
    uint32_t padding = usageSpec->padding;
    uint32_t digest = usageSpec->digest;

    if (padding == HKS_PADDING_NONE) {
        if (outData->size < keySize) {
            HKS_LOG_E("outData buffer too small size: %u, keySize: %u", outData->size, keySize);
            return HKS_ERROR_BUFFER_TOO_SMALL;
        }
    } else if (padding == HKS_PADDING_OAEP) {
        uint32_t digestLen;
        if (digest == HKS_DIGEST_NONE) {
            digest = HKS_DIGEST_SHA1;
        }
        int32_t ret = HksGetDigestLen(digest, &digestLen);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("GetDigestLen failed, ret = %x", ret);
            return ret;
        }
        if (keySize <= (HKS_RSA_OAEP_DIGEST_NUM * digestLen + HKS_RSA_OAEP_DIGEST_NUM)) {
            return HKS_ERROR_INVALID_KEY_FILE;
        }
        uint32_t size = keySize - HKS_RSA_OAEP_DIGEST_NUM * digestLen - HKS_RSA_OAEP_DIGEST_NUM;
        if (isEncrypt) {
            if (outData->size < keySize) {
                HKS_LOG_E("encrypt, outData buffer too small size: %u, keySize: %u", outData->size, keySize);
                return HKS_ERROR_BUFFER_TOO_SMALL;
            }
        } else {
            if (outData->size < size) {
                HKS_LOG_E("decrypt, outData buffer too small size: %u, keySize: %u", outData->size, keySize);
                return HKS_ERROR_BUFFER_TOO_SMALL;
            }
        }
    }

    return HKS_SUCCESS;
}

static int32_t CheckAesCipherAead(bool isEncrypt, const struct HksBlob *inData,
    const struct HksBlob *outData)
{
    if (isEncrypt) {
        if (outData->size < (inData->size + HKS_AE_TAG_LEN)) {
            HKS_LOG_E("encrypt, out buffer too small size: %u, inSize: %u", outData->size, inData->size);
            return HKS_ERROR_BUFFER_TOO_SMALL;
        }
    } else {
        if (outData->size < inData->size) {
            HKS_LOG_E("decryptfinal, out buffer too small size: %u, inSize: %u", outData->size, inData->size);
            return HKS_ERROR_BUFFER_TOO_SMALL;
        }
    }

    return HKS_SUCCESS;
}

static int32_t CheckAesCipherOther(bool isEncrypt, uint32_t padding, const struct HksBlob *inData,
    const struct HksBlob *outData)
{
    uint32_t paddingSize = 0;

    if (isEncrypt) {
        if (padding == HKS_PADDING_NONE) {
            if (inData->size % HKS_AES_CBC_BLOCK_SIZE != 0) {
                HKS_LOG_E("encrypt cbc no-padding, invalid inSize: %u", inData->size);
                return HKS_ERROR_INVALID_ARGUMENT;
            }
        } else {
            paddingSize = HKS_AES_CBC_BLOCK_SIZE - inData->size % HKS_AES_CBC_BLOCK_SIZE;
            if (inData->size > (UINT32_MAX - paddingSize)) {
                HKS_LOG_E("encrypt, invalid inData size: %u", inData->size);
                return HKS_ERROR_INVALID_ARGUMENT;
            }
        }
        if (outData->size < (inData->size + paddingSize)) {
            HKS_LOG_E("encrypt, outData buffer too small size: %u, need: %u",
                outData->size, inData->size + paddingSize);
            return HKS_ERROR_BUFFER_TOO_SMALL;
        }
    } else {
        if (outData->size < inData->size) {
            HKS_LOG_E("decrypt, outData buffer too small size: %u, inDataSize: %u", outData->size, inData->size);
            return HKS_ERROR_BUFFER_TOO_SMALL;
        }
    }

    return HKS_SUCCESS;
}

static int32_t CheckAesCipherData(bool isEncrypt, uint32_t padding, uint32_t mode,
    const struct HksBlob *inData, const struct HksBlob *outData)
{
    int32_t ret = HKS_FAILURE;
    if ((mode == HKS_MODE_CBC) || (mode == HKS_MODE_CTR) || (mode == HKS_MODE_ECB)) {
        ret = CheckAesCipherOther(isEncrypt, padding, inData, outData);
    } else if ((mode == HKS_MODE_GCM) || (mode == HKS_MODE_CCM)) {
        ret = CheckAesCipherAead(isEncrypt, inData, outData);
    }

    return ret;
}

static int32_t HksCheckFinishOutSize(bool isEncrypt, struct HksParamSet *paramSet,
    const struct HksBlob *inData, const struct HksBlob *outData)
{
    struct HksUsageSpec usageSpec = {0};
    HksFillUsageSpec(paramSet, &usageSpec);
    struct HksKeySpec cihperSpec = {0};
    HksFillKeySpec(paramSet, &cihperSpec);
    uint32_t alg = usageSpec.algType;

    switch (alg) {
        case HKS_ALG_RSA:
            return CheckRsaCipherData(isEncrypt, cihperSpec.keyLen, &usageSpec, outData);
        case HKS_ALG_AES:
            return CheckAesCipherData(isEncrypt, usageSpec.padding, usageSpec.mode, inData, outData);
        default:
            return HKS_ERROR_INVALID_ALGORITHM;
    }
}

static int32_t GetHksInnerKeyFormat(const struct HksParamSet *paramSet, const struct HksBlob *key,
    struct HksBlob *outKey)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get alg param failed");
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    switch (algParam->uint32Param) {
#ifdef HKS_SUPPORT_AES_C
        case HKS_ALG_AES:
            return TranslateToInnerAesFormat(key, outKey);
#endif
#if defined(HKS_SUPPORT_X25519_C) || defined(HKS_SUPPORT_ED25519_C)
        case HKS_ALG_ED25519:
        case HKS_ALG_X25519:
            return TranslateToInnerCurve25519Format(algParam->uint32Param, key, outKey);
#endif
#if defined(HKS_SUPPORT_RSA_C) || defined(HKS_SUPPORT_ECC_C) || defined(HKS_SUPPORT_DSA_C) || \
    defined(HKS_SUPPORT_DH_C)
        case HKS_ALG_RSA:
        case HKS_ALG_ECC:
        case HKS_ALG_ECDH:
        case HKS_ALG_DSA:
        case HKS_ALG_DH:
            return TranslateFromX509PublicKey(key, outKey);
#endif
        default:
            return HKS_ERROR_INVALID_ALGORITHM;
    }
}

static int32_t SignVerifyAuth(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get alg param failed!");
        return ret;
    }

    if (algParam->uint32Param == HKS_ALG_RSA) {
        return HksThreeStageAuth(HKS_AUTH_ID_SIGN_VERIFY_RSA, keyNode);
    } else if (algParam->uint32Param == HKS_ALG_ECC) {
        return HksThreeStageAuth(HKS_AUTH_ID_SIGN_VERIFY_ECC, keyNode);
    } else if (algParam->uint32Param == HKS_ALG_DSA) {
        return HKS_SUCCESS;
    } else if (algParam->uint32Param == HKS_ALG_ED25519) {
        return HKS_SUCCESS;
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

static int32_t CipherAuth(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet)
{
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get alg param failed!");
        return ret;
    }

    if (algParam->uint32Param == HKS_ALG_AES) {
        return HksThreeStageAuth(HKS_AUTH_ID_SYM_CIPHER, keyNode);
    } else if (algParam->uint32Param == HKS_ALG_RSA) {
        return HksThreeStageAuth(HKS_AUTH_ID_ASYM_CIPHER, keyNode);
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

static void *GetCryptoCtx(const struct HuksKeyNode *keyNode)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);

    return (ret != HKS_SUCCESS || ctxParam == NULL) ? NULL : (void *)(uintptr_t)ctxParam->uint64Param;
}

static void ClearCryptoCtx(const struct HuksKeyNode *keyNode)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS || ctxParam == NULL) {
        return;
    }

    ctxParam->uint64Param = (uint64_t)(uintptr_t)NULL;
    return;
}

static int32_t SetCacheModeCtx(const struct HuksKeyNode *keyNode)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob *tempData = (struct HksBlob *)HksMalloc(sizeof(struct HksBlob));
    if (tempData == NULL) {
        HKS_LOG_E("get cache mode ctx malloc fail.");
        return HKS_ERROR_MALLOC_FAIL;
    }
    tempData->size = 0;
    tempData->data = NULL;
    ctxParam->uint64Param = (uint64_t)(uintptr_t)tempData;
    return HKS_SUCCESS;
}

static int32_t CopyNewCachedData(const struct HksBlob *cachedBlob, const struct HksBlob *inData,
    uint8_t *newData, uint32_t newSize)
{
    if (cachedBlob->size != 0) {
        if (memcpy_s(newData, newSize, cachedBlob->data, cachedBlob->size) != EOK) {
            HKS_LOG_E("memcpy cached data failed");
            return HKS_ERROR_BAD_STATE;
        }
    }
    if (inData->size != 0) {
        if (memcpy_s(newData + cachedBlob->size, newSize - cachedBlob->size, inData->data, inData->size) != EOK) {
            HKS_LOG_E("memcpy in data failed");
            return HKS_ERROR_BAD_STATE;
        }
    }
    return HKS_SUCCESS;
}

static int32_t GetNewCachedData(const struct HksBlob *cachedBlob, const struct HksBlob *inData,
    struct HksBlob *newBlob)
{
    if ((cachedBlob->size > MAX_BUF_SIZE) || (inData->size > (MAX_BUF_SIZE - cachedBlob->size))) {
        HKS_LOG_E("input data size too large, size = %u", inData->size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint32_t newSize = cachedBlob->size + inData->size;
    uint8_t *newData = (uint8_t *)HksMalloc(newSize);
    if (newData == NULL) {
        HKS_LOG_E("update cache data malloc fail.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    int32_t ret = CopyNewCachedData(cachedBlob, inData, newData, newSize);
    if (ret != HKS_SUCCESS) {
        HKS_FREE_PTR(newData);
        return ret;
    }

    newBlob->data = newData;
    newBlob->size = newSize;
    return HKS_SUCCESS;
}

static int32_t UpdateCachedData(const struct HuksKeyNode *keyNode, const struct HksBlob *srcData)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob *cachedData = (struct HksBlob *)ctx;
    struct HksBlob *newCachedBlob = (struct HksBlob *)HksMalloc(sizeof(struct HksBlob));
    if (newCachedBlob == NULL) {
        HKS_LOG_E("malloc new blob failed");
        return HKS_ERROR_MALLOC_FAIL;
    }
    ret = GetNewCachedData(cachedData, srcData, newCachedBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get mew cached data failed, ret = %d", ret);
        HKS_FREE_PTR(newCachedBlob);
        return ret;
    }

    HKS_FREE_PTR(cachedData->data);
    HKS_FREE_PTR(cachedData);
    ctxParam->uint64Param = (uint64_t)(uintptr_t)newCachedBlob;
    return HKS_SUCCESS;
}

static void FreeCachedData(struct HksBlob **cachedData)
{
    if ((cachedData == NULL) || (*cachedData == NULL)) {
        return;
    }
    if ((*cachedData)->data != NULL) {
        (void)memset_s((*cachedData)->data, (*cachedData)->size, 0, (*cachedData)->size);
        HKS_FREE_PTR((*cachedData)->data);
    }
    HKS_FREE_PTR(*cachedData);
}

static int32_t FinishCachedData(const struct HuksKeyNode *keyNode, const struct HksBlob *srcData,
    struct HksBlob *outData)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob *cachedData = (struct HksBlob *)ctx;
    ret = GetNewCachedData(cachedData, srcData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get new cached data failed, ret = %d", ret);
    }

    FreeCachedData(&cachedData);
    ctxParam->uint64Param = 0; /* clear ctx to NULL */
    return ret;
}

static int32_t CoreHashInit(const struct HuksKeyNode *keyNode, uint32_t alg)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }

    void *ctx = NULL;
    uint32_t digest = (alg == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : alg;
    ret = HksCryptoHalHashInit(digest, &ctx);
    if (ret != HKS_SUCCESS)  {
        HKS_LOG_E("hal hash init failed ret : %d", ret);
        return ret;
    }
    ctxParam->uint64Param = (uint64_t)(uintptr_t)ctx;
    return HKS_SUCCESS;
}

static int32_t CoreHashUpdate(const struct HuksKeyNode *keyNode, const struct HksBlob *srcData)
{
    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_BAD_STATE;
    }

    int32_t ret = HksCryptoHalHashUpdate(srcData, ctx);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hal hash update failed ret : %d", ret);
    }
    return ret;
}

static int32_t CoreHashFinish(const struct HuksKeyNode *keyNode, const struct HksBlob *srcData,
    struct HksBlob *outData)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }

    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_BAD_STATE;
    }

    outData->size = MAX_HASH_SIZE;
    outData->data = (uint8_t *)HksMalloc(MAX_HASH_SIZE);
    if (outData->data == NULL) {
        HKS_LOG_E("malloc fail.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    ret = HksCryptoHalHashFinal(srcData, &ctx, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hal hash final failed ret : %d", ret);
        HKS_FREE_BLOB(*outData);
    }

    ctxParam->uint64Param = 0; /* clear ctx to NULL */
    return ret;
}

static int32_t CoreSignVerify(const struct HuksKeyNode *keyNode, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    struct HksBlob rawKey = { 0, NULL };
    int32_t ret = HksGetRawKey(keyNode->keyBlobParamSet, &rawKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerify get raw key failed!");
        return ret;
    }

    struct HksUsageSpec usageSpec;
    (void)memset_s(&usageSpec, sizeof(struct HksUsageSpec), 0, sizeof(struct HksUsageSpec));
    HksFillUsageSpec(keyNode->runtimeParamSet, &usageSpec);

    /* NONEwithECDSA/RSA default sha256 */
    usageSpec.digest = (usageSpec.digest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : usageSpec.digest;

    if (usageSpec.purpose == HKS_KEY_PURPOSE_SIGN) {
        ret = HksCryptoHalSign(&rawKey, &usageSpec, inData, outData);
    } else {
        ret = HksCryptoHalVerify(&rawKey, &usageSpec, inData, outData);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerify Finish failed, purpose = 0x%x, ret = %d", usageSpec.purpose, ret);
    }

    (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    HKS_FREE_PTR(rawKey.data);
    return ret;
}

static void FreeSignVerifyCtx(const struct HuksKeyNode *keyNode)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        return;
    }

    struct HksParam *algParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get alg param failed!");
        return;
    }

    if (algParam->uint32Param != HKS_ALG_ED25519) {
        HksCryptoHalHashFreeCtx(&ctx);
    } else {
        struct HksBlob *cachedData = (struct HksBlob *)ctx;
        FreeCachedData(&cachedData);
    }

    ctxParam->uint64Param = 0; /* clear ctx to NULL */
}

static int32_t CoreCipherInit(const struct HuksKeyNode *keyNode)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksParam *purposeParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_PURPOSE, &purposeParam);
    if (ret != HKS_SUCCESS) {
        return HKS_ERROR_CHECK_GET_PURPOSE_FAIL;
    }

    struct HksBlob rawKey = { 0, NULL };
    struct HksUsageSpec *usageSpec = NULL;
    do {
        uint8_t tmpData[HKS_TEMP_SIZE] = {0};
        struct HksBlob tmpInData = { HKS_TEMP_SIZE, tmpData };
        bool isEncrypt = (purposeParam->uint32Param == HKS_KEY_PURPOSE_ENCRYPT) ? true : false;
        ret = HksBuildCipherUsageSpec(keyNode->runtimeParamSet, isEncrypt, &tmpInData, &usageSpec);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("build cipher usage failed");
            break;
        }

        ret = HksGetRawKey(keyNode->keyBlobParamSet, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("cipher get raw key failed");
            break;
        }

        void *ctx = NULL;
        if (purposeParam->uint32Param == HKS_KEY_PURPOSE_ENCRYPT) {
            ret = HksCryptoHalEncryptInit(&rawKey, usageSpec, &ctx);
        } else {
            ret = HksCryptoHalDecryptInit(&rawKey, usageSpec, &ctx);
        }
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("cipher ctx init failed, ret = %d", ret);
            break;
        }
        ctxParam->uint64Param = (uint64_t)(uintptr_t)ctx;
    } while (0);

    if (rawKey.data != NULL) {
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    }
    HKS_FREE_PTR(rawKey.data);
    HksFreeUsageSpec(&usageSpec);
    return ret;
}

static int32_t CoreCipherUpdate(const struct HuksKeyNode *keyNode, const struct HksBlob *inData,
    struct HksBlob *outData, uint32_t alg)
{
    if (outData->size < inData->size) {
        HKS_LOG_E("cipher update, out buffer too small size: %u, inSize: %u", outData->size, inData->size);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksParam *purposeParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_PURPOSE, &purposeParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("append cipher get purpose param failed!");
        return ret;
    }

    if (purposeParam->uint32Param == HKS_KEY_PURPOSE_ENCRYPT) {
        ret = HksCryptoHalEncryptUpdate(inData, ctx, outData, alg);
    } else {
        ret = HksCryptoHalDecryptUpdate(inData, ctx, outData, alg);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher update failed! ret : %d", ret);
    }
    return ret;
}

static int32_t CoreAesEncryptFinish(const struct HuksKeyNode *keyNode,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    struct HksBlob tag = { 0, NULL };
    int32_t ret = HksGetEncryptAeTag(keyNode->runtimeParamSet, inData, outData, &tag);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher encrypt get ae tag failed!");
        return ret;
    }

    ret = HksCheckFinishOutSize(true, keyNode->runtimeParamSet, inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("aes encrypt finish check data size failed");
        return ret;
    }

    struct HksParam *ctxParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return ret;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_NULL_POINTER;
    }

    ret = HksCryptoHalEncryptFinal(inData, &ctx, outData, &tag, alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("aes encrypt Finish failed! ret : %d", ret);
        ctxParam->uint64Param = 0; /* clear ctx to NULL */
        return ret;
    }

    outData->size += tag.size;
    ctxParam->uint64Param = 0; /* clear ctx to NULL */
    return HKS_SUCCESS;
}

static int32_t CoreAesDecryptFinish(const struct HuksKeyNode *keyNode,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    bool isAes = false;
    bool isAeMode = false;
    int32_t ret = HksCheckAesAeMode(keyNode->runtimeParamSet, &isAes, &isAeMode);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get aeMode failed!");
        return ret;
    }

    struct HksBlob tag = { 0, NULL };
    if (isAes && isAeMode) {
        struct HksParam *tagParam = NULL;
        ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_AE_TAG, &tagParam);
        if (ret != HKS_SUCCESS || tagParam == NULL) {
            HKS_LOG_E("get tag failed!");
            return ret;
        }
        tag = tagParam->blob;
    }

    ret = HksCheckFinishOutSize(false, keyNode->runtimeParamSet, inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("aes decrypt finish check data size failed");
        return ret;
    }

    struct HksParam *ctxParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return ret;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        HKS_LOG_E("ctx is invalid: null!");
        return HKS_ERROR_NULL_POINTER;
    }

    ret = HksCryptoHalDecryptFinal(inData, &ctx, outData, &tag, alg);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher DecryptFinish failed! ret : %d", ret);
    }
    ctxParam->uint64Param = 0; /* clear ctx to NULL */
    return ret;
}

static int32_t RsaCipherFinish(const struct HuksKeyNode *keyNode, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    HKS_LOG_E("rsa inData.size = %u", inData->size);
    struct HksBlob rawKey = { 0, NULL };
    int32_t ret = HksGetRawKey(keyNode->keyBlobParamSet, &rawKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerify get raw key failed!");
        return ret;
    }

    struct HksUsageSpec usageSpec;
    (void)memset_s(&usageSpec, sizeof(struct HksUsageSpec), 0, sizeof(struct HksUsageSpec));
    HksFillUsageSpec(keyNode->runtimeParamSet, &usageSpec);
    /* NONEwithECDSA/RSA default sha256 */
    usageSpec.digest = (usageSpec.digest == HKS_DIGEST_NONE) ? HKS_DIGEST_SHA256 : usageSpec.digest;

    bool isEncrypt = (usageSpec.purpose == HKS_KEY_PURPOSE_ENCRYPT) ? true : false;
    ret = HksCheckFinishOutSize(isEncrypt, keyNode->runtimeParamSet, inData, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("rsa cipher finish check data size failed");
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
        HKS_FREE_PTR(rawKey.data);
        return ret;
    }

    if (usageSpec.purpose == HKS_KEY_PURPOSE_ENCRYPT) {
        struct HksBlob tag = { 0, NULL };
        ret = HksCryptoHalEncrypt(&rawKey, &usageSpec, inData, outData, &tag);
    } else {
        ret = HksCryptoHalDecrypt(&rawKey, &usageSpec, inData, outData);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("rsa cipher Finish failed, purpose = 0x%x, ret = %d", usageSpec.purpose, ret);
    }

    (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    HKS_FREE_PTR(rawKey.data);
    return ret;
}

static int32_t CoreRsaCipherFinish(const struct HuksKeyNode *keyNode, const struct HksBlob *inData,
    struct HksBlob *outData)
{
    struct HksBlob tempInData = { 0, NULL };
    int32_t ret = FinishCachedData(keyNode, inData, &tempInData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get rsa cipher cached data faile");
        return ret;
    }

    ret = RsaCipherFinish(keyNode, &tempInData, outData);
    HKS_FREE_BLOB(tempInData);
    return ret;
}

static void FreeCryptoCtx(const struct HuksKeyNode *keyNode, uint32_t alg)
{
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        return;
    }

    if (alg == HKS_ALG_AES) {
        HksCryptoHalEncryptFreeCtx(&ctx, alg);
    } else {
        struct HksBlob *cachedData = (struct HksBlob *)ctx;
        FreeCachedData(&cachedData);
    }
    ctxParam->uint64Param = 0; /* clear ctx to NULL */
}

static int32_t GetRawkey(const struct HuksKeyNode *keyNode, struct HksBlob *rawKey)
{
    if (GetCryptoCtx(keyNode) != NULL) {
        HKS_LOG_E("avoid running into this function multiple times!");
        return HKS_FAILURE;
    }

    int32_t ret = HksThreeStageAuth(HKS_AUTH_ID_DERIVE, keyNode);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("derive auth failed!");
        return ret;
    }

    ret = HksGetRawKey(keyNode->keyBlobParamSet, rawKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("Derive get raw key failed!");
    }

    return ret;
}

static void FreeOutBlob(struct HksBlob **out)
{
    if ((out == NULL) || (*out == NULL)) {
        return;
    }
    HKS_FREE_PTR((*out)->data);
    HKS_FREE_PTR(*out);
}

static int32_t ConstructDervieBlob(const struct HksParamSet *paramSet, struct HksBlob **out)
{
    struct HksParam *deriveSizeParam = NULL;
    int32_t ret = HksGetParam(paramSet, HKS_TAG_DERIVE_KEY_SIZE, &deriveSizeParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get derive size failed, ret = %d", ret);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint32_t deriveSize = deriveSizeParam->uint32Param;
    if ((deriveSize == 0) || (deriveSize > MAX_OUT_BLOB_SIZE)) {
        HKS_LOG_E("derive size invalid, size = %u", deriveSize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct HksBlob *tempOut = (struct HksBlob *)HksMalloc(sizeof(struct HksBlob));
    if (tempOut == NULL) {
        HKS_LOG_E("construct derive blob malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    tempOut->data = (uint8_t *)HksMalloc(deriveSize);
    if (tempOut->data == NULL) {
        HKS_FREE_PTR(tempOut);
        HKS_LOG_E("malloc out derive blob data failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }
    tempOut->size = deriveSize;
    *out = tempOut;
    return HKS_SUCCESS;
}

static int32_t ConstructAgreeBlob(struct HksBlob **agreeOut)
{
    struct HksBlob *agreeTemp = (struct HksBlob *)HksMalloc(sizeof(struct HksBlob));
    if (agreeTemp == NULL) {
        HKS_LOG_E("malloc agreeTemp failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    agreeTemp->size = MAX_KEY_SIZE;
    agreeTemp->data = (uint8_t *)HksMalloc(MAX_KEY_SIZE);
    if (agreeTemp->data == NULL) {
        HKS_LOG_E("malloc agreeTemp data failed.");
        HKS_FREE_PTR(agreeTemp);
        return HKS_ERROR_MALLOC_FAIL;
    }
    *agreeOut = agreeTemp;
    return HKS_SUCCESS;
}


int32_t HksCoreSignVerifyThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)paramSet;

    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param == HKS_ALG_ED25519) {
        return SetCacheModeCtx(keyNode);
    } else {
        return CoreHashInit(keyNode, alg);
    }
}

int32_t HksCoreSignVerifyThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg)
{
    (void)signature;
    (void)alg;
    (void)paramSet;

    int32_t ret = SignVerifyAuth(keyNode, keyNode->runtimeParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("HksCoreSignVerifyThreeStageUpdate SignAuth fail ret : %d", ret);
        return ret;
    }

    struct HksParam *algParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param == HKS_ALG_ED25519) {
        return UpdateCachedData(keyNode, srcData);
    } else {
        return CoreHashUpdate(keyNode, srcData);
    }
}

int32_t HksCoreSignVerifyThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    (void)paramSet;
    (void)alg;

    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    struct HksBlob signVerifyData = { 0, NULL };
    if (algParam->uint32Param == HKS_ALG_ED25519) {
        ret = FinishCachedData(keyNode, inData, &signVerifyData);
    } else {
        ret = CoreHashFinish(keyNode, inData, &signVerifyData);
    }
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("signVerify Finish get Data failed, ret = %d", ret);
        return ret;
    }

    ret = CoreSignVerify(keyNode, &signVerifyData, outData);
    HKS_FREE_BLOB(signVerifyData);
    return ret;
}

int32_t HksCoreSignVerifyThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)paramSet;
    (void)alg;

    FreeSignVerifyCtx(keyNode);
    return HKS_SUCCESS;
}

int32_t HksCoreCryptoThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)alg;

    int32_t ret = CipherAuth(keyNode, paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("cipher init failed, ret = %d", ret);
        return ret;
    }

    struct HksParam *algParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param == HKS_ALG_RSA) {
        return SetCacheModeCtx(keyNode);
    } else if (algParam->uint32Param == HKS_ALG_AES) {
        return CoreCipherInit(keyNode);
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksCoreCryptoThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    int32_t ret = CipherAuth(keyNode, paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    struct HksParam *algParam = NULL;
    ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param == HKS_ALG_RSA) {
        return UpdateCachedData(keyNode, inData);
    } else if (algParam->uint32Param == HKS_ALG_AES) {
        return CoreCipherUpdate(keyNode, inData, outData, alg);
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksCoreEncryptThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    (void)paramSet;

    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param == HKS_ALG_RSA) {
        return CoreRsaCipherFinish(keyNode, inData, outData);
    } else if (algParam->uint32Param == HKS_ALG_AES) {
        return CoreAesEncryptFinish(keyNode, inData, outData, alg);
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksCoreDecryptThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    (void)paramSet;
    struct HksParam *algParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_ALGORITHM, &algParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get param get 0x%x failed", HKS_TAG_ALGORITHM);
        return HKS_ERROR_CHECK_GET_ALG_FAIL;
    }

    if (algParam->uint32Param == HKS_ALG_RSA) {
        return CoreRsaCipherFinish(keyNode, inData, outData);
    } else if (algParam->uint32Param == HKS_ALG_AES) {
        return CoreAesDecryptFinish(keyNode, inData, outData, alg);
    } else {
        return HKS_ERROR_INVALID_ALGORITHM;
    }
}

int32_t HksCoreCryptoThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)paramSet;
    FreeCryptoCtx(keyNode, alg);
    return HKS_SUCCESS;
}

int32_t HksCoreDeriveThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)keyNode;
    (void)paramSet;
    (void)alg;

    HKS_LOG_D("HksCoreDeriveThreeStageInit start");
    return HKS_SUCCESS;
}

int32_t HksCoreDeriveThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *derive, uint32_t alg)
{
    (void)srcData;
    (void)alg;
    (void)derive;
    (void)paramSet;
    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob rawKey = { 0, NULL };
    do {
        ret = GetRawkey(keyNode, &rawKey);
        if (ret != HKS_SUCCESS) {
            break;
        }

        struct HksBlob *deriveBlob = NULL;
        ret = ConstructDervieBlob(keyNode->runtimeParamSet, &deriveBlob);
        if (ret != HKS_SUCCESS) {
            break;
        }

        struct HksKeyDerivationParam derParam = { { 0, NULL }, { 0, NULL }, 0, 0 };
        struct HksKeySpec derivationSpec = { 0, 0, &derParam };
        HksFillKeySpec(keyNode->runtimeParamSet, &derivationSpec);
        HksFillKeyDerivationParam(keyNode->runtimeParamSet, &derParam);
        ret = HksCryptoHalDeriveKey(&rawKey, &derivationSpec, deriveBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("HksCryptoHalDeriveKey fail");
            FreeOutBlob(&deriveBlob);
            break;
        }

        ctxParam->uint64Param = (uint64_t)(uintptr_t)deriveBlob;
    } while (0);

    if (rawKey.data != NULL) {
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    }
    HKS_FREE_PTR(rawKey.data);

    return ret;
}

int32_t HksCoreDeriveThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    HKS_LOG_D("HksCoreDeriveThreeStageFinish start");
    (void)inData;
    (void)alg;
    int32_t ret;

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksBlob *restoreData = (struct HksBlob *)ctx;
    if (restoreData == NULL) {
        HKS_LOG_E("derive restoreData is NULL!");
        return HKS_ERROR_NULL_POINTER;
    }

    bool needStore = false;
    struct HksParam *storage = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_KEY_STORAGE_FLAG, &storage);
    if (ret == HKS_SUCCESS) {
        if (storage != NULL && storage->uint32Param == HKS_STORAGE_PERSISTENT) {
            needStore = true;
        }
    }

    do {
        if (needStore) {
            ret = HksBuildKeyBlob(NULL, HKS_KEY_FLAG_DERIVE_KEY, restoreData, paramSet, outData);
            if (ret != HKS_SUCCESS) {
                HKS_LOG_E("HksBuildKeyBlob failed! ret: %d", ret);
                break;
            }
        } else {
            if (outData->size < restoreData->size) {
                HKS_LOG_E("outData size is too small, size : %u", outData->size);
                ret = HKS_ERROR_BUFFER_TOO_SMALL;
                break;
            }
            outData->size = restoreData->size;
            (void)memcpy_s(outData->data, outData->size, restoreData->data, outData->size);
            ret = HKS_SUCCESS;
        }
    } while (0);

    FreeCachedData(&restoreData);
    ClearCryptoCtx(keyNode);
    return ret;
}

int32_t HksCoreDeriveThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)paramSet;
    (void)alg;

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksBlob *restoreData = (struct HksBlob *)ctx;
    if (restoreData == NULL) {
        HKS_LOG_E("restoreData is NULL");
        return HKS_FAILURE;
    }

    FreeCachedData(&restoreData);
    ClearCryptoCtx(keyNode);
    return HKS_SUCCESS;
}

int32_t HksCoreAgreeThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)keyNode;
    (void)paramSet;
    (void)alg;

    return HKS_SUCCESS;
}

int32_t HksCoreAgreeThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *signature, uint32_t alg)
{
    (void)signature;
    (void)paramSet;
    (void)alg;

    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }
    if (ctxParam->uint64Param != 0) {
        HKS_LOG_E("avoid running into this function multiple times!");
        return HKS_FAILURE;
    }

    struct HksBlob rawKey = { 0, NULL };
    struct HksBlob publicKey = { 0, NULL };

    do {
        ret = GetHksInnerKeyFormat(keyNode->runtimeParamSet, srcData, &publicKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("get public key from x509 format failed, ret = %d.", ret);
            break;
        }

        struct HksBlob *agreeTemp = NULL;
        ret = ConstructAgreeBlob(&agreeTemp);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("HksCoreAgreeBuildData failed, ret = %d.", ret);
            break;
        }

        ret = HksGetRawKey(keyNode->keyBlobParamSet, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("agree get raw key failed!");
            FreeOutBlob(&agreeTemp);
            break;
        }

        struct HksKeySpec agreeSpec = { 0 };
        HksFillKeySpec(keyNode->runtimeParamSet, &agreeSpec);
        ret = HksCryptoHalAgreeKey(&rawKey, &publicKey, &agreeSpec, agreeTemp);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("HksCryptoHalAgreeKey failed, ret = %d.", ret);
            FreeOutBlob(&agreeTemp);
            break;
        }

        ctxParam->uint64Param = (uint64_t)(uintptr_t)agreeTemp;
    } while (0);

    if (rawKey.data != NULL) {
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    }
    HKS_FREE_PTR(rawKey.data);
    HKS_FREE_PTR(publicKey.data);
    return ret;
}

int32_t HksCoreAgreeThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    (void)inData;
    (void)alg;
    int32_t ret;

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksBlob *restoreData = (struct HksBlob *)ctx;
    if (restoreData == NULL) {
        HKS_LOG_E("agree restoreData is NULL!");
        return HKS_FAILURE;
    }

    bool needStore = false;
    struct HksParam *storage = NULL;
    ret = HksGetParam(paramSet, HKS_TAG_KEY_STORAGE_FLAG, &storage);
    if (ret == HKS_SUCCESS) {
        if (storage != NULL && storage->uint32Param == HKS_STORAGE_PERSISTENT) {
            needStore = true;
        }
    }

    do {
        if (needStore) {
            ret = HksBuildKeyBlob(NULL, HKS_KEY_FLAG_AGREE_KEY, restoreData, paramSet, outData);
            if (ret != HKS_SUCCESS) {
                HKS_LOG_E("HksBuildKeyBlob failed! ret: %d", ret);
                break;
            }
        } else {
            if (outData->size < restoreData->size) {
                HKS_LOG_E("outData size is too small, size : %u", outData->size);
                ret = HKS_ERROR_BUFFER_TOO_SMALL;
                break;
            }
            outData->size = restoreData->size;
            (void)memcpy_s(outData->data, outData->size, restoreData->data, outData->size);
            ret =  HKS_SUCCESS;
        }
    } while (0);

    FreeCachedData(&restoreData);
    ClearCryptoCtx(keyNode);
    return ret;
}

int32_t HksCoreAgreeThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet, uint32_t alg)
{
    (void)paramSet;
    (void)alg;

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    struct HksBlob *restoreData = (struct HksBlob *)ctx;
    if (restoreData == NULL) {
        HKS_LOG_E("restoreData is NULL");
        return HKS_FAILURE;
    }

    FreeCachedData(&restoreData);
    ClearCryptoCtx(keyNode);
    return HKS_SUCCESS;
}

int32_t HksCoreMacThreeStageInit(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    uint32_t alg)
{
    (void)paramSet;

    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }

    struct HksBlob rawKey = { 0, NULL };
    do {
        ret = HksGetRawKey(keyNode->keyBlobParamSet, &rawKey);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("Derive get raw key failed!");
            return ret;
        }

        void *ctx = NULL;
        ret = HksCryptoHalHmacInit(&rawKey, alg, &ctx);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hmac init failed! ret : %d", ret);
            break;
        }

        ctxParam->uint64Param = (uint64_t)(uintptr_t)ctx;
    } while (0);

    if (rawKey.data != NULL) {
        (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    }
    HKS_FREE_PTR(rawKey.data);

    return ret;
}

int32_t HksCoreMacThreeStageUpdate(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *srcData, struct HksBlob *mac, uint32_t alg)
{
    (void)paramSet;
    (void)mac;
    (void)alg;
    int32_t ret;

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    ret = HksCryptoHalHmacUpdate(srcData, ctx);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hmac update failed! ret : %d", ret);
        return ret;
    }

    return HKS_SUCCESS;
}

int32_t HksCoreMacThreeStageFinish(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet,
    const struct HksBlob *inData, struct HksBlob *outData, uint32_t alg)
{
    (void)paramSet;
    (void)alg;

    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(keyNode->runtimeParamSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get ctx from keyNode failed!");
        return HKS_ERROR_BAD_STATE;
    }
    void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
    if (ctx == NULL) {
        HKS_LOG_E("ctx invalid");
        return HKS_ERROR_NULL_POINTER;
    }

    ret = HksCryptoHalHmacFinal(inData, &ctx, outData);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("hmac final failed! ret : %d", ret);
    }
    ctxParam->uint64Param = 0; /* clear ctx to NULL */
    return ret;
}

int32_t HksCoreMacThreeStageAbort(const struct HuksKeyNode *keyNode, const struct HksParamSet *paramSet, uint32_t alg)
{
    (void)alg;
    (void)paramSet;

    void *ctx = GetCryptoCtx(keyNode);
    if (ctx == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    HksCryptoHalHmacFreeCtx(&ctx);
    ClearCryptoCtx(keyNode);

    return HKS_SUCCESS;
}

#endif /* _CUT_AUTHENTICATE_ */
