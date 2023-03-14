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
#include "audio_render.h"
#include "audio_adapter_info_common.h"
#include "audio_hal_log.h"
#include "audio_interface_lib_render.h"


#define HDF_LOG_TAG HDF_AUDIO_HAL_IMPL

#define CONFIG_OUT_LATENCY_MS  100 // unit: ms

/* 1 buffer: 8000(8kHz sample rate) * 2(bytes, PCM_16_BIT) * 1(channel) */
/* 1 frame: 1024(sample) * 2(bytes, PCM_16_BIT) * 1(channel) */
#define CONFIG_FRAME_SIZE      (1024 * 2 * 1)
#define FRAME_SIZE              1024
#define CONFIG_FRAME_COUNT     ((8000 * 2 * 1 + (CONFIG_FRAME_SIZE - 1)) / CONFIG_FRAME_SIZE)

#define DEEP_BUFFER_PLATFORM_DELAY (29*1000LL)
#define LOW_LATENCY_PLATFORM_DELAY (13*1000LL)

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
    /* channelCount Not greater than 4 , formatBits Not greater than 64 */
    if (frameRenderMode->attrs.channelCount > 4 || formatBits > 64) {
        return HDF_FAILURE;
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
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        LOG_FUN_ERR("The pointer is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (hwRender->renderParam.frameRenderMode.buffer != NULL) {
        LOG_FUN_ERR("AudioRender already start!");
        return AUDIO_HAL_ERR_AO_BUSY; // render is busy now
    }
    if (hwRender->devDataHandle == NULL) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam,
                                     AUDIO_DRV_PCM_IOCTRL_START);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRenderStart SetParams FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    char *buffer = (char *)calloc(1, FRAME_DATA);
    if (buffer == NULL) {
        LOG_FUN_ERR("Calloc Render buffer Fail!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwRender->renderParam.frameRenderMode.buffer = buffer;
    AudioLogRecord(INFO, "[%s]-[%s]-[%d] :> [%s]", __FILE__, __func__, __LINE__, "Audio Render Start");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderStop(AudioHandle handle)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        LOG_FUN_ERR("hwRender is invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->renderParam.frameRenderMode.buffer != NULL) {
        AudioMemFree((void **)&hwRender->renderParam.frameRenderMode.buffer);
    } else {
        LOG_FUN_ERR("Repeat invalid stop operation!");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    if (hwRender->devDataHandle == NULL) {
        LOG_FUN_ERR("RenderStart Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam,
                                     AUDIO_DRV_PCM_IOCTRL_STOP);
    hwRender->renderParam.renderMode.ctlParam.turnStandbyStatus = AUDIO_TURN_STANDBY_LATER;
    if (ret < 0) {
        LOG_FUN_ERR("AudioRenderStop SetParams FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioLogRecord(INFO, "[%s]-[%s]-[%d] :> [%s]", __FILE__, __func__, __LINE__, "Audio Render Stop");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderPause(AudioHandle handle)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        LOG_FUN_ERR("hwRender is null");
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
    if (hwRender->devDataHandle == NULL) {
        LOG_FUN_ERR("RenderPause Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    bool pauseStatus = hwRender->renderParam.renderMode.ctlParam.pause;
    hwRender->renderParam.renderMode.ctlParam.pause = true;
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam,
                                     AUDIODRV_CTL_IOCTL_PAUSE_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("RenderPause FAIL!");
        hwRender->renderParam.renderMode.ctlParam.pause = pauseStatus;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioLogRecord(INFO, "[%s]-[%s]-[%d] :> [%s]", __FILE__, __func__, __LINE__, "Audio Render Pause");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderResume(AudioHandle handle)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (!hwRender->renderParam.renderMode.ctlParam.pause) {
        LOG_FUN_ERR("Audio is already Resume !");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    if (hwRender->devDataHandle == NULL) {
        LOG_FUN_ERR("RenderResume Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    bool resumeStatus = hwRender->renderParam.renderMode.ctlParam.pause;
    hwRender->renderParam.renderMode.ctlParam.pause = false;
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam,
                                     AUDIODRV_CTL_IOCTL_PAUSE_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("RenderResume FAIL!");
        hwRender->renderParam.renderMode.ctlParam.pause = resumeStatus;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioLogRecord(INFO, "[%s]-[%s]-[%d] :> [%s]", __FILE__, __func__, __LINE__, "Audio Render Resume");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderFlush(AudioHandle handle)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    bool callBackStatus = hwRender->renderParam.renderMode.hwInfo.callBackEnable;
    if (callBackStatus) {
        return CallbackProcessing(hwRender, AUDIO_FLUSH_COMPLETED);
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioRenderGetFrameSize(AudioHandle handle, uint64_t *size)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || size == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t channelCount = hwRender->renderParam.frameRenderMode.attrs.channelCount;
    enum AudioFormat format = hwRender->renderParam.frameRenderMode.attrs.format;
    uint32_t formatBits = 0;
    ret = FormatToBits(format, &formatBits);
    if (ret != AUDIO_HAL_SUCCESS) {
        return ret;
    }
    *size = FRAME_SIZE * channelCount * (formatBits >> 3);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetFrameCount(AudioHandle handle, uint64_t *count)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || count == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    *count = hwRender->renderParam.frameRenderMode.frames;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetSampleAttributes(AudioHandle handle, const struct AudioSampleAttributes *attrs)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || attrs == NULL || hwRender->devDataHandle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioCheckParaAttr(attrs);
    if (ret != AUDIO_HAL_SUCCESS) {
        return ret;
    }
    /* attrs temp */
    struct AudioSampleAttributes tempAttrs = hwRender->renderParam.frameRenderMode.attrs;
    hwRender->renderParam.frameRenderMode.attrs.format = attrs->format;
    hwRender->renderParam.frameRenderMode.attrs.sampleRate = attrs->sampleRate;
    hwRender->renderParam.frameRenderMode.attrs.channelCount = attrs->channelCount;
    hwRender->renderParam.frameRenderMode.attrs.interleaved = attrs->interleaved;
    hwRender->renderParam.frameRenderMode.attrs.type = attrs->type;
    hwRender->renderParam.frameRenderMode.attrs.period = attrs->period;
    hwRender->renderParam.frameRenderMode.attrs.frameSize = attrs->frameSize;
    hwRender->renderParam.frameRenderMode.attrs.isBigEndian = attrs->isBigEndian;
    hwRender->renderParam.frameRenderMode.attrs.isSignedData = attrs->isSignedData;
    hwRender->renderParam.frameRenderMode.attrs.startThreshold = attrs->startThreshold;
    hwRender->renderParam.frameRenderMode.attrs.stopThreshold = attrs->stopThreshold;
    hwRender->renderParam.frameRenderMode.attrs.silenceThreshold = attrs->silenceThreshold;
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL || hwRender->devDataHandle == NULL) {
        hwRender->renderParam.frameRenderMode.attrs = tempAttrs;
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle,
                                             &hwRender->renderParam,
                                             AUDIO_DRV_PCM_IOCTL_HW_PARAMS);
    if (ret < 0) {
        LOG_FUN_ERR("SetSampleAttributes FAIL");
        hwRender->renderParam.frameRenderMode.attrs = tempAttrs;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetSampleAttributes(AudioHandle handle, struct AudioSampleAttributes *attrs)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
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
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
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
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || scene == NULL || supported == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    *supported = false;
    /* Temporary storage does not save the structure */
    struct AudioHwRenderParam renderParam;
    renderParam.frameRenderMode.attrs.type = (enum AudioCategory)scene->scene.id;
    renderParam.renderMode.hwInfo.deviceDescript.pins = scene->desc.pins;
    PathSelAnalysisJson *pPathSelAnalysisJson = AudioSoGetPathSelAnalysisJson();
    if (pPathSelAnalysisJson == NULL) {
        LOG_FUN_ERR("pPathSelAnalysisJson Is NULL!");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    ret = (*pPathSelAnalysisJson)((void *)&renderParam, CHECKSCENE_PATH_SELECT);
    if (ret < 0) {
        if (ret == AUDIO_HAL_ERR_NOT_SUPPORT) {
            LOG_FUN_ERR("AudioRenderCheckSceneCapability not Support!");
            return AUDIO_HAL_ERR_NOT_SUPPORT;
        } else {
            LOG_FUN_ERR("AudioRenderCheckSceneCapability fail!");
            return AUDIO_HAL_ERR_INTERNAL;
        }
    }
    *supported = true;
    return AUDIO_HAL_SUCCESS;
#else
    return AUDIO_HAL_ERR_NOT_SUPPORT;
#endif
}

int32_t AudioRenderSelectScene(AudioHandle handle, const struct AudioSceneDescriptor *scene)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || scene == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderSelectScene Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    PathSelAnalysisJson *pPathSelAnalysisJson = AudioSoGetPathSelAnalysisJson();
    if (pPathSelAnalysisJson == NULL) {
        LOG_FUN_ERR("pPathSelAnalysisJson Is NULL!");
        return AUDIO_HAL_ERR_NOT_SUPPORT;
    }
    enum AudioCategory sceneId = hwRender->renderParam.frameRenderMode.attrs.type;
    enum AudioPortPin descPins = hwRender->renderParam.renderMode.hwInfo.deviceDescript.pins;
    hwRender->renderParam.frameRenderMode.attrs.type = (enum AudioCategory)(scene->scene.id);
    hwRender->renderParam.renderMode.hwInfo.deviceDescript.pins = scene->desc.pins;
    if ((*pPathSelAnalysisJson)((void *)&hwRender->renderParam, RENDER_PATH_SELECT) < 0) {
        LOG_FUN_ERR("AudioRenderSelectScene Fail!");
        hwRender->renderParam.frameRenderMode.attrs.type = sceneId;
        hwRender->renderParam.renderMode.hwInfo.deviceDescript.pins = descPins;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        hwRender->renderParam.frameRenderMode.attrs.type = sceneId;
        hwRender->renderParam.renderMode.hwInfo.deviceDescript.pins = descPins;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devCtlHandle, &hwRender->renderParam,
                                     AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("SetParams FAIL!");
        hwRender->renderParam.frameRenderMode.attrs.type = sceneId;
        hwRender->renderParam.renderMode.hwInfo.deviceDescript.pins = descPins;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
#else
    return AUDIO_HAL_ERR_NOT_SUPPORT;
#endif
}

int32_t AudioRenderSetMute(AudioHandle handle, bool mute)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)handle;
    if (impl == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (impl->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderSetMute Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    bool muteStatus = impl->renderParam.renderMode.ctlParam.mute;
    impl->renderParam.renderMode.ctlParam.mute = mute;
    ret = (*pInterfaceLibModeRender)(impl->devCtlHandle, &impl->renderParam, AUDIODRV_CTL_IOCTL_MUTE_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("SetMute SetParams FAIL");
        impl->renderParam.renderMode.ctlParam.mute = muteStatus;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioLogRecord(INFO, "[%s]-[%s]-[%d] :> [Setmute = %d]", __FILE__, __func__, __LINE__, mute);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetMute(AudioHandle handle, bool *mute)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)handle;
    if (impl == NULL || mute == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    if (impl->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderGetMute Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(impl->devCtlHandle, &impl->renderParam, AUDIODRV_CTL_IOCTL_MUTE_READ);
    if (ret < 0) {
        LOG_FUN_ERR("Get Mute FAIL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *mute = impl->renderParam.renderMode.ctlParam.mute;
    LOG_PARA_INFO("GetMute SUCCESS!");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetVolume(AudioHandle handle, float volume)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    float volumeTemp = hwRender->renderParam.renderMode.ctlParam.volume;
    float volMax = (float)hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax;
    float volMin = (float)hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin;
    if (volume < 0 || volume > 1) {
        LOG_FUN_ERR("volume param Is error!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderSetVolume Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    volume = (volume == 0) ? 1 : (volume * VOLUME_CHANGE);
    /* change volume to db */
    float volTemp = ((volMax - volMin) / 2) * log10(volume) + volMin;
    if (volTemp < volMin || volTemp > volMax) {
        LOG_FUN_ERR("volTemp fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    hwRender->renderParam.renderMode.ctlParam.volume = volTemp;
    ret = (*pInterfaceLibModeRender)(hwRender->devCtlHandle, &hwRender->renderParam,
                                     AUDIODRV_CTL_IOCTL_ELEM_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSetVolume FAIL!");
        hwRender->renderParam.renderMode.ctlParam.volume = volumeTemp;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetVolume(AudioHandle handle, float *volume)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || volume == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderGetVolume Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devCtlHandle, &hwRender->renderParam, AUDIODRV_CTL_IOCTL_ELEM_READ);
    if (ret < 0) {
        LOG_FUN_ERR("RenderGetVolume FAIL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    float volumeTemp = hwRender->renderParam.renderMode.ctlParam.volume;
    float volMax = (float)hwRender->renderParam.renderMode.ctlParam.volThreshold.volMax;
    float volMin = (float)hwRender->renderParam.renderMode.ctlParam.volThreshold.volMin;
    if ((volMax - volMin) == 0) {
        LOG_FUN_ERR("Divisor cannot be zero!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    volumeTemp = (volumeTemp - volMin) / ((volMax - volMin) / 2);
    int volumeT = (int)((pow(10, volumeTemp) + 5) / 10); // delet 0.X num
    *volume = (float)volumeT / 10;  // get volume (0-1)
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetGainThreshold(AudioHandle handle, float *min, float *max)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || min == NULL || max == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderGetGainThreshold Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devCtlHandle, &hwRender->renderParam,
                                     AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ);
    if (ret < 0) {
        LOG_FUN_ERR("GetGainThreshold FAIL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *max = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMax;
    *min = hwRender->renderParam.renderMode.ctlParam.audioGain.gainMin;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetGain(AudioHandle handle, float *gain)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)handle;
    if (impl == NULL || gain == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (impl->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderGetGain Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(impl->devCtlHandle, &impl->renderParam, AUDIODRV_CTL_IOCTL_GAIN_READ);
    if (ret < 0) {
        LOG_FUN_ERR("RenderGetGain FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *gain = impl->renderParam.renderMode.ctlParam.audioGain.gain;
    LOG_PARA_INFO("RenderGetGain SUCCESS!");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderSetGain(AudioHandle handle, float gain)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)handle;
    if (impl == NULL || gain < 0) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    float gainTemp = impl->renderParam.renderMode.ctlParam.audioGain.gain;
    impl->renderParam.renderMode.ctlParam.audioGain.gain = gain;
    if (impl->devCtlHandle == NULL) {
        LOG_FUN_ERR("RenderSetGain Bind Fail!");
        impl->renderParam.renderMode.ctlParam.audioGain.gain = gainTemp;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        impl->renderParam.renderMode.ctlParam.audioGain.gain = gainTemp;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(impl->devCtlHandle, &impl->renderParam, AUDIODRV_CTL_IOCTL_GAIN_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSetGain FAIL");
        impl->renderParam.renderMode.ctlParam.audioGain.gain = gainTemp;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    LOG_PARA_INFO("RenderSetGain SUCCESS!");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetLatency(struct AudioRender *render, uint32_t *ms)
{
    int32_t ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)render;
    if (impl == NULL || ms == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t byteRate = impl->renderParam.frameRenderMode.byteRate;
    uint32_t periodSize = impl->renderParam.frameRenderMode.periodSize;
    uint32_t periodCount = impl->renderParam.frameRenderMode.periodCount;
    if (byteRate == 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t period_ms = (periodCount * periodSize * 1000) / byteRate;
    *ms = period_ms;
    return AUDIO_HAL_SUCCESS;
}

static int32_t LogErrorGetRensonAndTime(struct AudioHwRender *hwRender, int errorReason)
{
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->errorLog.iter >= ERROR_LOG_MAX_NUM) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwRender->errorLog.errorDump[hwRender->errorLog.iter].reason == NULL) {
        hwRender->errorLog.errorDump[hwRender->errorLog.iter].reason
            = (char *)calloc(1, ERROR_REASON_DESC_LEN);
        if (hwRender->errorLog.errorDump[hwRender->errorLog.iter].reason == NULL) {
            LOG_FUN_ERR("Calloc reasonDesc Fail!");
            return AUDIO_HAL_ERR_MALLOC_FAIL;
        }
    }
    if (hwRender->errorLog.errorDump[hwRender->errorLog.iter].currentTime == NULL) {
        hwRender->errorLog.errorDump[hwRender->errorLog.iter].currentTime
            = (char *)calloc(1, ERROR_REASON_DESC_LEN);
        if (hwRender->errorLog.errorDump[hwRender->errorLog.iter].currentTime == NULL) {
            LOG_FUN_ERR("Calloc time Fail!");
            return AUDIO_HAL_ERR_MALLOC_FAIL;
        }
    }
    (void)memset_s(hwRender->errorLog.errorDump[hwRender->errorLog.iter].reason,
        ERROR_REASON_DESC_LEN, 0, ERROR_REASON_DESC_LEN);
    (void)memset_s(hwRender->errorLog.errorDump[hwRender->errorLog.iter].currentTime,
        ERROR_REASON_DESC_LEN, 0, ERROR_REASON_DESC_LEN);
    int32_t ret = GetErrorReason(errorReason, hwRender->errorLog.errorDump[hwRender->errorLog.iter].reason);
    if (ret < 0) {
        LOG_FUN_ERR("GetErrorReason failed!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = GetCurrentTime(hwRender->errorLog.errorDump[hwRender->errorLog.iter].currentTime);
    if (ret < 0) {
        LOG_FUN_ERR("GetCurrentTime Fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

void LogError(AudioHandle handle, int32_t errorCode, int reason)
{
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL) {
        return;
    }
    hwRender->errorLog.totalErrors++;
    if (hwRender->errorLog.iter >= ERROR_LOG_MAX_NUM) {
        hwRender->errorLog.iter = 0;
    }
    int32_t ret = LogErrorGetRensonAndTime(hwRender, reason);
    if (ret < 0) {
        return;
    }
    if (errorCode == WRITE_FRAME_ERROR_CODE) {
        hwRender->errorLog.errorDump[hwRender->errorLog.iter].errorCode = errorCode;
        hwRender->errorLog.errorDump[hwRender->errorLog.iter].count = hwRender->errorLog.iter;
        hwRender->errorLog.errorDump[hwRender->errorLog.iter].frames = hwRender->renderParam.frameRenderMode.frames;
        hwRender->errorLog.iter++;
    }
}

int32_t AudioRenderRenderFramSplit(struct AudioHwRender *hwRender)
{
    int32_t ret;
    if (hwRender == NULL) {
        return HDF_FAILURE;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return HDF_FAILURE;
    }
    if (hwRender->devDataHandle == NULL) {
        return HDF_FAILURE;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam,
                                             AUDIO_DRV_PCM_IOCTL_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("Render Frame FAIL!");
        LogError((AudioHandle)hwRender, WRITE_FRAME_ERROR_CODE, ret);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return HDF_SUCCESS;
}

int32_t AudioRenderRenderFrame(struct AudioRender *render, const void *frame,
                               uint64_t requestBytes, uint64_t *replyBytes)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    if (hwRender == NULL || frame == NULL || replyBytes == NULL ||
        hwRender->renderParam.frameRenderMode.buffer == NULL) {
        LOG_FUN_ERR("Render Frame Paras is NULL!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (FRAME_DATA < requestBytes) {
        LOG_FUN_ERR("Out of FRAME_DATA size!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = memcpy_s(hwRender->renderParam.frameRenderMode.buffer, FRAME_DATA, frame, (uint32_t)requestBytes);
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
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetRenderPosition(struct AudioRender *render, uint64_t *frames, struct AudioTimeStamp *time)
{
    int32_t ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)render;
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
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioRenderGetRenderSpeed(struct AudioRender *render, float *speed)
{
    LOG_FUN_INFO();
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    if (hwRender == NULL || speed == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioRenderSetChannelMode(struct AudioRender *render, enum AudioChannelMode mode)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)render;
    if (impl == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (impl->devCtlHandle == NULL) {
        LOG_FUN_ERR("Bind Fail!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    enum AudioChannelMode tempMode = impl->renderParam.frameRenderMode.mode;
    impl->renderParam.frameRenderMode.mode = mode;
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        impl->renderParam.frameRenderMode.mode = tempMode;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(impl->devCtlHandle, &impl->renderParam,
                                     AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE);
    if (ret < 0) {
        LOG_FUN_ERR("Set ChannelMode FAIL!");
        impl->renderParam.frameRenderMode.mode = tempMode;
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetChannelMode(struct AudioRender *render, enum AudioChannelMode *mode)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *impl = (struct AudioHwRender *)render;
    if (impl == NULL || mode == NULL || impl->devCtlHandle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(impl->devCtlHandle, &impl->renderParam, AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ);
    if (ret < 0) {
        LOG_FUN_ERR("Get ChannelMode FAIL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *mode = impl->renderParam.frameRenderMode.mode;
    return AUDIO_HAL_SUCCESS;
}

int32_t SetValue(struct ExtraParams mExtraParams, struct AudioHwRender *render)
{
    if (render == NULL) {
        return HDF_FAILURE;
    }
    if (mExtraParams.route != -1) {
        render->renderParam.renderMode.hwInfo.pathroute = mExtraParams.route;
    }
    if (mExtraParams.format != -1) {
        render->renderParam.frameRenderMode.attrs.format = mExtraParams.format;
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
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
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
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL || keyValueList == NULL || listLenth <= 0) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t bufferSize = strlen(ROUTE_SAMPLE) + strlen(FORMAT_SAMPLE) + strlen(CHANNELS_SAMPLE)
                    + strlen(FRAME_COUNT_SAMPLE) + strlen(SAMPLING_RATE_SAMPLE) + 1;
    if (listLenth < bufferSize) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AddElementToList(keyValueList, listLenth, AUDIO_ATTR_PARAM_ROUTE,
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

int32_t AudioRenderReqMmapBufferInit(struct AudioHwRender *render,
    int32_t reqSize, struct AudioMmapBufferDescripter *desc)
{
    if (render == NULL || render->devDataHandle == NULL || desc == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t formatBits = 0;
    int32_t ret = FormatToBits(render->renderParam.frameRenderMode.attrs.format, &formatBits);
    if (ret < 0) {
        return ret;
    }
    int32_t flags;
    if (desc->isShareable) {
        flags = MAP_SHARED;
    } else {
        flags = MAP_PRIVATE;
    }
    int64_t filesize = lseek(desc->memoryFd, 0, SEEK_END);
    if ((int64_t)reqSize > filesize) {
        LOG_FUN_ERR("reqSize is out of file Size!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    desc->memoryAddress = mmap(NULL, reqSize, PROT_READ | PROT_WRITE, flags, desc->memoryFd, 0);
    if (desc->memoryAddress == NULL || desc->memoryAddress == (void *)(-1)) {
        LOG_FUN_ERR("AudioRenderReqMmapBuffer mmap FAIL and errno is:%d !", errno);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    desc->totalBufferFrames = reqSize / (render->renderParam.frameRenderMode.attrs.channelCount * (formatBits >> 3));
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        munmap(desc->memoryAddress, reqSize);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    render->renderParam.frameRenderMode.mmapBufDesc.memoryAddress = desc->memoryAddress;
    render->renderParam.frameRenderMode.mmapBufDesc.memoryFd = desc->memoryFd;
    render->renderParam.frameRenderMode.mmapBufDesc.totalBufferFrames = desc->totalBufferFrames;
    render->renderParam.frameRenderMode.mmapBufDesc.transferFrameSize = desc->transferFrameSize;
    render->renderParam.frameRenderMode.mmapBufDesc.isShareable = desc->isShareable;
    render->renderParam.frameRenderMode.mmapBufDesc.offset = desc->offset;
    ret = (*pInterfaceLibModeRender)(render->devDataHandle, &render->renderParam, AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRenderReqMmapBuffer FAIL!");
        munmap(desc->memoryAddress, reqSize);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderReqMmapBuffer(AudioHandle handle, int32_t reqSize, struct AudioMmapBufferDescripter *desc)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL || render->devDataHandle == NULL || desc == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    ret = AudioRenderReqMmapBufferInit(render, reqSize, desc);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRenderReqMmapBufferInit failed!");
        return ret;
    }
    LOG_PARA_INFO("AudioRenderReqMmapBuffer Success!");
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderGetMmapPosition(AudioHandle handle, uint64_t *frames, struct AudioTimeStamp *time)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL || frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
#ifndef AUDIO_HAL_USER
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("pInterfaceLibModeRender Is NULL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (render->devDataHandle == NULL) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = (*pInterfaceLibModeRender)(render->devDataHandle,
                                     &render->renderParam, AUDIO_DRV_PCM_IOCTL_MMAP_POSITION);
    if (ret < 0) {
        LOG_FUN_ERR("Get Position FAIL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
#endif
    *frames = render->renderParam.frameRenderMode.frames;
    render->renderParam.frameRenderMode.time.tvSec = (int64_t)(render->renderParam.frameRenderMode.frames /
        render->renderParam.frameRenderMode.attrs.sampleRate);
    uint64_t lastBufFrames = render->renderParam.frameRenderMode.frames %
                        render->renderParam.frameRenderMode.attrs.sampleRate;
    render->renderParam.frameRenderMode.time.tvNSec =
        (int64_t)((lastBufFrames * SEC_TO_NSEC) / render->renderParam.frameRenderMode.attrs.sampleRate);
    *time = render->renderParam.frameRenderMode.time;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderTurnStandbyMode(AudioHandle handle)
{
    struct AudioHwRender *render = (struct AudioHwRender *)handle;
    if (render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    render->renderParam.renderMode.ctlParam.turnStandbyStatus = AUDIO_TURN_STANDBY_NOW;
    int32_t ret = AudioRenderStop((AudioHandle)render);
    if (ret < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderAudioDevDump(AudioHandle handle, int32_t range, int32_t fd)
{
    int32_t ret = AudioCheckRenderAddr(handle);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
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
int32_t CallbackProcessing(AudioHandle handle, enum AudioCallbackType callBackType)
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
    int32_t ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwRender *pRender = (struct AudioHwRender *)render;
    if (pRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    pRender->renderParam.frameRenderMode.callback = callback;
    pRender->renderParam.frameRenderMode.cookie = cookie;
    pRender->renderParam.renderMode.hwInfo.callBackEnable = true;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioRenderDrainBuffer(struct AudioRender *render, enum AudioDrainNotifyType *type)
{
    struct AudioHwRender *pRender = (struct AudioHwRender *)render;
    if (pRender == NULL || type == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}
