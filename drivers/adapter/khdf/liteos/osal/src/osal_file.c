/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "osal_file.h"
#include "fs/file.h"
#include "unistd.h"
#include "limits.h"
#include "sys/stat.h"
#include "hdf_log.h"

#define HDF_LOG_TAG osal_file

int32_t OsalFileOpen(OsalFile *file, const char *path, int flags, uint32_t rights)
{
    int32_t fd = -1;
    char pathName[PATH_MAX + 1] = {0};

    if (file == NULL || path == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    file->realFile = (void *)(uintptr_t)fd;

    if (realpath(path, pathName) == NULL) {
        HDF_LOGE("%s file name is invalid\n", __func__);
        return HDF_FAILURE;
    }

    fd = open(pathName, flags, rights);
    if (fd < 0) {
        HDF_LOGE("%s open file fail %d %u %d", __func__, flags, rights, errno);
        return HDF_FAILURE;
    }

    file->realFile = (void *)(uintptr_t)fd;
    return HDF_SUCCESS;
}

ssize_t OsalFileWrite(OsalFile *file, const void *string, uint32_t length)
{
    ssize_t ret;

    if (file == NULL || (int32_t)(uintptr_t)file->realFile == -1 || string == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = write((int32_t)(uintptr_t)file->realFile, string, length);
    if (ret < 0) {
        HDF_LOGE("%s write file fail %d %d", __func__, length, errno);
        return HDF_FAILURE;
    }

    return ret;
}

void OsalFileClose(OsalFile *file)
{
    if (file == NULL || (int32_t)(uintptr_t)file->realFile == -1) {
        HDF_LOGE("%s invalid param", __func__);
        return;
    }

    close((int32_t)(uintptr_t)file->realFile);
    file->realFile = (void *)(uintptr_t)-1;
}

ssize_t OsalFileRead(OsalFile *file, void *buf, uint32_t length)
{
    ssize_t ret;

    if (file == NULL || (int32_t)(uintptr_t)file->realFile == -1 || buf == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = read((int32_t)(uintptr_t)file->realFile, buf, length);
    if (ret < 0) {
        HDF_LOGE("%s read file fail %u %d", __func__, length, errno);
        return HDF_FAILURE;
    }

    return ret;
}

off_t OsalFileLseek(OsalFile *file, off_t offset, int32_t whence)
{
    off_t ret;

    if (file == NULL || (int32_t)(uintptr_t)file->realFile == -1) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = lseek((int32_t)(uintptr_t)file->realFile, offset, whence);
    if (ret < 0) {
        HDF_LOGE("%s lseek file fail %lld %d %d", __func__, offset, whence, errno);
        return HDF_FAILURE;
    }

    return ret;
}

