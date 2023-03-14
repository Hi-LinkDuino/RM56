/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mmc_sdio.h"

#define HDF_LOG_TAG mmc_sdio_c
#define SDIO_READ_IO_READY_RETRY_TIMES 10000

static int32_t SdioDeviceDefaultIncrAddrReadBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    struct SdioRwBlockInfo info = {0};

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultIncrAddrReadBytes fail, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL) {
        HDF_LOGE("SdioDeviceDefaultIncrAddrReadBytes fail, data is null.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultIncrAddrReadBytes fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (size == 1) {
        return SdioReadWriteByte(cntlr, false, func->funcNum, addr, data);
    } else if (size > 1) {
        info.addr = addr;
        info.buf = data;
        info.incrAddrFlag = true;
        info.size = size;
        info.writeFlag = false;
        return SdioReadWriteBlock(cntlr, &info);
    }
    HDF_LOGE("SdioDeviceDefaultIncrAddrReadBytes fail, data size is 0.");
    return HDF_ERR_INVALID_PARAM;
}

static int32_t SdioDeviceDefaultIncrAddrWriteBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    struct SdioRwBlockInfo info = {0};

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultIncrAddrWriteBytes fail, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL) {
        HDF_LOGE("SdioDeviceDefaultIncrAddrWriteBytes fail, data is null.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultIncrAddrWriteBytes fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (size == 1) {
        return SdioReadWriteByte(cntlr, true, func->funcNum, addr, data);
    } else if (size > 1) {
        info.addr = addr;
        info.buf = data;
        info.incrAddrFlag = true;
        info.size = size;
        info.writeFlag = true;
        return SdioReadWriteBlock(cntlr, &info);
    }
    HDF_LOGE("SdioDeviceDefaultIncrAddrWriteBytes fail, data size is 0.");
    return HDF_ERR_INVALID_PARAM;
}

