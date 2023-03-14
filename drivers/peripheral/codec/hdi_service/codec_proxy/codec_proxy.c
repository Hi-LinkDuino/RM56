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

#include "proxy_msgproc.h"
#include <hdf_log.h>
#include <osal_mem.h>
#include <servmgr_hdi.h>
#include "icodec.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static int32_t CodecProxyCall(struct ICodec *self,
    int32_t id, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (self->remote == NULL || self->remote->dispatcher == NULL ||
        self->remote->dispatcher->Dispatch == NULL) {
            HDF_LOGE("%{public}s: obj is null", __func__);
            return HDF_ERR_INVALID_OBJECT;
    }
    return self->remote->dispatcher->Dispatch(self->remote, id, data, reply);
}

int32_t CodecProxyReqSBuf(struct HdfSBuf **data, struct HdfSBuf **reply)
{
    *data = HdfSbufTypedObtain(SBUF_IPC);
    if (*data == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain", __func__);
        return HDF_FAILURE;
    }
    *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (*reply == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain reply", __func__);
        HdfSbufRecycle(*data);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
void CodecProxySBufRecycle(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return;
}

int32_t CodecPorxyInit(struct ICodec *self)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_INIT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxyDeinit(struct ICodec *self)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_DEINIT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxyEnumerateCapbility(struct ICodec *self, uint32_t index, CodecCapbility *cap)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || cap == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, index)) {
        HDF_LOGE("%{public}s: write input index failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_ENUM_CAP, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    if (CodecProxyParseGottenCapbility(reply, cap) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: CodecProxyParseGottenCapbility failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxyGetCapbility(struct ICodec *self, AvCodecMime mime, CodecType type,
                               uint32_t flags, CodecCapbility *cap)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || cap == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)mime)) {
        HDF_LOGE("%{public}s: write input mime failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)type)) {
        HDF_LOGE("%{public}s: write input type failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, flags)) {
        HDF_LOGE("%{public}s: write input flags failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_GET_CAP, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    if (CodecProxyParseGottenCapbility(reply, cap) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: CodecProxyParseGottenCapbility failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxyCreate(struct ICodec *self, const char* name, const Param *attr, int len, CODEC_HANDLETYPE *handle)
{
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || name == NULL || attr == NULL || handle == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data) || !HdfSbufWriteString(data, name)) {
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, attr->key)) {
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteInt32(data, attr->size)) {
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteBuffer(data, attr->val, attr->size)) {
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteInt32(data, len)) {
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)handle)) {
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = CodecProxyCall(self, CMD_CODEC_CREATE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    if (!HdfSbufReadUint32(reply, (uint32_t *)handle)) {
        ret = HDF_ERR_INVALID_PARAM;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxyDestroy(struct ICodec *self, CODEC_HANDLETYPE handle)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;

    if (self == NULL || handle == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: Write handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_DESTROY, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxySetPortMode(struct ICodec *self, CODEC_HANDLETYPE handle, DirectionType type, BufferMode mode)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)type)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)mode)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_SET_MODE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxySetParameter(struct ICodec *self, CODEC_HANDLETYPE handle, const Param *params, int paramCnt)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || params == NULL || paramCnt < 0) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write size failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteInt32(data, paramCnt)) {
        HDF_LOGE("%{public}s: write paramCnt failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < paramCnt; i++) {
        if (CodecProxyPackParam(data, &params[i]) != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: write params failed!", __func__);
            CodecProxySBufRecycle(data, reply);
            return HDF_FAILURE;
        }
    }
    ret = CodecProxyCall(self, CMD_CODEC_SET_PARAMS, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

// params需客户端调用点释放
int32_t CodecProxyGetParameter(struct ICodec *self, CODEC_HANDLETYPE handle, Param *params, int paramCnt)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || params == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data) ||
        !HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write interface token or size failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteInt32(data, paramCnt)) {
        HDF_LOGE("%{public}s: write paramCnt failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < paramCnt; i++) {
        if (CodecProxyPackParam(data, &params[i]) != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: CodecProxyPackParam!", __func__);
            CodecProxySBufRecycle(data, reply);
            return HDF_FAILURE;
        }
    }
    ret = CodecProxyCall(self, CMD_CODEC_GET_PARAMS, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    for (int32_t i = 0; i < paramCnt; i++) {
        if (CodecProxyParseParam(data, &params[i]) != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: read params failed!", __func__);
            CodecProxySBufRecycle(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecProxyStart(struct ICodec *self, CODEC_HANDLETYPE handle)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_START, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecProxyStop(struct ICodec *self, CODEC_HANDLETYPE handle)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_STOP, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecProxyFlush(struct ICodec *self, CODEC_HANDLETYPE handle, DirectionType directType)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)directType)) {
        CodecProxySBufRecycle(data, reply);
        HDF_LOGE("%{public}s: write input directType failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_FLUSH, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecPorxyQueueInput(struct ICodec *self, CODEC_HANDLETYPE handle,
                             const InputInfo *inputData, uint32_t timeoutMs)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL || inputData == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyPackInputInfo(data, inputData)) {
        HDF_LOGE("%{public}s: write input buffer failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, timeoutMs)) {
        HDF_LOGE("%{public}s: write input timeoutMs failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_QUEQUE_INPUT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecProxyDequeInput(struct ICodec *self, CODEC_HANDLETYPE handle, uint32_t timeoutMs, InputInfo *inputData)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL || inputData == NULL || inputData->bufferCnt == 0) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, timeoutMs)) {
        HDF_LOGE("%{public}s: write input timeoutMs failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, inputData->bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_DEQUEQUE_INPUT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    if (CodecProxyParseInputInfo(reply, inputData)) {
        HDF_LOGE("%{public}s: read struct reply failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecProxyQueueOutput(struct ICodec *self, CODEC_HANDLETYPE handle, OutputInfo *outInfo,
                              uint32_t timeoutMs, int releaseFenceFd)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL || outInfo == NULL) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyPackOutputInfo(data, outInfo)) {
        HDF_LOGE("%{public}s: write output buffer failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, timeoutMs)) {
        HDF_LOGE("%{public}s: write input timeoutMs failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteFileDescriptor(data, releaseFenceFd)) {
        HDF_LOGE("%{public}s: write input releaseFenceFd failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_QUEQUE_OUTPUT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}
int32_t CodecProxyDequeueOutput(struct ICodec *self, CODEC_HANDLETYPE handle, uint32_t timeoutMs,
                                int *acquireFd, OutputInfo *outInfo)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || handle == NULL || acquireFd == NULL || outInfo == NULL || outInfo->bufferCnt == 0) {
        HDF_LOGE("%{public}s: params null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data) ||
        !HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write interface token or input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, timeoutMs)) {
        HDF_LOGE("%{public}s: write input timeoutMs failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, outInfo->bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_DEQUEQUE_OUTPUT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    *acquireFd = HdfSbufReadFileDescriptor(reply);
    if (*acquireFd < 0) {
        HDF_LOGE("%{public}s: read acquireFd failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyParseOutputInfo(reply, outInfo)) {
        HDF_LOGE("%{public}s: read reply failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

int32_t CodecProxySetCallback(struct ICodec *self, CODEC_HANDLETYPE handle, struct ICodecCallback *cb, UINTPTR instance)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (self == NULL || cb == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_FAILURE;
    }
    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("write interface token failed");
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)(uintptr_t)handle)) {
        HDF_LOGE("%{public}s: write input handle failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (HdfSbufWriteRemoteService(data, cb->remote) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write cb failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, instance)) {
        HDF_LOGE("%{public}s: write input instance failed!", __func__);
        CodecProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecProxyCall(self, CMD_CODEC_SET_CBK, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecProxySBufRecycle(data, reply);
        return ret;
    }
    CodecProxySBufRecycle(data, reply);
    return ret;
}

static void CodecIpmlConstruct(struct ICodec *instance)
{
    instance->CodecInit = CodecPorxyInit;
    instance->CodecDeinit = CodecProxyDeinit;
    instance->CodecEnumerateCapbility = CodecProxyEnumerateCapbility;
    instance->CodecGetCapbility = CodecProxyGetCapbility;
    instance->CodecCreate = CodecProxyCreate;
    instance->CodecDestroy = CodecProxyDestroy;
    instance->CodecSetPortMode = CodecProxySetPortMode;
    instance->CodecSetParameter = CodecProxySetParameter;
    instance->CodecGetParameter = CodecProxyGetParameter;
    instance->CodecStart = CodecProxyStart;
    instance->CodecStop = CodecProxyStop;
    instance->CodecFlush = CodecProxyFlush;
    instance->CodecQueueInput = CodecPorxyQueueInput;
    instance->CodecDequeInput = CodecProxyDequeInput;
    instance->CodecQueueOutput = CodecProxyQueueOutput;
    instance->CodecDequeueOutput = CodecProxyDequeueOutput;
    instance->CodecSetCallback = CodecProxySetCallback;
    return;
}

struct ICodec *HdiCodecGet(const char *serviceName)
{
    struct HDIServiceManager *serviceMgr = HDIServiceManagerGet();
    if (serviceMgr == NULL) {
        HDF_LOGE("%{public}s: HDIServiceManager not found!", __func__);
        return NULL;
    }

    struct HdfRemoteService *remote = serviceMgr->GetService(serviceMgr, serviceName);
    if (remote == NULL) {
        HDF_LOGE("%{public}s: HdfRemoteService not found!", __func__);
        return NULL;
    }

    if (!HdfRemoteServiceSetInterfaceDesc(remote, "ohos.hdi.codec_service")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }

    struct ICodec *codecClient = (struct ICodec *)OsalMemAlloc(sizeof(struct ICodec));
    if (codecClient == NULL) {
        HDF_LOGE("%{public}s: malloc codec instance failed!", __func__);
        HdfRemoteServiceRecycle(remote);
        return NULL;
    }

    codecClient->remote = remote;
    CodecIpmlConstruct(codecClient);
    return codecClient;
}

void HdiCodecRelease(struct ICodec *instance)
{
    if (instance == NULL) {
        return;
    }
    HdfRemoteServiceRecycle(instance->remote);
    OsalMemFree(instance);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */