/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "mtd_spi_common.h"
#include "hdf_log.h"

static void SpiFlashDumpDefualt(struct MtdDevice *mtdDevice)
{
    struct MtdSpiConfig *cfg = NULL;
    struct SpiFlash *spi = NULL;

    if (mtdDevice == NULL) {
        return;
    }
    MTD_DEVICE_DUMP(mtdDevice);

    spi = CONTAINER_OF(mtdDevice, struct SpiFlash, mtd);
    HDF_LOGD("%s: cs = %u, addrCycle = %u", __func__, spi->cs, spi->addrCycle);

    cfg = &spi->readCfg;
    HDF_LOGD("%s: readCfg -> ifType:%u, cmd:0x%x, dummy:%u, size:%u, clock:%u",
        __func__, cfg->ifType, cfg->cmd, cfg->dummy, cfg->size, cfg->clock);
    cfg = &spi->writeCfg;
    HDF_LOGD("%s: writeCfg -> ifType:%u, cmd:0x%x, dummy:%u, size:%u, clock:%u",
        __func__, cfg->ifType, cfg->cmd, cfg->dummy, cfg->size, cfg->clock);
    cfg = &spi->eraseCfg;
    HDF_LOGD("%s: eraseCfg -> ifType:%u, cmd:0x%x, dummy:%u, size:%u, clock:%u",
        __func__, cfg->ifType, cfg->cmd, cfg->dummy, cfg->size, cfg->clock);
}

int32_t SpiFlashAdd(struct SpiFlash *spi)
{
    if (spi == NULL || spi->mtd.ops == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (spi->mtd.ops->dump == NULL) {
        spi->mtd.ops->dump = SpiFlashDumpDefualt;
    }
    return MtdDeviceAdd(&spi->mtd);
}

void SpiFlashDel(struct SpiFlash *spi)
{
    if (spi != NULL) {
        MtdDeviceDel(&spi->mtd);
    }
}

int32_t SpiFlashWaitReady(struct SpiFlash *spi)
{
    int32_t ret;

    if (spi == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (spi->spiOps.waitReady == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = (spi->spiOps.waitReady(spi));
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("%s: wait dev ready failed, ret=%d", __func__, ret);
    }
    return ret;
}

int32_t SpiFlashWriteEnable(struct SpiFlash *spi)
{
    int32_t ret;

    if (spi == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (spi->spiOps.writeEnable == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = (spi->spiOps.writeEnable(spi));
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("%s: dev write enable failed, ret=%d", __func__, ret);
    }
    return ret;
}

int32_t SpiFlashQeEnable(struct SpiFlash *spi)
{
    int32_t ret;

    if (spi == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (spi->spiOps.qeEnable == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = (spi->spiOps.qeEnable(spi));
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("%s: dev qe enable failed, ret=%d", __func__, ret);
    }
    return ret;
}

int32_t SpiFlashEntry4Addr(struct SpiFlash *spi, int enable)
{
    int32_t ret;

    if (spi == NULL) {
        return HDF_ERR_INVALID_OBJECT;
    }
    if (spi->spiOps.entry4Addr == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = (spi->spiOps.entry4Addr(spi, enable));
    if (ret != HDF_SUCCESS) {
        HDF_LOGD("%s: dev set 4addr failed, enabl=%d, ret=%d", __func__, enable, ret);
    }
    return ret;
}
