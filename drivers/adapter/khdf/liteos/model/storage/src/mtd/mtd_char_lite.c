/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
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

#include "errno.h"
#include "fs/fs.h"
#include "los_config.h"
#include "los_mux.h"
#include "stdio.h"
#include "string.h"
#include "user_copy.h"

#include "hdf_log.h"
#include "mtd_char.h"
#include "mtd_core.h"
#include "mtd_legacy_lite.h"
#include "mtd_partition.h"
#include "mtd_user.h"
#include "osal_mem.h"

struct MtdFileInfo {
    unsigned int partIndex;
    int mode;
};

/*
 * open device interface
 */
static int MtdCharOpen(FAR struct file *filep)
{
    struct drv_data *drv = NULL;

    if (filep == NULL || filep->f_vnode == NULL || filep->f_vnode->data == NULL) {
        HDF_LOGE("%s: filep is NULL or f_vnode of filep is NULL or data of f_vnode is NULL", __func__);
        return -EINVAL;
    }
    drv = (struct drv_data *)filep->f_vnode->data;
    mtd_partition *partition = (mtd_partition *)drv->priv;
    if (partition == NULL) {
        HDF_LOGE("%s: partition is NULL", __func__);
        return -EINVAL;
    }
    struct MtdFileInfo *mfi = NULL;

    if (partition->user_num != 0) { // be opened
        return -EBUSY;
    }

    mfi = (struct MtdFileInfo *)malloc(sizeof(*mfi));
    if (mfi == NULL) {
        PRINTK("%s: malloc mtd file info failed", __func__);
        return -ENOMEM;
    }

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    partition->user_num = 1;

    (void)LOS_MuxUnlock(&partition->lock);
    mfi->partIndex = partition->patitionnum;
    filep->f_pos = 0;
    filep->f_priv = (void *)mfi;

    return ENOERR;
}

/*
 * close device interface
 */
static int MtdCharClose(FAR struct file *filep)
{
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    mtd_partition *partition = (mtd_partition *)drv->priv;
    struct MtdFileInfo *mfi = (struct MtdFileInfo *)(filep->f_priv);

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    partition->user_num = 0;

    (void)LOS_MuxUnlock(&partition->lock);
    free(mfi);
    filep->f_priv = NULL;

    return ENOERR;
}

/*
 * read device interface
 */
static ssize_t MtdCharRead(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
    ssize_t ret = 0;
    off_t ppos = filep->f_pos;
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    mtd_partition *partition = (mtd_partition *)drv->priv;
    struct MtdFileInfo *mfi = (struct MtdFileInfo *)filep->f_priv;
    struct MtdDev *mtdDev = (struct MtdDev *)(partition->mtd_info);
    struct MtdDevice *mtdDevice = (struct MtdDevice *)mtdDev->priv;
    size_t blockSize = mtdDevice->eraseSize;
    size_t partStart = partition->start_block * blockSize;
    size_t partSize = (partition->end_block + 1 - partition->start_block) * blockSize;
    size_t partOobSize = (partSize >> mtdDevice->writeSizeShift) * mtdDevice->oobSize;

    if (buffer == NULL) {
        HDF_LOGE("%s: buffer is NULL", __func__);
        return -EINVAL;
    }

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);
    if (ppos < 0 || ppos > partSize) {
        PRINTK("%s: current file offset:0x%x invalid\n", __func__, ppos);
        ret = -EINVAL;
        goto out1;
    }

    if ((mfi->mode == MTD_FILE_MODE_OOB && (ppos + buflen) > (partSize + partOobSize)) ||
        (mfi->mode != MTD_FILE_MODE_OOB && (ppos + buflen) > partSize)) {
        PRINTK("%s: buffer to large, buflen:0x%x\n", __func__, buflen);
        ret = -EINVAL;
        goto out1;
    }

    if (!buflen) {
        ret = ENOERR;
        goto out1;
    }

    if (mfi->mode == MTD_FILE_MODE_OOB) {
        ret = MtdDeviceReadWithOob(mtdDevice, partStart + ppos, buflen, (uint8_t *)buffer);
    } else {
        ret = MtdDeviceRead(mtdDevice, partStart + ppos, buflen, (uint8_t *)buffer);
    }

    if (ret != HDF_SUCCESS) {
        PRINTK("%s: read err in mode %d\n", __func__, mfi->mode);
        goto out1;
    }

    filep->f_pos += (mfi->mode == MTD_FILE_MODE_OOB) ?
        (buflen - (buflen >> mtdDevice->writeSizeShift) * mtdDevice->oobSize) : buflen;

