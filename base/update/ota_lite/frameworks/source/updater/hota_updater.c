/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "hota_updater.h"

#include <stdbool.h>
#include <stdio.h>

#include <securec.h>

#include "hal_hota_board.h"
#include "hota_verify.h"
#include "parameter.h"

#define HASH_LENGTH 32
#define SIGN_DATA_LEN 640
#define COMP_VERSION_LENGTH 10
#define PKG_VERSION_LENGTH 64
#define PRODUCT_ID_LENGTH 64

#define OTA_MAX_PARTITION_NUM 10
#define COMPONENT_INFO_START 176
#define COMPONENT_INFO_TYPE_SIZE 2
#define COMPONENT_INFO_HEADER_LENGTH 4

#define MAX_BUFFER_SIZE 1500
#define MAX_TRANSPORT_BUFF_SIZE (4 * 1024)
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#endif

// Currently downloading component information
typedef struct {
    unsigned char isInfoComp; /* 1: information component,0: non-information component */
    unsigned int currentSize; /* Currently downloaded size of the component */
    unsigned int index;       /* order of this component  */
    unsigned int remainSize;  /* remain size of this component */
    unsigned int totalSize;   /* total size of this component */
    unsigned int offset;      /*  offset of this component */
} CurrentDloadComp;

#pragma pack(1)
typedef struct {
    unsigned short type;
    unsigned short length;
    unsigned int infoCompSize;
    unsigned int upgradePkgVersion;
    char productId[PRODUCT_ID_LENGTH];
    char version[PKG_VERSION_LENGTH];
} PkgBasicInfo;
#pragma pack()

#pragma pack(1)
typedef struct {
    unsigned char addr[PARTITION_NAME_LENGTH];  /* partition name */
    unsigned short id;                          /* component ID */
    unsigned char type;                         /* component type */
    unsigned char operType;                     /* component operation type. 0: need upgrade 1: need delete */
    unsigned char isDiff;                       /* Is Diff component */
    unsigned char version[COMP_VERSION_LENGTH]; /* version of component */
    unsigned int length;                        /* size of component. if it is diff component,
                                                this mean diff component image size. */
    unsigned int destLength;                    /* size of component. if it is diff component,
                                                this mean total component image size after diff reduction. */
    unsigned char shaData[HASH_LENGTH];
} ComponentInfo;
#pragma pack()

typedef struct {
    ComponentInfo table[OTA_MAX_PARTITION_NUM];
} ComponentInfos;

typedef struct {
    void (*ErrorCallBackFunc)(HotaErrorCode errorCode);
    void (*StatusCallBackFunc)(HotaStatus status);
} HotaNotifier;

static HotaStatus g_otaStatus = HOTA_NOT_INIT;
static unsigned short g_allComponentNum = 0;
static unsigned short g_allComponentSize = 0;
static unsigned short g_recvComponentNum = 0;
static unsigned short g_infoCompAndSignSize;        /* the size of Info Component */
static CurrentDloadComp g_currentDloadComp = { 0 }; /* Currently downloading component information */
static ComponentInfos g_componentInfos = { 0 };
static HotaNotifier g_otaNotifier = { 0 };    /* OTA Notifier, notify caller when error or status changed */
static unsigned int g_useDefaultPkgFlag = 1; /* Use default Pkg format flag. default use it. */
static unsigned char *g_infoCompBuff = NULL;        /* info component buffer */
static ComponentTableInfo *g_otaComponents = NULL;    /* ota partitions info */
static unsigned int g_signDataLen = 0;    /* sign data len */
static unsigned int g_signStartAddr = 0;  /* sign start address */


static void HotaResetStatus(void)
{
    g_otaStatus = HOTA_NOT_INIT;
    g_allComponentNum = 0;
    g_allComponentSize = 0;
    g_recvComponentNum = 0;
    g_infoCompAndSignSize = 0;
    (void)memset_s(&g_currentDloadComp, sizeof(g_currentDloadComp), 0, sizeof(g_currentDloadComp));
    (void)memset_s(&g_componentInfos, sizeof(g_componentInfos), 0, sizeof(g_componentInfos));
    (void)memset_s(&g_otaNotifier, sizeof(g_otaNotifier), 0, sizeof(g_otaNotifier));
    g_useDefaultPkgFlag = 1;
    g_infoCompBuff = NULL;
}

