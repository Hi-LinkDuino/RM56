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

#include "camera_host_proxy.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include <message_parcel.h>
#include "utils_data_stub.h"
#include "metadata_utils.h"
#include "icamera_device.h"
#include "icamera_host_callback.h"
#include "icamera_device_callback.h"

namespace OHOS::Camera {
sptr<ICameraHost> ICameraHost::Get(const char *serviceName)
{
    do {
        using namespace OHOS::HDI::ServiceManager::V1_0;
        auto servMgr = IServiceManager::Get();
        if (servMgr == nullptr) {
            HDF_LOGE("%s: IServiceManager failed!", __func__);
            break;
        }

        auto remote = servMgr->GetService(serviceName);
        if (remote != nullptr) {
            sptr<CameraHostProxy> hostSptr = iface_cast<CameraHostProxy>(remote);
            return hostSptr;
        }
        HDF_LOGE("%s: GetService failed! serviceName = %s", __func__, serviceName);
    } while (false);

    HDF_LOGE("%s: get %s failed!", __func__, serviceName);
    return nullptr;
}

CamRetCode CameraHostProxy::SetCallback(const OHOS::sptr<ICameraHostCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    bool callbackFlag = (callback != nullptr);
    if (!data.WriteBool(callbackFlag)) {
        HDF_LOGE("%s: set callback flag failed!", __func__);
        return INVALID_ARGUMENT;
    }

    if (callbackFlag && !data.WriteRemoteObject(callback->AsObject())) {
        HDF_LOGE("%s: set callback write remote obj failed!", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_SET_CALLBACK, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}

CamRetCode CameraHostProxy::GetCameraIds(std::vector<std::string> &cameraIds)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_GET_CAMERAID, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());

    if (!reply.ReadStringVector(&cameraIds)) {
        HDF_LOGE("%{public}s: read cameraids failed.", __func__);
        return INVALID_ARGUMENT;
    }

    return retCode;
}

CamRetCode CameraHostProxy::GetCameraAbility(const std::string &cameraId,
    std::shared_ptr<CameraAbility> &ability)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteString(cameraId)) {
        HDF_LOGE("%{public}s: write cameraId failed!", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(
        CMD_CAMERA_HOST_GET_CAMERA_ABILITY, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    CameraStandard::MetadataUtils::DecodeCameraMetadata(reply, ability);

    return retCode;
}

CamRetCode CameraHostProxy::OpenCamera(const std::string &cameraId,
    const OHOS::sptr<ICameraDeviceCallback> &callback,
    OHOS::sptr<ICameraDevice> &pDevice)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteString(cameraId)) {
        HDF_LOGE("%{public}s: write cameraId failed!", __func__);
        return INVALID_ARGUMENT;
    }

    bool callbackFlag = (callback != nullptr);
    if (!data.WriteBool(callbackFlag)) {
        HDF_LOGE("%s: write camera callback flag failed", __func__);
        return INVALID_ARGUMENT;
    }

    if (callbackFlag && !data.WriteRemoteObject(callback->AsObject())) {
        HDF_LOGE("%s: write camera device callback failed", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_OPEN_CAMERA, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    CamRetCode retCode = static_cast<CamRetCode>(reply.ReadInt32());
    bool flag = reply.ReadBool();
    if (flag) {
        sptr<IRemoteObject> remoteCameraDevice = reply.ReadRemoteObject();
        if (remoteCameraDevice == nullptr) {
            HDF_LOGE("%{public}s: CameraHostProxy remoteCameraDevice is null", __func__);
            return INVALID_ARGUMENT;
        }
        pDevice = OHOS::iface_cast<ICameraDevice>(remoteCameraDevice);
    }

    return retCode;
}

CamRetCode CameraHostProxy::SetFlashlight(const std::string &cameraId, bool &isEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(CameraHostProxy::GetDescriptor())) {
        HDF_LOGE("%{public}s: write interface descriptor failed.", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteString(cameraId)) {
        HDF_LOGE("%{public}s: write cameraId failed!", __func__);
        return INVALID_ARGUMENT;
    }

    if (!data.WriteBool(isEnable)) {
        HDF_LOGE("%{public}s: write isEnable failed!", __func__);
        return INVALID_ARGUMENT;
    }

    int32_t ret = Remote()->SendRequest(CMD_CAMERA_HOST_SET_FLASH_LIGHT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: SendRequest failed, error code is %{public}d", __func__, ret);
        return INVALID_ARGUMENT;
    }

    return static_cast<CamRetCode>(reply.ReadInt32());
}
}