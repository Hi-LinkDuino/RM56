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

#ifdef HKS_CONFIG_FILE
#include HKS_CONFIG_FILE
#else
#include "hks_config.h"
#endif

#ifdef HKS_SUPPORT_UPGRADE_STORAGE_DATA

#include "hks_upgrade_storage_data.h"

#include "hks_crypto_hal.h"
#include "hks_file_operator.h"
#include "hks_get_process_info.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_storage.h"

#include "huks_access.h"

#define HKS_SEALING_NONCE_SIZE 16
#define HKS_HEADER_HASH_SIZE 64
#define HKS_KEY_PAIR_CIPHER_ED25519 80

#define HKS_RKC_KSF_BUF_LEN 258
#define HKS_FILE_OFFSET_BASE 0
#if defined(_HILINK_PRODUCT_)
#define HKS_LOCAL_STORAGE_KEY_MAX_NUM 15
#define HKS_BUF_BYTES 4096
#else
#define HKS_LOCAL_STORAGE_KEY_MAX_NUM 20
#define HKS_BUF_BYTES 5120
#endif

#define HKS_KEY_STORE_FILE_NAME "hks_keystore"
#define HKS_ROOT_MATERIAL_FILE_NAME "info1.data"
#define HKS_BAK_ROOT_MATERIAL_FILE_NAME "info2.data"
#define HKS_ROOT_MATERIAL_FILE_NAME_LEN 10

static struct HksBlob g_rootKeyMaterialFile[] = {
    { HKS_ROOT_MATERIAL_FILE_NAME_LEN, (uint8_t *)HKS_ROOT_MATERIAL_FILE_NAME },
    { HKS_ROOT_MATERIAL_FILE_NAME_LEN, (uint8_t *)HKS_BAK_ROOT_MATERIAL_FILE_NAME },
};

/*
 *         +----------------------------------------------------------------------+
 * Header: | version | keyCount | slotCount | sealingAlg | sealingNonce | Hash    |
 *         | 2bytes  | 1bytes   | 1bytes    | 4bytes     | 16bytes      | 64bytes |
 *         +----------------------------------------------------------------------+
*/
static uint32_t GetHeaderLen(void)
{
    return (sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint32_t) +
        HKS_SEALING_NONCE_SIZE + HKS_HEADER_HASH_SIZE);
}

/*
 *          +------------------------------------------------------------------------------------------+
 * KeyInfo: | flags   | sealingNonce | aliasSize | keyAlias   | keyType    | keyLen     | usage        |
 *          | 1bytes  | 16bytes      | 1bytes    | max64bytes | 4bytes     | 2bytes     | 4bytes       |
 *          +------------------------------------------------------------------------------------------+
 *          | padding | keyMode      | key_role  | domain     | authIdType | authIdSize | auth_id_data |
 *          | 1bytes  | 4bytes       | 4bytes    | 2bytes     | 1bytes     | 1bytes     | max64bytes   |
 *          +------------------------------------------------------------------------------------------+
 *          | keySize | key          |
 *          | 1bytes  | max80bytes   |
 *          +------------------------+
*/
static uint32_t GetKeySlotLen(void)
{
    return (sizeof(uint8_t) + HKS_SEALING_NONCE_SIZE + sizeof(uint8_t) + HKS_MAX_KEY_ALIAS_LEN +
        (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) +
        sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + HKS_MAX_KEY_AUTH_ID_LEN) +
        sizeof(uint8_t) + HKS_KEY_PAIR_CIPHER_ED25519);
}

uint32_t HksGetStoreFileOffset(void)
{
#if defined(_HILINK_PRODUCT_)
    return HKS_RKC_KSF_BUF_LEN;
#else
    return HKS_FILE_OFFSET_BASE;
#endif
}

