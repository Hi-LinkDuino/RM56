/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_aiao_impl.h"
#include "audio_host.h"
#include "audio_control.h"
#include "audio_dai_if.h"
#include "audio_dai_base.h"
#include "audio_driver_log.h"
#include "osal_io.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

/* Hi35xx IO register address */
#define HI35XX_I2C_REG_BASE_ADDR  (0x114F0000)
#define HI35XX_I2S_REG_BASE_ADDR  (0x112F0000)
#define CODEC_REG_BASE            (0x113C0000)
#define CODEC_MAX_REG_SIZE        (0x1000)

#define I2S_IOCFG2_BASE1 0x0020
#define I2S_IOCFG2_BASE2 0x0024
#define I2S_IOCFG2_BASE3 0x0028
#define I2S_IOCFG2_BASE4 0x002C
#define I2S_IOCFG2_BASE5 0x0030

#define I2S_IOCFG2_BASE1_VAL 0x663
#define I2S_IOCFG2_BASE2_VAL 0x673
#define I2S_IOCFG2_BASE3_VAL 0x573
#define I2S_IOCFG2_BASE4_VAL 0x473
#define I2S_IOCFG2_BASE5_VAL 0x433

void *g_regCodecBase = NULL;
void *g_regDaiBase = NULL;

/* i2c6 init */
static int I2c6PinInit(void)
{
    char *regI2cBase = (void *)OsalIoRemap(HI35XX_I2C_REG_BASE_ADDR, 0x1000);
    if (regI2cBase == NULL) {
        AUDIO_DRIVER_LOG_ERR("regI2cBase is null.");
        return HDF_FAILURE;
    }
    SysWritel((uintptr_t)regI2cBase + 0x0048, 0x0473); // I2C6_SCL
    SysWritel((uintptr_t)regI2cBase + 0x004C, 0x0473); // I2C6_SDA
    if (regI2cBase != NULL) {
        OsalIoUnmap(regI2cBase);
    }
    AUDIO_DRIVER_LOG_DEBUG("success!");
    return HDF_SUCCESS;
}

/* i2s0 pin init */
static void I2s0PinMux(const char *regI2sBase)
{
    SysWritel((uintptr_t)regI2sBase + I2S_IOCFG2_BASE1, I2S_IOCFG2_BASE1_VAL);
    SysWritel((uintptr_t)regI2sBase + I2S_IOCFG2_BASE2, I2S_IOCFG2_BASE2_VAL);
    SysWritel((uintptr_t)regI2sBase + I2S_IOCFG2_BASE3, I2S_IOCFG2_BASE3_VAL);
    SysWritel((uintptr_t)regI2sBase + I2S_IOCFG2_BASE4, I2S_IOCFG2_BASE4_VAL);
    SysWritel((uintptr_t)regI2sBase + I2S_IOCFG2_BASE5, I2S_IOCFG2_BASE5_VAL);
}

/* i2s init */
static int I2sPinInit(void)
{
    char *regI2sBase = (void *)OsalIoRemap(HI35XX_I2S_REG_BASE_ADDR, 0x1000);
    if (regI2sBase == NULL) {
        AUDIO_DRIVER_LOG_ERR("regI2sBase is null.");
        return HDF_FAILURE;
    }
    I2s0PinMux(regI2sBase);
    if (regI2sBase != NULL) {
        OsalIoUnmap(regI2sBase);
    }
    AUDIO_DRIVER_LOG_DEBUG("success!");
    return HDF_SUCCESS;
}