out1:
    (void)LOS_MuxUnlock(&partition->lock);
    return ret;
}

/*
 * write device interface
 */
static ssize_t MtdCharWrite(FAR struct file *filep, FAR const char *buffer, size_t buflen)
{
    ssize_t ret = 0;
    off_t ppos = filep->f_pos;
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    mtd_partition *partition = (mtd_partition *)drv->priv;
    struct MtdFileInfo *mfi = (struct MtdFileInfo *)filep->f_priv;
    struct MtdDev *mtdDev = (struct MtdDev *)(partition->mtd_info);
    struct MtdDevice *mtdDevice = (struct MtdDevice *)mtdDev->priv;
    size_t blockSize = mtdDevice->eraseSize;
    size_t partStart = partition->start_block * blockSize;
    size_t partSize = (partition->end_block + 1 - partition->start_block) * blockSize;
    size_t partOobSize = (partSize >> mtdDevice->writeSizeShift) * mtdDevice->oobSize;

    if (buffer == NULL) {
        HDF_LOGE("%s: buffer is NULL", __func__);
        return -EINVAL;
    }

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);
    if (ppos < 0 || ppos > partSize) {
        PRINTK("%s: current file offset:0x%x invalid\n", __func__, ppos);
        ret = -EINVAL;
        goto out1;
    }

    if ((mfi->mode == MTD_FILE_MODE_OOB && (ppos + buflen) > (partSize + partOobSize)) ||
        (mfi->mode != MTD_FILE_MODE_OOB && (ppos + buflen) > partSize)) {
        PRINTK("%s: buffer to large, buflen:0x%x\n", __func__, buflen);
        ret = -EINVAL;
        goto out1;
    }

    if (!buflen) {
        ret = 0;
        goto out1;
    }

    if (mfi->mode == MTD_FILE_MODE_OOB) {
        ret = MtdDeviceWriteWithOob(mtdDevice, partStart + ppos, buflen, (const uint8_t *)buffer);
    } else {
        ret = MtdDeviceWrite(mtdDevice, partStart + ppos, buflen, (const uint8_t *)buffer);
    }

    if (ret != HDF_SUCCESS) {
        PRINTK("%s: write err in mode %d\n", __func__, mfi->mode);
        goto out1;
    }

    filep->f_pos += (mfi->mode == MTD_FILE_MODE_OOB) ?
        (buflen - (buflen >> mtdDevice->writeSizeShift) * mtdDevice->oobSize) : buflen;

out1:
    (void)LOS_MuxUnlock(&partition->lock);
    return ret;
}

/*
 * lseek device interface
 */
static off_t MtdCharLseek(FAR struct file *filep, off_t offset, int whence)
{
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    mtd_partition *partition = (mtd_partition *)drv->priv;

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    struct MtdDev *mtdDev = (struct MtdDev *)(partition->mtd_info);
    size_t blockSize = mtdDev->eraseSize;
    size_t endAddr = (partition->end_block + 1) * blockSize;
    size_t startAddr = partition->start_block * blockSize;

    switch (whence) {
        case SEEK_SET:
            if (offset >= 0 && (size_t)offset < endAddr - startAddr) {
                filep->f_pos = (size_t)offset;
                goto out1;
            } else {
                goto err1;
            }

        case SEEK_CUR:
            if (offset + (size_t)filep->f_pos >= 0 &&
                    (size_t)(offset + filep->f_pos) < (endAddr - startAddr)) {
                filep->f_pos += offset;
                goto out1;
            } else {
                goto err1;
            }

        case SEEK_END:
            if (offset < 0 && (offset + endAddr) >= startAddr) {
                filep->f_pos = (off_t)(offset + endAddr - startAddr);
                goto out1;
            } else {
                goto err1;
            }

        default:
            goto err1;
    }
err1:
    (void)LOS_MuxUnlock(&partition->lock);
    return -EINVAL;
out1:
    (void)LOS_MuxUnlock(&partition->lock);
    return filep->f_pos;
}

static void MtdCharGetMtdInfo(const struct MtdDevice *mtdDevice, struct MtdInfo *mtdInfo)
{
    mtdInfo->type = (mtdDevice->type == MTD_TYPE_NOR || mtdDevice->type == MTD_TYPE_SPI_NOR) ?
                    MTD_NORFLASH : MTD_NANDFLASH;
    mtdInfo->size = mtdDevice->capacity;
    mtdInfo->erasesize = mtdDevice->eraseSize;
    mtdInfo->writesize = mtdDevice->writeSize;
    mtdInfo->oobsize = mtdDevice->oobSize;
}

