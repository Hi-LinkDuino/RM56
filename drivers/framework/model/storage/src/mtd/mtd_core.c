/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mtd_core.h"
#include "securec.h"

#include "hdf_log.h"
#include "mtd_block.h"
#include "mtd_char.h"
#include "platform_core.h"

static int32_t MtdDeviceCheckParms(struct MtdDevice *mtdDevice)
{
    if (mtdDevice->index < 0 || mtdDevice->index >= MTD_DEVICE_NUM_MAX) {
        HDF_LOGE("%s: invalid index: %d", __func__, mtdDevice->index);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->name == NULL) {
        HDF_LOGE("%s: name is NULL", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->type >= MTD_TYPE_MAX) {
        HDF_LOGE("%s: invalid mtd type:%d", __func__, mtdDevice->type);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->idLen > MTD_FLASH_ID_LEN_MAX) {
        HDF_LOGE("%s: invalid idLen:%u", __func__, mtdDevice->idLen);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->idLen > MTD_FLASH_ID_LEN_MAX) {
        HDF_LOGE("%s: invalid idLen:%u", __func__, mtdDevice->idLen);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->capacity == 0) {
        HDF_LOGE("%s: invalid capacity:%zu", __func__, mtdDevice->capacity);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->eraseSize == 0) {
        HDF_LOGE("%s: invalid erase size:%zu", __func__, mtdDevice->eraseSize);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->writeSize == 0) {
        HDF_LOGE("%s: invalid write size:%zu", __func__, mtdDevice->writeSize);
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->readSize == 0) {
        HDF_LOGE("%s: invalid read size:%zu", __func__, mtdDevice->readSize);
        return HDF_ERR_INVALID_OBJECT;
    }

    return HDF_SUCCESS;
}

static void MtdDeviceDumpDefault(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL) {
        MTD_DEVICE_DUMP(mtdDevice);
    }
    return;
}

static void MtdDeviceDump(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL && mtdDevice->ops != NULL && mtdDevice->ops->dump != NULL) {
        mtdDevice->ops->dump(mtdDevice);
    }
    return;
}

static int32_t MtdDeviceLockDefault(struct MtdDevice *mtdDevice)
{
    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    return OsalMutexLock(&mtdDevice->lock);
}

static void MtdDeviceUnlockDefault(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL) {
        OsalMutexUnlock(&mtdDevice->lock);
    }
    return;
}

struct PlatformManager *MtdManagerGet(void)
{
    int32_t ret;
    static struct PlatformManager *g_mtdManager = NULL;

    if (g_mtdManager == NULL) {
        ret = PlatformManagerCreate("STORAGE_MTD", &g_mtdManager);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("MtdManagerGet: create manager failed:%d", ret);
        }
    }
    return g_mtdManager;
}

int32_t MtdDeviceAdd(struct MtdDevice *mtdDevice)
{
    int32_t ret;

    if (mtdDevice == NULL || mtdDevice->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = MtdDeviceCheckParms(mtdDevice);
    if (ret != HDF_SUCCESS) {
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = OsalMutexInit(&mtdDevice->lock);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mutex init failed, ret=%d", __func__, ret);
        return ret;
    }

    if (mtdDevice->ops->dump == NULL) {
        mtdDevice->ops->dump = MtdDeviceDumpDefault;
    }

    if (mtdDevice->ops->lock == NULL || mtdDevice->ops->unlock == NULL) {
        mtdDevice->ops->lock = MtdDeviceLockDefault;
        mtdDevice->ops->unlock = MtdDeviceUnlockDefault;
    }

    mtdDevice->device.manager = MtdManagerGet();
    mtdDevice->device.number = mtdDevice->index;
    ret = PlatformDeviceAdd(&mtdDevice->device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mtd device add fail", __func__);
        return ret;
    }

    MtdDeviceDump(mtdDevice);

    ret = MtdCharInit(mtdDevice);
    if (ret != HDF_SUCCESS) {
        PlatformDeviceDel(&mtdDevice->device);
        return ret;
    }

    ret = MtdBlockInit(mtdDevice);
    if (ret != HDF_SUCCESS) {
        PlatformDeviceDel(&mtdDevice->device);
        return ret;
    }

    return HDF_SUCCESS;
}

void MtdDeviceDel(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL) {
        MtdCharUninit(mtdDevice);
        MtdBlockUninit(mtdDevice);
        PlatformDeviceDel(&mtdDevice->device);
        (void)OsalMutexDestroy(&mtdDevice->lock);
    }
}