int32_t DaiDeviceInit(struct AudioCard *audioCard, const struct DaiDevice *dai)
{
    if (dai == NULL || dai->devData == NULL) {
        AUDIO_DRIVER_LOG_ERR("dai is nullptr.");
        return HDF_FAILURE;
    }
    struct DaiData *data = dai->devData;
    struct AudioRegCfgData *regConfig = dai->devData->regConfig;
    if (regConfig == NULL) {
        AUDIO_DRIVER_LOG_ERR("regConfig is nullptr.");
        return HDF_FAILURE;
    }

    if (g_regCodecBase == NULL) {
        g_regCodecBase = OsalIoRemap(CODEC_REG_BASE, CODEC_MAX_REG_SIZE);
        if (g_regCodecBase == NULL) {
            AUDIO_DRIVER_LOG_ERR("OsalIoRemap fail.");
            return HDF_FAILURE;
        }
    }

    if (g_regDaiBase == NULL) {
        g_regDaiBase = OsalIoRemap(regConfig->audioIdInfo.chipIdRegister,
            regConfig->audioIdInfo.chipIdSize);
        if (g_regDaiBase == NULL) {
            AUDIO_DRIVER_LOG_ERR("OsalIoRemap fail.");
            return HDF_FAILURE;
        }
    }

    data->regVirtualAddr = (uintptr_t)g_regCodecBase;

    if (DaiSetConfigInfo(data) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set config info fail.");
        return HDF_FAILURE;
    }

    int ret = AudioAddControls(audioCard, data->controls, data->numControls);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add controls failed.");
        return HDF_FAILURE;
    }

    if (data->daiInitFlag == true) {
        AUDIO_DRIVER_LOG_DEBUG("dai init complete!");
        return HDF_SUCCESS;
    }

    if (I2c6PinInit() != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("I2c6PinInit fail.");
        return HDF_FAILURE;
    }

    data->daiInitFlag = true;

    return HDF_SUCCESS;
}

int32_t DaiTrigger(const struct AudioCard *card, int cmd, const struct DaiDevice *device)
{
    (void)card;
    (void)device;
    (void)cmd;

    return HDF_SUCCESS;
}

int32_t DaiStartup(const struct AudioCard *card, const struct DaiDevice *device)
{
    struct AudioMixerControl *regCfgItem = NULL;
    (void)card;

    if (device == NULL || device->devData == NULL || device->devData->regConfig == NULL ||
        device->devData->regConfig->audioRegParams[AUDIO_DAI_STARTUP_PATAM_GROUP] == NULL ||
        device->devData->regConfig->audioRegParams[AUDIO_DAI_STARTUP_PATAM_GROUP]->regCfgItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }
    regCfgItem = device->devData->regConfig->audioRegParams[AUDIO_DAI_STARTUP_PATAM_GROUP]->regCfgItem;
    int itemNum = device->devData->regConfig->audioRegParams[AUDIO_DAI_STARTUP_PATAM_GROUP]->itemNum;

    device->devData->regVirtualAddr = (uintptr_t)g_regDaiBase;
    for (int i = 0; i < itemNum; i++) {
        int ret = AudioUpdateDaiRegBits(device, regCfgItem[i].reg, regCfgItem[i].mask,
            regCfgItem[i].shift, regCfgItem[i].value);
        if (ret != HDF_SUCCESS) {
            AUDIO_DRIVER_LOG_ERR("set frequency fail.");
            return HDF_FAILURE;
        }
    }
    device->devData->regVirtualAddr = (uintptr_t)g_regCodecBase;

    if (I2sPinInit() != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("I2sPinInit fail.");
    }

    return HDF_SUCCESS;
}

