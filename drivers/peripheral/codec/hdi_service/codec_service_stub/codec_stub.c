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
#include "codec_stub.h"
#include <hdf_device_object.h>
#include <hdf_log.h>
#include <osal_mem.h>
#include "codec_callback_proxy.h"
#include "codec_interface.h"
#include "icodec.h"
#include "stub_msgproc.h"

#define HDF_CODEC_NAME_LEN 50

static int32_t SerCodecInit(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum = CodecInit();
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecInit fuc failed!", __func__);
        return errNum;
    }
    if (!HdfSbufWriteUint32(reply, errNum)) {
        HDF_LOGE("%{public}s: write errNum failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return errNum;
}

static int32_t SerCodecDeinit(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum = CodecDeinit();
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecDeinit fuc failed!", __func__);
        return errNum;
    }
    if (!HdfSbufWriteUint32(reply, errNum)) {
        HDF_LOGE("%{public}s: write errNum failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return errNum;
}
static int32_t SerCodecEnumerateCapbility(struct HdfDeviceIoClient *client, struct HdfSBuf *data,
                                          struct HdfSBuf *reply)
{
    uint32_t index;
    CodecCapbility capbility;

    if (!HdfSbufReadUint32(data, (uint32_t *)&index)) {
        HDF_LOGE("%{public}s: read index data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = HDF_SUCCESS;
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecEnumerateCapbility fuc failed!", __func__);
        return errNum;
    }
    if (CodecSerPackCapbility(reply, &capbility) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write capbility failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return errNum;
}

static int32_t SerCodecGetCapbility(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t flags;
    int32_t errNum;
    AvCodecMime mime;
    CodecType type;
    CodecCapbility cap;

    if (!HdfSbufReadUint32(data, (uint32_t*)&mime)) {
        HDF_LOGE("%{public}s: read input mime failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t*)&type)) {
        HDF_LOGE("%{public}s: read input type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &flags)) {
        HDF_LOGE("%{public}s: read input flags failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    errNum = CodecGetCapbility(mime, type, flags, &cap);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecGetCapbility fuc failed!", __func__);
        return errNum;
    }
    if (CodecSerPackCapbility(reply, &cap) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: write cap failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return errNum;
}

static int32_t SerCodecCreate(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t len = 0;
    int32_t errNum;
    uint32_t handle = 0;
    uint32_t dateSize = 0;
    const char *name = NULL;
    Param attr;

    name = HdfSbufReadString(data);
    if (name == NULL) {
        HDF_LOGE("%{public}s: Read name failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&attr.key)) {
        HDF_LOGE("%{public}s: Read name failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &attr.size)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadBuffer(data, (const void **)&attr.val, &dateSize)) {
        HDF_LOGE("%{public}s: struct attr's value Read failed", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &len)) {
        HDF_LOGE("%{public}s: Read name failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: Read handle failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    errNum = CodecCreate(name, (const Param *)&attr, len, (CODEC_HANDLETYPE *)&handle);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecCreate fuc failed!", __func__);
        return errNum;
    }
    if (!HdfSbufWriteUint32(reply, handle)) {
        HDF_LOGE("%{public}s: write handle failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return errNum;
}
static int32_t SerCodecDestroy(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum;
    uint32_t handle = 0;
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    errNum = CodecDestroy((CODEC_HANDLETYPE)&handle);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecDestroy fuc failed!", __func__);
        return errNum;
    }
    return errNum;
}

static int32_t SerCodecSetPortMode(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum;
    uint32_t handle = 0;
    DirectionType type;
    BufferMode mode;

    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t*)&type)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t*)&mode)) {
        HDF_LOGE("%{public}s: Read size failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    errNum = CodecSetPortMode((CODEC_HANDLETYPE)&handle, type, mode);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecSetPortMode fuc failed!", __func__);
        return errNum;
    }
    return errNum;
}

static int32_t SerCodecSetParameter(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum;
    int32_t paramCnt = 0;
    uint32_t handle = 0;
    Param *params = NULL;

    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: Read handle failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &paramCnt)) {
        HDF_LOGE("%{public}s: Read paramCnt failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (paramCnt <= 0) {
        HDF_LOGE("%{public}s: Param paramCnt err!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    params = (Param *)OsalMemAlloc(sizeof(Param)*paramCnt);
    if (params == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < paramCnt; i++) {
        if (CodecSerParseParam(data, &params[i]) != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: Read params failed!", __func__);
            OsalMemFree(params);
            return HDF_FAILURE;
        }
    }
    errNum = CodecSetParameter((CODEC_HANDLETYPE)&handle, params, paramCnt);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecSetParameter fuc failed!", __func__);
    }
    OsalMemFree(params);
    return errNum;
}
static int32_t SerCodecGetParameter(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum;
    int32_t paramCnt = 0;
    uint32_t handle = 0;
    Param *params = NULL;

    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: Read handle failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &paramCnt)) {
        HDF_LOGE("%{public}s: Read paramCnt failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (paramCnt <= 0) {
        HDF_LOGE("%{public}s: Param paramCnt err!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    params = (Param *)OsalMemAlloc(sizeof(Param)*paramCnt);
    if (params == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int32_t i = 0; i < paramCnt; i++) {
        if (CodecSerParseParam(data, &params[i]) != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: Read params failed!", __func__);
            OsalMemFree(params);
            return HDF_FAILURE;
        }
    }
    errNum = CodecGetParameter((CODEC_HANDLETYPE)&handle, params, paramCnt);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecGetParameter fuc failed!", __func__);
        OsalMemFree(params);
        return errNum;
    }
    for (int32_t i = 0; i < paramCnt; i++) {
        if (CodecSerPackParam(reply, &params[i]) != HDF_SUCCESS) {
            HDF_LOGE("%{public}s: CodecSerPackParam err!", __func__);
            OsalMemFree(params);
            return HDF_FAILURE;
        }
    }
    OsalMemFree(params);
    return errNum;
}
static int32_t SerCodecStart(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t errNum;
    uint32_t handle = 0;

    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: Read handle failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    errNum = CodecStart((CODEC_HANDLETYPE)&handle);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call SerCodecStart fuc failed!", __func__);
        return errNum;
    }
    return errNum;
}

static int32_t SerCodecStop(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t handle = 0;
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecStop((CODEC_HANDLETYPE)&handle);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecStop fuc failed!", __func__);
        return errNum;
    }
    return errNum;
}
static int32_t SerCodecFlush(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t handle = 0;
    uint32_t directType = 0;
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &directType)) {
        HDF_LOGE("%{public}s: read directType data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecFlush((CODEC_HANDLETYPE)&handle, (DirectionType)directType);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecFlush fuc failed!", __func__);
        return errNum;
    }
    return errNum;
}
int32_t SerCodecQueueInput(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t timeoutMs = 0;
    uint32_t handle = 0;
    uint32_t bufCnt;
    InputInfo inputData = {0};
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&inputData.bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    bufCnt = inputData.bufferCnt;
    if (bufCnt <= 0) {
        HDF_LOGE("%{public}s: Param bufCnt err!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    inputData.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * bufCnt);
    if (inputData.buffers == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecSerParseInputInfo(data, &inputData)) {
        HDF_LOGE("%{public}s: read inputData failed!", __func__);
        OsalMemFree(inputData.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &timeoutMs)) {
        HDF_LOGE("%{public}s: read timeoutMs data failed!", __func__);
        OsalMemFree(inputData.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecQueueInput((CODEC_HANDLETYPE)&handle, &inputData, timeoutMs);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecQueueInput fuc failed!", __func__);
        OsalMemFree(inputData.buffers);
        return errNum;
    }
    OsalMemFree(inputData.buffers);
    return errNum;
}
static int32_t SerCodecDequeInput(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t timeoutMs = 0;
    uint32_t handle = 0;
    InputInfo inputData = {0};
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &timeoutMs)) {
        HDF_LOGE("%{public}s: read timeoutMs data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &inputData.bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (inputData.bufferCnt <= 0) {
        HDF_LOGE("%{public}s: Param bufferCnt err!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    inputData.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * (inputData.bufferCnt));
    if (inputData.buffers == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecDequeInput((CODEC_HANDLETYPE)&handle, timeoutMs, &inputData);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecDequeInput fuc failed!", __func__);
        OsalMemFree(inputData.buffers);
        return errNum;
    }
    if (CodecSerPackInputInfo(reply, &inputData)) {
        HDF_LOGE("%{public}s: struct inputData write failed!", __func__);
        OsalMemFree(inputData.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMemFree(inputData.buffers);
    return errNum;
}
static int32_t SerCodecQueueOutput(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t timeoutMs = 0;
    int releaseFenceFd;
    uint32_t handle = 0;
    uint32_t bufCnt;
    OutputInfo outInfo = {0};
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, (uint32_t *)&outInfo.bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    bufCnt = outInfo.bufferCnt;
    if (bufCnt <= 0) {
        HDF_LOGE("%{public}s: Param bufferCnt err!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    outInfo.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * bufCnt);
    if (outInfo.buffers == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecSerParseOutputInfo(data, &outInfo)) {
        HDF_LOGE("%{public}s: read struct data failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &timeoutMs)) {
        HDF_LOGE("%{public}s: read timeoutMs data failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    releaseFenceFd = HdfSbufReadFileDescriptor(data);
    if (releaseFenceFd < 0) {
        HDF_LOGE("%{public}s: read timeoutMs data failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecQueueOutput((CODEC_HANDLETYPE)&handle, &outInfo, timeoutMs, releaseFenceFd);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecQueueOutput fuc failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return errNum;
    }
    OsalMemFree(outInfo.buffers);
    return errNum;
}
static int32_t SerCodecDequeueOutput(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t timeoutMs = 0;
    int acquireFd = 0;
    uint32_t handle = 0;
    OutputInfo outInfo = {0};
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &timeoutMs)) {
        HDF_LOGE("%{public}s: read timeoutMs data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadUint32(data, &outInfo.bufferCnt)) {
        HDF_LOGE("%{public}s: read bufferCnt data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    outInfo.buffers = (CodecBufferInfo *)OsalMemAlloc(sizeof(CodecBufferInfo) * (outInfo.bufferCnt));
    if (outInfo.buffers == NULL) {
        HDF_LOGE("%{public}s: OsalMemAlloc failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecDequeueOutput((CODEC_HANDLETYPE)&handle, timeoutMs, &acquireFd, &outInfo);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecDequeueOutput fuc failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return errNum;
    }
    if (!HdfSbufWriteFileDescriptor(reply, acquireFd)) {
        HDF_LOGE("%{public}s: write acquireFd failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    if (CodecSerPackOutputInfo(reply, &outInfo)) {
        HDF_LOGE("%{public}s: write outInfo buffer failed!", __func__);
        OsalMemFree(outInfo.buffers);
        return HDF_ERR_INVALID_PARAM;
    }
    OsalMemFree(outInfo.buffers);
    return errNum;
}

static int32_t SerCodecSetCallback(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    uint32_t handle = 0;
    UINTPTR instance;
    struct ICodecCallback *cb = NULL;
    if (!HdfSbufReadUint32(data, &handle)) {
        HDF_LOGE("%{public}s: read handle data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct HdfRemoteService *cbRemote = HdfSbufReadRemoteService(data);
    if (cbRemote == NULL) {
        HDF_LOGE("%{public}s: read cbRemote failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    cb = CodecProxyCallbackObtain(cbRemote);
    if (!HdfSbufReadUint32(data, (uint32_t *)&instance)) {
        HDF_LOGE("%{public}s: read instance data failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t errNum = CodecSetCallback((CODEC_HANDLETYPE)&handle, &cb->callback, instance);
    if (errNum != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: call CodecSetCallback fuc failed!", __func__);
        return errNum;
    }
    return errNum;
}

int32_t CodecServiceOnRemoteRequest(struct HdfDeviceIoClient *client, int cmdId,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        HDF_LOGE("check interface token failed");
        return HDF_ERR_INVALID_PARAM;
    }
    switch (cmdId) {
        case CMD_CODEC_INIT:
            return SerCodecInit(client, data, reply);
        case CMD_CODEC_DEINIT:
            return SerCodecDeinit(client, data, reply);
        case CMD_CODEC_ENUM_CAP:
            return SerCodecEnumerateCapbility(client, data, reply);
        case CMD_CODEC_GET_CAP:
            return SerCodecGetCapbility(client, data, reply);
        case CMD_CODEC_CREATE:
            return SerCodecCreate(client, data, reply);
        case CMD_CODEC_DESTROY:
            return SerCodecDestroy(client, data, reply);
        case CMD_CODEC_SET_MODE:
            return SerCodecSetPortMode(client, data, reply);
        case CMD_CODEC_SET_PARAMS:
            return SerCodecSetParameter(client, data, reply);
        case CMD_CODEC_GET_PARAMS:
            return SerCodecGetParameter(client, data, reply);
        case CMD_CODEC_START:
            return SerCodecStart(client, data, reply);
        case CMD_CODEC_STOP:
            return SerCodecStop(client, data, reply);
        case CMD_CODEC_FLUSH:
            return SerCodecFlush(client, data, reply);
        case CMD_CODEC_QUEQUE_INPUT:
            return SerCodecQueueInput(client, data, reply);
        case CMD_CODEC_DEQUEQUE_INPUT:
            return SerCodecDequeInput(client, data, reply);
        case CMD_CODEC_QUEQUE_OUTPUT:
            return SerCodecQueueOutput(client, data, reply);
        case CMD_CODEC_DEQUEQUE_OUTPUT:
            return SerCodecDequeueOutput(client, data, reply);
        case CMD_CODEC_SET_CBK:
            return SerCodecSetCallback(client, data, reply);
        default: {
            HDF_LOGE("%{public}s: not support cmd %{public}d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
}
