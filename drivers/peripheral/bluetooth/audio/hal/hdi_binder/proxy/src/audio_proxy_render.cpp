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
#include <hdf_log.h>
#include "audio_proxy_common.h"

namespace OHOS::HDI::Audio_Bluetooth {
int32_t AudioProxyRenderCtrl(int cmId, AudioHandle handle)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, cmId, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderStart(AudioHandle handle)
{
    HDF_LOGI("%{public}s, ", __func__);
    return AudioProxyRenderCtrl(AUDIO_HDI_RENDER_START, handle);
}

int32_t AudioProxyRenderStop(AudioHandle handle)
{
    HDF_LOGI("%{public}s, ", __func__);
    return AudioProxyRenderCtrl(AUDIO_HDI_RENDER_STOP, handle);
}

int32_t AudioProxyRenderPause(AudioHandle handle)
{
    HDF_LOGI("%{public}s, ", __func__);
    return AudioProxyRenderCtrl(AUDIO_HDI_RENDER_PAUSE, handle);
}

int32_t AudioProxyRenderResume(AudioHandle handle)
{
    HDF_LOGI("%{public}s, ", __func__);
    return AudioProxyRenderCtrl(AUDIO_HDI_RENDER_RESUME, handle);
}

int32_t AudioProxyRenderFlush(AudioHandle handle)
{
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioProxyRenderGetFrameParameter(int cmId, const AudioHandle handle, uint64_t *param)
{
    if (param == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("invalid pointer");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, cmId, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint64(reply, param)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderGetFrameSize(const AudioHandle handle, uint64_t *size)
{
    return AudioProxyRenderGetFrameParameter(AUDIO_HDI_RENDER_GET_FRAME_SIZE, handle, size);
}

int32_t AudioProxyRenderGetFrameCount(const AudioHandle handle, uint64_t *count)
{
    return AudioProxyRenderGetFrameParameter(AUDIO_HDI_RENDER_GET_FRAME_COUNT, handle, count);
}

int32_t AudioProxyRenderSetSampleAttributes(const AudioHandle handle, const struct AudioSampleAttributes *attrs)
{
    if (handle == NULL || attrs == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("pointer is invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyWriteSampleAttributes(data, attrs) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_SET_SAMPLE_ATTR, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderGetSampleAttributes(const AudioHandle handle,
    struct AudioSampleAttributes *attrs)
{
    if (attrs == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_SAMPLE_ATTR, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderGetSampleAttributes FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (AudioProxyReadSapmleAttrbutes(reply, attrs)) {
        HDF_LOGE("AudioProxyReadSapmleAttrbutes FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderGetCurrentChannelId(const AudioHandle handle, uint32_t *channelId)
{
    if (channelId == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("pointer is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_CUR_CHANNEL_ID, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderGetCurrentChannelId FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint32(reply, channelId)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderCheckSceneCapability(const AudioHandle handle,
    const struct AudioSceneDescriptor *scene, bool *supported)
{
    if (scene == NULL || supported == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    uint32_t tempPins;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender pointer is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyPreprocessRender FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteUint32(data, scene->scene.id)) {
        HDF_LOGE("scene->scene.id Write FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    tempPins = (uint32_t)scene->desc.pins;
    if (!HdfSbufWriteUint32(data, tempPins)) {
        HDF_LOGE("tempPins Write FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle,
        AUDIO_HDI_RENDER_CHECK_SCENE_CAPABILITY, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderCheckSceneCapability FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t tempSupported = 0;
    if (!HdfSbufReadUint32(reply, &tempSupported)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *supported = (bool)tempSupported;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderSelectScene(const AudioHandle handle, const struct AudioSceneDescriptor *scene)
{
    if (scene == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteUint32(data, scene->scene.id)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t temporaryPins = scene->desc.pins;
    if (!HdfSbufWriteUint32(data, temporaryPins)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_SELECT_SCENE, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderSetMute(const AudioHandle handle, bool mute)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The pointer is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempMute = (uint32_t)mute;
    if (!HdfSbufWriteUint32(data, tempMute)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_SET_MUTE, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderGetMute(const AudioHandle handle, bool *mute)
{
    if (mute == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_MUTE, data, reply);
    uint32_t tempMute = 0;
    if (!HdfSbufReadUint32(reply, &tempMute)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *mute = (bool)tempMute;
    AudioProxyBufReplyRecycle(data, reply);
    LOG_PARA_INFO("GetMute SUCCESS!");
    return ret;
}

int32_t AudioProxyRenderSetVolume(const AudioHandle handle, float volume)
{
    HDF_LOGI("%{public}s, ", __func__);
    return AudioProxyCommonSetRenderCtrlParam(AUDIO_HDI_RENDER_SET_VOLUME, handle, volume);
}

int32_t AudioProxyRenderGetVolume(const AudioHandle handle, float *volume)
{
    HDF_LOGI("%{public}s, ", __func__);
    return AudioProxyCommonGetRenderCtrlParam(AUDIO_HDI_RENDER_GET_VOLUME, handle, volume);
}

int32_t AudioProxyRenderGetGainThreshold(const AudioHandle handle, float *min, float *max)
{
    if (min == NULL || max == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The pointer is invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_GAIN_THRESHOLD, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderGetGainThreshold FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t tempMin = 0;
    uint32_t tempMax = 0;
    if (!HdfSbufReadUint32(reply, &tempMin)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufReadUint32(reply, &tempMax)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *min = (float)tempMin;
    *max = (float)tempMax;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderGetGain(const AudioHandle handle, float *gain)
{
    return AudioProxyCommonGetRenderCtrlParam(AUDIO_HDI_RENDER_GET_GAIN, handle, gain);
}

int32_t AudioProxyRenderSetGain(const AudioHandle handle, float gain)
{
    return AudioProxyCommonSetRenderCtrlParam(AUDIO_HDI_RENDER_SET_GAIN, handle, gain);
}

int32_t AudioProxyRenderGetLatency(struct AudioRender *render, uint32_t *ms)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL || ms == NULL) {
        HDF_LOGE("The pointer is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_LATENCY, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderGetLatency FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint32(reply, ms)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderRenderFrame(struct AudioRender *render, const void *frame,
                                    uint64_t requestBytes, uint64_t *replyBytes)
{
    HDF_LOGI("%{public}s, ", __func__);
    if (frame == NULL || replyBytes == NULL) {
        HDF_LOGE("Render Frame Paras is NULL!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyPreprocessRender FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteBuffer(data, frame, (uint32_t)requestBytes)) {
        AudioProxyBufReplyRecycle(data, reply);
        HDF_LOGE("HdfSbufWriteBuffer FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_RENDER_FRAME, data, reply);
    if (ret < 0) {
        if (ret != AUDIO_HAL_ERR_INVALID_OBJECT) {
            HDF_LOGE("AudioRenderRenderFrame FAIL");
        }
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    *replyBytes = requestBytes;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderGetRenderPosition(struct AudioRender *render, uint64_t *frames, struct AudioTimeStamp *time)
{
    if (frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("hwRender or hwRender->proxyRemoteHandle invalid");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyPreprocessRender FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle,
        AUDIO_HDI_RENDER_GET_RENDER_POSITION, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderGetRenderPosition FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    if (AudioProxyGetMmapPositionRead(reply, frames, time) < 0) {
        HDF_LOGE("AudioProxyGetMmapPositionRead FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderSetRenderSpeed(struct AudioRender *render, float speed)
{
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioProxyRenderGetRenderSpeed(struct AudioRender *render, float *speed)
{
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || speed == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return AUDIO_HAL_ERR_NOT_SUPPORT;
}

int32_t AudioProxyRenderSetChannelMode(struct AudioRender *render, AudioChannelMode mode)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyPreprocessRender FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempMode = (uint32_t)mode;
    if (!HdfSbufWriteUint32(data, tempMode)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_SET_CHANNEL_MODE, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderGetChannelMode(struct AudioRender *render, AudioChannelMode *mode)
{
    if (mode == NULL || render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("hwRender or hwRender->proxyRemoteHandle is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyPreprocessRender FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_CHANNEL_MODE, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioRenderGetChannelMode FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t tempMode = 0;
    if (!HdfSbufReadUint32(reply, &tempMode)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    *mode = (AudioChannelMode)tempMode;
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderSetExtraParams(AudioHandle handle, const char *keyValueList)
{
    if (handle == NULL || keyValueList == NULL) {
        HDF_LOGE("The parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteString(data, keyValueList)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_SET_EXTRA_PARAMS, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyRenderGetExtraParams(AudioHandle handle, char *keyValueList, int32_t listLenth)
{
    LOG_PARA_INFO("AudioProxyRenderGetExtraParams enter!");
    if (handle == NULL || keyValueList == NULL || listLenth <= 0) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderGetExtraParams FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteInt32(data, listLenth)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_EXTRA_PARAMS, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioProxyRenderGetExtraParams FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    const char *strKeyValueList = NULL;
    if ((strKeyValueList = HdfSbufReadString(reply)) == NULL) {
        HDF_LOGE("keyValueList Is empty");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    ret = strncpy_s(keyValueList, listLenth, strKeyValueList, strlen(strKeyValueList));
    if (ret != 0) {
        HDF_LOGE("Copy failed!");
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderReqMmapBuffer(AudioHandle handle, int32_t reqSize, struct AudioMmapBufferDescripter *desc)
{
    if (handle == NULL || desc == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("hwRender or hwRender->proxyRemoteHandle is NULL");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderReqMmapBuffer FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (AudioProxyReqMmapBufferWrite(data, reqSize, desc) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_REQ_MMAP_BUFFER, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioProxyRenderReqMmapBuffer FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }

    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyRenderGetMmapPosition(AudioHandle handle, uint64_t *frames, struct AudioTimeStamp *time)
{
    if (handle == NULL || frames == NULL || time == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderGetMmapPosition FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_GET_MMAP_POSITION, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        HDF_LOGE("AudioProxyRenderGetMmapPosition FAIL");
        return ret;
    }
    if (AudioProxyGetMmapPositionRead(reply, frames, time) < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyRenderTurnStandbyMode(AudioHandle handle)
{
    if (handle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }

    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderTurnStandbyMode FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_TURN_STAND_BY_MODE, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioProxyRenderTurnStandbyMode FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyRenderAudioDevDump(AudioHandle handle, int32_t range, int32_t fd)
{
    if (handle == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(handle);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderAudioDevDump FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteInt32(data, range)) {
        AudioProxyBufReplyRecycle(data, reply);
        HDF_LOGE("Write FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (!HdfSbufWriteInt32(data, fd)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_DEV_DUMP, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioProxyRenderAudioDevDump FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyRenderRegCallback(struct AudioRender *render, RenderCallback callback, void *cookie)
{
    if (render == NULL || callback == NULL || cookie == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderRegCallback FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint64_t tempAddr = (uint64_t)(uintptr_t)cookie;
    if (!HdfSbufWriteUint64(data, tempAddr)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    tempAddr = (uint64_t)callback;
    if (!HdfSbufWriteUint64(data, tempAddr)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_REG_CALLBACK, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioProxyRenderRegCallback FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
int32_t AudioProxyRenderDrainBuffer(struct AudioRender *render, AudioDrainNotifyType *type)
{
    if (render == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = reinterpret_cast<struct AudioHwRender *>(render);
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender parameter is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        HDF_LOGE("AudioProxyRenderDrainBuffer FAIL");
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempType = (uint32_t)(uintptr_t)type;
    if (!HdfSbufWriteUint32(data, tempType)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, AUDIO_HDI_RENDER_DRAIN_BUFFER, data, reply);
    if (ret < 0) {
        HDF_LOGE("AudioProxyRenderDrainBuffer FAIL");
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}
}