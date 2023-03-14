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

#include "hks_ipc_serialization.h"

#include "hks_log.h"
#include "hks_mem.h"

static int32_t CopyUint32ToBuffer(uint32_t value, const struct HksBlob *destBlob, uint32_t *destOffset)
{
    if (*destOffset > destBlob->size) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset, &(value), sizeof(value)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    *destOffset += sizeof(value);
    return HKS_SUCCESS;
}

static int32_t CopyBlobToBuffer(const struct HksBlob *blob, const struct HksBlob *destBlob, uint32_t *destOffset)
{
    if (CheckBlob(blob) != HKS_SUCCESS) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((*destOffset > destBlob->size) ||
        ((destBlob->size - *destOffset) < (sizeof(blob->size) + ALIGN_SIZE(blob->size)))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset,
        &(blob->size), sizeof(blob->size)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    *destOffset += sizeof(blob->size);

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset, blob->data, blob->size) != EOK) {
        *destOffset -= sizeof(blob->size);
        return HKS_ERROR_BAD_STATE;
    }

    *destOffset += ALIGN_SIZE(blob->size);
    return HKS_SUCCESS;
}

static int32_t CopyParamSetToBuffer(const struct HksParamSet *paramSet,
    const struct HksBlob *destBlob, uint32_t *destOffset)
{
    if (paramSet == NULL) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if ((*destOffset > destBlob->size) || (destBlob->size - *destOffset < ALIGN_SIZE(paramSet->paramSetSize))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(destBlob->data + *destOffset, destBlob->size - *destOffset,
        paramSet, paramSet->paramSetSize) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    *destOffset += ALIGN_SIZE(paramSet->paramSetSize);
    return HKS_SUCCESS;
}

static int32_t GetUint32FromBuffer(uint32_t *value, const struct HksBlob *srcBlob, uint32_t *srcOffset)
{
    if ((*srcOffset > srcBlob->size) || (srcBlob->size - *srcOffset < sizeof(uint32_t))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(value, sizeof(*value), srcBlob->data + *srcOffset, sizeof(uint32_t)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    *srcOffset += sizeof(uint32_t);
    return HKS_SUCCESS;
}

int32_t GetBlobFromBuffer(struct HksBlob *blob, const struct HksBlob *srcBlob, uint32_t *srcOffset)
{
    if ((*srcOffset > srcBlob->size) || ((srcBlob->size - *srcOffset) < sizeof(uint32_t))) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    uint32_t size = *((uint32_t *)(srcBlob->data + *srcOffset));
    if (ALIGN_SIZE(size) > srcBlob->size - *srcOffset - sizeof(uint32_t)) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    blob->size = size;
    *srcOffset += sizeof(blob->size);
    blob->data = (uint8_t *)(srcBlob->data + *srcOffset);
    *srcOffset += ALIGN_SIZE(blob->size);
    return HKS_SUCCESS;
}

static int32_t GetParamSetFromBuffer(struct HksParamSet **paramSet,
    const struct HksBlob *srcBlob, uint32_t *srcOffset)
{
    if (*srcOffset > srcBlob->size) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint32_t size = *((uint32_t *)(srcBlob->data + *srcOffset));
    if (ALIGN_SIZE(size) > srcBlob->size - *srcOffset) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    *paramSet = (struct HksParamSet*)(srcBlob->data + *srcOffset);
    *srcOffset += ALIGN_SIZE((*paramSet)->paramSetSize);
    return HKS_SUCCESS;
}

static int32_t GetKeyAndParamSetFromBuffer(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSet, uint32_t *offset)
{
    int32_t ret = GetBlobFromBuffer(keyAlias, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyAlias failed");
        return ret;
    }

    ret = GetParamSetFromBuffer(paramSet, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get paramSet failed");
    }

    return ret;
}

static int32_t MallocBlobFromBuffer(const struct HksBlob *srcData, struct HksBlob *blob, uint32_t *offset)
{
    uint32_t blobSize = 0;
    int32_t ret = GetUint32FromBuffer(&blobSize, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get blobSize failed");
        return ret;
    }

    if (IsInvalidLength(blobSize)) {
        HKS_LOG_E("get blobSize failed");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    uint8_t *blobData = (uint8_t *)HksMalloc(blobSize);
    if (blobData == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    blob->data = blobData;
    blob->size = blobSize;
    return HKS_SUCCESS;
}

static int32_t MallocParamSetFromBuffer(const struct HksBlob *srcData, struct HksParamSet **paramSet, uint32_t *offset)
{
    uint32_t paramSetOutSize = 0;
    int32_t ret = GetUint32FromBuffer(&paramSetOutSize, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get paramSetOutSize failed");
        return ret;
    }

    if (IsInvalidLength(paramSetOutSize)) {
        HKS_LOG_E("get paramSetOutSize failed");
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    *paramSet = (struct HksParamSet *)HksMalloc(paramSetOutSize);
    if (*paramSet == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    (*paramSet)->paramSetSize = paramSetOutSize;
    return HKS_SUCCESS;
}

int32_t HksGenerateKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSetIn, struct HksBlob *keyOut)
{
    uint32_t offset = 0;
    int32_t ret = GetKeyAndParamSetFromBuffer(srcData, keyAlias, paramSetIn, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("GetKeyAndParamSetFromBuffer failed");
        return ret;
    }

    uint32_t keyOutSize = 0;
    ret = GetUint32FromBuffer(&keyOutSize, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyOutSize failed");
        return ret;
    }

    if (keyOutSize > MAX_OUT_BLOB_SIZE) {
        HKS_LOG_E("keyOutSize out of range %u", keyOutSize);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    /* no allocate memory when keyOutSize is 0 */
    if (keyOutSize > 0) {
        uint8_t *keyData = (uint8_t *)HksMalloc(keyOutSize);
        if (keyData == NULL) {
            return HKS_ERROR_MALLOC_FAIL;
        }

        keyOut->data = keyData;
        keyOut->size = keyOutSize;
    }

    return HKS_SUCCESS;
}

int32_t HksImportKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias, struct HksParamSet **paramSet,
    struct HksBlob *key)
{
    uint32_t offset = 0;
    int32_t ret = GetKeyAndParamSetFromBuffer(srcData, keyAlias, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("GetKeyAndParamSetFromBuffer failed");
        return ret;
    }

    return GetBlobFromBuffer(key, srcData, &offset);
}

int32_t HksExportPublicKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias, struct HksBlob *key)
{
    uint32_t offset = 0;
    int32_t ret = GetBlobFromBuffer(keyAlias, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyAlias failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, key, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc key data failed");
    }
    return ret;
}

int32_t HksGetKeyParamSetUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSet)
{
    uint32_t offset = 0;
    int32_t ret = GetBlobFromBuffer(keyAlias, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyAlias failed");
        return ret;
    }

    ret = MallocParamSetFromBuffer(srcData, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc paramSet failed");
    }
    return ret;
}

static int32_t SignVerifyMacUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *inputData, uint32_t *offset)
{
    int32_t ret = GetKeyAndParamSetFromBuffer(srcData, key, paramSet, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("GetKeyAndParamSetFromBuffer failed");
    }

    ret = GetBlobFromBuffer(inputData, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get unsignedData failed");
    }

    return ret;
}

int32_t HksSignUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *unsignedData, struct HksBlob *signature)
{
    uint32_t offset = 0;
    int32_t ret = SignVerifyMacUnpack(srcData, key, paramSet, unsignedData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerifyMacUnpack failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, signature, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc signature data failed");
    }
    return ret;
}

int32_t HksVerifyUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *unsignedData, struct HksBlob *signature)
{
    uint32_t offset = 0;
    int32_t ret = SignVerifyMacUnpack(srcData, key, paramSet, unsignedData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerifyMacUnpack failed");
        return ret;
    }

    return GetBlobFromBuffer(signature, srcData, &offset);
}

int32_t HksEncryptDecryptUnpack(const struct HksBlob *srcData, struct HksBlob *key,
    struct HksParamSet **paramSet, struct HksBlob *inputText, struct HksBlob *outputText)
{
    uint32_t offset = 0;
    int32_t ret = GetKeyAndParamSetFromBuffer(srcData, key, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("getKeyAndParamSetFromBuffer failed");
    }

    ret = GetBlobFromBuffer(inputText, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get inputText failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, outputText, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc outputText data failed");
    }
    return ret;
}

int32_t HksAgreeKeyUnpack(const struct HksBlob *srcData, struct HksParamSet **paramSet, struct HksBlob *privateKey,
    struct HksBlob *peerPublicKey, struct HksBlob *agreedKey)
{
    uint32_t offset = 0;
    int32_t ret = GetParamSetFromBuffer(paramSet, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get paramSet failed");
        return ret;
    }

    ret = GetBlobFromBuffer(privateKey, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get privateKey failed");
        return ret;
    }

    ret = GetBlobFromBuffer(peerPublicKey, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get peerPublicKey failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, agreedKey, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc agreedKey data failed");
    }
    return ret;
}

int32_t HksDeriveKeyUnpack(const struct HksBlob *srcData, struct HksParamSet **paramSet, struct HksBlob *kdfKey,
    struct HksBlob *derivedKey)
{
    uint32_t offset = 0;
    int32_t ret = GetParamSetFromBuffer(paramSet, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get paramSet failed");
        return ret;
    }

    ret = GetBlobFromBuffer(kdfKey, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get kdfKey failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, derivedKey, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc derivedKey data failed");
    }

    return ret;
}

int32_t HksHmacUnpack(const struct HksBlob *srcData, struct HksBlob *key, struct HksParamSet **paramSet,
    struct HksBlob *inputData, struct HksBlob *mac)
{
    uint32_t offset = 0;
    int32_t ret = SignVerifyMacUnpack(srcData, key, paramSet, inputData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("SignVerifyMacUnpack failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, mac, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc mac data failed");
    }

    return ret;
}

static int32_t KeyInfoListInit(struct HksKeyInfo *keyInfoList, uint32_t listCount,
    const struct HksBlob *srcData, uint32_t *offset)
{
    uint32_t i = 0;
    int32_t ret = HKS_SUCCESS;
    for (; i < listCount; ++i) {
        ret = MallocBlobFromBuffer(srcData, &keyInfoList[i].alias, offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("malloc keyInfoList alias failed");
            break;
        }

        ret = MallocParamSetFromBuffer(srcData, &keyInfoList[i].paramSet, offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("malloc keyInfoList paramSetSize failed");
            break;
        }
    }

    if (ret != HKS_SUCCESS) {
        for (uint32_t j = 0; j < i; ++j) {
            HKS_FREE_BLOB(keyInfoList[j].alias);
            HKS_FREE_PTR(keyInfoList[j].paramSet);
        }
    }
    return ret;
}

int32_t HksGetKeyInfoListUnpack(const struct HksBlob *srcData, uint32_t *listCount, struct HksKeyInfo **keyInfoList)
{
    uint32_t offset = 0;
    int32_t ret = GetUint32FromBuffer(listCount, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get count failed");
        return ret;
    }

    if ((UINT32_MAX / sizeof(struct HksKeyInfo)) < *listCount) {
        HKS_LOG_E("listCount too big %u", *listCount);
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    uint32_t keyInfoListSize = (*listCount) * sizeof(struct HksKeyInfo);
    if (IsInvalidLength(keyInfoListSize)) {
        HKS_LOG_E("keyInfoListSize too big %u", keyInfoListSize);
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }

    *keyInfoList = (struct HksKeyInfo *)HksMalloc(keyInfoListSize);
    if (*keyInfoList == NULL) {
        HKS_LOG_E("*keyInfoList is NULL");
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(*keyInfoList, keyInfoListSize, 0, keyInfoListSize);

    ret = KeyInfoListInit(*keyInfoList, *listCount, srcData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("KeyInfoListInit failed");
        HKS_FREE_PTR(*keyInfoList);
    }

    return ret;
}

int32_t HksGetKeyInfoListPackFromService(struct HksBlob *destData, uint32_t listCount,
    const struct HksKeyInfo *keyInfoList)
{
    uint32_t offset = 0;
    int32_t ret = CopyUint32ToBuffer(listCount, destData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("copy CopyUint32ToBuffer failed");
        return ret;
    }

    for (uint32_t i = 0; i < listCount; ++i) {
        ret = CopyBlobToBuffer(&keyInfoList[i].alias, destData, &offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("copy alias failed");
            return ret;
        }

        ret = CopyParamSetToBuffer(keyInfoList[i].paramSet, destData, &offset);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("copy paramSet failed");
            return ret;
        }
    }

    return HKS_SUCCESS;
}

int32_t HksCertificateChainUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksParamSet **paramSet, struct HksBlob *certChainBlob)
{
    uint32_t offset = 0;
    int32_t ret = GetKeyAndParamSetFromBuffer(srcData, keyAlias, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("GetKeyAndParamSetFromBuffer failed");
    }

    ret = MallocBlobFromBuffer(srcData, certChainBlob, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc certChainBlob data failed");
    }
    return ret;
}

static int32_t WrapUnwrapKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksBlob *targetKeyAlias, struct HksParamSet **paramSet, uint32_t *offset)
{
    int32_t ret = GetBlobFromBuffer(keyAlias, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get keyAlias failed");
        return ret;
    }

    ret = GetBlobFromBuffer(targetKeyAlias, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get targetKeyAlias failed");
        return ret;
    }

    ret = GetParamSetFromBuffer(paramSet, srcData, offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get paramSet failed");
    }

    return ret;
}

