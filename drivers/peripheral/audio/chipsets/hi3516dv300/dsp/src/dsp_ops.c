/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "dsp_ops.h"
#include "spi_if.h"
#include "audio_dsp_if.h"
#include "audio_driver_log.h"
#include "audio_accessory_base.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

#define DEFAULT_SPEED 2000000
#define BITS_PER_WORD_EIGHT 8
#define DSP_CS_NUM 1
#define DSP_SPI_BUS_NUM 1

enum DspI2sFormatRegVal {
    I2S_SAMPLE_FORMAT_REG_VAL_MSB_24    = 0x2,    /*  MSB-justified data up to 24 bits */
    I2S_SAMPLE_FORMAT_REG_VAL_24        = 0x3,    /*  I2S data up to 24 bits */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_16    = 0x4,    /*  LSB-justified 16-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_18    = 0x5,    /*  LSB-justified 18-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_20    = 0x6,    /*  LSB-justified 20-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_24    = 0x7,    /*  LSB-justified 24-bit data */
};

struct SpiDevInfo g_devInfo = {
    .busNum = DSP_SPI_BUS_NUM,
    .csNum = DSP_CS_NUM,
};

int32_t DspDaiStartup(const struct AudioCard *card, const struct DaiDevice *device)
{
    (void)card;
    (void)device;
    return HDF_SUCCESS;
}

