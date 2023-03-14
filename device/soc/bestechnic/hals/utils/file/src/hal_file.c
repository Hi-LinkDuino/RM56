/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "hal_trace.h"
#include "utils_file.h"
#include "hal_file.h"

#define PATH_NAME_LEN 64
#define INVALID_KEY 92
static int check_path_valid(const char *path)
{
    if (strlen(path) > PATH_NAME_LEN) {
        TRACE(0, "path name is too long ,must less than %d", PATH_NAME_LEN);
        return -1;
    }

    for (int i = 0; i < strlen(path); i++) {
        if ((int)path[i] == INVALID_KEY) {
            return -1;
        }
    }

    return 0;
}

int HalFileOpen(const char *path, int oflag, int mode)
{
    (void)mode;

    if (check_path_valid(path) < 0) {
        return -1;
    }

    int ret = open(path, oflag);
    return ret;
}

int HalFileClose(int fd)
{
    return close(fd);
}

int HalFileRead(int fd, char *buf, unsigned int len)
{
    if (fd > LOSCFG_LFS_MAX_OPEN_FILES) {
        return -1;
    }

    return read(fd, buf, len);
}

int HalFileWrite(int fd, const char *buf, unsigned int len)
{
    if (fd > LOSCFG_LFS_MAX_OPEN_FILES) {
        return -1;
    }

    return write(fd, buf, len);
}

int HalFileDelete(const char *path)
{
    return unlink(path);
}

int HalFileStat(const char *path, unsigned int *fileSize)
{
    struct stat st_buf = {0};
    if (stat(path, &st_buf) != 0) {
        return -1;
    }

    if (fileSize) {
        *fileSize = st_buf.st_size;
    }

    return 0;
}

int HalFileSeek(int fd, int offset, unsigned int whence)
{
    if (fd > LOSCFG_LFS_MAX_OPEN_FILES) {
        return -1;
    }

    int len = (int)lseek(fd, (off_t)0, SEEK_END_FS);
    if (offset > len) {
        return -1;
    }

    return (int)lseek(fd, (off_t)offset, whence);
}
