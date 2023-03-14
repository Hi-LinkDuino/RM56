/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_core.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

static const char *g_audioDaiControlsList[AUDIO_CTRL_LIST_MAX] = {
    "Main Playback Volume", "Main Capture Volume",
    "Playback Mute", "Capture Mute", "Mic Left Gain",
    "Mic Right Gain", "External Codec Enable",
    "Internally Codec Enable", "Render Channel Mode", "Captrue Channel Mode"
};

struct DaiData *DaiDataFromCard(const struct AudioCard *card)
{
    if (card == NULL || card->rtd == NULL || card->rtd->cpuDai == NULL) {
        AUDIO_DRIVER_LOG_ERR("param is null.");
        return NULL;
    }
    return card->rtd->cpuDai->devData;
}


int32_t DaiGetConfigInfo(const struct HdfDeviceObject *device, struct DaiData *data)
{
    if (device == NULL || data == NULL) {
        AUDIO_DRIVER_LOG_ERR("param is null!");
        return HDF_FAILURE;
    }

    if (data->regConfig != NULL) {
        ADM_LOG_ERR("g_codecData regConfig  fail!");
        return HDF_FAILURE;
    }

    data->regConfig = (struct AudioRegCfgData *)OsalMemCalloc(sizeof(*(data->regConfig)));
    if (data->regConfig == NULL) {
        ADM_LOG_ERR("malloc AudioRegCfgData fail!");
        return HDF_FAILURE;
    }

    if (CodecGetRegConfig(device, data->regConfig) != HDF_SUCCESS) {
        ADM_LOG_ERR("dai GetRegConfig fail!");
        OsalMemFree(data->regConfig);
        data->regConfig = NULL;
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t DaiCheckSampleRate(unsigned int sampleRates)
{
    bool check = (sampleRates == AUDIO_SAMPLE_RATE_8000 || sampleRates == AUDIO_SAMPLE_RATE_12000 ||
                  sampleRates == AUDIO_SAMPLE_RATE_11025 || sampleRates == AUDIO_SAMPLE_RATE_16000 ||
                  sampleRates == AUDIO_SAMPLE_RATE_22050 || sampleRates == AUDIO_SAMPLE_RATE_24000 ||
                  sampleRates == AUDIO_SAMPLE_RATE_32000 || sampleRates == AUDIO_SAMPLE_RATE_44100 ||
                  sampleRates == AUDIO_SAMPLE_RATE_48000 || sampleRates == AUDIO_SAMPLE_RATE_64000 ||
                  sampleRates == AUDIO_SAMPLE_RATE_96000);
    if (check) {
        return HDF_SUCCESS;
    } else {
        AUDIO_DRIVER_LOG_ERR("FramatToSampleRate fail: Invalid sampling rate: %u.", sampleRates);
        return HDF_ERR_NOT_SUPPORT;
    }
}

int32_t DaiSetConfigInfo(struct DaiData *data)
{
    uint16_t index;
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    struct AudioMixerControl    *patRegCfgItemTmp = NULL;
    struct AudioControlConfig   *item = NULL;

    if (data == NULL || data->regConfig == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    regCfgGroup = data->regConfig->audioRegParams;
    if (regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_DEBUG("regCfgGroup is null.");
        return HDF_SUCCESS;
    }

    patRegCfgItemTmp = regCfgGroup[AUDIO_CTRL_PATAM_GROUP]->regCfgItem;
    item = regCfgGroup[AUDIO_CTRL_CFG_GROUP]->ctrlCfgItem;

    if (patRegCfgItemTmp == NULL || item == NULL) {
        AUDIO_DRIVER_LOG_ERR("patRegCfgItemTmp or item is NULL.");
        return HDF_FAILURE;
    }

    data->numControls = regCfgGroup[AUDIO_CTRL_CFG_GROUP]->itemNum;
    data->controls = (struct AudioKcontrol *)OsalMemCalloc(data->numControls * sizeof(struct AudioKcontrol));
    if (data->controls == NULL) {
        AUDIO_DRIVER_LOG_ERR("OsalMemCalloc failed.");
        return HDF_FAILURE;
    }

    for (index = 0; index < data->numControls; index++) {
        data->controls[index].iface    = item[index].iface;
        if (item[index].arrayIndex >= AUDIO_CTRL_LIST_MAX) {
            AUDIO_DRIVER_LOG_ERR("Array super index.");
            OsalMemFree(data->controls);
            data->controls = NULL;
            return HDF_FAILURE;
        }
        data->controls[index].name     = g_audioDaiControlsList[item[index].arrayIndex];
        data->controls[index].Info     = AudioInfoCtrlOps;
        data->controls[index].privateValue = (unsigned long)(uintptr_t)(void*)(&patRegCfgItemTmp[index]);
        data->controls[index].Get = AudioCpuDaiGetCtrlOps;
        data->controls[index].Set = AudioCpuDaiSetCtrlOps;
    }

    return HDF_SUCCESS;
}

