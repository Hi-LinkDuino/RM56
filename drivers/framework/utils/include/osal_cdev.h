/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OSAL_CDEV_H
#define OSAL_CDEV_H

#include "osal_cdev_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef __user
#define __user
#endif

struct OsalCdev;
struct file;

struct OsalCdevOps {
    int64_t (*seek)(struct file* filep, int64_t offset, int whence);
    ssize_t (*read)(struct file* filep, char __user* buffer, size_t buflen, int64_t* offset);
    ssize_t (*write)(struct file* filep, const char __user* buffer, size_t buflen, int64_t* offset);
    unsigned int (*poll)(struct file* filep, poll_table* pollTable);
    long (*ioctl)(struct file* filep, unsigned int cmd, unsigned long arg);
    int (*open)(struct OsalCdev* cdev, struct file* filep);
    int (*release)(struct OsalCdev* cdev, struct file* filep);
};

struct OsalCdev* OsalAllocCdev(const struct OsalCdevOps* fops);
int OsalRegisterCdev(struct OsalCdev* cdev, const char *name, unsigned int mode, void *priv);
void OsalUnregisterCdev(struct OsalCdev* cdev);
void OsalFreeCdev(struct OsalCdev* cdev);

void* OsalGetCdevPriv(struct OsalCdev* cdev);

void OsalSetFilePriv(struct file* filep, void *priv);
void* OsalGetFilePriv(struct file* filep);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSAL_CDEV_H */
/** @} */
