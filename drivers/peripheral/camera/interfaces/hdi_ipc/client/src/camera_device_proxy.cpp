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

#include "camera_device_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include "metadata_utils.h"
#include "istream_operator.h"
#include "istream_operator_callback.h"

namespace OHOS::Camera {
CamRetCode CameraDeviceProxy::GetStreamOperator(
    const OHOS::sptr<IStreamOperatorCallback> &callback,
    OHOS::sptr<IStreamOperator> &streamOperator)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    bool nullFlag = (callback != nullptr);
    if (!data.WriteBool(nullFlag)) {
        HDF_LOGE("%s: stream operator callback flag write failed!", __func__);
        return INVALID_ARGUMENT;
    }

    if (nullFlag && !data.WriteRemoteObject(callback->AsObject())) {
        HDF_LOGE("%s: write stream operator failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_GET_STREAM_OPERATOR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    bool flag = reply.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteStreamOperator = reply.ReadRemoteObject();
        streamOperator = OHOS::iface_cast<IStreamOperator>(remoteStreamOperator);
    }

    return retCode;
}

CamRetCode CameraDeviceProxy::UpdateSettings(const std::shared_ptr<CameraSetting> &settings)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(settings, data);
    if (!bRet) {
        HDF_LOGE("%s: write metadata failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_UPDATE_SETTINGS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode CameraDeviceProxy::SetResultMode(const ResultCallbackMode &mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32(static_cast<int32_t>(mode))) {
        HDF_LOGE("%s: write result callback mode failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_SET_RESULT_MODE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode CameraDeviceProxy::GetEnabledResults(std::vector<MetaType> &results)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_GET_ENABLED_RESULTS, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    if (!reply.ReadInt32Vector(&results)) {
        HDF_LOGE("%s: read results failed", __func__);
        return INVALID_ARGUMENT;
    }

    return retCode;
}

CamRetCode CameraDeviceProxy::EnableResult(const std::vector<MetaType> &results)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32Vector(results)) {
        HDF_LOGE("%s: write results failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_ENABLE_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode CameraDeviceProxy::DisableResult(const std::vector<MetaType> &results)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteInt32Vector(results)) {
        HDF_LOGE("%s: write results failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_DISABLE_RESULT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

void CameraDeviceProxy::Close()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraDeviceProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_DEVICE_CLOSE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
    }
}
}