// refresh ota status
static void UpdateStatus(HotaStatus status)
{
    if (status == g_otaStatus) {
        return;
    }

    if (status == HOTA_CANCELED || status == HOTA_FAILED ||
        status == HOTA_TRANSPORT_ALL_DONE) {
        if (g_infoCompBuff != NULL) {
            free(g_infoCompBuff);
            g_infoCompBuff = NULL;
        }
    }

    UpdateMetaData data = {0};
    HotaHalGetMetaData(&data);
    g_otaStatus = status;
    data.otaStatus = status;
    HotaHalSetMetaData(&data);
    if (g_otaNotifier.StatusCallBackFunc != NULL) {
        g_otaNotifier.StatusCallBackFunc(status);
    }
}

static void ReportErrorCode(HotaErrorCode errorCode)
{
    if (g_otaNotifier.ErrorCallBackFunc != NULL) {
        g_otaNotifier.ErrorCallBackFunc(errorCode);
    }
}

static bool IsDigest(const char *str)
{
    const unsigned int len = strlen(str);
    for (unsigned int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }

    return true;
}

static bool IsLatestVersion(const char *pkgVersion, const char *currentVersion)
{
    char pkgVerCopy[PKG_VERSION_LENGTH] = {0};
    char currentVerCopy[PKG_VERSION_LENGTH] = {0};
    char *currentVerSplit = NULL;
    char *pkgVerSplit = NULL;
    char *currentVerTemp = NULL;
    char *pkgVerTemp = NULL;
    char split[] = ".| ";
    int ret = strcpy_s(pkgVerCopy, PKG_VERSION_LENGTH, pkgVersion);
    ret += strcpy_s(currentVerCopy, PKG_VERSION_LENGTH, currentVersion);
    if (ret != 0) {
        return false;
    }

    bool isLatest = true;
    currentVerSplit = strtok_s(currentVerCopy, split, &currentVerTemp);
    pkgVerSplit = strtok_s(pkgVerCopy, split, &pkgVerTemp);
    while (pkgVerSplit != NULL && currentVerSplit != NULL) {
        if (IsDigest(pkgVerSplit) && IsDigest(currentVerSplit)) {
            if (atoi(pkgVerSplit) < atoi(currentVerSplit)) {
                isLatest = false;
                break;
            } else if (atoi(pkgVerSplit) > atoi(currentVerSplit)) {
                isLatest = true;
                break;
            }
        } else if (strncmp(pkgVerSplit, currentVerSplit, PKG_VERSION_LENGTH) != EOK) {
            isLatest = false;
            break;
        }

        currentVerSplit = strtok_s(NULL, split, &currentVerTemp);
        pkgVerSplit = strtok_s(NULL, split, &pkgVerTemp);
    }

    if (isLatest && strncmp(currentVersion, pkgVersion, PKG_VERSION_LENGTH) == EOK) {
        isLatest = false;
    }

    if (!isLatest) {
        isLatest = HotaHalCheckVersionValid(currentVersion, pkgVersion, PKG_VERSION_LENGTH) ? true : false;
    }

    return isLatest;
}

static int CheckPkgVersionValid(const char *pkgVersion)
{
    if (pkgVersion == NULL) {
        return OHOS_FAILURE;
    }
    const char *currentVersion = GetIncrementalVersion();
    if (currentVersion == NULL) {
        return OHOS_FAILURE;
    }

    if (!IsLatestVersion(pkgVersion, currentVersion)) {
        printf("pkgVersion is valid\r\n");
        return OHOS_FAILURE;
    }
    return OHOS_SUCCESS;
}

