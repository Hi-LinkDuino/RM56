/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_sdio_intf.h"
#include "osal_mem.h"
#include "sdio_if.h"
#include "wifi_inc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static int32_t HdfGetSdioInfo(struct BusDev *dev, struct BusConfig *busCfg)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    SdioCommonInfo palSdioCommonInfo;
    if (dev == NULL || busCfg == NULL || busCfg->busType != BUS_SDIO) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    (void)memset_s(&palSdioCommonInfo, sizeof(SdioCommonInfo), 0, sizeof(SdioCommonInfo));

    ret = SdioGetCommonInfo(handle, &palSdioCommonInfo, SDIO_FUNC_INFO);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:get sdio info error!", __func__);
        return ret;
    }
    busCfg->busInfo.sdioInfo.maxBlockNum = palSdioCommonInfo.funcInfo.maxBlockNum;
    busCfg->busInfo.sdioInfo.maxBlockSize = palSdioCommonInfo.funcInfo.maxBlockSize;
    busCfg->busInfo.sdioInfo.maxRequestSize = palSdioCommonInfo.funcInfo.maxRequestSize;
    busCfg->busInfo.sdioInfo.funcNumSize = palSdioCommonInfo.funcInfo.funcNum;
    busCfg->busInfo.sdioInfo.irqCap = palSdioCommonInfo.funcInfo.irqCap;
    busCfg->busInfo.sdioInfo.data = palSdioCommonInfo.funcInfo.data;

    return ret;
}

static void HdfSdioReleaseDev(struct BusDev *dev)
{
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return;
    }
    if (dev->priData.data != NULL) {
        dev->priData.release(dev->priData.data);
        dev->priData.data = NULL;
    }
    OsalMemFree(dev);
    dev = NULL;
}

static int32_t HdfSdioEnableFunc(struct BusDev *dev)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioEnableFunc(handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:enable sdio func failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioDisableFunc(struct BusDev *dev)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioDisableFunc(handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:disable sdio func failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioCliamIrq(struct BusDev *dev, IrqHandler *handler, void *data)
{
    (void)data;
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioClaimIrq(handle, (SdioIrqHandler *)handler);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:claim sdio irq failed!", __func__);
    }
    return ret;
}

static void HdfSdioClaimHost(struct BusDev *dev)
{
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return;
    }
    handle = (struct DevHandle *)dev->devBase;
    SdioClaimHost(handle);
}

static void HdfSdioReleaseHost(struct BusDev *dev)
{
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return;
    }
    handle = (struct DevHandle *)dev->devBase;
    SdioReleaseHost(handle);
}


