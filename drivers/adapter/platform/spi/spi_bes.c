/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "spi_bes.h"
#include <stdlib.h>
#include <string.h>
#include "spi_core.h"
#include "hal_cache.h"
#include "hal_trace.h"
#include "hdf_log.h"
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#include "hcs_macro.h"
#include "hdf_config_macro.h"
#else
#include "device_resource_if.h"
#endif

#define SPI_DMA_MAX 4095
#define DEC_NUM 10
#define GROUP_PIN_NUM 8
#define TIMEOUT 1000
#define SPI_PIN_0 0
#define SPI_PIN_1 1
#define SPI_PIN_2 2
#define SPI_PIN_3 3
#define DRIVER_LEVEL 3
#define MAX_SPI_NUMBER 2
#define MAX_SPI_SPEED 26000000
static void Spi0DmaIrq(int error);
static void Spi1DmaIrq(int error);

static struct SPI_CTX_OBJ_T spiCtx[MAX_SPI_NUMBER] = {
    {
        .spiPinCS0 = 0,
#if defined (CHIP_BEST1600)
        .spiFunDI0 = HAL_IOMUX_FUNC_SYS_SPI_DI0,
        .spiFunCLK = HAL_IOMUX_FUNC_SYS_SPI_CLK,
        .spiFunCS0 = HAL_IOMUX_FUNC_SYS_SPI_CS0,
        .spiFunDIO = HAL_IOMUX_FUNC_SYS_SPI_DIO,
#elif defined (CHIP_BEST2003)
        .spiFunDI0 = HAL_IOMUX_FUNC_SPI_DI0,
        .spiFunCLK = HAL_IOMUX_FUNC_SPI_CLK,
        .spiFunCS0 = HAL_IOMUX_FUNC_SPI_CS0,
        .spiFunDIO = HAL_IOMUX_FUNC_SPI_DIO,
#endif
        .sem = { NULL },
        .mutex = { NULL },
        .SpiOpen = hal_spi_open,
        .SpiDmaSend = hal_spi_dma_send,
        .SpiDmaRecv = hal_spi_dma_recv,
        .SpiSend = hal_spi_send,
        .SpiRecv = hal_spi_recv,
        .SpiDmaIrq = Spi0DmaIrq,
        .SpiClose = hal_spi_close
    },
    {
        .spiPinCS0 = 0,
#if defined (CHIP_BEST1600)
        .spiFunDI0 = HAL_IOMUX_FUNC_SYS_SPILCD_DI0,
        .spiFunCLK = HAL_IOMUX_FUNC_SYS_SPILCD_CLK,
        .spiFunCS0 = HAL_IOMUX_FUNC_SYS_SPILCD_CS0,
        .spiFunDIO = HAL_IOMUX_FUNC_SYS_SPILCD_DIO,
#elif defined (CHIP_BEST2003)
        .spiFunDI0 = HAL_IOMUX_FUNC_SPILCD_DI0,
        .spiFunCLK = HAL_IOMUX_FUNC_SPILCD_CLK,
        .spiFunCS0 = HAL_IOMUX_FUNC_SPILCD_CS0,
        .spiFunDIO = HAL_IOMUX_FUNC_SPILCD_DIO,
#endif
        .sem = { NULL },
        .mutex = { NULL },
        .SpiOpen = hal_spilcd_open,
        .SpiDmaSend = hal_spilcd_dma_send,
        .SpiDmaRecv = hal_spilcd_dma_recv,
        .SpiSend = hal_spilcd_send,
        .SpiRecv = hal_spilcd_recv,
        .SpiDmaIrq = Spi1DmaIrq,
        .SpiClose = hal_spilcd_close
    }
};

static struct HAL_IOMUX_PIN_FUNCTION_MAP pinMuxSpi[] = {
    {0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL },
    {0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL },
    {0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL },
    {0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL },
};

void Spi0DmaIrq(int error)
{
    if (HDF_SUCCESS != OsalSemPost(&spiCtx[0].sem)) {
        HDF_LOGE("spi0dmairq OsalSemPost failed!\r\n");
        return;
    }
}

void Spi1DmaIrq(int error)
{
    if (HDF_SUCCESS != OsalSemPost(&spiCtx[1].sem)) {
        HDF_LOGE("spi1dmairq OsalSemPost failed!\r\n");
        return;
    }
}

