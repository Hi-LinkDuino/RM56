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

#include "mtd_legacy_lite.h"
#include "hdf_log.h"
#include "mtd_core.h"
#include "mtd_list.h"
#include "osal_mem.h"
#include "string.h"

struct MtdDevNode {
    char *type;
    int status;
    struct MtdDev *mtdDev;
    struct MtdDevNode *next;
};

static struct MtdDevNode g_mtdHead;

void *GetMtd(const char *type)
{
    struct MtdDevNode *p = NULL;

    if (type == NULL) {
        return NULL;
    }

    for (p = g_mtdHead.next; p != NULL; p = p->next) {
        if (strcmp(type, p->type) == 0) {
            p->status++;
            return p->mtdDev;
        }
    }
    return NULL;
}

int GetMtdInfo(const char *type)
{
    struct MtdDevNode *p = NULL;

    if (type == NULL) {
        return -1;
    }

    for (p = g_mtdHead.next; p != NULL; p = p->next) {
        if (strcmp(type, p->type) == 0) {
            return 0;
        }
    }
    return -1;
}

int FreeMtd(struct MtdDev *mtdDev)
{
    struct MtdDevNode *p = NULL;

    if (mtdDev == NULL) {
        return -1;
    }

    for (p = g_mtdHead.next; p != NULL; p = p->next) {
        if (p->mtdDev == mtdDev) {
            p->status--;
            return 0;
        }
    }

    return -1;
}

int DelMtdList(struct MtdDev *mtdDev)
{
    struct MtdDevNode *p = NULL;
    struct MtdDevNode *q = NULL;

    if (mtdDev == NULL) {
        return -1;
    }

    for (p = g_mtdHead.next, q = &g_mtdHead; p != NULL; q = p, p = p->next) {
        if (p->mtdDev == mtdDev) {
            if (p->status == 0) {
                q->next = p->next;
                free(p);
                return 0;
            }
            return -1;
        }
    }
    return -1;
}

void AddMtdList(char *type, struct MtdDev *mtdDev)
{
    struct MtdDevNode *p = NULL;
    struct MtdDevNode *q = NULL;

    if (type == NULL || mtdDev == NULL) {
        return;
    }

    p = g_mtdHead.next;
    q = (struct MtdDevNode *)OsalMemCalloc(sizeof(*q));
    if (q == NULL) {
        return;
    }

    q->type = type;
    q->mtdDev = mtdDev;
    q->status = 0;
    g_mtdHead.next = q;
    q->next = p;
}

MtdInfoLegacy g_mtdInfo;
MtdInfoLegacy *nand_mtd;
struct MtdDevice *g_nandMtd;
/************** compatibale of old hisi interfaces ***************************/
// from device/hisilicon/drivers/xxxx/mtd/nand/src/common/nand.c
int nand_init(void)
{
    HDF_LOGI("%s: this is a stub cause new mtd drivers has been used", __func__);
    return 0;
}

// form device/hisilicon/drivers/xxxx/mtd/nand/src/common/nand_utils.c
int hinand_read(void *memaddr, unsigned long start, unsigned long size)
{
    int32_t ret;

    if (g_nandMtd == NULL) {
        HDF_LOGE("%s: g_nandMtd is null", __func__);
        return -ENODEV;
    }
    ret = MtdDeviceRead(g_nandMtd, (off_t)start, (size_t)size, (uint8_t *)memaddr);
    if ((unsigned long)ret != size) {
        HDF_LOGE("%s: ret=%d, size=%lu", __func__, ret, size);
    }
    return ((unsigned long)ret == size) ? HDF_SUCCESS : ret;
}

int hinand_write(void *memaddr, unsigned long start, unsigned long size)
{
    int32_t ret;

    if (g_nandMtd == NULL) {
        HDF_LOGE("%s: g_nandMtd is null", __func__);
        return -ENODEV;
    }
    ret = MtdDeviceWrite(g_nandMtd, (off_t)start, (size_t)size, (const uint8_t *)memaddr);
    if ((unsigned long)ret != size) {
        HDF_LOGE("%s: ret=%d, size=%lu", __func__, ret, size);
    }
    return ((unsigned long)ret == size) ? HDF_SUCCESS : ret;
}

int hinand_erase(unsigned long start, unsigned long size)
{
    int ret;
    off_t failAddr;

    if (g_nandMtd == NULL) {
        HDF_LOGE("%s: g_nandMtd is null", __func__);
        return -ENODEV;
    }
    ret = MtdDeviceErase(g_nandMtd, (off_t)start, (size_t)size, &failAddr);
    if (ret != 0) {
        HDF_LOGE("%s: MtdDeviceErase failed, ret=%d", __func__, ret);
    }
    return ret;
}

int hinand_yaffs_read(void *memaddr, unsigned long start, unsigned long size)
{
    int32_t ret;

    if (g_nandMtd == NULL) {
        HDF_LOGE("%s: g_nandMtd is null", __func__);
        return -ENODEV;
    }
    ret = MtdDeviceReadWithOob(g_nandMtd, (off_t)start, (size_t)size, (uint8_t *)memaddr);
    if ((unsigned long)ret != size) {
        HDF_LOGE("%s: ret=%d, size=%lu", __func__, ret, size);
    }
    return ((unsigned long)ret == size) ? HDF_SUCCESS : ret;
}

