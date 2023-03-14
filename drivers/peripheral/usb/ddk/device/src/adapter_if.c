/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adapter_if.h"
#include <dirent.h>
#include <endian.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "osal_time.h"

#define HDF_LOG_TAG adapter_if
#define SLEEP_100MS 100000
#define DELAY_100MS 100
#define OPEN_CNT 30

static struct RawUsbRamTestList *g_usbRamTestHead = NULL;
static bool g_usbRamTestFlag = false;

static bool IsDirExist(const char *path)
{
    DIR *dir = NULL;
    if (path == NULL) {
        return false;
    }
    
    dir= opendir(path);
    if (dir == NULL) {
        HDF_LOGE("%{public}s: %{public}d: opendir failed!, path is %{public}s, errno is %{public}d",
            __func__,  __LINE__, path, errno);
        return false;
    }
    closedir(dir);
    return true;
}

static bool IsDir(const char *path)
{
    struct stat statBuf;
    if (lstat(path, &statBuf) == 0) {
        return S_ISDIR(statBuf.st_mode) != 0;
    }
    return false;
}

static void GetFilePath(const char *path, const char *fileName, char *filePath)
{
    int32_t ret = strcpy_s(filePath, MAX_PATHLEN - 1, path);
    if (ret != EOK) {
        HDF_LOGE("%s: strcpy_s failure!", __func__);
        return;
    }

    if (filePath[strlen(path) - 1] != '/') {
        ret = strcat_s(filePath, MAX_PATHLEN - 1, "/");
        if (ret != EOK) {
            HDF_LOGE("%s: strcat_s failure!", __func__);
            return;
        }
    }

    ret = strcat_s(filePath, MAX_PATHLEN - 1, fileName);
    if (ret != EOK) {
        HDF_LOGE("%s: strcat_s failure!", __func__);
    }
}

static bool IsSpecialDir(const char *path)
{
    return (strcmp(path, ".") == 0) || strcmp(path, "..") == 0;
}

static void DeleteFile(const char *path)
{
    DIR *dir = NULL;
    struct dirent *dirInfo = NULL;
    char filePath[PATH_MAX];

    if (IsDir(path)) {
        if ((dir = opendir(path)) == NULL) {
            return;
        }
        while ((dirInfo = readdir(dir)) != NULL) {
            GetFilePath(path, dirInfo->d_name, filePath);
            if (IsSpecialDir(dirInfo->d_name)) {
                continue;
            }
            closedir(dir);
            DeleteFile(filePath);
            remove(filePath);
        }
    }
}

static bool IsDeviceDirExist(const char *deviceName)
{
    char tmp[MAX_PATHLEN] = {0};
    int32_t ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return false;
    }

    return IsDirExist(tmp);
}

static int32_t UsbFnWriteFile(const char *path, const char *str)
{
    int32_t ret;
    if (strlen(str) == 0) {
        return 0;
    }
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        HDF_LOGE("%s: UsbFnWriteFile failure!", __func__);
        return HDF_ERR_BAD_FD;
    }
    ret = fwrite(str, strlen(str), 1, fp);
    if (ret != 1) {
        if (fclose(fp)) {
            return HDF_FAILURE;
        }
        return HDF_FAILURE;
    }
    if (fclose(fp)) {
        return HDF_FAILURE;
    }
    return 0;
}

static int32_t UsbFnWriteProp(const char *deviceName, const char *propName, uint32_t propValue)
{
    char tmp[MAX_PATHLEN] = {0};
    char tmpVal[MAX_NAMELEN] = {0};
    int32_t ret;
    if (deviceName == NULL || propName == NULL) {
        HDF_LOGE("%s: INVALID PARAM!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/%s",
        CONFIGFS_DIR, deviceName, propName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }

    ret = snprintf_s(tmpVal, MAX_NAMELEN, MAX_NAMELEN - 1, "0x%x", propValue);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }

    return UsbFnWriteFile(tmp, tmpVal);
}

static int32_t UsbFnWriteConfString(const char *deviceName,
    int32_t configVal, uint16_t lang, const char *stringValue)
{
    int32_t ret;
    char tmp[MAX_PATHLEN] = {0};
    char tmpPath[MAX_PATHLEN] = {0};
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/configs/b.%d/strings/0x%x",
                     CONFIGFS_DIR, deviceName, configVal, lang);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }

    if (!IsDirExist(tmp)) {
        ret = mkdir(tmp, S_IREAD | S_IWRITE);
        if (ret) {
            HDF_LOGE("%s: mkdir failure!", __func__);
            return HDF_ERR_IO;
        }
    }

    ret = snprintf_s(tmpPath, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/configuration", tmp);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }

    ret = UsbFnWriteFile(tmpPath, stringValue);
    return ret;
}

static int32_t UsbFnWriteDesString(const char *deviceName,
    uint16_t lang, const char *stringName, const char *stringValue)
{
    int32_t ret;
    char tmp[MAX_PATHLEN] = {0};
    char tmpPath[MAX_PATHLEN] = {0};
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/strings/0x%x",
        CONFIGFS_DIR, deviceName, lang);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }
    if (!IsDirExist(tmp)) {
        ret = mkdir(tmp, S_IREAD | S_IWRITE);
        if (ret) {
            HDF_LOGE("%s: mkdir failure!", __func__);
            return HDF_ERR_IO;
        }
    }

    ret = snprintf_s(tmpPath, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s", tmp, stringName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }

    ret = UsbFnWriteFile(tmpPath, stringValue);
    return ret;
}

