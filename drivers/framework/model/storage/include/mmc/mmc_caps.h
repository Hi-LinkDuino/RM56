/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MMC_CAPS_H
#define MMC_CAPS_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum MmcVolt { VOLT_3V3 = 0, VOLT_1V8, VOLT_1V2 };

enum MmcPowerMode { MMC_POWER_MODE_POWER_OFF = 0, MMC_POWER_MODE_POWER_UP, MMC_POWER_MODE_POWER_ON };

enum MmcBusWidth { BUS_WIDTH1 = 0, BUS_WIDTH4 = 2, BUS_WIDTH8 = 3 };

enum MmcBusTiming {
    BUS_TIMING_MMC_DS = 0,
    BUS_TIMING_MMC_HS,
    BUS_TIMING_SD_HS,
    BUS_TIMING_UHS_SDR12,
    BUS_TIMING_UHS_SDR25,
    BUS_TIMING_UHS_SDR50,
    BUS_TIMING_UHS_SDR104,
    BUS_TIMING_UHS_DDR50,
    BUS_TIMING_UHS_DDR52,
    BUS_TIMING_MMC_HS200, /* for emmc */
    BUS_TIMING_MMC_HS400, /* for emmc */
};

union MmcCaps {
    uint32_t capsData;
    struct CapsBitsData {
        uint32_t cap4Bit : 1;            /* bit:0 support 4 bit transfer */
        uint32_t cap8Bit : 1;            /* bit:1 support 8 bit transfer */
        uint32_t highSpeed : 1;          /* bit:2 support high-speed timing */
        uint32_t sdioIrq : 1;            /* bit:3 signal pending SDIO irqs */
        uint32_t onlySpi : 1;            /* bit:4 only support spi protocols */
        uint32_t needPoll : 1;           /* bit:5 need polling for card-detection */
        uint32_t nonremovable : 1;       /* bit:6 Nonremovable eg. eMMC */
        uint32_t waitWhileBusy : 1;      /* bit:7 waits while card is busy */
        uint32_t erase : 1;              /* bit:8 allow erase */
        uint32_t ddr1v8 : 1;             /* bit:9 support ddr mode at 1.8V */
        uint32_t ddr1v2 : 1;             /* bit:10 support ddr mode at 1.2V */
        uint32_t powerOffCard : 1;       /* bit:11 support power off after boot */
        uint32_t busWidthTest : 1;       /* bit:12 CMD14/CMD19 bus width ok */
        uint32_t uhsSdr12 : 1;           /* bit:13 support UHS SDR12 mode */
        uint32_t uhsSdr25 : 1;           /* bit:14 support UHS SDR25 mode */
        uint32_t uhsSdr50 : 1;           /* bit:15 support UHS SDR50 mode */
        uint32_t uhsSdr104 : 1;          /* bit:16 support UHS SDR104 mode */
        uint32_t uhsDdr50 : 1;           /* bit:17 support UHS DDR50 mode */
        uint32_t xpc330 : 1;             /* bit:18 support >150mA current at 3.3V */
        uint32_t xpc300 : 1;             /* bit:19 support >150mA current at 3.0V */
        uint32_t xpc180 : 1;             /* bit:20 support >150mA current at 1.8V */
        uint32_t driverTypeA : 1;        /* bit:21 support driver type A */
        uint32_t driverTypeC : 1;        /* bit:22 support driver type C */
        uint32_t driverTypeD : 1;        /* bit:23 support driver type D */
        uint32_t maxCurrentLimit200 : 1; /* bit:24 max current limit 200mA */
        uint32_t maxCurrentLimit400 : 1; /* bit:25 max current limit 400mA */
        uint32_t maxCurrentLimit600 : 1; /* bit:26 max current limit 600mA */
        uint32_t maxCurrentLimit800 : 1; /* bit:27 max current limit 800mA */
        uint32_t cmd23 : 1;              /* bit:28 support CMD23 */
        uint32_t hardwareReset : 1;      /* bit:29 support hardware reset */
        uint32_t sdSupportProtocol3 : 1; /* bit:30 SD support Protocol 3.0 */
        uint32_t cmdStop : 1;            /* bit:31 support cmd stop */
    } bits;
};

union MmcCaps2 {
    uint32_t caps2Data;
    struct Caps2BitsData {
        uint32_t bootPartNoAcc : 1;       /* bit:0 boot partition no access */
        uint32_t cacheCtrl : 1;           /* bit:1 allow cache control */
        uint32_t poweroffNotify : 1;      /* bit:2 support notify power off */
        uint32_t noMultiRead : 1;         /* bit:3 not support multiblock read */
        uint32_t noSleepCmd : 1;          /* bit:4 not support sleep command */
        uint32_t hs200Sdr1v8 : 1;         /* bit:5 support hs200 sdr 1.8V */
        uint32_t hs200Sdr1v2 : 1;         /* bit:6 support sdr hs200 1.2V */
        uint32_t brokenVoltage : 1;       /* bit:7 use broken voltage */
        uint32_t detectNoErr : 1;         /* bit:8 I/O err check card removal */
        uint32_t hcEraseSize : 1;         /* bit:9 High-capacity erase size */
        uint32_t hs400Support1v8 : 1;     /* bit:10 support hs400 1.8V */
        uint32_t hs400Support1v2 : 1;     /* bit:11 support hs400 1.2V */
        uint32_t hs400EnhancedStrobe : 1; /* bit:12 support hs400 enhanced strobe */
        uint32_t reserved : 18;           /* bits:13~31, reserved */
    } bits;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _MMC_CAPS_H */
