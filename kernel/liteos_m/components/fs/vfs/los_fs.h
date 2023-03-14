/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd. All rights reserved.
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

/**
 * @defgroup los_fs fs
 * @ingroup kernel
 */

#ifndef _LOS_FS_H_
#define _LOS_FS_H_

#include "los_config.h"
#include "dirent.h"
#include "sys/mount.h"
#include "sys/statfs.h"
#include "sys/stat.h"
#include "sys/uio.h"
#include "unistd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int LOS_Open(const char *path, int flags, ...);
int LOS_Close(int fd);
ssize_t LOS_Read(int fd, void *buff, size_t bytes);
ssize_t LOS_Write(int fd, const void *buff, size_t bytes);
off_t LOS_Lseek(int fd, off_t off, int whence);
int LOS_Stat(const char *path, struct stat *stat);
int LOS_Statfs(const char *path, struct statfs *buf);
int LOS_Unlink(const char *path);
int LOS_Rename(const char *oldpath, const char *newpath);
int LOS_Fsync(int fd);
DIR *LOS_Opendir(const char *path);
struct dirent *LOS_Readdir(DIR *dir);
int LOS_Closedir(DIR *dir);
int LOS_Mkdir(const char *path, mode_t mode);
int LOS_Rmdir(const char *path);
int LOS_Lstat(const char *path, struct stat *buffer);
int LOS_Fstat(int fd, struct stat *buf);
int LOS_Fcntl(int fd, int cmd, ...);
int LOS_Ioctl(int fd, int req, ...);
ssize_t LOS_Readv(int fd, const struct iovec *iovBuf, int iovcnt);
ssize_t LOS_Writev(int fd, const struct iovec *iovBuf, int iovcnt);
ssize_t LOS_Pread(int fd, void *buff, size_t bytes, off_t off);
ssize_t LOS_Pwrite(int fd, const void *buff, size_t bytes, off_t off);
int LOS_Isatty(int fd);
int LOS_Access(const char *path, int amode);
int LOS_Ftruncate(int fd, off_t length);
int LOS_FsUmount(const char *target);
int LOS_FsUmount2(const char *target, int flag);
int LOS_FsMount(const char *source, const char *target,
          const char *fsType, unsigned long mountflags,
          const void *data);
/*
 * @brief Divide the device into partitions.
 *
 * @param dev Device name, which customized by caller, it is recommended to
 * name it as: "emmc0", "emmc1", "flash0", etc. The name is combined with
 * "device_type" + "device_id", and the last character is device_id.
 * device_id >= 0 && device_id <= 9.
 * @param fsType Filesystem type.
 * @param lengthArray List of partition size. For example:
 *     [0x10000000, 0x2000000], 256M and 512M partitions will be created and
 *     left all will not allocated.
 * @param partNum Length of 'lengthArray'.
 *
 * @return Return LOS_NOK if error. Return LOS_OK if success.
 * Partition naming rules:
 *     In the current vfs, after successfully calling the 'fdisk' hook,
 *     "partNum" partitions will be created.
 *     The partition naming rules is:
 *         The name is a combination of: 'deviceName'+'p'+'partitionId',
 *         such as "emmc0p0", "emmc0p1", "emmc0p2"...
 */
int LOS_DiskPartition(const char *dev, const char *fsType, int *lengthArray,
                      int partnum);

/*
 * @brief Format a partition.
 *
 * @param partName PartitionName, following the rule of fdisk hook.
 * @param data For FatFs, the data indicates a pointer to a byte which
 * specifies combination of FAT type flags, FM_FAT, FM_FAT32, FM_EXFAT and
 * bitwise-or of these three, FM_ANY.
 *
 * @return Return LOS_NOK if error. Return LOS_OK if success.
 */
int LOS_PartitionFormat(const char *partName, char *fsType, void *data);

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_FS_H_ */