static void SpiIomuxInit(struct SpiDevice *spiDevice)
{
    struct HAL_SPI_CFG_T *spiDevCfg = NULL;
    struct SpiResource *resource = NULL;
    uint32_t spiPort;
    HDF_LOGI("%s: Enter\r\n", __func__);

    if (spiDevice == NULL) {
        HDF_LOGE("%s: invalid parameter\r\n", __func__);
        return;
    }

    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is null\r\n");
        return;
    }
    spiDevCfg = &spiDevice->spiDevCfg;
    if (spiDevCfg == NULL) {
        HDF_LOGE("resource is null\r\n");
        return;
    }
    spiDevCfg->rate = resource->speed;

    spiPort = spiDevice->spiId;

    pinMuxSpi[SPI_PIN_0].pin = resource->spiMisoPin;
    pinMuxSpi[SPI_PIN_1].pin = resource->spiClkPin;
    pinMuxSpi[SPI_PIN_2].pin = resource->spiCsPin;
    pinMuxSpi[SPI_PIN_3].pin = resource->spiMosiPin;

    spiCtx[spiPort].spiPinCS0 = resource->spiCsPin;

    pinMuxSpi[SPI_PIN_0].function = spiCtx[spiPort].spiFunDI0;
    pinMuxSpi[SPI_PIN_1].function = spiCtx[spiPort].spiFunCLK;

    if (resource->spiCsSoft) {
        spiCtx[spiPort].spiFunCS0 = HAL_IOMUX_FUNC_AS_GPIO;
    }

    pinMuxSpi[SPI_PIN_2].function = spiCtx[spiPort].spiFunCS0;
    pinMuxSpi[SPI_PIN_3].function = spiCtx[spiPort].spiFunDIO;

    hal_iomux_set_io_drv(pinMuxSpi[SPI_PIN_0].pin, DRIVER_LEVEL);
    hal_iomux_set_io_drv(pinMuxSpi[SPI_PIN_1].pin, DRIVER_LEVEL);
    hal_iomux_set_io_drv(pinMuxSpi[SPI_PIN_2].pin, DRIVER_LEVEL);
    hal_iomux_set_io_drv(pinMuxSpi[SPI_PIN_3].pin, DRIVER_LEVEL);

    hal_iomux_init(pinMuxSpi, ARRAY_SIZE(pinMuxSpi));
}

static int32_t SpiDevCfgInit(struct HAL_SPI_CFG_T *spiDevCfg, struct SpiResource *resource)
{
    if (spiDevCfg == NULL || resource == NULL) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    switch (resource->mode) {
        case SPI_WORK_MODE_0:
            spiDevCfg->clk_delay_half = false;
            spiDevCfg->clk_polarity = false;
            break;
        case SPI_WORK_MODE_1:
            spiDevCfg->clk_delay_half = true;
            spiDevCfg->clk_polarity = false;
            break;
        case SPI_WORK_MODE_2:
            spiDevCfg->clk_delay_half = false;
            spiDevCfg->clk_polarity = true;
            break;
        case SPI_WORK_MODE_3:
            spiDevCfg->clk_delay_half = true;
            spiDevCfg->clk_polarity = true;
            break;
        default:
            spiDevCfg->clk_delay_half = true;
            spiDevCfg->clk_polarity = true;
    }
    spiDevCfg->slave = 0;
    if (resource->transmode == SPI_TRANSFER_DMA) {
        spiDevCfg->dma_rx = true;
        spiDevCfg->dma_tx = true;
    } else {
        spiDevCfg->dma_rx = false;
        spiDevCfg->dma_tx = false;
    }
    spiDevCfg->cs = 0;
    spiDevCfg->rx_bits = resource->dataSize;
    spiDevCfg->tx_bits = resource->dataSize;
    spiDevCfg->rx_frame_bits = 0;
    return HDF_SUCCESS;
}
/**
 * Spi send
 *
 * @param[in]  spiId   the spi bus id
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeOut  timeOut in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */

