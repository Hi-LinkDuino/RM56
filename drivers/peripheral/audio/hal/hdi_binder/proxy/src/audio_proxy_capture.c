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

#include "audio_proxy_common.h"
#include "audio_hal_log.h"

#define HDF_LOG_TAG HDF_AUDIO_HAL_PROXY

static int32_t AudioProxyCaptureCtrl(int cmId, const AudioHandle handle)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The pointer is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, cmId, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCaptureStart(const AudioHandle handle)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCaptureCtrl(AUDIO_HDI_CAPTURE_START, handle);
}

int32_t AudioProxyCaptureStop(const AudioHandle handle)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCaptureCtrl(AUDIO_HDI_CAPTURE_STOP, handle);
}

int32_t AudioProxyCapturePause(const AudioHandle handle)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCaptureCtrl(AUDIO_HDI_CAPTURE_PAUSE, handle);
}

int32_t AudioProxyCaptureResume(const AudioHandle handle)
{
    LOG_FUN_INFO();
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCaptureCtrl(AUDIO_HDI_CAPTURE_RESUME, handle);
}

int32_t AudioProxyCaptureFlush(const AudioHandle handle)
{
    LOG_FUN_INFO();
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioProxyCaptureGetFrameParameter(int cmId, const AudioHandle handle, uint64_t *param)
{
    if (param == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The pointer is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, cmId, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureGetFrameSize FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint64(reply, param)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCaptureGetFrameSize(const AudioHandle handle, uint64_t *size)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCaptureGetFrameParameter(AUDIO_HDI_CAPTURE_GET_FRAME_SIZE, handle, size);
}

int32_t AudioProxyCaptureGetFrameCount(const AudioHandle handle, uint64_t *count)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCaptureGetFrameParameter(AUDIO_HDI_CAPTURE_GET_FRAME_COUNT, handle, count);
}

int32_t AudioProxyCaptureSetSampleAttributes(const AudioHandle handle, const struct AudioSampleAttributes *attrs)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL || attrs == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("hwCapture or hwCapture->proxyRemoteHandle is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyPreprocessCapture Fail");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyWriteSampleAttributes(data, attrs) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_SET_SAMPLE_ATTR, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCaptureGetSampleAttributes(const AudioHandle handle, struct AudioSampleAttributes *attrs)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (attrs == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The Invalid is pointer");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_GET_SAMPLE_ATTR, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureGetSampleAttributes FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (AudioProxyReadSapmleAttrbutes(reply, attrs)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureGetCurrentChannelId(const AudioHandle handle, uint32_t *channelId)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (channelId == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("hwCapture parameter is invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle,
        AUDIO_HDI_CAPTURE_GET_CUR_CHANNEL_ID, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureGetFrameSize FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint32(reply, channelId)) {
        LOG_FUN_ERR("Read reply FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureCheckSceneCapability(const AudioHandle handle,
    const struct AudioSceneDescriptor *scene, bool *supported)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (scene == NULL || supported == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t tempPins;
    uint32_t tempSupported = 0;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("pointer invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteUint32(data, scene->scene.id)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    tempPins = scene->desc.pins;
    if (!HdfSbufWriteUint32(data, tempPins)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle,
        AUDIO_HDI_CAPTURE_CHECK_SCENE_CAPABILITY, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioProxyCaptureCheckSceneCapability FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint32(reply, &tempSupported)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *supported = (bool)tempSupported;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureSelectScene(const AudioHandle handle, const struct AudioSceneDescriptor *scene)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (scene == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The hwCapture pointer is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteUint32(data, (enum AudioCategory)scene->scene.id)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempPins = scene->desc.pins;
    if (!HdfSbufWriteUint32(data, tempPins)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_SELECT_SCENE, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCaptureSetMute(const AudioHandle handle, bool mute)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The hwCapture parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempMute = (uint32_t)mute;
    if (!HdfSbufWriteUint32(data, tempMute)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_SET_MUTE, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCaptureGetMute(const AudioHandle handle, bool *mute)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (mute == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The hwCapture parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_GET_MUTE, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureGetMute FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t tempMute = 0;
    if (!HdfSbufReadUint32(reply, &tempMute)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *mute = (bool)tempMute;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureSetVolume(const AudioHandle handle, float volume)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCommonSetCaptureCtrlParam(AUDIO_HDI_CAPTURE_SET_VOLUME, handle, volume);
}

int32_t AudioProxyCaptureGetVolume(const AudioHandle handle, float *volume)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCommonGetCaptureCtrlParam(AUDIO_HDI_CAPTURE_GET_VOLUME, handle, volume);
}

int32_t AudioProxyCaptureGetGainThreshold(const AudioHandle handle, float *min, float *max)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (min == NULL || max == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The hwCapture pointer is invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle,
        AUDIO_HDI_CAPTURE_GET_GAIN_THRESHOLD, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureGetGainThreshold FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t temp = 0;
    if (!HdfSbufReadUint32(reply, &temp)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *min = temp;
    if (!HdfSbufReadUint32(reply, &temp)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *max = temp;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureGetGain(const AudioHandle handle, float *gain)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCommonGetCaptureCtrlParam(AUDIO_HDI_CAPTURE_GET_GAIN, handle, gain);
}

int32_t AudioProxyCaptureSetGain(const AudioHandle handle, float gain)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    return AudioProxyCommonSetCaptureCtrlParam(AUDIO_HDI_CAPTURE_SET_GAIN, handle, gain);
}

int32_t AudioProxyCaptureCaptureFrameSplit(struct AudioCapture *capture,
    uint64_t requestBytes, struct HdfSBuf **data, struct HdfSBuf **reply)
{
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL || data == NULL || reply == NULL) {
        LOG_FUN_ERR("The pointer is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, data, reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteUint64(*data, requestBytes)) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_CAPTURE_FRAME, *data, *reply);
    if (ret < 0) {
        if (ret != AUDIO_HAL_ERR_INVALID_OBJECT) {
            LOG_FUN_ERR("AudioCaptureCaptureFrame FAIL");
        }
        return ret;
    }
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureCaptureFrame(struct AudioCapture *capture, void *frame,
                                      uint64_t requestBytes, uint64_t *replyBytes)
{
    int32_t ret = AudioCheckCaptureAddr((AudioHandle)capture); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    const char *buffer = NULL;
    uint32_t length = 0;
    if (frame == NULL || replyBytes == NULL) {
        LOG_FUN_ERR("capture Frame Paras is NULL!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    ret = AudioProxyCaptureCaptureFrameSplit(capture, requestBytes, &data, &reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioProxyCaptureCaptureFrameSplit Fail!");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadBuffer(reply, (const void **)&buffer, &length)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if ((uint64_t)length > requestBytes) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = memcpy_s(frame, (size_t)requestBytes, buffer, (size_t)length);
    if (ret != EOK) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufReadUint64(reply, replyBytes)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureGetCapturePosition(struct AudioCapture *capture,
    uint64_t *frames, struct AudioTimeStamp *time)
{
    int32_t ret = AudioCheckCaptureAddr((AudioHandle)capture); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)capture;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The hwCapture parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture(hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle,
        AUDIO_HDI_CAPTURE_GET_CAPTURE_POSITION, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioCaptureGetCapturePosition FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (AudioProxyGetMmapPositionRead(reply, frames, time) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}
int32_t AudioProxyCaptureSetExtraParams(const AudioHandle handle, const char *keyValueList)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL || keyValueList == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("hwCapture or hwCapture->proxyRemoteHandle is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture((AudioHandle)hwCapture, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteString(data, keyValueList)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_SET_EXTRA_PARAMS, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyCaptureGetExtraParams(const AudioHandle handle, char *keyValueList, int32_t listLenth)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL || keyValueList == NULL || listLenth <= 0) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture((AudioHandle)hwCapture, &data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyCaptureGetExtraParams FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteInt32(data, listLenth)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_GET_EXTRA_PARAMS, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioProxyCaptureGetExtraParams FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    const char *strKeyValueList = NULL;
    if ((strKeyValueList = HdfSbufReadString(reply)) == NULL) {
        LOG_FUN_ERR("keyValueList Is NULL");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = strncpy_s(keyValueList, listLenth, strKeyValueList, strlen(strKeyValueList));
    if (ret != 0) {
        LOG_FUN_ERR("strncpy_s failed!");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyCaptureReqMmapBuffer(const AudioHandle handle,
    int32_t reqSize, struct AudioMmapBufferDescripter *desc)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL || desc == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("hwCapture parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture((AudioHandle)hwCapture, &data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyCaptureReqMmapBuffer FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyReqMmapBufferWrite(data, reqSize, desc) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_REQ_MMAP_BUFFER, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioProxyCaptureReqMmapBuffer FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }

    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCaptureGetMmapPosition(const AudioHandle handle, uint64_t *frames, struct AudioTimeStamp *time)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL || frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture((AudioHandle)hwCapture, &data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyCaptureGetMmapPosition FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle,
        AUDIO_HDI_CAPTURE_GET_MMAP_POSITION, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        LOG_FUN_ERR("AudioProxyCaptureGetMmapPosition FAIL");
        return ret;
    }
    if (AudioProxyGetMmapPositionRead(reply, frames, time) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        LOG_FUN_ERR("AudioProxyGetMmapPositionRead FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyCaptureTurnStandbyMode(const AudioHandle handle)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("The hwCapture parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessCapture((AudioHandle)hwCapture, &data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyCaptureTurnStandbyMode FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle,
        AUDIO_HDI_CAPTURE_TURN_STAND_BY_MODE, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioProxyCaptureTurnStandbyMode FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyCaptureAudioDevDump(const AudioHandle handle, int32_t range, int32_t fd)
{
    int32_t ret = AudioCheckCaptureAddr(handle); // Fuzz test
    if (ret < 0) {
        LOG_FUN_ERR("The proxy capture address passed in is invalid");
        return ret;
    }
    if (handle == NULL) {
        LOG_FUN_ERR("handle is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwCapture *hwCapture = (struct AudioHwCapture *)handle;
    if (hwCapture == NULL || hwCapture->proxyRemoteHandle == NULL) {
        LOG_FUN_ERR("hwCapture parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    if (AudioProxyPreprocessCapture((AudioHandle)hwCapture, &data, &reply) < 0) {
        LOG_FUN_ERR("AudioProxyCaptureAudioDevDump FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteInt32(data, range)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteFileDescriptor(data, fd)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwCapture->proxyRemoteHandle, AUDIO_HDI_CAPTURE_DEV_DUMP, data, reply);
    if (ret < 0) {
        LOG_FUN_ERR("AudioProxyCaptureAudioDevDump FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