static int32_t UsbFnAdapterCreateFunc(const char *configPath, const char *funcPath)
{
    int32_t ret;

    ret = mkdir(funcPath, S_IREAD | S_IWRITE);
    if (ret) {
        HDF_LOGE("%s: mkdir failure!", __func__);
        return HDF_ERR_IO;
    }

    ret = symlink(funcPath, configPath);
    if (ret) {
        HDF_LOGE("%s: symlink failed", __func__);
        return HDF_ERR_IO;
    }
    usleep(SLEEP_100MS);
    return 0;
}

static int32_t UsbFnReadFile(const char *path, char *str, uint16_t len)
{
    int32_t ret;
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        HDF_LOGE("%s: fopen failure!", __func__);
        return HDF_ERR_BAD_FD;
    }
    ret = fread(str, len, 1, fp);
    if (ret < 0) {
        if (fclose(fp)) {
            return HDF_FAILURE;
        }
        return HDF_ERR_IO;
    }
    if (fclose(fp)) {
        return HDF_FAILURE;
    }
    return 0;
}

static int32_t UsbFnAdapterWriteUDC(const char *deviceName, const char *udcName, int32_t enable)
{
    int32_t ret, i;
    char tmp[MAX_PATHLEN] = {0};
    char udcTmp[MAX_NAMELEN] = {0};
    char tmpCmd[MAX_PATHLEN] = {0};
    if (deviceName == NULL || udcName == NULL || IsDeviceDirExist(deviceName) == false) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/UDC", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }
    if (enable) {
        (void)UsbFnWriteFile(tmp, udcName);
        for (i = 0; i < OPEN_CNT; i++) {
            (void)UsbFnReadFile(tmp, udcTmp, strlen(udcName));
            if (!strcmp(udcName, udcTmp)) {
                return 0;
            }
            usleep(SLEEP_100MS);
        }
        if (strcmp(udcName, udcTmp)) {
            return HDF_ERR_IO;
        }
    } else {
        ret = snprintf_s(tmpCmd, MAX_PATHLEN, MAX_PATHLEN - 1, "echo \"\" > %s/%s/UDC",
            CONFIGFS_DIR, deviceName);
        if (ret < 0) {
            HDF_LOGE("%s: snprintf_s failed", __func__);
            return HDF_ERR_IO;
        }
        system(tmpCmd);
    }
    return 0;
}

static int32_t UsbFnAdapterOpenFn(void)
{
    int32_t i;
    int32_t ep = -1;
    for (i = 0; i < OPEN_CNT; i++) {
        ep = open(USBFN_DEV, O_RDWR);
        if (ep > 0) {
            break;
        }
        usleep(SLEEP_100MS);
    }
    if (ep < 0) {
        HDF_LOGE("func not alloc!");
    }
    return ep;
}