static int MtdCharIoctlGetInfo(const mtd_partition *part, const struct MtdDevice *mtdDevice,
    int cmd, unsigned long arg)
{
    int ret;
    struct MtdInfo mtdInfo;
    size_t startAddr;
    size_t endAddr;

    (void)cmd;
    MtdCharGetMtdInfo(mtdDevice, &mtdInfo);
    startAddr = part->start_block * mtdDevice->eraseSize;
    endAddr = (part->end_block + 1) * mtdDevice->eraseSize;
    ret = LOS_CopyFromKernel((void *)(uintptr_t)arg, sizeof(mtdInfo), (void *)&mtdInfo, sizeof(mtdInfo));
    if (ret == 0) {
        ((struct MtdInfo *)(uintptr_t)arg)->size = endAddr - startAddr;
    }
    return ret;
}

static int MtdCharIoctlErase(const mtd_partition *part, struct MtdDevice *mtdDevice,
    int cmd, unsigned long arg)
{
    int ret;
    struct EraseInfo erase;
    size_t startAddr;

    (void)cmd;
    ret = LOS_CopyToKernel((void *)&erase, sizeof(erase), (void *)(uintptr_t)arg, sizeof(erase));
    if (ret != 0) {
        return -EINVAL;
    }

    startAddr = part->start_block * mtdDevice->eraseSize;
    return (int)MtdDeviceErase(mtdDevice, startAddr + erase.start, startAddr + erase.length, NULL);
}

static int MtdCharIoctlGetBadBlock(const mtd_partition *part, struct MtdDevice *mtdDevice,
    int cmd, unsigned long arg)
{
    int ret;
    loff_t offs;
    size_t startAddr;

    (void)cmd;
    ret = LOS_CopyToKernel((void *)&offs, sizeof(offs), (void *)(uintptr_t)arg, sizeof(offs));
    if (ret != 0) {
        return -EINVAL;
    }

    startAddr = part->start_block * mtdDevice->eraseSize;
    return (int)MtdDeviceIsBadBlock(mtdDevice, (loff_t)startAddr + offs);
}

static int MtdCharIoctlSetBadBlock(const mtd_partition *part, struct MtdDevice *mtdDevice,
    int cmd, unsigned long arg)
{
    int ret;
    loff_t offs;
    size_t startAddr;

    (void)cmd;
    ret = LOS_CopyToKernel((void *)&offs, sizeof(offs), (void *)(uintptr_t)arg, sizeof(offs));
    if (ret != 0) {
        return -EINVAL;
    }

    startAddr = part->start_block * mtdDevice->eraseSize;
    return (int)MtdDeviceMarkBadBlock(mtdDevice, (loff_t)startAddr + offs);
}

/*
 * ioctl device interface
 */
static int MtdCharIoctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    int ret = ENOERR;
    struct drv_data *drv = (struct drv_data *)filep->f_vnode->data;
    mtd_partition *partition = (mtd_partition *)drv->priv;
    struct MtdFileInfo *mfi = (struct MtdFileInfo *)filep->f_priv;
    struct MtdDev *mtdDev = (struct MtdDev *)(partition->mtd_info);
    struct MtdDevice *mtdDevice = (struct MtdDevice *)mtdDev->priv;

    if (mtdDevice == NULL || arg == 0) {
        HDF_LOGE("%s: mtdDevice is NULL or arg is 0", __func__);
        return -EINVAL;
    }

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    switch (cmd) {
        case MTD_IOC_GETINFO: {
            ret = MtdCharIoctlGetInfo(partition, mtdDevice, cmd, arg);
            break;
        }
        case MTD_IOC_ERASE:
        case MTD_IOC_ERASE64: {
            ret = MtdCharIoctlErase(partition, mtdDevice, cmd, arg);
            break;
        }
        case MTD_IOC_GETBADBLOCK: {
            ret = MtdCharIoctlGetBadBlock(partition, mtdDevice, cmd, arg);
            break;
        }
        case MTD_IOC_SETBADBLOCK: {
            ret = MtdCharIoctlSetBadBlock(partition, mtdDevice, cmd, arg);
            break;
        }
        case MTD_IOC_SETFILEMODE:
            mfi->mode = 0;
            if (arg >= MTD_FILE_MODE_MAX) {
                PRINTK("%s: invalid mtd file mode:%d\n", __func__, arg);
                ret = -EINVAL;
                break;
            }
            mfi->mode = arg;
            break;
        default:
            ret = -EINVAL;
    }

    (void)LOS_MuxUnlock(&partition->lock);

    return ret;
}