static int ParseHotaInfoComponent(unsigned char *infoCompBuffer, unsigned int bufLen)
{
    if (infoCompBuffer == NULL || g_signDataLen == 0) {
        return OHOS_FAILURE;
    }

    if (bufLen <= SIGN_DATA_LEN) {
        printf("buffLen is invalid.\r\n");
        return OHOS_FAILURE;
    }

    unsigned char *infoHeaderBuf = (unsigned char *)malloc(bufLen - SIGN_DATA_LEN);
    if (infoHeaderBuf == NULL) {
        printf("malloc infoHeaderBuf failed.\r\n");
        return OHOS_FAILURE;
    }
    if (memcpy_s(infoHeaderBuf, bufLen - SIGN_DATA_LEN, infoCompBuffer, bufLen - SIGN_DATA_LEN) != EOK) {
        free(infoHeaderBuf);
        return OHOS_FAILURE;
    }

    if (HotaSignVerify(infoHeaderBuf, bufLen - SIGN_DATA_LEN,
        infoCompBuffer + (bufLen - g_signStartAddr), g_signDataLen)) {
        UpdateStatus(HOTA_FAILED);
        ReportErrorCode(HOTA_DATA_SIGN_CHECK_ERR);
        printf("Verify file failed.\r\n");
        free(infoHeaderBuf);
        return OHOS_FAILURE;
    }

    free(infoHeaderBuf);
    if (memcpy_s(&g_allComponentSize, sizeof(g_allComponentSize),
        infoCompBuffer + COMPONENT_INFO_START + COMPONENT_INFO_TYPE_SIZE, sizeof(g_allComponentSize)) != EOK) {
        return OHOS_FAILURE;
    }
    g_allComponentNum = g_allComponentSize / sizeof(ComponentInfo);
    if (g_allComponentNum > OTA_MAX_PARTITION_NUM) {
        return OHOS_FAILURE;
    }

    unsigned int pos = COMPONENT_INFO_START + COMPONENT_INFO_HEADER_LENGTH;
    for (unsigned int i = 0; i < g_allComponentNum; i++) {
        if (memcpy_s(g_componentInfos.table + i, sizeof(g_componentInfos.table[i]), infoCompBuffer + pos,
            sizeof(g_componentInfos.table[i])) != EOK) {
            return OHOS_FAILURE;
        }
        pos += sizeof(ComponentInfo);
    }
    if (HotaHalWrite(PARTITION_INFO_COMP, infoCompBuffer, 0, bufLen) != OHOS_SUCCESS) {
        printf("HotaHalWrite  failed, partition = PARTITION_INFO_COMP.\r\n");
        ReportErrorCode(HOTA_DATA_WRITE_ERR);
        UpdateStatus(HOTA_FAILED);
        return OHOS_FAILURE;
    }
    UpdateStatus(HOTA_TRANSPORT_INFO_DONE);
    return OHOS_SUCCESS;
}

static int InitDloadNextComp(unsigned int currentOffset)
{
    unsigned char tempIndex = g_currentDloadComp.index;
    if (memset_s(&g_currentDloadComp, sizeof(g_currentDloadComp), 0, sizeof(g_currentDloadComp)) != EOK) {
        UpdateStatus(HOTA_FAILED);
        printf("memset_s failed.\r\n");
        return OHOS_FAILURE;
    }
    g_currentDloadComp.isInfoComp = false;
    g_currentDloadComp.currentSize = 0;
    g_currentDloadComp.index = tempIndex + 1;
    g_currentDloadComp.offset = currentOffset;

    if (g_currentDloadComp.index - 1 >= OTA_MAX_PARTITION_NUM) {
        UpdateStatus(HOTA_FAILED);
        printf("the index of g_componentInfos.table is out of range.\r\n");
        return OHOS_FAILURE;
    }

    g_currentDloadComp.totalSize = g_componentInfos.table[g_currentDloadComp.index - 1].length;
    g_currentDloadComp.remainSize = g_currentDloadComp.totalSize;
    HotaHashInit();
    return OHOS_SUCCESS;
}

static int ParseHotaComponent(void)
{
    unsigned char hashOut[HASH_LENGTH] = {0};
    HotaGetHash(hashOut, HASH_LENGTH);

    if (g_currentDloadComp.index - 1 >= OTA_MAX_PARTITION_NUM) {
        UpdateStatus(HOTA_FAILED);
        printf("the index of g_componentInfos.table is out of range.\r\n");
        return OHOS_FAILURE;
    }

    // Check hash is or not same
    if (memcmp(hashOut, g_componentInfos.table[g_currentDloadComp.index - 1].shaData, HASH_LENGTH) != EOK) {
        UpdateStatus(HOTA_FAILED);
        ReportErrorCode(HOTA_DATA_VERIFY_HASH_ERR);
        printf("Verify hash failed. package maybe be damaged!\r\n");
        return OHOS_FAILURE;
    }

    g_recvComponentNum++;
    return OHOS_SUCCESS;
}

static bool HotaIsRejected(void)
{
    return (g_otaStatus == HOTA_CANCELED || g_otaStatus == HOTA_FAILED);
}

