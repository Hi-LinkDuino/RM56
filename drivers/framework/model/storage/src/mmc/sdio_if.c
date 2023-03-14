/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "sdio_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "mmc_sdio.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG sdio_if_c

static int32_t SdioDeviceGetFromHandle(DevHandle handle, struct SdioDevice **sdio)
{
    struct MmcDevice *mmc = NULL;

    if (handle == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }

    if (sdio == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    mmc = MmcCntlrGetDevice((struct MmcCntlr *)handle);
    if (mmc == NULL) {
        return HDF_PLT_ERR_NO_DEV;
    }
    if (mmc->type != MMC_DEV_SDIO && mmc->type != MMC_DEV_COMBO) {
        MmcDevicePut(mmc);
        return HDF_PLT_ERR_DEV_TYPE;
    }

    *sdio = (struct SdioDevice *)mmc;
    return HDF_SUCCESS;
}

DevHandle SdioOpen(int16_t mmcBusNum, struct SdioFunctionConfig *config)
{
    int32_t ret;
    struct MmcCntlr *cntlr = NULL;
    struct SdioDevice *sdio = NULL;
    DevHandle handle = NULL;

    if (config == NULL) {
        HDF_LOGE("SdioOpen: config can't be null!");
        return NULL;
    }

    handle = MmcOpen(mmcBusNum);
    if (handle == NULL) {
        HDF_LOGE("SdioOpen: SdioGetCntlrByBusNum fail!");
        return NULL;
    }
    cntlr = (struct MmcCntlr *)handle;
    if (cntlr != NULL && cntlr->ops != NULL && cntlr->ops->rescanSdioDev != NULL) {
        ret = cntlr->ops->rescanSdioDev(cntlr);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("SdioOpen: sdio rescan fail!");
            MmcClose(handle);
            return NULL;
        }
    }

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioOpen: get sdio dev fail!");
        MmcClose(handle);
        return NULL;
    }
    ret = SdioDeviceFindFunction(sdio, config);
    MmcDevicePut((struct MmcDevice *)sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioOpen: set function fail!");
        MmcClose(handle);
        return NULL;
    }

    return (DevHandle)cntlr;
}

void SdioClose(DevHandle handle)
{
    (void)handle;
}

int32_t SdioReadBytes(DevHandle handle, uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioReadBytes: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceIncrAddrReadBytes(sdio, data, addr, size);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioWriteBytes(DevHandle handle, uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioWriteBytes: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceIncrAddrWriteBytes(sdio, data, addr, size);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioReadBytesFromFixedAddr(DevHandle handle, uint8_t *data,
    uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioReadBytesFromFixedAddr: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceFixedAddrReadBytes(sdio, data, addr, size, scatterLen);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioWriteBytesToFixedAddr(DevHandle handle, uint8_t *data,
    uint32_t addr, uint32_t size, uint32_t scatterLen)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioWriteBytesToFixedAddr: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceFixedAddrWriteBytes(sdio, data, addr, size, scatterLen);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioReadBytesFromFunc0(DevHandle handle, uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioReadBytesFromFunc0: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceFunc0ReadBytes(sdio, data, addr, size);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioWriteBytesToFunc0(DevHandle handle, uint8_t *data, uint32_t addr, uint32_t size)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioWriteBytesToFunc0: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceFunc0WriteBytes(sdio, data, addr, size);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioSetBlockSize(DevHandle handle, uint32_t blockSize)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioSetBlockSize: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceSetBlockSize(sdio, blockSize);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioGetCommonInfo(DevHandle handle, SdioCommonInfo *info, SdioCommonInfoType infoType)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioGetCommonInfo: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceGetCommonInfo(sdio, info, infoType);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioSetCommonInfo(DevHandle handle, SdioCommonInfo *info, SdioCommonInfoType infoType)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioSetCommonInfo: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceSetCommonInfo(sdio, info, infoType);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioFlushData(DevHandle handle)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioFlushData: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceFlushData(sdio);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

void SdioClaimHost(DevHandle handle)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioClaimHost: get sdio dev fail!");
        return;
    }
    ret = SdioDeviceClaimHost(sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioClaimHost: claim host fail!");
    }
    MmcDevicePut((struct MmcDevice *)sdio);
}

void SdioReleaseHost(DevHandle handle)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioReleaseHost: get sdio dev fail!");
        return;
    }
    ret = SdioDeviceReleaseHost(sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioReleaseHost: claim host fail!");
    }
    MmcDevicePut((struct MmcDevice *)sdio);
}

int32_t SdioEnableFunc(DevHandle handle)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioEnableFunc: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceEnableFunc(sdio);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioDisableFunc(DevHandle handle)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioDisableFunc: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceDisableFunc(sdio);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioClaimIrq(DevHandle handle, SdioIrqHandler *irqHandler)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioClaimIrq: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceClaimIrq(sdio, irqHandler);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}

int32_t SdioReleaseIrq(DevHandle handle)
{
    int32_t ret;
    struct SdioDevice *sdio = NULL;

    ret = SdioDeviceGetFromHandle(handle, &sdio);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("SdioReleaseIrq: get sdio dev fail!");
        return ret;
    }
    ret = SdioDeviceReleaseIrq(sdio);
    MmcDevicePut((struct MmcDevice *)sdio);
    return ret;
}
