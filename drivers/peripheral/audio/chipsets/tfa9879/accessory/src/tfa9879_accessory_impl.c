/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "tfa9879_accessory_impl.h"
#include "audio_accessory_base.h"
#include "audio_driver_log.h"
#include "gpio_if.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

/* Hi35xx register address */
#define HI35XX_GPIO_REG_BASE_ADDR   (0x112F0000)
#define HI35XX_GPIO_REG_OFFSET      (0x0034)
#define HI35XX_GPIO_REG_REMAP_SIZE  (0x1000)
#define HI35XX_GPIO_REG_VAL         (0x0400)
#define HI35XX_GPIO_NUMBER          (6)
/* TFA9879 I2C Device address 1 1 0 1 1 A2 A1 R/W */
#define TFA9879_I2C_DEV_ADDR        (0x6D) // 1101101
#define TFA9879_I2C_BUS_NUMBER      (6)    // i2c6

struct AccessoryTransferData g_accessoryTransferData;

// Init Function
/*
 * Hi35xx gpio0_6 pin init
*/
static int32_t Hi35xxGpioPinInit(void)
{
    int32_t ret;
    char *regGpioBase = 0;
    regGpioBase = (void *)OsalIoRemap(HI35XX_GPIO_REG_BASE_ADDR, HI35XX_GPIO_REG_REMAP_SIZE);
    if (regGpioBase == NULL) {
        AUDIO_DEVICE_LOG_ERR("regGpioBase is null.");
        return HDF_FAILURE;
    }
    SysWritel((uintptr_t)regGpioBase + HI35XX_GPIO_REG_OFFSET, HI35XX_GPIO_REG_VAL); // GPIO0_6
    if (regGpioBase != NULL) {
        OsalIoUnmap(regGpioBase);
    }
    ret = GpioSetDir(HI35XX_GPIO_NUMBER, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("GpioSetDir failed. ret:%d", ret);
        return ret;
    }
    ret = GpioWrite(HI35XX_GPIO_NUMBER, GPIO_VAL_HIGH);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("GpioWrite failed. ret:%d", ret);
        return ret;
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

static int32_t Tfa9879FormatParse(enum AudioFormat format, uint16_t *bitWidth)
{
    // current set default format(standard) for 16/24 bit
    if (bitWidth == NULL) {
        AUDIO_DEVICE_LOG_ERR("input param is NULL");
        return HDF_FAILURE;
    }
    switch (format) {
        case AUDIO_FORMAT_PCM_16_BIT:
            *bitWidth = I2S_SAMPLE_FORMAT_REG_VAL_24;
            break;
        case AUDIO_FORMAT_PCM_24_BIT:
            *bitWidth = I2S_SAMPLE_FORMAT_REG_VAL_24;
            break;
        default:
            AUDIO_DEVICE_LOG_ERR("format: %d is not support.", format);
            return HDF_ERR_NOT_SUPPORT;
    }
    AUDIO_DEVICE_LOG_DEBUG(" success.");
    return HDF_SUCCESS;
}

static int32_t Tfa9879WorkStatusEnable()
{
    int ret;
    uint8_t i;
    struct AudioMixerControl *daiStartupParamsRegCfgItem = NULL;
    uint8_t daiStartupParamsRegCfgItemCount;
    ret = (g_accessoryTransferData.accessoryRegCfgGroupNode == NULL
        || g_accessoryTransferData.accessoryRegCfgGroupNode[AUDIO_DAI_STARTUP_PATAM_GROUP] == NULL
        || g_accessoryTransferData.accessoryRegCfgGroupNode[AUDIO_DAI_STARTUP_PATAM_GROUP]->regCfgItem == NULL);
    if (ret) {
        AUDIO_DEVICE_LOG_ERR("g_audioRegCfgGroupNode[AUDIO_DAI_STARTUP_PATAM_GROUP] is NULL.");
        return HDF_FAILURE;
    }
    daiStartupParamsRegCfgItem =
        g_accessoryTransferData.accessoryRegCfgGroupNode[AUDIO_DAI_STARTUP_PATAM_GROUP]->regCfgItem;
    daiStartupParamsRegCfgItemCount =
        g_accessoryTransferData.accessoryRegCfgGroupNode[AUDIO_DAI_STARTUP_PATAM_GROUP]->itemNum;
    for (i = 0; i < daiStartupParamsRegCfgItemCount; i++) {
        ret = AccessoryRegBitsUpdate(daiStartupParamsRegCfgItem[i]);
        if (ret != HDF_SUCCESS) {
            AUDIO_DEVICE_LOG_ERR("AccessoryRegBitsUpdate fail.");
            return HDF_FAILURE;
        }
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t Tfa9879DeviceInit(struct AudioCard *audioCard, const struct AccessoryDevice *device)
{
    int32_t ret;
    if ((audioCard == NULL) || (device == NULL)) {
        AUDIO_DEVICE_LOG_ERR("input para is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    g_accessoryTransferData.i2cDevAddr = TFA9879_I2C_DEV_ADDR;
    g_accessoryTransferData.i2cBusNumber = TFA9879_I2C_BUS_NUMBER;
    ret = AccessoryDeviceCfgGet(device->devData, &g_accessoryTransferData);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AccessoryDeviceCfgGet failed.");
        return HDF_FAILURE;
    }
    ret = Hi35xxGpioPinInit();
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("Hi35xxGpioPinInit failed.");
        return HDF_FAILURE;
    }
    // Initial tfa9879 register
    ret = AccessoryDeviceCtrlRegInit();
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AccessoryDeviceRegInit failed.");
        return HDF_FAILURE;
    }

    ret = AudioAddControls(audioCard, g_accessoryTransferData.accessoryControls,
                           g_accessoryTransferData.accessoryCfgCtrlCount);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AudioAddControls failed.");
        return HDF_FAILURE;
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t Tfa9879DaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device)
{
    if (device == NULL || device->devDaiName == NULL) {
        AUDIO_DEVICE_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }
    (void)card;
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t Tfa9879DaiStartup(const struct AudioCard *card, const struct DaiDevice *device)
{
    int ret;
    (void)card;
    (void)device;
    ret = Tfa9879WorkStatusEnable();
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("Tfa9879WorkStatusEnable failed.");
        return HDF_FAILURE;
    }
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t Tfa9879DaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param)
{
    int32_t ret;
    uint16_t frequency, bitWidth;
    struct DaiParamsVal daiParamsVal;
    (void)card;
    if (param == NULL || param->cardServiceName == NULL) {
        AUDIO_DEVICE_LOG_ERR("input para is NULL.");
        return HDF_ERR_INVALID_PARAM;
    }
    ret = AccessoryDeviceFrequencyParse(param->rate, &frequency);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AccessoryDeviceFrequencyParse failed.");
        return HDF_ERR_NOT_SUPPORT;
    }
    ret = Tfa9879FormatParse(param->format, &bitWidth);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("Tfa9879FormatParse failed.");
        return HDF_ERR_NOT_SUPPORT;
    }
    daiParamsVal.frequencyVal = frequency;
    daiParamsVal.formatVal = bitWidth;
    daiParamsVal.channelVal = param->channels;
    ret = AccessoryDaiParamsUpdate(daiParamsVal);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AccessoryDaiParamsUpdate failed.");
        return HDF_FAILURE;
    }
    AUDIO_DEVICE_LOG_DEBUG("channels = %d, rate = %d, periodSize = %d, \
        periodCount = %d, format = %d, cardServiceName = %s \n",
        param->channels, param->rate, param->periodSize,
        param->periodCount, (uint32_t)param->format, param->cardServiceName);
    AUDIO_DEVICE_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}
