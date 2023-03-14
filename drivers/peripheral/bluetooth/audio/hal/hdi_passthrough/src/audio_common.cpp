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

#ifndef AUDIO_COMMON_H
#define AUDIO_COMMON_H
#include <securec.h>
#include "audio_internal.h"
#include "osal_time.h"
namespace OHOS::HDI::Audio_Bluetooth {
void AudioMemFree(void **ppMem)
{
    if ((ppMem != NULL) && ((*ppMem) != NULL)) {
        free(*ppMem);
        *ppMem = NULL;
    }
    return;
}

int32_t AudioGetSysTime(char *s, int32_t len)
{
    OsalTimespec time;
    if (s == NULL) {
        return -1;
    }
    OsalGetTime(&time);
    s[0] = 0;
    int32_t ret = snprintf_s(s, len, len - 1, "[%llu.%llu]", time.sec, time.usec);
    return ret;
}

int32_t CheckAttrSamplingRate(uint32_t param)
{
    switch (param) {
        case TELHPONE_RATE:
        case BROADCAST_AM_RATE:
        case BROADCAST_FM_RATE:
        case MINI_CAM_DV_RATE:
        case MUSIC_RATE:
        case HIGHT_MUSIC_RATE:
        case AUDIO_SAMPLE_RATE_12000:
        case AUDIO_SAMPLE_RATE_16000:
        case AUDIO_SAMPLE_RATE_24000:
        case AUDIO_SAMPLE_RATE_64000:
        case AUDIO_SAMPLE_RATE_96000:
            return HDF_SUCCESS;
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
}

int32_t CheckAttrFormat(AudioFormat param)
{
    switch (param) {
        case AUDIO_FORMAT_PCM_8_BIT:
        case AUDIO_FORMAT_PCM_16_BIT:
        case AUDIO_FORMAT_PCM_24_BIT:
        case AUDIO_FORMAT_PCM_32_BIT:
        case AUDIO_FORMAT_AAC_MAIN:
        case AUDIO_FORMAT_AAC_LC:
        case AUDIO_FORMAT_AAC_LD:
        case AUDIO_FORMAT_AAC_ELD:
        case AUDIO_FORMAT_AAC_HE_V1:
        case AUDIO_FORMAT_AAC_HE_V2:
            break;
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
    return HDF_SUCCESS;
}

int32_t AudioCheckParaAttr(const struct AudioSampleAttributes *attrs)
{
    if (attrs == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    AudioCategory audioCategory = attrs->type;
    if (AUDIO_IN_MEDIA != audioCategory && AUDIO_IN_COMMUNICATION != audioCategory) {
        return HDF_ERR_NOT_SUPPORT;
    }
    AudioFormat audioFormat = attrs->format;
    ret = CheckAttrFormat(audioFormat);
    if (ret < 0) {
        return ret;
    }
    uint32_t sampleRateTemp = attrs->sampleRate;
    return CheckAttrSamplingRate(sampleRateTemp);
}

int32_t TimeToAudioTimeStamp(uint64_t bufferFrameSize, struct AudioTimeStamp *time, uint32_t sampleRate)
{
    if (time == NULL) {
        return HDF_FAILURE;
    }
    time->tvSec += (int64_t)bufferFrameSize / (int64_t)sampleRate;
    int64_t lastBufFrames = bufferFrameSize % ((int64_t)sampleRate);
    time->tvNSec += (lastBufFrames * SEC_TO_NSEC) / ((int64_t)sampleRate);
    if (time->tvNSec >= SEC_TO_NSEC) {
        time->tvSec += 1;
        time->tvNSec -= SEC_TO_NSEC;
    }
    return HDF_SUCCESS;
}
}
#endif