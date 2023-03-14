/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "osal_spinlock.h"
#include "platform_core.h"

static struct PlatformModuleInfo g_platformModules[] = {
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_GPIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_GPIO)
    {
        .moduleType = PLATFORM_MODULE_GPIO,
        .moduleName = "PLATFORM_MODULE_GPIO",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I2C) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I2C)
    {
        .moduleType = PLATFORM_MODULE_I2C,
        .moduleName = "PLATFORM_MODULE_I2C",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SPI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SPI)
    {
        .moduleType = PLATFORM_MODULE_SPI,
        .moduleName = "PLATFORM_MODULE_SPI",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PIN) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PIN)
    {
        .moduleType = PLATFORM_MODULE_PIN,
        .moduleName = "PLATFORM_MODULE_PIN",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_CLOCK) || defined(CONFIG_DRIVERS_HDF_PLATFORM_CLOCK)
    {
        .moduleType = PLATFORM_MODULE_CLOCK,
        .moduleName = "PLATFORM_MODULE_CLOCK",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_REGULATOR) || defined(CONFIG_DRIVERS_HDF_PLATFORM_REGULATOR)
    {
        .moduleType = PLATFORM_MODULE_REGULATOR,
        .moduleName = "PLATFORM_MODULE_REGULATOR",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_DSI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_DSI)
    {
        .moduleType = PLATFORM_MODULE_MIPI_DSI,
        .moduleName = "PLATFORM_MODULE_MIPI_DSI",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MIPI_CSI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MIPI_CSI)
    {
        .moduleType = PLATFORM_MODULE_MIPI_CSI,
        .moduleName = "PLATFORM_MODULE_MIPI_CSI",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_UART) || defined(CONFIG_DRIVERS_HDF_PLATFORM_UART)
    {
        .moduleType = PLATFORM_MODULE_UART,
        .moduleName = "PLATFORM_MODULE_UART",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_SDIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_SDIO)
    {
        .moduleType = PLATFORM_MODULE_SDIO,
        .moduleName = "PLATFORM_MODULE_SDIO",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MDIO) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MDIO)
    {
        .moduleType = PLATFORM_MODULE_MDIO,
        .moduleName = "PLATFORM_MODULE_MDIO",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_APB) || defined(CONFIG_DRIVERS_HDF_PLATFORM_APB)
    {
        .moduleType = PLATFORM_MODULE_APB,
        .moduleName = "PLATFORM_MODULE_APB",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PCIE) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PCIE)
    {
        .moduleType = PLATFORM_MODULE_PCIE,
        .moduleName = "PLATFORM_MODULE_PCIE",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PCM) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PCM)
    {
        .moduleType = PLATFORM_MODULE_PCM,
        .moduleName = "PLATFORM_MODULE_PCM",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I2S) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I2S)
    {
        .moduleType = PLATFORM_MODULE_I2S,
        .moduleName = "PLATFORM_MODULE_I2S",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_PWM) || defined(CONFIG_DRIVERS_HDF_PLATFORM_PWM)
    {
        .moduleType = PLATFORM_MODULE_PWM,
        .moduleName = "PLATFORM_MODULE_PWM",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_DMA) || defined(CONFIG_DRIVERS_HDF_PLATFORM_DMA)
    {
        .moduleType = PLATFORM_MODULE_DMA,
        .moduleName = "PLATFORM_MODULE_DMA",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_ADC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_ADC)
    {
        .moduleType = PLATFORM_MODULE_ADC,
        .moduleName = "PLATFORM_MODULE_ADC",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_RTC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_RTC)
    {
        .moduleType = PLATFORM_MODULE_RTC,
        .moduleName = "PLATFORM_MODULE_RTC",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_WDT) || defined(CONFIG_DRIVERS_HDF_PLATFORM_WDT)
    {
        .moduleType = PLATFORM_MODULE_WDT,
        .moduleName = "PLATFORM_MODULE_WDT",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_I3C) || defined(CONFIG_DRIVERS_HDF_PLATFORM_I3C)
    {
        .moduleType = PLATFORM_MODULE_I3C,
        .moduleName = "PLATFORM_MODULE_I3C",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_CAN) || defined(CONFIG_DRIVERS_HDF_PLATFORM_CAN)
    {
        .moduleType = PLATFORM_MODULE_CAN,
        .moduleName = "PLATFORM_MODULE_CAN",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_HDMI) || defined(CONFIG_DRIVERS_HDF_PLATFORM_HDMI)
    {
        .moduleType = PLATFORM_MODULE_HDMI,
        .moduleName = "PLATFORM_MODULE_HDMI",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MMC) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MMC)
    {
        .moduleType = PLATFORM_MODULE_MMC,
        .moduleName = "PLATFORM_MODULE_MMC",
    },
#endif
#if defined(LOSCFG_DRIVERS_HDF_PLATFORM_MTD) || defined(CONFIG_DRIVERS_HDF_PLATFORM_MTD)
    {
        .moduleType = PLATFORM_MODULE_MTD,
        .moduleName = "PLATFORM_MODULE_MTD",
    },
#endif
    {
        .moduleType = PLATFORM_MODULE_DEFAULT,
        .moduleName = "PLATFORM_MODULE_DEFAULT",
    },
};

static OsalSpinlock g_platformSpin;
static bool g_platformReady;

void PlatformGlobalLock(void)
{
    if (g_platformReady == false) {
        (void)OsalSpinInit(&g_platformSpin);
        g_platformReady = true;
    }
    (void)OsalSpinLock(&g_platformSpin);
}

void PlatformGlobalUnlock(void)
{
    (void)OsalSpinUnlock(&g_platformSpin);
}

struct PlatformModuleInfo *PlatformModuleInfoGet(enum PlatformModuleType moduleType)
{
    int32_t i;

    for (i = 0; i < PlatformModuleInfoCount(); i++) {
        if (g_platformModules[i].moduleType == moduleType) {
            return &g_platformModules[i];
        }
    }
    return NULL;
}

int32_t PlatformModuleInfoCount(void)
{
    return sizeof(g_platformModules) / sizeof(g_platformModules[0]);
}
