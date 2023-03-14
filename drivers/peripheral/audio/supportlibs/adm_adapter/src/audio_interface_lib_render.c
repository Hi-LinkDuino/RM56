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

#include "audio_interface_lib_render.h"
#include "audio_hal_log.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_LIB

#define AUDIODRV_CTL_ELEM_IFACE_DAC 0
#define AUDIODRV_CTL_ELEM_IFACE_MIX 3
#define AUDIO_SBUF_EXTEND 16

#define AUDIODRV_CTL_ELEM_IFACE_MIXER ((int32_t)2) /* virtual mixer device */
#define AUDIODRV_CTL_ELEM_IFACE_ACODEC ((int32_t)4) /* Acodec device */
#define AUDIODRV_CTL_ELEM_IFACE_AIAO ((int32_t)6)

#define AUDIODRV_CTL_ACODEC_ENABLE 1
#define AUDIODRV_CTL_ACODEC_DISABLE 0
#define AUDIODRV_CTL_INTERNAL_ACODEC_ENABLE 1
#define AUDIODRV_CTL_EXTERN_ACODEC_ENABLE 2
#define AUDIODRV_CTL_EXTERN_CODEC_STR "External Codec Enable"
#define AUDIODRV_CTL_INTERNAL_CODEC_STR "Internally Codec Enable"
#ifdef ALSA_MODE
#include "alsa_audio.h"
struct pcm *pcm;
struct DevInfo g_outDevInfo;
#endif

/* Out Put Render */
static struct AudioPcmHwParams g_hwParams;
char *g_audioLibRenderService[AUDIO_SERVICE_MAX] = {
    [AUDIO_SERVICE_IN] = "hdf_audio_codec_dev0",
    [AUDIO_SERVICE_OUT] = "hdf_audio_smartpa_dev0",
};

