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

#include "fs/fs.h"
#include "hdf_log.h"
#include "mtd_block.h"
#include "mtd_core.h"
#include "sys/ioctl.h"

#define HDF_LOG_TAG mtd_block_lite_c

static int LiteosMtdBlockOpen(FAR struct Vnode *vnode)
{
    (void)vnode;
    return 0;
}

static int LiteosMtdBlockClose(FAR struct Vnode *vnode)
{
    (void)vnode;
    return 0;
}

static ssize_t LiteosMtdBlockRead(FAR struct Vnode *vnode, FAR unsigned char *buf,
    unsigned long long secStart, unsigned int nSecs)
{
    (void)vnode;
    (void)buf;
    (void)secStart;
    (void)nSecs;
    return 0;
}

static ssize_t LiteosMtdBlockWrite(FAR struct Vnode *vnode, FAR const unsigned char *buf,
    unsigned long long secStart, unsigned int nSecs)
{
    (void)vnode;
    (void)buf;
    (void)secStart;
    (void)nSecs;
    return 0;
}

static int LiteosMtdBlockGeometry(FAR struct Vnode *vnode, FAR struct geometry *geometry)
{
    (void)vnode;
    (void)geometry;
    return 0;
}

static int32_t LiteosMtdBlockIoctl(FAR struct Vnode *vnode, int cmd, unsigned long arg)
{
    (void)vnode;
    (void)cmd;
    (void)arg;
    return 0;
}

static struct block_operations g_liteosNandOps = {
    .open = LiteosMtdBlockOpen,
    .close = LiteosMtdBlockClose,
    .read = LiteosMtdBlockRead,
    .write = LiteosMtdBlockWrite,
    .geometry = LiteosMtdBlockGeometry,
    .ioctl = LiteosMtdBlockIoctl,
    .unlink = NULL,
};

static struct block_operations g_liteosSpinorOps = {
    .open = LiteosMtdBlockOpen,
    .close = LiteosMtdBlockClose,
    .read = LiteosMtdBlockRead,
    .write = LiteosMtdBlockWrite,
    .geometry = LiteosMtdBlockGeometry,
    .ioctl = LiteosMtdBlockIoctl,
    .unlink = NULL,
};

struct block_operations *GetDevNandOps(void)
{
    return &g_liteosNandOps;
}

struct block_operations *GetDevSpinorOps(void)
{
    return &g_liteosSpinorOps;
}

#define MTD_LITE_BLOCK_DRV_MODE 0755
int32_t MtdBlockOsInit(struct MtdDevice *mtdDevice)
{
    int ret;
    const char *devPath = NULL;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->type == MTD_TYPE_SPI_NOR) {
        devPath = "/dev/spinor";
    } else if (mtdDevice->type == MTD_TYPE_SPI_NAND) {
        devPath = "/dev/nand";
    } else {
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = register_blockdriver(devPath, GetDevNandOps(), MTD_LITE_BLOCK_DRV_MODE, mtdDevice);
    if (ret != 0) {
        HDF_LOGE("%s: register block dev(%s) failed: %d", __func__, devPath, ret);
        return HDF_PLT_ERR_OS_API;
    }
    HDF_LOGI("%s: register block dev(%s) success", __func__, devPath);
    return HDF_SUCCESS;
}

void MtdBlockOsUninit(struct MtdDevice *mtdDevice)
{
    (void)mtdDevice;
}