static int ProcessInfoCompHeader(const unsigned char *infoCompBuffer, unsigned int bufLen)
{
    if (infoCompBuffer == NULL) {
        return OHOS_FAILURE;
    }

    if (bufLen < sizeof(PkgBasicInfo)) {
        UpdateStatus(HOTA_FAILED);
        printf("buffLen is illegal.\r\n");
        return OHOS_FAILURE;
    }
    PkgBasicInfo basicInfo = { 0 };
    if (memcpy_s(&basicInfo, sizeof(PkgBasicInfo), infoCompBuffer, sizeof(PkgBasicInfo)) != EOK) {
        UpdateStatus(HOTA_FAILED);
        printf("memcpy_s basicInfo failed.\r\n");
        return OHOS_FAILURE;
    }

    if (basicInfo.infoCompSize < sizeof(PkgBasicInfo)) {
        return OHOS_FAILURE;
    }

    // version check, only allow upgrade with higher version.
    if (CheckPkgVersionValid(basicInfo.version) != OHOS_SUCCESS) {
        UpdateStatus(HOTA_FAILED);
        ReportErrorCode(HOTA_VERSION_INVALID);
        printf("CheckPkgVersionValid failed. Pkg version: %s\r\n", basicInfo.version);
        return OHOS_FAILURE;
    }

    if (basicInfo.type == SIGN_ARITHMETIC_RSA2048) {
        g_signDataLen = SIGN_RSA2048_LEN;
        g_signStartAddr = SIGN_DATA_LEN;
    } else if (basicInfo.type == SIGN_ARITHMETIC_RSA3072) {
        g_signDataLen = SIGN_RSA3072_LEN;
        g_signStartAddr = SIGN_RSA3072_LEN;
    }
    g_infoCompAndSignSize = basicInfo.infoCompSize + SIGN_DATA_LEN;
    g_currentDloadComp.isInfoComp = true;
    g_currentDloadComp.offset = 0;
    g_currentDloadComp.index = 0;
    g_currentDloadComp.currentSize = sizeof(PkgBasicInfo);
    g_currentDloadComp.totalSize = g_infoCompAndSignSize;
    if (g_infoCompAndSignSize < sizeof(PkgBasicInfo)) {
        return OHOS_FAILURE;
    }
    g_currentDloadComp.remainSize = g_infoCompAndSignSize - sizeof(PkgBasicInfo);
    return OHOS_SUCCESS;
}

static bool DloadIsDone(void)
{
    if ((g_allComponentNum != 0) && (g_recvComponentNum >= g_allComponentNum) &&
        (!g_currentDloadComp.isInfoComp)) {
        printf("OTA dload file success.\r\n");
        return true;
    }

    return false;
}

static int ProcessOneComponent(void)
{
    int result;
    if (g_currentDloadComp.isInfoComp) {
        result = ParseHotaInfoComponent(g_infoCompBuff, g_infoCompAndSignSize);
    } else {
        result = ParseHotaComponent();
    }

    if (result != OHOS_SUCCESS) {
        return result;
    }
    // download OK
    if (DloadIsDone()) {
        // done
        UpdateStatus(HOTA_TRANSPORT_ALL_DONE);
        HotaResetStatus();
    }

    return OHOS_SUCCESS;
}

static int CopyToDloadCompBuffer(const unsigned char *buffer, unsigned int buffSize)
{
    if (g_currentDloadComp.currentSize + buffSize > MAX_BUFFER_SIZE) {
        UpdateStatus(HOTA_FAILED);
        printf("Size is out of range.\r\n");
        return OHOS_FAILURE;
    }
    if (memcpy_s(g_infoCompBuff + g_currentDloadComp.currentSize, MAX_BUFFER_SIZE - g_currentDloadComp.currentSize,
        buffer, buffSize) != EOK) {
        UpdateStatus(HOTA_FAILED);
        ReportErrorCode(HOTA_DATA_COPY_TO_BUFFER_ERR);
        printf("memcpy_s g_infoCompBuff failed.\r\n");
        return OHOS_FAILURE;
    }

    return OHOS_SUCCESS;
}

static void GetCurrentDloadCompPartition(int *partition)
{
    if (g_currentDloadComp.index - 1 >= OTA_MAX_PARTITION_NUM) {
        UpdateStatus(HOTA_FAILED);
        printf("the index of g_componentInfos.table is out of range.\r\n");
        return;
    }

    for (unsigned int i = 0; g_otaComponents[i].imgPath != NULL; i++) {
        if (strncmp((const char *)(g_otaComponents[i].componentName),
            (const char *)g_componentInfos.table[g_currentDloadComp.index - 1].addr,
            PARTITION_NAME_LENGTH) == 0) {
            *partition = g_otaComponents[i].id;
            return;
        }
    }

    return;
}

