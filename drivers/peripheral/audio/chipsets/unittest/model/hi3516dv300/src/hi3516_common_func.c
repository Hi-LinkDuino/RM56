/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_common_func.h"

int32_t InitHwParam(struct AudioPcmHwParams *hwParam)
{
    const uint32_t channelNum = 2;
    const uint32_t sampleRate = 48000;
    const uint32_t periodSize = 4096;
    const uint32_t periodCount = 8;
    const int format = 2;
    const uint32_t startThreshold = 1024;
    const uint32_t stopThreshold = 0x7fffffff;
    const uint32_t silenceThreshold = 1024 * 16;

    if (hwParam == NULL) {
        HDF_LOGE("InitHwParam: input param is NULL.");
        return HDF_FAILURE;
    }

    hwParam->channels  = channelNum;
    hwParam->rate = sampleRate;
    hwParam->periodSize = periodSize;
    hwParam->periodCount = periodCount;
    hwParam->format = format;
    hwParam->cardServiceName = "hdf_audio_codec_dev0";
    hwParam->isBigEndian = false;
    hwParam->isSignedData = true;
    hwParam->startThreshold = startThreshold;
    hwParam->stopThreshold = stopThreshold;
    hwParam->silenceThreshold = silenceThreshold;
    hwParam->streamType = 0; // AUDIO_RENDER_STREAM
    return HDF_SUCCESS;
}


int32_t GetAudioCard(struct AudioCard **card)
{
    int i;
    const char *audioServiceName[] = {
        "hdf_audio_codec_dev0",
        "hdf_audio_smartpa_dev0",
    };
    HDF_LOGI("%s: enter", __func__);
    if (card == NULL) {
        HDF_LOGE("input param is NULL");
        return HDF_FAILURE;
    }

    for (i = 0; i < sizeof(audioServiceName) / sizeof(audioServiceName[0]); ++i) {
        if (GetCardInstance(audioServiceName[i]) != NULL) {
            HDF_LOGI("%s: get %s success!", __func__, audioServiceName[i]);
            *card = GetCardInstance(audioServiceName[i]);
            break;
        }
    }

    if (*card == NULL) {
        HDF_LOGE("get card instance failed");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
