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

#include "audio_internal.h"
#include "audio_adapter_info_common.h"
#include "audio_bluetooth_manager.h"
#include "audio_render.h"
namespace OHOS::HDI::Audio_Bluetooth {
/* 1 buffer: 8000(8kHz sample rate) * 2(bytes, PCM_16_BIT) * 1(channel) */
/* 1 frame: 1024(sample) * 2(bytes, PCM_16_BIT) * 1(channel) */
constexpr int FRAME_SIZE = 1024;

int32_t PcmBytesToFrames(const struct AudioFrameRenderMode *frameRenderMode, uint64_t bytes, uint32_t *frameCount)
{
    if (frameRenderMode == NULL || frameCount == NULL) {
        return HDF_FAILURE;
    }
    uint32_t formatBits = 0;
    int32_t ret = FormatToBits(frameRenderMode->attrs.format, &formatBits);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    uint32_t frameSize = frameRenderMode->attrs.channelCount * (formatBits >> 3); // Bit to byte >> 3
    if (frameSize == 0) {
        return HDF_FAILURE;
    }
    *frameCount = (uint32_t)bytes / frameSize;
    return HDF_SUCCESS;
}

int32_t AudioRenderStart(AudioHandle handle)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->renderParam.frameRenderMode.buffer != NULL) {
        LOG_FUN_ERR("AudioRender already start!");
        return AUDIO_HAL_ERR_AO_BUSY; // render is busy now
    }

    LOG_PARA_INFO("%s, StartPlaying", __func__);
    if (OHOS::Bluetooth::GetPlayingState() == false) {
        OHOS::Bluetooth::StartPlaying();
    }

    char *buffer = (char *)calloc(1, FRAME_DATA);
    if (buffer == NULL) {
        LOG_FUN_ERR("Calloc Render buffer Fail!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwRender->renderParam.frameRenderMode.buffer = buffer;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderStop(AudioHandle handle)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->renderParam.frameRenderMode.buffer != NULL) {
        AudioMemFree((void **)&hwRender->renderParam.frameRenderMode.buffer);
    } else {
        LOG_FUN_ERR("Repeat invalid stop operation!");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }

    LOG_PARA_INFO("%s, StopPlaying", __func__);
    if (OHOS::Bluetooth::GetPlayingState() == true) {
        OHOS::Bluetooth::StopPlaying();
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderPause(AudioHandle handle)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->renderParam.frameRenderMode.buffer == NULL) {
        LOG_FUN_ERR("AudioRender already stop!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (hwRender->renderParam.renderMode.ctlParam.pause) {
        LOG_FUN_ERR("Audio is already pause!");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    
    LOG_PARA_INFO("%s, SuspendPlaying", __func__);
    if (OHOS::Bluetooth::GetPlayingState() == true) {
        OHOS::Bluetooth::SuspendPlaying();
    }

    hwRender->renderParam.renderMode.ctlParam.pause = true;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderResume(AudioHandle handle)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (!hwRender->renderParam.renderMode.ctlParam.pause) {
        LOG_FUN_ERR("Audio is already Resume !");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    
    LOG_PARA_INFO("%s, StartPlaying", __func__);
    if (OHOS::Bluetooth::GetPlayingState() == false) {
        OHOS::Bluetooth::StartPlaying();
    }

    hwRender->renderParam.renderMode.ctlParam.pause = false;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderFlush(AudioHandle handle)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioRenderGetFrameSize(AudioHandle handle, uint64_t *size)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    const int shift = 3;
    if (hwRender == NULL || size == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t channelCount = hwRender->renderParam.frameRenderMode.attrs.channelCount;
    AudioFormat format = hwRender->renderParam.frameRenderMode.attrs.format;
    uint32_t formatBits = 0;
    int32_t ret = FormatToBits(format, &formatBits);
    if (ret != AUDIO_HAL_SUCCESS) {
        return ret;
    }
    *size = FRAME_SIZE * channelCount * (formatBits >> shift);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetFrameCount(AudioHandle handle, uint64_t *count)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || count == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    *count = hwRender->renderParam.frameRenderMode.frames;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs)
{
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || attrs == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    attrs->format = hwRender->renderParam.frameRenderMode.attrs.format;
    attrs->sampleRate = hwRender->renderParam.frameRenderMode.attrs.sampleRate;
    attrs->channelCount = hwRender->renderParam.frameRenderMode.attrs.channelCount;
    attrs->type = hwRender->renderParam.frameRenderMode.attrs.type;
    attrs->interleaved = hwRender->renderParam.frameRenderMode.attrs.interleaved;
    attrs->period = hwRender->renderParam.frameRenderMode.attrs.period;
    attrs->frameSize = hwRender->renderParam.frameRenderMode.attrs.frameSize;
    attrs->isBigEndian = hwRender->renderParam.frameRenderMode.attrs.isBigEndian;
    attrs->isSignedData = hwRender->renderParam.frameRenderMode.attrs.isSignedData;
    attrs->startThreshold = hwRender->renderParam.frameRenderMode.attrs.startThreshold;
    attrs->stopThreshold = hwRender->renderParam.frameRenderMode.attrs.stopThreshold;
    attrs->silenceThreshold = hwRender->renderParam.frameRenderMode.attrs.silenceThreshold;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetCurrentChannelId(AudioHandle handle, uint32_t *channelId)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || channelId == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    *channelId = hwRender->renderParam.frameRenderMode.attrs.channelCount;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderCheckSceneCapability(AudioHandle handle, const struct AudioSceneDescriptor *scene,
                                        bool *supported)
{
    LOG_FUN_INFO();
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene)
{
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetMute(AudioHandle handle, bool mute)
{
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetMute(AudioHandle handle, bool *mute)
{
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetVolume(AudioHandle handle, float volume)
{
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetVolume(AudioHandle handle, float *volume)
{
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetGainThreshold(AudioHandle handle, float *min, float *max)
{
    LOG_FUN_INFO();
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetGain(AudioHandle handle, float *gain)
{
    LOG_FUN_INFO();
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetGain(AudioHandle handle, float gain)
{
    LOG_FUN_INFO();
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetLatency(struct AudioRender *render, uint32_t *ms)
{
    struct AudioHwRender *impl = reinterpret_cast<struct AudioHwRender *>(render);
    if (impl == NULL || ms == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t byteRate = impl->renderParam.frameRenderMode.byteRate;
    uint32_t periodSize = impl->renderParam.frameRenderMode.periodSize;
    uint32_t periodCount = impl->renderParam.frameRenderMode.periodCount;
    if (byteRate == 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t periodMs = (periodCount * periodSize * 1000) / byteRate;
    *ms = periodMs;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderRenderFramSplit(struct AudioHwRender *hwRender)
{
    return HDF_SUCCESS;
}

int32_t AudioRenderRenderFrame(struct AudioRender *render, const void *frame,
                               uint64_t requestBytes, uint64_t *replyBytes)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || frame == NULL || replyBytes == NULL ||
        hwRender->renderParam.frameRenderMode.buffer == NULL) {
        LOG_FUN_ERR("Render Frame Paras is NULL!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (FRAME_DATA < requestBytes) {
        LOG_FUN_ERR("Out of FRAME_DATA size!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = memcpy_s(hwRender->renderParam.frameRenderMode.buffer, FRAME_DATA, frame, (uint32_t)requestBytes);
    if (ret != EOK) {
        LOG_FUN_ERR("memcpy_s fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    hwRender->renderParam.frameRenderMode.bufferSize = requestBytes;
    uint32_t frameCount = 0;
    ret = PcmBytesToFrames(&hwRender->renderParam.frameRenderMode, requestBytes, &frameCount);
    if (ret != AUDIO_HAL_SUCCESS) {
        return ret;
    }
    hwRender->renderParam.frameRenderMode.bufferFrameSize = (uint64_t)frameCount;
    if (AudioRenderRenderFramSplit(hwRender) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *replyBytes = requestBytes;
    hwRender->renderParam.frameRenderMode.frames += hwRender->renderParam.frameRenderMode.bufferFrameSize;
    if (hwRender->renderParam.frameRenderMode.attrs.sampleRate == 0) {
        LOG_FUN_ERR("Divisor cannot be zero!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (TimeToAudioTimeStamp(hwRender->renderParam.frameRenderMode.bufferFrameSize,
        &hwRender->renderParam.frameRenderMode.time,
        hwRender->renderParam.frameRenderMode.attrs.sampleRate) == HDF_FAILURE) {
        LOG_FUN_ERR("Frame is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }

    LOGV("%s, WriteFrame", __func__);
    OHOS::Bluetooth::WriteFrame((const uint8_t *)frame, (uint32_t)requestBytes);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetRenderPosition(struct AudioRender *render, uint64_t *frames, struct AudioTimeStamp *time)
{
    struct AudioHwRender *impl = reinterpret_cast<struct AudioHwRender *>(render);
    if (impl == NULL || frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    *frames = impl->renderParam.frameRenderMode.frames;
    *time = impl->renderParam.frameRenderMode.time;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetRenderSpeed(struct AudioRender *render, float speed)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioRenderGetRenderSpeed(struct AudioRender *render, float *speed)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || speed == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioRenderSetChannelMode(struct AudioRender *render, AudioChannelMode mode)
{
    LOG_FUN_INFO();
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetChannelMode(struct AudioRender *render, AudioChannelMode *mode)
{
    LOG_FUN_INFO();
    return AUDIO_HAL_SUCCESS;
}

int32_t SetValue(struct ExtraParams mExtraParams, struct AudioHwRender *render)
{
    if (render == NULL) {
        return HDF_FAILURE;
    }
    if (mExtraParams.route != -1) {
        render->renderParam.renderMode.hwInfo.pathroute = (PathRoute)mExtraParams.route;
    }
    if (mExtraParams.format != -1) {
        render->renderParam.frameRenderMode.attrs.format = (AudioFormat)mExtraParams.format;
    }
    if (mExtraParams.channels != 0) {
        render->renderParam.frameRenderMode.attrs.channelCount = mExtraParams.channels;
    }
    if (mExtraParams.flag) {
        render->renderParam.frameRenderMode.frames = mExtraParams.frames;
    }
    if (mExtraParams.sampleRate != 0) {
        render->renderParam.frameRenderMode.attrs.sampleRate = mExtraParams.sampleRate;
    }
    return HDF_SUCCESS;
}

int32_t AudioRenderSetExtraParams(AudioHandle handle, const char *keyValueList)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL || keyValueList == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t count = 0;
    int32_t sumOk = 0;
    struct ExtraParams mExtraParams;
    if (AudioSetExtraParams(keyValueList, &count, &mExtraParams, &sumOk) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (count != 0 && sumOk == count) {
        SetValue(mExtraParams, render);
        return AUDIO_HAL_SUCCESS;
    } else {
        return AUDIO_HAL_ERR_INTERNAL;
    }
}

int32_t AudioRenderGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL || keyValueList == NULL || listLenth <= 0) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t bufferSize = strlen(ROUTE_SAMPLE) + strlen(FORMAT_SAMPLE) + strlen(CHANNELS_SAMPLE)
                    + strlen(FRAME_COUNT_SAMPLE) + strlen(SAMPLING_RATE_SAMPLE) + 1;
    if (listLenth < bufferSize) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AddElementToList(keyValueList, listLenth, AUDIO_ATTR_PARAM_ROUTE,
        &render->renderParam.renderMode.hwInfo.pathroute);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AddElementToList(keyValueList, listLenth, AUDIO_ATTR_PARAM_FORMAT,
        &render->renderParam.frameRenderMode.attrs.format);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AddElementToList(keyValueList, listLenth, AUDIO_ATTR_PARAM_CHANNELS,
        &render->renderParam.frameRenderMode.attrs.channelCount);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AddElementToList(keyValueList, listLenth, AUDIO_ATTR_PARAM_FRAME_COUNT,
        &render->renderParam.frameRenderMode.frames);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AddElementToList(keyValueList, listLenth, AUDIO_ATTR_PARAM_SAMPLING_RATE,
        &render->renderParam.frameRenderMode.attrs.sampleRate);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderReqMmapBuffer(AudioHandle handle, int32_t reqSize, struct AudioMmapBufferDescripter *desc)
{
    LOG_PARA_INFO("AudioRenderReqMmapBuffer Success!");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetMmapPosition(AudioHandle handle, uint64_t *frames, struct AudioTimeStamp *time)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL || frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    *frames = render->renderParam.frameRenderMode.frames;
    render->renderParam.frameRenderMode.time.tvSec = (int64_t)render->renderParam.frameRenderMode.frames /
                                       (int64_t)render->renderParam.frameRenderMode.attrs.sampleRate;
    int64_t lastBufFrames = render->renderParam.frameRenderMode.frames %
                        ((int64_t)render->renderParam.frameRenderMode.attrs.sampleRate);
    render->renderParam.frameRenderMode.time.tvNSec =
        (lastBufFrames * SEC_TO_NSEC) / ((int64_t)render->renderParam.frameRenderMode.attrs.sampleRate);
    *time = render->renderParam.frameRenderMode.time;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderTurnStandbyMode(AudioHandle handle)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = AudioRenderStop((AudioHandle)render);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderAudioDevDump(AudioHandle handle, int32_t range, int32_t fd)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    dprintf(fd, "%s%d\n", "Number of errors: ", render->errorLog.totalErrors);
    if (range < RANGE_MIN - 1 || range > RANGE_MAX) {
        dprintf(fd, "%s\n", "Out of range, invalid output");
        return AUDIO_HAL_SUCCESS;
    }
    uint32_t mSize = render->errorLog.iter;
    if (range < RANGE_MIN) {
        dprintf(fd, "%-5s  %-10s  %s\n", "count", "errorCode", "Time");
        for (uint32_t i = 0; i < mSize; i++) {
            dprintf(fd, FORMAT_TWO, render->errorLog.errorDump[i].count + 1,
                    render->errorLog.errorDump[i].errorCode,
                    render->errorLog.errorDump[i].currentTime);
        }
    } else {
        dprintf(fd, "%-5s  %-10s  %-20s  %-15s  %s\n", "count", "errorCode", "frames", "fail reason", "Time");
        for (uint32_t i = 0; i < mSize; i++) {
            dprintf(fd, FORMAT_ONE, render->errorLog.errorDump[i].count + 1,
                    render->errorLog.errorDump[i].errorCode,
                    render->errorLog.errorDump[i].frames,
                    render->errorLog.errorDump[i].reason,
                    render->errorLog.errorDump[i].currentTime);
        }
    }
    return AUDIO_HAL_SUCCESS;
}
int32_t CallbackProcessing(AudioHandle handle, AudioCallbackType callBackType)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL) {
        LOG_PARA_INFO("Unregistered callback.\n");
        return HDF_FAILURE;
    }
    if (render->renderParam.frameRenderMode.callback == NULL) {
        return HDF_FAILURE;
    }
    bool isCallBack = true;
    switch (callBackType) {
        case AUDIO_NONBLOCK_WRITE_COMPELETED:
        case AUDIO_DRAIN_COMPELETED:
        case AUDIO_FLUSH_COMPLETED:
        case AUDIO_RENDER_FULL:
        case AUDIO_ERROR_OCCUR:
            isCallBack = true;
            break;
        default:
            isCallBack = false;
            break;
    }
    if (!isCallBack) {
        LOG_PARA_INFO("No callback processing is required.\n");
        return HDF_ERR_NOT_SUPPORT;
    }
    render->renderParam.frameRenderMode.callback(callBackType, NULL, render->renderParam.frameRenderMode.cookie);
    return HDF_SUCCESS;
}

int32_t AudioRenderRegCallback(struct AudioRender *render, RenderCallback callback, void *cookie)
{
    struct AudioHwRender *pRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (pRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    pRender->renderParam.frameRenderMode.callback = callback;
    pRender->renderParam.frameRenderMode.cookie = cookie;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderDrainBuffer(struct AudioRender *render, AudioDrainNotifyType *type)
{
    struct AudioHwRender *pRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (pRender == NULL || type == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}
}