// Stash buffer to memory, or storage to HAL.
static int StashRecvDataToBuffer(unsigned char *buffer, unsigned int startAddr, unsigned int endAddr)
{
    if (g_currentDloadComp.isInfoComp) {
        if (CopyToDloadCompBuffer(buffer, (endAddr - startAddr)) != OHOS_SUCCESS) {
            printf("StashRecvDataToBuffer HotaHalWrite failed,\r\n");
            return OHOS_FAILURE;
        }
    } else {
        int partition = PARTITION_ERROR;
        GetCurrentDloadCompPartition(&partition);
        if (HotaHalWrite(partition, buffer, startAddr - g_currentDloadComp.offset,
            endAddr - startAddr) != OHOS_SUCCESS) {
            printf("StashRecvDataToBuffer HotaHalWrite failed, partition = %d .\r\n", partition);
            ReportErrorCode(HOTA_DATA_WRITE_ERR);
            UpdateStatus(HOTA_FAILED);
            return OHOS_FAILURE;
        }
        // calc Hash
        HotaHashCalc(buffer, endAddr - startAddr);
    }

    g_currentDloadComp.remainSize -= (endAddr - startAddr);
    g_currentDloadComp.currentSize += (endAddr - startAddr);
    return OHOS_SUCCESS;
}

static int ProcessCompData(unsigned char *buffer, unsigned int startAddr, unsigned int endAddr)
{
    if (buffer == NULL || endAddr < startAddr) {
        return OHOS_FAILURE;
    }

    // Current Component download is not completed, so stash to buffer.
    if ((g_currentDloadComp.remainSize > 0) && (StashRecvDataToBuffer(buffer, startAddr, endAddr)) != OHOS_SUCCESS) {
        return OHOS_FAILURE;
    }

    // Download one component OK and init g_currentDloadComp order to download next component.
    if (g_currentDloadComp.remainSize == 0) {
        if (ProcessOneComponent() != OHOS_SUCCESS) {
            UpdateStatus(HOTA_FAILED);
            return OHOS_FAILURE;
        }
        return InitDloadNextComp(endAddr);
    }

    return OHOS_SUCCESS;
}

static int HotaDefaultWrite(unsigned char *buffer, unsigned int offset, unsigned int buffSize)
{
    if ((buffer == NULL) || (buffSize == 0) || (buffSize > MAX_TRANSPORT_BUFF_SIZE)) {
        printf("Param is illegal. buffer is NULL or buffSize too big. buffsize = %u.\r\n", buffSize);
        return OHOS_FAILURE;
    }

    if (HotaIsRejected()) {
        printf("Hota is canceled or verify failed.\r\n");
        return OHOS_FAILURE;
    }

    unsigned char *tempBuffer = buffer;
    unsigned int tempOffset = offset;
    unsigned int tempBuffSize = buffSize;
    do {
        if (tempOffset < sizeof(PkgBasicInfo) && tempOffset + tempBuffSize >= sizeof(PkgBasicInfo)) {
            if (CopyToDloadCompBuffer(tempBuffer, (sizeof(PkgBasicInfo) - tempOffset)) != OHOS_SUCCESS) {
                return OHOS_FAILURE;
            }
            g_currentDloadComp.currentSize += (sizeof(PkgBasicInfo) - tempOffset);
            if (ProcessInfoCompHeader(g_infoCompBuff, g_currentDloadComp.currentSize) != OHOS_SUCCESS) {
                return OHOS_FAILURE;
            }
            tempBuffer = tempBuffer + sizeof(PkgBasicInfo) - tempOffset;
            tempBuffSize -= (sizeof(PkgBasicInfo) - tempOffset);
            tempOffset = sizeof(PkgBasicInfo);
        } else if (tempOffset + tempBuffSize < sizeof(PkgBasicInfo)) {
            if (CopyToDloadCompBuffer(tempBuffer, tempBuffSize) != OHOS_SUCCESS) {
                return OHOS_FAILURE;
            }
            g_currentDloadComp.currentSize += tempBuffSize;
            break;
        } else {
            unsigned int startAddr = (tempOffset > g_currentDloadComp.offset) ? tempOffset : g_currentDloadComp.offset;
            unsigned int endAddr =
                (g_currentDloadComp.offset + g_currentDloadComp.totalSize) < (tempOffset + tempBuffSize) ?
                (g_currentDloadComp.offset + g_currentDloadComp.totalSize) :
                (tempOffset + tempBuffSize);

            if (ProcessCompData(tempBuffer, startAddr, endAddr) != OHOS_SUCCESS) {
                return OHOS_FAILURE;
            }
            tempBuffSize -= (endAddr - tempOffset);
            tempBuffer = tempBuffer + endAddr - tempOffset;
            tempOffset = endAddr;
        }
    } while (tempBuffSize > 0);
    return OHOS_SUCCESS;
}

