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
#include <hdf_remote_service.h>
#include <servmgr_hdi.h>
#include "audio_proxy_common.h"

namespace OHOS::HDI::Audio_Bluetooth {
constexpr int PROXY_VOLUME_CHANGE = 100;

struct HdfSBuf *AudioProxyObtainHdfSBuf()
{
    HdfSbufType bufType = SBUF_IPC;
    return HdfSbufTypedObtain(bufType);
}

int32_t AudioProxyDispatchCall(struct HdfRemoteService *self,
    int32_t id, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data == NULL || reply == NULL || self == NULL) {
        HDF_LOGE("AudioProxyDispatchCall parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (self->dispatcher == NULL || self->dispatcher->Dispatch == NULL) {
        HDF_LOGE("AudioProxyDispatchCall Remote obj is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    return self->dispatcher->Dispatch(self, id, data, reply);
}

void AudioProxyBufReplyRecycle(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return;
}

int32_t AudioProxyPreprocessSBuf(struct HdfSBuf **data, struct HdfSBuf **reply)
{
    if (data == NULL || reply == NULL) {
        return HDF_FAILURE;
    }
    *data = AudioProxyObtainHdfSBuf();
    if (*data == NULL) {
        HDF_LOGE("Failed to obtain data");
        return HDF_FAILURE;
    }
    *reply = AudioProxyObtainHdfSBuf();
    if (*reply == NULL) {
        HDF_LOGE("Failed to obtain reply");
        HdfSbufRecycle(*data);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyPreprocessRender(struct AudioHwRender *render, struct HdfSBuf **data, struct HdfSBuf **reply)
{
    struct AudioHwRender *hwRender = render;
    if (hwRender == NULL || data == NULL || reply == NULL) {
        return HDF_FAILURE;
    }
    uint32_t renderPid = (uint32_t)getpid();
    const char *adapterName = hwRender->renderParam.renderMode.hwInfo.adapterName;
    if (adapterName == NULL) {
        HDF_LOGE("adapterName is NULL");
        return HDF_FAILURE;
    }
    if (AudioProxyPreprocessSBuf(data, reply) < 0) {
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(render->proxyRemoteHandle, *data)) {
        AudioProxyBufReplyRecycle(*data, *reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteString(*data, adapterName)) {
        AudioProxyBufReplyRecycle(*data, *reply);
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(*data, renderPid)) {
        AudioProxyBufReplyRecycle(*data, *reply);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyWriteSampleAttributes(struct HdfSBuf *data, const struct AudioSampleAttributes *attrs)
{
    if (data == NULL || attrs == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)attrs->type)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)attrs->interleaved)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)attrs->format)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->sampleRate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->channelCount)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->period)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->frameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(attrs->isBigEndian))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(attrs->isSignedData))) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->startThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->stopThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, attrs->silenceThreshold)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyReadSapmleAttrbutes(struct HdfSBuf *reply, struct AudioSampleAttributes *attrs)
{
    if (reply == NULL || attrs == NULL) {
        return HDF_FAILURE;
    }
    uint32_t tempType = 0;
    if (!HdfSbufReadUint32(reply, &tempType)) {
        return HDF_FAILURE;
    }
    attrs->type = (AudioCategory)tempType;
    uint32_t tempInterleaved = 0;
    if (!HdfSbufReadUint32(reply, &tempInterleaved)) {
        return HDF_FAILURE;
    }
    attrs->interleaved = (bool)tempInterleaved;
    uint32_t tempFormat = 0;
    if (!HdfSbufReadUint32(reply, &tempFormat)) {
        return HDF_FAILURE;
    }
    attrs->format = (AudioFormat)tempFormat;
    if (!HdfSbufReadUint32(reply, &attrs->sampleRate)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &attrs->channelCount)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &attrs->period)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &attrs->frameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &tempInterleaved)) {
        return HDF_FAILURE;
    }
    attrs->isBigEndian = (bool)tempInterleaved;
    if (!HdfSbufReadUint32(reply, &tempInterleaved)) {
        return HDF_FAILURE;
    }
    attrs->isSignedData = (bool)tempInterleaved;
    if (!HdfSbufReadUint32(reply, &attrs->startThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &attrs->stopThreshold)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint32(reply, &attrs->silenceThreshold)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyCommonSetRenderCtrlParam(int cmId, AudioHandle handle, float param)
{
    HDF_LOGI("%{public}s, ", __func__);
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The hwRender parameter is null");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (param < 0) {
        HDF_LOGE("Set param is invalid, Please check param!");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (cmId == AUDIO_HDI_RENDER_SET_VOLUME) {
        if (param > 1.0) {
            HDF_LOGE("volume param Is error!");
            return AUDIO_HAL_ERR_INVALID_PARAM;
        }
        param = param * PROXY_VOLUME_CHANGE;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    uint32_t tempParam = (uint32_t)param;
    if (!HdfSbufWriteUint32(data, tempParam)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, cmId, data, reply);
    AudioProxyBufReplyRecycle(data, reply);
    return ret;
}

int32_t AudioProxyCommonGetRenderCtrlParam(int cmId, AudioHandle handle, float *param)
{
    HDF_LOGI("%{public}s, ", __func__);
    if (param == NULL) {
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    struct AudioHwRender *hwRender = (struct AudioHwRender *)handle;
    if (hwRender == NULL || hwRender->proxyRemoteHandle == NULL) {
        HDF_LOGE("The pointer is empty");
        return AUDIO_HAL_ERR_INVALID_PARAM;
    }
    if (AudioProxyPreprocessRender(hwRender, &data, &reply) < 0) {
        return AUDIO_HAL_ERR_INTERNAL;
    }
    int32_t ret = AudioProxyDispatchCall(hwRender->proxyRemoteHandle, cmId, data, reply);
    if (ret < 0) {
        AudioProxyBufReplyRecycle(data, reply);
        return ret;
    }
    uint32_t tempParam = 0;
    if (!HdfSbufReadUint32(reply, &tempParam)) {
        AudioProxyBufReplyRecycle(data, reply);
        return AUDIO_HAL_ERR_INTERNAL;
    }
    if (cmId == AUDIO_HDI_RENDER_GET_VOLUME) {
        *param = (float)tempParam / PROXY_VOLUME_CHANGE;
    } else {
        *param = (float)tempParam;
    }
    AudioProxyBufReplyRecycle(data, reply);
    return AUDIO_HAL_SUCCESS;
}

int32_t AudioProxyGetMmapPositionRead(struct HdfSBuf *reply, uint64_t *frames, struct AudioTimeStamp *time)
{
    if (reply == NULL || frames == NULL || time == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadUint64(reply, frames)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt64(reply, &time->tvSec)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufReadInt64(reply, &time->tvNSec)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t AudioProxyReqMmapBufferWrite(struct HdfSBuf *data, int32_t reqSize,
    const struct AudioMmapBufferDescripter *desc)
{
    if (data == NULL || desc == NULL) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, reqSize)) {
        return HDF_FAILURE;
    }
    uint64_t memAddr = (uint64_t)(uintptr_t)desc->memoryAddress;
    if (!HdfSbufWriteUint64(data, memAddr)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteFileDescriptor(data, desc->memoryFd)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, desc->totalBufferFrames)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, desc->transferFrameSize)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteInt32(data, desc->isShareable)) {
        return HDF_FAILURE;
    }
    if (!HdfSbufWriteUint32(data, desc->offset)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
}