static int32_t SdioDeviceDefaultFixedAddrReadBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    struct SdioRwBlockInfo info = {0};

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultFixedAddrReadBytes fail, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL || size == 0) {
        HDF_LOGE("SdioDeviceDefaultFixedAddrReadBytes fail, param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultFixedAddrReadBytes fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    info.addr = addr;
    info.buf = data;
    info.incrAddrFlag = false;
    info.size = size;
    info.writeFlag = false;
    if (scatterLen > 0) {
        info.scatterFlag = true;
        info.scatterLen = scatterLen;
    }
    return SdioReadWriteBlock(cntlr, &info);
}

static int32_t SdioDeviceDefaultFixedAddrWriteBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    struct SdioRwBlockInfo info = {0};

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultFixedAddrWriteBytes fail, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL || size == 0) {
        HDF_LOGE("SdioDeviceDefaultFixedAddrWriteBytes fail, param is invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultFixedAddrWriteBytes fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    info.addr = addr;
    info.buf = data;
    info.incrAddrFlag = false;
    info.size = size;
    info.writeFlag = true;
    if (scatterLen > 0) {
        info.scatterFlag = true;
        info.scatterLen = scatterLen;
    }
    return SdioReadWriteBlock(cntlr, &info);
}

static int32_t SdioDeviceDefaultFunc0ReadBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    uint32_t i;
    int32_t ret;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultFunc0ReadBytes fail, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL) {
        HDF_LOGE("SdioDeviceDefaultFunc0ReadBytes fail, data is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultFunc0ReadBytes fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    for (i = 0; i < size; i++) {
        ret = SdioReadWriteByte(cntlr, false, 0, (addr + i), &data[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioDeviceDefaultFunc0ReadBytes fail, i = %u.", i);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SdioDeviceDefaultFunc0WriteBytes(struct SdioDevice *dev,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    int32_t ret;
    uint32_t i;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultFunc0WriteBytes fail, func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (data == NULL) {
        HDF_LOGE("SdioDeviceDefaultFunc0WriteBytes fail, data is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultFunc0WriteBytes fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }

    for (i = 0; i < size; i++) {
        ret = SdioReadWriteByte(cntlr, true, 0, (addr + i), &data[i]);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioDeviceDefaultFunc0WriteBytes fail, i = %u.", i);
            return ret;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SdioDeviceDefaultSetBlockSize(struct SdioDevice *dev, uint32_t blkSize)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    uint32_t blockSize = blkSize;
    int32_t ret;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultSetBlockSize: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (blockSize == 0) {
        blockSize = ((func->maxBlkSize < cntlr->maxBlkSize) ? func->maxBlkSize : cntlr->maxBlkSize);
    }
    blockSize = MMC_MIN(blockSize, MMC_SEC_SIZE);

    ret = SdioSetFbrIoBlockSize(cntlr, blockSize);
    if (ret == HDF_SUCCESS) {
        func->curBlkSize = blockSize;
    }
    return ret;
}

static int32_t SdioDeviceDefaultGetCommonInfo(struct SdioDevice *dev,
    SdioCommonInfo *info, SdioCommonInfoType infoType)
{
    struct MmcCntlr *cntlr = NULL;
    struct SdioFunction *func = dev->curFunction;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultGetCommonInfo: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (info == NULL) {
        HDF_LOGE("SdioDeviceDefaultGetCommonInfo: info is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoType != SDIO_FUNC_INFO) {
        HDF_LOGE("SdioDeviceDefaultGetCommonInfo: cur type %d is not support.", infoType);
        return HDF_ERR_NOT_SUPPORT;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultGetCommonInfo fail, cntlr is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    info->funcInfo.enTimeout = func->timeOut;
    info->funcInfo.maxBlockNum = cntlr->maxBlkNum;
    info->funcInfo.maxBlockSize = cntlr->maxBlkSize;
    info->funcInfo.maxRequestSize = cntlr->maxReqSize;
    info->funcInfo.funcNum = func->funcNum;
    info->funcInfo.irqCap = cntlr->caps.bits.sdioIrq;
    info->funcInfo.data = (void *)func;
    return HDF_SUCCESS;
}

static int32_t SdioDeviceDefaultSetCommonInfo(struct SdioDevice *dev,
    SdioCommonInfo *info, SdioCommonInfoType infoType)
{
    struct MmcCntlr *cntlr = NULL;
    struct SdioFunction *func = dev->curFunction;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultSetCommonInfo: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (info == NULL) {
        HDF_LOGE("SdioDeviceDefaultSetCommonInfo: info is null.");
        return HDF_ERR_INVALID_PARAM;
    }
    if (infoType != SDIO_FUNC_INFO) {
        HDF_LOGE("SdioDeviceDefaultSetCommonInfo: cur type %d is not support.", infoType);
        return HDF_ERR_NOT_SUPPORT;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultSetCommonInfo fail, cntlr is null.");
        return HDF_ERR_INVALID_PARAM;
    }

    func->timeOut = info->funcInfo.enTimeout;
    cntlr->maxBlkNum = info->funcInfo.maxBlockNum;
    cntlr->maxBlkSize = info->funcInfo.maxBlockSize;
    cntlr->maxReqSize = info->funcInfo.maxRequestSize;
    func->funcNum = info->funcInfo.funcNum;
    return HDF_SUCCESS;
}

static int32_t SdioDeviceDefaultFlushData(struct SdioDevice *dev)
{
    struct MmcCntlr *cntlr = NULL;

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultFlushData fail, cntlr is null.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return SdioReinit(cntlr);
}

static int32_t SdioDeviceDefaultEnableFunc(struct SdioDevice *dev)
{
    struct SdioFunction *func = dev->curFunction;
    int32_t ret;
    uint32_t i;
    uint8_t val;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultEnableFunc: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = SdioCccrIoEnable(dev->sd.mmc.cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioDeviceDefaultEnableFunc: Io Enable fail.");
        return ret;
    }

    for (i = 0; i < SDIO_READ_IO_READY_RETRY_TIMES; i++) {
        ret = SdioReadCccrIoReady(dev->sd.mmc.cntlr, &val);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioDeviceDefaultEnableFunc: read Io Ready fail.");
            return ret;
        }
        if ((val & (1 << func->funcNum)) > 0) {
            return HDF_SUCCESS;
        }
    }

    HDF_LOGE("SdioDeviceDefaultEnableFunc: Io not Ready.");
    return HDF_ERR_TIMEOUT;
}

static int32_t SdioDeviceDefaultDisableFunc(struct SdioDevice *dev)
{
    struct SdioFunction *func = dev->curFunction;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultDisableFunc: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    return SdioCccrIoDisable(dev->sd.mmc.cntlr);
}

static int32_t SdioDeviceDefaultClaimIrq(struct SdioDevice *dev, SdioIrqHandler *irqHandler)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;
    int32_t ret;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultClaimIrq: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (func->irqHandler != NULL) {
        HDF_LOGE("SdioDeviceDefaultClaimIrq: irq has been registered.");
        return HDF_ERR_DEVICE_BUSY;
    }
    if (irqHandler == NULL) {
        HDF_LOGE("SdioDeviceDefaultClaimIrq: irqHandler is NULL.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultClaimIrq: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = SdioCccrIntEnable(cntlr);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    func->irqHandler = irqHandler;
    if (cntlr->caps.bits.sdioIrq > 0) {
        ret = MmcCntlrCreatSdioIrqThread(cntlr);
    }
    return ret;
}

static int32_t SdioDeviceDefaultReleaseIrq(struct SdioDevice *dev)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultReleaseIrq: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultReleaseIrq: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (func->irqHandler != NULL) {
        func->irqHandler = NULL;
        MmcCntlrDestroySdioIrqThread(cntlr);
    }
    if (cntlr->caps.bits.sdioIrq > 0 && cntlr->ops != NULL && cntlr->ops->setSdioIrq != NULL) {
        (void)cntlr->ops->setSdioIrq(cntlr, false);
    }
    return SdioCccrIntDisable(cntlr);
}

static int32_t SdioDeviceDefaultFindFunc(struct SdioDevice *dev, struct SdioFunctionConfig *cfg)
{
    uint32_t i;
    struct SdioFunction *func = NULL;

    if (dev->functions > SDIO_MAX_FUNCTION_NUMBER) {
        HDF_LOGE("SdioDeviceDefaultFindFunc: functions = %d, error!", dev->functions);
        return HDF_ERR_INVALID_PARAM;
    }

    for (i = 0; i < dev->functions; i++) {
        func = dev->sdioFunc[i];
        if (func == NULL) {
            continue;
        }
        if (cfg->deviceId == func->deviceId &&
            cfg->vendorId == func->vendorId &&
            cfg->funcNr == func->funcNum) {
            dev->curFunction = func;
            return HDF_SUCCESS;
        }
    }

    HDF_LOGE("SdioDeviceDefaultFindFunc: find func fail!");
    return HDF_FAILURE;
}

static int32_t SdioDeviceDefaultClaimHost(struct SdioDevice *dev)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultClaimHost: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultClaimHost: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    MmcCntlrLock(cntlr);
    return HDF_SUCCESS;
}

static int32_t SdioDeviceDefaultReleaseHost(struct SdioDevice *dev)
{
    struct SdioFunction *func = dev->curFunction;
    struct MmcCntlr *cntlr = NULL;

    if (func == NULL) {
        HDF_LOGE("SdioDeviceDefaultReleaseHost: func is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    cntlr = dev->sd.mmc.cntlr;
    if (cntlr == NULL) {
        HDF_LOGE("SdioDeviceDefaultReleaseHost: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    MmcCntlrUnlock(cntlr);
    return HDF_SUCCESS;
}

static struct SdioDeviceOps g_sdioDefaultOps = {
    .incrAddrReadBytes = SdioDeviceDefaultIncrAddrReadBytes,
    .incrAddrWriteBytes = SdioDeviceDefaultIncrAddrWriteBytes,
    .fixedAddrReadBytes = SdioDeviceDefaultFixedAddrReadBytes,
    .fixedAddrWriteBytes = SdioDeviceDefaultFixedAddrWriteBytes,
    .func0ReadBytes = SdioDeviceDefaultFunc0ReadBytes,
    .func0WriteBytes = SdioDeviceDefaultFunc0WriteBytes,
    .setBlockSize = SdioDeviceDefaultSetBlockSize,
    .getCommonInfo = SdioDeviceDefaultGetCommonInfo,
    .setCommonInfo = SdioDeviceDefaultSetCommonInfo,
    .flushData = SdioDeviceDefaultFlushData,
    .enableFunc = SdioDeviceDefaultEnableFunc,
    .disableFunc = SdioDeviceDefaultDisableFunc,
    .claimIrq = SdioDeviceDefaultClaimIrq,
    .releaseIrq = SdioDeviceDefaultReleaseIrq,
    .findFunc = SdioDeviceDefaultFindFunc,
    .claimHost = SdioDeviceDefaultClaimHost,
    .releaseHost = SdioDeviceDefaultReleaseHost,
};

int32_t SdioDeviceFindFunction(struct SdioDevice *sdio, struct SdioFunctionConfig *config)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceFindFunction: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->findFunc == NULL) {
        HDF_LOGE("SdioDeviceFindFunction: func is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->findFunc(sdio, config);
}

int32_t SdioDeviceIncrAddrReadBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceIncrAddrReadBytes: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->incrAddrReadBytes == NULL) {
        HDF_LOGE("SdioDeviceIncrAddrReadBytes: incrAddrReadBytes is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->incrAddrReadBytes(sdio, data, addr, size);
}

int32_t SdioDeviceIncrAddrWriteBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceIncrAddrWriteBytes: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->incrAddrWriteBytes == NULL) {
        HDF_LOGE("SdioDeviceIncrAddrWriteBytes: incrAddrWriteBytes is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->incrAddrWriteBytes(sdio, data, addr, size);
}

int32_t SdioDeviceFixedAddrReadBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceFixedAddrReadBytes: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->fixedAddrReadBytes == NULL) {
        HDF_LOGE("SdioDeviceFixedAddrReadBytes: fixedAddrReadBytes is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->fixedAddrReadBytes(sdio, data, addr, size, scatterLen);
}

int32_t SdioDeviceFixedAddrWriteBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceFixedAddrWriteBytes: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->fixedAddrWriteBytes == NULL) {
        HDF_LOGE("SdioDeviceFixedAddrWriteBytes: fixedAddrWriteBytes is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->fixedAddrWriteBytes(sdio, data, addr, size, scatterLen);
}

int32_t SdioDeviceFunc0ReadBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceFunc0ReadBytes: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->func0ReadBytes == NULL) {
        HDF_LOGE("SdioDeviceFunc0ReadBytes: func0ReadBytes is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->func0ReadBytes(sdio, data, addr, size);
}

int32_t SdioDeviceFunc0WriteBytes(struct SdioDevice *sdio,
    uint8_t *data, uint32_t addr, uint32_t size)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceFunc0WriteBytes: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->func0WriteBytes == NULL) {
        HDF_LOGE("SdioDeviceFunc0WriteBytes: func0WriteBytes is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->func0WriteBytes(sdio, data, addr, size);
}

int32_t SdioDeviceSetBlockSize(struct SdioDevice *sdio, uint32_t blockSize)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceSetBlockSize: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->setBlockSize == NULL) {
        HDF_LOGE("SdioDeviceSetBlockSize: setBlockSize is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->setBlockSize(sdio, blockSize);
}

int32_t SdioDeviceGetCommonInfo(struct SdioDevice *sdio,
    SdioCommonInfo *info, SdioCommonInfoType infoType)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceGetCommonInfo: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->getCommonInfo == NULL) {
        HDF_LOGE("SdioDeviceGetCommonInfo: getCommonInfo is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->getCommonInfo(sdio, info, infoType);
}

int32_t SdioDeviceSetCommonInfo(struct SdioDevice *sdio,
    SdioCommonInfo *info, SdioCommonInfoType infoType)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceSetCommonInfo: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->setCommonInfo == NULL) {
        HDF_LOGE("SdioDeviceSetCommonInfo: setCommonInfo is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->setCommonInfo(sdio, info, infoType);
}

int32_t SdioDeviceFlushData(struct SdioDevice *sdio)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceFlushData: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->flushData == NULL) {
        HDF_LOGE("SdioDeviceFlushData: flushData is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->flushData(sdio);
}

int32_t SdioDeviceClaimHost(struct SdioDevice *sdio)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceClaimHost: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (sdio->sdioOps->claimHost != NULL) {
        return sdio->sdioOps->claimHost(sdio);
    }
    return HDF_SUCCESS;
}

int32_t SdioDeviceReleaseHost(struct SdioDevice *sdio)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceReleaseHost: cntlr is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (sdio->sdioOps->releaseHost != NULL) {
        return sdio->sdioOps->releaseHost(sdio);
    }
    return HDF_SUCCESS;
}

int32_t SdioDeviceEnableFunc(struct SdioDevice *sdio)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceEnableFunc: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->enableFunc == NULL) {
        HDF_LOGE("SdioDeviceEnableFunc: enableFunc is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->enableFunc(sdio);
}

int32_t SdioDeviceDisableFunc(struct SdioDevice *sdio)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceDisableFunc: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->disableFunc == NULL) {
        HDF_LOGE("SdioDeviceDisableFunc: disableFunc is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->disableFunc(sdio);
}

int32_t SdioDeviceClaimIrq(struct SdioDevice *sdio, SdioIrqHandler *irqHandler)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceClaimIrq: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->claimIrq == NULL) {
        HDF_LOGE("SdioDeviceClaimIrq: claimIrq is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->claimIrq(sdio, irqHandler);
}

int32_t SdioDeviceReleaseIrq(struct SdioDevice *sdio)
{
    if (sdio->sdioOps == NULL) {
        HDF_LOGE("SdioDeviceReleaseIrq: ops is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    if (sdio->sdioOps->releaseIrq == NULL) {
        HDF_LOGE("SdioDeviceReleaseIrq: releaseIrq is NULL.");
        return HDF_FAILURE;
    }
    return sdio->sdioOps->releaseIrq(sdio);
}

void SdioDeviceAddOps(struct SdioDevice *sdio, void *ops)
{
    if (sdio == NULL) {
        HDF_LOGE("SdioDeviceAddOps: sdio is NULL.");
        return;
    }
    if (ops == NULL) {
        sdio->sdioOps = &g_sdioDefaultOps;
        HDF_LOGD("SdioDeviceAddOps: use default ops.");
    } else {
        sdio->sdioOps = (struct SdioDeviceOps *)ops;
    }
}
