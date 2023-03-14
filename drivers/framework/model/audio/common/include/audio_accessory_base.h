/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef AUDIO_ACCESSORY_BASE_H
#define AUDIO_ACCESSORY_BASE_H
#include "audio_control.h"
#include "audio_core.h"
#include "audio_host.h"
#include "audio_parse.h"
#include "audio_platform_base.h"
#include "audio_sapm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum I2sFrequency {
    I2S_SAMPLE_FREQUENCY_8000  = 8000,    /* 8kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_11025 = 11025,   /* 11.025kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_12000 = 12000,   /* 12kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_16000 = 16000,   /* 16kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_22050 = 22050,   /* 22.050kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_24000 = 24000,   /* 24kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_32000 = 32000,   /* 32kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_44100 = 44100,   /* 44.1kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_48000 = 48000,   /* 48kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_64000 = 64000,   /* 64kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_88200 = 88200,   /* 88.2kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_96000 = 96000    /* 96kHz sample_rate */
};

enum I2sFrequencyRegVal {
    I2S_SAMPLE_FREQUENCY_REG_VAL_8000  = 0x0,   /* 8kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_11025 = 0x1,   /* 11.025kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_12000 = 0x2,   /* 12kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_16000 = 0x3,   /* 16kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_22050 = 0x4,   /* 22.050kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_24000 = 0x5,   /* 24kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_32000 = 0x6,   /* 32kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_44100 = 0x7,   /* 44.1kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_48000 = 0x8,   /* 48kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_64000 = 0x9,   /* 64kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_88200 = 0xA,   /* 88.2kHz sample_rate */
    I2S_SAMPLE_FREQUENCY_REG_VAL_96000 = 0xB    /* 96kHz sample_rate */
};

struct AccessoryTransferData {
    uint16_t i2cDevAddr;
    uint16_t i2cBusNumber;
    uint32_t accessoryCfgCtrlCount;
    struct AudioRegCfgGroupNode **accessoryRegCfgGroupNode;
    struct AudioKcontrol *accessoryControls;
};

struct DaiParamsVal {
    uint32_t frequencyVal;
    uint32_t formatVal;
    uint32_t channelVal;
};

int32_t AccessoryI2cReadWrite(struct AudioAddrConfig *regAttr, uint16_t rwFlag);
int32_t AccessoryRegBitsRead(struct AudioMixerControl *regAttr, uint32_t *regValue);
int32_t AccessoryRegBitsUpdate(struct AudioMixerControl regAttr);

int32_t AccessoryDeviceFrequencyParse(uint32_t rate, uint16_t *freq);
int32_t AccessoryDaiParamsUpdate(struct DaiParamsVal daiParamsVal);
int32_t AccessoryDeviceCfgGet(struct AccessoryData *data, struct AccessoryTransferData *transferData);
int32_t AccessoryDeviceCtrlRegInit(void);
int32_t AccessoryDeviceRegRead(const struct AccessoryDevice *codec, uint32_t reg, uint32_t *value);
int32_t AccessoryDeviceRegWrite(const struct AccessoryDevice *codec, uint32_t reg, uint32_t value);
int32_t AccessoryGetConfigInfo(const struct HdfDeviceObject *device, struct AccessoryData *accessoryData);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
