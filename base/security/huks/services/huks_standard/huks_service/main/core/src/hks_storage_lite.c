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

#ifndef _CUT_AUTHENTICATE_

#ifdef _STORAGE_LITE_

#include "hks_storage.h"

#include "hks_file_operator.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_param.h"
#include "hks_storage_adapter.h"

#include "huks_access.h"

#define HKS_FILE_OFFSET_BASE 0
#define MAX_STORAGE_SIZE 5120
#define MAX_BUF_SIZE 65536
#define MAX_KEY_COUNT 256
#define BUF_SIZE_ADDEND_PER_TIME 1024
#define HKS_STORAGE_VERSION 1
#define HKS_STORAGE_RESERVED_SEALING_ALG 0xFEDCBA98

struct HksBlob g_storageImageBuffer = { 0, NULL };

static uint32_t HksGetStoreFileOffset(void)
{
    return HKS_FILE_OFFSET_BASE;
}

static int32_t ConstructCalcMacParamSet(struct HksParamSet **paramSet)
{
    struct HksParamSet *outputParamSet = NULL;
    int32_t ret = HksInitParamSet(&outputParamSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    do {
        struct HksParam digestParam = {
            .tag = HKS_TAG_DIGEST,
            .uint32Param = HKS_DIGEST_SHA512
        };

        ret = HksAddParams(outputParamSet, &digestParam, 1); /* 1: param count */
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = HksBuildParamSet(&outputParamSet);
    } while (0);

    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(&outputParamSet);
        return ret;
    }

    *paramSet = outputParamSet;
    return ret;
}

static int32_t CalcHeaderMac(const struct HksBlob *salt, const uint8_t *buf,
    const uint32_t srcSize, struct HksBlob *mac)
{
    if (srcSize == 0) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    struct HksBlob srcData = { srcSize, NULL };
    srcData.data = (uint8_t *)HksMalloc(srcData.size);
    if (srcData.data == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    int32_t ret;
    struct HksParamSet *paramSet = NULL;
    do {
        if (memcpy_s(srcData.data, srcData.size, buf, srcSize) != EOK) {
            ret = HKS_ERROR_BAD_STATE;
            break;
        }

        ret = ConstructCalcMacParamSet(&paramSet);
        if (ret != HKS_SUCCESS) {
            break;
        }

        ret = HuksAccessCalcHeaderMac(paramSet, salt, &srcData, mac);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("access calc header mac failed, ret = %d.", ret);
        }
    } while (0);

    HKS_FREE_BLOB(srcData);
    HksFreeParamSet(&paramSet);
    return ret;
}

static int32_t InitImageBuffer(void)
{
    /* caller func ensure g_storageImageBuffer.size is larger than sizeof(*keyInfoHead) */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    keyInfoHead->version = HKS_STORAGE_VERSION;
    keyInfoHead->keyCount = 0;
    keyInfoHead->totalLen = sizeof(*keyInfoHead);
    keyInfoHead->sealingAlg = HKS_STORAGE_RESERVED_SEALING_ALG;

    struct HksBlob salt = { HKS_DERIVE_DEFAULT_SALT_LEN, keyInfoHead->salt };
    int32_t ret = HuksAccessGenerateRandom(NULL, &salt);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("generate random failed, ret = %d", ret);
        return ret;
    }

    struct HksBlob mac = { HKS_HMAC_DIGEST_SHA512_LEN, keyInfoHead->hmac };
    uint16_t size = sizeof(*keyInfoHead) - HKS_HMAC_DIGEST_SHA512_LEN;

    return CalcHeaderMac(&salt, g_storageImageBuffer.data, size, &mac);
}

static void CleanImageBuffer(void)
{
    if (g_storageImageBuffer.data == NULL) {
        return;
    }
    (void)memset_s(g_storageImageBuffer.data, g_storageImageBuffer.size, 0, g_storageImageBuffer.size);
}

static int32_t ApplyImageBuffer(uint32_t size)
{
    if (g_storageImageBuffer.data != NULL) {
        return HKS_SUCCESS;
    }

    if ((size == 0) || (size > MAX_BUF_SIZE)) {
        HKS_LOG_E("invalid size = %u", size);
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    g_storageImageBuffer.data = (uint8_t *)HksMalloc(size);
    if (g_storageImageBuffer.data == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    g_storageImageBuffer.size = size;

    return HKS_SUCCESS;
}

static void FreeImageBuffer(void)
{
    CleanImageBuffer();
    HKS_FREE_BLOB(g_storageImageBuffer);
}

static int32_t FreshImageBuffer(const char *fileName)
{
    /* caller func ensure g_storageImageBuffer.size is larger than sizeof(*keyInfoHead) */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    uint32_t totalLen = keyInfoHead->totalLen;

    /* check totalLen */
    if ((totalLen < sizeof(*keyInfoHead)) || (totalLen > MAX_STORAGE_SIZE)) {
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    if (totalLen == sizeof(*keyInfoHead)) {
        return HKS_SUCCESS;
    }

    uint32_t offset = HksGetStoreFileOffset();
    uint32_t fileLen = HksFileSize(HKS_KEY_STORE_PATH, fileName);
    if (fileLen < (totalLen + offset)) { /* keyfile len at least totalLen + offset */
        HKS_LOG_E("total Len: %u, invalid file size: %u", totalLen, fileLen);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    uint8_t *buf = (uint8_t *)HksMalloc(totalLen);
    if (buf == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    fileLen = HksFileRead(HKS_KEY_STORE_PATH, fileName, offset, buf, totalLen);
    if (fileLen == 0) {
        HKS_FREE_PTR(buf);
        return HKS_ERROR_READ_FILE_FAIL;
    }

    FreeImageBuffer();
    g_storageImageBuffer.data = buf;
    g_storageImageBuffer.size = totalLen;

    return HKS_SUCCESS;
}

static int32_t CheckKeyInfoHeaderValid(void)
{
    /* caller func ensure g_storageImageBuffer.size is larger than sizeof(*keyInfoHead) */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;

    uint8_t mac512[HKS_HMAC_DIGEST_SHA512_LEN] = {0};
    struct HksBlob mac = { HKS_HMAC_DIGEST_SHA512_LEN, mac512 };
    struct HksBlob salt = { HKS_DERIVE_DEFAULT_SALT_LEN, keyInfoHead->salt };
    uint16_t size = sizeof(*keyInfoHead) - HKS_HMAC_DIGEST_SHA512_LEN;

    int32_t ret = CalcHeaderMac(&salt, g_storageImageBuffer.data, size, &mac);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    if (HksMemCmp(mac.data, keyInfoHead->hmac, HKS_HMAC_DIGEST_SHA512_LEN) != 0) {
        HKS_LOG_E("hmac value not match");
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    return HKS_SUCCESS;
}

static int32_t RefreshKeyInfoHeaderHmac(struct HksStoreHeaderInfo *keyInfoHead)
{
    struct HksBlob mac = { HKS_HMAC_DIGEST_SHA512_LEN, keyInfoHead->hmac };
    struct HksBlob salt = { HKS_DERIVE_DEFAULT_SALT_LEN, keyInfoHead->salt };
    uint16_t size = sizeof(*keyInfoHead) - HKS_HMAC_DIGEST_SHA512_LEN;

    uint8_t *buffer = (uint8_t *)HksMalloc(sizeof(*keyInfoHead));
    if (buffer == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memcpy_s(buffer, sizeof(*keyInfoHead), keyInfoHead, sizeof(*keyInfoHead));

    int32_t ret = CalcHeaderMac(&salt, buffer, size, &mac);
    HKS_FREE_PTR(buffer);
    return ret;
}

static struct HksBlob HksGetImageBuffer(void)
{
    return g_storageImageBuffer;
}

static int32_t LoadFileToBuffer(const char *fileName)
{
    /* 1. read key info header */
    uint32_t offset = HksGetStoreFileOffset();
    uint32_t len = HksFileRead(HKS_KEY_STORE_PATH, fileName, offset,
        g_storageImageBuffer.data, g_storageImageBuffer.size);

    int32_t ret;
    do {
        /* 2. file not exist or read nothing, init image */
        if (len == 0) {
            HKS_LOG_I("file not exist, init buffer.");
            ret = InitImageBuffer();
            if (ret != HKS_SUCCESS) {
                break; /* init fail, need free global buf */
            }
            return ret;
        }

        /* 3. read header success, check keyinfo header */
        HKS_LOG_I("file exist, check buffer.");
        ret = CheckKeyInfoHeaderValid();
        if (ret != HKS_SUCCESS) {
            break;
        }

        /* 4. check success, load full buffer */
        ret = FreshImageBuffer(fileName);
    } while (0);

    if (ret != HKS_SUCCESS) {
        FreeImageBuffer();
    }

    return ret;
}

int32_t HksLoadFileToBuffer(void)
{
    if (g_storageImageBuffer.data != NULL) {
        return HKS_SUCCESS;
    }

    /* 1. malloc keyinfo header size buffer */
    int32_t ret = ApplyImageBuffer(sizeof(struct HksStoreHeaderInfo));
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    CleanImageBuffer();

    /* 2. read file to buffer */
    return LoadFileToBuffer(HKS_KEY_STORE_FILE_NAME);
}

static int32_t CleanStorageKeyInfo(const char *fileName)
{
    int32_t ret = InitImageBuffer();
    if (ret != HKS_SUCCESS) {
        FreeImageBuffer();
        return ret;
    }

    /* write to file */
    uint32_t totalLen = sizeof(struct HksStoreHeaderInfo);
    uint32_t fileOffset = HksGetStoreFileOffset();
    ret = HksFileWrite(HKS_KEY_STORE_PATH, fileName, fileOffset, g_storageImageBuffer.data, totalLen);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("write file failed when hks refresh file buffer");
        FreeImageBuffer();
    }
    return ret;
}

int32_t HksFileBufferRefresh(void)
{
    /* malloc keyinfo header size buffer */
    int32_t ret = ApplyImageBuffer(sizeof(struct HksStoreHeaderInfo));
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    CleanImageBuffer();

    return CleanStorageKeyInfo(HKS_KEY_STORE_FILE_NAME);
}

/*
 * Storage format:
 * keyInfoHeader + keyInfo1 + keyInfo2 + ... + keyInfoN
 *
 *                +--------------------------------------------------------------+
 * KeyInfoHeader: | version | keyCount | totalLen | sealingAlg |  salt  |  hmac  |
 *                | 2bytes  |  2bytes  |  4bytes  |   4bytes   |16bytes |64bytes |
 *                +--------------------------------------------------------------+
 *
 *          +---------------------------------------------------------------------+
 * KeyInfo: | keyInfoLen | keySize |  nonce  |  flag  | keyAlg | keyMode | digest |
 *          |   2bytes   | 2bytes  | 16bytes | 1bytes | 1bytes | 1bytes  | 1bytes |
 *          +---------------------------------------------------------------------+
 *          | padding |  rsv   | keyLen | purpose |  role  | domain  | aliasSize |
 *          | 1bytes  | 1bytes | 2bytes | 4bytes  | 4bytes | 21bytes |  1bytes   |
 *          +--------------------------------------------------------------------+
 *          | AuthIdSize |  keyAlias   |  keyAuthId  |         key          |
 *          |   1bytes   | max 64bytes | max 64bytes | max keyMaterial size |
 *          +---------------------------------------------------------------+
 */
static int32_t GetKeyOffsetByKeyAlias(const struct HksBlob *keyAlias, uint32_t *keyOffset)
{
    struct HksBlob storageBuf = HksGetImageBuffer();
    if (storageBuf.size < sizeof(struct HksStoreHeaderInfo)) {
        HKS_LOG_E("invalid keyinfo buffer size %u.", storageBuf.size);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    /* 1. get imageBuffer total Len */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)storageBuf.data;
    uint32_t keyCount = keyInfoHead->keyCount;
    uint32_t totalLen = keyInfoHead->totalLen;
    if (keyCount == 0) {
        return HKS_ERROR_NOT_EXIST;
    }
    if ((keyCount > MAX_KEY_COUNT) || (totalLen > storageBuf.size)) {
        HKS_LOG_E("keyCount invalid, or storageBuf size invalid");
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    /* 2. traverse imageBuffer to search for keyAlias */
    uint32_t offset = sizeof(*keyInfoHead);
    for (uint32_t i = 0; i < keyCount; ++i) {
        if ((totalLen < offset) || ((totalLen - offset) < sizeof(struct HksStoreKeyInfo))) {
            HKS_LOG_E("invalid keyinfo size.");
            return HKS_ERROR_INVALID_KEY_FILE;
        }

        uint8_t *tmpBuf = storageBuf.data + offset;
        struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)tmpBuf;
        if (HksIsKeyInfoLenInvalid(keyInfo) || (keyInfo->keyInfoLen > (totalLen - offset))) {
            HKS_LOG_E("invalid keyinfo len");
            return HKS_ERROR_INVALID_KEY_FILE;
        }

        if (keyInfo->aliasSize == keyAlias->size) {
            if (HksMemCmp(keyAlias->data, tmpBuf + sizeof(*keyInfo), keyAlias->size) == 0) {
                *keyOffset = offset;
                return HKS_SUCCESS;
            }
        }

        offset += keyInfo->keyInfoLen;
    }

    return HKS_ERROR_NOT_EXIST;
}

static int32_t AdjustImageBuffer(uint32_t totalLenAdded, const struct HksBlob *keyBlob)
{
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;

    /* buffer has been checked will not overflow */
    uint32_t newBufLen = g_storageImageBuffer.size +
        ((keyBlob->size > BUF_SIZE_ADDEND_PER_TIME) ? keyBlob->size : BUF_SIZE_ADDEND_PER_TIME);
    uint8_t *buf = (uint8_t *)HksMalloc(newBufLen);
    if (buf == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(buf, newBufLen, 0, newBufLen);

    /* copy old imagebuf to new malloc buf */
    if (memcpy_s(buf, newBufLen, g_storageImageBuffer.data, keyInfoHead->totalLen) != EOK) {
        HKS_FREE_PTR(buf);
        return HKS_ERROR_BAD_STATE;
    }

    /* append new add key buffer to the end */
    if (memcpy_s(buf + keyInfoHead->totalLen, newBufLen - keyInfoHead->totalLen,
        keyBlob->data, keyBlob->size) != EOK) {
        HKS_FREE_PTR(buf);
        return HKS_ERROR_BAD_STATE;
    }

    struct HksStoreHeaderInfo *newHead = (struct HksStoreHeaderInfo *)buf;
    newHead->totalLen = totalLenAdded;
    newHead->keyCount += 1;

    FreeImageBuffer();
    g_storageImageBuffer.data = buf;
    g_storageImageBuffer.size = newBufLen;

    return HKS_SUCCESS;
}

static int32_t AppendNewKey(const struct HksBlob *keyBlob)
{
    struct HksBlob storageBuf = HksGetImageBuffer();
    if (storageBuf.size < sizeof(struct HksStoreHeaderInfo)) {
        HKS_LOG_E("invalid keyinfo buffer size %u.", storageBuf.size);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    /* 1. get imagebuf total Len */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)storageBuf.data;

    if (IsAdditionOverflow(keyInfoHead->totalLen, keyBlob->size)) {
        return HKS_ERROR_BAD_STATE;
    }

    uint32_t totalLenAdded = keyInfoHead->totalLen + keyBlob->size;
    if (totalLenAdded > MAX_STORAGE_SIZE) {
        HKS_LOG_E("after add, buffer too big to store");
        return HKS_ERROR_STORAGE_FAILURE;
    }

    /* imagebuf is enough to append new keyinfo */
    if (storageBuf.size >= totalLenAdded) {
        if (memcpy_s(storageBuf.data + keyInfoHead->totalLen, storageBuf.size - keyInfoHead->totalLen,
            keyBlob->data, keyBlob->size) != EOK) {
            return HKS_ERROR_BAD_STATE;
        }
        keyInfoHead->totalLen = totalLenAdded;
        keyInfoHead->keyCount += 1;
        return HKS_SUCCESS;
    }

    /* need malloc new buffer */
    return AdjustImageBuffer(totalLenAdded, keyBlob);
}

static int32_t GetLenAfterAddKey(const struct HksBlob *keyBlob, uint32_t totalLen, uint32_t *totalLenAdded)
{
    if (IsAdditionOverflow(totalLen, keyBlob->size)) {
        return HKS_ERROR_BAD_STATE;
    }

    uint32_t newTotalLen = totalLen + keyBlob->size;
    if (newTotalLen > MAX_STORAGE_SIZE) {
        HKS_LOG_E("after add, buffer too big to store");
        return HKS_ERROR_STORAGE_FAILURE;
    }

    *totalLenAdded = newTotalLen;
    return HKS_SUCCESS;
}

static int32_t DeleteKey(uint32_t keyOffset)
{
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)(g_storageImageBuffer.data + keyOffset);

    uint32_t keyInfoLen = keyInfo->keyInfoLen;
    uint32_t nextKeyOffset = keyOffset + keyInfoLen;
    if (nextKeyOffset > keyInfoHead->totalLen) {
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    (void)memset_s(keyInfo, keyInfoLen, 0, keyInfoLen);

    /* If key to delete is not the last key, need to be move image buffer */
    if (nextKeyOffset < keyInfoHead->totalLen) {
        if (memmove_s(keyInfo, keyInfoHead->totalLen - keyOffset, g_storageImageBuffer.data + nextKeyOffset,
            keyInfoHead->totalLen - nextKeyOffset) != EOK) {
            HKS_LOG_E("memmove image buffer failed");
            return HKS_ERROR_BAD_STATE;
        }
        /* clear the last buffer */
        (void)memset_s(g_storageImageBuffer.data + keyInfoHead->totalLen - keyInfoLen, keyInfoLen, 0, keyInfoLen);
    }
    keyInfoHead->keyCount -= 1;
    keyInfoHead->totalLen -= keyInfoLen;

    return HKS_SUCCESS;
}

static int32_t StoreKeyBlob(bool needDeleteKey, uint32_t offset, const struct HksBlob *keyBlob)
{
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)(g_storageImageBuffer.data + offset);

    struct HksStoreHeaderInfo newkeyInfoHead;
    if (memcpy_s(&newkeyInfoHead, sizeof(newkeyInfoHead), keyInfoHead, sizeof(*keyInfoHead)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    uint32_t totalLenAdded;
    int32_t ret;

    /* 1. check storage buffer enough for store new key */
    if (needDeleteKey) {
        ret = GetLenAfterAddKey(keyBlob, keyInfoHead->totalLen - keyInfo->keyInfoLen, &totalLenAdded);
    } else {
        newkeyInfoHead.keyCount += 1,
        ret = GetLenAfterAddKey(keyBlob, keyInfoHead->totalLen, &totalLenAdded);
    }
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. calc temp hmac */
    newkeyInfoHead.totalLen = totalLenAdded;
    ret = RefreshKeyInfoHeaderHmac(&newkeyInfoHead);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 3. delete key if keyExist */
    if (needDeleteKey) {
        ret = DeleteKey(offset);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
    }

    /* 4. append key */
    ret = AppendNewKey(keyBlob);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 5. replace header */
    if (memcpy_s(g_storageImageBuffer.data, sizeof(newkeyInfoHead), &newkeyInfoHead, sizeof(newkeyInfoHead)) != EOK) {
        HKS_LOG_E("replace header memcpy failed");
        return HKS_ERROR_BAD_STATE;
    }
    return HKS_SUCCESS;
}

static int32_t GetFileName(const struct HksBlob *name, char **fileName)
{
    char *tmpName = (char *)HksMalloc(name->size + 1); /* \0 at the end */
    if (tmpName == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(tmpName, name->size, name->data, name->size);
    tmpName[name->size] = '\0';
    *fileName = tmpName;
    return HKS_SUCCESS;
}

static int32_t StoreRootMaterial(const struct HksBlob *name, const struct HksBlob *buffer)
{
    char *fileName = NULL;
    int32_t ret = GetFileName(name, &fileName);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    ret = HksFileWrite(HKS_KEY_STORE_PATH, fileName, 0, buffer->data, buffer->size);
    HKS_FREE_PTR(fileName);
    return ret;
}

static int32_t IsRootMaterialExist(const struct HksBlob *name)
{
    char *fileName = NULL;
    int32_t ret = GetFileName(name, &fileName);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    ret = HksIsFileExist(HKS_KEY_STORE_PATH, fileName);
    HKS_FREE_PTR(fileName);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("file not exist");
        return HKS_ERROR_NOT_EXIST;
    }
    return ret;
}

static int32_t GetRootMaterial(const struct HksBlob *name, struct HksBlob *buffer)
{
    char *fileName = NULL;
    int32_t ret = GetFileName(name, &fileName);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    uint32_t len = HksFileRead(HKS_KEY_STORE_PATH, fileName, 0, buffer->data, buffer->size);
    HKS_FREE_PTR(fileName);
    if (len == 0) {
        return HKS_ERROR_READ_FILE_FAIL;
    }
    return HKS_SUCCESS;
}

int32_t HksStoreKeyBlob(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    enum HksStorageType storageType, const struct HksBlob *keyBlob)
{
    (void)processInfo;
    if (storageType == HKS_STORAGE_TYPE_ROOT_KEY) {
        return StoreRootMaterial(keyAlias, keyBlob);
    }

    /* 1. check key exist or not */
    uint32_t offset = 0;
    int32_t ret = GetKeyOffsetByKeyAlias(keyAlias, &offset);
    if ((ret != HKS_SUCCESS) && (ret != HKS_ERROR_NOT_EXIST)) {
        return ret;
    }

    /* 2. store key blob */
    bool needDeleteKey = (ret == HKS_SUCCESS);
    ret = StoreKeyBlob(needDeleteKey, offset, keyBlob);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 3. write to file */
    uint32_t totalLen = 0;
    ret = HksStoreGetToatalSize(&totalLen);
    if (ret != HKS_SUCCESS) {
        return ret;
    }
    uint32_t fileOffset = HksGetStoreFileOffset();
    return HksFileWrite(HKS_KEY_STORE_PATH, HKS_KEY_STORE_FILE_NAME, fileOffset, g_storageImageBuffer.data, totalLen);
}

int32_t HksStoreDeleteKeyBlob(const struct HksProcessInfo *processInfo,
    const struct HksBlob *keyAlias, uint32_t storageType)
{
    (void)processInfo;
    (void)storageType;

    /* 1. check key exist or not */
    uint32_t offset = 0;
    int32_t ret = GetKeyOffsetByKeyAlias(keyAlias, &offset);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. calc tmp header hmac */
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)(g_storageImageBuffer.data + offset);
    struct HksStoreHeaderInfo newkeyInfoHead;
    if (memcpy_s(&newkeyInfoHead, sizeof(newkeyInfoHead), keyInfoHead, sizeof(*keyInfoHead)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    newkeyInfoHead.totalLen -= keyInfo->keyInfoLen;
    newkeyInfoHead.keyCount -= 1;

    ret = RefreshKeyInfoHeaderHmac(&newkeyInfoHead);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 3. delete key */
    ret = DeleteKey(offset);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 4. replace header */
    if (memcpy_s(keyInfoHead, sizeof(*keyInfoHead), &newkeyInfoHead, sizeof(newkeyInfoHead)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }

    uint32_t fileOffset = HksGetStoreFileOffset();
    return HksFileWrite(HKS_KEY_STORE_PATH, HKS_KEY_STORE_FILE_NAME, fileOffset,
        g_storageImageBuffer.data, keyInfoHead->totalLen);
}

int32_t HksStoreIsKeyBlobExist(const struct HksProcessInfo *processInfo,
    const struct HksBlob *keyAlias, uint32_t storageType)
{
    (void)processInfo;
    if (storageType == HKS_STORAGE_TYPE_ROOT_KEY) {
        return IsRootMaterialExist(keyAlias);
    }

    uint32_t offset = 0;
    return GetKeyOffsetByKeyAlias(keyAlias, &offset);
}

int32_t HksStoreGetKeyBlob(const struct HksProcessInfo *processInfo,
    const struct HksBlob *keyAlias, uint32_t storageType, struct HksBlob *keyBlob)
{
    (void)processInfo;
    if (storageType == HKS_STORAGE_TYPE_ROOT_KEY) {
        return GetRootMaterial(keyAlias, keyBlob);
    }

    uint32_t offset = 0;
    int32_t ret = GetKeyOffsetByKeyAlias(keyAlias, &offset);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* get offset success, len has been check valid */
    uint8_t *tmpBuf = g_storageImageBuffer.data + offset;
    struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)tmpBuf;

    keyBlob->data = (uint8_t *)HksMalloc(keyInfo->keyInfoLen); /* need be freed by caller functions */
    if (keyBlob->data == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    keyBlob->size = keyInfo->keyInfoLen;

    if (memcpy_s(keyBlob->data, keyBlob->size, tmpBuf, keyInfo->keyInfoLen) != EOK) {
        HKS_LOG_E("memcpy to key blob failed.");
        HKS_FREE_PTR(keyBlob->data);
        return HKS_ERROR_BAD_STATE;
    }

    return HKS_SUCCESS;
}

int32_t HksStoreGetKeyBlobSize(const struct HksBlob *processName,
    const struct HksBlob *keyAlias, uint32_t storageType, uint32_t *keyBlobSize)
{
    (void)processName;
    (void)storageType;

    uint32_t offset = 0;
    int32_t ret = GetKeyOffsetByKeyAlias(keyAlias, &offset);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* get offset success, len has been check valid */
    struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)(g_storageImageBuffer.data + offset);
    *keyBlobSize = keyInfo->keyInfoLen;
    return HKS_SUCCESS;
}

int32_t HksGetKeyCountByProcessName(const struct HksBlob *processName, uint32_t *keyCount)
{
    (void)processName;
    if (g_storageImageBuffer.size < sizeof(struct HksStoreHeaderInfo)) {
        HKS_LOG_E("invalid keyinfo buffer size %u.", g_storageImageBuffer.size);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    *keyCount = keyInfoHead->keyCount;
    return HKS_SUCCESS;
}

int32_t HksStoreGetToatalSize(uint32_t *size)
{
    if (g_storageImageBuffer.size < sizeof(struct HksStoreHeaderInfo)) {
        HKS_LOG_E("invalid keyinfo buffer size %u.", g_storageImageBuffer.size);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)g_storageImageBuffer.data;
    *size = keyInfoHead->totalLen;
    return HKS_SUCCESS;
}

static int32_t GetKeyInfoList(struct HksKeyInfo *keyInfoList, const struct HksBlob *keyInfoBlob)
{
    struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)keyInfoBlob->data;

    if (keyInfoList->alias.size < keyInfo->aliasSize) {
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }

    if (memcpy_s(keyInfoList->alias.data, keyInfoList->alias.size,
        keyInfoBlob->data + sizeof(*keyInfo), keyInfo->aliasSize) != EOK) {
        HKS_LOG_E("memcpy keyAlias failed");
        return HKS_ERROR_BAD_STATE;
    }
    keyInfoList->alias.size = keyInfo->aliasSize;

    struct HksParamSet *paramSet = NULL;
    int32_t ret = TranslateKeyInfoBlobToParamSet(NULL, keyInfoBlob, &paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    if (keyInfoList->paramSet->paramSetSize < paramSet->paramSetSize) {
        HksFreeParamSet(&paramSet);
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    if (memcpy_s(keyInfoList->paramSet, keyInfoList->paramSet->paramSetSize,
        paramSet, paramSet->paramSetSize) != EOK) {
        HKS_LOG_E("memcpy paramSet failed.");
        HksFreeParamSet(&paramSet);
        return HKS_ERROR_BAD_STATE;
    }

    HksFreeParamSet(&paramSet);
    return HKS_SUCCESS;
}

static int32_t GetAndCheckKeyCount(uint32_t *inputCount, uint32_t *keyCount)
{
    struct HksBlob storageBuf = HksGetImageBuffer();
    if (storageBuf.size < sizeof(struct HksStoreHeaderInfo)) {
        HKS_LOG_E("invalid keyinfo buffer size %u.", storageBuf.size);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)storageBuf.data;
    *keyCount = keyInfoHead->keyCount;
    if (*keyCount == 0) {
        *inputCount = 0;
        return HKS_SUCCESS;
    }

    if ((*keyCount > MAX_KEY_COUNT) || (storageBuf.size < keyInfoHead->totalLen)) {
        HKS_LOG_E("keyCount invalid, or storageBuf size invalid");
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    if (*inputCount < *keyCount) {
        HKS_LOG_E("listCount space not enough");
        return HKS_ERROR_BUFFER_TOO_SMALL;
    }
    return HKS_SUCCESS;
}

int32_t HksStoreGetKeyInfoList(struct HksKeyInfo *keyInfoList, uint32_t *listCount)
{
    uint32_t keyCount;
    int32_t ret = GetAndCheckKeyCount(listCount, &keyCount);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    /* 2. traverse ImageBuffer to search for keyAlias */
    struct HksBlob storageBuf = HksGetImageBuffer();
    struct HksStoreHeaderInfo *keyInfoHead = (struct HksStoreHeaderInfo *)storageBuf.data;
    uint32_t totalLen = keyInfoHead->totalLen;
    uint32_t num = 0;
    uint32_t offset = sizeof(*keyInfoHead);
    for (uint32_t i = 0; i < keyCount; ++i) {
        if ((totalLen < offset) || ((totalLen - offset) < sizeof(struct HksStoreKeyInfo))) {
            HKS_LOG_E("invalid keyinfo size.");
            return HKS_ERROR_INVALID_KEY_FILE;
        }

        uint8_t *tmpBuf = storageBuf.data + offset; /* storageBuf.size has been checked */
        struct HksStoreKeyInfo *keyInfo = (struct HksStoreKeyInfo *)tmpBuf;

        if (HksIsKeyInfoLenInvalid(keyInfo) || ((totalLen - offset) < keyInfo->keyInfoLen)) {
            HKS_LOG_E("invalid keyinfo len");
            return HKS_ERROR_INVALID_KEY_FILE;
        }

        struct HksBlob keyInfoBlob = { keyInfo->keyInfoLen, tmpBuf };
        ret = GetKeyInfoList(&keyInfoList[i], &keyInfoBlob);
        if (ret != HKS_SUCCESS) {
            return ret;
        }
        num++;
        offset += keyInfo->keyInfoLen;
    }

    *listCount = num;
    return HKS_SUCCESS;
}

int32_t HksStoreDestroy(const struct HksBlob *processName)
{
    (void)processName;
    int32_t ret = HksFileRemove(HKS_KEY_STORE_PATH, HKS_KEY_STORE_FILE_NAME);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("remove key store file failed"); /* only record log, continue delete */
    }
    ret = HksFileRemove(HKS_KEY_STORE_PATH, "info1.data");
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("remove info1 file failed"); /* only record log, continue delete */
    }
    ret = HksFileRemove(HKS_KEY_STORE_PATH, "info2.data");
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("remove info2 file failed");
    }
    return ret;
}
#endif /* _STORAGE_LITE_ */

#endif /* _CUT_AUTHENTICATE_ */