static int32_t UsbFnAdapterClosefn(int32_t fd)
{
    if (fd <= 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    return close(fd);
}

static int32_t UsbFnAdapterCreatInterface(const char *interfaceName, int32_t nameLen)
{
    int32_t ret;
    int32_t fd;
    struct FuncNew fnnew;
    if (interfaceName == NULL || nameLen <= 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    fd = UsbFnAdapterOpenFn();
    if (fd <= 0) {
        HDF_LOGE("%s: UsbFnAdapterOpenFn failure!",  __func__);
        return HDF_ERR_IO;
    }

    fnnew.nameLen = nameLen;
    ret = snprintf_s(fnnew.name, MAX_NAMELEN, MAX_NAMELEN - 1, "%s", interfaceName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }
    ret = ioctl(fd, FUNCTIONFS_NEWFN, &fnnew);
    if (ret) {
        HDF_LOGE("%s: FUNCTIONFS_NEWFN failure!",  __func__);
        return HDF_ERR_IO;
    }
    ret = UsbFnAdapterClosefn(fd);
    usleep(SLEEP_100MS);
    return ret;
}

static int32_t UsbFnAdapterDelInterface(const char *interfaceName, int32_t nameLen)
{
    int32_t ret;
    struct FuncNew fnnew;
    if (interfaceName == NULL || nameLen <= 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t fd = UsbFnAdapterOpenFn();
    if (fd <= 0) {
        return HDF_ERR_IO;
    }

    fnnew.nameLen = nameLen;
    ret = snprintf_s(fnnew.name, MAX_NAMELEN, MAX_NAMELEN - 1, "%s", interfaceName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }
    ret = ioctl(fd, FUNCTIONFS_DELFN, &fnnew);
    if (ret) {
        HDF_LOGE("%s: FUNCTIONFS_DELFN failure!",  __func__);
        return HDF_ERR_IO;
    }
    ret = UsbFnAdapterClosefn(fd);
    return ret;
}

static int32_t UsbFnAdapterOpenPipe(const char *interfaceName, int32_t epIndex)
{
    if (interfaceName == NULL || epIndex < 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    char epName[MAX_NAMELEN];
    int32_t ret = snprintf_s(epName, MAX_NAMELEN, MAX_NAMELEN - 1, "/dev/functionfs/%s.ep%d", interfaceName, epIndex);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }

    int32_t ep = -1;
    for (int32_t i = 0; i < OPEN_CNT; i++) {
        ep = open(epName, O_RDWR);
        if (ep > 0) {
            break;
        }
        usleep(SLEEP_100MS);
    }
    if (ep < 0) {
        HDF_LOGE("unable to open %s", epName);
        return HDF_DEV_ERR_NO_DEVICE;
    }
    return ep;
}

static int32_t UsbFnAdapterClosePipe(int32_t ep)
{
    if (ep <= 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    return close(ep);
}

static void GetHeaderStr(struct UsbFnStrings **strings,
    struct UsbFunctionfsStringsHead *headerStr)
{
    uint32_t i, j;
    uint32_t langCount = 0;
    uint32_t strCount;
    uint32_t len = 0;
    for (i = 0; strings[i] != NULL; i++) {
        langCount++;
        for (j = 0; strings[i]->strings[j].s; j++) {
            len += strlen(strings[i]->strings[j].s) + sizeof(char);
        }
        strCount = j;
    }
    headerStr->magic = htole32(FUNCTIONFS_STRINGS_MAGIC);
    headerStr->length = htole32(sizeof(struct UsbFunctionfsStringsHead) +
        langCount * sizeof(uint16_t) + len);
    headerStr->str_count = strCount;
    headerStr->lang_count = langCount;
}

static int32_t UsbFnWriteStrings(int32_t ep0, struct UsbFnStrings **strings)
{
    uint8_t *str = NULL;
    uint8_t *whereDec = NULL;
    uint32_t i, j;
    int32_t ret;
    struct UsbFunctionfsStringsHead headerStr = {0};

    GetHeaderStr(strings, &headerStr);
    str = UsbFnMemCalloc(headerStr.length);
    if (str == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    whereDec = str;
    ret = memcpy_s(whereDec, headerStr.length, &headerStr,
        sizeof(struct UsbFunctionfsStringsHead));
    if (ret != EOK) {
        goto ERR;
    }
    whereDec += sizeof(struct UsbFunctionfsStringsHead);

    for (i = 0; i < headerStr.lang_count; i++) {
        ret = memcpy_s(whereDec, headerStr.length - (whereDec - str),
            &strings[i]->language, sizeof(uint16_t));
        if (ret != EOK) {
            goto ERR;
        }
        whereDec += sizeof(uint16_t);
        for (j = 0; j < headerStr.str_count; j++) {
            if (strlen(strings[i]->strings[j].s)) {
                ret = memcpy_s(whereDec, headerStr.length - (whereDec - str),
                    strings[i]->strings[j].s, strlen(strings[i]->strings[j].s));
                whereDec += strlen(strings[i]->strings[j].s) + sizeof(char);
            } else {
                break;
            }
            if (ret != EOK) {
                goto ERR;
            }
        }
    }

    if (write(ep0, str, headerStr.length) < 0) {
        goto ERR;
    }
    UsbFnMemFree(str);
    return 0;
ERR:
    UsbFnMemFree(str);
    return HDF_FAILURE;
}

static int32_t CopyCount(uint8_t **whereDec, uint32_t fsCount, uint32_t hsCount, uint32_t ssCount)
{
    int32_t ret;
    if (fsCount) {
        ret = memcpy_s(*whereDec, sizeof(uint32_t), &fsCount, sizeof(uint32_t));
        if (ret != EOK) {
            return HDF_FAILURE;
        }
        *whereDec += sizeof(uint32_t);
    }
    if (hsCount) {
        ret = memcpy_s(*whereDec, sizeof(uint32_t), &hsCount, sizeof(uint32_t));
        if (ret != EOK) {
            return HDF_FAILURE;
        }
        *whereDec += sizeof(uint32_t);
    }
    if (ssCount) {
        ret = memcpy_s(*whereDec, sizeof(uint32_t), &ssCount, sizeof(uint32_t));
        if (ret != EOK) {
            return HDF_FAILURE;
        }
        *whereDec += sizeof(uint32_t);
    }

    return 0;
}

static int32_t WriteFuncDescriptors(uint8_t **whereDec, struct UsbDescriptorHeader **headDes)
{
    int32_t ret;
    int32_t i;

    for (i = 0; headDes[i] != NULL; i++) {
        ret = memcpy_s(*whereDec, headDes[i]->bLength, headDes[i], headDes[i]->bLength);
        if (ret != EOK) {
            HDF_LOGE("%s: memcpy_s failure!", __func__);
            return HDF_FAILURE;
        }
        *whereDec += headDes[i]->bLength;
    }
    return 0;
}

void GetCountAndHead(struct UsbFunctionfsDescsHeadV2 *header, uint32_t *fsCount,
    uint32_t *hsCount, uint32_t *ssCount, const struct UsbFnFunction *func)
{
    int32_t i;
    uint32_t lenCount = 0;
    uint32_t lenDes = 0;
    *fsCount = 0;
    *hsCount = 0;
    *ssCount = 0;

    for (i = 0; func->fsDescriptors[i] != NULL; i++) {
        (*fsCount)++;
        lenDes += func->fsDescriptors[i]->bLength;
    }
    for (i = 0; func->hsDescriptors[i] != NULL; i++) {
        (*hsCount)++;
        lenDes += func->hsDescriptors[i]->bLength;
    }
    for (i = 0; func->ssDescriptors[i] != NULL; i++) {
        (*ssCount)++;
        lenDes += func->ssDescriptors[i]->bLength;
    }

    if (*fsCount) {
        lenCount += sizeof(uint32_t);
        header->flags |= htole32(FUNCTIONFS_HAS_FS_DESC);
    }
    if (*hsCount) {
        lenCount += sizeof(uint32_t);
        header->flags |= htole32(FUNCTIONFS_HAS_HS_DESC);
    }
    if (*ssCount) {
        lenCount += sizeof(uint32_t);
        header->flags |= htole32(FUNCTIONFS_HAS_SS_DESC);
    }

    header->magic = htole32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2);
    header->length = htole32(sizeof(struct UsbFunctionfsDescsHeadV2) + lenCount + lenDes);
}

static int32_t UsbFnAdapterCreatPipes(int32_t ep0, const struct UsbFnFunction *func)
{
    uint8_t *dec = NULL;
    uint8_t *whereDec = NULL;
    int32_t ret;
    uint32_t fsCount;
    uint32_t hsCount;
    uint32_t ssCount;
    struct UsbFunctionfsDescsHeadV2 header = {0};

    GetCountAndHead(&header, &fsCount, &hsCount, &ssCount, func);

    dec = UsbFnMemCalloc(header.length);
    if (dec == NULL) {
        HDF_LOGE("%s: UsbFnMemCalloc failure!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    whereDec = dec;

    ret = memcpy_s(whereDec, header.length, &header,
        sizeof(struct UsbFunctionfsDescsHeadV2));
    if (ret != EOK) {
        return HDF_FAILURE;
    }
    whereDec += sizeof(struct UsbFunctionfsDescsHeadV2);

    ret = CopyCount(&whereDec, fsCount, hsCount, ssCount);
    if (ret != EOK) {
        return HDF_FAILURE;
    }

    ret = WriteFuncDescriptors(&whereDec, func->fsDescriptors);
    if (ret != EOK) {
        return HDF_FAILURE;
    }

    ret = WriteFuncDescriptors(&whereDec, func->hsDescriptors);
    if (ret != EOK) {
        return HDF_FAILURE;
    }

    ret = WriteFuncDescriptors(&whereDec, func->ssDescriptors);
    if (ret != EOK) {
        return HDF_FAILURE;
    }

    if (write(ep0, dec, header.length) < 0) {
        HDF_LOGE("unable do write descriptors");
        UsbFnMemFree(dec);
        return HDF_ERR_IO;
    }

    UsbFnMemFree(dec);
    ret = UsbFnWriteStrings(ep0, func->strings);

    usleep(SLEEP_100MS);
    return ret;
}

static int32_t WriteDeviceId(const char *devName, const struct UsbDeviceDescriptor *desc)
{
    int32_t ret;
    ret = UsbFnWriteProp(devName, "idVendor", desc->idVendor);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "idProduct", desc->idProduct);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "bcdUSB", desc->bcdUSB);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "bcdDevice", desc->bcdDevice);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "bDeviceClass", desc->bDeviceClass);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "bDeviceSubClass", desc->bDeviceSubClass);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "bDeviceProtocol", desc->bDeviceProtocol);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnWriteProp(devName, "bMaxPacketSize0", desc->bMaxPacketSize0);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }
    return 0;
}

