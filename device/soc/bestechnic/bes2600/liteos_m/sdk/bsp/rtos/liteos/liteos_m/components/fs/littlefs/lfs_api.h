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

#ifndef _LFS_API_H_
#define _LFS_API_H_

#include "fcntl.h"
#include "sys/stat.h"

#include "dirent.h"
#include "errno.h"
#include "fs_operations.h"
#include "lfs.h"
#include "lfs_conf.h"
#include "lfs_util.h"
#include "memory.h"
#include "pthread.h"

#define INVALID_FD (-1)

#ifndef VFS_ERROR
#define VFS_ERROR (-1)
#endif

#ifndef VFS_OK
#define VFS_OK  0
#endif

typedef struct {
    uint8_t useFlag;
    const char *pathName;
    lfs_t *lfsHandle;
    lfs_file_t file;
} LittleFsHandleStruct;

struct FileOpInfo {
    uint8_t useFlag;
    const struct FileOps *fsVops;
    char *dirName;
    lfs_t lfsInfo;
};

typedef struct {
    uint8_t useFlag;
    char *dirName;
    lfs_t *lfsHandle;
    lfs_dir_t dir;
} FileDirInfo;

LittleFsHandleStruct *GetFreeFd(int *fd);

int LfsMount(const char *source, const char *target, const char *fileSystemType, unsigned long mountflags,
    const void *data);

int LfsUmount(const char *target);
int LfsUnlink(const char *fileName);
int LfsMkdir(const char *dirName, mode_t mode);
int LfsRmdir(const char *dirName);
DIR *LfsOpendir(const char *dirName);
struct dirent *LfsReaddir(DIR *dir);
int LfsClosedir(DIR *dir);
int LfsOpen(const char *pathName, int openFlag, ...);
int LfsRead(int fd, void *buf, unsigned int len);
int LfsWrite(int fd, const void *buf, unsigned int len);
off_t LfsSeek(int fd, off_t offset, int whence);
int LfsClose(int fd);
int LfsRename(const char *oldName, const char *newName);
int LfsStat(const char *path, struct stat *buf);
int LfsFsync(int fd);
int LfsFstat(int fd, struct stat *buf);
int SetDefaultMountPath(int pathNameIndex, const char* target);
int LfsPread(int fd, void *buf, size_t nbyte, off_t offset);
int LfsPwrite(int fd, const void *buf, size_t nbyte, off_t offset);

#endif /* _LFS_API_H_ */