int32_t SetHwParams(const struct AudioHwRenderParam *handleData)
{
    if (handleData == NULL) {
        LOG_FUN_ERR("handleData is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("card is Error!");
        return HDF_FAILURE;
    }
    (void)memset_s(&g_hwParams, sizeof(struct AudioPcmHwParams), 0, sizeof(struct AudioPcmHwParams));
    g_hwParams.streamType = AUDIO_RENDER_STREAM;
    g_hwParams.channels = handleData->frameRenderMode.attrs.channelCount;
    g_hwParams.rate = handleData->frameRenderMode.attrs.sampleRate;
    g_hwParams.periodSize = handleData->frameRenderMode.periodSize;
    g_hwParams.periodCount = handleData->frameRenderMode.periodCount;
    g_hwParams.cardServiceName = g_audioLibRenderService[card];
    g_hwParams.format = handleData->frameRenderMode.attrs.format;
    g_hwParams.period = handleData->frameRenderMode.attrs.period;
    g_hwParams.frameSize = handleData->frameRenderMode.attrs.frameSize;
    g_hwParams.isBigEndian = handleData->frameRenderMode.attrs.isBigEndian;
    g_hwParams.isSignedData = handleData->frameRenderMode.attrs.isSignedData;
    g_hwParams.startThreshold = handleData->frameRenderMode.attrs.startThreshold;
    g_hwParams.stopThreshold = handleData->frameRenderMode.attrs.stopThreshold;
    g_hwParams.silenceThreshold = handleData->frameRenderMode.attrs.silenceThreshold;

#ifdef ALSA_MODE
    LOG_PARA_INFO("ALSA_MODE streamType %d, channels %d, rate %d, periodSize %d, periodCount %d, \
        cardServiceName %s, format %d, period %d, frameSize %d, isBigEndian %d, isSignedData %d, \
        startThreshold %d, stopThreshold %d, silenceThreshold %d",
        g_hwParams.streamType, g_hwParams.channels, g_hwParams.rate, g_hwParams.periodSize,
        g_hwParams.periodCount, g_hwParams.cardServiceName, g_hwParams.format, g_hwParams.period,
        g_hwParams.frameSize, g_hwParams.isBigEndian, g_hwParams.isSignedData, g_hwParams.startThreshold,
        g_hwParams.stopThreshold, g_hwParams.silenceThreshold);
#endif
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSetVolumeSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif

    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSetVolumeSBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderSetVolumeSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC;
    elemValue.id.itemName = "Main Playback Volume";
    elemValue.value[0] = (int32_t)handleData->renderMode.ctlParam.volume;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSetVolumeSBuf Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSetVolumeSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSetVolumeSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSetVolumeSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderGetVolumeSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderGetVolumeSBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderGetVolumeSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC;
    elemValue.id.itemName = "Main Playback Volume";
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderGetVolumeSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderGetVolumeSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderGetVolumeSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSetVolume(const struct DevHandle *handle, int cmdId,
    const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    RouteSetVoiceVolume(handleData->renderMode.ctlParam.volume);
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSetVolume parameter is empty!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderSetVolumeSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to Set Volume sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("Failed to AudioServiceDispatch!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

#ifdef ALSA_MODE
int32_t AudioCtlRenderGetVolumeALSA(struct AudioHwRenderParam *handleData)
{
    char *ctlName = "DACL Playback Volume";
    ReadOutSoundCard();
    (void)memset_s(&g_outDevInfo, sizeof(struct DevInfo), 0, sizeof(struct DevInfo));
    GetOutDevInfo(SND_OUT_SOUND_CARD_SPEAKER, &g_outDevInfo);
    MixerOpenLegacy(true, g_outDevInfo.card);
    handleData->renderMode.ctlParam.volume = RouteGetVoiceVolume(ctlName, true);
    return HDF_SUCCESS;
}
#endif

int32_t AudioCtlRenderGetVolume(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData)
{
    int32_t ret;
#ifdef ALSA_MODE
    return AudioCtlRenderGetVolumeALSA(handleData);
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderGetVolume parameter is empty!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderGetVolume Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("RenderGetVolume Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderGetVolumeSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderGetVolume Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderGetVolume RenderDispatch Failed!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    (void)memset_s(&elemValue, sizeof(struct AudioCtlElemValue), 0, sizeof(struct AudioCtlElemValue));
    if (!HdfSbufReadInt32(reply, &elemValue.value[0])) {
        LOG_FUN_ERR("RenderGetVolume Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->renderMode.ctlParam.volume = elemValue.value[0];
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlRenderSetPauseBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSetPauseBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderSetPauseBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_MIXER;
    elemValue.id.itemName = "Main Playback Pause";
    elemValue.value[0] = (int32_t)handleData->renderMode.ctlParam.pause;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSetPauseBuf pause Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSetPauseBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSetPauseBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSetPauseBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSetPauseStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSetPauseStu parameter is empty!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderSetPauseStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderSetPauseBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSetPauseStu Failed to Set Pause sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderSetPauseStu Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = handleData->renderMode.ctlParam.pause ?
        AUDIO_DRV_PCM_IOCTRL_PAUSE : AUDIO_DRV_PCM_IOCTRL_RESUME;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderSetPauseStu Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlRenderSetMuteBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSetMuteBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderSetMuteBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC;
    elemValue.id.itemName = "Playback Mute";
    elemValue.value[0] = (int32_t)handleData->renderMode.ctlParam.mute;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSetMuteBuf value[0] Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSetMuteBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSetMuteBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSetMuteBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSetMuteStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSetMuteStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderSetMuteStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderSetMuteBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSetMuteStu Failed to Set Mute sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderSetMuteStu Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE - CTRL_NUM;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderSetMuteStu Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlRenderGetMuteSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderGetMuteSBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderGetMuteSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC;
    elemValue.id.itemName = "Playback Mute";
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderGetMuteSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderGetMuteSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderGetMuteSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderGetMuteStu(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderGetMuteStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderGetMuteStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("RenderGetMuteStu Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderGetMuteSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderGetMuteStu Failed to Get Mute sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderGetMuteStu RenderDispatch Failed!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue muteValueStu;
    (void)memset_s(&muteValueStu, sizeof(struct AudioCtlElemValue), 0, sizeof(struct AudioCtlElemValue));
    if (!HdfSbufReadInt32(reply, &muteValueStu.value[0])) {
        LOG_FUN_ERR("RenderGetMuteStu Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->renderMode.ctlParam.mute = muteValueStu.value[0];
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlRenderSetGainBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSetGainBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderSetGainBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN;
    elemValue.id.itemName = "Mic Left Gain";
    elemValue.value[0] = (int32_t)handleData->renderMode.ctlParam.audioGain.gain;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSetGainBuf value[0] Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSetGainBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSetGainBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSetGainBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSetGainStu(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSetGainStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderSetGainStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderSetGainBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSetGainStu Failed to Set Gain sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderSetGainStu Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE - CTRL_NUM;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderSetGainStu Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlRenderGetGainSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderGetGainSBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderGetGainSBuf card is invalid!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_GAIN;
    elemValue.id.itemName = "Mic Left Gain";
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderGetGainSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderGetGainSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderGetGainSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderGetGainStu(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderGetGainStu paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderGetGainStu Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("RenderGetGainStu Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderGetGainSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderGetGainStu ailed to Get Gain sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("Dispatch Fail!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue gainValueStu;
    (void)memset_s(&gainValueStu, sizeof(struct AudioCtlElemValue), 0, sizeof(struct AudioCtlElemValue));
    if (!HdfSbufReadInt32(reply, &gainValueStu.value[0])) {
        LOG_FUN_ERR("Failed to Get Gain sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->renderMode.ctlParam.audioGain.gain = gainValueStu.value[0];
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlRenderSceneSelectSBuf(struct HdfSBuf *sBuf,
                                      const struct AudioHwRenderParam *handleData,
                                      const int32_t deviceIndex)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSceneSelectSBuf handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    if (deviceIndex < 0 || deviceIndex > PATHPLAN_COUNT - 1) {
        LOG_FUN_ERR("deviceIndex is error!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("card is invalid!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC;
    elemValue.id.itemName =
        handleData->renderMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[deviceIndex].deviceSwitch;
    elemValue.value[0] =
        handleData->renderMode.hwInfo.pathSelect.deviceInfo.deviceSwitchs[deviceIndex].value;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSceneSelectSBuf Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSceneSelectSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSceneSelectSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSceneSelectSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSceneSelect(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t index;
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSceneSelect paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderSceneSelect Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    int32_t deviceNum = handleData->renderMode.hwInfo.pathSelect.deviceInfo.deviceNum;
    if (deviceNum < AUDIO_MIN_DEVICENUM) {
        LOG_FUN_ERR("AUDIO_MIN_ADAPTERNUM Failed!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE - CTRL_NUM;
    for (index = 0; index < deviceNum; index++) {
        HdfSbufFlush(sBuf);
        if (AudioCtlRenderSceneSelectSBuf(sBuf, handleData, index) < 0) {
            LOG_FUN_ERR("AudioCtlRenderSceneSelectSBuf Failed!");
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
        if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
            LOG_FUN_ERR("Service is NULL!");
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
        if (service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply) < 0) {
            LOG_FUN_ERR("Failed to send service call!");
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderGetVolThresholdSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderGetVolThresholdSBuf paras is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderGetVolThresholdSBuf card is Invalid!");
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo elemInfo;
    elemInfo.id.cardServiceName = g_audioLibRenderService[card];
    elemInfo.id.iface = AUDIODRV_CTL_ELEM_IFACE_DAC;
    elemInfo.id.itemName = "Main Playback Volume";
    if (!HdfSbufWriteInt32(sBuf, elemInfo.id.iface)) {
        LOG_FUN_ERR("RenderGetVolThresholdSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemInfo.id.cardServiceName)) {
        LOG_FUN_ERR("RenderGetVolThresholdSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemInfo.id.itemName)) {
        LOG_FUN_ERR("RenderGetVolThresholdSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSceneGetGainThresholdSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSceneGetGainThresholdSBuf paras is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderSceneGetGainThresholdSBuf card is Invalid!");
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo elemInfo;
    elemInfo.id.cardServiceName = g_audioLibRenderService[card];
    elemInfo.id.iface = AUDIODRV_CTL_ELEM_IFACE_MIXER;
    elemInfo.id.itemName = "Mic Left Gain";
    if (!HdfSbufWriteInt32(sBuf, elemInfo.id.iface)) {
        LOG_FUN_ERR("RenderSceneGetGainThresholdSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemInfo.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSceneGetGainThresholdSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemInfo.id.itemName)) {
        LOG_FUN_ERR("RenderSceneGetGainThresholdSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSceneGetGainThreshold(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSceneGetGainThreshold paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderSceneGetGainThreshold Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("RenderSceneGetGainThreshold reply is NULL");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderSceneGetGainThresholdSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSceneGetGainThreshold Get Threshold sBuf Fail!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_INFO - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo gainThreshold;
    (void)memset_s(&gainThreshold, sizeof(struct AudioCtrlElemInfo), 0, sizeof(struct AudioCtrlElemInfo));
    if (!HdfSbufReadInt32(reply, &gainThreshold.type)) {
        LOG_FUN_ERR("RenderSceneGetGainThreshold Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt32(reply, &gainThreshold.max)) {
        LOG_FUN_ERR("RenderSceneGetGainThreshold Failed to Get Volume sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->renderMode.ctlParam.audioGain.gainMax = gainThreshold.max;
    handleData->renderMode.ctlParam.audioGain.gainMin = 0;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlRenderGetVolThreshold(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    int64_t volMin = 0;
    int64_t volMax = 0;
    char *ctlName = "DACL Playback Volume";
    ReadOutSoundCard();
    (void)memset_s(&g_outDevInfo, sizeof(struct DevInfo), 0, sizeof(struct DevInfo));
    GetOutDevInfo(SND_OUT_SOUND_CARD_SPEAKER, &g_outDevInfo);
    MixerOpenLegacy(true, g_outDevInfo.card);
    RouteGetVoiceMinMaxStep(&volMin, &volMax, ctlName, true);
    handleData->renderMode.ctlParam.volThreshold.volMax = volMax;
    handleData->renderMode.ctlParam.volThreshold.volMin = volMin;
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    int32_t ret = AudioCtlRenderGetVolThresholdSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("Get Threshold sBuf Fail!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    struct HdfIoService *service = (struct HdfIoService *)handle->object;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_INFO - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtrlElemInfo volThreshold;
    if (AudioCtlGetVolThresholdRead(reply, &volThreshold) < 0) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->renderMode.ctlParam.volThreshold.volMax = volThreshold.max;
    handleData->renderMode.ctlParam.volThreshold.volMin = volThreshold.min;
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlRenderSetChannelModeBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderSetChannelModeBuf parameter is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderSetChannelModeBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO;
    elemValue.id.itemName = "Render Channel Mode";
    elemValue.value[0] = handleData->frameRenderMode.mode;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSetChannelModeBuf mode Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSetChannelModeBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSetChannelModeBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSetChannelModeBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderSetChannelMode(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderSetChannelMode paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderSetChannelMode Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderSetChannelModeBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderSetChannelMode Failed to Set ChannelMode sBuf!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderSetChannelMode Service is NULL!");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE - CTRL_NUM;
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderSetChannelMode Failed to send service call!");
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioCtlRenderGetChannelModeSBuf(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handleData == NULL || sBuf == NULL) {
        LOG_FUN_ERR("RenderGetChannelModeSBuf parameter is empty!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("RenderGetChannelModeSBuf card is Error!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[card];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_AIAO;
    elemValue.id.itemName = "Render Channel Mode";
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderGetChannelModeSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderGetChannelModeSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderGetChannelModeSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderGetChannelMode(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;

    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("RenderGetChannelMode paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("RenderGetChannelMode Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("RenderGetChannelMode Failed to obtain reply");
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = AudioCtlRenderGetChannelModeSBuf(sBuf, handleData);
    if (ret < 0) {
        LOG_FUN_ERR("RenderGetChannelMode Failed to Get Channel Mode sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return ret;
    }
    service = (struct HdfIoService *)handle->object;
    handleData->frameRenderMode.mode = 1;
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_READ - CTRL_NUM;
    ret = AudioServiceDispatch(service, cmdId, sBuf, reply);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    (void)memset_s(&elemValue, sizeof(struct AudioCtlElemValue), 0, sizeof(struct AudioCtlElemValue));
    if (!HdfSbufReadInt32(reply, &elemValue.value[0])) {
        LOG_FUN_ERR("Failed to Get ChannelMode sBuf!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    handleData->frameRenderMode.mode = (enum AudioChannelMode)elemValue.value[0];
    AudioBufReplyRecycle(sBuf, reply);
    return ret;
}

int32_t AudioCtlRenderSetAcodecSBuf(struct HdfSBuf *sBuf, const char *codec,
    int enable, enum AudioServiceNameType serviceNameMode)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (sBuf == NULL) {
        LOG_FUN_ERR("handleData or sBuf is NULL!");
        return HDF_FAILURE;
    }
    if (serviceNameMode >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("serviceNameMode is fail!");
        return HDF_FAILURE;
    }
    struct AudioCtlElemValue elemValue;
    elemValue.id.cardServiceName = g_audioLibRenderService[serviceNameMode];
    elemValue.id.iface = AUDIODRV_CTL_ELEM_IFACE_ACODEC;
    elemValue.id.itemName = codec;
    elemValue.value[0] = enable;
    if (!HdfSbufWriteInt32(sBuf, elemValue.value[0])) {
        LOG_FUN_ERR("RenderSetAcodecSBuf value Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, elemValue.id.iface)) {
        LOG_FUN_ERR("RenderSetAcodecSBuf iface Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.cardServiceName)) {
        LOG_FUN_ERR("RenderSetAcodecSBuf cardServiceName Write Fail!");
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, elemValue.id.itemName)) {
        LOG_FUN_ERR("RenderSetAcodecSBuf itemName Write Fail!");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioCtlRenderChangeInAcodec(struct HdfIoService *service,
    const char *codecName, struct HdfSBuf *sBuf, const int32_t status, int cmdId)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    enum AudioServiceNameType serviceNameMode;
    if (service == NULL || sBuf == NULL) {
        LOG_FUN_ERR("service or sBuf is NULL!");
        return HDF_FAILURE;
    }
    if (cmdId == AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_OUT) {
        serviceNameMode = AUDIO_SERVICE_OUT;
    } else {
        serviceNameMode = AUDIO_SERVICE_IN;
    }
    if (AudioCtlRenderSetAcodecSBuf(sBuf, codecName, status, serviceNameMode)) {
        return HDF_FAILURE;
    }
    cmdId = AUDIODRV_CTL_IOCTL_ELEM_WRITE - CTRL_NUM;
    return (AudioServiceDispatch(service, cmdId, sBuf, NULL));
}

int32_t AudioCtlRenderSetAcodecMode(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        LOG_FUN_ERR("Failed to obtain sBuf");
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (cmdId == AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN) {
        LOG_PARA_INFO("****Acodec is In****");
        /* enable Internally Codec */
        HdfSbufFlush(sBuf);
        if (AudioCtlRenderChangeInAcodec(service, AUDIODRV_CTL_INTERNAL_CODEC_STR,
            sBuf, AUDIODRV_CTL_INTERNAL_ACODEC_ENABLE, cmdId)) {
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
    } else if (cmdId == AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_OUT) {
        LOG_PARA_INFO("****Acodec is Out****");
        /* enable External Codec */
        HdfSbufFlush(sBuf);
        if (AudioCtlRenderChangeInAcodec(service, AUDIODRV_CTL_EXTERN_CODEC_STR,
            sBuf, AUDIODRV_CTL_EXTERN_ACODEC_ENABLE, cmdId)) {
            AudioBufReplyRecycle(sBuf, NULL);
            return HDF_FAILURE;
        }
    } else {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t AudioInterfaceLibCtlRender(const struct DevHandle *handle, int cmdId, struct AudioHwRenderParam *handleData)
{
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    if (cmdId < AUDIODRV_CTL_IOCTL_ELEM_INFO || cmdId > AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ) {
        LOG_FUN_ERR("cmdId Not Supported!");
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case AUDIODRV_CTL_IOCTL_ELEM_READ:
            return (AudioCtlRenderGetVolume(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_ELEM_WRITE:
            return (AudioCtlRenderSetVolume(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_MUTE_READ:
            return (AudioCtlRenderGetMuteStu(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_MUTE_WRITE:
            return (AudioCtlRenderSetMuteStu(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ:
            return (AudioCtlRenderGetChannelMode(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE:
            return (AudioCtlRenderSetChannelMode(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_GAIN_WRITE:
            return (AudioCtlRenderSetGainStu(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_GAIN_READ:
            return (AudioCtlRenderGetGainStu(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE:
            return (AudioCtlRenderSceneSelect(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ:
            return (AudioCtlRenderSceneGetGainThreshold(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN:
        case AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_OUT:
            return (AudioCtlRenderSetAcodecMode(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ:
            return (AudioCtlRenderGetVolThreshold(handle, cmdId, handleData));
        default:
            LOG_FUN_ERR("Output Mode not support!");
            break;
    }
    return HDF_FAILURE;
}

int32_t ParamsSbufWriteBuffer(struct HdfSBuf *sBuf)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
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

int32_t FrameSbufWriteBuffer(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    if (!pcm) {
        int bits;
        if (g_hwParams.format == AUDIO_FORMAT_PCM_8_BIT) {
            bits = TINYALSAPCM_8_BIT;
        } else if (g_hwParams.format == AUDIO_FORMAT_PCM_16_BIT) {
            bits = TINYALSAPCM_16_BIT;
        } else if (g_hwParams.format == AUDIO_FORMAT_PCM_24_BIT) {
            bits = TINYALSAPCM_24_BIT;
        } else if (g_hwParams.format == AUDIO_FORMAT_PCM_32_BIT) {
            bits = TINYALSAPCM_32_BIT;
        } else {
            bits = TINYALSAPCM_16_BIT;
        }
        ReadOutSoundCard();
        (void)memset_s(&g_outDevInfo, sizeof(struct DevInfo), 0, sizeof(struct DevInfo));
        GetOutDevInfo(SND_OUT_SOUND_CARD_SPEAKER, &g_outDevInfo);
        struct PcmRenderParam param;
        (void)memset_s(&param, sizeof(param), 0, sizeof(param));
        param.card = g_outDevInfo.card;
        param.device = g_outDevInfo.device;
        param.channels = g_hwParams.channels;
        param.rate = g_hwParams.rate;
        param.bits = bits;
        param.periodSize = g_hwParams.periodSize / 4; // Because the data frame size is limited to 16K,periodSize/4.
        param.periodCount = g_hwParams.periodCount / 2; // Because the data frame size is limited to 16K,periodcount/2
        RenderSample(&pcm, &param);
        RoutePcmCardOpen(g_outDevInfo.card, DEV_OUT_SPEAKER_HEADPHONE_NORMAL_ROUTE);
    }
    if (pcm) {
        pcm_write(pcm, handleData->frameRenderMode.buffer, handleData->frameRenderMode.bufferSize);
        return HDF_SUCCESS;
    } else {
        return HDF_FAILURE;
    }
#endif
    if (sBuf == NULL || handleData == NULL || handleData->frameRenderMode.buffer == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, (uint32_t)(handleData->frameRenderMode.bufferFrameSize))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteBuffer(sBuf, handleData->frameRenderMode.buffer,
        (uint32_t)handleData->frameRenderMode.bufferSize)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioOutputRenderHwParams(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifndef ALSA_MODE
    int32_t ret;
    struct HdfIoService *service = NULL;
#else
    int32_t ret = HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("The parameter is empty");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (SetHwParams(handleData) < 0) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    if (ParamsSbufWriteBuffer(sBuf)) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }

#ifndef ALSA_MODE
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("The pointer is null!");
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

int32_t AudioCallbackModeStatus(const struct AudioHwRenderParam *handleData,
    enum AudioCallbackType callbackType)
{
    if (handleData == NULL) {
        return HDF_FAILURE;
    }
    bool callBackStatus = handleData->renderMode.hwInfo.callBackEnable;
    if (callBackStatus) {
        handleData->frameRenderMode.callbackProcess(handleData->frameRenderMode.renderhandle, callbackType);
    }
    return HDF_SUCCESS;
}

int32_t AudioOutputRenderWriteFrame(struct HdfIoService *service,
    int cmdId, struct HdfSBuf *sBuf, struct HdfSBuf *reply, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    int32_t ret;
    int32_t tryNum = 50; // try send sBuf 50 count
    uint32_t buffStatus = 0;
    if (service == NULL || sBuf == NULL || reply == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    if (service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        return HDF_FAILURE;
    }
    do {
        ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, reply);
        if (ret != HDF_SUCCESS) {
            LOG_FUN_ERR("Failed to send service call!");
            AudioBufReplyRecycle(sBuf, reply);
            return ret;
        }
        if (!HdfSbufReadUint32(reply, &buffStatus)) {
            LOG_FUN_ERR("Failed to Get buffStatus!");
            AudioBufReplyRecycle(sBuf, reply);
            return HDF_FAILURE;
        }
        if (buffStatus == CIR_BUFF_FULL) {
            LOG_PARA_INFO("Cir buff fulled wait 10ms");
            (void)AudioCallbackModeStatus(handleData, AUDIO_RENDER_FULL);
            tryNum--;
            usleep(AUDIO_WAIT_DELAY);
            continue;
        }
        break;
    } while (tryNum > 0);
    AudioBufReplyRecycle(sBuf, reply);
    if (tryNum > 0) {
        (void)AudioCallbackModeStatus(handleData, AUDIO_NONBLOCK_WRITE_COMPELETED);
        return HDF_SUCCESS;
    } else {
        (void)AudioCallbackModeStatus(handleData, AUDIO_ERROR_OCCUR);
        LOG_FUN_ERR("Out of tryNum!");
        return HDF_FAILURE;
    }
}

#ifdef ALSA_MODE
int32_t TinyAlsaAudioOutputRenderWrite(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    size_t sbufSize = handleData->frameRenderMode.bufferSize + AUDIO_SBUF_EXTEND;
    struct HdfSBuf *sBuf = HdfSbufTypedObtainCapacity(SBUF_RAW, sbufSize);
    if (sBuf == NULL) {
        LOG_FUN_ERR("Get sBuf Fail");
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("reply is empty");
        HdfSbufRecycle(sBuf);
        return HDF_FAILURE;
    }
    if (FrameSbufWriteBuffer(sBuf, handleData)) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
#endif

int32_t AudioOutputRenderWrite(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    int32_t ret = TinyAlsaAudioOutputRenderWrite(handle, cmdId, handleData);
    if (ret == HDF_FAILURE) {
        return HDF_FAILURE;
    }
#else
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    size_t sbufSize = (size_t)handleData->frameRenderMode.bufferSize + AUDIO_SBUF_EXTEND;
    struct HdfSBuf *sBuf = HdfSbufTypedObtainCapacity(SBUF_RAW, sbufSize);
    if (sBuf == NULL) {
        LOG_FUN_ERR("Get sBuf Fail");
        return HDF_FAILURE;
    }
    struct HdfSBuf *reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("reply is empty");
        HdfSbufRecycle(sBuf);
        return HDF_FAILURE;
    }
    if (FrameSbufWriteBuffer(sBuf, handleData)) {
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("Service is NULL!");
        AudioBufReplyRecycle(sBuf, reply);
        return HDF_FAILURE;
    }
    int32_t ret = AudioOutputRenderWriteFrame(service, cmdId, sBuf, reply, handleData);
    if (ret != 0) {
        LOG_FUN_ERR("AudioOutputRenderWriteFrame is Fail!");
        return HDF_FAILURE;
    }
#endif
    return HDF_SUCCESS;
}

int32_t AudioOutputRenderStartPrepare(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    struct HdfIoService *service = NULL;
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderStartPrepare Service is NULL!");
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, NULL, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderStartPrepare Failed to send service call cmdId = %d!", cmdId);
        return ret;
    }
    return HDF_SUCCESS;
}

int32_t AudioOutputRenderOpen(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    struct HdfIoService *service = NULL;
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderStartPrepare Service is NULL!");
        return HDF_FAILURE;
    }
    uint32_t card = handleData->renderMode.hwInfo.card;
    if (card >= AUDIO_SERVICE_MAX) {
        LOG_FUN_ERR("card is Error!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(sBuf, g_audioLibRenderService[card])) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderStartPrepare Failed to send service call cmdId = %d!", cmdId);
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return ret;
}

int32_t AudioOutputRenderStop(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    if (pcm) {
        pcm_close(pcm);
        pcm = NULL;
    }
    RoutePcmClose(DEV_OFF_PLAYBACK_OFF_ROUTE);
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("RenderStop Service is NULL!");
        return HDF_FAILURE;
    }
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, handleData->renderMode.ctlParam.turnStandbyStatus)) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    int32_t ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("RenderStop Failed to send service call!");
        AudioBufReplyRecycle(sBuf, NULL);
        return ret;
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t MmapDescWriteBuffer(struct HdfSBuf *sBuf, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (sBuf == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    uint64_t mmapAddr = (uint64_t)(uintptr_t)(handleData->frameRenderMode.mmapBufDesc.memoryAddress);
    if (!HdfSbufWriteUint64(sBuf, mmapAddr)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameRenderMode.mmapBufDesc.memoryFd)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameRenderMode.mmapBufDesc.totalBufferFrames)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameRenderMode.mmapBufDesc.transferFrameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(sBuf, handleData->frameRenderMode.mmapBufDesc.isShareable)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(sBuf, handleData->frameRenderMode.mmapBufDesc.offset)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioOutputRenderReqMmapBuffer(const struct DevHandle *handle,
    int cmdId, const struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    int32_t ret;
    struct HdfSBuf *sBuf = AudioObtainHdfSBuf();
    if (sBuf == NULL) {
        return HDF_FAILURE;
    }
    if (MmapDescWriteBuffer(sBuf, handleData)) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        AudioBufReplyRecycle(sBuf, NULL);
        LOG_FUN_ERR("Failed to send service call!");
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, sBuf, NULL);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(sBuf, NULL);
        return HDF_FAILURE;
    }
    AudioBufReplyRecycle(sBuf, NULL);
    return HDF_SUCCESS;
}

int32_t AudioOutputRenderGetMmapPosition(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData)
{
#ifdef ALSA_MODE
    return HDF_SUCCESS;
#endif
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    struct HdfIoService *service = NULL;
    struct HdfSBuf *reply = NULL;
    int32_t ret;
    service = (struct HdfIoService *)handle->object;
    if (service == NULL || service->dispatcher == NULL || service->dispatcher->Dispatch == NULL) {
        LOG_FUN_ERR("Failed to send service call!");
        return HDF_FAILURE;
    }
    reply = AudioObtainHdfSBuf();
    if (reply == NULL) {
        LOG_FUN_ERR("RenderGetMmapPosition Failed to obtain reply");
        return HDF_FAILURE;
    }
    ret = service->dispatcher->Dispatch(&service->object, cmdId, NULL, reply);
    if (ret != HDF_SUCCESS) {
        AudioBufReplyRecycle(NULL, reply);
        return HDF_FAILURE;
    }
    uint64_t frames = 0;
    if (!HdfSbufReadUint64(reply, &frames)) {
        LOG_FUN_ERR("Failed to  Get Mmap Position sBuf!");
        AudioBufReplyRecycle(NULL, reply);
        return HDF_FAILURE;
    }
    handleData->frameRenderMode.frames = frames;
    AudioBufReplyRecycle(NULL, reply);
    return HDF_SUCCESS;
}

int32_t AudioInterfaceLibOutputRender(const struct DevHandle *handle,
    int cmdId, struct AudioHwRenderParam *handleData)
{
    if (handle == NULL) {
        LOG_FUN_ERR("Input Render handle is NULL!");
        return HDF_FAILURE;
    }
    if (handle->object == NULL || handleData == NULL) {
        return HDF_FAILURE;
    }
    int32_t ret;
    switch (cmdId) {
        case AUDIO_DRV_PCM_IOCTL_HW_PARAMS:
            ret = AudioOutputRenderHwParams(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTL_WRITE:
            ret = AudioOutputRenderWrite(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTRL_STOP:
            ret = AudioOutputRenderStop(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTRL_START:
        case AUDIO_DRV_PCM_IOCTL_PREPARE:
        case AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE:
            ret = AudioOutputRenderStartPrepare(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN:
            ret = AudioOutputRenderOpen(handle, cmdId, handleData);
            break;
        case AUDIODRV_CTL_IOCTL_PAUSE_WRITE:
            ret = AudioCtlRenderSetPauseStu(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER:
            ret = AudioOutputRenderReqMmapBuffer(handle, cmdId, handleData);
            break;
        case AUDIO_DRV_PCM_IOCTL_MMAP_POSITION:
            ret = (AudioOutputRenderGetMmapPosition(handle, cmdId, handleData));
            break;
        default:
            LOG_FUN_ERR("Output Mode not support!");
            ret = HDF_FAILURE;
            break;
    }
    return ret;
}

int32_t AudioBindServiceRenderObject(struct DevHandle *handle, const char *name)
{
    if (handle == NULL || name == NULL) {
        LOG_FUN_ERR("service name or handle is NULL!");
        return HDF_FAILURE;
    }
    char *serviceName = (char *)calloc(1, NAME_LEN);
    if (serviceName == NULL) {
        LOG_FUN_ERR("Failed to OsalMemCalloc serviceName");
        return HDF_FAILURE;
    }
    int ret = snprintf_s(serviceName, NAME_LEN - 1, SERVIC_NAME_MAX_LEN + 1, "hdf_audio_%s", name);
    if (ret < 0) {
        LOG_FUN_ERR("Failed to snprintf_s");
        AudioMemFree((void **)&serviceName);
        return HDF_FAILURE;
    }
    struct HdfIoService *service = HdfIoServiceBindName(serviceName);
    if (service == NULL) {
        LOG_FUN_ERR("Failed to get service!");
        AudioMemFree((void **)&serviceName);
        return HDF_FAILURE;
    }
    AudioMemFree((void **)&serviceName);
    handle->object = service;
    return HDF_SUCCESS;
}

/* CreatRender for Bind handle */
struct DevHandle *AudioBindServiceRender(const char *name)
{
    struct DevHandle *handle = NULL;
    if (name == NULL) {
        LOG_FUN_ERR("service name NULL!");
        return NULL;
    }
    handle = (struct DevHandle *)calloc(1, sizeof(struct DevHandle));
    if (handle == NULL) {
        LOG_FUN_ERR("Failed to OsalMemCalloc handle");
        return NULL;
    }
    int32_t ret = AudioBindServiceRenderObject(handle, name);
    if (ret != HDF_SUCCESS) {
        LOG_FUN_ERR("handle->object is NULL!");
        AudioMemFree((void **)&handle);
        return NULL;
    }
    LOG_PARA_INFO("BIND SERVICE SUCCESS!");
    return handle;
}

void AudioCloseServiceRender(const struct DevHandle *handle)
{
#ifdef ALSA_MODE
    return;
#endif
    if (handle == NULL || handle->object == NULL) {
        LOG_FUN_ERR("Render handle or handle->object is NULL");
        return;
    }
    struct HdfIoService *service = (struct HdfIoService *)handle->object;
    HdfIoServiceRecycle(service);
    AudioMemFree((void **)&handle);
    return;
}

int32_t AudioInterfaceLibModeRender(const struct DevHandle *handle,
    struct AudioHwRenderParam *handleData, int cmdId)
{
    if (handle == NULL || handle->object == NULL || handleData == NULL) {
        LOG_FUN_ERR("paras is NULL!");
        return HDF_FAILURE;
    }
    switch (cmdId) {
        case AUDIO_DRV_PCM_IOCTL_HW_PARAMS:
        case AUDIO_DRV_PCM_IOCTL_WRITE:
        case AUDIO_DRV_PCM_IOCTRL_STOP:
        case AUDIO_DRV_PCM_IOCTRL_START:
        case AUDIO_DRV_PCM_IOCTL_PREPARE:
        case AUDIODRV_CTL_IOCTL_PAUSE_WRITE:
        case AUDIO_DRV_PCM_IOCTL_MMAP_BUFFER:
        case AUDIO_DRV_PCM_IOCTL_MMAP_POSITION:
        case AUDIO_DRV_PCM_IOCTRL_RENDER_OPEN:
        case AUDIO_DRV_PCM_IOCTRL_RENDER_CLOSE:
            return (AudioInterfaceLibOutputRender(handle, cmdId, handleData));
        case AUDIODRV_CTL_IOCTL_ELEM_WRITE:
        case AUDIODRV_CTL_IOCTL_ELEM_READ:
        case AUDIODRV_CTL_IOCTL_MUTE_WRITE:
        case AUDIODRV_CTL_IOCTL_MUTE_READ:
        case AUDIODRV_CTL_IOCTL_GAIN_WRITE:
        case AUDIODRV_CTL_IOCTL_GAIN_READ:
        case AUDIODRV_CTL_IOCTL_CHANNEL_MODE_WRITE:
        case AUDIODRV_CTL_IOCTL_CHANNEL_MODE_READ:
        case AUDIODRV_CTL_IOCTL_SCENESELECT_WRITE:
        case AUDIODRV_CTL_IOCTL_GAINTHRESHOLD_READ:
        case AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_IN:
        case AUDIODRV_CTL_IOCTL_ACODEC_CHANGE_OUT:
        case AUDIODRV_CTL_IOCTL_VOL_THRESHOLD_READ:
            return (AudioInterfaceLibCtlRender(handle, cmdId, handleData));
        default:
            LOG_FUN_ERR("Mode Error!");
            break;
    }
    return HDF_ERR_NOT_SUPPORT;
}