static int32_t DspCfgI2sFrequency(uint32_t rate, uint16_t *frequency)
{
    if (frequency == NULL) {
        AUDIO_DRIVER_LOG_ERR("input param is nullptr.");
        return HDF_ERR_INVALID_PARAM;
    }

    switch (rate) {
        case I2S_SAMPLE_FREQUENCY_8000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_8000;
            break;
        case I2S_SAMPLE_FREQUENCY_11025:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_11025;
            break;
        case I2S_SAMPLE_FREQUENCY_12000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_12000;
            break;
        case I2S_SAMPLE_FREQUENCY_16000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_16000;
            break;
        case I2S_SAMPLE_FREQUENCY_22050:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_22050;
            break;
        case I2S_SAMPLE_FREQUENCY_24000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_24000;
            break;
        case I2S_SAMPLE_FREQUENCY_32000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_32000;
            break;
        case I2S_SAMPLE_FREQUENCY_44100:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_44100;
            break;
        case I2S_SAMPLE_FREQUENCY_48000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_48000;
            break;
        case I2S_SAMPLE_FREQUENCY_64000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_64000;
            break;
        case I2S_SAMPLE_FREQUENCY_88200:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_88200;
            break;
        case I2S_SAMPLE_FREQUENCY_96000:
            *frequency = I2S_SAMPLE_FREQUENCY_REG_VAL_96000;
            break;
        default:
            AUDIO_DRIVER_LOG_ERR("rate: %d is not support.", rate);
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int32_t DspSetI2sBitWidth(enum AudioFormat format, uint16_t *bitWidth)
{
    if (bitWidth == NULL) {
        AUDIO_DRIVER_LOG_ERR("input param is nullptr.");
        return HDF_ERR_INVALID_PARAM;
    }

    switch (format) {
        case AUDIO_FORMAT_PCM_8_BIT:
            *bitWidth = I2S_SAMPLE_FORMAT_REG_VAL_24;
            break;
        case AUDIO_FORMAT_PCM_16_BIT:
            *bitWidth = I2S_SAMPLE_FORMAT_REG_VAL_24;
            break;
        case AUDIO_FORMAT_PCM_24_BIT:
            *bitWidth = I2S_SAMPLE_FORMAT_REG_VAL_24;
            break;
        default:
            AUDIO_DRIVER_LOG_ERR("format: %d is not support.", format);
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

static int DspSetI2sFrequency(uint16_t frequencyVal)
{
    return HDF_SUCCESS;
}

static int DspSetI2sFormat(uint16_t formatVal)
{
    return HDF_SUCCESS;
}

int32_t DspDaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param)
{
    int ret;
    uint16_t frequency, bitWidth;
    (void)card;

    AUDIO_DRIVER_LOG_DEBUG("entry.");
    if (param == NULL ||  param->cardServiceName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input param is nullptr.");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = DspCfgI2sFrequency(param->rate, &frequency);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("RateToFrequency fail.");
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = DspSetI2sBitWidth(param->format, &bitWidth);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("FormatToBitWidth fail.");
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = DspSetI2sFrequency(frequency);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("SetDspI2sFs fail.");
        return HDF_FAILURE;
    }
    ret = DspSetI2sFormat(bitWidth);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("SetDspI2sFormat fail.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("DspDaiHwParams: channels = %d, rate = %d, periodSize = %d, \
        periodCount = %d, format = %d, cardServiceName = %s \n",
        param->channels, param->rate, param->periodSize,
        param->periodCount, (uint32_t)param->format, param->cardServiceName);
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int DspPowerEnable(void)
{
    return HDF_SUCCESS;
}

static int DspGpioPinInit(void)
{
    return HDF_SUCCESS;
}

static int DspI2cPinInit(void)
{
    return HDF_SUCCESS;
}

static int DspI2sInit(void)
{
    return HDF_SUCCESS;
}

static int DspI2cInit(void)
{
    return HDF_SUCCESS;
}

/* not init dsp gpio */
static int DspSpiPinInit(void)
{
    return HDF_SUCCESS;
}

DevHandle DspSpiOpen(const struct SpiDevInfo *info)
{
    if (info == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspSpiOpen fail");
        return NULL;
    }
    AUDIO_DRIVER_LOG_INFO("DspSpiOpen success");
    return OsalMemCalloc(1);
}

void DspSpiClose(DevHandle handle)
{
    if (handle == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspSpiClose fail");
        return;
    }
    OsalMemFree(handle);
    AUDIO_DRIVER_LOG_DEBUG("DspSpiClose success");
}
int32_t DspSpiTransfer(DevHandle handle, const uint8_t *msgs, const uint32_t count)
{
    if (handle == NULL || msgs == NULL || count == 0) {
        AUDIO_DRIVER_LOG_ERR("DspSpiTransfer fail");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("DspSpiTransfer success");
    return HDF_SUCCESS;
}
int32_t DspSpiRead(DevHandle handle, const uint8_t *buf, const uint32_t len)
{
    if (handle == NULL || buf == NULL || len == 0) {
        AUDIO_DRIVER_LOG_ERR("DspSpiRead fail");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("DspSpiRead success");
    return HDF_SUCCESS;
}

int32_t DspSpiSetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    if (handle == NULL || cfg == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspSpiSetCfg fail");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("DspSpiSetCfg success");
    return HDF_SUCCESS;
}
int32_t DspSpiGetCfg(DevHandle handle, struct SpiCfg *cfg)
{
    if (handle == NULL || cfg == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspSpiGetCfg fail");
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("DspSpiGetCfg success");
    return HDF_SUCCESS;
}

int32_t DspDeviceInit(const struct DspDevice *device)
{
    DevHandle devHandle;
    struct SpiCfg devCfg = {
        .maxSpeedHz = DEFAULT_SPEED,
        .mode = SPI_CLK_POLARITY,
        .transferMode = SPI_DMA_TRANSFER,
        .bitsPerWord = BITS_PER_WORD_EIGHT,
    };

    if (DspPowerEnable() != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("DspPowerEnable: return Error!");
        return HDF_FAILURE;
    }

    if (DspGpioPinInit() != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("DspGpioPinInit: return Error!");
        return HDF_FAILURE;
    }

    if (DspI2cPinInit() != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("DspI2cPinInit: return Error!");
        return HDF_FAILURE;
    }

    if (DspSpiPinInit() == HDF_SUCCESS) {
        devHandle = DspSpiOpen(&g_devInfo);
        if (devHandle == NULL) {
            AUDIO_DRIVER_LOG_ERR("DspDeviceOpen: Spi failed!");
            return HDF_FAILURE;
        }

        if (DspSpiSetCfg(devHandle, &devCfg) != HDF_SUCCESS) {
            DspSpiClose(devHandle);
            AUDIO_DRIVER_LOG_ERR("DspDeviceCfg: spi failed!");
            return HDF_FAILURE;
        }
        DspSpiClose(devHandle);
    } else {
        AUDIO_DRIVER_LOG_ERR("Dsp Gpio Pin: not init!");
    }

    if (DspI2cInit() != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (DspI2sInit() != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t DspDeviceReadReg(const struct DspDevice *device, const void *msgs, const uint32_t len)
{
    int32_t ret;
    if (msgs == NULL || len == 0) {
        AUDIO_DRIVER_LOG_ERR("input param is nullptr.");
        return HDF_FAILURE;
    }

    DevHandle devHandle = DspSpiOpen(&g_devInfo);
    if (devHandle == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspDeviceOpen: Spi failed!");
        return HDF_FAILURE;
    }

    ret = DspSpiRead(devHandle, msgs, len);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("DspDeviceRead: spi failed!");
        DspSpiClose(devHandle);
        return HDF_FAILURE;
    }

    DspSpiClose(devHandle);

    return HDF_SUCCESS;
}

int32_t DspDeviceWriteReg(const struct DspDevice *device, const void *msgs, const uint32_t len)
{
    int32_t ret;

    if (msgs == NULL || len == 0) {
        AUDIO_DRIVER_LOG_ERR("input param is nullptr.");
        return HDF_FAILURE;
    }

    DevHandle devHandle = DspSpiOpen(&g_devInfo);
    if (devHandle == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspDeviceOpen: Spi failed!");
        return HDF_FAILURE;
    }

    ret = DspSpiTransfer(devHandle, msgs, len);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("DspDeviceRead: spi failed!");
        DspSpiClose(devHandle);
        return HDF_FAILURE;
    }

    DspSpiClose(devHandle);

    return HDF_SUCCESS;
}

int32_t DspDaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device)
{
    if (device == NULL || device->devDaiName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("dsp Dai device name: %s\n", device->devDaiName);
    (void)card;
    return HDF_SUCCESS;
}

int32_t DspDecodeAudioStream(const struct AudioCard *card, const uint8_t *buf, const struct DspDevice *device)
{
    (void)card;
    (void)buf;
    (void)device;
    AUDIO_DRIVER_LOG_DEBUG("decode run!!!");
    return HDF_SUCCESS;
}

int32_t DspEncodeAudioStream(const struct AudioCard *card, const uint8_t *buf, const struct DspDevice *device)
{
    (void)card;
    (void)buf;
    (void)device;
    AUDIO_DRIVER_LOG_DEBUG("encode run!!!");
    return HDF_SUCCESS;
}


int32_t DspEqualizerActive(const struct AudioCard *card, const uint8_t *buf, const struct DspDevice *device)
{
    (void)card;
    (void)buf;
    (void)device;
    AUDIO_DRIVER_LOG_DEBUG("equalizer run!!!");
    return HDF_SUCCESS;
}
