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

#ifndef _CUT_AUTHENTICATE_

#include "hks_keynode.h"

#include "hks_core_service.h"
#include "hks_crypto_hal.h"
#include "hks_keyblob.h"
#include "hks_log.h"
#include "hks_mem.h"

static struct DoubleList g_keyNodeList = { NULL, NULL };

struct DoubleList *GetKeyNodeList(void)
{
    return &g_keyNodeList;
}

static int32_t BuildRuntimeParamSet(const struct HksParamSet *inParamSet, struct HksParamSet **outParamSet)
{
    struct HksParamSet *paramSet = NULL;
    int32_t ret = HksInitParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("init keyNode param set fail");
        return ret;
    }

    if (inParamSet != NULL) {
        ret = HksAddParams(paramSet, inParamSet->params, inParamSet->paramsCnt);
        if (ret != HKS_SUCCESS) {
            HksFreeParamSet(&paramSet);
            HKS_LOG_E("add in params fail");
            return ret;
        }
    }

    struct HksParam params[] = {
        {
            .tag = HKS_TAG_ACCESS_TIME,
            .uint32Param = 0
        }, {
            .tag = HKS_TAG_USES_TIME,
            .uint32Param = 0
        }, {
            .tag = HKS_TAG_CRYPTO_CTX,
            .uint64Param = 0
        },
    };

    ret = HksAddParams(paramSet, params, sizeof(params) / sizeof(params[0]));
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&paramSet);
        HKS_LOG_E("add runtime params fail");
        return ret;
    }

    ret = HksBuildParamSet(&paramSet);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&paramSet);
        HKS_LOG_E("build paramSet fail");
        return ret;
    }

    *outParamSet = paramSet;
    return HKS_SUCCESS;
}

static int32_t GenerateKeyNodeHandle(uint64_t *handle)
{
    uint64_t handleData = 0;
    struct HksBlob opHandle = {
        .size = sizeof(uint64_t),
        .data = (uint8_t *)&handleData
    };

    int32_t ret = HksCryptoHalFillRandom(&opHandle);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("fill keyNode handle failed");
        return ret;
    }

    if (memcpy_s(handle, sizeof(*handle), opHandle.data, opHandle.size) != EOK) {
        HKS_LOG_E("memcpy handle failed");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    return HKS_SUCCESS;
}

#ifdef _STORAGE_LITE_
struct HuksKeyNode *HksCreateKeyNode(const struct HksBlob *key, const struct HksParamSet *paramSet)
{
    struct HuksKeyNode *keyNode = (struct HuksKeyNode *)HksMalloc(sizeof(struct HuksKeyNode));
    if (keyNode == NULL) {
        HKS_LOG_E("malloc hks keyNode failed");
        return NULL;
    }

    int32_t ret = GenerateKeyNodeHandle(&keyNode->handle);
    if (ret != HKS_SUCCESS) {
        HksFree((void *)keyNode);
        HKS_LOG_E("get keynode handle failed");
        return NULL;
    }

    struct HksParamSet *runtimeParamSet = NULL;
    ret = BuildRuntimeParamSet(paramSet, &runtimeParamSet);
    if (ret != HKS_SUCCESS) {
        HksFree((void *)keyNode);
        HKS_LOG_E("get runtime paramSet failed");
        return NULL;
    }

    struct HksBlob rawKey = { 0, NULL };
    ret = HksGetRawKeyMaterial(key, &rawKey);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get raw key material failed, ret = %d", ret);
        HksFreeParamSet(&runtimeParamSet);
        HksFree((void *)keyNode);
        return NULL;
    }

    struct HksParamSet *keyBlobParamSet = NULL;
    ret = HksTranslateKeyInfoBlobToParamSet(&rawKey, key, &keyBlobParamSet);
    (void)memset_s(rawKey.data, rawKey.size, 0, rawKey.size);
    HKS_FREE_BLOB(rawKey);

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("translate key info to paramset failed, ret = %d", ret);
        HksFreeParamSet(&runtimeParamSet);
        HksFree((void *)keyNode);
        return NULL;
    }

    keyNode->keyBlobParamSet = keyBlobParamSet;
    keyNode->runtimeParamSet = runtimeParamSet;
    HksMutexLock(HksCoreGetHuksMutex());
    AddNodeAfterDoubleListHead(&g_keyNodeList, &keyNode->listHead);
    HksMutexUnlock(HksCoreGetHuksMutex());

    return keyNode;
}
#else // _STORAGE_LITE_
struct HuksKeyNode *HksCreateKeyNode(const struct HksBlob *key, const struct HksParamSet *paramSet)
{
    struct HuksKeyNode *keyNode = (struct HuksKeyNode *)HksMalloc(sizeof(struct HuksKeyNode));
    if (keyNode == NULL) {
        HKS_LOG_E("malloc hks keyNode failed");
        return NULL;
    }

    int32_t ret = GenerateKeyNodeHandle(&keyNode->handle);
    if (ret != HKS_SUCCESS) {
        HksFree((void *)keyNode);
        HKS_LOG_E("get keynode handle failed");
        return NULL;
    }

    struct HksParamSet *runtimeParamSet = NULL;
    ret = BuildRuntimeParamSet(paramSet, &runtimeParamSet);
    if (ret != HKS_SUCCESS) {
        HksFree((void *)keyNode);
        HKS_LOG_E("get runtime paramSet failed");
        return NULL;
    }

