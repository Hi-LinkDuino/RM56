/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISP_COMMON_H
#define DISP_COMMON_H
#include <stdint.h>
#include "hdf_log.h"

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif
#define HDF_LOG_TAG HDF_DISP

#define CHECK_NULLPOINTER_RETURN_VALUE(pointer, ret) do { \
    if ((pointer) == NULL) { \
        HDF_LOGE("%s: pointer is null", __func__); \
        return (ret); \
    } \
} while (0)

#define CHECK_NULLPOINTER_RETURN(pointer) do { \
    if ((pointer) == NULL) { \
        HDF_LOGE("%s: pointer is null", __func__); \
        return; \
    } \
} while (0)

#define DISPLAY_CHK_RETURN(val, ret, ...) \
    do { \
        if (val) { \
            __VA_ARGS__; \
            return (ret); \
        } \
    } while (0)

#define DISPLAY_CHK_RETURN_NOT_VALUE(val, ...) \
    do { \
        if (val) { \
            __VA_ARGS__; \
            return; \
        } \
    } while (0)

enum LayerId {
    GRA_LAYER_0,
    GRA_LAYER_1,
    GRA_LAYER_2,
    GRA_LAYER_MAX,
    OV_LAYER_0,
    OV_LAYER_1,
    OV_LAYER_2,
    OV_LAYER_MAX
};

enum PowerMode {
    DISP_ON,
    DISP_OFF,
};

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

struct DispInfo {
    uint32_t width;
    uint32_t hbp;
    uint32_t hfp;
    uint32_t hsw;
    uint32_t height;
    uint32_t vbp;
    uint32_t vfp;
    uint32_t vsw;
    uint32_t frameRate;
    uint32_t intfType;
    enum IntfSync intfSync;
    uint32_t minLevel;
    uint32_t maxLevel;
    uint32_t defLevel;
};
#endif // DISP_COMMON_H