static int32_t WriteDeviceDescriptor(const char *devName,
    const struct UsbDeviceDescriptor *desc, struct UsbFnStrings **strings)
{
    int32_t i, ret;
    ret = WriteDeviceId(devName, desc);
    if (ret) {
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; strings[i] != NULL; i++) {
        ret = UsbFnWriteDesString(devName, strings[i]->language, "manufacturer",
            strings[i]->strings[desc->iManufacturer].s);
        if (ret) {
            return HDF_ERR_INVALID_PARAM;
        }
        ret = UsbFnWriteDesString(devName, strings[i]->language, "product",
            strings[i]->strings[desc->iProduct].s);
        if (ret) {
            return HDF_ERR_INVALID_PARAM;
        }
        ret = UsbFnWriteDesString(devName, strings[i]->language, "serialnumber",
            strings[i]->strings[desc->iSerialNumber].s);
        if (ret) {
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return 0;
}

static int32_t CreatDeviceDir(const char *devName)
{
    int32_t ret;
    char tmp[MAX_PATHLEN];
    (void)memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s", CONFIGFS_DIR, devName);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    if (!IsDirExist(tmp)) {
        ret = mkdir(tmp, S_IREAD | S_IWRITE);
        if (ret) {
            HDF_LOGE("%s: mkdir failure!", __func__);
            return HDF_ERR_IO;
        }
    }
    return 0;
}

static int32_t WriteConfPowerAttributes(const char *devName,
    struct UsbFnConfiguration *config, uint8_t confVal)
{
    int32_t ret;
    char configName[MAX_PATHLEN];
    char tmp[MAX_PATHLEN], val[MAX_NAMELEN];
    memset_s(configName, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(configName, MAX_PATHLEN, MAX_PATHLEN - 1,
        "%s/%s/configs/b.%d", CONFIGFS_DIR, devName, confVal);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    if (!IsDirExist(configName)) {
        ret = mkdir(configName, S_IREAD | S_IWRITE);
        if (ret) {
            return HDF_ERR_IO;
        }
    }
    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/MaxPower", configName);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    memset_s(val, MAX_NAMELEN, 0, MAX_NAMELEN);
    ret = snprintf_s(val, MAX_NAMELEN, MAX_NAMELEN - 1, "%d", config->maxPower);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    ret = UsbFnWriteFile(tmp, val);
    if (ret < 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/bmAttributes", configName);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    memset_s(val, MAX_NAMELEN, 0, MAX_NAMELEN);
    ret = snprintf_s(val, MAX_NAMELEN, MAX_NAMELEN - 1, "0x%x", config->attributes);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    ret = UsbFnWriteFile(tmp, val);
    if (ret < 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    return 0;
}

static int32_t CreatKernelFunc(const char *devName, const struct UsbFnFunction *functions,
    uint8_t confVal)
{
    int32_t ret;
    char configPath[MAX_PATHLEN];
    char funcPath[MAX_PATHLEN];

    memset_s(funcPath, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(funcPath, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/functions/%s",
        CONFIGFS_DIR, devName, functions->funcName);
    if (ret < 0) {
        return HDF_ERR_IO;
    }

    memset_s(configPath, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(configPath, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/configs/b.%d/%s",
        CONFIGFS_DIR, devName, confVal, functions->funcName);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    ret = UsbFnAdapterCreateFunc(configPath, funcPath);
    if (ret) {
        HDF_LOGE("%s: UsbFnAdapterCreateFunc failure!",  __func__);
        return HDF_ERR_IO;
    }
    return ret;
}

static void CleanConfigFs(const char *devName, const char *funcName);
static int32_t CreatFunc(const char *devName, const struct UsbFnFunction *functions,
    uint8_t confVal)
{
    int32_t fd, ret;
    char interfaceName[MAX_NAMELEN];

    ret = CreatKernelFunc(devName, functions, confVal);
    if (ret < 0) {
        return HDF_ERR_IO;
    }

    memset_s(interfaceName, MAX_NAMELEN, 0, MAX_NAMELEN);
    ret = snprintf_s(interfaceName, MAX_NAMELEN, MAX_NAMELEN - 1, "%s", functions->funcName);
    if (ret < 0) {
        return HDF_ERR_IO;
    }
    ret = UsbFnAdapterCreatInterface(interfaceName, strlen(interfaceName));
    if (ret) {
        HDF_LOGE("%s: UsbFnAdapterCreatInterface failure!",  __func__);
        CleanConfigFs(devName, interfaceName);
        return HDF_ERR_IO;
    }

    fd = UsbFnAdapterOpenPipe(interfaceName, 0);
    if (fd <= 0) {
        HDF_LOGE("%s: UsbFnAdapterOpenPipe failure!",  __func__);
        CleanConfigFs(devName, interfaceName);
        return HDF_ERR_IO;
    }
    ret = UsbFnAdapterCreatPipes(fd, functions);
    if (ret) {
        HDF_LOGE("%s: UsbFnAdapterCreatPipes failure!",  __func__);
        return HDF_ERR_IO;
    }
    ret = UsbFnAdapterClosePipe(fd);
    if (ret) {
        HDF_LOGE("%s: UsbFnAdapterClosePipe failure!",  __func__);
        return HDF_ERR_IO;
    }
    return 0;
}

static void DelConfigDevice(const char *deviceName)
{
    int32_t ret;
    char tmp[MAX_PATHLEN];
    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1,
        "%s/%s/configs", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        return;
    }
    DeleteFile(tmp);

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1,
        "%s/%s/functions", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        return;
    }
    DeleteFile(tmp);

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1,
        "%s/%s/strings", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        return;
    }
    DeleteFile(tmp);

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1,
        "%s/%s", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        return;
    }
    rmdir(tmp);
}

static void CleanConfigFs(const char *devName, const char *funcName)
{
    int32_t ret;
    char tmp[MAX_PATHLEN];

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/configs/b.1/%s",
        CONFIGFS_DIR, devName, funcName);
    if (ret < 0) {
        return;
    }
    remove(tmp);

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/functions/%s",
        CONFIGFS_DIR, devName, funcName);
    if (ret < 0) {
        return;
    }
    remove(tmp);
}