int32_t MtdDeviceLock(struct MtdDevice *mtdDevice)
{
    int32_t ret;

    if (mtdDevice == NULL || mtdDevice->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->ops->lock == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = mtdDevice->ops->lock(mtdDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: lock mtd device failed, ret=%d", __func__, ret);
    }
    return ret;
}

void MtdDeviceUnlock(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL && mtdDevice->ops != NULL && mtdDevice->ops->unlock != NULL) {
        mtdDevice->ops->unlock(mtdDevice);
    }
    return;
}

static void MtdDumpBuf(uint8_t *buf, size_t len)
{
    int ret;
    size_t i;
    size_t idx;
    size_t lidx;
    size_t line;
#define MTD_DUMP_SIGLE_WIDTH  2
#define MTD_DUMP_LINE_LEN     32
#define MTD_DUMP_BUF_LEN    (MTD_DUMP_LINE_LEN * MTD_DUMP_SIGLE_WIDTH + 1)
    char lineBuf[MTD_DUMP_BUF_LEN];
    idx = 0;
    while (idx < len) {
        line = (MTD_DUMP_LINE_LEN <= (len - idx)) ? MTD_DUMP_LINE_LEN : (len - idx);
        for (i = 0, lidx = 0; i < line; i++, lidx += MTD_DUMP_SIGLE_WIDTH, buf++) {
            ret = snprintf_s(lineBuf + lidx, MTD_DUMP_SIGLE_WIDTH + 1, MTD_DUMP_SIGLE_WIDTH, "%02x", *buf);
            if (ret < 0) {
                HDF_LOGD("%s: format string failed, ret=%d", __func__, ret);
                return;
            }
        }
        HDF_LOGD("0x%08zx : %s", idx, lineBuf);
        idx += line;
    }
    return;
}

