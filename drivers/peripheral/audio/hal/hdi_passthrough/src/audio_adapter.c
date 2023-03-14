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

#include "audio_adapter.h"
#include "audio_adapter_info_common.h"
#include "audio_hal_log.h"
#include "audio_interface_lib_capture.h"
#include "audio_interface_lib_render.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_IMPL

#define CONFIG_CHANNEL_COUNT  2 // two channels
#define GAIN_MAX 50.0
#define AUDIO_SERVICE_PORTID_FLAG 10
#define DEFAULT_RENDER_SAMPLING_RATE 48000
#define DEEP_BUFFER_RENDER_PERIOD_SIZE 4096
#define DEEP_BUFFER_RENDER_PERIOD_COUNT 8
#define TYPE_RENDER "Render"
#define TYPE_CAPTURE "Capture"
#define INT_32_MAX 0x7fffffff
#define SHIFT_RIGHT_31_BITS 31

int32_t GetAudioRenderFunc(struct AudioHwRender *hwRender)
{
    if (hwRender == NULL) {
        return HDF_FAILURE;
    }
    hwRender->common.control.Start = AudioRenderStart;
    hwRender->common.control.Stop = AudioRenderStop;
    hwRender->common.control.Pause = AudioRenderPause;
    hwRender->common.control.Resume = AudioRenderResume;
    hwRender->common.control.Flush = AudioRenderFlush;
    hwRender->common.control.TurnStandbyMode = AudioRenderTurnStandbyMode;
    hwRender->common.control.AudioDevDump = AudioRenderAudioDevDump;
    hwRender->common.attr.GetFrameSize = AudioRenderGetFrameSize;
    hwRender->common.attr.GetFrameCount = AudioRenderGetFrameCount;
    hwRender->common.attr.SetSampleAttributes = AudioRenderSetSampleAttributes;
    hwRender->common.attr.GetSampleAttributes = AudioRenderGetSampleAttributes;
    hwRender->common.attr.GetCurrentChannelId = AudioRenderGetCurrentChannelId;
    hwRender->common.attr.SetExtraParams = AudioRenderSetExtraParams;
    hwRender->common.attr.GetExtraParams = AudioRenderGetExtraParams;
    hwRender->common.attr.ReqMmapBuffer = AudioRenderReqMmapBuffer;
    hwRender->common.attr.GetMmapPosition = AudioRenderGetMmapPosition;
    hwRender->common.scene.CheckSceneCapability = AudioRenderCheckSceneCapability;
    hwRender->common.scene.SelectScene = AudioRenderSelectScene;
    hwRender->common.volume.SetMute = AudioRenderSetMute;
    hwRender->common.volume.GetMute = AudioRenderGetMute;
    hwRender->common.volume.SetVolume = AudioRenderSetVolume;
    hwRender->common.volume.GetVolume = AudioRenderGetVolume;
    hwRender->common.volume.GetGainThreshold = AudioRenderGetGainThreshold;
    hwRender->common.volume.GetGain = AudioRenderGetGain;
    hwRender->common.volume.SetGain = AudioRenderSetGain;
    hwRender->common.GetLatency = AudioRenderGetLatency;
    hwRender->common.RenderFrame = AudioRenderRenderFrame;
    hwRender->common.GetRenderPosition = AudioRenderGetRenderPosition;
    hwRender->common.SetRenderSpeed = AudioRenderSetRenderSpeed;
    hwRender->common.GetRenderSpeed = AudioRenderGetRenderSpeed;
    hwRender->common.SetChannelMode = AudioRenderSetChannelMode;
    hwRender->common.GetChannelMode = AudioRenderGetChannelMode;
    hwRender->common.RegCallback = AudioRenderRegCallback;
    hwRender->common.DrainBuffer = AudioRenderDrainBuffer;
    hwRender->renderParam.frameRenderMode.callbackProcess = CallbackProcessing;
    return HDF_SUCCESS;
}

int32_t CheckParaDesc(const struct AudioDeviceDescriptor *desc, const char *type)
{
    if (desc == NULL || type == NULL) {
        return HDF_FAILURE;
    }
    if ((desc->portId) >> SHIFT_RIGHT_31_BITS) {
        return HDF_ERR_NOT_SUPPORT;
    }
    enum AudioPortPin pins = desc->pins;
    if (!strcmp(type, TYPE_CAPTURE)) {
        if (pins == PIN_IN_MIC || pins == PIN_IN_HS_MIC || pins == PIN_IN_LINEIN) {
            return HDF_SUCCESS;
        } else {
            return HDF_ERR_NOT_SUPPORT;
        }
    } else if (!strcmp(type, TYPE_RENDER)) {
        if (pins == PIN_OUT_SPEAKER || pins == PIN_OUT_HEADSET || pins == PIN_OUT_LINEOUT || pins == PIN_OUT_HDMI) {
            return HDF_SUCCESS;
        } else {
            return HDF_ERR_NOT_SUPPORT;
        }
    }
    return HDF_ERR_NOT_SUPPORT;
}

