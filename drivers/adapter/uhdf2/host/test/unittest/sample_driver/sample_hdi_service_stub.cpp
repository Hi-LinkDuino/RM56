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

#include <hdf_device_desc.h>
#include <hdf_device_object.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include <osal_mem.h>

#include "hdf_sbuf_ipc.h"
#include "sample_hdi.h"

using OHOS::HDI::Base::SharedMemQueueMeta;

static int32_t SampleServiceStubPing(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    char *outInfo = nullptr;
    const char *info = HdfSbufReadString(data);

    int32_t ret = SampleHdiImplInstance()->ping(client->device, info, &outInfo);

    HdfSbufWriteString(reply, outInfo);
    OsalMemFree(outInfo);
    return ret;
}

static int32_t SampleServiceStubSum(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t parm0;
    int32_t parm1;
    int32_t result;
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &parm0)) {
        HDF_LOGE("SampleHdi.sum: miss parm0");
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufReadInt32(data, &parm1)) {
        HDF_LOGE("SampleHdi.sum: miss parm1");
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ret = SampleHdiImplInstance()->sum(client->device, parm0, parm1, &result);
    if (ret == HDF_SUCCESS) {
        if (!HdfSbufWriteInt32(reply, result)) {
            HDF_LOGE("SampleHdi.sum: failed to write result ");
            return HDF_FAILURE;
        }
    }

    return ret;
}

static int32_t SampleServiceStubCallback(struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t code;
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if (!HdfSbufReadInt32(data, &code)) {
        HDF_LOGE("SampleHdi.callback: miss parameter code");
        return HDF_ERR_INVALID_PARAM;
    }

    struct HdfRemoteService *callback = HdfSbufReadRemoteService(data);
    if (callback == nullptr) {
        HDF_LOGE("SampleHdi.callback: miss parameter callback");
        return HDF_ERR_INVALID_PARAM;
    }
    return SampleHdiImplInstance()->callback(client->device, callback, code);
}

static int32_t SampleServiceStubStructTrans(
    struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("SampleServiceStubStructTrans: in");
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    struct DataBlock *dataBlock = DataBlockBlockUnmarshalling(data);
    if (dataBlock == nullptr) {
        HDF_LOGE("SampleServiceStubStructTrans: failed to read dataBlock");
        return HDF_ERR_INVALID_PARAM;
    }

    int32_t ret = HDF_SUCCESS;
    if (!DataBlockBlockMarshalling(dataBlock, reply)) {
        HDF_LOGE("SampleServiceStubStructTrans: failed to write dataBlock");
        ret = HDF_ERR_INVALID_PARAM;
    } else {
        HDF_LOGI("SampleServiceStubStructTrans: good return");
    }
    DataBlockFree(dataBlock);
    return ret;
}

static int32_t SampleServiceStubBufferTrans(
    struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("SampleServiceStubBufferTrans: in");
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    constexpr int SAMPLE_TEST_BUFFER_SIZE = 10;
    const uint8_t *buffer = HdfSbufReadUnpadBuffer(data, SAMPLE_TEST_BUFFER_SIZE);
    if (buffer == nullptr) {
        HDF_LOGI("SampleServiceStubBufferTrans: read buffer failed");
        return HDF_ERR_INVALID_PARAM;
    }

    if (!HdfSbufWriteUnpadBuffer(reply, buffer, SAMPLE_TEST_BUFFER_SIZE)) {
        HDF_LOGE("SampleServiceStubBufferTrans: failed to write buffer");
        return HDF_ERR_INVALID_PARAM;
    } else {
        HDF_LOGI("SampleServiceStubBufferTrans: good return");
    }

    return HDF_SUCCESS;
}

static int32_t SampleServiceRegisterDevice(
    struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    (void)reply;
    const char *deviceName = HdfSbufReadString(data);
    if (deviceName == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }

    return SampleHdiImplInstance()->registerDevice(client->device, deviceName);
}

static int32_t SampleServiceUnregisterDevice(
    struct HdfDeviceIoClient *client, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)reply;
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *deviceName = HdfSbufReadString(data);
    if (deviceName == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }

    return SampleHdiImplInstance()->unregisterDevice(client->device, deviceName);
}

static int32_t SampleServiceUpdateDevice(struct HdfDeviceIoClient *client, struct HdfSBuf *data)
{
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    const char *servInfo = HdfSbufReadString(data);
    if (servInfo == nullptr) {
        return HDF_ERR_INVALID_PARAM;
    }

    return SampleHdiImplInstance()->updateService(client->device, servInfo);
}

static int32_t SampleServiceSmqTrans(struct HdfDeviceIoClient *client, struct HdfSBuf *data)
{
    if (!HdfDeviceObjectCheckInterfaceDesc(client->device, data)) {
        return HDF_ERR_INVALID_PARAM;
    }
    OHOS::MessageParcel *parcel = nullptr;
    if (SbufToParcel(data, &parcel) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    std::shared_ptr<SharedMemQueueMeta<SampleSmqElement>> smqMeta =
        SharedMemQueueMeta<SampleSmqElement>::UnMarshalling(*parcel);
    if (smqMeta == nullptr) {
        HDF_LOGE("failed to read smq meta form parcel");
        return HDF_ERR_INVALID_PARAM;
    }

    uint32_t element = parcel->ReadUint32();

    return SampleHdiImplInstance()->tansSmq(client->device, smqMeta.get(), element);
}

int32_t SampleServiceOnRemoteRequest(
    struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    HDF_LOGI("SampleServiceDispatch: cmd %{public}d", cmdId);
    switch (cmdId) {
        case SAMPLE_SERVICE_PING:
            return SampleServiceStubPing(client, data, reply);
        case SAMPLE_SERVICE_SUM:
            return SampleServiceStubSum(client, data, reply);
        case SAMPLE_SERVICE_CALLBACK:
            return SampleServiceStubCallback(client, data, reply);
        case SAMPLE_STRUCT_TRANS:
            return SampleServiceStubStructTrans(client, data, reply);
        case SAMPLE_BUFFER_TRANS:
            return SampleServiceStubBufferTrans(client, data, reply);
        case SAMPLE_REGISTER_DEVICE:
            return SampleServiceRegisterDevice(client, data, reply);
        case SAMPLE_UNREGISTER_DEVICE:
            return SampleServiceUnregisterDevice(client, data, reply);
        case SAMPLE_UPDATE_SERVIE:
            return SampleServiceUpdateDevice(client, data);
        case SAMPLE_TRANS_SMQ:
            return SampleServiceSmqTrans(client, data);
        default:
            HDF_LOGE("SampleServiceDispatch: not support cmd %{public}d", cmdId);
            return HDF_ERR_INVALID_PARAM;
    }
}