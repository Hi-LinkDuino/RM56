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

#include "audio_interface_lib_capture.h"
#include "audio_hal_log.h"
#ifdef ALSA_MODE
#include "alsa_audio.h"
struct pcm *pcm;
struct DevInfo g_inDevInfo;
#endif

#define HDF_LOG_TAG HDF_AUDIO_HAL_LIB

/* virtual mixer device */
#define AUDIODRV_CTL_CAPTUREELEM_IFACE_MIXER ((int32_t)2)
#define AUDIODRV_CTL_ELEM_IFACE_ADC 1
#define AUDIODRV_CTL_ELEM_IFACE_SELECT 5
#define AUDIO_REPLY_EXTEND 16
#define AUDIO_SIZE_FRAME_16K (16 * 1024)
#define AUDIO_TRYNUM 2
#define AUDIO_TRYNUM_TIME 3000

/* Out Put Capture */
static struct AudioPcmHwParams g_hwParams;
static char *g_audioLibCaptureService[AUDIO_SERVICE_MAX] = {
    [AUDIO_SERVICE_IN] = "hdf_audio_codec_dev0",
    [AUDIO_SERVICE_OUT] = "hdf_audio_smartpa_dev0",
};

int32_t SetHwParamsCapture(const struct AudioHwCaptureParam * const handleData)
{
    if (handleData == NULL) {
        LOG_FUN_ERR("handleData is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("card is Error!");
        return HDF_FAILURE;
    }
    (void)memset_s(&g_hwParams, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));
    g_hwParams.streamType = AUDIO_CAPTURE_STREAM;
    g_hwParams.channels = handleData->frameCaptureMode.attrs.channelCount;
    g_hwParams.rate = handleData->frameCaptureMode.attrs.sampleRate;
    g_hwParams.periodSize = handleData->frameCaptureMode.periodSize;
    g_hwParams.periodCount = handleData->frameCaptureMode.periodCount;
    g_hwParams.cardServiceName = g_audioLibCaptureService[card];
    g_hwParams.format = handleData->frameCaptureMode.attrs.format;
    g_hwParams.period = handleData->frameCaptureMode.attrs.period;
    g_hwParams.frameSize = handleData->frameCaptureMode.attrs.frameSize;
    g_hwParams.isBigEndian = handleData->frameCaptureMode.attrs.isBigEndian;
    g_hwParams.isSignedData = handleData->frameCaptureMode.attrs.isSignedData;
    g_hwParams.startThreshold = handleData->frameCaptureMode.attrs.startThreshold;
    g_hwParams.stopThreshold = handleData->frameCaptureMode.attrs.stopThreshold;
    g_hwParams.silenceThreshold = handleData->frameCaptureMode.attrs.silenceThreshold;
#ifdef ALSA_MODE
    LOG_FUN_ERR("ALSA_MODE streamType %d, channels %d, rate %d, periodSize %d, periodCount %d, \
        cardServiceName %s, format %d, period %d, frameSize %d, isBigEndian %d, isSignedData %d, \
        startThreshold %d, stopThreshold %d, silenceThreshold %d",
        g_hwParams.streamType, g_hwParams.channels, g_hwParams.rate, g_hwParams.periodSize,
        g_hwParams.periodCount, g_hwParams.cardServiceName, g_hwParams.format, g_hwParams.period,
        g_hwParams.frameSize, g_hwParams.isBigEndian, g_hwParams.isSignedData, g_hwParams.startThreshold,
        g_hwParams.stopThreshold, g_hwParams.silenceThreshold);
#endif
    return HDF_SUCCESS;
}