    struct HksBlob aad = { 0, NULL };
    struct HksParamSet *keyBlobParamSet = NULL;
    ret = HksGetAadAndParamSet(key, &aad, &keyBlobParamSet);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&runtimeParamSet);
        HksFree((void *)keyNode);
        HKS_LOG_E("get aad and paramSet failed");
        return NULL;
    }

    ret = HksDecryptKeyBlob(&aad, keyBlobParamSet);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("decrypt keyBlob failed");
        HKS_FREE_BLOB(aad);
        HksFreeParamSet(&runtimeParamSet);
        HksFreeParamSet(&keyBlobParamSet);
        HksFree((void *)keyNode);
        return NULL;
    }

    keyNode->keyBlobParamSet = keyBlobParamSet;
    keyNode->runtimeParamSet = runtimeParamSet;
    HksMutexLock(HksCoreGetHuksMutex());
    AddNodeAfterDoubleListHead(&g_keyNodeList, &keyNode->listHead);
    HksMutexUnlock(HksCoreGetHuksMutex());
    HKS_FREE_BLOB(aad);
    return keyNode;
}
#endif // _STORAGE_LITE_

struct HuksKeyNode *HksQueryKeyNode(uint64_t handle)
{
    struct HuksKeyNode *keyNode = NULL;
    HksMutexLock(HksCoreGetHuksMutex());
    HKS_DLIST_ITER(keyNode, &g_keyNodeList) {
        if ((keyNode != NULL) && (keyNode->handle == handle)) {
            HksMutexUnlock(HksCoreGetHuksMutex());
            return keyNode;
        }
    }
    HksMutexUnlock(HksCoreGetHuksMutex());
    return NULL;
}

static void FreeKeyBlobParamSet(struct HksParamSet **paramSet)
{
    if ((paramSet == NULL) || (*paramSet == NULL)) {
        HKS_LOG_E("invalid keyblob paramset");
        return;
    }
    struct HksParam *keyParam = NULL;
    int32_t ret = HksGetParam(*paramSet, HKS_TAG_KEY, &keyParam);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get key param failed!");
        HksFreeParamSet(paramSet);
        return;
    }
    (void)memset_s(keyParam->blob.data, keyParam->blob.size, 0, keyParam->blob.size);
    HksFreeParamSet(paramSet);
}

static void FreeCachedData(void **ctx)
{
    struct HksBlob *cachedData = (struct HksBlob *)*ctx;
    if (cachedData == NULL) {
        return;
    }
    if (cachedData->data != NULL) {
        (void)memset_s(cachedData->data, cachedData->size, 0, cachedData->size);
        HKS_FREE_PTR(cachedData->data);
    }
    HKS_FREE_PTR(*ctx);
}

static void FreeCtx(uint32_t purpose, uint32_t alg, void **ctx)
{
    switch (purpose) {
        case HKS_KEY_PURPOSE_AGREE:
        case HKS_KEY_PURPOSE_DERIVE:
            FreeCachedData(ctx);
            break;
        case HKS_KEY_PURPOSE_SIGN:
        case HKS_KEY_PURPOSE_VERIFY:
            if (alg != HKS_ALG_ED25519) {
                HksCryptoHalHashFreeCtx(ctx);
            } else {
                FreeCachedData(ctx);
            }
            break;
        case HKS_KEY_PURPOSE_ENCRYPT:
        case HKS_KEY_PURPOSE_DECRYPT:
            if (alg != HKS_ALG_RSA) {
                HksCryptoHalEncryptFreeCtx(ctx, alg);
            } else {
                FreeCachedData(ctx);
            }
            break;
        case HKS_KEY_PURPOSE_MAC:
            HksCryptoHalHmacFreeCtx(ctx);
            break;
        default:
            return;
    }
}

static void FreeRuntimeParamSet(struct HksParamSet **paramSet)
{
    if ((paramSet == NULL) || (*paramSet == NULL)) {
        HKS_LOG_E("invalid keyblob paramset");
        return;
    }

    struct HksParam *ctxParam = NULL;
    int32_t ret = HksGetParam(*paramSet, HKS_TAG_CRYPTO_CTX, &ctxParam);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(paramSet);
        HKS_LOG_E("get ctx from keyNode failed!");
        return;
    }

    if (ctxParam->uint64Param != 0) {
        void *ctx = (void *)(uintptr_t)ctxParam->uint64Param;
        struct HksParam *param1 = NULL;
        ret = HksGetParam(*paramSet, HKS_TAG_PURPOSE, &param1);
        if (ret != HKS_SUCCESS) {
            HksFreeParamSet(paramSet);
            return;
        }
        struct HksParam *param2 = NULL;
        ret = HksGetParam(*paramSet, HKS_TAG_ALGORITHM, &param2);
        if (ret != HKS_SUCCESS) {
            HksFreeParamSet(paramSet);
            return;
        }
        FreeCtx(param1->uint32Param, param2->uint32Param, &ctx);
        ctxParam->uint64Param = 0; /* clear ctx to NULL */
    }
    HksFreeParamSet(paramSet);
}

void HksDeleteKeyNode(uint64_t handle)
{
    struct HuksKeyNode *keyNode = NULL;
    HksMutexLock(HksCoreGetHuksMutex());
    HKS_DLIST_ITER(keyNode, &g_keyNodeList) {
        if ((keyNode != NULL) && (keyNode->handle == handle)) {
            RemoveDoubleListNode(&keyNode->listHead);
            FreeKeyBlobParamSet(&keyNode->keyBlobParamSet);
            FreeRuntimeParamSet(&keyNode->runtimeParamSet);
            HKS_FREE_PTR(keyNode);
            HksMutexUnlock(HksCoreGetHuksMutex());
            return;
        }
    }
    HksMutexUnlock(HksCoreGetHuksMutex());
}
#endif /* _CUT_AUTHENTICATE_ */
