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

#include "disk.h"
#include "fs/fs.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "mmc_block.h"
#include "mmc_sd.h"
#include "sys/ioctl.h"
#include "user_copy.h"

#define HDF_LOG_TAG mmc_block_lite_c

/* block ioctl */
#define MMC_IOC_MAGIC   'm'
#define RT_DEVICE_CTRL_BLK_GETGEOME _IOR(MMC_IOC_MAGIC, 1, struct RtDeviceBlkGeometry)
#define RT_DEVICE_CARD_STATUS       _IOR(MMC_IOC_MAGIC, 2, int)
#define RT_DEVICE_CARD_AU_SIZE      _IOR(MMC_IOC_MAGIC, 3, unsigned int)
#define RT_DEVICE_BLOCK_ERROR_COUNT _IOR(MMC_IOC_MAGIC, 4, int)

#define RT_BLOCK_SIZE 512

/**
 * block device geometry structure
 */
struct RtDeviceBlkGeometry {
    unsigned long sectorCount;        /**< count of sectors */
    unsigned long bytesPerSector;     /**< number of bytes per sector */
    unsigned long blockSize;          /**< number of bytes to erase one block */
};

struct disk_divide_info g_emmcInfo = {.sector_count = 0xffffffff};

#ifdef LOSCFG_STORAGE_EMMC
struct disk_divide_info *StorageBlockGetEmmc(void)
{
    return &g_emmcInfo;
}
#endif

char *StorageBlockGetEmmcNodeName(void *block)
{
    struct MmcBlock *mb = (struct MmcBlock *)block;

    if (mb == NULL) {
        return NULL;
    }
    return mb->name;
}

static int LiteosBlockOpen(FAR struct Vnode *vnode)
{
    (void)vnode;
    return 0;
}

static int LiteosBlockClose(FAR struct Vnode *vnode)
{
    (void)vnode;
    return 0;
}

