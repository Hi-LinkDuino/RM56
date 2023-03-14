/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "hc_file.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hc_log.h"
#include "hc_types.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_FILE_PATH_SIZE 256
#define MAX_FOLDER_NAME_SIZE 128

static int32_t CreateDirectory(const char *filePath)
{
    int32_t ret;
    errno_t eno;
    char *chPtr = NULL;
    char dirCache[MAX_FOLDER_NAME_SIZE];

    chPtr = (char *)filePath;
    while ((chPtr = strchr(chPtr, '/')) != NULL) {
        unsigned long len = (unsigned long)((uintptr_t)chPtr - (uintptr_t)filePath);
        if (len == 0ul) {
            chPtr++;
            continue;
        }
        eno = memcpy_s(dirCache, sizeof(dirCache), filePath, len);
        if (eno != EOK) {
            LOGE("memory copy failed");
            return -1;
        }
        dirCache[len] = 0;
        if (access(dirCache, F_OK) != 0) {
            ret = mkdir(dirCache, S_IRWXU);
            if (ret != 0) {
                LOGE("make dir failed, err code %d", ret);
                return -1;
            }
        }
        chPtr++;
    }
    return 0;
}

static FILE *HcFileOpenRead(const char *path)
{
    return fopen(path, "rb");
}

static FILE *HcFileOpenWrite(const char *path)
{
    if (access(path, F_OK) != 0) {
        int32_t ret = CreateDirectory(path);
        if (ret != 0) {
            return NULL;
        }
    }
    return fopen(path, "w+");
}

int HcFileOpen(const char *path, int mode, FileHandle *file)
{
    if (path == NULL || file == NULL) {
        return -1;
    }
    if (mode == MODE_FILE_READ) {
        file->pfd = HcFileOpenRead(path);
    } else {
        file->pfd = HcFileOpenWrite(path);
    }
    if (file->pfd == NULL) {
        return -1;
    } else {
        return 0;
    }
}

int HcFileSize(FileHandle file)
{
    FILE *fp = (FILE *)file.pfd;
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) != 0) {
            return -1;
        }
        int size = ftell(fp);
        if (fseek(fp, 0L, SEEK_SET) != 0) {
            return -1;
        }
        return size;
    } else {
        return -1;
    }
}

int HcFileRead(FileHandle file, void *dst, int dstSize)
{
    FILE *fp = (FILE *)file.pfd;
    if (fp == NULL || dstSize < 0 || dst == NULL) {
        return -1;
    }

    char *dstBuffer = (char *)dst;
    int total = 0;
    while (total < dstSize) {
        int readCount = fread(dstBuffer + total, 1, dstSize - total, fp);
        if (ferror(fp) != 0) {
            LOGE("read file error!");
        }
        if (readCount == 0) {
            return total;
        }
        total += readCount;
    }

    return total;
}

int HcFileWrite(FileHandle file, const void *src, int srcSize)
{
    FILE *fp = (FILE *)file.pfd;
    if (fp == NULL || srcSize < 0 || src == NULL) {
        return -1;
    }

    const char *srcBuffer = (const char *)src;
    int total = 0;
    while (total < srcSize) {
        int writeCount = fwrite(srcBuffer + total, 1, srcSize - total, fp);
        if (ferror(fp) != 0) {
            LOGE("write file error!");
        }
        total += writeCount;
    }
    return total;
}

void HcFileClose(FileHandle file)
{
    FILE *fp = (FILE *)file.pfd;
    if (fp == NULL) {
        return;
    }

    (void)fclose(fp);
}

void HcFileRemove(const char *path)
{
    if (path == NULL) {
        LOGE("Invalid file path");
        return;
    }
    (void)remove(path);
}

void HcFileGetSubFileName(const char *path, StringVector *nameVec)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    if ((dir = opendir(path)) == NULL) {
        LOGI("opendir failed!");
        return;
    }
    while ((entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) {
            continue;
        }
        HcString subFileName = CreateString();
        if (!StringSetPointer(&subFileName, entry->d_name)) {
            LOGE("Failed to copy subFileName!");
            DeleteString(&subFileName);
            continue;
        }
        if (nameVec->pushBackT(nameVec, subFileName) == NULL) {
            LOGE("Failed to push path to pathVec!");
            DeleteString(&subFileName);
        }
    }
    if (closedir(dir) < 0) {
        LOGE("Failed to close file");
    }
}

#ifdef __cplusplus
}
#endif
