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

#include "hks_file_operator.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include "securec.h"

#include "hks_log.h"
#include "hks_mem.h"

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
    char *tmpFileName = (char *)HksMalloc(nameLen);
    if (tmpFileName == NULL) {
        return HKS_ERROR_MALLOC_FAIL;
    }
    (void)memset_s(tmpFileName, nameLen, 0, nameLen);

    int32_t ret = GetFileName(path, fileName, tmpFileName, nameLen);
    if (ret != HKS_SUCCESS) {
        HKS_LOG_E("get full fileName failed");
        HKS_FREE_PTR(tmpFileName);
        return ret;
    }

    *fullFileName = tmpFileName;
    return HKS_SUCCESS;
}

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
        HKS_LOG_E("invalid filePath, path %s", filePath);
        return 0;
    }

    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        HKS_LOG_E("failed to open file");
        return 0;
    }

    uint32_t size = fread(buf, 1, len, fp);
    if (fclose(fp) < 0) {
        HKS_LOG_E("failed to close file");
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
        HKS_LOG_E("file stat fail.");
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
        HKS_LOG_E("invalid filePath, path %s", filePath);
        return HKS_ERROR_INVALID_KEY_FILE;
    }

    /* caller function ensures that the folder exists */
    FILE *fp = fopen(filePath, "wb+");
    if (fp == NULL) {
        HKS_LOG_E("open file fail");
        return HKS_ERROR_OPEN_FILE_FAIL;
    }

    if (chmod(filePath, S_IRUSR | S_IWUSR) < 0) {
        HKS_LOG_E("chmod file fail.");
        fclose(fp);
        return HKS_ERROR_OPEN_FILE_FAIL;
    }

    uint32_t size = fwrite(buf, 1, len, fp);
    if (size != len) {
        HKS_LOG_E("write file size fail.");
        fclose(fp);
        return HKS_ERROR_WRITE_FILE_FAIL;
    }

    if (fclose(fp) < 0) {
        HKS_LOG_E("failed to close file");
        return HKS_ERROR_CLOSE_FILE_FAIL;
    }

    return HKS_SUCCESS;
}

static int32_t FileRemove(const char *fileName)
{
    int32_t ret = IsFileExist(fileName);
    if (ret != HKS_SUCCESS) {
        return HKS_SUCCESS; /* if file not exist, return ok */
    }

    struct stat tmp;
    if (stat(fileName, &tmp) != 0) {
        return HKS_ERROR_INTERNAL_ERROR;
    }

    if (S_ISDIR(tmp.st_mode)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    if ((unlink(fileName) != 0) && (errno != ENOENT)) {
        HKS_LOG_E("failed to remove file: filename = %s, errno = 0x%x", fileName, errno);
        return HKS_ERROR_REMOVE_FILE_FAIL;
    }

    return HKS_SUCCESS;
}

int32_t HksFileRemove(const char *path, const char *fileName)
{
    if (fileName == NULL) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }

    char *fullFileName = NULL;
    int32_t ret = GetFullFileName(path, fileName, &fullFileName);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = FileRemove(fullFileName);
    HKS_FREE_PTR(fullFileName);
    return ret;
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
    HKS_FREE_PTR(fullFileName);
    return ret;
}

int32_t HksIsDirExist(const char *path)
{
    if (path == NULL) {
        return HKS_ERROR_NULL_POINTER;
    }
    return IsFileExist(path);
}

int32_t HksMakeDir(const char *path)
{
    int result = mkdir(path, S_IRWXU);
    if (result == 0) {
        return HKS_SUCCESS;
    } else {
        switch (errno) {
            case EEXIST:
                return HKS_ERROR_ALREADY_EXISTS;
                break;
            default:
                return HKS_ERROR_MAKE_DIR_FAIL;
                break;
        }
    }
}

void *HksOpenDir(const char *path)
{
    return (void *)opendir(path);
}

int32_t HksCloseDir(void *dirp)
{
    return closedir((DIR *)dirp);
}

int32_t HksGetDirFile(void *dirp, struct HksFileDirentInfo *direntInfo)
{
    DIR *dir = (DIR *)dirp;
    struct dirent *dire = readdir(dir);

    while (dire != NULL) {
        if (dire->d_type != DT_REG) { /* only care about files. */
            dire = readdir(dir);
            continue;
        }

        uint32_t len = strlen(dire->d_name);
        if (memcpy_s(direntInfo->fileName, sizeof(direntInfo->fileName) - 1, dire->d_name, len) != EOK) {
            return HKS_ERROR_BAD_STATE;
        }
        direntInfo->fileName[len] = '\0';
        return HKS_SUCCESS;
    }

    return HKS_ERROR_NOT_EXIST;
}

