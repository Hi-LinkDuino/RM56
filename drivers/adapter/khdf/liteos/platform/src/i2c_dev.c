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

#include "i2c_dev.h"
#include <fs/driver.h>
#include "i2c_core.h"
#include "osal_mem.h"
#include "user_copy.h"

#define I2C_FS_MODE                 0660
#define I2C_RDWR_IOCTL_MAX_MSGS     42
#define I2C_BUF_MAX                 8192
#define I2C_NAME_SIZE               32
#define I2C_CNTLR_MAX               32

struct I2cClient {
    DevHandle handle;
    struct I2cHost *host;
    uint16_t addr;
    uint16_t flags;
};

static inline int32_t I2cMsgInitFromUser(struct I2cMsg *kMsgs, struct i2c_msg *uMsgs)
{
    int32_t ret;

    kMsgs->len = uMsgs->len;
    kMsgs->addr = uMsgs->addr;
    kMsgs->flags = uMsgs->flags;
    if ((uMsgs->flags & I2C_M_RD) != 0) {
        return HDF_SUCCESS;
    }
    ret = LOS_CopyToKernel((void *)(kMsgs->buf), kMsgs->len, (void *)(uMsgs->buf), uMsgs->len);
    return (ret == LOS_OK) ? HDF_SUCCESS : HDF_ERR_IO;
}

static inline int32_t I2cMsgBackToUser(struct I2cMsg *kMsgs, struct i2c_msg *uMsgs)
{
    int32_t ret;

    if ((kMsgs->flags & I2C_M_RD) == 0) {
        return HDF_SUCCESS;
    }
    ret = LOS_CopyFromKernel((void *)(uMsgs->buf), uMsgs->len, (void *)(kMsgs->buf), kMsgs->len);
    return (ret == LOS_OK) ? HDF_SUCCESS : HDF_ERR_IO;
}

static inline int32_t I2cMsgsCopyBackToUser(I2cIoctlWrap *wrap,
    struct I2cMsg *kMsgs, struct i2c_msg *uMsgs)
{
    int ret;
    unsigned int i;

    for (i = 0; i < wrap->nmsgs; i++) {
        ret = I2cMsgBackToUser(&kMsgs[i], &uMsgs[i]);
        if (ret != HDF_SUCCESS) {
            break;
        }
    }
    return ret;
}

static void I2cMsgsDestroy(struct I2cMsg *kMsgs, struct i2c_msg *uMsgs)
{
    if (kMsgs != NULL) {
        if (kMsgs[0].buf != NULL) {
            OsalMemFree(kMsgs[0].buf);
            kMsgs[0].buf = NULL;
        }
    }
    if (uMsgs != NULL) {
        OsalMemFree(uMsgs);
    }
}

static int32_t I2cMsgsCreateFromUser(I2cIoctlWrap *wrap,
    struct I2cMsg **kMsgsPp, struct i2c_msg **uMsgsPp)
{
    int32_t ret;
    size_t i;
    size_t bufLen;
    struct i2c_msg *uMsgs = NULL;
    struct I2cMsg *kMsgs = NULL;
    uint8_t *dataBuf = NULL;