#ifdef HalSpiSend
#undef HalSpiSend
#endif
int32_t HalSpiSend(struct SpiDevice *spiDevice, const uint8_t *data, uint16_t size, uint32_t timeOut)
{
    int32_t ret;
    uint32_t spiId;
    uint32_t len = size;
    struct SpiResource *resource = NULL;
    int32_t status = HDF_FAILURE;

    if (spiDevice == NULL || data == NULL || size == 0) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    spiId = spiDevice->spiId;
    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is null\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }
    status = OsalMutexLock(&spiCtx[spiId].mutex);
    if (HDF_SUCCESS != status) {
        HDF_LOGE("%s spi_mutex wait error = 0x%X!\r\n", __func__, status);
        return HDF_ERR_TIMEOUT;
    }
#ifdef CHIP_BEST1600
    hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
#endif
    if (resource->transmode == SPI_TRANSFER_DMA) {
        ret = spiCtx[spiId].SpiDmaSend(data, len, spiCtx[spiId].SpiDmaIrq);
        if (OsalSemWait(&spiCtx[spiId].sem, timeOut) != HDF_SUCCESS) {
            HDF_LOGE("spi dma send timeOut\r\n");
            goto OUT;
        }
    } else {
        ret = spiCtx[spiId].SpiSend(data, len);
    }

    if (ret) {
        HDF_LOGE("spi tail send fail %ld, size %ld\r\n", ret, len);
        goto OUT;
    }
OUT:
    OsalMutexUnlock(&spiCtx[spiId].mutex);
    return ret;
}

#ifdef HalSpiRecv
#undef HalSpiRecv
#endif

