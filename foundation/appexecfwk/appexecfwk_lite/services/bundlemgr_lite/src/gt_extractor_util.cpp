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

#include "gt_extractor_util.h"

#include <climits>
#include "appexecfwk_errors.h"
#include "bundle_common.h"
#include "bundle_util.h"
#include "bundlems_log.h"
#include "cstdio"
#include "dirent.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "unistd.h"
#include "utils.h"

namespace OHOS {
const uint8_t MAGIC_NUMBER_VALUE = 190;
const uint8_t SHIFT_NUM = 8;

uint32_t GtExtractorUtil::ReadInt(int32_t fp)
{
    char buf[INT_LENGTH] = {0};
    if (read(fp, buf, INT_LENGTH) != INT_LENGTH) {
        return UINT_MAX;
    }

    uint32_t result = 0;
    for (uint8_t i = 0; i < INT_LENGTH; i++) {
        if (result > MAX_INT) {
            return UINT_MAX;
        }
        result += (static_cast<uint32_t>(buf[i])) << (SHIFT_NUM * (INT_LENGTH - i - 1));
    }
    return result;
}

bool GtExtractorUtil::CheckMagicNumber(int32_t fp)
{
    char buf[MAGIC_NUMBER_LEN] = {0};
    if (read(fp, buf, MAGIC_NUMBER_LEN) != MAGIC_NUMBER_LEN) {
        return false;
    }
    if (static_cast<uint8_t>(buf[MAGIC_NUMBER_LEN - 1]) != MAGIC_NUMBER_VALUE) {
        return false;
    }
    return true;
}

uint64_t GtExtractorUtil::ReadLong(int32_t fp)
{
    char buf[LONG_LENGTH] = {0};
    if (read(fp, buf, LONG_LENGTH) != LONG_LENGTH) {
        return 0;
    }

    uint64_t result = 0;
    for (uint8_t i = 0; i < LONG_LENGTH; i++) {
        if (result > LONG_MAX) {
            return 0;
        }
        result += (static_cast<uint64_t>(buf[i])) << (SHIFT_NUM * (LONG_LENGTH - i - 1));
    }
    return result;
}

char *GtExtractorUtil::ReadString(int32_t fp, uint32_t len)
{
    char *buf = reinterpret_cast<char *>(UI_Malloc((len + 1) * sizeof(char)));
    if (buf == nullptr) {
        return nullptr;
    }

    if (len == 0) {
        *buf = '\0';
        return buf;
    }

    if (read(fp, buf, len) != len) {
        UI_Free(buf);
        return nullptr;
    }
    *(buf + len) = '\0';
    return buf;
}

uint8_t GtExtractorUtil::ExtractFileHeaderInfo(int32_t fp, char **bundleName)
{
    if (!CheckMagicNumber(fp)) {
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }

    uint32_t bundleNameLen = ReadInt(fp);
    if (bundleNameLen == UINT_MAX) {
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }

    if ((*bundleName = ReadString(fp, bundleNameLen)) == nullptr) {
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }
    return ERR_OK;
}

uint8_t GtExtractorUtil::ExtractFileToPath(const char *appInstallPath, int32_t fp, uint64_t &fileSize, char **fileName,
    char **relativeFilePath)
{
    uint8_t errorCode = ExtractFileAttr(fp, fileName, relativeFilePath, fileSize);
    if (errorCode != ERR_OK) {
        return errorCode;
    }

    if (!HasWrittenFile(appInstallPath, *relativeFilePath, *fileName, fp, fileSize)) {
        return ERR_APPEXECFWK_INSTALL_FAILED_CREATE_FILE_ERROR;
    }
    return ERR_OK;
}

uint8_t GtExtractorUtil::ExtractFileAttr(int32_t fp, char **fileName, char **relativeFilePath, uint64_t &fileSize)
{
    uint32_t nameLen = ReadInt(fp);
    if (nameLen == UINT_MAX) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read name Int fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }

    if ((*fileName = ReadString(fp, nameLen)) == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read fileName fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }

    uint32_t pathLen = ReadInt(fp);
    if (pathLen == UINT_MAX) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read path Int fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    } else {
        if ((*relativeFilePath = ReadString(fp, pathLen)) == nullptr) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read relativeFilePath fail");
            return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
        }
    }

    fileSize = ReadLong(fp);
    if (fileSize == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read fail size fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }
    return ERR_OK;
}

uint8_t GtExtractorUtil::ExtractFileAttr(int32_t fp, char **fileName, uint32_t &pathLen, uint64_t &fileSize)
{
    uint32_t nameLen = ReadInt(fp);
    if (nameLen == UINT_MAX) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read name Int fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }

    if ((*fileName = ReadString(fp, nameLen)) == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read fileName fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }

    pathLen = ReadInt(fp);
    if (pathLen == UINT_MAX) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read path Int fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    } else {
        int32_t ret = lseek(fp, pathLen, SEEK_CUR);
        if (ret < 0) {
            return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
        }
    }

    fileSize = ReadLong(fp);
    if (fileSize == 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] Read fail size fail");
        return ERR_APPEXECFWK_INSTALL_FAILED_FILE_DATA_INVALID;
    }
    return ERR_OK;
}

bool GtExtractorUtil::HasWrittenFile(const char *installPath, const char *path, const char *filename, int32_t fp,
    uint64_t size)
{
    if (installPath == nullptr || path == nullptr || filename == nullptr) {
        return false;
    }

    int32_t len = strlen(installPath) + strlen(path) + 1;
    char *destPath = reinterpret_cast<char *>(UI_Malloc(len));
    if (destPath == nullptr) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] malloc destPath addr fail");
        return false;
    }
    if (sprintf_s(destPath, len, "%s%s", installPath, path) < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] strcat destPath path fail");
        UI_Free(destPath);
        return false;
    }

    if (!BundleUtil::MkDirs(destPath)) {
        UI_Free(destPath);
        return false;
    }
    UI_Free(destPath);

    char destName[PATH_LENGTH] = { 0 };
    if (sprintf_s(destName, PATH_LENGTH, "%s%s/%s", installPath, path, filename) < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] strcat file path fail");
        return false;
    }

    if (!HasCopiedData(destName, fp, size)) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] copy file data fail");
        return false;
    }
    return true;
}

bool GtExtractorUtil::HasCopiedData(const char *filePath, int32_t fp, uint64_t size)
{
    uint64_t remain = size;
    uint64_t reading = (remain > READ_SIZE) ? READ_SIZE : remain;

    if (!BundleUtil::CheckRealPath(filePath)) {
        return false;
    }

    int32_t dfp = open(filePath, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
    if (dfp < 0) {
        HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] open file when copy file data");
        return false;
    }

    char *buf = reinterpret_cast<char *>(UI_Malloc(reading));
    if (buf == nullptr) {
        close(dfp);
        return false;
    }
    while (remain > 0) {
        int32_t ret = read(fp, buf, reading);
        if (ret < 0 || ret != reading) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] read file when copy file data");
            close(dfp);
            UI_Free(buf);
            return false;
        }
        ret = write(dfp, buf, reading);
        if (ret < 0 || ret != reading) {
            HILOG_ERROR(HILOG_MODULE_AAFWK, "[BMS] write file when copy file data");
            close(dfp);
            UI_Free(buf);
            return false;
        }
        remain = remain - reading;
        reading = (remain > READ_SIZE) ? READ_SIZE : remain;
    }
    UI_Free(buf);
    close(dfp);
    return true;
}
} // namespace OHOS