static int32_t CopyRootKeyMaterialFile(bool isFileExist, const char *oldFileName, const struct HksBlob *newFile)
{
    if (!isFileExist) {
        return HKS_SUCCESS;
    }

    char *processName = NULL;
    int32_t ret = HksGetProcessName(&processName);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };

    int32_t fileSize = HksOldVersionFileSize(oldFileName);
    if (fileSize < HKS_RKC_KSF_BUF_LEN) {
        HKS_LOG_E("get old version file size failed, ret = %d", fileSize);
        return HKS_ERROR_FILE_SIZE_FAIL;
    }

    uint8_t *tmpBuf = (uint8_t *)HksMalloc(fileSize);
    if (tmpBuf == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    do {
        ret = HksOldVersionFileRead(oldFileName, 0, tmpBuf, fileSize);
        if (ret <= 0) {
            HKS_LOG_E("read old version file failed");
            ret = HKS_ERROR_READ_FILE_FAIL;
            break;
        }

        struct HksBlob newFileBlob = { fileSize, tmpBuf };
        ret = HksStoreKeyBlob(&processNameBlob, newFile, HKS_STORAGE_TYPE_ROOT_KEY, &newFileBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("copy to new file failed");
            break;
        }
    } while (0);

    HKS_FREE_PTR(tmpBuf);
    return ret;
}

static int32_t RollBackNewVersionFiles(void)
{
    char *processName = NULL;
    int32_t ret = HksGetProcessName(&processName);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed, ret = %d", ret);
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };

    ret = HksStoreDestroy(&processNameBlob);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_D("destroy new files failed");
    }
    return ret;
}

static int32_t RemoveOldVersionFiles(void)
{
    int32_t ret = HksOldVersionFileRemove(HKS_KEY_STORE_FILE_NAME);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("remove key store file failed, ret = %d", ret); /* continue delete */
    }
#ifndef _HARDWARE_ROOT_KEY_
    ret = HksOldVersionFileRemove(HKS_ROOT_MATERIAL_FILE_NAME);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("remove root key material file failed, ret = %d", ret); /* continue delete */
    }

    ret = HksOldVersionFileRemove(HKS_BAK_ROOT_MATERIAL_FILE_NAME);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("remove backup root key material file failed, ret = %d", ret);
    }
#endif
    return ret;
}

static int32_t UpgradeRootKeyMaterialFile(void)
{
    char *processName = NULL;
    if (HksGetProcessName(&processName) != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };

    /* check if new root key material file exist, if exist, return error, no need update hks_keystore file */
    for (uint32_t i = 0; i < HKS_ARRAY_SIZE(g_rootKeyMaterialFile); ++i) {
        int32_t ret = HksStoreIsKeyBlobExist(&processNameBlob, &g_rootKeyMaterialFile[i], HKS_STORAGE_TYPE_ROOT_KEY);
        if (ret != HKS_ERROR_NOT_EXIST) {
            HKS_LOG_D("exist new root key material file, exit upgrading process.");
            (void)RemoveOldVersionFiles(); /* new version file exist, remove old version files */
            return HKS_ERROR_NEW_ROOT_KEY_MATERIAL_EXIST;
        }
    }

    /* check old root key material files */
    bool isMainFileExist = HksOldVersionIsFileExist(HKS_ROOT_MATERIAL_FILE_NAME);
    bool isBakFileExist = HksOldVersionIsFileExist(HKS_BAK_ROOT_MATERIAL_FILE_NAME);
    if (!isMainFileExist && !isBakFileExist) {
        HKS_LOG_E("old root key material file not exist.");
        return HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL;
    }

    int32_t copyMainRet = CopyRootKeyMaterialFile(isMainFileExist, HKS_ROOT_MATERIAL_FILE_NAME,
        &g_rootKeyMaterialFile[0]);
    int32_t copyBakRet = CopyRootKeyMaterialFile(isBakFileExist, HKS_BAK_ROOT_MATERIAL_FILE_NAME,
        &g_rootKeyMaterialFile[1]);
    if ((copyMainRet != HKS_SUCCESS) && (copyBakRet != HKS_SUCCESS)) {
        HKS_LOG_E("copy old key materail file to new file failed.");
        return HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL;
    }

    return HKS_SUCCESS;
}

