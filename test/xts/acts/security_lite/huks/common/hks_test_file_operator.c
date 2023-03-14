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

#include "hks_test_file_operator.h"

#ifndef __LITEOS_M__
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

/* use product definitions temporarily */
#define DEFAULT_FILE_PERMISSION 0666
#else

#include <utils_file.h>

#endif /* _STORAGE_LITE_ */


#include "hks_param.h"
#include "hks_test_log.h"
#include "hks_test_mem.h"
#include "hks_type.h"
#include "securec.h"

#define HKS_MAX_FILE_NAME_LEN 512

static int32_t GetFileName(const char *path, const char *fileName, char *fullFileName, uint32_t fullFileNameLen)
{
    if (path != NULL) {
        if (strncpy_s(fullFileName, fullFileNameLen, path, strlen(path)) != EOK) {
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if (path[strlen(path) - 1] != '/') {
            if (strncat_s(fullFileName, fullFileNameLen, "/", strlen("/")) != EOK) {
                return HKS_ERROR_INTERNAL_ERROR;
            }
        }

        if (strncat_s(fullFileName, fullFileNameLen, fileName, strlen(fileName)) != EOK) {
            return HKS_ERROR_INTERNAL_ERROR;
        }
    } else {
        if (strncpy_s(fullFileName, fullFileNameLen, fileName, strlen(fileName)) != EOK) {
            return HKS_ERROR_INTERNAL_ERROR;
        }
    }

    return HKS_SUCCESS;
}

static int32_t GetFullFileName(const char *path, const char *fileName, char **fullFileName)
{
    uint32_t nameLen = HKS_MAX_FILE_NAME_LEN;
    char *tmpFileName = (char *)HksTestMalloc(nameLen);
    if (tmpFileName == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(tmpFileName, nameLen, 0, nameLen);

    int32_t ret = GetFileName(path, fileName, tmpFileName, nameLen);
    if (ret != HKS_SUCCESS) {
        HKS_TEST_LOG_E("get full fileName failed");
        HksTestFree(tmpFileName);
        return ret;
    }

    *fullFileName = tmpFileName;
    return HKS_SUCCESS;
}

#ifndef __LITEOS_M__
static int32_t IsFileExist(const char *fileName)
{
    if (access(fileName, F_OK) != 0) {
        return HKS_ERROR_NOT_EXIST;
    }

    return HKS_SUCCESS;
}

static uint32_t FileRead(const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len)
{
    (void)offset;
    if (IsFileExist(fileName) != HKS_SUCCESS) {
        return 0;
    }

    char filePath[PATH_MAX + 1] = {0};
    (void)realpath(fileName, filePath);
    if (strstr(filePath, "../") != NULL) {
        HKS_TEST_LOG_E("invalid filePath, path %s", filePath);
        return 0;
    }

    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        HKS_TEST_LOG_E("failed to open file");
        return 0;
    }

    uint32_t size = fread(buf, 1, len, fp);
    if (fclose(fp) < 0) {
        HKS_TEST_LOG_E("failed to close file");
        return 0;
    }

    return size;
}

static uint32_t FileSize(const char *fileName)
{
    if (IsFileExist(fileName) != HKS_SUCCESS) {
        return 0;
    }

    struct stat fileStat;
    (void)memset_s(&fileStat, sizeof(fileStat), 0, sizeof(fileStat));
    if (stat(fileName, &fileStat) != 0) {
        HKS_TEST_LOG_E("file stat fail.");
        return 0;
    }

    return fileStat.st_size;
}

static int32_t FileWrite(const char *fileName, uint32_t offset, const uint8_t *buf, uint32_t len)
{
    (void)offset;
    char filePath[PATH_MAX + 1] = {0};
    if (memcpy_s(filePath, sizeof(filePath) - 1, fileName, strlen(fileName)) != EOK) {
        return HKS_ERROR_BAD_STATE;
    }
    (void)realpath(fileName, filePath);
    if (strstr(filePath, "../") != NULL) {
        HKS_TEST_LOG_E("invalid filePath, path %s", filePath);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    /* caller function ensures that the folder exists */
    FILE *fp = fopen(filePath, "wb+");
    if (fp == NULL) {
        HKS_TEST_LOG_E("open file fail");
        return HKS_ERROR_OPEN_FILE_FAIL;
    }

    if (chmod(filePath, S_IRUSR | S_IWUSR) < 0) {
        HKS_TEST_LOG_E("chmod file fail.");
        fclose(fp);
        return HKS_ERROR_OPEN_FILE_FAIL;
    }

    uint32_t size = fwrite(buf, 1, len, fp);
    if (size != len) {
        HKS_TEST_LOG_E("write file size fail.");
        fclose(fp);
        return HKS_ERROR_WRITE_FILE_FAIL;
    }

    if (fclose(fp) < 0) {
        HKS_TEST_LOG_E("failed to close file");
        return HKS_ERROR_CLOSE_FILE_FAIL;
    }

    return HKS_SUCCESS;
}

int32_t HksIsFileExist(const char *path, const char *fileName)
{
    if (fileName == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }

    char *fullFileName = NULL;
    int32_t ret = GetFullFileName(path, fileName, &fullFileName);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = IsFileExist(fullFileName);
    HksTestFree(fullFileName);
    return ret;
}
#else
static uint32_t FileRead(const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len)
{
    /* now offset is 0, but we maybe extend hi1131 file interfaces in the future */
    if (offset != 0) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    unsigned int fileSize;
    int32_t ret = UtilsFileStat(fileName, &fileSize);
    if (ret < 0) {
        HKS_TEST_LOG_E("stat file failed, errno = 0x%x", ret);
        return 0;
    }

    if (len > fileSize) {
        HKS_TEST_LOG_E("read data over file size!\n, file size = %d\n, buf len = %d\n", fileSize, len);
        return 0;
    }

    int fd = UtilsFileOpen(fileName, O_RDONLY_FS, 0);
    if (fd < 0) {
        HKS_TEST_LOG_E("failed to open file, errno = 0x%x", fd);
        return 0;
    }

    ret = UtilsFileRead(fd, buf, len);
    UtilsFileClose(fd);
    if (ret < 0) {
        HKS_TEST_LOG_E("failed to read file, errno = 0x%x", ret);
        return 0;
    }

    return len;
}

static int32_t FileWrite(const char *fileName, uint32_t offset, const uint8_t *buf, uint32_t len)
{
    /* now offset is 0, but we maybe extend hi1131 file interfaces in the future */
    if (offset != 0) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    int fd = UtilsFileOpen(fileName, O_CREAT_FS | O_TRUNC_FS | O_RDWR_FS, 0);
    if (fd < 0) {
        HKS_TEST_LOG_E("failed to open key file, errno = 0x%x\n", fd);
        return HKS_ERROR_OPEN_FILE_FAIL;
    }

    int32_t ret = UtilsFileWrite(fd, buf, len);
    if (ret < 0) {
        HKS_TEST_LOG_E("failed to write key file, errno = 0x%x\n", ret);
        ret = HKS_ERROR_WRITE_FILE_FAIL;
    }

    ret = UtilsFileClose(fd);
    if (ret < 0) {
        HKS_TEST_LOG_E("failed to close file, errno = 0x%x\n", ret);
        ret = HKS_ERROR_CLOSE_FILE_FAIL;
    }

    return ret;
}

static uint32_t FileSize(const char *fileName)
{
    unsigned int fileSize;
    int32_t ret = UtilsFileStat(fileName, &fileSize);
    if (ret < 0) {
        HKS_TEST_LOG_E("stat file failed, errno = 0x%x", ret);
        return 0;
    }
    return fileSize;
}

int32_t HksTestFileRemove(const char *path, const char *fileName)
{
    char *fullFileName = NULL;
    int32_t ret = GetFullFileName(path, fileName, &fullFileName);
    if (ret != HKS_SUCCESS) {
        return 0;
    }

    return UtilsFileDelete(fullFileName);
}

#endif
uint32_t HksTestFileRead(const char *path, const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len)
{
    if ((fileName == NULL) || (buf == NULL) || (len == 0)) {
        return 0;
    }

    char *fullFileName = NULL;
    int32_t ret = GetFullFileName(path, fileName, &fullFileName);
    if (ret != HKS_SUCCESS) {
        return 0;
    }

    uint32_t size = FileRead(fullFileName, offset, buf, len);
    HksTestFree(fullFileName);
    return size;
}

int32_t HksTestFileWrite(const char *path, const char *fileName, uint32_t offset, const uint8_t *buf, uint32_t len)
{
    if ((fileName == NULL) || (buf == NULL) || (len == 0)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    char *fullFileName = NULL;
    int32_t ret = GetFullFileName(path, fileName, &fullFileName);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = FileWrite(fullFileName, offset, buf, len);
    HksTestFree(fullFileName);
    return ret;
}

uint32_t HksTestFileSize(const char *path, const char *fileName)
{
    if (fileName == NULL) {
        return 0;
    }

    char *fullFileName = NULL;
    int32_t ret = GetFullFileName(path, fileName, &fullFileName);
    if (ret != HKS_SUCCESS) {
        return 0;
    }

    uint32_t size = FileSize(fullFileName);
    HksTestFree(fullFileName);
    return size;
}
