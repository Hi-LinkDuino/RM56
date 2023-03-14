/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI35XX_DISP_H
#define HI35XX_DISP_H
#include "hdf_base.h"

#define IO_CFG1_BASE                0x111F0000
#define IO_CFG2_BASE                0x112F0000
#define IO_CFG_SIZE                 0x10000

#define PWM_DEV0                    0
#define PWM_DEV1                    1

/* output interface type */
#define INTF_LCD_6BIT               (0x01L << 9)
#define INTF_LCD_8BIT               (0x01L << 10)
#define INTF_LCD_16BIT              (0x01L << 11)
#define INTF_LCD_18BIT              (0x01L << 12)
#define INTF_LCD_24BIT              (0x01L << 13)
#define INTF_MIPI                   (0x01L << 14)

/* output timing */
enum IntfSync {
    OUTPUT_USER = 0,          /* User timing */
    OUTPUT_PAL,               /* PAL standard */
    OUTPUT_NTSC,              /* NTSC standard */
    OUTPUT_1080P24,           /* 1920 x 1080 at 24 Hz. */
    OUTPUT_1080P25,           /* 1920 x 1080 at 25 Hz. */
    OUTPUT_1080P30,           /* 1920 x 1080 at 30 Hz. */
    OUTPUT_720P50,            /* 1280 x  720 at 50 Hz. */
    OUTPUT_720P60,            /* 1280 x  720 at 60 Hz. */
    OUTPUT_1080I50,           /* 1920 x 1080 at 50 Hz, interlace. */
    OUTPUT_1080I60,           /* 1920 x 1080 at 60 Hz, interlace. */
    OUTPUT_1080P50,           /* 1920 x 1080 at 50 Hz. */
    OUTPUT_1080P60,           /* 1920 x 1080 at 60 Hz. */
    OUTPUT_576P50,            /* 720  x  576 at 50 Hz. */
    OUTPUT_480P60,            /* 720  x  480 at 60 Hz. */
    OUTPUT_800X600_60,        /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    OUTPUT_1024X768_60,       /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    OUTPUT_1280X1024_60,      /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    OUTPUT_1366X768_60,       /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    OUTPUT_1440X900_60,       /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    OUTPUT_1280X800_60,       /* 1280*800@60Hz VGA@60Hz */
    OUTPUT_1600X1200_60,      /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    OUTPUT_1680X1050_60,      /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    OUTPUT_1920X1200_60,      /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking) */
    OUTPUT_640X480_60,        /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    OUTPUT_960H_PAL,          /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced) */
    OUTPUT_960H_NTSC,         /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced) */
    OUTPUT_1920X2160_30,      /* 1920x2160_30 */
    OUTPUT_2560X1440_30,      /* 2560x1440_30 */
    OUTPUT_2560X1440_60,      /* 2560x1440_60 */
    OUTPUT_2560X1600_60,      /* 2560x1600_60 */
    OUTPUT_3840X2160_24,      /* 3840x2160_24 */
    OUTPUT_3840X2160_25,      /* 3840x2160_25 */
    OUTPUT_3840X2160_30,      /* 3840x2160_30 */
    OUTPUT_3840X2160_50,      /* 3840x2160_50 */
    OUTPUT_3840X2160_60,      /* 3840x2160_60 */
    OUTPUT_4096X2160_24,      /* 4096x2160_24 */
    OUTPUT_4096X2160_25,      /* 4096x2160_25 */
    OUTPUT_4096X2160_30,      /* 4096x2160_30 */
    OUTPUT_4096X2160_50,      /* 4096x2160_50 */
    OUTPUT_4096X2160_60,      /* 4096x2160_60 */
    OUTPUT_320X240_60,        /* For ota5182 at 60 Hz (8bit) */
    OUTPUT_320X240_50,        /* For ili9342 at 50 Hz (6bit) */
    OUTPUT_240X320_50,        /* Hi3559AV100: For ili9341 at 50 Hz (6bit) */
    OUTPUT_240X320_60,        /* For ili9341 at 60 Hz (16bit) */
    OUTPUT_800X600_50,        /* For LCD     at 50 Hz (24bit) */
    OUTPUT_720X1280_60,       /* For MIPI DSI Tx 720 x1280 at 60 Hz */
    OUTPUT_1080X1920_60,      /* For MIPI DSI Tx 1080x1920 at 60 Hz */
    OUTPUT_7680X4320_30,      /* For HDMI2.1 at 30 Hz */
};

#endif /* HI35XX_DISP_H */