static ssize_t MtdCharMap(FAR struct file* filep, FAR LosVmMapRegion *region)
{
    (void)filep;
    (void)region;

    PRINTK("%s %d, mmap is not support\n", __FUNCTION__, __LINE__);
    return 0;
}

static const struct file_operations_vfs g_MtdCharFops = {
    .open   =   MtdCharOpen,
    .close  =   MtdCharClose,
    .read   =   MtdCharRead,
    .write  =   MtdCharWrite,
    .seek   =   MtdCharLseek,
    .ioctl  =   MtdCharIoctl,
    .mmap   =   MtdCharMap,
#ifndef CONFIG_DISABLE_POLL
    .poll   =   NULL,
#endif
    .unlink =   NULL,
};

static int ToLiteOsMtdType(enum MtdDevType hdfType)
{
    int ret = HDF_FAILURE;

    switch (hdfType) {
        case MTD_TYPE_NAND:
        case MTD_TYPE_SPI_NAND:
            ret = MTD_NANDFLASH;
            break;
        case MTD_TYPE_NOR:
        case MTD_TYPE_SPI_NOR:
            ret = MTD_NORFLASH;
            break;
        default:
            ret = MTD_TYPE_MAX;
            break;
    }
    return ret;
}

int HdfMtdDevErase(struct MtdDev *mtdDev, UINT64 start, UINT64 len, UINT64 *failAddr)
{
    int ret;
    off_t failPos;

    if (mtdDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (failAddr == NULL) {
        HDF_LOGE("%s: failAddr is NULL", __func__);
        return -EINVAL;
    }
    ret = MtdDeviceErase((struct MtdDevice *)mtdDev->priv, start, len, &failPos);
    if (ret != HDF_SUCCESS) {
        *failAddr = failPos;
    }
    return ret;
}

int HdfMtdDevRead(struct MtdDev *mtdDev, UINT64 start, UINT64 len, const char *buf)
{
    if (mtdDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return MtdDeviceRead((struct MtdDevice *)mtdDev->priv, start, len, (uint8_t *)buf);
}

int HdfMtdDevWrite(struct MtdDev *mtdDev, UINT64 start, UINT64 len, const char *buf)
{
    if (mtdDev == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return MtdDeviceWrite((struct MtdDevice *)mtdDev->priv, start, len, (const uint8_t *)buf);
}

static const char *MtdCharGetName(struct MtdDevice *mtdDevice)
{
    switch (mtdDevice->type) {
        case MTD_TYPE_SPI_NOR:
            return "spinor";
        case MTD_TYPE_SPI_NAND:
            return "nand";
        default:
            HDF_LOGE("%s: mtd type:%d not support", __func__, mtdDevice->type);
            return NULL;
    }
}

int32_t MtdCharOsInit(struct MtdDevice *mtdDevice)
{
    struct MtdDev *mtdDev = NULL;
    const char *devName = NULL;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    devName = MtdCharGetName(mtdDevice);
    if (devName == NULL) {
        return HDF_FAILURE;
    }

    if (GetMtdInfo(devName) == 0) {
        HDF_LOGE("%s: MtdDev(%s) already been added", __func__, devName);
        return HDF_ERR_NOT_SUPPORT;
    }

    mtdDev = (struct MtdDev *)OsalMemCalloc(sizeof(*mtdDev));
    if (mtdDev == NULL) {
        return HDF_ERR_IO;
    }
    mtdDev->priv = mtdDevice;
    mtdDev->type = ToLiteOsMtdType(mtdDevice->type);
    mtdDev->size = mtdDevice->capacity;
    mtdDev->eraseSize = mtdDevice->eraseSize;
    mtdDev->erase = HdfMtdDevErase;
    mtdDev->read = HdfMtdDevRead;
    mtdDev->write = HdfMtdDevWrite;
    AddMtdList((char *)devName, mtdDev);
    HDF_LOGI("%s: add MtdDev(%s) done", __func__, devName);
    mtdDevice->osData = mtdDev;

    MtdDeviceLegacyFillMtdInfo(mtdDevice);
    return HDF_SUCCESS;
}

void MtdCharOsUninit(struct MtdDevice *mtdDevice)
{
    struct MtdDev *mtdDev = NULL;
    const char *devName = NULL;

    if (mtdDevice == NULL) {
        return;
    }

    devName = MtdCharGetName(mtdDevice);
    if (devName == NULL) {
        return;
    }

    mtdDev = (struct MtdDev *)mtdDevice->osData;
    (void)DelMtdList(mtdDev);
    HDF_LOGI("%s: remove MtdDev(%s) done", __func__, devName);
}

const struct file_operations_vfs *GetMtdCharFops(VOID)
{
    return &g_MtdCharFops;
}