int32_t ParamsSbufWriteBuffer(struct HdfSBuf *sBuf)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)g_hwParams.streamType)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.channels)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.rate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.periodSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.periodCount)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(g_hwParams.format))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, g_hwParams.cardServiceName)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.period)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.frameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(g_hwParams.isBigEndian))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(g_hwParams.isSignedData))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.startThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.stopThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, g_hwParams.silenceThreshold)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureSetPauseBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("wrong card!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_CAPTUREELEM_IFACE_MIXER;
    elemCaptureValue.id.itemName = "Main Playback Pause";
    elemCaptureValue.value[0] = (int32_t)handleData->captureMode.ctlParam.pause;
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.value[0])) {
        LOG_FUN_ERR("CaptureSetPauseBuf pause Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureSetPauseBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureSetPauseBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureSetPauseBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureSetPauseStu(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureSetPauseStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureSetPauseStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureSetPauseBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to Set Pause sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("SetPauseStu Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = handleData->captureMode.ctlParam.pause ?
        AUDIO_DRV_PCM_IOCTRL_PAUSE_CAPTURE : AUDIO_DRV_PCM_IOCTRL_RESUME_CAPTURE;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("SetPauseStu Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlCaptureGetVolumeSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetVolumeSBuf  parameter is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_ADC;
    elemCaptureValue.id.itemName = "Main Capture Volume";
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureGetVolumeSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureGetVolumeSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureGetVolumeSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureGetVolume(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    char *ctlName = "DACL Capture Volume";
    ReadInSoundCard();
    (void)memset_s(&g_inDevInfo, sizeof(struct DevInfo), 0, sizeof(struct DevInfo));
    GetInDevInfo(SND_IN_SOUND_CARD_MIC, &g_inDevInfo);
    MixerOpenLegacy(false, g_inDevInfo.card);
    handleData->captureMode.ctlParam.volume = RouteGetVoiceVolume(ctlName, false);
    return HDF_SUCCESS;
#endif
    int32_t ret;
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureGetVolume paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetVolume Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureGetVolumeSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("GetVolume Dispatch Fail!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    int32_t elemValue = 0;
    if (!HdfSbufReadInt32(reply, &elemValue)) {
        LOG_FUN_ERR("Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->captureMode.ctlParam.volume = (float)elemValue;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlCaptureSetVolumeSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureSetVolumeSBuf parameter is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureSetVolumeSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_ADC;
    elemCaptureValue.id.itemName = "Main Capture Volume";
    elemCaptureValue.value[0] = (int32_t)handleData->captureMode.ctlParam.volume;
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.value[0])) {
        LOG_FUN_ERR("CaptureSetVolumeSBuf value[0] Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureSetVolumeSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureSetVolumeSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureSetVolumeSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureSetVolume(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    float volume = handleData->captureMode.ctlParam.volume;
    RouteSetCaptureVoiceVolume(volume);
    return HDF_SUCCESS;
#endif
    int32_t ret;
    LOG_FUN_INFO();
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureSetVolume paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureSetVolume Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureSetVolumeSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("CaptureSetVolume Failed to Set Volume sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("CaptureSetVolume Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE - CTRL_NUM;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CaptureSetVolume Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlCaptureSetMuteSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureSetMuteSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_ADC;
    elemCaptureValue.id.itemName = "Capture Mute";
    elemCaptureValue.value[0] = (int32_t)handleData->captureMode.ctlParam.mute;
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.value[0])) {
        LOG_FUN_ERR("CaptureSetMuteSBuf mute Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureSetMuteSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureSetMuteSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureSetMuteSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureSetMuteStu(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureSetMuteStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureSetMuteStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureSetMuteSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("CaptureSetMuteStu Failed to Get Mute sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("CaptureSetMuteStu Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE - CTRL_NUM;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CaptureSetMuteStu Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlCaptureGetMuteSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("The parameter is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureGetMuteSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_ADC;
    elemCaptureValue.id.itemName = "Capture Mute";
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureGetMuteSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureGetMuteSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureGetMuteSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureGetMuteStu(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureGetMuteStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetMuteStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureGetMuteSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to Get Mute sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CaptureGetMuteStu Dispatch Fail!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    int32_t muteValueStu = 0;
    if (!HdfSbufReadInt32(reply, &muteValueStu)) {
        LOG_FUN_ERR("CaptureGetMuteStu Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->captureMode.ctlParam.mute = (bool)muteValueStu;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlCaptureSetGainSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureSetGainSBuf( handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureSetGainSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN;
    elemCaptureValue.id.itemName = "Mic Left Gain";
    elemCaptureValue.value[0] = handleData->captureMode.ctlParam.audioGain.gain;
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.value[0])) {
        LOG_FUN_ERR("CaptureSetGainSBuf mute Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureSetGainSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureSetGainSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureSetGainSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureSetGainStu(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureSetGainStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureSetGainStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureSetGainSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("CaptureSetGainStu Failed to Get Gain sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("CaptureSetGainStu Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE - CTRL_NUM;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CaptureSetGainStu Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlCaptureGetGainSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetGainSBuf paras is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureGetGainSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN;
    elemCaptureValue.id.itemName = "Mic Left Gain";
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureGetGainSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureGetGainSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureGetGainSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureGetGainStu(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureGetGainStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetGainStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureGetGainSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to Get Gain sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("GetGainStu Dispatch Fail!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    int32_t muteValueStu = 0;
    if (!HdfSbufReadInt32(reply, &muteValueStu)) {
        LOG_FUN_ERR("Failed to GetGain sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->captureMode.ctlParam.audioGain.gain = (float)muteValueStu;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlCaptureSceneSelectSBuf(struct HdfSBuf *sBuf,
    const struct AudioHwCaptureParam *handleData, int32_t deviceIndex)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureSceneSelectSBuf handleData or sBufs is NULL!");
        return HDF_FAILURE;
    }
    if (deviceIndex < 0 || deviceIndex > PATHPLAN_COUNT - 1) {
        LOG_FUN_ERR("deviceIndex is Invalid!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureSceneSelectSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemCaptureValue;
    elemCaptureValue.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_SELECT;
    elemCaptureValue.id.itemName =
        handleData->captureMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[deviceIndex].deviceSwitch;
    elemCaptureValue.value[0] = handleData->captureMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[deviceIndex].value;
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.value[0])) {
        LOG_FUN_ERR("CaptureSceneSelectSBuf Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemCaptureValue.id.iface)) {
        LOG_FUN_ERR("CaptureSceneSelectSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureSceneSelectSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureValue.id.itemName)) {
        LOG_FUN_ERR("CaptureSceneSelectSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureSceneSelect(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t index;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureSceneSelect parameter is empty!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    int32_t deviceNum = handleData->captureMode.hwInfo.pathSelect.deviceInfo.deviceNum;
    if (deviceNum < AUDIO_MIN_DEVICENUM) {
        LOG_FUN_ERR("AUDIO_MIN_ADAPTERNUM Failed!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE - CTRL_NUM;
    for (index = 0; index < deviceNum; index++) {
        HdfSbufFlush(sBuf);
        if (AudioCtlCaptureSceneSelectSBuf(sBuf, handleData, index) < 0) {
            LOG_FUN_ERR("AudioCtlRenderSceneSelectSBuf Failed!");
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
        if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
            LOG_FUN_ERR("CaptureSceneSelect Service is NULL!");
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
        if (service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply) < 0) {
            LOG_FUN_ERR("CaptureSceneSelect Failed to send service call!");
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureGetGainThresholdSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetGainThresholdSBuf paras is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureGetGainThresholdSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo elemCaptureInfo;
    elemCaptureInfo.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureInfo.id.iface = AUDIODRV_CTL_CAPTUREELEM_IFACE_MIXER;
    elemCaptureInfo.id.itemName = "Mic Left Gain";
    if (!HdfSbufWriteInt32(sBuf, elemCaptureInfo.id.iface)) {
        LOG_FUN_ERR("CaptureGetGainThresholdSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureInfo.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureGetGainThresholdSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureInfo.id.itemName)) {
        LOG_FUN_ERR("CaptureGetGainThresholdSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlCaptureGetGainThreshold(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureGetGainThreshold paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetGainThreshold Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("CaptureGetGainThreshold Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlCaptureGetGainThresholdSBuf(sBuf, handleData);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CaptureGetGainThreshold Failed to Get Threshold sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_INFO_CAPTURE - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo gainThreshold;
    (void)memset_s(&gainThreshold, sizeof(struct AudioCtrlElemInfo), 0, sizeof(struct AudioCtrlElemInfo));
    if (!HdfSbufReadInt32(reply, &gainThreshold.type)) {
        LOG_FUN_ERR("CaptureGetGainThreshold Failed to HdfSbufReadBuffer!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reply, &gainThreshold.max)) {
        LOG_FUN_ERR("CaptureGetGainThreshold Failed to HdfSbufReadBuffer!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->captureMode.ctlParam.audioGain.gainMax = gainThreshold.max;
    handleData->captureMode.ctlParam.audioGain.gainMin = 0;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlCaptureGetVolThresholdSBuf(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("CaptureGetVolThresholdSBuf paras is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("CaptureGetVolThresholdSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo elemCaptureInfo;
    elemCaptureInfo.id.cardServiceName = g_audioLibCaptureService[card];
    elemCaptureInfo.id.iface = AUDIODRV_CTL_ELEM_IFACE_ADC;
    elemCaptureInfo.id.itemName = "Main Capture Volume";
    if (!HdfSbufWriteInt32(sBuf, elemCaptureInfo.id.iface)) {
        LOG_FUN_ERR("elemCaptureInfo.id.iface iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureInfo.id.cardServiceName)) {
        LOG_FUN_ERR("CaptureGetVolThresholdSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemCaptureInfo.id.itemName)) {
        LOG_FUN_ERR("CaptureGetVolThresholdSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

#ifdef ALSA_MODE
int32_t TinyAlsaAudioCtlCaptureGetVolThreshold(struct AudioHwCaptureParam *handleData)
{
    int64_t volMin = 0;
    int64_t volMax = 0;
    char *ctlName = "DACL Capture Volume";
    ReadInSoundCard();
    (void)memset_s(&g_inDevInfo, sizeof(struct DevInfo), 0, sizeof(struct DevInfo));
    GetInDevInfo(SND_IN_SOUND_CARD_MIC, &g_inDevInfo);
    MixerOpenLegacy(false, g_inDevInfo.card);
    RouteGetVoiceMinMaxStep(&volMin, &volMax, ctlName, false);
    handleData->captureMode.ctlParam.volThreshold.volMax = volMax;
    handleData->captureMode.ctlParam.volThreshold.volMin = volMin;
    return HDF_SUCCESS;
}
#endif

int32_t AudioCtlCaptureGetVolThreshold(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return TinyAlsaAudioCtlCaptureGetVolThreshold(handleData);
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to Get sBuf");
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    int32_t ret = AudioCtlCaptureGetVolThresholdSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to Get Threshold sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    struct HdfIoService *service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_INFO_CAPTURE - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo volThreshold;
    ret = AudioCtlGetVolThresholdRead(reply, &volThreshold);
    if (ret < 0) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->captureMode.ctlParam.volThreshold.volMax = volThreshold.max;
    handleData->captureMode.ctlParam.volThreshold.volMin = volThreshold.min;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioInterfaceLibCtlCapture(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    switch (cmdId) {
        /* setPara: */
        case AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE:
            ret = AudioCtlCaptureSetVolume(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE:
            ret = AudioCtlCaptureSetMuteStu(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE:
            ret = AudioCtlCaptureGetMuteStu(handle, cmdId, handleData);
            break;
        /* getPara: */
        case AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE:
            ret = AudioCtlCaptureGetVolume(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE:
            ret = AudioCtlCaptureSetGainStu(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE:
            ret = AudioCtlCaptureGetGainStu(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_SCENESELECT_CAPTURE:
            ret = AudioCtlCaptureSceneSelect(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE:
            ret = AudioCtlCaptureGetGainThreshold(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE:
            ret = AudioCtlCaptureGetVolThreshold(handle, cmdId, handleData);
            break;
        default:
            LOG_FUN_ERR("Ctl Mode not support!");
            ret = HDF_FAILURE;
            break;
    }
    return ret;
}

int32_t AudioOutputCaptureHwParams(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("Function parameter is NULL!");
        return HDF_FAILURE;
    }
#ifndef ALSA_MODE
    int32_t ret;
    struct HdfIoService *service = NULL;
#else
    int32_t ret = HDF_SUCCESS;
#endif
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    if (SetHwParamsCapture(handleData) < 0) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    if (ParamsSbufWriteBuffer(sBuf) < 0) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
#ifndef ALSA_MODE
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("Function parameter is empty!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("Failed to send service call!");
    }
#endif
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioOutputCaptureOpen(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("Function parameter is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->captureMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("card is Error!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, g_audioLibCaptureService[card])) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    struct HdfIoService *service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("Function parameter is empty!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    int32_t ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioOutputCaptureReadFrame(struct HdfIoService *service, int cmdId, struct HdfSBuf *reply)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;
    uint32_t buffStatus = 0;
    int32_t tryNumReply = 100; // try get reply count
    if (service == NULL || reply == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("service->dispatcher is null!");
        return HDF_FAILURE;
    }
    do {
        ret = service->dispatcher->Dispatch(&service->object, cmdId, NULL, reply);
        if (ret != HDF_SUCCESS) {
            LOG_FUN_ERR("Failed to send service call!");
            HdfSbufRecycle(reply);
            return ret;
        }
        if (!HdfSbufReadUint32(reply, &buffStatus)) {
            LOG_FUN_ERR("Failed to Get buffStatus!");
            HdfSbufRecycle(reply);
            return HDF_FAILURE;
        }
        if (buffStatus == CIR_BUFF_EMPTY) {
            LOG_PARA_INFO("Cir buff empty wait 50ms");
            tryNumReply--;
            HdfSbufFlush(reply);
            // wait (5 + tryNum)ms when buff empty
            usleep(AUDIO_CAP_WAIT_DELAY + (tryNumReply % AUDIO_TRYNUM) * AUDIO_TRYNUM_TIME);
            continue;
        }
        break;
    } while (tryNumReply > 0);
    if (tryNumReply <= 0) {
        HdfSbufRecycle(reply);
        LOG_FUN_ERR("Out of tryNumReply!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

#ifdef ALSA_MODE
int32_t TinyalsaAudioOutputCaptureRead(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
    char *buffer = NULL;
    uint32_t dataSize;
    if (!pcm) { // if pcm is num, need create it first
        int format = PCM_FORMAT_S16_LE;
        if (g_hwParams.format == AUDIO_FORMAT_PCM_8_BIT) {
            format = PCM_FORMAT_S8;
        } else if (g_hwParams.format == AUDIO_FORMAT_PCM_16_BIT) {
            format = PCM_FORMAT_S16_LE;
        } else if (g_hwParams.format == AUDIO_FORMAT_PCM_24_BIT) {
            format = PCM_FORMAT_S24_LE;
        } else if (g_hwParams.format == AUDIO_FORMAT_PCM_32_BIT) {
            format = PCM_FORMAT_S32_LE;
        } else {
            format = PCM_FORMAT_S16_LE;
        }
        ReadInSoundCard();
        (void)memset_s(&g_inDevInfo, sizeof(struct DevInfo), 0, sizeof(struct DevInfo));
        GetInDevInfo(SND_IN_SOUND_CARD_MIC, &g_inDevInfo);
        struct PcmCaptureParam param;
        (void)memset_s(&param, sizeof(param), 0, sizeof(param));
        param.card = g_inDevInfo.card;
        param.device = g_inDevInfo.device;
        param.channels = g_hwParams.channels;
        param.rate = g_hwParams.rate;
        param.format = format;
        param.periodSize = g_hwParams.periodSize / 4; // limited to 16K,periodSize/4.
        param.periodCount = g_hwParams.periodCount / 2; // limited to 16K, periodCount/2.
        CaptureSample(&pcm, &param);
        RoutePcmCardOpen(g_inDevInfo.card, DEV_IN_HANDS_FREE_MIC_CAPTURE_ROUTE);
    }
    if (pcm) {
        dataSize = pcm_frames_to_bytes(pcm, pcm_get_buffer_size(pcm));
        if (dataSize < 1) {
            return HDF_FAILURE;
        }
        buffer = malloc(dataSize);
        if (!buffer) {
            fprintf(stderr, "Unable to allocate \n");
            pcm_close(pcm);
            return HDF_FAILURE;
        }
        if (!pcm_read(pcm, buffer, dataSize)) {
            memcpy_s(handleData->frameCaptureMode.buffer, FRAME_DATA, buffer, dataSize);
        }
        handleData->frameCaptureMode.bufferSize = dataSize;
        handleData->frameCaptureMode.bufferFrameSize = pcm_bytes_to_frames(pcm, dataSize);
        free(buffer);
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}
#endif

int32_t AudioOutputCaptureRead(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return TinyalsaAudioOutputCaptureRead(handle, cmdId, handleData);
#endif
    uint32_t dataSize = 0;
    uint32_t frameCount = 0;
    size_t replySize = AUDIO_SIZE_FRAME_16K + AUDIO_REPLY_EXTEND;
    char *frame = NULL;
    struct HdfIoService *service = NULL;
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = HdfSbufTypedObtainCapacity(SBUF_RAW, replySize);
    if (reply == NULL) {
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("Service is NULL!");
        AudioBufReplyRecycle(NULL, reply);
        return HDF_FAILURE;
    }
    int32_t ret = AudioOutputCaptureReadFrame(service, cmdId, reply);
    if (ret != 0) {
        LOG_FUN_ERR("AudioOutputCaptureReadFrame is invalid!");
        return HDF_FAILURE;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)&frame, &dataSize)) {
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }
    if (dataSize > FRAME_DATA || handleData->frameCaptureMode.buffer == NULL) {
        LOG_FUN_ERR("Buffer is NULL or DataSize overflow!");
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &frameCount)) {
        LOG_FUN_ERR("Failed to Get buffStatus!");
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }
    ret = memcpy_s(handleData->frameCaptureMode.buffer, FRAME_DATA, frame, dataSize);
    if (ret != 0) {
        HdfSbufRecycle(reply);
        return HDF_FAILURE;
    }
    handleData->frameCaptureMode.bufferSize = dataSize;
    handleData->frameCaptureMode.bufferFrameSize = frameCount;
    HdfSbufRecycle(reply);
    return HDF_SUCCESS;
}

int32_t AudioOutputCaptureStartPrepare(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;
    struct HdfIoService *service = NULL;
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureStartPrepare paras is NULL!");
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("CaptureStartPrepare Service is NULL!");
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("CaptureStartPrepare Failed to send service call!");
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t AudioOutputCaptureStop(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    if (pcm) {
        pcm_close(pcm);
        pcm = NULL;
    }
    RoutePcmClose(DEV_OFF_CAPTURE_OFF_ROUTE);
    return HDF_SUCCESS;
#endif
    int32_t ret;
    struct HdfIoService *service = NULL;
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("CaptureStop paras is NULL!");
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("CaptureStop Service is NULL!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, AUDIO_TURN_STANDBY_LATER)) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, NULL);
        LOG_FUN_ERR("CaptureStop Failed to send service call!");
        return ret;
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t MmapDescWriteBufferCapture(struct HdfSBuf *sBuf, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (sBuf == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    uint64_t mmapAddr = (uint64_t)(uintptr_t)(handleData->frameCaptureMode.mmapBufDesc.memoryAddress);
    if (!HdfSbufWriteUint64(sBuf, mmapAddr)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameCaptureMode.mmapBufDesc.memoryFd)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameCaptureMode.mmapBufDesc.totalBufferFrames)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameCaptureMode.mmapBufDesc.transferFrameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameCaptureMode.mmapBufDesc.isShareable)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, handleData->frameCaptureMode.mmapBufDesc.offset)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioOutputCaptureReqMmapBuffer(const struct DevHandleCapture *handle,
    int cmdId, const struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (MmapDescWriteBufferCapture(sBuf, handleData)) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        AudioBufReplyRecycle(sBuf, NULL);
        LOG_FUN_ERR("The pointer is empty!");
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("Failed to send service call!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t AudioOutputCaptureGetMmapPosition(const struct DevHandleCapture *handle,
    int cmdId, struct AudioHwCaptureParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("The pointer is empty!");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("CaptureGetMmapPosition Failed to obtain reply");
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, NULL, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("Failed to send service call!");
        AudioBufReplyRecycle(NULL, reply);
        return HDF_FAILURE;
    }
    uint64_t frames = 0;
    if (!HdfSbufReadUint64(reply, &frames)) {
        LOG_FUN_ERR("Failed to Get frames sBuf!");
        AudioBufReplyRecycle(NULL, reply);
        return HDF_FAILURE;
    }
    handleData->frameCaptureMode.frames = frames;
    AudioBufReplyRecycle(NULL, reply);
    return HDF_SUCCESS;
}

int32_t AudioInterfaceLibOutputCapture(const struct DevHandleCapture *handle, int cmdId,
                                       struct AudioHwCaptureParam *handleData)
{
    if (handle == NULL) {
        LOG_FUN_ERR("Input Capture handle is NULL!");
        return HDF_FAILURE;
    }
    if (handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    switch (cmdId) {
        case AUDIO_DRV_PCM_IOCTL_HW_PARAMS:
            ret = AudioOutputCaptureHwParams(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTL_READ:
            ret = AudioOutputCaptureRead(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTRL_START_CAPTURE:
        case AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE:
        case AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE:
            ret = AudioOutputCaptureStartPrepare(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN:
            ret = AudioOutputCaptureOpen(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTRL_STOP_CAPTURE:
            ret = AudioOutputCaptureStop(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_PAUSE_WRITE_CAPTURE:
            ret = AudioCtlCaptureSetPauseStu(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER_CAPTURE:
            ret = AudioOutputCaptureReqMmapBuffer(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTL_MMAP_POSITION_CAPTURE:
            ret = AudioOutputCaptureGetMmapPosition(handle, cmdId, handleData);
            break;
        default:
            LOG_FUN_ERR("Output Mode not support!");
            ret = HDF_FAILURE;
            break;
    }
    return ret;
}

struct DevHandleCapture *AudioBindServiceCaptureObject(struct DevHandleCapture * const handle,
    const char *name)
{
    if (handle == NULL || name == NULL) {
        LOG_FUN_ERR("service name or handle is NULL!");
        return NULL;
    }
    char *serviceName = (char *)calloc(1, NAME_LEN);
    if (serviceName == NULL) {
        LOG_FUN_ERR("Failed to OsalMemCalloc serviceName");
        AudioMemFree((void **)&handle);
        return NULL;
    }
    int ret = snprintf_s(serviceName, NAME_LEN - 1, SERVIC_NAME_MAX_LEN + 1, "hdf_audio_%s", name);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to snprintf_s");
        AudioMemFree((void **)&serviceName);
        AudioMemFree((void **)&handle);
        return NULL;
    }
    struct HdfIoService *service = HdfIoServiceBindName(serviceName);
    if (service == NULL) {
        LOG_FUN_ERR("Failed to get service!");
        AudioMemFree((void **)&serviceName);
        AudioMemFree((void **)&handle);
        return NULL;
    }
    AudioMemFree((void **)&serviceName);
    handle->object = service;
    return handle->object;
}

/* CreatCapture for Bind handle */
struct DevHandleCapture *AudioBindServiceCapture(const char *name)
{
    struct DevHandleCapture *handle = NULL;
    struct DevHandleCapture *object = NULL;
    if (name == NULL) {
        LOG_FUN_ERR("service name NULL!");
        return NULL;
    }
    handle = (struct DevHandleCapture *)calloc(1, sizeof(struct DevHandleCapture));
    if (handle == NULL) {
        LOG_FUN_ERR("Failed to OsalMemCalloc handle");
        return NULL;
    }
    object = AudioBindServiceCaptureObject(handle, name);
    if (object != NULL) {
        handle->object = object;
    } else {
        LOG_FUN_ERR("handle->object is NULL!");
        return NULL;
    }
    LOG_PARA_INFO("BIND SERVICE SUCCESS!");
    return handle;
}

void AudioCloseServiceCapture(const struct DevHandleCapture *handle)
{
#ifdef ALSA_MODE
    return;
#endif
    LOG_FUN_INFO();
    if (handle == NULL || handle->object == NULL) {
        LOG_FUN_ERR("Capture handle or handle->object is NULL");
        return;
    }
    struct HdfIoService *service = (struct HdfIoService *)handle->object;
    HdfIoServiceRecycle(service);
    AudioMemFree((void **)&handle);
    return;
}

int32_t AudioInterfaceLibModeCapture(const struct DevHandleCapture *handle,
    struct AudioHwCaptureParam *handleData, int cmdId)
{
    LOG_FUN_INFO();
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case AUDIO_DRV_PCM_IOCTL_HW_PARAMS:
        case AUDIO_DRV_PCM_IOCTL_READ:
        case AUDIO_DRV_PCM_IOCTRL_START_CAPTURE:
        case AUDIO_DRV_PCM_IOCTRL_STOP_CAPTURE:
        case AUDIO_DRV_PCM_IOCTL_PREPARE_CAPTURE:
        case AUDIODRV_CTL_IOCTL_PAUSE_WRITE_CAPTURE:
        case AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER_CAPTURE:
        case AUDIO_DRV_PCM_IOCTL_MMAP_POSITION_CAPTURE:
        case AUDIO_DRV_PCM_IOCTRL_CAPTURE_OPEN:
        case AUDIO_DRV_PCM_IOCTRL_CAPTURE_CLOSE:
            return (AudioInterfaceLibOutputCapture(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_ELEM_WRITE_CAPTURE:
        case AUDIODRV_CTL_IOCTL_ELEM_READ_CAPTURE:
        case AUDIODRV_CTL_IOCTL_MUTE_WRITE_CAPTURE:
        case AUDIODRV_CTL_IOCTL_MUTE_READ_CAPTURE:
        case AUDIODRV_CTL_IOCTL_GAIN_WRITE_CAPTURE:
        case AUDIODRV_CTL_IOCTL_GAIN_READ_CAPTURE:
        case AUDIODRV_CTL_IOCTL_SCENESELECT_CAPTURE:
        case AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_CAPTURE:
        case AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_CAPTURE:
            return (AudioInterfaceLibCtlCapture(handle, cmdId, handleData));
        default:
            LOG_FUN_ERR("Mode Error!");
            break;
    }
    return HDF_ERR_NOT_SUPPORT;
}