static ssize_t LiteosBlockRead(FAR struct Vnode *vnode, FAR unsigned char *buf,
    unsigned long long secStart, unsigned int nSecs)
{
    size_t max;
    struct MmcBlock *mb = NULL;

    if (vnode == NULL || vnode->data == NULL) {
        HDF_LOGE("%s: vnode is NULL or data of vnode is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    max = (size_t)(-1);
    mb = (struct MmcBlock *)((struct drv_data*)vnode->data)->priv;

    if (secStart >= max || nSecs >= max) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (mb == NULL) {
        HDF_LOGE("%s: mmc block is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return MmcDeviceRead(mb->mmc, buf, (size_t)secStart, (size_t)nSecs);
}

static ssize_t LiteosBlockWrite(FAR struct Vnode *vnode, FAR const unsigned char *buf,
    unsigned long long secStart, unsigned int nSecs)
{
    size_t max;
    struct MmcBlock *mb = NULL;

    if (vnode == NULL || vnode->data == NULL) {
        HDF_LOGE("%s: vnode is NULL or data of vnode is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    max = (size_t)(-1);
    mb = (struct MmcBlock *)((struct drv_data*)vnode->data)->priv;

    if (secStart >= max || nSecs >= max) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (mb == NULL) {
        HDF_LOGE("%s: mmc block is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    return MmcDeviceWrite(mb->mmc, (uint8_t *)buf, (size_t)secStart, (size_t)nSecs);
}

static int LiteosBlockGeometry(FAR struct Vnode *vnode, FAR struct geometry *geometry)
{
    struct MmcBlock *mb = NULL;

    if (vnode == NULL || vnode->data == NULL) {
        HDF_LOGE("%s: vnode is NULL or data of vnode is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    mb = (struct MmcBlock *)((struct drv_data*)vnode->data)->priv;
    if (mb == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (geometry == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    geometry->geo_available    = true;
    geometry->geo_mediachanged = false;
    geometry->geo_writeenabled = true;
    geometry->geo_nsectors = mb->capacity; /* MmcDevice sized by sectors */
    geometry->geo_sectorsize = mb->secSize;
    return HDF_SUCCESS;
}

static int32_t LiteosBlockSaveGeometry(FAR struct Vnode *vnode, unsigned long arg)
{
    int32_t ret;
    struct geometry gm;
    struct RtDeviceBlkGeometry rtGeo = {0};

    if (vnode == NULL || vnode->data == NULL) {
        HDF_LOGE("%s: vnode is NULL or data of vnode is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = LiteosBlockGeometry(vnode, &gm);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    rtGeo.sectorCount = gm.geo_nsectors;
    rtGeo.bytesPerSector = gm.geo_sectorsize;
    rtGeo.blockSize = RT_BLOCK_SIZE;
    ret = LOS_CopyFromKernel((void *)(uintptr_t)arg, sizeof(struct RtDeviceBlkGeometry),
              &rtGeo, sizeof(struct RtDeviceBlkGeometry));
    if (ret != 0) {
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t MmcBlockGetAuSize(struct MmcBlock *mb, uint32_t *auSize)
{
    struct MmcDevice *mmc = NULL;
    struct SdDevice *sd = NULL;

    if (mb == NULL || mb->mmc == NULL) {
        HDF_LOGE("%s: mmc block or device is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    mmc = (struct MmcDevice *)mb->mmc;
    if (mmc->type != MMC_DEV_SD) {
        HDF_LOGE("%s: media is not sd", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }
    sd = (struct SdDevice *)mmc;

    *auSize = sd->reg.ssr.auSize;
    return HDF_SUCCESS;
}

static int32_t LiteosBlockIoctl(FAR struct Vnode *vnode, int cmd, unsigned long arg)
{
    int32_t ret;
    int flag, errCnt;
    unsigned int au;
    uint32_t auSize;
    struct MmcBlock *mb = NULL;

    if (vnode == NULL || vnode->data == NULL) {
        HDF_LOGE("%s: vnode is NULL or data of vnode is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    mb = (struct MmcBlock *)((struct drv_data*)vnode->data)->priv;

    switch (cmd) {
        case RT_DEVICE_CTRL_BLK_GETGEOME:
            ret = LiteosBlockSaveGeometry(vnode, arg);
            break;
        case RT_DEVICE_CARD_STATUS:
            flag = (MmcDeviceIsPresent(mb->mmc)) ? 1 : 0;
            ret = LOS_CopyFromKernel((void *)(uintptr_t)arg, sizeof(int), &flag, sizeof(int));
            if (ret) {
                ret = HDF_ERR_IO;
            }
            break;
        case RT_DEVICE_CARD_AU_SIZE:
            ret = MmcBlockGetAuSize(mb, &auSize);
            if (ret == HDF_SUCCESS) {
                au = (unsigned int)auSize;
                ret = LOS_CopyFromKernel((void *)(uintptr_t)arg, sizeof(au), &au, sizeof(au));
                if (ret) {
                    ret = HDF_ERR_IO;
                }
            }
            break;
        case RT_DEVICE_BLOCK_ERROR_COUNT:
            errCnt = (int)mb->errCnt;
            ret = LOS_CopyFromKernel((void *)(uintptr_t)arg, sizeof(int), &errCnt,
                      sizeof(int));
            if (ret) {
                ret = HDF_ERR_IO;
            }
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    return ret;
}

static struct block_operations g_blockOps = {
    .open = LiteosBlockOpen,
    .close = LiteosBlockClose,
    .read = LiteosBlockRead,
    .write = LiteosBlockWrite,
    .geometry = LiteosBlockGeometry,
    .ioctl = LiteosBlockIoctl,
};

struct block_operations *StorageBlockGetMmcOps(void)
{
    return &g_blockOps;
}

static struct MmcBlock *g_diskIdTable[SYS_MAX_DISK] = { };

int32_t MmcBlockOsInit(struct MmcDevice *mmcDevice)
{
    int32_t ret;
    int32_t diskId;
    struct disk_divide_info *info = NULL;
    struct MmcBlock *mb = NULL;

    if (mmcDevice == NULL || mmcDevice->mb == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    mb = mmcDevice->mb;

    if (mb->name[0] == '\0') {
        HDF_LOGE("%s: mmc block name invalid", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    diskId = los_alloc_diskid_byname(mb->name);
    if (!mb->removable) {
        info = &g_emmcInfo;
        info->sector_count = mb->capacity;
    }

    ret = los_disk_init(mb->name, &g_blockOps, (void *)mb, diskId, info);
    if (ret != ENOERR) {
        HDF_LOGE("%s: los_disk_init fail:%d", __func__, ret);
        return ret;
    }

    mb->osData = get_disk(diskId);
    if (mb->osData == NULL) {
        HDF_LOGE("%s: los_disk_init fail:%d", __func__, ret);
        return HDF_PLT_ERR_OS_API;
    }

    if (diskId >= 0 && diskId < SYS_MAX_DISK) {
        g_diskIdTable[diskId] = mb;
    }

    return HDF_SUCCESS;
}

void MmcBlockOsUninit(struct MmcDevice *mmcDevice)
{
    int diskId;
    struct MmcBlock *mb = NULL;

    if (mmcDevice == NULL || mmcDevice->mb == NULL) {
        return;
    }
    mb = mmcDevice->mb;

    diskId = los_get_diskid_byname(mb->name);
    (void)los_disk_deinit(diskId);

    if (diskId >= 0 && diskId < SYS_MAX_DISK) {
        g_diskIdTable[diskId] = NULL;
    }
}

struct MmcBlock *MmcBlockFromDiskId(int32_t diskId)
{
    if (diskId >= 0 && diskId < SYS_MAX_DISK) {
        return g_diskIdTable[diskId];
    }
    return NULL;
}

ssize_t StorageBlockMmcErase(uint32_t diskId, size_t secStart, size_t secNr)
{
    struct MmcBlock *mb = NULL;

    mb = MmcBlockFromDiskId((int32_t)diskId);
    if (mb == NULL || mb->mmc == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    return MmcDeviceErase(mb->mmc, secStart, secNr);
}
