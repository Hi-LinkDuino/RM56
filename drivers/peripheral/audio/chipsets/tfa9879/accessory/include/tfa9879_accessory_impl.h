/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef TFA9879_ACCESSORY_IMPL_H
#define TFA9879_ACCESSORY_IMPL_H

#include "audio_accessory_if.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "osal_io.h"
#include "securec.h"
#include <linux/types.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum Tfa9879I2sFormatRegVal {
    I2S_SAMPLE_FORMAT_REG_VAL_MSB_24    = 0x2,    /*  MSB-justified data up to 24 bits */
    I2S_SAMPLE_FORMAT_REG_VAL_24        = 0x3,    /*  I2S data up to 24 bits */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_16    = 0x4,    /*  LSB-justified 16-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_18    = 0x5,    /*  LSB-justified 18-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_20    = 0x6,    /*  LSB-justified 20-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_24    = 0x7,    /*  LSB-justified 24-bit data */
};

int32_t Tfa9879DeviceInit(struct AudioCard *audioCard, const struct AccessoryDevice *device);
int32_t Tfa9879DaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device);
int32_t Tfa9879DaiStartup(const struct AudioCard *card, const struct DaiDevice *device);
int32_t Tfa9879DaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
