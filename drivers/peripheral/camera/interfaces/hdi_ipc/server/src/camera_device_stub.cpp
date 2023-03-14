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

#include "camera_device_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "metadata_utils.h"
#include "istream_operator.h"
#include "istream_operator_callback.h"
#include "cmd_common.h"

namespace OHOS::Camera {
int32_t CameraDeviceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    HDF_LOGE("%s: CameraDeviceStub::OnRemoteRequest entry!", __func__);
    int32_t ret = HDF_SUCCESS;
    switch (code) {
        case CMD_CAMERA_DEVICE_GET_STREAM_OPERATOR: {
            ret = CameraDeviceStubGetStreamOperator(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_UPDATE_SETTINGS: {
            ret = CameraDeviceStubUpdateSettings(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_SET_RESULT_MODE: {
            ret = CameraDeviceStubSetResultMode(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_GET_ENABLED_RESULTS: {
            ret = CameraDeviceStubGetEnabledReuslts(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_ENABLE_RESULT: {
            ret = CameraDeviceStubEnableResult(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_DISABLE_RESULT: {
            ret = CameraDeviceStubDisableResult(data, reply, option);
            break;
        }
        case CMD_CAMERA_DEVICE_CLOSE: {
            ret = CameraDeviceStubClose(data, reply, option);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ret;
}

int32_t CameraDeviceStub::CameraDeviceStubGetStreamOperator(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    sptr<IStreamOperatorCallback> spStreamOperatorCallback = nullptr;
    bool flag = data.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
        spStreamOperatorCallback = OHOS::iface_cast<IStreamOperatorCallback>(remoteObj);
        if (spStreamOperatorCallback == nullptr) {
            HDF_LOGE("%s: read operator callback failed", __func__);
            return HDF_FAILURE;
        }
    }

    OHOS::sptr<IStreamOperator> streamOperator = nullptr;
    CamRetCode ret = GetStreamOperator(spStreamOperatorCallback, streamOperator);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: get stream operator failed", __func__);
        return HDF_FAILURE;
    }

    bool nullFlag = (streamOperator != nullptr);
    if (!reply.WriteBool(nullFlag)) {
        HDF_LOGE("%s: stream operator flag write failed!", __func__);
        return INVALID_ARGUMENT;
    }

    if (nullFlag && !reply.WriteRemoteObject(streamOperator->AsObject())) {
        HDF_LOGE("%s: write stream operator failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraDeviceStub::CameraDeviceStubUpdateSettings(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::shared_ptr<CameraStandard::CameraMetadata> metadata = nullptr;
    CameraStandard::MetadataUtils::DecodeCameraMetadata(data, metadata);

    CamRetCode ret = UpdateSettings(metadata);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: get stream operator failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraDeviceStub::CameraDeviceStubSetResultMode(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    ResultCallbackMode mode = static_cast<ResultCallbackMode>(data.ReadInt32());
    CamRetCode ret = SetResultMode(mode);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraDeviceStub::CameraDeviceStubGetEnabledReuslts(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> results;
    CamRetCode ret = GetEnabledResults(results);
    if (!reply.WriteInt32(static_cast<CamRetCode>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32Vector(results)) {
        HDF_LOGE("%s: write results failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraDeviceStub::CameraDeviceStubEnableResult(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> results;
    if (!data.ReadInt32Vector(&results)) {
        HDF_LOGE("%s: read results failed", __func__);
        return HDF_FAILURE;
    }

    CamRetCode ret = EnableResult(results);
    if (!reply.WriteInt32(static_cast<CamRetCode>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32Vector(results)) {
        HDF_LOGE("%s: write results failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraDeviceStub::CameraDeviceStubDisableResult(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    std::vector<int32_t> results;
    if (!data.ReadInt32Vector(&results)) {
        HDF_LOGE("%s: read results failed", __func__);
        return HDF_FAILURE;
    }

    CamRetCode ret = DisableResult(results);
    if (!reply.WriteInt32(static_cast<CamRetCode>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteInt32Vector(results)) {
        HDF_LOGE("%s: write results failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraDeviceStub::CameraDeviceStubClose(
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != CameraDeviceStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    Close();
    return HDF_SUCCESS;
}
}