int32_t HksWrapKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias, struct HksBlob *targetKeyAlias,
    struct HksParamSet **paramSet, struct HksBlob *wrappedData)
{
    uint32_t offset = 0;
    int32_t ret = WrapUnwrapKeyUnpack(srcData, keyAlias, targetKeyAlias, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("WrapUnwrapKeyUnpack failed");
        return ret;
    }

    ret = MallocBlobFromBuffer(srcData, wrappedData, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc wrappedData data failed");
    }

    return ret;
}

int32_t HksUnwrapKeyUnpack(const struct HksBlob *srcData, struct HksBlob *keyAlias,
    struct HksBlob *targetKeyAlias, struct HksBlob *wrappedData, struct HksParamSet **paramSet)
{
    uint32_t offset = 0;
    int32_t ret = WrapUnwrapKeyUnpack(srcData, keyAlias, targetKeyAlias, paramSet, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("WrapUnwrapKeyUnpack failed");
        return ret;
    }

    return GetBlobFromBuffer(wrappedData, srcData, &offset);
}

int32_t HksTrustCertsUnpack(const struct HksBlob *srcData, struct HksBlob *certChainBlob)
{
    uint32_t offset = 0;
    int32_t ret = MallocBlobFromBuffer(srcData, certChainBlob, &offset);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("malloc certChainBlob data failed");
    }
    return ret;
}