int32_t CheckParaAttr(const struct AudioSampleAttributes *attrs)
{
    if (attrs == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret = ((attrs->sampleRate) >> SHIFT_RIGHT_31_BITS) + ((attrs->channelCount) >> SHIFT_RIGHT_31_BITS) +
        ((attrs->period) >> SHIFT_RIGHT_31_BITS) + ((attrs->frameSize) >> SHIFT_RIGHT_31_BITS) +
        ((attrs->startThreshold) >> SHIFT_RIGHT_31_BITS) + ((attrs->stopThreshold) >> SHIFT_RIGHT_31_BITS) +
        ((attrs->silenceThreshold) >> SHIFT_RIGHT_31_BITS);
    if (ret > 0) {
        return HDF_ERR_NOT_SUPPORT;
    }
    enum AudioCategory audioCategory = attrs->type;
    if (AUDIO_IN_MEDIA != audioCategory && AUDIO_IN_COMMUNICATION != audioCategory) {
        return HDF_ERR_NOT_SUPPORT;
    }
    enum AudioFormat audioFormat = attrs->format;
    return CheckAttrFormat(audioFormat);
}

int32_t AttrFormatToBit(const struct AudioSampleAttributes *attrs, int32_t *format)
{
    if (attrs == NULL || format == NULL) {
        return HDF_FAILURE;
    }
    enum AudioFormat audioFormat = attrs->format;
    switch (audioFormat) {
        case AUDIO_FORMAT_PCM_8_BIT:
            *format = BIT_NUM_8;
            return HDF_SUCCESS;
        case AUDIO_FORMAT_PCM_16_BIT:
            *format = BIT_NUM_16;
            return HDF_SUCCESS;
        case AUDIO_FORMAT_PCM_24_BIT:
            *format = BIT_NUM_24;
            return HDF_SUCCESS;
        case AUDIO_FORMAT_PCM_32_BIT:
            *format = BIT_NUM_32;
            return HDF_SUCCESS;
        default:
            return HDF_ERR_NOT_SUPPORT;
    }
}

int32_t InitHwRenderParam(struct AudioHwRender *hwRender, const struct AudioDeviceDescriptor *desc,
                          const struct AudioSampleAttributes *attrs)
{
    if (hwRender == NULL || desc == NULL || attrs == NULL) {
        LOG_FUN_ERR("InitHwRenderParam param Is NULL");
        return HDF_FAILURE;
    }
    int32_t ret = CheckParaDesc(desc, TYPE_RENDER);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CheckParaDesc Fail");
        return ret;
    }
    ret = CheckParaAttr(attrs);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CheckParaAttr Fail");
        return ret;
    }
    int32_t formatValue = -1;
    ret = AttrFormatToBit(attrs, &formatValue);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("AttrFormatToBit Fail");
        return ret;
    }
    if (attrs->channelCount == 0) {
        return HDF_FAILURE;
    }
    hwRender->renderParam.renderMode.hwInfo.deviceDescript = *desc;
    hwRender->renderParam.renderMode.hwInfo.callBackEnable = false;
    hwRender->renderParam.frameRenderMode.attrs = *attrs;
    hwRender->renderParam.renderMode.ctlParam.audioGain.gainMax = GAIN_MAX;  // init gainMax
    hwRender->renderParam.renderMode.ctlParam.audioGain.gainMin = 0;
    hwRender->renderParam.frameRenderMode.frames = 0;
    hwRender->renderParam.frameRenderMode.time.tvNSec = 0;
    hwRender->renderParam.frameRenderMode.time.tvSec = 0;
    hwRender->renderParam.frameRenderMode.byteRate = DEFAULT_RENDER_SAMPLING_RATE;
    hwRender->renderParam.frameRenderMode.periodSize = DEEP_BUFFER_RENDER_PERIOD_SIZE;
    hwRender->renderParam.frameRenderMode.periodCount = DEEP_BUFFER_RENDER_PERIOD_COUNT;
    hwRender->renderParam.frameRenderMode.attrs.period = attrs->period;
    hwRender->renderParam.frameRenderMode.attrs.frameSize = attrs->frameSize;
    hwRender->renderParam.frameRenderMode.attrs.startThreshold = attrs->startThreshold;
    hwRender->renderParam.frameRenderMode.attrs.stopThreshold = attrs->stopThreshold;
    hwRender->renderParam.frameRenderMode.attrs.silenceThreshold = attrs->silenceThreshold;
    hwRender->renderParam.frameRenderMode.attrs.isBigEndian = attrs->isBigEndian;
    hwRender->renderParam.frameRenderMode.attrs.isSignedData = attrs->isSignedData;
    hwRender->renderParam.frameRenderMode.renderhandle = (AudioHandle)hwRender;
    hwRender->renderParam.renderMode.ctlParam.turnStandbyStatus = AUDIO_TURN_STANDBY_LATER;
    return HDF_SUCCESS;
}

