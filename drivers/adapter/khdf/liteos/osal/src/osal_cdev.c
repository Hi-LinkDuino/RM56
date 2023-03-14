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

#include <linux/fs.h>
#include "hdf_log.h"
#include "hdf_io_service.h"
#include "osal_cdev.h"
#include "osal_file.h"
#include "osal_mem.h"
#include "osal_uaccess.h"

#define HDF_LOG_TAG osal_cdev

struct OsalCdev {
    struct file_operations_vfs fops;
    const struct OsalCdevOps* opsImpl;
    const char* path;
    void* priv;
};

int OsalCdevOpen(struct file* filep)
{
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->open(dev, filep);
}

int OsalCdevRelease(struct file* filep)
{
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->release(dev, filep);
}

ssize_t OsalCdevRead(struct file* filep, char* buffer, size_t buflen)
{
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->read(filep, buffer, buflen, 0);
}

ssize_t OsalCdevWrite(struct file* filep, const char* buffer, size_t buflen)
{
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->write(filep, buffer, buflen, 0);
}

off_t OsalCdevSeek(struct file* filep, off_t offset, int whence)
{
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->seek(filep, offset, whence);
}

int OsalCdevIoctl(struct file* filep, int cmd, unsigned long arg)
{
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->ioctl(filep, cmd, arg);
}

int OsalCdevPoll(struct file* filep, poll_table* fds)
{
    if (filep == NULL || filep->f_vnode == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    struct drv_data* drvData = (struct drv_data* )filep->f_vnode->data;
    struct OsalCdev* dev = (struct OsalCdev* )drvData->priv;
    return dev->opsImpl->poll(filep, fds);
}

static void AssignFileOps(struct file_operations_vfs* fops, const struct OsalCdevOps* src)
{
    fops->seek = src->seek != NULL ? OsalCdevSeek : NULL;
    fops->read = src->read != NULL ? OsalCdevRead : NULL;
    fops->write = src->write != NULL ? OsalCdevWrite : NULL;
    fops->poll = src->poll != NULL ? OsalCdevPoll : NULL;
    fops->ioctl = src->ioctl != NULL ? OsalCdevIoctl : NULL;
    fops->open = src->open != NULL ? OsalCdevOpen : NULL;
    fops->close = src->release != NULL ? OsalCdevRelease : NULL;
}

struct OsalCdev* OsalAllocCdev(const struct OsalCdevOps* fops)
{
    struct OsalCdev* cdev = OsalMemCalloc(sizeof(struct OsalCdev));
    if (cdev == NULL) {
        return NULL;
    }

    AssignFileOps(&cdev->fops, fops);
    cdev->opsImpl = fops;

    return cdev;
}

int OsalRegisterCdev(struct OsalCdev* cdev, const char* name, unsigned int mode, void* priv)
{
    if (cdev == NULL || name == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("%s:register %s", __func__, name);
    static bool devPathInitted = false;
    if (!devPathInitted && mkdir(DEV_NODE_PATH, DEV_NODE_PATH_MODE) == 0) {
        devPathInitted = true;
    }
    int ret =  register_driver(name, &cdev->fops, mode, cdev);
    if (ret == HDF_SUCCESS) {
        cdev->priv = priv;
        cdev->path = name;
    }
    return ret;
}

void OsalUnregisterCdev(struct OsalCdev* cdev)
{
    if (cdev != NULL) {
        unregister_driver(cdev->path);
    }
}

void OsalFreeCdev(struct OsalCdev* cdev)
{
    if (cdev != NULL) {
        OsalMemFree(cdev);
    }
}

void* OsalGetCdevPriv(struct OsalCdev* cdev)
{
    return cdev != NULL ? cdev->priv : NULL;
}

void OsalSetFilePriv(struct file* filep, void* priv)
{
    if (filep != NULL) {
        filep->f_priv = priv;
    }
}

void* OsalGetFilePriv(struct file* filep)
{
    return filep != NULL ? filep->f_priv : NULL;
}
