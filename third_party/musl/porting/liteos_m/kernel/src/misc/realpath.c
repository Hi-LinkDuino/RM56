/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <securec.h>

/*
 * LiteOS-M file system uses abs path
 */

static unsigned int vfs_strnlen(const char *str, size_t maxlen)
{
    const char *p = NULL;

    for (p = str; ((maxlen-- != 0) && (*p != '\0')); ++p) {}

    return p - str;
}

char *realpath(const char *__restrict path, char *__restrict resolved_path)
{
    int namelen;
    int res;
    struct stat st;

    if (!*path) {
        errno = ENOENT;
        return NULL;
    }

    namelen = vfs_strnlen(path, PATH_MAX);
    if (!namelen) {
        errno = EINVAL;
        return NULL;
    } else if (namelen >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return NULL;
    }

    // LiteOS-M file system uses abs path, so we just stat path and strcpy it.
    if (stat(path, &st) < 0) {
        errno = ENOENT;
        return NULL;
    }

    res = strcpy_s(resolved_path, PATH_MAX, path);
    if (res != 0) {
        errno = EINVAL;
        return NULL;
    } else {
        return resolved_path;
    }
}