int32_t HksParamSetToParams(const struct HksParamSet *paramSet, struct HksParamOut *outParams, uint32_t cnt)
{
    int32_t ret;
    struct HksParam *param = NULL;
    for (uint32_t i = 0; i < cnt; i++) {
        ret = HksGetParam(paramSet, outParams[i].tag, &param);
        if (ret != HKS_SUCCESS) {
            return ret;
        }

        switch (GetTagType(outParams[i].tag)) {
            case HKS_TAG_TYPE_INT:
                *outParams[i].int32Param = param->int32Param;
                break;
            case HKS_TAG_TYPE_UINT:
                *outParams[i].uint32Param = param->uint32Param;
                break;
            case HKS_TAG_TYPE_ULONG:
                *outParams[i].uint64Param = param->uint64Param;
                break;
            case HKS_TAG_TYPE_BOOL:
                *outParams[i].boolParam = param->boolParam;
                break;
            case HKS_TAG_TYPE_BYTES:
                *outParams[i].blob = param->blob;
                break;
            default:
                HKS_LOG_I("invalid tag type:%x", GetTagType(outParams[i].tag));
                return HKS_ERROR_INVALID_ARGUMENT;
        }
    }
    return HKS_SUCCESS;
}

enum HksTagType GetTagType(enum HksTag tag)
{
    return (enum HksTagType)((uint32_t)tag & HKS_TAG_TYPE_MASK);
}