enum AudioFormat g_formatIdZero = AUDIO_FORMAT_PCM_16_BIT;
int32_t InitForGetPortCapability(struct AudioPort portIndex, struct AudioPortCapability *capabilityIndex)
{
    if (capabilityIndex == NULL) {
        LOG_FUN_ERR("capabilityIndex Is NULL");
        return HDF_FAILURE;
    }
    /* get capabilityIndex from driver or default */
    if (portIndex.dir != PORT_OUT) {
        capabilityIndex->hardwareMode = true;
        capabilityIndex->channelMasks = AUDIO_CHANNEL_STEREO;
        capabilityIndex->channelCount = CONFIG_CHANNEL_COUNT;
        return HDF_SUCCESS;
    }
    if (portIndex.portId == 0) {
        capabilityIndex->hardwareMode = true;
        capabilityIndex->channelMasks = AUDIO_CHANNEL_STEREO;
        capabilityIndex->channelCount = CONFIG_CHANNEL_COUNT;
        capabilityIndex->deviceType = portIndex.dir;
        capabilityIndex->deviceId = PIN_OUT_SPEAKER;
        capabilityIndex->formatNum = 1;
        capabilityIndex->formats = &g_formatIdZero;
        capabilityIndex->sampleRateMasks = AUDIO_SAMPLE_RATE_MASK_16000;
        capabilityIndex->subPortsNum = 1;
        capabilityIndex->subPorts = (struct AudioSubPortCapability *)calloc(capabilityIndex->subPortsNum,
                                                                            sizeof(struct AudioSubPortCapability));
        if (capabilityIndex->subPorts == NULL) {
            LOG_FUN_ERR("capabilityIndex subPorts is NULL!");
            return HDF_FAILURE;
        }
        capabilityIndex->subPorts->portId = portIndex.portId;
        capabilityIndex->subPorts->desc = portIndex.portName;
        capabilityIndex->subPorts->mask = PORT_PASSTHROUGH_LPCM;
        return HDF_SUCCESS;
    }
    if (portIndex.portId == 1) {
        capabilityIndex->hardwareMode = true;
        capabilityIndex->channelMasks = AUDIO_CHANNEL_STEREO;
        capabilityIndex->channelCount = CONFIG_CHANNEL_COUNT;
        capabilityIndex->deviceType = portIndex.dir;
        capabilityIndex->deviceId = PIN_OUT_HEADSET;
        capabilityIndex->formatNum = 1;
        capabilityIndex->formats = &g_formatIdZero;
        capabilityIndex->sampleRateMasks = AUDIO_SAMPLE_RATE_MASK_16000 | AUDIO_SAMPLE_RATE_MASK_8000;
        return HDF_SUCCESS;
    }
    if (portIndex.portId == HDMI_PORT_ID) {
        return HdmiPortInit(portIndex, capabilityIndex);
    }
    return HDF_FAILURE;
}

void AudioAdapterReleaseCapSubPorts(const struct AudioPortAndCapability *portCapabilitys, int32_t num)
{
    int32_t i = 0;
    if (portCapabilitys == NULL) {
        return;
    }
    while (i < num) {
        if (&portCapabilitys[i] == NULL) {
            break;
        }
        AudioMemFree((void **)(&portCapabilitys[i].capability.subPorts));
        i++;
    }
    return;
}