static int32_t HdfSdioReleaseIrq(struct BusDev *dev)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioReleaseIrq(handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:release sdio irq failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioReset(struct BusDev *dev)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioFlushData(handle);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:reset sdio failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioReadN(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioReadBytes(handle, buf, addr, cnt);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:read sdio data failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioReadFunc0(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioReadBytesFromFunc0(handle, buf, addr, cnt);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:read sdio func0 data failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioReadSpcReg(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf, uint32_t sg_len)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioReadBytesFromFixedAddr(handle, buf, addr, cnt, sg_len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:read sdio special reg data failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioWriteN(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL || buf == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioWriteBytes(handle, buf, addr, cnt);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:write sdio data failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioWriteFunc0(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL || buf == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioWriteBytesToFunc0(handle, buf, addr, cnt);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:write sdio func0 data failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioWriteSpcReg(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf, uint32_t sg_len)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL || buf == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioWriteBytesToFixedAddr(handle, buf, addr, cnt, sg_len);
    if (ret != HDF_SUCCESS) {
        HDF_LOGI("%s:write sdio special reg data failed!", __func__);
    }
    return ret;
}

static int32_t HdfSdioSetBlk(struct BusDev *dev, uint32_t blkSize)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    if (dev == NULL) {
        HDF_LOGE("%s:input parameter error!", __func__);
        return HDF_FAILURE;
    }
    handle = (struct DevHandle *)dev->devBase;
    ret = SdioSetBlockSize(handle, blkSize);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:sdio set block size failed!", __func__);
    }
    return ret;
}

static struct DevHandle *HdfGetDevHandle(struct BusDev *dev, const struct HdfConfigWlanBus *busCfg)
{
    struct DevHandle *handle = NULL;
    int32_t cnt;
    struct SdioFunctionConfig palSdioConfig[WLAN_MAX_CHIP_NUM] = {0};
    struct HdfConfigWlanChipList *tmpChipList = NULL;

    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        HDF_LOGE("%s: NULL ptr!", __func__);
        return NULL;
    }
    tmpChipList = &rootConfig->wlanConfig.chipList;
    for (cnt = 0; (cnt < tmpChipList->chipInstSize) && (cnt < WLAN_MAX_CHIP_NUM); cnt++) {
        // once detected card break
        palSdioConfig[cnt].deviceId = tmpChipList->chipInst[cnt].chipSdio.deviceId[0];
        palSdioConfig[cnt].vendorId = tmpChipList->chipInst[cnt].chipSdio.vendorId;
        palSdioConfig[cnt].funcNr = busCfg->funcNum[0];
        handle = SdioOpen(busCfg->busIdx, &palSdioConfig[cnt]);
        if (handle != NULL) {
            HDF_LOGI("%s: sdio card detected!", __func__);
            break;
        }
    }
    if (cnt == tmpChipList->chipInstSize || cnt == WLAN_MAX_CHIP_NUM) {
        HDF_LOGE("%s: NO sdio card detected!", __func__);
        SdioClose(handle);
        return NULL;
    }
    dev->devBase = handle;
    dev->priData.driverName = tmpChipList->chipInst[cnt].driverName;
    return handle;
}

static int32_t HdfSdioInit(struct BusDev *dev, const struct HdfConfigWlanBus *busCfg)
{
    int32_t ret;
    struct DevHandle *handle = NULL;
    SdioCommonInfo palSdioCommonInfo;

    if (dev == NULL || busCfg == NULL) {
        HDF_LOGE("%s: input parameter error!", __func__);
        goto sdioInitFail;
    }
    handle = HdfGetDevHandle(dev, busCfg);
    if (handle == NULL) {
        HDF_LOGE("%s: sdio card detected fail!", __func__);
        return HDF_FAILURE;
    }
    SdioClaimHost(handle);
    (void)memset_s(&palSdioCommonInfo, sizeof(SdioCommonInfo), 0, sizeof(SdioCommonInfo));
    SdioGetCommonInfo(handle, &palSdioCommonInfo, SDIO_FUNC_INFO);
    palSdioCommonInfo.funcInfo.enTimeout = busCfg->timeout;
    SdioSetCommonInfo(handle, &palSdioCommonInfo, SDIO_FUNC_INFO);
    ret = HdfSdioEnableFunc(dev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable sdio failed!", __func__);
        goto sdioInitFail;
    }
    ret = HdfSdioSetBlk(dev, busCfg->blockSize);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set sdio block size failed!", __func__);
        goto sdioInitFail;
    }
    SdioReleaseHost(handle);
    HDF_LOGI("%s: sdio bus init success!", __func__);
    return ret;

sdioInitFail:
    SdioClose(handle);
    return HDF_FAILURE;
}

static void HdfSetBusOps(struct BusDev *dev)
{
    dev->ops.getBusInfo = HdfGetSdioInfo;
    dev->ops.deInit = HdfSdioReleaseDev;
    dev->ops.init = HdfSdioInit;

    dev->ops.readData = HdfSdioReadN;
    dev->ops.writeData = HdfSdioWriteN;
    dev->ops.bulkRead = HdfSdioReadSpcReg;
    dev->ops.bulkWrite = HdfSdioWriteSpcReg;
    dev->ops.readFunc0 = HdfSdioReadFunc0;
    dev->ops.writeFunc0 = HdfSdioWriteFunc0;

    dev->ops.claimIrq = HdfSdioCliamIrq;
    dev->ops.releaseIrq = HdfSdioReleaseIrq;
    dev->ops.disableBus = HdfSdioDisableFunc;
    dev->ops.reset = HdfSdioReset;

    dev->ops.claimHost = HdfSdioClaimHost;
    dev->ops.releaseHost = HdfSdioReleaseHost;
}
int32_t HdfSdioBusInit(struct BusDev *dev, const struct HdfConfigWlanBus *busConfig)
{
    if (dev == NULL) {
        HDF_LOGE("%s:set sdio device ops failed!", __func__);
        return HDF_FAILURE;
    }
    HdfSetBusOps(dev);
    return HdfSdioInit(dev, busConfig);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