static int32_t MtdDeviceEraseUnlock(struct MtdDevice *mtdDevice, off_t addr, size_t len, off_t *faddr)
{
    int32_t ret;

    if (mtdDevice == NULL || mtdDevice->ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->ops->erase == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    ret = mtdDevice->ops->erase(mtdDevice, addr, len, faddr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: erase mtd device failed, addr=%jx, ret=%d", __func__, addr, ret);
    }
    return ret;
}

ssize_t MtdDeviceErase(struct MtdDevice *mtdDevice, off_t addr, size_t len, off_t *failAddr)
{
    int32_t ret;

    if ((ret = MtdDeviceLock(mtdDevice)) != HDF_SUCCESS) {
        return ret;
    }
    ret = MtdDeviceEraseUnlock(mtdDevice, addr, len, failAddr);
    MtdDeviceUnlock(mtdDevice);
    return ret;
}

static bool MtdDeviceIsBadBlockUnlocked(struct MtdDevice *mtdDevice, off_t addr)
{
    if (mtdDevice != NULL && mtdDevice->ops != NULL && mtdDevice->ops->isBadBlock != NULL) {
        return mtdDevice->ops->isBadBlock(mtdDevice, addr);
    }
    return false;
}

static int32_t MtdDeviceMarkBadBlockUnlocked(struct MtdDevice *mtdDevice, off_t addr)
{
    int32_t ret;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (mtdDevice->ops == NULL || mtdDevice->ops->markBadBlock == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = mtdDevice->ops->markBadBlock(mtdDevice, addr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mark bad block failed, addr=%jx, ret=%d", __func__, addr, ret);
    }
    return ret;
}

bool MtdDeviceIsBadBlock(struct MtdDevice *mtdDevice, off_t addr)
{
    bool ret = false;

    if (MtdDeviceLock(mtdDevice) != HDF_SUCCESS) {
        return false;
    }
    ret = MtdDeviceIsBadBlockUnlocked(mtdDevice, addr);
    MtdDeviceUnlock(mtdDevice);
    return ret;
}

int32_t MtdDeviceMarkBadBlock(struct MtdDevice *mtdDevice, off_t addr)
{
    int32_t ret;

    if ((ret = MtdDeviceLock(mtdDevice)) != HDF_SUCCESS) {
        return ret;
    }
    ret = MtdDeviceMarkBadBlockUnlocked(mtdDevice, addr);
    MtdDeviceUnlock(mtdDevice);
    return ret;
}

static int32_t MtdDevicePageTransferUnlocked(struct MtdDevice *mtdDevice, struct MtdPage *mtdPage)
{
    int32_t ret;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->ops == NULL || mtdDevice->ops->pageTransfer == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

#ifdef MTD_DEBUG
    HDF_LOGD("%s: mtdPage-> type=%d, addr=0x%jx, databuf=%p, datalen=%zu, oobbuf=%p, ooblen=%zu", __func__,
        mtdPage->type, mtdPage->addr, mtdPage->dataBuf, mtdPage->dataLen, mtdPage->oobBuf, mtdPage->oobLen);
#endif

    ret = mtdDevice->ops->pageTransfer(mtdDevice, mtdPage);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: do page transfer failed, ret = %d, addr = 0x%jx", __func__, ret, mtdPage->addr);
    }
    return ret;
}

static int32_t MtdDeviceCheckMsg(struct MtdDevice *mtdDevice, struct MtdMsg *msg)
{
    size_t oobSize;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (msg == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if (msg->buf == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    if ((msg->addr + msg->len) > mtdDevice->capacity) {
        HDF_LOGE("%s: over range, addr=%jx, len=%zu", __func__, msg->addr, msg->len);
        return HDF_ERR_INVALID_PARAM;
    }

    if (msg->type == MTD_MSG_TYPE_ERASE) {
        if ((msg->addr % mtdDevice->eraseSize) != 0) {
            HDF_LOGE("%s: not erase size aligned, addr=%jd, erase size=%zu", __func__,
                msg->addr, mtdDevice->eraseSize);
            return HDF_ERR_INVALID_PARAM;
        }
        return HDF_SUCCESS;
    }

    oobSize = (msg->withOob) ? mtdDevice->oobSize : 0;
    if ((msg->addr % mtdDevice->writeSize) != 0 || (msg->len % (mtdDevice->writeSize + oobSize)) != 0) {
        if (msg->type != MTD_MSG_TYPE_READ || msg->withOob) {
            HDF_LOGE("%s: not page aligned, addr=%jd, type=%d, withOob=%d", __func__,
                msg->addr, msg->type, msg->withOob);
            return HDF_ERR_INVALID_PARAM;
        }
        return HDF_SUCCESS;
    }

    return HDF_SUCCESS;
}

static int32_t MtdDeviceWriteReadByPageUnlock(struct MtdDevice *mtdDevice, struct MtdMsg *msg)
{
    int32_t ret;
    off_t addr;
    uint8_t *buf = NULL;
    size_t dataLenLeft;
    size_t blockSize;
    off_t eraseOffset;
    struct MtdPage mtdPage;

    dataLenLeft = msg->withOob ?
        (msg->len / (mtdDevice->writeSize + mtdDevice->oobSize)) * mtdDevice->writeSize : msg->len;
    for (addr = msg->addr, buf = msg->buf; (dataLenLeft > 0) && addr < mtdDevice->capacity;) {
        if (MtdDeviceIsBadBlockUnlocked(mtdDevice, addr)) {
            if (!msg->skipBad) {
                HDF_LOGE("%s: failed on bad block @0x%jx", __func__, addr);
                return HDF_ERR_IO;
            }
            HDF_LOGW("%s: skip bad block @0x%jx", __func__, addr);
            addr = (addr & ~(mtdDevice->eraseSize - 1)) + mtdDevice->eraseSize;
            continue;
        }
        eraseOffset = addr & (mtdDevice->eraseSize - 1);
        blockSize = (dataLenLeft < (mtdDevice->eraseSize - eraseOffset)) ?
                    dataLenLeft : (mtdDevice->eraseSize - eraseOffset);
        // no more than one block at once
        mtdPage.type = msg->type;
        while (blockSize > 0) {
            mtdPage.addr = addr;
            mtdPage.dataBuf = (uint8_t *)buf;
            mtdPage.dataLen = mtdDevice->writeSize - (addr & (mtdDevice->writeSize - 1));
            if (mtdPage.dataLen > blockSize) {
                mtdPage.dataLen = blockSize;
            }
            mtdPage.oobBuf = msg->withOob ? (buf + mtdPage.dataLen) : NULL;
            mtdPage.oobLen = msg->withOob ? mtdDevice->oobSize : 0;
            ret = MtdDevicePageTransferUnlocked(mtdDevice, &mtdPage);
            if (ret != HDF_SUCCESS) {
                MtdDumpBuf(mtdPage.dataBuf, mtdPage.dataLen + mtdPage.oobLen);
                return ret;
            }
            buf += mtdPage.dataLen + mtdPage.oobLen;
            addr += mtdPage.dataLen;
            blockSize -= mtdPage.dataLen;
            dataLenLeft -= mtdPage.dataLen;
        }
    }

    if (dataLenLeft > 0) {
        HDF_LOGE("%s: no enough space, dataLenLeft=%zu, addr=0x%jx", __func__, dataLenLeft, addr);
        return HDF_ERR_IO;
    }
    return HDF_SUCCESS;
}

static int32_t MtdDeviceRequest(struct MtdDevice *mtdDevice, struct MtdMsg *msg)
{
    int32_t ret;

    ret = MtdDeviceCheckMsg(mtdDevice, msg);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    if ((ret = MtdDeviceLock(mtdDevice)) != HDF_SUCCESS) {
        return ret;
    }

    switch (msg->type) {
        case MTD_MSG_TYPE_READ:
        case MTD_MSG_TYPE_WRITE:
            ret = MtdDeviceWriteReadByPageUnlock(mtdDevice, msg);
            break;
        case MTD_MSG_TYPE_ERASE:
            ret = MtdDeviceEraseUnlock(mtdDevice, msg->addr, msg->len, &msg->faddr);
            break;
        default:
            ret = HDF_ERR_NOT_SUPPORT;
            break;
    }

    MtdDeviceUnlock(mtdDevice);
    return ret;
}

ssize_t MtdDeviceWrite(struct MtdDevice *mtdDevice, off_t to, size_t len, const uint8_t *buf)
{
    int32_t ret;
    struct MtdMsg msg;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->ops == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (mtdDevice->ops->write != NULL) {
        if ((ret = MtdDeviceLock(mtdDevice)) != HDF_SUCCESS) {
            return ret;
        }
        ret = mtdDevice->ops->write(mtdDevice, to, len, buf);
        MtdDeviceUnlock(mtdDevice);
    } else {
        msg.type = MTD_MSG_TYPE_WRITE;
        msg.addr = to;
        msg.buf = (uint8_t *)buf;
        msg.len = len;
        msg.withOob = false;
        msg.skipBad = true;
        ret = MtdDeviceRequest(mtdDevice, &msg);
    }

    return (ret == HDF_SUCCESS) ? len : ret;
}

ssize_t MtdDeviceRead(struct MtdDevice *mtdDevice, off_t from, size_t len, uint8_t *buf)
{
    int32_t ret;
    struct MtdMsg msg;

    if (mtdDevice == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (mtdDevice->ops == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (mtdDevice->ops->read != NULL) {
        if ((ret = MtdDeviceLock(mtdDevice)) != HDF_SUCCESS) {
            return ret;
        }
        ret = mtdDevice->ops->read(mtdDevice, from, len, buf);
        MtdDeviceUnlock(mtdDevice);
    } else {
        msg.type = MTD_MSG_TYPE_READ;
        msg.addr = from;
        msg.buf = (uint8_t *)buf;
        msg.len = len;
        msg.withOob = false;
        msg.skipBad = true;
        ret = MtdDeviceRequest(mtdDevice, &msg);
    }

    return (ret == HDF_SUCCESS) ? len : ret;
}

ssize_t MtdDeviceWriteWithOob(struct MtdDevice *mtdDevice, off_t to, size_t len, const uint8_t *buf)
{
    int32_t ret;
    struct MtdMsg msg;

    msg.type = MTD_MSG_TYPE_WRITE;
    msg.addr = to;
    msg.buf = (uint8_t *)buf;
    msg.len = len;
    msg.withOob = true;
    msg.skipBad = true;
    ret = MtdDeviceRequest(mtdDevice, &msg);
    return (ret == HDF_SUCCESS) ? len : ret;
}

ssize_t MtdDeviceReadWithOob(struct MtdDevice *mtdDevice, off_t from, size_t len, uint8_t *buf)
{
    int32_t ret;
    struct MtdMsg msg;

    msg.type = MTD_MSG_TYPE_READ;
    msg.addr = from;
    msg.buf = (uint8_t *)buf;
    msg.len = len;
    msg.withOob = true;
    msg.skipBad = true;
    ret = MtdDeviceRequest(mtdDevice, &msg);
    return (ret == HDF_SUCCESS) ? len : ret;
}


#define MTD_FFS_SHIFT_16BIT     16
#define MTD_FFS_SHIFT_8BIT      8
#define MTD_FFS_SHIFT_4BIT      4
#define MTD_FFS_SHIFT_2BIT      2

int MtdFfs(int x)
{
    int r = 1;
    unsigned int f = (unsigned int)x;

    if (f == 0) {
        return 0;
    }

    if ((f & 0xffff) == 0) {
        f >>= MTD_FFS_SHIFT_16BIT;
        r += MTD_FFS_SHIFT_16BIT;
    }

    if ((f & 0xff) == 0) {
        f >>= MTD_FFS_SHIFT_8BIT;
        r += MTD_FFS_SHIFT_8BIT;
    }

    if ((f & 0xf) == 0) {
        f >>= MTD_FFS_SHIFT_4BIT;
        r += MTD_FFS_SHIFT_4BIT;
    }

    if ((f & 0x3) == 0) {
        f >>= MTD_FFS_SHIFT_2BIT;
        r += MTD_FFS_SHIFT_2BIT;
    }

    if ((f & 0x1) == 0) {
        r += 1;
    }

    return r;
}