int32_t AudioAdapterInitAllPorts(struct AudioAdapter *adapter)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL) {
        LOG_FUN_ERR("hwAdapter Is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwAdapter->portCapabilitys != NULL) {
        LOG_PARA_INFO("portCapabilitys already Init!");
        return AUDIO_HAL_SUCCESS;
    }
    uint32_t portNum = hwAdapter->adapterDescriptor.portNum;
    struct AudioPort *ports = hwAdapter->adapterDescriptor.ports;
    if (ports == NULL) {
        LOG_FUN_ERR("ports is NULL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (portNum == 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioPortAndCapability *portCapability =
        (struct AudioPortAndCapability *)calloc(portNum, sizeof(struct AudioPortAndCapability));
    if (portCapability == NULL) {
        LOG_FUN_ERR("portCapability is NULL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    for (uint32_t i = 0; i < portNum; i++) {
        portCapability[i].port = ports[i];
        if (InitForGetPortCapability(ports[i], &portCapability[i].capability)) {
            LOG_FUN_ERR("ports Init Fail!");
            AudioAdapterReleaseCapSubPorts(portCapability, portNum);
            AudioMemFree((void **)&portCapability);
            return AUDIO_HAL_ERR_INTERNAL;
        }
    }
    hwAdapter->portCapabilitys = portCapability;
    hwAdapter->portCapabilitys->mode = PORT_PASSTHROUGH_LPCM;
    return AUDIO_HAL_SUCCESS;
}

void AudioReleaseRenderHandle(struct AudioHwRender *hwRender)
{
    if (hwRender == NULL) {
        return;
    }
    CloseServiceRenderSo *pCloseServiceRender = AudioSoGetCloseServiceRender();
    if (pCloseServiceRender == NULL || (*pCloseServiceRender) == NULL) {
        LOG_FUN_ERR("pCloseServiceRender func not exist");
        return;
    }
    if (hwRender->devDataHandle != NULL) {
        (*pCloseServiceRender)(hwRender->devDataHandle);
        hwRender->devDataHandle = NULL;
    }
    if (hwRender->devCtlHandle != NULL) {
        (*pCloseServiceRender)(hwRender->devCtlHandle);
        hwRender->devCtlHandle = NULL;
    }
    return;
}

int32_t AudioSetAcodeModeRender(struct AudioHwRender *hwRender,
    const InterfaceLibModeRenderSo *pInterfaceLibMode)
{
    LOG_FUN_INFO();
    if (hwRender == NULL || pInterfaceLibMode == NULL || hwRender->devCtlHandle == NULL) {
        return HDF_FAILURE;
    }
    if (hwRender->renderParam.renderMode.hwInfo.deviceDescript.portId < AUDIO_SERVICE_PORTID_FLAG) {
        return(*pInterfaceLibMode)(hwRender->devCtlHandle, &hwRender->renderParam,
                                         AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN);
    } else {
        return(*pInterfaceLibMode)(hwRender->devCtlHandle, &hwRender->renderParam,
                                         AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_OUT);
    }
}

int32_t AudioAdapterCreateRenderPre(struct AudioHwRender *hwRender, const struct AudioDeviceDescriptor *desc,
                                    const struct AudioSampleAttributes *attrs, const struct AudioHwAdapter *hwAdapter)
{
    LOG_FUN_INFO();
    if (hwAdapter == NULL || hwRender == NULL || desc == NULL || attrs == NULL) {
        LOG_FUN_ERR("Pointer is null!");
        return HDF_FAILURE;
    }

#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    PathSelAnalysisJson *pPathSelAnalysisJson = AudioSoGetPathSelAnalysisJson();
    if (pPathSelAnalysisJson == NULL || *pPathSelAnalysisJson == NULL) {
        LOG_FUN_ERR("PathSelAnalysisJson not exist");
        return HDF_FAILURE;
    }
#endif
    if (GetAudioRenderFunc(hwRender) < 0) {
        return HDF_FAILURE;
    }
    /* Fill hwRender para */
    if (InitHwRenderParam(hwRender, desc, attrs) < 0) {
        return HDF_FAILURE;
    }
    /* Select Path */
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    if ((*pPathSelAnalysisJson)((void *)&hwRender->renderParam, RENDER_PATH_SELECT) < 0) {
        LOG_FUN_ERR("Path Select Fail!");
        return HDF_FAILURE;
    }
#endif
    if (hwAdapter->adapterDescriptor.adapterName == NULL) {
        LOG_FUN_ERR("pointer is null!");
        return HDF_FAILURE;
    }
    uint32_t adapterNameLen = strlen(hwAdapter->adapterDescriptor.adapterName);
    if (adapterNameLen == 0) {
        LOG_FUN_ERR("adapterNameLen is null!");
        return HDF_FAILURE;
    }
    /* Get Adapter name */
    int32_t ret = strncpy_s(hwRender->renderParam.renderMode.hwInfo.adapterName, NAME_LEN,
                            hwAdapter->adapterDescriptor.adapterName, adapterNameLen);
    if (ret != EOK) {
        LOG_FUN_ERR("copy fail");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}


int32_t BindServiceRenderOpen(struct AudioHwRender *hwRender,
    InterfaceLibModeRenderSo *pInterfaceLibModeRender)
{
    if (hwRender == NULL || hwRender->devDataHandle == NULL ||
        pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("Input para is null!");
        return HDF_FAILURE;
    }
    /* render open */
    if (hwRender->renderParam.renderMode.hwInfo.deviceDescript.portId < AUDIO_SERVICE_PORTID_FLAG) {
        hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_IN;
    } else {
        hwRender->renderParam.renderMode.hwInfo.card = AUDIO_SERVICE_OUT;
    }
    int32_t ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle,
        &hwRender->renderParam, AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRender render open FAIL");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioAdapterBindServiceRender(struct AudioHwRender *hwRender)
{
    int32_t ret;
    if (hwRender == NULL || hwRender->devDataHandle == NULL || hwRender->devCtlHandle == NULL) {
        return HDF_FAILURE;
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("InterfaceLibModeRender not exist");
        return HDF_FAILURE;
    }
    if (BindServiceRenderOpen(hwRender, pInterfaceLibModeRender)) {
        return HDF_FAILURE;
    }
#ifndef AUDIO_HAL_USER
    /* Select Codec Mode */
    ret = AudioSetAcodeModeRender(hwRender, pInterfaceLibModeRender);
    if (ret < 0) {
        LOG_FUN_ERR("Select Codec Mode FAIL!");
        return HDF_FAILURE;
    }
#endif
    /* Init RenderPathSelect send first */
    /* portId small than  AUDIO_SERVICE_PORTID_FLAG should SceneSelect */
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    if (hwRender->renderParam.renderMode.hwInfo.deviceDescript.portId < AUDIO_SERVICE_PORTID_FLAG) {
        ret = (*pInterfaceLibModeRender)(hwRender->devCtlHandle, &hwRender->renderParam,
            AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE);
        if (ret < 0) {
            LOG_FUN_ERR("SetParams FAIL!");
            return HDF_FAILURE;
        }
    }
#endif
    /* set Attr Para */
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam, AUDIO_DRV_PCM_IOCTL_HW_PARAMS);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRender SetParams FAIL");
        return HDF_FAILURE;
    }
    /* get volThreshold */
    ret = (*pInterfaceLibModeRender)(hwRender->devCtlHandle, &hwRender->renderParam,
           AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ);
    if (ret < 0) {
        LOG_FUN_ERR("SetParams FAIL!");
        return HDF_FAILURE;
    }
#ifndef AUDIO_HAL_USER
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam, AUDIO_DRV_PCM_IOCTL_PREPARE);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRender perpare FAIL");
        return HDF_FAILURE;
    }
#endif
    return HDF_SUCCESS;
}

int32_t AudioRenderBindService(struct AudioHwRender *hwRender, BindServiceRenderSo *pBindServiceRender)
{
    if (hwRender == NULL || pBindServiceRender == NULL || *pBindServiceRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    /* bindRenderService */
    hwRender->devDataHandle = (*pBindServiceRender)(RENDER_CMD);
    if (hwRender->devDataHandle == NULL) {
        LOG_FUN_ERR("Render bind service failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    hwRender->devCtlHandle = (*pBindServiceRender)(CTRL_CMD);
    if (hwRender->devCtlHandle == NULL) {
        LOG_FUN_ERR("Render bind service failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioAdapterBindServiceRender(hwRender);
    if (ret != 0) {
        LOG_FUN_ERR("AudioAdapterBindServiceRender fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioAdapterCreateRender(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
                                 const struct AudioSampleAttributes *attrs, struct AudioRender **render)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || desc == NULL || attrs == NULL || render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwAdapter->adapterMgrRenderFlag > 0) {
        LOG_FUN_ERR("Create render repeatedly!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    BindServiceRenderSo *pBindServiceRender = AudioSoGetBindServiceRender();
    if (pBindServiceRender == NULL || *pBindServiceRender == NULL) {
        LOG_FUN_ERR("lib render func not exist");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    if (hwRender == NULL) {
        LOG_FUN_ERR("hwRender is NULL!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    ret = AudioAdapterCreateRenderPre(hwRender, desc, attrs, hwAdapter);
    if (ret != 0) {
        LOG_FUN_ERR("AudioAdapterCreateRenderPre fail");
        AudioMemFree((void **)&hwRender);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioRenderBindService(hwRender, pBindServiceRender);
    if (ret < 0) {
        LOG_FUN_ERR("AudioRenderBindService fail");
        AudioReleaseRenderHandle(hwRender);
        AudioMemFree((void **)&hwRender);
        return ret;
    }
    /* add for Fuzz */
    ret = AudioAddRenderAddrToList((AudioHandle)(&hwRender->common));
    if (ret < 0) {
        LOG_FUN_ERR("The render address get is invalid");
        AudioReleaseRenderHandle(hwRender);
        AudioMemFree((void **)&hwRender);
        return ret;
    }
    *render = &hwRender->common;
    hwAdapter->adapterMgrRenderFlag++;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioAdapterDestroyRender(struct AudioAdapter *adapter, struct AudioRender *render)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The render address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwAdapter->adapterMgrRenderFlag > 0) {
        hwAdapter->adapterMgrRenderFlag--;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (hwRender->renderParam.frameRenderMode.buffer != NULL) {
        int ret = render->control.Stop((AudioHandle)render);
        if (ret < 0) {
            LOG_FUN_ERR("render Stop failed");
        }
    }
    InterfaceLibModeRenderSo *pInterfaceLibModeRender = AudioSoGetInterfaceLibModeRender();
    if (pInterfaceLibModeRender == NULL || *pInterfaceLibModeRender == NULL) {
        LOG_FUN_ERR("InterfaceLibModeRender not exist");
        return HDF_FAILURE;
    }
    ret = (*pInterfaceLibModeRender)(hwRender->devDataHandle, &hwRender->renderParam,
                                     AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE);
    if (ret < 0) {
        LOG_FUN_ERR("Audio RENDER_CLOSE FAIL");
    }
    if (AudioDelRenderAddrFromList((AudioHandle)render)) {
        LOG_FUN_ERR("adapter or render not in MgrList");
    }
    AudioReleaseRenderHandle(hwRender);
    AudioMemFree((void **)&hwRender->renderParam.frameRenderMode.buffer);
    for (int i = 0; i < ERROR_LOG_MAX_NUM; i++) {
        AudioMemFree((void **)&hwRender->errorLog.errorDump[i].reason);
        AudioMemFree((void **)&hwRender->errorLog.errorDump[i].currentTime);
    }
    AudioMemFree((void **)&render);
    return AUDIO_HAL_SUCCESS;
}

int32_t GetAudioCaptureFunc(struct AudioHwCapture *hwCapture)
{
    if (hwCapture == NULL) {
        return HDF_FAILURE;
    }
    hwCapture->common.control.Start = AudioCaptureStart;
    hwCapture->common.control.Stop = AudioCaptureStop;
    hwCapture->common.control.Pause = AudioCapturePause;
    hwCapture->common.control.Resume = AudioCaptureResume;
    hwCapture->common.control.Flush = AudioCaptureFlush;
    hwCapture->common.control.TurnStandbyMode = AudioCaptureTurnStandbyMode;
    hwCapture->common.control.AudioDevDump = AudioCaptureAudioDevDump;
    hwCapture->common.attr.GetFrameSize = AudioCaptureGetFrameSize;
    hwCapture->common.attr.GetFrameCount = AudioCaptureGetFrameCount;
    hwCapture->common.attr.SetSampleAttributes = AudioCaptureSetSampleAttributes;
    hwCapture->common.attr.GetSampleAttributes = AudioCaptureGetSampleAttributes;
    hwCapture->common.attr.GetCurrentChannelId = AudioCaptureGetCurrentChannelId;
    hwCapture->common.attr.SetExtraParams = AudioCaptureSetExtraParams;
    hwCapture->common.attr.GetExtraParams = AudioCaptureGetExtraParams;
    hwCapture->common.attr.ReqMmapBuffer = AudioCaptureReqMmapBuffer;
    hwCapture->common.attr.GetMmapPosition = AudioCaptureGetMmapPosition;
    hwCapture->common.scene.CheckSceneCapability = AudioCaptureCheckSceneCapability;
    hwCapture->common.scene.SelectScene = AudioCaptureSelectScene;
    hwCapture->common.volume.SetMute = AudioCaptureSetMute;
    hwCapture->common.volume.GetMute = AudioCaptureGetMute;
    hwCapture->common.volume.SetVolume = AudioCaptureSetVolume;
    hwCapture->common.volume.GetVolume = AudioCaptureGetVolume;
    hwCapture->common.volume.GetGainThreshold = AudioCaptureGetGainThreshold;
    hwCapture->common.volume.GetGain = AudioCaptureGetGain;
    hwCapture->common.volume.SetGain = AudioCaptureSetGain;
    hwCapture->common.CaptureFrame = AudioCaptureCaptureFrame;
    hwCapture->common.GetCapturePosition = AudioCaptureGetCapturePosition;
    return HDF_SUCCESS;
}

int32_t InitHwCaptureParam(struct AudioHwCapture *hwCapture, const struct AudioDeviceDescriptor *desc,
                           const struct AudioSampleAttributes *attrs)
{
    if (hwCapture == NULL || desc == NULL || attrs == NULL) {
        LOG_FUN_ERR("InitHwCaptureParam param Is NULL");
        return HDF_FAILURE;
    }
    int32_t ret = CheckParaDesc(desc, TYPE_CAPTURE);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    ret = CheckParaAttr(attrs);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    int32_t formatValue = -1;
    ret = AttrFormatToBit(attrs, &formatValue);
    if (ret != HDF_SUCCESS) {
        return ret;
    }
    if (attrs->channelCount == 0) {
        return HDF_FAILURE;
    }
    hwCapture->captureParam.captureMode.hwInfo.deviceDescript = *desc;
    hwCapture->captureParam.frameCaptureMode.attrs = *attrs;
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMax = GAIN_MAX;  // init gainMax
    hwCapture->captureParam.captureMode.ctlParam.audioGain.gainMin = 0;
    hwCapture->captureParam.frameCaptureMode.frames = 0;
    hwCapture->captureParam.frameCaptureMode.time.tvNSec = 0;
    hwCapture->captureParam.frameCaptureMode.time.tvSec = 0;
    hwCapture->captureParam.frameCaptureMode.byteRate = DEFAULT_RENDER_SAMPLING_RATE;
    hwCapture->captureParam.frameCaptureMode.periodSize = DEEP_BUFFER_RENDER_PERIOD_SIZE;
    hwCapture->captureParam.frameCaptureMode.periodCount = DEEP_BUFFER_RENDER_PERIOD_COUNT;
    hwCapture->captureParam.frameCaptureMode.attrs.period = attrs->period;
    hwCapture->captureParam.frameCaptureMode.attrs.frameSize = attrs->frameSize;
    hwCapture->captureParam.frameCaptureMode.attrs.startThreshold = attrs->startThreshold;
    hwCapture->captureParam.frameCaptureMode.attrs.stopThreshold = attrs->stopThreshold;
    hwCapture->captureParam.frameCaptureMode.attrs.silenceThreshold = attrs->silenceThreshold;
    hwCapture->captureParam.frameCaptureMode.attrs.isBigEndian = attrs->isBigEndian;
    hwCapture->captureParam.frameCaptureMode.attrs.isSignedData = attrs->isSignedData;
    /* Select Codec Mode */
    if (hwCapture->captureParam.captureMode.hwInfo.deviceDescript.portId < AUDIO_SERVICE_PORTID_FLAG) {
        hwCapture->captureParam.captureMode.hwInfo.card = AUDIO_SERVICE_IN;
    } else {
        hwCapture->captureParam.captureMode.hwInfo.card = AUDIO_SERVICE_OUT;
    }
    return HDF_SUCCESS;
}

void AudioReleaseCaptureHandle(struct AudioHwCapture *hwCapture)
{
    if (hwCapture == NULL) {
        return;
    }
    CloseServiceCaptureSo *pCloseServiceCapture = AudioSoGetCloseServiceCapture();
    if (pCloseServiceCapture == NULL || (*pCloseServiceCapture) == NULL) {
        LOG_FUN_ERR("pCloseServiceCapture func not exist");
        return;
    }
    if (hwCapture->devDataHandle != NULL) {
        (*pCloseServiceCapture)(hwCapture->devDataHandle);
        hwCapture->devDataHandle = NULL;
    }
    if (hwCapture->devCtlHandle != NULL) {
        (*pCloseServiceCapture)(hwCapture->devCtlHandle);
        hwCapture->devCtlHandle = NULL;
    }
    return;
}

int32_t AudioAdapterCreateCapturePre(struct AudioHwCapture *hwCapture, const struct AudioDeviceDescriptor *desc,
                                     const struct AudioSampleAttributes *attrs, struct AudioHwAdapter *hwAdapter)
{
    if (hwCapture == NULL || desc == NULL || attrs == NULL || hwAdapter == NULL) {
        LOG_FUN_ERR("Pointer Is Empty!");
        return HDF_FAILURE;
    }
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    PathSelAnalysisJson *pPathSelAnalysisJson = AudioSoGetPathSelAnalysisJson();
    if (pPathSelAnalysisJson == NULL || *pPathSelAnalysisJson == NULL) {
        LOG_FUN_ERR("PathSelAnalysisJson not exist");
        return HDF_FAILURE;
    }
#endif
    if (GetAudioCaptureFunc(hwCapture) < 0) {
        return HDF_FAILURE;
    }
    /* Fill hwCapture para */
    if (InitHwCaptureParam(hwCapture, desc, attrs) < 0) {
        return HDF_FAILURE;
    }

#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    /* Select Path */
    if ((*pPathSelAnalysisJson)((void *)&hwCapture->captureParam, CAPTURE_PATH_SELECT) < 0) {
        LOG_FUN_ERR("Path Select Fail!");
        return HDF_FAILURE;
    }
#endif
    if (hwAdapter->adapterDescriptor.adapterName == NULL) {
        LOG_FUN_ERR("adapterName is NULL!");
        return HDF_FAILURE;
    }
    uint32_t adapterNameLen = strlen(hwAdapter->adapterDescriptor.adapterName);
    if (adapterNameLen == 0) {
        LOG_FUN_ERR("adapterNameLen is null!");
        return HDF_FAILURE;
    }
    /* Get Adapter name */
    int32_t ret = strncpy_s(hwCapture->captureParam.captureMode.hwInfo.adapterName, NAME_LEN,
                            hwAdapter->adapterDescriptor.adapterName, adapterNameLen);
    if (ret != EOK) {
        LOG_FUN_ERR("copy fail");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioAdapterInterfaceLibModeCapture(struct AudioHwCapture *hwCapture)
{
    LOG_FUN_INFO();
    if (hwCapture == NULL || hwCapture->devCtlHandle == NULL || hwCapture->devDataHandle == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    InterfaceLibModeCaptureSo *pInterfaceLibModeCapture = AudioSoGetInterfaceLibModeCapture();
    if (pInterfaceLibModeCapture == NULL || *pInterfaceLibModeCapture == NULL) {
        LOG_FUN_ERR("lib capture func not exist");
        return HDF_FAILURE;
    }
    ret = (*pInterfaceLibModeCapture)(hwCapture->devDataHandle, &hwCapture->captureParam,
                                      AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN);
    if (ret < 0) {
        LOG_FUN_ERR("CAPTURE_OPEN FAIL");
        return HDF_FAILURE;
    }
#ifndef AUDIO_HAL_NOTSUPPORT_PATHSELECT
    /* Init CapturePathSelect send first */
    ret = (*pInterfaceLibModeCapture)(hwCapture->devCtlHandle, &hwCapture->captureParam,
                                              AUDIODRV_CTL_IOCTL_SCENESELECT_CAPTURE);
    if (ret < 0) {
        LOG_FUN_ERR("SetParams FAIL!");
        return HDF_FAILURE;
    }
#endif
    ret = (*pInterfaceLibModeCapture)(hwCapture->devDataHandle, &hwCapture->captureParam,
                                      AUDIO_DRV_PCM_IOCTL_HW_PARAMS);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureStart SetParams FAIL");
        return HDF_FAILURE;
    }
    /* get volThreshold capture */
    ret = (*pInterfaceLibModeCapture)(hwCapture->devCtlHandle, &hwCapture->captureParam,
                                              AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE);
    if (ret < 0) {
        LOG_FUN_ERR("SetParams FAIL!");
        return HDF_FAILURE;
    }
#ifdef AUDIO_HAL_USER
#else
    ret = (*pInterfaceLibModeCapture)(hwCapture->devDataHandle, &hwCapture->captureParam,
                                      AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureStart prepare FAIL");
        return HDF_FAILURE;
    }
#endif
    return HDF_SUCCESS;
}

int32_t AudioCaptureBindService(struct AudioHwCapture *hwCapture, BindServiceCaptureSo *pBindServiceCapture)
{
    if (hwCapture == NULL || pBindServiceCapture == NULL || *pBindServiceCapture == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    hwCapture->devDataHandle = (*pBindServiceCapture)(CAPTURE_CMD);
    if (hwCapture->devDataHandle == NULL) {
        LOG_FUN_ERR("Capture bind service failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    hwCapture->devCtlHandle = (*pBindServiceCapture)(CTRL_CMD);
    if (hwCapture->devCtlHandle == NULL) {
        LOG_FUN_ERR("Capture bind service failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioAdapterInterfaceLibModeCapture(hwCapture);
    if (ret != 0) {
        LOG_FUN_ERR("AudioAdapterInterfaceLibModeCapture failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioAdapterCreateCapture(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
                                  const struct AudioSampleAttributes *attrs, struct AudioCapture **capture)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || desc == NULL || attrs == NULL || capture == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwAdapter->adapterMgrCaptureFlag > 0) {
        LOG_FUN_ERR("Create capture repeatedly!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    BindServiceCaptureSo *pBindServiceCapture = AudioSoGetBindServiceCapture();
    if (pBindServiceCapture == NULL || *pBindServiceCapture == NULL) {
        LOG_FUN_ERR("lib capture func not exist");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)calloc(1, sizeof(*hwCapture));
    if (hwCapture == NULL) {
        LOG_FUN_ERR("calloc AudioHwCapture failed!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    ret = AudioAdapterCreateCapturePre(hwCapture, desc, attrs, hwAdapter);
    if (ret != 0) {
        LOG_FUN_ERR("AudioAdapterCreateCapturePre fail");
        AudioMemFree((void **)&hwCapture);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioCaptureBindService(hwCapture, pBindServiceCapture);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureBindService fail");
        AudioReleaseCaptureHandle(hwCapture);
        AudioMemFree((void **)&hwCapture);
        return ret;
    }
    ret = AudioAddCaptureAddrToList((AudioHandle)(&hwCapture->common));
    if (ret < 0) {
        LOG_FUN_ERR("The capture address get is invalid");
        AudioReleaseCaptureHandle(hwCapture);
        AudioMemFree((void **)&hwCapture);
        return ret;
    }
    *capture = &hwCapture->common;
    hwAdapter->adapterMgrCaptureFlag++;
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioAdapterDestroyCapture(struct AudioAdapter *adapter, struct AudioCapture *capture)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    ret = AudioCheckCaptureAddr((AudioHandle)capture);
    if (ret < 0) {
        LOG_FUN_ERR("The capture address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || capture == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (hwAdapter->adapterMgrCaptureFlag > 0) {
        hwAdapter->adapterMgrCaptureFlag--;
    }
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    if (hwCapture == NULL) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (hwCapture->captureParam.frameCaptureMode.buffer != NULL) {
        ret = capture->control.Stop((AudioHandle)capture);
        if (ret < 0) {
            LOG_FUN_ERR("capture Stop failed");
        }
    }
    InterfaceLibModeCaptureSo *pInterfaceLibModeCapture = AudioSoGetInterfaceLibModeCapture();
    if (pInterfaceLibModeCapture == NULL || *pInterfaceLibModeCapture == NULL) {
        LOG_FUN_ERR("lib capture func not exist");
        return HDF_FAILURE;
    }
    ret = (*pInterfaceLibModeCapture)(hwCapture->devDataHandle, &hwCapture->captureParam,
                                      AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE);
    if (ret < 0) {
        LOG_FUN_ERR("CAPTURE_CLOSE FAIL");
    }
    if (AudioDelCaptureAddrFromList((AudioHandle)capture)) {
        LOG_FUN_ERR("adapter or capture not in MgrList");
    }
    AudioReleaseCaptureHandle(hwCapture);
    AudioMemFree((void **)&hwCapture->captureParam.frameCaptureMode.buffer);
    for (int i = 0; i < ERROR_LOG_MAX_NUM; i++) {
        AudioMemFree((void **)&hwCapture->errorLog.errorDump[i].reason);
        AudioMemFree((void **)&hwCapture->errorLog.errorDump[i].currentTime);
    }
    AudioMemFree((void **)&capture);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioAdapterGetPortCapability(struct AudioAdapter *adapter, const struct AudioPort *port,
                                      struct AudioPortCapability *capability)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || port == NULL || port->portName == NULL || capability == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioPortAndCapability *hwAdapterPortCapabilitys = hwAdapter->portCapabilitys;
    if (hwAdapterPortCapabilitys == NULL) {
        LOG_FUN_ERR("hwAdapter portCapabilitys is NULL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t portNum = hwAdapter->adapterDescriptor.portNum;
    while (hwAdapterPortCapabilitys != NULL && portNum) {
        if (hwAdapterPortCapabilitys->port.portId == port->portId) {
            *capability = hwAdapterPortCapabilitys->capability;
            return AUDIO_HAL_SUCCESS;
        }
        hwAdapterPortCapabilitys++;
        portNum--;
    }
    return AUDIO_HAL_ERR_INTERNAL;
}

int32_t AudioAdapterSetPassthroughMode(struct AudioAdapter *adapter,
                                       const struct AudioPort *port, enum AudioPortPassthroughMode mode)
{
    int32_t ret = AudioCheckAdapterAddr(adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    if (adapter == NULL || port == NULL || port->portName == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (port->dir != PORT_OUT || port->portId < 0 || strcmp(port->portName, "AOP") != 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter->portCapabilitys == NULL) {
        LOG_FUN_ERR("The pointer is null!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioPortAndCapability *portCapabilityTemp = hwAdapter->portCapabilitys;
    struct AudioPortCapability *portCapability = NULL;
    int32_t portNum = hwAdapter->adapterDescriptor.portNum;
    while (portCapabilityTemp != NULL && portNum > 0) {
        if (portCapabilityTemp->port.portId == port->portId) {
            portCapability = &portCapabilityTemp->capability;
            break;
        }
        portCapabilityTemp++;
        portNum--;
    }
    if (portCapability == NULL || portNum <= 0) {
        LOG_FUN_ERR("hwAdapter portCapabilitys is Not Find!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioSubPortCapability *subPortCapability = portCapability->subPorts;
    if (subPortCapability == NULL) {
        LOG_FUN_ERR("portCapability->subPorts is NULL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t subPortNum = portCapability->subPortsNum;
    while (subPortCapability != NULL && subPortNum > 0) {
        if (subPortCapability->mask == mode) {
            portCapabilityTemp->mode = mode;
            break;
        }
        subPortCapability++;
        subPortNum--;
    }
    if (subPortNum > 0) {
        return AUDIO_HAL_SUCCESS;
    }
    return AUDIO_HAL_ERR_INTERNAL;
}

int32_t AudioAdapterGetPassthroughMode(struct AudioAdapter *adapter, const struct AudioPort *port,
                                       enum AudioPortPassthroughMode *mode)
{
    int32_t ret = AudioCheckAdapterAddr(adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The adapter address passed in is invalid");
        return ret;
    }
    if (adapter == NULL || port == NULL || port->portName == NULL || mode == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (port->dir != PORT_OUT || port->portId < 0 || strcmp(port->portName, "AOP") != 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter->portCapabilitys == NULL) {
        LOG_FUN_ERR("portCapabilitys pointer is null!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioPortAndCapability *portCapabilitys = hwAdapter->portCapabilitys;
    int32_t portNum = hwAdapter->adapterDescriptor.portNum;
    while (portCapabilitys != NULL && portNum > 0) {
        if (portCapabilitys->port.portId == port->portId) {
            *mode = portCapabilitys->mode;
            return AUDIO_HAL_SUCCESS;
        }
        portCapabilitys++;
        portNum--;
    }
    return AUDIO_HAL_ERR_INTERNAL;
}