int hinand_yaffs_write(void *memaddr, unsigned long start, unsigned long size)
{
    int32_t ret;

    if (g_nandMtd == NULL) {
        HDF_LOGE("%s: g_nandMtd is null", __func__);
        return -ENODEV;
    }
    ret = MtdDeviceWriteWithOob(g_nandMtd, (off_t)start, (size_t)size, (uint8_t *)memaddr);
    if ((unsigned long)ret != size) {
        HDF_LOGE("%s: ret=%d, size=%lu", __func__, ret, size);
    }
    return ((unsigned long)ret == size) ? HDF_SUCCESS : ret;
}

int hinand_yaffs_nand_block_isbad(loff_t ofs)
{
    return MtdDeviceIsBadBlock(g_nandMtd, ofs);
}

int hinand_yaffs_nand_block_markbad(loff_t ofs)
{
    return MtdDeviceMarkBadBlock(g_nandMtd, ofs);
}

int spinor_init(void)
{
    HDF_LOGI("%s: this function has been hacked ...", __func__);
    return HDF_SUCCESS;
}

static int MtdDeviceLegacyNandErase(MtdInfoLegacy *mtd, EraseInfoLegacy *ops)
{
    int ret;
    off_t failAddr;

    if (mtd == NULL || ops == NULL) {
        return -EINVAL;
    }
    ops->fail_addr = MTD_FAIL_ADDR_UNKNOWN;

    ret = MtdDeviceErase((struct MtdDevice *)mtd->priv, (off_t)ops->addr, (size_t)ops->len, &failAddr);
    if (ret != HDF_SUCCESS) {
        ops->fail_addr = failAddr;
    }
    return ret;
}

static int MtdDeviceLegacyNandRead(MtdInfoLegacy *mtd, off_t addr, size_t len, size_t *retlen, char *buf)
{
    int ret;

    if (mtd == NULL || mtd->priv == NULL || retlen == NULL || buf == NULL) {
        HDF_LOGE("%s: function parameter is null", __func__);
        return -EINVAL;
    }

    ret = MtdDeviceRead((struct MtdDevice *)mtd->priv, addr, len, (uint8_t *)buf);
    *retlen = (ret == HDF_SUCCESS) ? len : 0;
    return ret;
}

static int MtdDeviceLegacyNandWrite(MtdInfoLegacy *mtd, off_t addr, size_t len, size_t *retlen, const char *buf)
{
    int ret;

    if (mtd == NULL || retlen == NULL) {
        HDF_LOGE("%s: function parameter is null", __func__);
        return -EINVAL;
    }

    ret = MtdDeviceWrite((struct MtdDevice *)mtd->priv, addr, len, (const uint8_t *)buf);
    *retlen = (ret == HDF_SUCCESS) ? len : 0;
    return ret;
}

static int MtdDeviceLegacyNandReadOob(MtdInfoLegacy *mtd, off_t addr, size_t len, size_t *retlen, char *buf)
{
    int ret;

    if (mtd == NULL || retlen == NULL) {
        HDF_LOGE("%s: function parameter is null", __func__);
        return -EINVAL;
    }

    ret = MtdDeviceReadWithOob((struct MtdDevice *)mtd->priv, addr, len, (uint8_t *)buf);
    *retlen = (ret == HDF_SUCCESS) ? len : 0;
    return ret;
}

static int MtdDeviceLegacyNandWriteOob(MtdInfoLegacy *mtd, off_t addr, size_t len, size_t *retlen, const char *buf)
{
    int ret;

    if (mtd == NULL || retlen == NULL) {
        HDF_LOGE("%s: function parameter is null", __func__);
        return -EINVAL;
    }

    ret = MtdDeviceWriteWithOob((struct MtdDevice *)mtd->priv, addr, len, (const uint8_t *)buf);
    *retlen = (ret == HDF_SUCCESS) ? len : 0;
    return ret;
}

static int MtdDeviceLegacyBlockIsBad(MtdInfoLegacy *mtd, off_t ofs)
{
    if (mtd == NULL) {
        return -EINVAL;
    }

    return (int)MtdDeviceIsBadBlock((struct MtdDevice *)mtd->priv, ofs);
}

static int MtdDeviceLegacyBlockMarkBad(MtdInfoLegacy *mtd, off_t ofs)
{
    if (mtd == NULL) {
        return -EINVAL;
    }

    return (int)MtdDeviceMarkBadBlock((struct MtdDevice *)mtd->priv, ofs);
}

void MtdDeviceLegacyFillMtdInfo(struct MtdDevice *mtdDevice)
{
    if (mtdDevice == NULL) {
        return;
    }
    nand_mtd = &g_mtdInfo;

    nand_mtd->size = mtdDevice->capacity;
    nand_mtd->erasesize = mtdDevice->eraseSize;
    nand_mtd->writesize = mtdDevice->writeSize;
    nand_mtd->oobsize = mtdDevice->oobSize;
    nand_mtd->name = "nand";
    nand_mtd->type = MTD_NANDFLASH;
    nand_mtd->flags = MTD_CAP_NANDFLASH;

    nand_mtd->erase = MtdDeviceLegacyNandErase;
    nand_mtd->read = MtdDeviceLegacyNandRead;
    nand_mtd->write = MtdDeviceLegacyNandWrite;
    nand_mtd->read_oob = MtdDeviceLegacyNandReadOob;
    nand_mtd->write_oob = MtdDeviceLegacyNandWriteOob;
    nand_mtd->block_isbad = MtdDeviceLegacyBlockIsBad;
    nand_mtd->block_markbad = MtdDeviceLegacyBlockMarkBad;
    nand_mtd->priv = mtdDevice;

    g_nandMtd = mtdDevice;
}