static int32_t SetIISRate(const struct DaiDevice *device, const struct AudioMixerControl *regCfgItem)
{
    const uint32_t shiftMax = 4;
    uint32_t mclkSel;
    uint32_t shift = 0;
    uint32_t bclkRegVal;

    if (device == NULL || device->devData == NULL || regCfgItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }
    uint32_t rate = device->devData->pcmInfo.rate;
    uint32_t bitWidth = device->devData->pcmInfo.bitWidth;

    if (device->devData->pcmInfo.streamType == AUDIO_CAPTURE_STREAM) {
        shift = shiftMax;
    }

    (void)memset_s(&mclkSel, sizeof(uint32_t), 0, sizeof(uint32_t));
    if (AiaoGetMclk(rate, &mclkSel) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioUpdateDaiRegBits(device, regCfgItem[0 + shift].reg, regCfgItem[0 + shift].mask,
        regCfgItem[0 + shift].shift, mclkSel) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set frequency fail.");
        return HDF_FAILURE;
    }

    (void)memset_s(&bclkRegVal, sizeof(uint32_t), 0, sizeof(uint32_t));
    if (AiaoSetSysCtlRegValue(mclkSel, bitWidth, rate, &bclkRegVal) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (AudioUpdateDaiRegBits(device, regCfgItem[1 + shift].reg, regCfgItem[1 + shift].mask,
        regCfgItem[1 + shift].shift, bclkRegVal) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set frequency fail.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DaiParamsUpdate(const struct DaiDevice *device)
{
    uint32_t value;
    struct AudioMixerControl *regCfgItem = NULL;
    const uint32_t shiftMax = 4;
    uint32_t shift = 0;
    const uint32_t index2 = 2;
    const uint32_t index3 = 3;

    if (device == NULL || device->devData == NULL || device->devData->regConfig == NULL ||
        device->devData->regConfig->audioRegParams[AUDIO_DAI_PATAM_GROUP] == NULL ||
        device->devData->regConfig->audioRegParams[AUDIO_DAI_PATAM_GROUP]->regCfgItem == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }
    regCfgItem = device->devData->regConfig->audioRegParams[AUDIO_DAI_PATAM_GROUP]->regCfgItem;

    if (device->devData->pcmInfo.streamType == AUDIO_CAPTURE_STREAM) {
        shift = shiftMax;
    }

    if (SetIISRate(device, regCfgItem) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set Rate fail.");
        return HDF_FAILURE;
    }

    uint32_t bitWidth = device->devData->pcmInfo.bitWidth;

    if (bitWidth == BIT_WIDTH16) {
        value = 0x1;
    } else if (bitWidth == BIT_WIDTH24) {
        value = 0x2; /* 2: 24bit */
    } else {
        AUDIO_DEVICE_LOG_ERR(" invalued bitWidth: %d.", bitWidth);
        return HDF_FAILURE;
    }

    if (AudioUpdateDaiRegBits(device, regCfgItem[index2 + shift].reg, regCfgItem[index2 + shift].mask,
        regCfgItem[index2 + shift].shift, value) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set bitWidth fail.");
        return HDF_FAILURE;
    }

    value = device->devData->pcmInfo.channels - 1;
    if (AudioUpdateDaiRegBits(device, regCfgItem[index3 + shift].reg, regCfgItem[index3 + shift].mask,
        regCfgItem[index3 + shift].shift, value) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set channels fail.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t DaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param)
{
    uint32_t bitWidth;

    if (card == NULL || card->rtd == NULL || card->rtd->cpuDai == NULL ||
        param == NULL || param->cardServiceName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }
    struct DaiDevice *device = card->rtd->cpuDai;

    if (DaiCheckSampleRate(param->rate) != HDF_SUCCESS) {
        return HDF_ERR_NOT_SUPPORT;
    }

    struct DaiData *data = DaiDataFromCard(card);
    if (data == NULL) {
        AUDIO_DRIVER_LOG_ERR("platformHost is nullptr.");
        return HDF_FAILURE;
    }

    data->pcmInfo.channels = param->channels;

    if (AudioFormatToBitWidth(param->format, &bitWidth) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    data->pcmInfo.bitWidth = bitWidth;
    data->pcmInfo.rate = param->rate;
    data->pcmInfo.streamType = param->streamType;
    data->regVirtualAddr = (uintptr_t)g_regDaiBase;

    if (DaiParamsUpdate(device) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("DaiParamsUpdate:  fail.");
        return HDF_FAILURE;
    }
    data->regVirtualAddr = (uintptr_t)g_regCodecBase;
    return HDF_SUCCESS;
}