/**
 * SpiRecv
 *
 * @param[in]   spiId   the spi bus id
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeOut  timeOut in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
static int32_t HalSpiRecv(struct SpiDevice *spiDevice, uint8_t *data, uint16_t size, uint32_t timeOut)
{
    int32_t ret;
    int32_t len = (int32_t)size;
    uint32_t remainder;
    int32_t status = HDF_FAILURE;
    uint8_t *cmd = NULL;
    uint32_t spiId;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL || data == NULL || size == 0) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    spiId = spiDevice->spiId;
    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("resource is null\r\n");
        return HDF_ERR_INVALID_OBJECT;
    }
    cmd = (uint8_t *)0x20020000; // 0x20020000 : non secure sram base. tx buf is useless, use sram instead of malloc.

    status = OsalMutexLock(&spiCtx[spiId].mutex);
    if (HDF_SUCCESS != status) {
        HDF_LOGE("%s spi_mutex wait error = 0x%X!\r\n", __func__, status);
        return HDF_ERR_TIMEOUT;
    }
#ifdef CHIP_BEST1600
    hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
#endif
    do {
        remainder = len <= SPI_DMA_MAX ? len : SPI_DMA_MAX;
        if (resource->transmode == SPI_TRANSFER_DMA) {
            ret = spiCtx[spiId].SpiDmaRecv(cmd, data, remainder, spiCtx[spiId].SpiDmaIrq);
            if (OsalSemWait(&spiCtx[spiId].sem, timeOut) <= 0) {
                HDF_LOGE("SPI Read timeOut!\r\n");
                goto OUT;
            }
        } else {
            ret = spiCtx[spiId].SpiRecv(cmd, data, remainder);
        }

        len -= remainder;
        data += remainder;

        if (ret  != 0) {
            HDF_LOGE("spi tail fail %ld, size %ld\r\n", ret, len);
            goto OUT;
        }
    } while (len);
OUT:
    OsalMutexUnlock(&spiCtx[spiId].mutex);
    return ret;
}

#ifdef HalSpiSendRecv
#undef HalSpiSendRecv
#endif
static int32_t HalSpiSendRecv(struct SpiDevice *spiDevice, uint8_t *txData, uint16_t txSize, uint8_t *rxData,
                        uint16_t rxSize)
{
    int32_t ret;
    int32_t status;
    uint32_t spiId;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL || txData == NULL || txSize == 0 || rxData == NULL || rxSize == 0) {
        HDF_LOGE("spi input para err\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    spiId = spiDevice->spiId;
    resource = &spiDevice->resource;
    status = OsalMutexLock(&spiCtx[spiId].mutex);
    if (HDF_SUCCESS != status) {
        HDF_LOGE("%s OsalMutexLock error = 0x%X!\r\n", __func__, status);
        return HDF_ERR_TIMEOUT;
    }
#ifdef CHIP_BEST1600
    hal_cache_sync_all(HAL_CACHE_ID_D_CACHE);
#endif
    if (resource->transmode == SPI_TRANSFER_DMA) {
        ret = spiCtx[spiId].SpiDmaRecv(txData, rxData, rxSize, spiCtx[spiId].SpiDmaIrq);
        if (OsalSemWait(&spiCtx[spiId].sem, TIMEOUT) <= 0) {
            HDF_LOGE("%s:SPI Read timeOut!\r\n", __func__);
            goto OUT;
        }
    } else {
        ret = spiCtx[spiId].SpiRecv(txData, rxData, rxSize);
    }
    if (ret) {
        HDF_LOGE("spi dma tail fail %d\r\n", ret);
    }
OUT:
    OsalMutexUnlock(&spiCtx[spiId].mutex);
    return ret;
}

static int32_t InitSpiDevice(struct SpiDevice *spiDevice)
{
    uint32_t spiPort;
    int32_t ret;
    struct HAL_SPI_CFG_T *spiDevCfg = NULL;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL) {
        HDF_LOGE("%s: invalid parameter\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    resource = &spiDevice->resource;
    spiDevCfg = &spiDevice->spiDevCfg;
    spiPort = spiDevice->spiId;

    SpiIomuxInit(spiDevice);
    ret = SpiDevCfgInit(spiDevCfg, resource);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SPI config init failed\r\n", __func__);
        return HDF_FAILURE;
    }
    /* spi open move to hdf open api */
    /* if cs use as gpio ,pull up at first */
    if (spiCtx[spiPort].spiFunCS0 == HAL_IOMUX_FUNC_AS_GPIO) {
        hal_gpio_pin_set_dir(spiCtx[spiPort].spiPinCS0, HAL_GPIO_DIR_OUT, 1);
    }

    if (&spiCtx[spiPort].sem == NULL) {
        if (OsalSemInit(&spiCtx[spiPort].sem, 0) != HDF_SUCCESS) {
            HDF_LOGE("spiDmaSemaphore create failed!\r\n");
            return HDF_FAILURE;
        }
    }

    if (&spiCtx[spiPort].mutex == NULL) {
        if (OsalMutexInit(&spiCtx[spiPort].mutex) != HDF_SUCCESS) {
            HDF_LOGE("spi Mutex create failed!\r\n");
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

/* get spi config from hcs file */
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
#define SPI_FIND_CONFIG(node, name, resource) \
    do { \
        if (strcmp(HCS_PROP(node, match_attr), name) == 0) { \
            resource->num = HCS_PROP(node, busNum); \
            resource->speed = HCS_PROP(node, speed); \
            resource->transmode = HCS_PROP(node, transmode); \
            resource->spiCsSoft = HCS_PROP(node, spiCsSoft); \
            resource->mode = HCS_PROP(node, mode); \
            resource->dataSize = HCS_PROP(node, dataSize); \
            resource->csNum = HCS_PROP(node, csNum); \
            tempPin = HCS_PROP(node, spiClkPin); \
            resource->spiClkPin = ((tempPin / DEC_NUM) * GROUP_PIN_NUM) + (tempPin % DEC_NUM); \
            tempPin = HCS_PROP(node, spiMosiPin); \
            resource->spiMosiPin = ((tempPin / DEC_NUM) * GROUP_PIN_NUM) + (tempPin % DEC_NUM); \
            tempPin = HCS_PROP(node, spiMisoPin); \
            resource->spiMisoPin = ((tempPin / DEC_NUM) * GROUP_PIN_NUM) + (tempPin % DEC_NUM); \
            tempPin = HCS_PROP(node, spiCsPin); \
            resource->spiCsPin = ((tempPin / DEC_NUM) * GROUP_PIN_NUM) + (tempPin % DEC_NUM); \
            result = HDF_SUCCESS; \
            break; \
        } \
    } while (0)

#define PLATFORM_CONFIG HCS_NODE(HCS_ROOT, platform)
#define PLATFORM_SPI_CONFIG HCS_NODE(HCS_NODE(HCS_ROOT, platform), spi_config)
static int32_t GetSpiDeviceResource(struct SpiDevice *spiDevice, const char *deviceMatchAttr)
{
    uint32_t tempPin;
    int32_t result = HDF_FAILURE;
    struct SpiResource *resource = NULL;
    if (spiDevice == NULL || deviceMatchAttr == NULL) {
        HDF_LOGE("device or deviceMatchAttr is NULL\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &spiDevice->resource;
#if HCS_NODE_HAS_PROP(PLATFORM_CONFIG, spi_config)
    HCS_FOREACH_CHILD_VARGS(PLATFORM_SPI_CONFIG, SPI_FIND_CONFIG, deviceMatchAttr, resource);
#endif
    if (result != HDF_SUCCESS) {
        HDF_LOGE("resourceNode %s is NULL\r\n", deviceMatchAttr);
    }
    return result;
}
#else
static int32_t GetSpiDeviceResource(struct SpiDevice *spiDevice, const struct DeviceResourceNode *resourceNode)
{
    uint32_t relPin;
    struct SpiResource *resource = NULL;
    struct DeviceResourceIface *dri = NULL;
    if (spiDevice == NULL || resourceNode == NULL) {
        HDF_LOGE("%s: PARAM is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    resource = &spiDevice->resource;
    if (resource == NULL) {
        HDF_LOGE("%s: resource is NULL\r\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    dri = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);   // open HDF
    if (dri == NULL || dri->GetUint32 == NULL) {
        HDF_LOGE("DeviceResourceIface is invalid\r\n");
        return HDF_ERR_INVALID_PARAM;
    }

    if (dri->GetUint32(resourceNode, "busNum", &resource->num, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read num fail\r\n");
        return HDF_FAILURE;
    }

    spiDevice->spiId = resource->num;
    if (dri->GetUint32(resourceNode, "speed", &resource->speed, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read base fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "transmode", &resource->transmode, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read transmode fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "spiCsSoft", &resource->spiCsSoft, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read spiCsSoft fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "mode", &resource->mode, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read mode fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "dataSize", &resource->dataSize, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read dataSize fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "csNum", &resource->csNum, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read csNum fail\r\n");
        return HDF_FAILURE;
    }

    if (dri->GetUint32(resourceNode, "spiClkPin", &resource->spiClkPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read spiClkPin fail\r\n");
        return HDF_FAILURE;
    }

    relPin = (resource->spiClkPin / DEC_NUM) * GROUP_PIN_NUM + (resource->spiClkPin % DEC_NUM);
    resource->spiClkPin = relPin;

    if (dri->GetUint32(resourceNode, "spiMosiPin", &resource->spiMosiPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read spiMosiPin fail\r\n");
        return HDF_FAILURE;
    }

    relPin = (resource->spiMosiPin / DEC_NUM) * GROUP_PIN_NUM + (resource->spiMosiPin % DEC_NUM);
    resource->spiMosiPin = relPin;
    if (dri->GetUint32(resourceNode, "spiMisoPin", &resource->spiMisoPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read spiMisoPin fail\r\n");
        return HDF_FAILURE;
    }

    relPin = ((resource->spiMisoPin / DEC_NUM) * GROUP_PIN_NUM) + (resource->spiMisoPin % DEC_NUM);
    resource->spiMisoPin = relPin;

    if (dri->GetUint32(resourceNode, "spiCsPin", &resource->spiCsPin, 0) != HDF_SUCCESS) {
        HDF_LOGE("spi config read spiCsPin fail\r\n");
        return HDF_FAILURE;
    }

    relPin = ((resource->spiCsPin / DEC_NUM) * GROUP_PIN_NUM) + (resource->spiCsPin % DEC_NUM);
    resource->spiCsPin = relPin;

    return HDF_SUCCESS;
}
#endif

int32_t AttachSpiDevice(struct SpiCntlr *spiCntlr, struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SpiDevice *spiDevice = NULL;
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    if (spiCntlr == NULL || device == NULL) {
#else
    if (spiCntlr == NULL || device == NULL || device->property == NULL) {
#endif
        HDF_LOGE("%s: property is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spiDevice = (struct SpiDevice *)OsalMemAlloc(sizeof(struct SpiDevice));
    if (spiDevice == NULL) {
        HDF_LOGE("%s: OsalMemAlloc spiDevice error\r\n", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
#ifdef LOSCFG_DRIVERS_HDF_CONFIG_MACRO
    ret = GetSpiDeviceResource(spiDevice, device->deviceMatchAttr);
#else
    ret = GetSpiDeviceResource(spiDevice, device->property);
#endif
    if (ret != HDF_SUCCESS) {
        (void)OsalMemFree(spiDevice);
        return HDF_FAILURE;
    }

    spiCntlr->priv = spiDevice;
    spiCntlr->busNum = spiDevice->spiId;
    return InitSpiDevice(spiDevice);
}
/* SPI Method */
static int32_t SpiDevGetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg);
static int32_t SpiDevSetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg);
static int32_t SpiDevTransfer(struct SpiCntlr *spiCntlr, struct SpiMsg *spiMsg, uint32_t count);
static int32_t SpiDevOpen(struct SpiCntlr *spiCntlr);
static int32_t SpiDevClose(struct SpiCntlr *spiCntlr);

struct SpiCntlrMethod g_SpiCntlrMethod = {
    .GetCfg = SpiDevGetCfg,
    .SetCfg = SpiDevSetCfg,
    .Transfer = SpiDevTransfer,
    .Open = SpiDevOpen,
    .Close = SpiDevClose,
};

/* HdfDriverEntry method definitions */
static int32_t SpiDriverBind(struct HdfDeviceObject *device);
static int32_t SpiDriverInit(struct HdfDeviceObject *device);
static void SpiDriverRelease(struct HdfDeviceObject *device);

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_SpiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "BES_SPI_MODULE_HDF",
    .Bind = SpiDriverBind,
    .Init = SpiDriverInit,
    .Release = SpiDriverRelease,
};

HDF_INIT(g_SpiDriverEntry);

static int32_t SpiDriverBind(struct HdfDeviceObject *device)
{
    struct SpiCntlr *spiCntlr = NULL;
    if (device == NULL) {
        HDF_LOGE("Sample device object is null!\r\n");
        return HDF_ERR_INVALID_PARAM;
    }
    HDF_LOGI("Enter %s:\r\n", __func__);
    spiCntlr = (struct SpiCntlr *)OsalMemAlloc(sizeof(struct SpiCntlr));
    if (spiCntlr == NULL) {
        HDF_LOGE("%s: host is NULL\r\n", __func__);
        return HDF_FAILURE;
    }
    device->service = &spiCntlr->service;
    spiCntlr->device = device;
    spiCntlr->priv = NULL;
    return HDF_SUCCESS;
}

static int32_t SpiDriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct SpiCntlr *spiCntlr = NULL;

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    HDF_LOGI("Enter %s:", __func__);
    spiCntlr = SpiCntlrFromDevice(device);
    if (spiCntlr == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL", __func__);
        return HDF_DEV_ERR_NO_DEVICE;
    }

    ret = AttachSpiDevice(spiCntlr, device); // SpiCntlr add SpiDevice to priv
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: attach error\r\n", __func__);
        return HDF_DEV_ERR_ATTACHDEV_FAIL;
    }

    spiCntlr->method = &g_SpiCntlrMethod; // register callback

    return ret;
}

static void SpiDriverRelease(struct HdfDeviceObject *device)
{
    struct SpiCntlr *spiCntlr = NULL;
    struct SpiDevice *spiDevice = NULL;

    HDF_LOGI("Enter %s\r\n", __func__);

    if (device == NULL) {
        HDF_LOGE("%s: device is NULL\r\n", __func__);
        return;
    }

    spiCntlr = SpiCntlrFromDevice(device);
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return;
    }

    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice != NULL) {
        OsalMemFree(spiDevice);
    }
    return;
}

static int32_t SpiDevOpen(struct SpiCntlr *spiCntlr)
{
    HDF_LOGI("Enter %s\r\n", __func__);
    int ret;
    uint32_t spiPort;
    struct SpiDevice *spiDevice = NULL;
    struct HAL_SPI_CFG_T *spiDevCfg = NULL;
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    spiPort = spiDevice->spiId;
    spiDevCfg = &spiDevice->spiDevCfg;
    if (spiDevCfg == NULL) {
        HDF_LOGE("spi %u config is NULL\r\n", spiPort);
        return HDF_DEV_ERR_NO_DEVICE;
    }
    ret = spiCtx[spiPort].SpiOpen(spiDevCfg);
    if (ret != 0) {
        HDF_LOGE("spi %u open error %d\r\n", spiPort, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SpiDevClose(struct SpiCntlr *spiCntlr)
{
    int ret;
    uint32_t spiPort;
    struct SpiDevice *spiDevice = NULL;
    struct HAL_SPI_CFG_T *spiDevCfg = NULL;
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    spiPort = spiDevice->spiId;
    spiDevCfg = &spiDevice->spiDevCfg;
    if (spiDevCfg == NULL) {
        HDF_LOGE("spi %u config is NULL\r\n", spiPort);
        return HDF_DEV_ERR_NO_DEVICE;
    }
    ret = spiCtx[spiPort].SpiClose(spiDevCfg->cs);
    if (ret != 0) {
        HDF_LOGE("spi %ld open error %d\r\n", spiPort, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t SpiDevGetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg)
{
    struct SpiDevice *spiDevice = NULL;
    if (spiCntlr == NULL || spiCfg == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
    spiCfg->maxSpeedHz = spiDevice->resource.speed;
    spiCfg->mode = spiDevice->resource.mode;
    spiCfg->transferMode = spiDevice->resource.transmode;
    spiCfg->bitsPerWord = spiDevice->resource.dataSize;

    return HDF_SUCCESS;
}

static int32_t SpiDevSetCfg(struct SpiCntlr *spiCntlr, struct SpiCfg *spiCfg)
{
    struct SpiDevice *spiDevice = NULL;
    struct HAL_SPI_CFG_T *spiDevCfg = NULL;
    if (spiCntlr == NULL || spiCfg == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    if (spiDevice == NULL) {
        return HDF_DEV_ERR_NO_DEVICE;
    }
    spiDevCfg = &spiDevice->spiDevCfg;
    if (spiDevCfg == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    spiDevice->resource.speed = spiCfg->maxSpeedHz;
    spiDevice->resource.mode = spiCfg->mode;
    spiDevice->resource.transmode = spiCfg->transferMode;
    spiDevice->resource.dataSize  = spiCfg->bitsPerWord;
    spiDevCfg->rate = spiCfg->maxSpeedHz;
    return InitSpiDevice(spiDevice);
}

static int32_t SpiDevTransfer(struct SpiCntlr *spiCntlr, struct SpiMsg *spiMsg, uint32_t count)
{
    uint32_t spiId;
    struct SpiDevice *spiDevice = NULL;
    struct HAL_SPI_CFG_T *spiDevCfg = NULL;
    struct SpiMsg *msg = NULL;
    if (spiCntlr == NULL || spiCntlr->priv == NULL) {
        HDF_LOGE("%s: spiCntlr is NULL\r\n", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    spiDevice = (struct SpiDevice *)spiCntlr->priv;
    spiId = spiDevice->spiId;
    for (size_t i = 0; i < count; i++) {
        msg = &spiMsg[i];
        /* pull down cs at first */
        if (spiDevice->resource.spiCsSoft) {
            hal_gpio_pin_set_dir(spiCtx[spiId].spiPinCS0, HAL_GPIO_DIR_OUT, 0);
        }

        if ((msg->wbuf != NULL) && (msg->rbuf == NULL)) {
            HalSpiSend(spiDevice, msg->wbuf, msg->len, TIMEOUT);
        }
        if ((msg->rbuf != NULL) && (msg->wbuf == NULL)) {
            HalSpiRecv(spiDevice, msg->rbuf, msg->len, TIMEOUT);
        }
        if ((msg->wbuf != NULL) && (msg->rbuf != NULL)) {
            HalSpiSendRecv(spiDevice, msg->wbuf, msg->len, msg->rbuf, msg->len);
        }

        /* pull pull up cs at the end */
        if (msg->csChange) {
            hal_gpio_pin_set_dir(spiCtx[spiId].spiPinCS0, HAL_GPIO_DIR_OUT, 1);
        }
        DelayUs(msg->delayUs);
    }
    return HDF_SUCCESS;
}