    uMsgs = (struct i2c_msg *)OsalMemCalloc((sizeof(*uMsgs) + sizeof(*kMsgs)) * wrap->nmsgs);
    if (uMsgs == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    kMsgs = (struct I2cMsg *)((uint8_t *)uMsgs + sizeof(*uMsgs) * wrap->nmsgs);

    ret = LOS_CopyToKernel((void *)uMsgs, sizeof(*uMsgs) * wrap->nmsgs,
        (void *)wrap->msgs, sizeof(*uMsgs) * wrap->nmsgs);
    if (ret != LOS_OK) {
        HDF_LOGE("%s: copy msgs from user fail!", __func__);
        goto __ERR__;
    }

    for (i = 0, bufLen = 0; i < wrap->nmsgs; i++) {
        if (uMsgs[i].buf == NULL || uMsgs[i].len == 0) {
            ret = HDF_ERR_INVALID_PARAM;
            goto __ERR__;
        }
        bufLen += uMsgs[i].len;
    }
    if (bufLen >= I2C_BUF_MAX) {
        HDF_LOGE("%s: buf too long:%u", __func__, bufLen);
        ret = HDF_ERR_INVALID_PARAM;
        goto __ERR__;
    }

    dataBuf = (uint8_t *)OsalMemCalloc(bufLen);
    if (dataBuf == NULL) {
        ret = HDF_ERR_MALLOC_FAIL;
        goto __ERR__;
    }

    for (i = 0; i < wrap->nmsgs; i++) {
        kMsgs[i].buf = dataBuf;
        dataBuf += uMsgs[i].len;
        ret = I2cMsgInitFromUser(&kMsgs[i], &uMsgs[i]);
        if (ret != HDF_SUCCESS) {
            goto __ERR__;
        }
    }

    *kMsgsPp = kMsgs;
    *uMsgsPp = uMsgs;
    return HDF_SUCCESS;

__ERR__:
    I2cMsgsDestroy(kMsgs, uMsgs);
    return ret;
}

static int32_t I2cCntlrRead(DevHandle handle, uint16_t addr,
    uint8_t *buf, int16_t len, uint16_t flags)
{
    int32_t ret;
    struct I2cMsg msg;

    msg.addr = addr;
    msg.buf = buf;
    msg.len = (uint16_t)len;
    msg.flags = flags | I2C_FLAG_READ;

    ret = I2cTransfer(handle, &msg, 1);
    return (ret == 1) ? HDF_SUCCESS : ret;
}

ssize_t I2cFsRead(struct file *filep, char *buf, size_t count)
{
    int32_t ret;
    uint8_t *kbuf = NULL;
    struct I2cClient *client = filep->f_priv;

    if (client == NULL) {
        HDF_LOGE("%s: client is null!", __func__);
        return 0;
    }

    kbuf = (uint8_t *)OsalMemCalloc(count);
    if (kbuf == NULL) {
        HDF_LOGE("%s: malloc kbuf fail!", __func__);
        return 0;
    }

    ret = I2cCntlrRead(client->handle, client->addr, kbuf, count, client->flags);
    PLAT_LOGV("%s: I2cRead called, ret:%d", __func__, ret);

    if (ret == HDF_SUCCESS) {
        if (LOS_CopyFromKernel(buf, count, kbuf, count) != LOS_OK) {
            HDF_LOGE("%s: copy from kernel fail:%d", __func__, ret);
            ret = HDF_ERR_IO;
        }
    }
    OsalMemFree(kbuf);
    return (ret == HDF_SUCCESS) ? count : 0;
}

static int32_t I2cCntlrWrite(DevHandle handle, uint16_t addr,
    const uint8_t *buf, int16_t len, uint16_t flags)
{
    int32_t ret;
    struct I2cMsg msg;

    msg.addr = addr;
    msg.buf = (uint8_t *)buf;
    msg.len = (uint16_t)len;
    msg.flags = flags & (~I2C_FLAG_READ);

    ret = I2cTransfer(handle, &msg, 1);
    return (ret == 1) ? HDF_SUCCESS : ret;
}


ssize_t I2cFsWrite(struct file *filep, const char *buf, size_t count)
{
    int32_t ret;
    uint8_t *kbuf = NULL;
    struct I2cClient *client = filep->f_priv;

    if (client == NULL) {
        HDF_LOGE("%s: client is null!", __func__);
        return 0;
    }

    kbuf = (uint8_t *)OsalMemCalloc(count);
    if (kbuf == NULL) {
        HDF_LOGE("%s: malloc kbuf fail!", __func__);
        return 0;
    }
    if (LOS_CopyToKernel(kbuf, count, buf, count) != LOS_OK) {
        HDF_LOGE("%s: copy to kernel fail!", __func__);
        OsalMemFree(kbuf);
        return 0;
    }

    ret = I2cCntlrWrite(client->handle, client->addr, kbuf, count, client->flags);
    PLAT_LOGV("%s: I2cWrite called, ret:%d", __func__, ret);

    OsalMemFree(kbuf);
    return (ret == HDF_SUCCESS) ? count : 0;
}

static int I2cIoctlReadWrite(const struct I2cClient *client, const void *arg)
{
    int ret;
    I2cIoctlWrap wrap;
    struct i2c_msg *uMsgs = NULL;
    struct I2cMsg *kMsgs = NULL;

    if (arg == NULL) {
        HDF_LOGE("%s: arg is null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = LOS_CopyToKernel(&wrap, sizeof(wrap), (void *)arg, sizeof(wrap));
    if (ret != LOS_OK) {
        HDF_LOGE("%s: copy wrap fail!", __func__);
        return HDF_ERR_IO;
    }

    if (wrap.msgs == NULL || wrap.nmsgs == 0 || wrap.nmsgs > I2C_RDWR_IOCTL_MAX_MSGS) {
        HDF_LOGE("%s: wrap msgs is null or invalid num:%u!", __func__, wrap.nmsgs);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = I2cMsgsCreateFromUser(&wrap, &kMsgs, &uMsgs);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: recreate msgs fail!", __func__);
        return ret;
    }

    ret = I2cTransfer(client->handle, kMsgs, wrap.nmsgs);
    PLAT_LOGV("%s: I2cTransfer called, ret:%d", __func__, ret);
    if ((unsigned int)ret == wrap.nmsgs) {
        ret = I2cMsgsCopyBackToUser(&wrap, kMsgs, uMsgs);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: copy back fail! ret:%d", __func__, ret);
        }
    } else {
        HDF_LOGE("%s: transfer fail, ret:%d, nmsgs:%u", __func__, ret, wrap.nmsgs);
    }

    I2cMsgsDestroy(kMsgs, uMsgs);
    return ret;
}

static int I2cFsIoctl(struct file *filep, int cmd, unsigned long arg)
{
    int retval = ENOERR;
    struct I2cClient *client = filep->f_priv;

    switch (cmd) {
        case IOCTL_CLIENT_FORCE:
        case IOCTL_CLIENT:
            if ((((client->flags & I2C_M_TEN) == 0) && arg > 0xfe) || (arg > 0x3ff)) {
                HDF_LOGE("%s:Not support arg(%0lu)!!!", __func__, arg);
                retval = -EINVAL;
                break;
            }
            client->addr = arg;
            break;
        case IOCTL_RDWR: {
            retval = I2cIoctlReadWrite(client, (void *)(uintptr_t)arg);
            break;
        }
        case IOCTL_16BIT_REG:
            if (arg == 0) {
                client->flags &= ~I2C_M_16BIT_REG;
            } else {
                client->flags |= I2C_M_16BIT_REG;
            }
            break;
        case IOCTL_16BIT_DATA:
            if (arg == 0) {
                client->flags &= ~I2C_M_16BIT_DATA;
            } else {
                client->flags |= I2C_M_16BIT_DATA;
            }
            break;
        case IOCTL_TENBIT:
            if (arg == 0) {
                client->flags &= ~I2C_M_TEN;
            } else {
                client->flags |= I2C_M_TEN;
            }
            break;
        case IOCTL_PEC:
        case IOCTL_RETRIES:
        case IOCTL_TIMEOUT:
        default:
            HDF_LOGE("Not support cmd(%0d)!!!", cmd);
            retval = -EINVAL;
    }
    return retval;
}

static int I2cFsOpen(struct file *filep)
{
    DevHandle handle = NULL;
    struct I2cClient *client = NULL;
    struct drv_data *drvData = NULL;
    int16_t id;

    if (filep == NULL || filep->f_vnode == NULL || filep->f_vnode->data == NULL) {
        HDF_LOGE("%s: function parameter is null", __func__);
        return -EINVAL;
    }
    drvData = (struct drv_data *)filep->f_vnode->data;
    id = (int16_t)(uintptr_t)drvData->priv;

    handle = I2cOpen(id);
    if (handle == NULL) {
        HDF_LOGE("%s:Fail to get host:%d handle!", __func__, id);
        return -1;
    }

    client = (struct I2cClient *)OsalMemCalloc(sizeof(*client));
    if (client == NULL) {
        HDF_LOGE("%s:Fail to malloc client-%d!", __func__, id);
        return -1;
    }
    client->handle = handle;

    /* record the client as file private data */
    filep->f_priv = client;
    return 0;
}

static int I2cFsClose(struct file *filep)
{
    struct I2cClient *client = filep->f_priv;

    if (client == NULL) {
        HDF_LOGE("%s: has't opened!", __func__);
        return 0;
    }

    if (client->handle == NULL) {
        return 0;
    }
    I2cClose(client->handle);
    client->handle = NULL;
    OsalMemFree(client);
    client = NULL;

    filep->f_priv = NULL;
    return 0;
}

static ssize_t I2cFsMap(struct file* filep, LosVmMapRegion *region)
{
    size_t size = region->range.size;

    (void)filep;
    PADDR_T paddr = region->pgOff << PAGE_SHIFT;
    VADDR_T vaddr = region->range.base;
    LosVmSpace *space = LOS_SpaceGet(vaddr);

    if ((space == NULL) || ((paddr >= SYS_MEM_BASE) && (paddr < SYS_MEM_END))) {
        return -EINVAL;
    }

    if (LOS_ArchMmuMap(&space->archMmu, vaddr, paddr, size >> PAGE_SHIFT, region->regionFlags) <= 0) {
        return -EAGAIN;
    }

    return 0;
}

static const struct file_operations_vfs g_i2cFops = {
    .open = I2cFsOpen,
    .close = I2cFsClose,
    .read = I2cFsRead,
    .write = I2cFsWrite,
    .ioctl = I2cFsIoctl,
    .mmap = I2cFsMap,
};

int32_t I2cAddVfsById(int16_t id)
{
#ifdef LOSCFG_FS_VFS
    int32_t ret;
    char *name = NULL;

    if (id < 0 || id >= I2C_CNTLR_MAX) {
        HDF_LOGE("%s: id:%d exceed max:%d", __func__, id, I2C_CNTLR_MAX);
        return HDF_ERR_INVALID_PARAM;
    }
    name = (char *)OsalMemCalloc(I2C_NAME_SIZE);
    if (name == NULL) {
        HDF_LOGE("%s: malloc name fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    /* create /dev/i2c-x device files for the i2c adatpers */
    ret = snprintf_s(name, I2C_NAME_SIZE, I2C_NAME_SIZE - 1, "/dev/i2c-%d", id);
    if (ret < 0) {
        HDF_LOGE("%s: format name fail! ret:%d", __func__, ret);
        OsalMemFree(name);
        return ret;
    }
    ret = register_driver(name, &g_i2cFops, I2C_FS_MODE, (void *)((uintptr_t)id));
    if (ret != 0) {
        HDF_LOGE("%s: register %s fail! ret:%d", __func__, name, ret);
    }
    OsalMemFree(name);
    return ret;
#else /* LOSCFG_FS_VFS */
    (void)id;
    return HDF_SUCCESS;
#endif
}

void I2cRemoveVfsById(int16_t id)
{
#ifdef LOSCFG_FS_VFS
    int32_t ret;
    char *name = NULL;

    if (id < 0 || id >= I2C_CNTLR_MAX) {
        HDF_LOGE("%s: id:%d exceed max:%d", __func__, id, I2C_CNTLR_MAX);
        return;
    }
    name = (char *)OsalMemCalloc(I2C_NAME_SIZE);
    if (name == NULL) {
        HDF_LOGE("%s: malloc name fail!", __func__);
        return;
    }
    /* remove /dev/i2c-x device files for the i2c controllers */
    ret = snprintf_s(name, I2C_NAME_SIZE, I2C_NAME_SIZE - 1, "/dev/i2c-%d", id);
    if (ret < 0) {
        HDF_LOGE("%s: format name fail! ret:%d", __func__, ret);
        OsalMemFree(name);
        return;
    }
    ret = unregister_driver(name);
    if (ret != 0) {
        HDF_LOGE("%s: unregister %s fail!", __func__, name);
    }
    OsalMemFree(name);
#else
    (void)id;
#endif /* LOSCFG_FS_VFS */
}
