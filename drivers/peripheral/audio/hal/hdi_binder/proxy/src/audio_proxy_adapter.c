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

#include "audio_hal_log.h"
#include "audio_proxy_common.h"
#include "audio_proxy_internal.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_PROXY

int32_t AudioProxyCommonInitAttrs(struct HdfSBuf *data, const struct AudioSampleAttributes *attrs)
{
    if (data == NULL || attrs == NULL) {
        LOG_FUN_ERR("data == NULL || attrs == NULL");
        return HDF_FAILURE;
    }
    uint32_t tempAtrr;
    tempAtrr = (uint32_t)attrs->interleaved;
    if (!HdfSbufWriteUint32(data, tempAtrr)) {
        return HDF_FAILURE;
    }
    tempAtrr = (uint32_t)attrs->type;
    if (!HdfSbufWriteUint32(data, tempAtrr)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->period)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->frameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->startThreshold)) {
        LOG_FUN_ERR("startThreshold Write Fail");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->stopThreshold)) {
        LOG_FUN_ERR("stopThreshold Write Fail");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->silenceThreshold)) {
        return HDF_FAILURE;
    }
    tempAtrr = (uint32_t)attrs->isBigEndian;
    if (!HdfSbufWriteUint32(data, tempAtrr)) {
        return HDF_FAILURE;
    }
    tempAtrr = (uint32_t)attrs->isSignedData;
    if (!HdfSbufWriteUint32(data, tempAtrr)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyCommonInitCreateData(struct HdfSBuf *data, const struct AudioHwAdapter *adapter,
    const struct AudioDeviceDescriptor *desc, const struct AudioSampleAttributes *attrs)
{
    LOG_FUN_INFO();
    if (data == NULL || adapter == NULL || desc == NULL || attrs == NULL) {
        LOG_FUN_ERR("data == NULL || adapter == NULL || desc == NULL || attrs == NULL");
        return HDF_FAILURE;
    }
    uint32_t tempDesc;
    uint32_t tempAtrr;
    int32_t pid = getpid();
    const char *adapterName = adapter->adapterDescriptor.adapterName;
    if (adapterName == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, adapterName)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, pid)) {
        return HDF_FAILURE;
    }
    tempAtrr = (uint32_t)attrs->format;
    if (!HdfSbufWriteUint32(data, tempAtrr)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->channelCount)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->sampleRate)) {
        return HDF_FAILURE;
    }
    if (AudioProxyCommonInitAttrs(data, attrs) < 0) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, desc->portId)) {
        return HDF_FAILURE;
    }
    tempDesc = (uint32_t)desc->pins;
    if (!HdfSbufWriteUint32(data, tempDesc)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t GetAudioProxyRenderFunc(struct AudioHwRender *hwRender)
{
    if (hwRender == NULL) {
        return HDF_FAILURE;
    }
    hwRender->common.control.Start = AudioProxyRenderStart;
    hwRender->common.control.Stop = AudioProxyRenderStop;
    hwRender->common.control.Pause = AudioProxyRenderPause;
    hwRender->common.control.Resume = AudioProxyRenderResume;
    hwRender->common.control.Flush = AudioProxyRenderFlush;
    hwRender->common.control.TurnStandbyMode = AudioProxyRenderTurnStandbyMode;
    hwRender->common.control.AudioDevDump = AudioProxyRenderAudioDevDump;
    hwRender->common.attr.GetFrameSize = AudioProxyRenderGetFrameSize;
    hwRender->common.attr.GetFrameCount = AudioProxyRenderGetFrameCount;
    hwRender->common.attr.SetSampleAttributes = AudioProxyRenderSetSampleAttributes;
    hwRender->common.attr.GetSampleAttributes = AudioProxyRenderGetSampleAttributes;
    hwRender->common.attr.GetCurrentChannelId = AudioProxyRenderGetCurrentChannelId;
    hwRender->common.attr.SetExtraParams = AudioProxyRenderSetExtraParams;
    hwRender->common.attr.GetExtraParams = AudioProxyRenderGetExtraParams;
    hwRender->common.attr.ReqMmapBuffer = AudioProxyRenderReqMmapBuffer;
    hwRender->common.attr.GetMmapPosition = AudioProxyRenderGetMmapPosition;
    hwRender->common.scene.CheckSceneCapability = AudioProxyRenderCheckSceneCapability;
    hwRender->common.scene.SelectScene = AudioProxyRenderSelectScene;
    hwRender->common.volume.SetMute = AudioProxyRenderSetMute;
    hwRender->common.volume.GetMute = AudioProxyRenderGetMute;
    hwRender->common.volume.SetVolume = AudioProxyRenderSetVolume;
    hwRender->common.volume.GetVolume = AudioProxyRenderGetVolume;
    hwRender->common.volume.GetGainThreshold = AudioProxyRenderGetGainThreshold;
    hwRender->common.volume.GetGain = AudioProxyRenderGetGain;
    hwRender->common.volume.SetGain = AudioProxyRenderSetGain;
    hwRender->common.GetLatency = AudioProxyRenderGetLatency;
    hwRender->common.RenderFrame = AudioProxyRenderRenderFrame;
    hwRender->common.GetRenderPosition = AudioProxyRenderGetRenderPosition;
    hwRender->common.SetRenderSpeed = AudioProxyRenderSetRenderSpeed;
    hwRender->common.GetRenderSpeed = AudioProxyRenderGetRenderSpeed;
    hwRender->common.SetChannelMode = AudioProxyRenderSetChannelMode;
    hwRender->common.GetChannelMode = AudioProxyRenderGetChannelMode;
    hwRender->common.RegCallback = AudioProxyRenderRegCallback;
    hwRender->common.DrainBuffer = AudioProxyRenderDrainBuffer;
    return HDF_SUCCESS;
}

int32_t InitHwRenderParam(struct AudioHwRender *hwRender, const struct AudioDeviceDescriptor *desc,
                          const struct AudioSampleAttributes *attrs)
{
    if (hwRender == NULL || desc == NULL || attrs == NULL) {
        LOG_FUN_ERR("InitHwRenderParam param Is NULL");
        return HDF_FAILURE;
    }
    hwRender->renderParam.renderMode.hwInfo.deviceDescript = *desc;
    hwRender->renderParam.frameRenderMode.attrs = *attrs;
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
            LOG_FUN_ERR("pointer is null!");
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

void AudioAdapterReleaseCapSubPorts(const struct AudioPortAndCapability *portCapabilitys, const int32_t num)
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
int32_t InitAllPortsDispatchSplit(struct AudioHwAdapter *hwAdapter)
{
    if (hwAdapter == NULL || hwAdapter->adapterDescriptor.adapterName == NULL || hwAdapter->proxyRemoteHandle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    const char *adapterName = NULL;
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(hwAdapter->proxyRemoteHandle, data)) {
        LOG_FUN_ERR("write interface token failed");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    adapterName = hwAdapter->adapterDescriptor.adapterName;
    if (!HdfSbufWriteString(data, adapterName)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwAdapter->proxyRemoteHandle, AUDIO_HDI_ADT_INIT_PORTS, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("Get Failed AudioAdapter!");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterInitAllPorts(struct AudioAdapter *adapter)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxyAdapter address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || hwAdapter->adapterDescriptor.adapterName == NULL ||
        hwAdapter->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("hwAdapter Is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    /* Fake data */
    uint32_t portNum = hwAdapter->adapterDescriptor.portNum;
    struct AudioPort *ports = hwAdapter->adapterDescriptor.ports;
    if (ports == NULL || portNum == 0) {
        LOG_FUN_ERR("ports is NULL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioPortAndCapability *portCapability = (struct AudioPortAndCapability *)calloc(portNum,
        sizeof(struct AudioPortAndCapability));
    if (portCapability == NULL) {
        LOG_FUN_ERR("portCapability is NULL!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    for (uint32_t i = 0; i < portNum; i++) {
        portCapability[i].port = ports[i];
        if (InitForGetPortCapability(ports[i], &portCapability[i].capability)) {
            LOG_FUN_ERR("ports Init Invalid!");
            AudioAdapterReleaseCapSubPorts(portCapability, portNum);
            AudioMemFree((void **)&portCapability);
            return AUDIO_HAL_ERR_INTERNAL;
        }
    }
    hwAdapter->portCapabilitys = portCapability;
    hwAdapter->portCapabilitys->mode = PORT_PASSTHROUGH_LPCM;
    ret = InitAllPortsDispatchSplit(hwAdapter);
    if (ret < 0) {
        LOG_FUN_ERR("InitAllPortsDispatchSplit Fail");
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterCreateRenderSplit(const struct AudioHwAdapter *hwAdapter, struct AudioHwRender *hwRender)
{
    if (hwAdapter == NULL || hwRender == NULL) {
        return HDF_FAILURE;
    }
    if (hwAdapter->adapterDescriptor.adapterName == NULL) {
        return HDF_FAILURE;
    }
    uint32_t adapterNameLen = strlen(hwAdapter->adapterDescriptor.adapterName);
    /* Get Adapter name */
    int32_t ret = strncpy_s(hwRender->renderParam.renderMode.hwInfo.adapterName, NAME_LEN,
        hwAdapter->adapterDescriptor.adapterName, adapterNameLen);
    if (ret != EOK) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyRenderDispatchSplit(const struct AudioHwAdapter *hwAdapter,
    struct AudioHwRender *hwRender, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (hwAdapter == NULL || hwRender == NULL ||
        hwRender->proxyRemoteHandle == NULL || data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyAdapterCreateRenderSplit(hwAdapter, hwRender) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioAddRenderAddrToList((AudioHandle)(&hwRender->common));
    if (ret < 0) {
        LOG_FUN_ERR("The proxyRender address get is invalid");
        return ret;
    }
    ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_CREATE_RENDER, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("Send Server fail!");
        if (AudioDelRenderAddrFromList((AudioHandle)(&hwRender->common)) < 0) {
            LOG_FUN_ERR("AudioDelRenderAddrFromList failed.");
        }
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

static inline int32_t AudioProxyWriteTokenAndInitDataForRender(struct AudioHwAdapter *hwAdapter, struct HdfSBuf *data,
                                                               const struct AudioDeviceDescriptor *desc,
                                                               const struct AudioSampleAttributes *attrs)
{
    if (!HdfRemoteServiceWriteInterfaceToken(hwAdapter->proxyRemoteHandle, data)) {
        LOG_FUN_ERR("write interface token failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyCommonInitCreateData(data, hwAdapter, desc, attrs) < 0) {
        LOG_FUN_ERR("Failed to obtain reply");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterCreateRender(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
                                      const struct AudioSampleAttributes *attrs, struct AudioRender **render)
{
    LOG_FUN_INFO();
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxyAdapter address passed in is invalid");
        return ret;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL || desc == NULL || attrs == NULL || render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)calloc(1, sizeof(*hwRender));
    if (hwRender == NULL) {
        LOG_FUN_ERR("hwRender is NULL!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwRender->proxyRemoteHandle = hwAdapter->proxyRemoteHandle;
    if (GetAudioProxyRenderFunc(hwRender) < 0) {
        AudioMemFree((void **)&hwRender);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    /* Fill hwRender para */
    if (InitHwRenderParam(hwRender, desc, attrs) < 0) {
        AudioMemFree((void **)&hwRender);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        AudioMemFree((void **)&hwRender);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyWriteTokenAndInitDataForRender(hwAdapter, data, desc, attrs) != AUDIO_HAL_SUCCESS) {
        LOG_FUN_ERR("write interface token or initdata failed");
        AudioProxyBufReplyRecycle(data, reply);
        AudioMemFree((void **)&hwRender);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyRenderDispatchSplit(hwAdapter, hwRender, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        AudioMemFree((void **)&hwRender);
        return ret;
    }
    *render = &hwRender->common;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterDestroyRender(struct AudioAdapter *adapter, struct AudioRender *render)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxyAdapter address passed in is invalid");
        return ret;
    }
    ret = AudioCheckRenderAddr((AudioHandle)render);
    if (ret < 0) {
        LOG_FUN_ERR("The proxyRender address passed in is invalid");
        return ret;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (adapter == NULL || render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioHwRender *hwRender = (struct AudioHwRender *)render;
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender((AudioHandle)render, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_DESTROY, data, reply);
    if (ret < 0) {
        if (ret != AUDIO_HAL_ERR_INVALID_OBJECT) {
            LOG_FUN_ERR("AudioRenderRenderFrame FAIL");
            AudioProxyBufReplyRecycle(data, reply);
            return ret;
        }
    }
    if (AudioDelRenderAddrFromList((AudioHandle)render) < 0) {
        LOG_FUN_ERR("proxyAdapter or proxyRender not in MgrList");
    }
    AudioMemFree((void **)&hwRender->renderParam.frameRenderMode.buffer);
    AudioMemFree((void **)&render);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t GetAudioProxyCaptureFunc(struct AudioHwCapture *hwCapture)
{
    if (hwCapture == NULL) {
        return HDF_FAILURE;
    }
    hwCapture->common.control.Start = AudioProxyCaptureStart;
    hwCapture->common.control.Stop = AudioProxyCaptureStop;
    hwCapture->common.control.Pause = AudioProxyCapturePause;
    hwCapture->common.control.Resume = AudioProxyCaptureResume;
    hwCapture->common.control.Flush = AudioProxyCaptureFlush;
    hwCapture->common.control.TurnStandbyMode = AudioProxyCaptureTurnStandbyMode;
    hwCapture->common.control.AudioDevDump = AudioProxyCaptureAudioDevDump;
    hwCapture->common.attr.GetFrameSize = AudioProxyCaptureGetFrameSize;
    hwCapture->common.attr.GetFrameCount = AudioProxyCaptureGetFrameCount;
    hwCapture->common.attr.SetSampleAttributes = AudioProxyCaptureSetSampleAttributes;
    hwCapture->common.attr.GetSampleAttributes = AudioProxyCaptureGetSampleAttributes;
    hwCapture->common.attr.GetCurrentChannelId = AudioProxyCaptureGetCurrentChannelId;
    hwCapture->common.attr.SetExtraParams = AudioProxyCaptureSetExtraParams;
    hwCapture->common.attr.GetExtraParams = AudioProxyCaptureGetExtraParams;
    hwCapture->common.attr.ReqMmapBuffer = AudioProxyCaptureReqMmapBuffer;
    hwCapture->common.attr.GetMmapPosition = AudioProxyCaptureGetMmapPosition;
    hwCapture->common.scene.CheckSceneCapability = AudioProxyCaptureCheckSceneCapability;
    hwCapture->common.scene.SelectScene = AudioProxyCaptureSelectScene;
    hwCapture->common.volume.SetMute = AudioProxyCaptureSetMute;
    hwCapture->common.volume.GetMute = AudioProxyCaptureGetMute;
    hwCapture->common.volume.SetVolume = AudioProxyCaptureSetVolume;
    hwCapture->common.volume.GetVolume = AudioProxyCaptureGetVolume;
    hwCapture->common.volume.GetGainThreshold = AudioProxyCaptureGetGainThreshold;
    hwCapture->common.volume.GetGain = AudioProxyCaptureGetGain;
    hwCapture->common.volume.SetGain = AudioProxyCaptureSetGain;
    hwCapture->common.CaptureFrame = AudioProxyCaptureCaptureFrame;
    hwCapture->common.GetCapturePosition = AudioProxyCaptureGetCapturePosition;
    return HDF_SUCCESS;
}

int32_t InitProxyHwCaptureParam(struct AudioHwCapture *hwCapture, const struct AudioDeviceDescriptor *desc,
                                const struct AudioSampleAttributes *attrs)
{
    if (hwCapture == NULL || desc == NULL || attrs == NULL) {
        LOG_FUN_ERR("InitHwCaptureParam param Is NULL");
        return HDF_FAILURE;
    }
    hwCapture->captureParam.captureMode.hwInfo.deviceDescript = *desc;
    hwCapture->captureParam.frameCaptureMode.attrs = *attrs;
    return HDF_SUCCESS;
}

int32_t AudioProxyAdapterCreateCaptureSplit(const struct AudioHwAdapter *hwAdapter, struct AudioHwCapture *hwCapture)
{
    if (hwAdapter == NULL || hwCapture == NULL) {
        return HDF_FAILURE;
    }
    if (hwAdapter->adapterDescriptor.adapterName == NULL) {
        return HDF_FAILURE;
    }
    uint32_t adapterNameLen = strlen(hwAdapter->adapterDescriptor.adapterName);
    /* Get AdapterName */
    int32_t ret = strncpy_s(hwCapture->captureParam.captureMode.hwInfo.adapterName, NAME_LEN,
        hwAdapter->adapterDescriptor.adapterName, adapterNameLen);
    if (ret != EOK) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyCaptureDispatchSplit(const struct AudioHwAdapter *hwAdapter,
    struct AudioHwCapture *hwCapture, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (hwAdapter == NULL || hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL ||
        data == NULL || reply == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyAdapterCreateCaptureSplit(hwAdapter, hwCapture) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioAddCaptureAddrToList((AudioHandle)(&hwCapture->common));
    if (ret < 0) {
        LOG_FUN_ERR("The proxyCapture address get is invalid");
        return ret;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_CREATE_CAPTURE, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("Send Server fail!");
        if (AudioDelCaptureAddrFromList((AudioHandle)(&hwCapture->common)) < 0) {
            LOG_FUN_ERR("AudioDelRenderAddrFromList failed.");
        }
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}
static inline int32_t AudioProxyWriteTokenAndInitDataForCapture(struct AudioHwAdapter *hwAdapter, struct HdfSBuf *data,
                                                                const struct AudioDeviceDescriptor *desc,
                                                                const struct AudioSampleAttributes *attrs)
{
    if (!HdfRemoteServiceWriteInterfaceToken(hwAdapter->proxyRemoteHandle, data)) {
        LOG_FUN_ERR("write interface token failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyCommonInitCreateData(data, hwAdapter, desc, attrs) < 0) {
        LOG_FUN_ERR("Failed to obtain reply");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterCreateCapture(struct AudioAdapter *adapter, const struct AudioDeviceDescriptor *desc,
                                       const struct AudioSampleAttributes *attrs, struct AudioCapture **capture)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxyAdapter address passed in is invalid");
        return ret;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL || desc == NULL ||
        attrs == NULL || capture == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)calloc(1, sizeof(struct AudioHwCapture));
    if (hwCapture == NULL) {
        LOG_FUN_ERR("hwCapture is NULL!");
        return AUDIO_HAL_ERR_MALLOC_FAIL;
    }
    hwCapture->proxyRemoteHandle = hwAdapter->proxyRemoteHandle;
    if (GetAudioProxyCaptureFunc(hwCapture) < 0) {
        AudioMemFree((void **)&hwCapture);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    /* Fill hwRender para */
    if (InitProxyHwCaptureParam(hwCapture, desc, attrs) < 0) {
        AudioMemFree((void **)&hwCapture);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        AudioMemFree((void **)&hwCapture);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyWriteTokenAndInitDataForCapture(hwAdapter, data, desc, attrs) != AUDIO_HAL_SUCCESS) {
        LOG_FUN_ERR("write interface token or initdata failed");
        AudioProxyBufReplyRecycle(data, reply);
        AudioMemFree((void **)&hwCapture);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyCaptureDispatchSplit(hwAdapter, hwCapture, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        AudioMemFree((void **)&hwCapture);
        return ret;
    }
    *capture = &hwCapture->common;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterDestroyCapture(struct AudioAdapter *adapter, struct AudioCapture *capture)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (adapter == NULL || capture == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxy adapter address passed in is invalid");
        return ret;
    }
    ret = AudioCheckCaptureAddr((AudioHandle)capture);
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture((AudioHandle)capture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_DESTROY, data, reply);
    if (ret < 0) {
        if (ret != AUDIO_HAL_ERR_INVALID_OBJECT) {
            LOG_FUN_ERR("AudioCaptureCaptuerFrame fail!");
            AudioProxyBufReplyRecycle(data, reply);
            return ret;
        }
    }
    if (AudioDelCaptureAddrFromList((AudioHandle)capture)) {
        LOG_FUN_ERR("proxy adapter or capture not in MgrList");
    }
    AudioMemFree((void **)&hwCapture->captureParam.frameCaptureMode.buffer);
    AudioMemFree((void **)&capture);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyAdapterWritePortCapability(const struct AudioHwAdapter *hwAdapter,
    const struct AudioPort *port, struct HdfSBuf *data)
{
    if (hwAdapter == NULL || port == NULL || data == NULL) {
        return HDF_FAILURE;
    }
    if (hwAdapter->adapterDescriptor.adapterName == NULL) {
        return HDF_FAILURE;
    }
    const char *adapterName = hwAdapter->adapterDescriptor.adapterName;
    if (!HdfSbufWriteString(data, adapterName)) {
        return HDF_FAILURE;
    }
    uint32_t tempDir = (uint32_t)port->dir;
    if (!HdfSbufWriteUint32(data, tempDir)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, port->portId)) {
        return HDF_FAILURE;
    }
    if (port->portName == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, port->portName)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyAdapterGetPortCapability(struct AudioAdapter *adapter,
    const struct AudioPort *port, struct AudioPortCapability *capability)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxy adapter address passed in is invalid");
        return ret;
    }
    if (adapter == NULL || port == NULL || port->portName == NULL || capability == NULL || (port->portId < 0)) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(hwAdapter->proxyRemoteHandle, data)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyAdapterWritePortCapability(hwAdapter, port, data)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwAdapter->proxyRemoteHandle, AUDIO_HDI_ADT_GET_PORT_CAPABILITY, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    /* proxy must init local capability ,this capability the same of Server's */
    struct AudioPortAndCapability *hwAdapterPortCapabilitys = hwAdapter->portCapabilitys;
    if (hwAdapterPortCapabilitys == NULL) {
        LOG_FUN_ERR("hwAdapter portCapabilitys is NULL!");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t portNum = hwAdapter->adapterDescriptor.portNum;
    while (hwAdapterPortCapabilitys != NULL && (portNum > 0)) {
        if (hwAdapterPortCapabilitys->port.portId == port->portId) {
            *capability = hwAdapterPortCapabilitys->capability;
            return AUDIO_HAL_SUCCESS;
        }
        hwAdapterPortCapabilitys++;
        portNum--;
    }
    return AUDIO_HAL_ERR_INTERNAL;
}

int32_t AudioProxyAdapterSetAndGetPassthroughModeSBuf(struct HdfSBuf *data,
    const struct HdfSBuf *reply, const struct AudioPort *port)
{
    if (data == NULL || port == NULL || port->portName == NULL) {
        return HDF_FAILURE;
    }
    uint32_t tempDir = port->dir;
    if (!HdfSbufWriteUint32(data, tempDir)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, port->portId)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(data, port->portName)) {
        LOG_FUN_ERR("HdfSbufWriteString error");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
int32_t AudioProxyWriteTokenAndNameForSetPassThrough(struct AudioHwAdapter *hwAdapter, struct HdfSBuf *data)
{
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL ||
        hwAdapter->adapterDescriptor.adapterName == NULL) {
        LOG_FUN_ERR("The input para is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(hwAdapter->proxyRemoteHandle, data)) {
        LOG_FUN_ERR("write interface token failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    const char *adapterName = hwAdapter->adapterDescriptor.adapterName;
    if (!HdfSbufWriteString(data, adapterName)) {
        LOG_FUN_ERR("adapterName Write Fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterSetPassthroughMode(struct AudioAdapter *adapter,
    const struct AudioPort *port, enum AudioPortPassthroughMode mode)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxy adapter address passed in is invalid");
        return ret;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (adapter == NULL || port == NULL || port->portName == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (port->dir != PORT_OUT || port->portId < 0 || strcmp(port->portName, "AOP") != 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyPreprocessSBuf Fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL ||
        hwAdapter->adapterDescriptor.adapterName == NULL) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }

    if (AudioProxyWriteTokenAndNameForSetPassThrough(hwAdapter, data) != AUDIO_HAL_SUCCESS) {
        LOG_FUN_ERR("Failed to write token or adapter name");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyAdapterSetAndGetPassthroughModeSBuf(data, reply, port) < 0) {
        LOG_FUN_ERR("Failed to obtain data");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }

    uint32_t tempMode = (uint32_t)mode;
    if (!HdfSbufWriteUint32(data, tempMode)) {
        LOG_FUN_ERR("Mode Write Fail");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwAdapter->proxyRemoteHandle, AUDIO_HDI_ADT_SET_PASS_MODE, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        LOG_FUN_ERR("Failed to send server");
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyWriteTokenAndNameForGetPassThrough(struct AudioHwAdapter *hwAdapter, struct HdfSBuf *data)
{
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL ||
        hwAdapter->adapterDescriptor.adapterName == NULL) {
        LOG_FUN_ERR("The input para is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(hwAdapter->proxyRemoteHandle, data)) {
        LOG_FUN_ERR("write interface token failed");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    const char *adapterName = hwAdapter->adapterDescriptor.adapterName;
    if (!HdfSbufWriteString(data, adapterName)) {
        LOG_FUN_ERR("adapterName Write Fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyAdapterGetPassthroughMode(struct AudioAdapter *adapter,
    const struct AudioPort *port, enum AudioPortPassthroughMode *mode)
{
    int32_t ret = AudioCheckAdapterAddr((AudioHandle)adapter);
    if (ret < 0) {
        LOG_FUN_ERR("The proxy adapter address passed in is invalid");
        return ret;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (adapter == NULL || port == NULL || port->portName == NULL || mode == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (port->dir != PORT_OUT || port->portId < 0 || strcmp(port->portName, "AOP") != 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyPreprocessSBuf(&data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    struct AudioHwAdapter *hwAdapter = (struct AudioHwAdapter *)adapter;
    if (hwAdapter == NULL || hwAdapter->proxyRemoteHandle == NULL) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyWriteTokenAndNameForGetPassThrough(hwAdapter, data) != AUDIO_HAL_SUCCESS) {
        LOG_FUN_ERR("Failed to write token or adapter name");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyAdapterSetAndGetPassthroughModeSBuf(data, reply, port) < 0) {
        LOG_FUN_ERR("Failed to obtain data");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwAdapter->proxyRemoteHandle, AUDIO_HDI_ADT_GET_PASS_MODE, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t tempMode = 0;
    if (!HdfSbufReadUint32(reply, &tempMode)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *mode = (enum AudioPortPassthroughMode)tempMode;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

