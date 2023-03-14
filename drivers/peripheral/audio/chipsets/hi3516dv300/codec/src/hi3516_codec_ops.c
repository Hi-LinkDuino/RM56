/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_codec_ops.h"
#include "audio_sapm.h"
#include "audio_platform_base.h"
#include "hi3516_codec_impl.h"
#include "audio_driver_log.h"
#include "audio_codec_base.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

static const struct AudioSapmRoute g_audioRoutes[] = {
    { "SPKL", "Dacl enable", "DACL"},
    { "SPKR", "Dacr enable", "DACR"},

    { "ADCL", NULL, "LPGA"},
    { "LPGA", "LPGA MIC Switch", "MIC"},

    { "ADCR", NULL, "RPGA"},
    { "RPGA", "RPGA MIC Switch", "MIC"},
};

int32_t CodecDeviceInit(struct AudioCard *audioCard, const struct CodecDevice *codec)
{
    if (audioCard == NULL || codec == NULL || codec->devData == NULL ||
        codec->devData->sapmComponents == NULL || codec->devData->controls == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (CodecSetCtlFunc(codec->devData, AudioCodecAiaoGetCtrlOps, AudioCodecAiaoSetCtrlOps) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AudioCodecSetCtlFunc failed.");
        return HDF_FAILURE;
    }

    if (CodecHalSysInit(codec->devData) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("CodecHalSysInit failed.");
        return HDF_FAILURE;
    }

    if (CodecRegDefaultInit(codec->devData->regCfgGroup) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("CodecRegDefaultInit failed.");
        return HDF_FAILURE;
    }

    if (AudioAddControls(audioCard, codec->devData->controls, codec->devData->numControls) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add controls failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmNewComponents(audioCard, codec->devData->sapmComponents,
        codec->devData->numSapmComponent) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("new components failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmAddRoutes(audioCard, g_audioRoutes, HDF_ARRAY_SIZE(g_audioRoutes)) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add route failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmNewControls(audioCard) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add sapm controls failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmSleep(audioCard) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add sapm sleep modular failed.");
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t CodecDaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device)
{
    if (device == NULL || device->devDaiName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("codec dai device name: %s\n", device->devDaiName);
    (void)card;
    return HDF_SUCCESS;
}

int32_t CodecDaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param)
{
    unsigned int bitWidth;
    struct CodecDaiParamsVal codecDaiParamsVal;

    if (param == NULL || param->cardServiceName == NULL || card == NULL ||
        card->rtd == NULL || card->rtd->codecDai == NULL || card->rtd->codecDai->devData == NULL ||
        card->rtd->codecDai->devData->regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    (void)memset_s(&bitWidth, sizeof(unsigned int), 0, sizeof(unsigned int));
    int ret = AudioFormatToBitWidth(param->format, &bitWidth);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    (void)memset_s(&codecDaiParamsVal, sizeof(struct CodecDaiParamsVal), 0, sizeof(struct CodecDaiParamsVal));
    codecDaiParamsVal.frequencyVal = param->rate;
    codecDaiParamsVal.formatVal = bitWidth;
    ret = CodecDaiParamsUpdate(card->rtd->codecDai->devData->regCfgGroup, codecDaiParamsVal);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("CodecDaiParamsUpdate failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t CodecDaiStartup(const struct AudioCard *card, const struct DaiDevice *device)
{
    int32_t ret;
    if (device == NULL || device->devData == NULL ||
        device->devData->regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }
    (void)card;
    ret = CodecSetAdcTuneEnable(device->devData->regCfgGroup);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("CodecSetAdcTuneEnable failed.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