int32_t HksGetStoragePath(enum HksStoragePathType pathType, char *path, uint32_t *len)
{
    if ((path == NULL) || (len == NULL) || (*len <= 1)) {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    errno_t ret;
    uint32_t pathLen;
    if (pathType == HKS_STORAGE_MAIN_PATH) {
        pathLen = strlen(HKS_KEY_STORE_PATH);
        ret = memcpy_s(path, *len - 1, HKS_KEY_STORE_PATH, pathLen);
    } else if (pathType == HKS_STORAGE_BACKUP_PATH) {
        pathLen = strlen(HKS_KEY_STORE_BAK_PATH);
        ret = memcpy_s(path, *len - 1, HKS_KEY_STORE_BAK_PATH, pathLen);
    } else {
        return HKS_ERROR_INVALID_ARGUMENT;
    }
    if (ret != EOK) {
        HKS_LOG_E("memcpy failed");
        return HKS_ERROR_BAD_STATE;
    }
    path[pathLen] = '\0';
    *len = pathLen + 1;
    return HKS_SUCCESS;
}

int32_t HksRemoveDir(const char *dirPath)
{
    struct stat fileStat;
    int32_t ret = stat(dirPath, &fileStat);
    if (ret != 0) {
        HKS_LOG_E("file stat failed");
        return HKS_FAILURE;
    }

    if (!S_ISDIR(fileStat.st_mode)) {
        HKS_LOG_E("path is not dir");
        return HKS_FAILURE;
    }

    DIR *dir = opendir(dirPath);
    if (dir  == NULL) {
        HKS_LOG_E("open dir failed");
        return HKS_ERROR_OPEN_FILE_FAIL;
    }

    struct dirent *dire = readdir(dir);
    while (dire != NULL) {
        if (dire->d_type == DT_REG) { /* only care about files. */
            ret = HksFileRemove(dirPath, dire->d_name);
            if (ret != HKS_SUCCESS) { /* Continue to delete remaining files */
                HKS_LOG_E("remove file failed when remove dir files, ret = %d.", ret);
            }
        }
        dire = readdir(dir);
    }

    closedir(dir);
    return HKS_SUCCESS;
}

int32_t HksDeletDirPartTwo(const char *path)
{
    int32_t ret;
    char deletePath[HKS_MAX_FILE_NAME_LEN] = {0};
    DIR *dir = opendir(path);
    if (dir  == NULL) {
        HKS_LOG_E("open dir failed");
        return HKS_ERROR_OPEN_FILE_FAIL;
    }
    struct dirent *dire = readdir(dir);
    while (dire != NULL) {
        if (strncpy_s(deletePath, sizeof(deletePath), path, strlen(path)) != EOK) {
            closedir(dir);
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if (deletePath[strlen(deletePath) - 1] != '/') {
            if (strncat_s(deletePath, sizeof(deletePath), "/", strlen("/")) != EOK) {
                closedir(dir);
                return HKS_ERROR_INTERNAL_ERROR;
            }
        }

        if (strncat_s(deletePath, sizeof(deletePath), dire->d_name, strlen(dire->d_name)) != EOK) {
            closedir(dir);
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if ((strcmp("..", dire->d_name) != 0) && (strcmp(".", dire->d_name) != 0)) {
            ret = remove(deletePath);
        }
        dire = readdir(dir);
    }
    closedir(dir);
    ret = remove(path);
    return ret;
}

int32_t HksDeletDirPartOne(const char *path)
{
    int32_t ret;
    char deletePath[HKS_MAX_FILE_NAME_LEN] = {0};
    DIR *dir = opendir(path);
    if (dir  == NULL) {
        HKS_LOG_E("open dir failed");
        return HKS_ERROR_OPEN_FILE_FAIL;
    }
    struct dirent *dire = readdir(dir);
    while (dire != NULL) {
        if (strncpy_s(deletePath, sizeof(deletePath), path, strlen(path)) != EOK) {
            closedir(dir);
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if (deletePath[strlen(deletePath) - 1] != '/') {
            if (strncat_s(deletePath, sizeof(deletePath), "/", strlen("/")) != EOK) {
                closedir(dir);
                return HKS_ERROR_INTERNAL_ERROR;
            }
        }

        if (strncat_s(deletePath, sizeof(deletePath), dire->d_name, strlen(dire->d_name)) != EOK) {
            closedir(dir);
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if (dire->d_type == DT_DIR && (strcmp("..", dire->d_name) != 0) && (strcmp(".", dire->d_name) != 0)) {
            HksDeletDirPartTwo(deletePath);
        } else if (dire->d_type != DT_DIR) {
            ret = remove(deletePath);
        }
        dire = readdir(dir);
    }
    closedir(dir);
    ret = remove(path);
    return ret;
}

int32_t HksDeleteDir(const char *path)
{
    int32_t ret;
    char deletePath[HKS_MAX_FILE_NAME_LEN] = { 0 };

    DIR *dir = opendir(path);
    if (dir  == NULL) {
        HKS_LOG_E("open dir failed");
        return HKS_ERROR_OPEN_FILE_FAIL;
    }
    struct dirent *dire = readdir(dir);
    while (dire != NULL) {
        if (strncpy_s(deletePath, sizeof(deletePath), path, strlen(path)) != EOK) {
            closedir(dir);
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if (deletePath[strlen(deletePath) - 1] != '/') {
            if (strncat_s(deletePath, sizeof(deletePath), "/", strlen("/")) != EOK) {
                closedir(dir);
                return HKS_ERROR_INTERNAL_ERROR;
            }
        }

        if (strncat_s(deletePath, sizeof(deletePath), dire->d_name, strlen(dire->d_name)) != EOK) {
            closedir(dir);
            return HKS_ERROR_INTERNAL_ERROR;
        }

        if (dire->d_type == DT_DIR && (strcmp("..", dire->d_name) != 0) && (strcmp(".", dire->d_name) != 0)) {
            HksDeletDirPartOne(deletePath);
        } else if (dire->d_type != DT_DIR) {
            ret = remove(deletePath);
        }
        dire = readdir(dir);
    }
    closedir(dir);
    ret = remove(path);
    return ret;
}

uint32_t HksFileRead(const char *path, const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len)
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
    HKS_FREE_PTR(fullFileName);
    return size;
}

int32_t HksFileWrite(const char *path, const char *fileName, uint32_t offset, const uint8_t *buf, uint32_t len)
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
    HKS_FREE_PTR(fullFileName);
    return ret;
}

uint32_t HksFileSize(const char *path, const char *fileName)
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
    HKS_FREE_PTR(fullFileName);
    return size;
}

int32_t HksGetFileName(const char *path, const char *fileName, char *fullFileName, uint32_t fullFileNameLen)
{
    return GetFileName(path, fileName, fullFileName, fullFileNameLen);
}