static void CleanFunction(const char *devName, const char *funcName)
{
    int32_t ret;
    ret = UsbFnAdapterDelInterface(funcName, strlen(funcName));
    if (ret) {
        HDF_LOGE("%s: UsbFnAdapterDelInterface failure!", __func__);
        return;
    }
    CleanConfigFs(devName, funcName);
}

static void UsbFnAdapterCleanDevice(const char *devName)
{
    int32_t ret;
    char tmp[MAX_PATHLEN];
    DIR *dir = NULL;
    struct dirent *ptr = NULL;

    memset_s(tmp, MAX_PATHLEN, 0, MAX_PATHLEN);
    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/functions/",
        CONFIGFS_DIR, devName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return;
    }
    if ((dir = opendir(tmp)) == NULL) {
        return;
    }
    while ((ptr = readdir(dir)) != NULL) {
        if (strncmp(ptr->d_name, FUNCTION_GENERIC, strlen(FUNCTION_GENERIC))) {
            continue;
        }
        CleanFunction(devName, ptr->d_name);
    }
    closedir(dir);
}

static int32_t UsbFnAdapterDelDevice(const char *deviceName, const char *udcName,
    struct UsbFnDeviceDesc *des)
{
    uint32_t i, j;
    int32_t ret;
    if (deviceName == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    ret = UsbFnAdapterWriteUDC(deviceName, udcName, 0);
    if (ret < 0) {
        return ret;
    }
    for (i = 0; des->configs[i] != NULL; i++) {
        for (j = 0; des->configs[i]->functions[j] != NULL; j++) {
            if (strncmp(des->configs[i]->functions[j]->funcName,
                FUNCTION_GENERIC, strlen(FUNCTION_GENERIC))) {
                CleanConfigFs(deviceName, des->configs[i]->functions[j]->funcName);
                continue;
            }
            if (des->configs[i]->functions[j]->enable == false) {
                continue;
            }
            CleanFunction(deviceName, des->configs[i]->functions[j]->funcName);
        }
    }

    if (strcmp("g1", deviceName)) {
        DelConfigDevice(deviceName);
    }
    return 0;
}

static int32_t UsbFnAdapterWaitUDC(const char *deviceName, const char *udcName)
{
    int32_t ret, i;
    char tmp[MAX_PATHLEN] = {0};
    char udcTmp[MAX_NAMELEN] = {0};
    if (deviceName == NULL || udcName == NULL || IsDeviceDirExist(deviceName) == false) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = snprintf_s(tmp, MAX_PATHLEN, MAX_PATHLEN - 1, "%s/%s/UDC", CONFIGFS_DIR, deviceName);
    if (ret < 0) {
        HDF_LOGE("%s: snprintf_s failed", __func__);
        return HDF_ERR_IO;
    }
    for (i = 0; i < OPEN_CNT * 0x2; i++) {
        (void)UsbFnReadFile(tmp, udcTmp, strlen(udcName));
        if (!strcmp(udcName, udcTmp)) {
            HDF_LOGD("%s: hdc enable udc!", __func__);
            return 0;
        }
        OsalMDelay(DELAY_100MS);
    }
    return HDF_ERR_IO;
}

static int32_t EnableDevice(const char *udcName,
    const char *devName, struct UsbFnDeviceDesc *descriptor)
{
    int32_t ret;
    (void)UsbFnAdapterWriteUDC(devName, "none", 0);
    if (!UsbFnAdapterWaitUDC(devName, udcName)) {
        return 0;
    }
    ret = UsbFnAdapterWriteUDC(devName, udcName, 1);
    if (ret) {
        (void)UsbFnAdapterDelDevice(devName, udcName, descriptor);
    }
    return ret;
}

#define FFS_ADB "/config/usb_gadget/g1/functions/ffs.adb"
#define FFS_CONFIG "/config/usb_gadget/g1/configs/b.1/f1"

static bool CreateFun(struct UsbFnFunction *function, const char *devName, uint8_t *confVal, int32_t *ret)
{
    if (strncmp(function->funcName,
        FUNCTION_GENERIC, strlen(FUNCTION_GENERIC))) {
        *ret = CreatKernelFunc(devName, function, *confVal);
    } else {
        if (function->enable == false) {
            return false;
        }
        *ret = CreatFunc(devName, function, *confVal);
    }
    return true;
}

static int32_t UsbFnAdapterCreateDevice(const char *udcName,
    const char *devName, struct UsbFnDeviceDesc *descriptor)
{
    uint32_t i, j;
    int32_t ret;
    uint8_t confVal;

    (void)symlink(FFS_ADB, FFS_CONFIG);

    UsbFnAdapterCleanDevice(devName);

    ret = CreatDeviceDir(devName);
    if (ret) {
        return HDF_ERR_IO;
    }

    ret = WriteDeviceDescriptor(devName, descriptor->deviceDesc, descriptor->deviceStrings);
    if (ret) {
        HDF_LOGE("%s: WriteDeviceDescriptor failure!", __func__);
        return HDF_ERR_IO;
    }

    for (i = 0; descriptor->configs[i] != NULL; i++) {
        confVal = descriptor->configs[i]->configurationValue;
        ret = WriteConfPowerAttributes(devName, descriptor->configs[i], confVal);
        if (ret) {
            HDF_LOGE("%s: WriteConfPowerAttributes failure!", __func__);
            return HDF_ERR_IO;
        }

        for (j = 0; descriptor->deviceStrings[j] != NULL; j++) {
            ret = UsbFnWriteConfString(devName, confVal, descriptor->deviceStrings[j]->language,
                descriptor->deviceStrings[j]->strings[descriptor->configs[i]->iConfiguration].s);
            if (ret < 0) {
                HDF_LOGE("%s: UsbFnWriteConfString failure!", __func__);
                return HDF_ERR_INVALID_PARAM;
            }
        }

        for (j = 0; descriptor->configs[i]->functions[j] != NULL; j++) {
            if (!CreateFun(descriptor->configs[i]->functions[j], devName, &confVal, &ret)) {
                continue;
            }
            if (ret < 0) {
                HDF_LOGE("%s: CreatFunc failure!", __func__);
                (void)UsbFnAdapterWriteUDC(devName, "none", 0);
                (void)UsbFnAdapterWriteUDC(devName, udcName, 1);
                return HDF_ERR_INVALID_PARAM;
            }
        }
    }

    return EnableDevice(udcName, devName, descriptor);
}

static int32_t UsbFnAdapterGetPipeInfo(int32_t ep, struct UsbFnPipeInfo *pipeInfo)
{
    int32_t ret;
    if (ep <= 0 || pipeInfo == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    struct UsbEndpointDescriptor desc;
    ret = ioctl(ep, FUNCTIONFS_ENDPOINT_DESC, &desc);
    if (ret) {
        HDF_LOGE("%s: FUNCTIONFS_ENDPOINT_DESC failure!", __func__);
        return HDF_ERR_IO;
    }

    pipeInfo->type = desc.bmAttributes;
    pipeInfo->dir = USB_PIPE_DIRECTION_OUT;
    if (desc.bEndpointAddress & 0x80) {
        pipeInfo->dir = USB_PIPE_DIRECTION_IN;
    }

    pipeInfo->maxPacketSize = desc.wMaxPacketSize;
    pipeInfo->interval = desc.bInterval;

    return ret;
}

int32_t UsbFnAdapterQueueInit(int32_t ep)
{
    if (ep <= 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    return ioctl(ep, FUNCTIONFS_ENDPOINT_QUEUE_INIT, 0);
}

static int32_t UsbFnAdapterQueueDel(int32_t ep)
{
    if (ep <= 0) {
        return HDF_ERR_INVALID_PARAM;
    }

    return ioctl(ep, FUNCTIONFS_ENDPOINT_QUEUE_DEL, 0);
}

static int32_t UsbFnAdapterReleaseBuf(int32_t ep, const struct GenericMemory *mem)
{
    if (ep <= 0 || mem == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return ioctl(ep, FUNCTIONFS_ENDPOINT_RELEASE_BUF, mem);
}

static int32_t UsbFnAdapterPipeIo(int32_t ep, struct IoData *ioData)
{
    int32_t ret;
    if (ep <= 0 || ioData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (ioData->read) {
        ret = ioctl(ep, FUNCTIONFS_ENDPOINT_READ, ioData);
    } else {
        ret = ioctl(ep, FUNCTIONFS_ENDPOINT_WRITE, ioData);
    }

    return ret;
}

static int32_t UsbFnAdapterCancelIo(int32_t ep, const struct IoData *ioData)
{
    if (ep <= 0 || ioData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return ioctl(ep, FUNCTIONFS_ENDPOINT_RW_CANCEL, ioData);
}

static uint8_t *UsbFnAdapterMapAddr(int32_t ep, uint32_t len)
{
    if (ep <= 0) {
        return NULL;
    }

    return mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, ep, 0);
}

static int32_t UsbFnAdapterUnmapAddr(uint8_t *mapAddr, uint32_t len)
{
    if (mapAddr == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    return munmap(mapAddr, len);
}

static void Ep0Event(struct UsbFnEventAll *event, struct pollfd *pfds)
{
    int32_t ret;
    uint8_t i;
    for (i = 0; i < event->ep0Num; i++) {
        if ((uint32_t)pfds[i].revents & POLLIN) {
            ret = read(event->ep0[i], &event->ep0Event[i].ctrlEvent, sizeof(struct UsbFnCtrlEvent));
            if (!ret) {
                HDF_LOGE("unable to read event from ep0");
            }
            event->ep0Event[i].type = USB_EP0_CTRL_EVENT;
        } else if ((uint32_t)pfds[i].revents & POLLOUT) {
            ret = ioctl(event->ep0[i], FUNCTIONFS_ENDPOINT_GET_EP0_EVENT, &event->ep0Event[i].reqEvent);
            if (!ret) {
                HDF_LOGE("unable to read reqEvent from ep0");
            }
            event->ep0Event[i].type = USB_EP0_IO_COMPLETED;
        }
    }
}

static void EpEvent(struct UsbFnEventAll *event, struct pollfd *pfds)
{
    uint8_t i;
    for (i = 0; i < event->epNum; i++) {
        if ((pfds[i + event->ep0Num].revents & POLLIN)) {
            event->numEvent[i] = read(event->epx[i], event->reqEvent[i],
                MAX_REQUEST * sizeof(struct UsbFnReqEvent)) / sizeof(struct UsbFnReqEvent);
            if (!event->numEvent[i]) {
                HDF_LOGE("unable to read indexBuf from ep#");
            }
        }
    }
}

static int32_t UsbFnAdapterPollEvent(struct UsbFnEventAll *event, int32_t timeout)
{
    int32_t ret;
    uint8_t i;
    struct pollfd *pfds = NULL;

    if (event == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (event->ep0Num + event->epNum == 0) {
        return HDF_ERR_INVALID_PARAM;
    }
    pfds = UsbFnMemCalloc((event->ep0Num + event->epNum) * sizeof(struct pollfd));
    if (pfds == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    for (i = 0; i < event->ep0Num; i++) {
        if (event->ep0[i] <= 0) {
            UsbFnMemFree(pfds);
            HDF_LOGE("%s: ep[%d] = %d", __func__, i, event->ep0[i]);
            return HDF_ERR_INVALID_PARAM;
        }
        pfds[i].fd = event->ep0[i];
        pfds[i].events = POLLIN | POLLOUT;
    }
    for (i = 0; i < event->epNum; i++) {
        if (event->epx[i] <= 0) {
            UsbFnMemFree(pfds);
            HDF_LOGE("%s: ep[%d] = %d", __func__, i, event->epx[i]);
            return HDF_ERR_INVALID_PARAM;
        }
        pfds[i + event->ep0Num].fd = event->epx[i];
        pfds[i + event->ep0Num].events = POLLIN;
    }
    ret = poll(pfds, event->ep0Num + event->epNum, timeout);
    if (ret == 0) {
        UsbFnMemFree(pfds);
        return HDF_ERR_TIMEOUT;
    } else if (ret < 0) {
        HDF_LOGE("%s: interrupt", __func__);
        UsbFnMemFree(pfds);
        return HDF_ERR_IO;
    }
    Ep0Event(event, pfds);
    EpEvent(event, pfds);
    UsbFnMemFree(pfds);
    return 0;
}

static int32_t UsbFnAdapterRequestGetStatus(int32_t ep, const struct IoData *ioData)
{
    if (ep <= 0 || ioData == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return ioctl(ep, FUNCTIONFS_ENDPOINT_GET_REQ_STATUS, ioData);
}

void *UsbFnMemAlloc(size_t size)
{
    return UsbFnMemCalloc(size);
}

void *UsbFnMemCalloc(size_t size)
{
    void *buf = NULL;
    struct RawUsbRamTestList *testEntry = NULL;
    struct RawUsbRamTestList *pos = NULL;
    uint32_t totalSize = 0;
    int32_t ret = 0;

    buf = OsalMemAlloc(size);
    if (buf == NULL) {
        HDF_LOGE("%{public}s: %{public}d, OsalMemAlloc failed", __func__, __LINE__);
        return NULL;
    }
    ret = memset_s(buf, size, 0, size);
    if (ret != EOK) {
        HDF_LOGE("%{public}s: %{public}d, memset_s failed", __func__, __LINE__);
        OsalMemFree(buf);
        return NULL;
    }

    if (g_usbRamTestFlag) {
        if (g_usbRamTestHead == NULL) {
            g_usbRamTestHead = OsalMemAlloc(sizeof(struct RawUsbRamTestList));
            OsalMutexInit(&g_usbRamTestHead->lock);
            DListHeadInit(&g_usbRamTestHead->list);
        }
        testEntry = OsalMemAlloc(sizeof(struct RawUsbRamTestList));
        testEntry->address = (uintptr_t)buf;
        testEntry->size = size;

        OsalMutexLock(&g_usbRamTestHead->lock);
        DListInsertTail(&testEntry->list, &g_usbRamTestHead->list);
        DLIST_FOR_EACH_ENTRY(pos, &g_usbRamTestHead->list, struct RawUsbRamTestList, list) {
            totalSize += pos->size;
        }
        OsalMutexUnlock(&g_usbRamTestHead->lock);
        HDF_LOGE("%{public}s add size=%{public}u totalSize=%{public}u", __func__, (uint32_t)size, totalSize);
    }
    return buf;
}

void UsbFnMemFree(const void *mem)
{
    struct RawUsbRamTestList *pos = NULL;
    struct RawUsbRamTestList *tmp = NULL;
    uint32_t totalSize = 0;
    uint32_t size = 0;

    if (mem == NULL) {
        return;
    }

    if (g_usbRamTestFlag && (g_usbRamTestHead != NULL)) {
        OsalMutexLock(&g_usbRamTestHead->lock);
        DLIST_FOR_EACH_ENTRY_SAFE(pos, tmp, &g_usbRamTestHead->list, struct RawUsbRamTestList, list) {
            if (pos->address == (uintptr_t)mem) {
                size = pos->size;
                DListRemove(&pos->list);
                OsalMemFree(pos);
                continue;
            }
            totalSize += pos->size;
        }
        OsalMutexUnlock(&g_usbRamTestHead->lock);
        HDF_LOGE("%{public}s rm size=%{public}d totalSize=%{public}d", __func__, size, totalSize);
    }

    if (mem != NULL) {
        OsalMemFree((void *)mem);
    }
}

int32_t UsbFnAdpMemTestTrigger(bool enable)
{
    g_usbRamTestFlag = enable;
    return HDF_SUCCESS;
}

static struct UsbFnAdapterOps g_usbFnAdapter = {
    .createDevice = UsbFnAdapterCreateDevice,
    .delDevice = UsbFnAdapterDelDevice,

    .openPipe = UsbFnAdapterOpenPipe,
    .closePipe = UsbFnAdapterClosePipe,
    .getPipeInfo = UsbFnAdapterGetPipeInfo,

    .queueInit = UsbFnAdapterQueueInit,
    .queueDel = UsbFnAdapterQueueDel,
    .releaseBuf = UsbFnAdapterReleaseBuf,
    .pipeIo = UsbFnAdapterPipeIo,
    .cancelIo = UsbFnAdapterCancelIo,
    .getReqStatus = UsbFnAdapterRequestGetStatus,
    .mapAddr = UsbFnAdapterMapAddr,
    .unmapAddr = UsbFnAdapterUnmapAddr,
    .pollEvent = UsbFnAdapterPollEvent,
    .writeUDC = UsbFnAdapterWriteUDC,
    .writeProp = UsbFnWriteProp,
    .writeDesString = UsbFnWriteDesString,
};

struct UsbFnAdapterOps *UsbFnAdapterGetOps(void)
{
    return &g_usbFnAdapter;
}

