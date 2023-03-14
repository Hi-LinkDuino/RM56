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

#include "stub_msgproc.h"
#include <hdf_log.h>
#include <osal_mem.h>
#include <servmgr_hdi.h>
#include "codec_callback_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int32_t SerCodecOnEvent(struct ICodecCallback *serviceImpl, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    UINTPTR comp = 0;
    UINTPTR appData = 0;
    EventType event = 0;
    uint32_t data1 = 0;
    uint32_t data2 = 0;
    UINTPTR eventData = 0;
    if (!HdfSbufReadUint32(data, (uint32_t *)&comp)) {
        HDF_LOGE("%{public}s: read comp data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&appData)) {
        HDF_LOGE("%{public}s: read appData data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&event)) {
        HDF_LOGE("%{public}s: read event data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &data1)) {
        HDF_LOGE("%{public}s: read data1 data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &data2)) {
        HDF_LOGE("%{public}s: read data2 data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&eventData)) {
        HDF_LOGE("%{public}s: read event data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = serviceImpl->callback.OnEvent(comp, appData, event, data1, data2, eventData);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call OnEvent fuc failed!", __func__);
    }
    return ret;
}

int32_t SerCodecInputBufferAvailable(struct ICodecCallback *serviceImpl, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t bufCnt;
    UINTPTR comp = 0;
    UINTPTR appData = 0;
    InputInfo inBuf = {0};
    if (!HdfSbufReadUint32(data, (uint32_t *)&comp)) {
        HDF_LOGE("%{public}s: read comp data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&appData)) {
        HDF_LOGE("%{public}s: read appData data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&inBuf.bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt failed!", __func__);
        return HDF_FAILURE;
    }
    bufCnt = inBuf.bufferCnt;
    inBuf.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * bufCnt);
    if (inBuf.buffers == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc CodecCallbackStub obj failed!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    if (CodecSerParseInputInfo(data, &inBuf)) {
        HDF_LOGE("%{public}s: read struct reply failed!", __func__);
        OsalMemFree(inBuf.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = serviceImpl->callback.InputBufferAvailable(comp, appData, &inBuf);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call OnEvent fuc failed!", __func__);
    }
    OsalMemFree(inBuf.buffers);
    return ret;
}

int32_t SerCodecOutputBufferAvailable(struct ICodecCallback *serviceImpl, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    uint32_t bufCnt;
    UINTPTR comp = 0;
    UINTPTR appData = 0;
    OutputInfo outBuf = {0};
    if (!HdfSbufReadUint32(data, (uint32_t *)&comp)) {
        HDF_LOGE("%{public}s: read comp data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&appData)) {
        HDF_LOGE("%{public}s: read appData data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&outBuf.bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt failed!", __func__);
        return HDF_FAILURE;
    }
    bufCnt = outBuf.bufferCnt;
    outBuf.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * bufCnt);
    if (outBuf.buffers == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecSerParseOutputInfo(data, &outBuf)) {
        HDF_LOGE("%{public}s: read struct outBuf failed!", __func__);
        OsalMemFree(outBuf.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    ret = serviceImpl->callback.OutputBufferAvailable(comp, appData, &outBuf);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call OnEvent fuc failed!", __func__);
        OsalMemFree(outBuf.buffers);
        return ret;
    }
    OsalMemFree(outBuf.buffers);
    return ret;
}

int32_t CodecCallbackServiceOnRemoteRequest(struct HdfRemoteService *service, int cmdId,
                                            struct HdfSBuf *data, struct HdfSBuf *reply)
{
    struct ICodecCallback *serviceImpl = (struct ICodecCallback *)service;
    switch (cmdId) {
        case CMD_CODEC_ON_EVENT:
            return SerCodecOnEvent(serviceImpl, data, reply);
        case CMD_CODEC_INPUT_BUFFER_AVAILABLE:
            return SerCodecInputBufferAvailable(serviceImpl, data, reply);
        case CMD_CODEC_OUTPUT_BUFFER_AVAILABLE:
            return SerCodecOutputBufferAvailable(serviceImpl, data, reply);
        default: {
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
}

struct CodecCallbackStub {
    struct ICodecCallback service;
    struct HdfRemoteDispatcher dispatcher;
};

struct ICodecCallback *CodecCallbackStubObtain()
{
    struct CodecCallbackStub *stub = (struct CodecCallbackStub *)OsalMemAlloc(sizeof(struct CodecCallbackStub));
    if (stub == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc CodecCallbackStub obj failed!", __func__);
        return NULL;
    }
    stub->dispatcher.Dispatch = CodecCallbackServiceOnRemoteRequest;
    stub->service.remote = HdfRemoteServiceObtain((struct HdfObject *)stub, &(stub->dispatcher));
    if (stub->service.remote == NULL) {
        HDF_LOGE("%{public}s: stub->service.remote is null", __func__);
        return NULL;
    }
    CodecCallbackServiceConstruct(&stub->service);
    return &stub->service;
}

void CodecCallbackStubRelease(struct ICodecCallback *stub)
{
    if (stub == NULL) {
        return;
    }
    OsalMemFree(stub);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */