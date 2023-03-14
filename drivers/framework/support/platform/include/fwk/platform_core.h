/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef PLATFORM_CORE_H
#define PLATFORM_CORE_H

#include "hdf_base.h"

#include "platform_log.h"
#include "platform_errno.h"
#include "platform_event.h"
#include "platform_queue.h"
#ifndef __USER__
#include "platform_device.h"
#include "platform_manager.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum PlatformModuleType {
    PLATFORM_MODULE_GPIO,
    PLATFORM_MODULE_I2C,
    PLATFORM_MODULE_SPI,
    PLATFORM_MODULE_PIN,
    PLATFORM_MODULE_CLOCK,
    PLATFORM_MODULE_REGULATOR,
    PLATFORM_MODULE_MIPI_DSI,
    PLATFORM_MODULE_MIPI_CSI,
    PLATFORM_MODULE_UART,
    PLATFORM_MODULE_SDIO,
    PLATFORM_MODULE_MDIO,
    PLATFORM_MODULE_APB,
    PLATFORM_MODULE_PCIE,
    PLATFORM_MODULE_PCM,
    PLATFORM_MODULE_I2S,
    PLATFORM_MODULE_PWM,
    PLATFORM_MODULE_DMA,
    PLATFORM_MODULE_ADC,
    PLATFORM_MODULE_RTC,
    PLATFORM_MODULE_WDT,
    PLATFORM_MODULE_I3C,
    PLATFORM_MODULE_CAN,
    PLATFORM_MODULE_HDMI,
    PLATFORM_MODULE_MMC,
    PLATFORM_MODULE_MTD,
    PLATFORM_MODULE_DEFAULT,
    PLATFORM_MODULE_MAX,
};

struct PlatformModuleInfo {
    enum PlatformModuleType moduleType;
    const char *moduleName;
    void *priv;
};

struct PlatformModuleInfo *PlatformModuleInfoGet(enum PlatformModuleType moduleType);
int32_t PlatformModuleInfoCount(void);

void PlatformGlobalLock(void);
void PlatformGlobalUnlock(void);

/* Os adapt */
bool PlatInIrqContext(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PLATFORM_CORE_H */
