/*
 * Copyright (c) 2022 Shenzhen Kaihong DID Co., Ltd.
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
#include <osal_mem.h>
#include <servmgr_hdi.h>
#include "codec_callback_if.h"
#include "codec_internal.h"
#include "codec_types.h"

#define HDF_LOG_TAG codec_hdi_cb_client

struct CodecCallbackTypeProxy {
    struct CodecCallbackType instance;
    struct HdfRemoteService *remote;
};

static void ReleaseSbuf(struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (data != NULL) {
        HdfSbufRecycle(data);
    }
    if (reply != NULL) {
        HdfSbufRecycle(reply);
    }
}

static int32_t CodecCallbackTypeProxyCall(struct CodecCallbackType *self, int32_t id, struct HdfSBuf *data,
    struct HdfSBuf *reply)
{
    if (self->remote == NULL ||
        self->remote->dispatcher == NULL ||
        self->remote->dispatcher->Dispatch == NULL) {
        HDF_LOGE("%{public}s: obj is null", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }
    return self->remote->dispatcher->Dispatch(self->remote, id, data, reply);
}

static int32_t WriteArray(struct HdfSBuf *data, int8_t *array, uint32_t arrayLen)
{
    if (!HdfSbufWriteUint32(data, arrayLen)) {
        HDF_LOGE("%{public}s: write appData failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < arrayLen; i++) {
        if (!HdfSbufWriteInt8(data, array[i])) {
            HDF_LOGE("%{public}s: write array[i] failed!", __func__);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}

static int32_t WriteEventData(struct HdfSBuf *data, enum OMX_EVENTTYPE eEvent, uint32_t data1, uint32_t data2)
{
    if (!HdfSbufWriteUint32(data, (uint32_t)eEvent)) {
        HDF_LOGE("%{public}s: write eEvent failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, data1)) {
        HDF_LOGE("%{public}s: write data1 failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUint32(data, data2)) {
        HDF_LOGE("%{public}s: write data2 failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    return HDF_SUCCESS;
}

static int32_t CodecCallbackTypeProxyEventHandler(struct CodecCallbackType *self,
    int8_t *appData, uint32_t appDataLen, enum OMX_EVENTTYPE eEvent, uint32_t data1,
    uint32_t data2, int8_t *eventData, uint32_t eventDataLen)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    ret = WriteArray(data, appData, appDataLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write appData failed!", __func__);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ret = WriteEventData(data, eEvent, data1, data2);
    if (ret != HDF_SUCCESS) {
        ReleaseSbuf(data, reply);
        return ret;
    }

    ret = WriteArray(data, eventData, eventDataLen);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write eventData failed!", __func__);
        ReleaseSbuf(data, reply);
        return ret;
    }

    for (uint32_t i = 0; i < eventDataLen; i++) {
        if (!HdfSbufWriteInt8(data, eventData[i])) {
            HDF_LOGE("%{public}s: write eventData[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    ret = CodecCallbackTypeProxyCall(self, CMD_EVENT_HANDLER, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecCallbackTypeProxyEmptyBufferDone(struct CodecCallbackType *self,
    int8_t *appData, uint32_t appDataLen, const struct OmxCodecBuffer *buffer)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, appDataLen)) {
        HDF_LOGE("%{public}s: write appData failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < appDataLen; i++) {
        if (!HdfSbufWriteInt8(data, appData[i])) {
            HDF_LOGE("%{public}s: write appData[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecCallbackTypeProxyCall(self, CMD_EMPTY_BUFFER_DONE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static int32_t CodecCallbackTypeProxyFillBufferDone(struct CodecCallbackType *self,
    int8_t* appData, uint32_t appDataLen, struct OmxCodecBuffer* buffer)
{
    int32_t ret;

    struct HdfSBuf *data = HdfSbufTypedObtain(SBUF_IPC);
    struct HdfSBuf *reply = HdfSbufTypedObtain(SBUF_IPC);
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%{public}s: HdfSubf malloc failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_MALLOC_FAIL;
    }

    if (!HdfRemoteServiceWriteInterfaceToken(self->remote, data)) {
        HDF_LOGE("%{public}s: write interface token failed", __func__);
        ReleaseSbuf(data, reply);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteUint32(data, appDataLen)) {
        HDF_LOGE("%{public}s: write appData failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < appDataLen; i++) {
        if (!HdfSbufWriteInt8(data, appData[i])) {
            HDF_LOGE("%{public}s: write appData[i] failed!", __func__);
            ReleaseSbuf(data, reply);
            return HDF_ERR_INVALID_PARAM;
        }
    }

    if (!OmxCodecBufferBlockMarshalling(data, buffer)) {
        HDF_LOGE("%{public}s: write buffer failed!", __func__);
        ReleaseSbuf(data, reply);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = CodecCallbackTypeProxyCall(self, CMD_FILL_BUFFER_DONE, data, reply);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call failed! error code is %{public}d", __func__, ret);
        ReleaseSbuf(data, reply);
        return ret;
    }

    ReleaseSbuf(data, reply);
    return ret;
}

static void CodecCallbackTypeProxyConstruct(struct CodecCallbackType *instance)
{
    instance->EventHandler = CodecCallbackTypeProxyEventHandler;
    instance->EmptyBufferDone = CodecCallbackTypeProxyEmptyBufferDone;
    instance->FillBufferDone = CodecCallbackTypeProxyFillBufferDone;
}

struct CodecCallbackType *CodecCallbackTypeGet(struct HdfRemoteService *remote)
{
    if (remote == NULL) {
        HDF_LOGE("%{public}s: remote is null", __func__);
        return NULL;
    }

    struct CodecCallbackType *instance = (struct CodecCallbackType*)OsalMemAlloc(sizeof(struct CodecCallbackType));
    if (instance == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return NULL;
    }

    if (!HdfRemoteServiceSetInterfaceDesc(remote, "ohos.hdi.codec_service")) {
        HDF_LOGE("%{public}s: failed to init interface desc", __func__);
        return NULL;
    }

    instance->remote = remote;
    CodecCallbackTypeProxyConstruct(instance);
    return instance;
}

void CodecCallbackTypeRelease(struct CodecCallbackType *instance)
{
    if (instance == NULL) {
        HDF_LOGE("%{public}s: instance is null", __func__);
        return;
    }
    OsalMemFree(instance);
}