static int32_t GetOldVersionKeyStoreBuf(struct HksBlob *keyInfo)
{
    uint32_t offset = HksGetStoreFileOffset();
    int32_t fileSize = HksOldVersionFileSize(HKS_KEY_STORE_FILE_NAME);
    if ((fileSize < 0) || ((uint32_t)fileSize < offset)) {
        HKS_LOG_E("get old version file size or invalid file size failed");
        return HKS_ERROR_FILE_SIZE_FAIL;
    }

    uint8_t *tmpBuf = (uint8_t *)HksMalloc(HKS_BUF_BYTES);
    if (tmpBuf == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    fileSize = HksOldVersionFileRead(HKS_KEY_STORE_FILE_NAME, offset, tmpBuf, HKS_BUF_BYTES - offset);
    if ((fileSize <= 0) || ((uint32_t)fileSize < (HKS_BUF_BYTES - offset))) {
        HKS_LOG_E("read key store file failed");
        HKS_FREE_PTR(tmpBuf);
        return HKS_ERROR_READ_FILE_FAIL;
    }

    keyInfo->size = fileSize;
    keyInfo->data = tmpBuf;
    return HKS_SUCCESS;
}

static int32_t CheckKeyStoreHeaderValid(const struct HksBlob *keyInfo)
{
    uint32_t headerLen = GetHeaderLen();
    /* keyInfo->size has been checked lager than header len */
    struct HksBlob srcData = { headerLen - HKS_HEADER_HASH_SIZE, keyInfo->data };

    uint8_t *tmpBuf = (uint8_t *)HksMalloc(HKS_HEADER_HASH_SIZE);
    if (tmpBuf == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }
    struct HksBlob hashBlob = { HKS_HEADER_HASH_SIZE, tmpBuf };

    int32_t ret;
    do {
        ret = HksCryptoHalHash(HKS_DIGEST_SHA512, &srcData, &hashBlob);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("hks calculate header hash failed, ret = %d", ret);
            break;
        }

        if ((hashBlob.size != HKS_HEADER_HASH_SIZE) ||
            (HksMemCmp(hashBlob.data, keyInfo->data + srcData.size, HKS_HEADER_HASH_SIZE) != 0)) {
            HKS_LOG_E("hks header hash invalid");
            ret = HKS_ERROR_INVALID_KEY_FILE;
            break;
        }
    } while (0);

    HKS_FREE_BLOB(hashBlob);
    return ret;
}

static uint8_t GetSlotCountLoop(const struct HksBlob *keyInfo)
{
    uint32_t slotCountPos = sizeof(uint16_t) + sizeof(uint8_t); /* offset version and keyCount */
    uint8_t slotCount = *(keyInfo->data + slotCountPos); /* keyInfo size has been checked */

    return (slotCount < HKS_LOCAL_STORAGE_KEY_MAX_NUM) ? slotCount : HKS_LOCAL_STORAGE_KEY_MAX_NUM;
}

static int32_t GetKeyAlias(const struct HksBlob *keyInfo, struct HksBlob *keyAlias)
{
    /* keyInfo size has been checked */
    uint32_t aliasSizePos = sizeof(uint8_t) + HKS_SEALING_NONCE_SIZE;
    uint8_t aliasSize = *(keyInfo->data + aliasSizePos);
    uint32_t aliasPos = aliasSizePos + sizeof(uint8_t);

    if ((aliasSize == 0) || (aliasSize > HKS_MAX_KEY_ALIAS_LEN)) {
        HKS_LOG_E("invalid key alias size, size = %u", aliasSize);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    uint8_t *tmpBuf = (uint8_t *)HksMalloc(aliasSize);
    if (tmpBuf == NULL) {
        HKS_LOG_E("malloc failed");
        return HKS_ERROR_MALLOC_FAIL;
    }

    (void)memcpy_s(tmpBuf, aliasSize, keyInfo->data + aliasPos, aliasSize);
    keyAlias->size = aliasSize;
    keyAlias->data = tmpBuf;
    return HKS_SUCCESS;
}

static int32_t UpgradeSingleKeyInfo(const struct HksBlob *keyInfo)
{
    char *processName = NULL;
    int32_t ret = HksGetProcessName(&processName);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get process name failed");
        return HKS_ERROR_INTERNAL_ERROR;
    }
    struct HksBlob processNameBlob = { strlen(processName), (uint8_t *)processName };

    struct HksBlob keyAlias = { 0, NULL };
    ret = GetKeyAlias(keyInfo, &keyAlias);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get key alias failed, ret = %d", ret);
        return ret;
    }

    struct HksBlob output = { MAX_KEY_SIZE, NULL };
    do {
        output.data = (uint8_t *)HksMalloc(output.size);
        if (output.data == NULL) {
            HKS_LOG_E("malloc failed");
            ret = HKS_ERROR_MALLOC_FAIL;
            break;
        }

        ret = HuksAccessUpgradeKeyInfo(&keyAlias, keyInfo, &output);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("access update key info faild");
            break;
        }

        ret = HksStoreKeyBlob(&processNameBlob, &keyAlias, HKS_STORAGE_TYPE_KEY, &output);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("store keyblob to storage failed, ret = %d", ret);
            break;
        }
    } while (0);

    HKS_FREE_BLOB(keyAlias);
    HKS_FREE_BLOB(output);
    return ret;
}

