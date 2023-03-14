/*
 * Copyright (c) 2021-2022 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 *
 * This file is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef __SPI_BES_H__
#define __SPI_BES_H__

#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#ifdef CHIP_BEST2003
#include "hal_iomux_best2003.h"
#endif
#include "osal_mutex.h"
#include "osal_sem.h"

#ifdef __cplusplus
extern "C" {
#endif

enum SPI_WORK_MODE {
    SPI_WORK_MODE_0, // CPOL = 0; CPHA = 0
    SPI_WORK_MODE_2, // CPOL = 1; CPHA = 0
    SPI_WORK_MODE_1, // CPOL = 0; CPHA = 1
    SPI_WORK_MODE_3, // CPOL = 1; CPHA = 1
};

enum SPI_TRANSFER_MODE {
    SPI_TRANSFER_DMA,
    SPI_TRANSFER_NORMAL,
};

struct SpiResource {
    uint32_t num;
    uint32_t speed;
    enum SPI_TRANSFER_MODE transmode;
    enum SPI_WORK_MODE mode;
    uint32_t dataSize;
    uint32_t csNum;
    uint32_t spiCsSoft;
    uint32_t spiClkPin;
    uint32_t spiMosiPin;
    uint32_t spiMisoPin;
    uint32_t spiCsPin;
};

struct SPI_CTX_OBJ_T {
    enum HAL_IOMUX_PIN_T spiPinCS0;
    enum HAL_IOMUX_FUNCTION_T spiFunDI0;
    enum HAL_IOMUX_FUNCTION_T spiFunCLK;
    enum HAL_IOMUX_FUNCTION_T spiFunCS0;
    enum HAL_IOMUX_FUNCTION_T spiFunDIO;
    struct OsalSem sem;
    struct OsalMutex mutex;
    int (*SpiOpen)(const struct HAL_SPI_CFG_T *cfg);
    int (*SpiDmaSend)(const void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);
    int (*SpiDmaRecv)(const void *cmd, void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);
    int (*SpiSend)(const void *data, uint32_t len);
    int (*SpiRecv)(const void *cmd, void *data, uint32_t len);
    void (*SpiDmaIrq)(int error);
    int (*SpiClose)(uint32_t cs);
};

struct SpiDevice {
    uint32_t spiId;
    struct SpiResource resource;
    struct HAL_SPI_CFG_T spiDevCfg;
};

#ifdef __cplusplus
}
#endif

#endif