int HotaSetPackageType(unsigned int flag)
{
    if (flag != USE_DEFAULT_PKG && flag != NOT_USE_DEFAULT_PKG) {
        printf("flag is invalid. value = %u\r\n", flag);
        return OHOS_FAILURE;
    }

    g_useDefaultPkgFlag = flag;
    return OHOS_SUCCESS;
}

int HotaGetUpdateIndex(unsigned int *index)
{
    if (index == NULL) {
        printf("index == NULL\r\n");
        return OHOS_FAILURE;
    }
    return HotaHalGetUpdateIndex(index);
}

int HotaInit(ErrorCallBackFunc errorCallback, StatusCallBackFunc statusCallback)
{
    if (errorCallback != NULL) {
        g_otaNotifier.ErrorCallBackFunc = errorCallback;
    }
    if (statusCallback != NULL) {
        g_otaNotifier.StatusCallBackFunc = statusCallback;
    }

    int result = HotaHalInit();
    if (result != OHOS_SUCCESS) {
        return result;
    }

    g_infoCompBuff = (unsigned char *)malloc(MAX_BUFFER_SIZE);
    if (g_infoCompBuff == NULL) {
        return OHOS_FAILURE;
    }

    g_otaComponents = (ComponentTableInfo *)HotaHalGetPartitionInfo();

    UpdateStatus(HOTA_INITED);
    return OHOS_SUCCESS;
}

int HotaRead(unsigned int offset, unsigned int bufLen, unsigned char *buf)
{
    if (buf == NULL) {
        printf("HotaRead failed. buf == NULL\r\n");
        return OHOS_FAILURE;
    }

    if (g_useDefaultPkgFlag == NOT_USE_DEFAULT_PKG) {
        return HotaHalRead(0, offset, bufLen, buf);
    }

    printf("UseOHOSPkgFlag is not open!");
    return OHOS_FAILURE;
}

int HotaWrite(unsigned char *buffer, unsigned int offset, unsigned int buffSize)
{
    if (g_useDefaultPkgFlag == NOT_USE_DEFAULT_PKG) {
        return HotaHalWrite(0, buffer, offset, buffSize);
    }

    return HotaDefaultWrite(buffer, offset, buffSize);
}

int HotaCancel(void)
{
    UpdateStatus(HOTA_CANCELED);
    HotaResetStatus();
    return HotaHalDeInit();
}

int HotaRestart(void)
{
    if (g_useDefaultPkgFlag == NOT_USE_DEFAULT_PKG) {
        return HotaHalRestart();
    }
    UpdateMetaData data = {0};
    HotaHalGetMetaData(&data);
    if (data.otaStatus != HOTA_TRANSPORT_ALL_DONE) {
        printf("HotaRestart, failed. ota is canceled or verify failed.\r\n");
        return OHOS_FAILURE;
    }

    printf("HotaRestart run!\r\n");
    return HotaHalRestart();
}

int HotaSetBootSettings(void)
{
    if (g_useDefaultPkgFlag == NOT_USE_DEFAULT_PKG) {
        return HotaHalSetBootSettings();
    }

    UpdateMetaData data = {0};
    HotaHalGetMetaData(&data);
    if (data.otaStatus != HOTA_TRANSPORT_ALL_DONE) {
        printf("HotaSetBootSettings, failed. ota is canceled or verify failed.\r\n");
        return OHOS_FAILURE;
    }

    printf("HotaSetBootSettings run!\r\n");
    return HotaHalSetBootSettings();
}
int HotaGetUpdateAbility(void)
{
    return HotaHalGetUpdateAbility();
}

int HotaGetOtaPkgPath(char *path, int len)
{
    return HotaHalGetOtaPkgPath(path, len);
}

int HotaIsDeviceCanReboot(void)
{
    return HotaHalIsDeviceCanReboot();
}

int HotaIsDevelopMode(void)
{
    return HotaHalIsDevelopMode();
}

int HotaGetUpdateStatus(void)
{
    UpdateMetaData data = {0};
    HotaHalGetMetaData(&data);
    return data.updateStatus ? 1 : 0;
}

int HotaRebootAndCleanUserData(void)
{
    return HotaHalRebootAndCleanUserData();
}

int HotaRebootAndCleanCache(void)
{
    return HotaHalRebootAndCleanCache();
}