static bool IsBufferAllZero(uint8_t *buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        if (buf[i] != 0) {
            return false;
        }
    }
    return true;
}

static int32_t UpgradeKeyStoreInfo(const struct HksBlob *keyInfo)
{
    uint8_t slotCountLoop = GetSlotCountLoop(keyInfo);
    uint32_t slotShift = GetKeySlotLen();
    uint8_t *tmp = keyInfo->data + GetHeaderLen(); /* keyInfo size has been checked lager than header len */
    int32_t ret = HKS_SUCCESS;

    /* Ensure that the number of cycles is within the range of keyinfo size */
    for (uint8_t i = 0; i < slotCountLoop; ++i) {
        if (IsBufferAllZero(tmp, slotShift)) { /* current key slot all zero, no need update */
            tmp += slotShift;
            continue;
        }

        struct HksBlob key = { slotShift, tmp };
        ret = UpgradeSingleKeyInfo(&key);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("update the [%u]st key failed, ret = %d", i, ret);
            break;
        }
        tmp += slotShift;
    }

    return ret;
}

static int32_t UpgradeKeyStoreFiles(void)
{
    struct HksBlob keyInfo = { 0, NULL };
    int32_t ret = GetOldVersionKeyStoreBuf(&keyInfo);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get old version key info failed");
        return ret;
    }

    do {
        ret = CheckKeyStoreHeaderValid(&keyInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("key store file header invalid, ret = %d", ret);
            break;
        }

        ret = UpgradeKeyStoreInfo(&keyInfo);
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("update key store info failed, ret = %d", ret);
            break;
        }
    } while (0);

    HKS_FREE_BLOB(keyInfo);
    return ret;
}

int32_t HksUpgradeStorageData(void)
{
    bool isOldKeyStoreFileExist = HksOldVersionIsFileExist(HKS_KEY_STORE_FILE_NAME);
    if (!isOldKeyStoreFileExist) {
        HKS_LOG_D("old key store file not exist, no need upgrading process.");
        return HKS_SUCCESS;
    }

    int32_t ret;
#ifndef _HARDWARE_ROOT_KEY_
    ret = UpgradeRootKeyMaterialFile();
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("update root key material file failed, ret = %d", ret);
        return (ret == HKS_ERROR_UPDATE_ROOT_KEY_MATERIAL_FAIL) ? ret : HKS_SUCCESS;
    }
#endif

    do {
#ifndef _HARDWARE_ROOT_KEY_
        /* init rkc and update key store files, if process failed, need roolback all new files */
        ret = HuksAccessModuleInit();
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("init rkc failed, ret = %d", ret);
            break;
        }
#endif
#ifdef _STORAGE_LITE_
        ret = HksLoadFileToBuffer();
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("load file to buffer failed, ret = %d", ret);
            break;
        }
#endif
        ret = UpgradeKeyStoreFiles();
        if (ret != HKS_SUCCESS) {
            HKS_LOG_E("update key store files failed, ret = %d", ret);
            break;
        }
        HKS_LOG_I("upgrading key info files success");

        if (RemoveOldVersionFiles() != HKS_SUCCESS) {
            HKS_LOG_E("remove old key store files failed, ret = %d", ret); /* add log, no proccess error code */
        }
    } while (0);

    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("update storage data failed, need delete new files, ret = %d", ret);
        if (RollBackNewVersionFiles() != HKS_SUCCESS) {
            HKS_LOG_D("rollback new files failed"); /* add log, no proccess error code */
        }
    }
    return ret;
}

int32_t HksDestroyOldVersionFiles(void)
{
    return RemoveOldVersionFiles();
}
#endif /* HKS_SUPPORT_UPGRADE_STORAGE_DATA */
#endif /* _CUT_AUTHENTICATE_ */