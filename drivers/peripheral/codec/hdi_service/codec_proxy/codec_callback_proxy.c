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
#include "codec_callback_proxy.h"
#include <hdf_log.h>
#include <osal_mem.h>
#include <servmgr_hdi.h>
#include "proxy_msgproc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ICodecCallback *g_callback = NULL;

static int32_t CodecCallbackProxyCall(int32_t id, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (g_callback->remote == NULL || g_callback->remote->dispatcher == NULL ||
        g_callback->remote->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%{public}s: obj is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return g_callback->remote->dispatcher->Dispatch(g_callback->remote, id, data, reply);
}

int32_t CodecCallbackProxyReqSBuf(struct HdfSBuf **data, struct HdfSBuf **reply)
{
    *data = HdfSbufTypedObtain(SBUF_IPC);
    if (*data == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (*reply == NULL) {
        HDF_LOGE("%{public}s: Failed to obtain reply", __func__);
        HdfSbufRecycle(*data);
        return HDF_ERR_MALLOC_FAIL;
    }
    return HDF_SUCCESS;
}

void CodecCallbackProxySBufRecycle(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
    return;
}

static int CodecCallbackProxyOnEvent(UINTPTR comp, UINTPTR appData, EventType event,
                                     uint32_t data1, uint32_t data2, UINTPTR eventData)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (CodecCallbackProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)comp)) {
        HDF_LOGE("%{public}s: write input comp failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)appData)) {
        HDF_LOGE("%{public}s: write input appData failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)event)) {
        HDF_LOGE("%{public}s: write input event failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)data1)) {
        HDF_LOGE("%{public}s: write input data1 failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)data2)) {
        HDF_LOGE("%{public}s: write input data2 failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)eventData)) {
        HDF_LOGE("%{public}s: write input eventData failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecCallbackProxyCall(CMD_CODEC_ON_EVENT, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecCallbackProxySBufRecycle(data, reply);
        return ret;
    }
    CodecCallbackProxySBufRecycle(data, reply);
    return ret;
}

static int CodecCallbackProxyInputBufferAvailable(UINTPTR comp, UINTPTR appData, InputInfo *inBuf)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (inBuf == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecCallbackProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)comp)) {
        HDF_LOGE("%{public}s: write input comp failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)appData)) {
        HDF_LOGE("%{public}s: write input appData failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyPackInputInfo(data, inBuf)) {
        HDF_LOGE("%{public}s: write input buffer failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecCallbackProxyCall(CMD_CODEC_INPUT_BUFFER_AVAILABLE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecCallbackProxySBufRecycle(data, reply);
        return ret;
    }
    CodecCallbackProxySBufRecycle(data, reply);
    return ret;
}

static int CodecCallbackProxyOutputBufferAvailable(UINTPTR comp, UINTPTR appData, OutputInfo *outBuf)
{
    int32_t ret;
    struct HdfSBuf *data = NULL;
    struct HdfSBuf *reply = NULL;
    if (outBuf == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecCallbackProxyReqSBuf(&data, &reply) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)comp)) {
        HDF_LOGE("%{public}s: write input comp failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufWriteUint32(data, (uint32_t)appData)) {
        HDF_LOGE("%{public}s: write input appData failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecProxyPackOutputInfo(data, outBuf)) {
        HDF_LOGE("%{public}s: write input buffer failed!", __func__);
        CodecCallbackProxySBufRecycle(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = CodecCallbackProxyCall(CMD_CODEC_OUTPUT_BUFFER_AVAILABLE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        CodecCallbackProxySBufRecycle(data, reply);
        return ret;
    }
    CodecCallbackProxySBufRecycle(data, reply);
    return ret;
}

static void CodecCallbackProxyConstruct(struct ICodecCallback *callback)
{
    callback->callback.OnEvent = CodecCallbackProxyOnEvent;
    callback->callback.InputBufferAvailable = CodecCallbackProxyInputBufferAvailable;
    callback->callback.OutputBufferAvailable = CodecCallbackProxyOutputBufferAvailable;
}

struct ICodecCallback *CodecProxyCallbackObtain(struct HdfRemoteService *remote)
{
    g_callback = (struct ICodecCallback *)OsalMemAlloc(sizeof(struct ICodecCallback));
    if (g_callback == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return NULL;
    }
    g_callback->remote = remote;
    CodecCallbackProxyConstruct(g_callback);
    return g_callback;
}

void CodecProxyCallbackRelease(struct ICodecCallback *callback)
{
    if (callback == NULL) {
        return;
    }
    OsalMemFree(callback);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */