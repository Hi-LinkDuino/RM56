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

#include "camera_host_stub.h"
#include <hdf_log.h>
#include <hdf_base.h>
#include <hdf_sbuf_ipc.h>
#include "metadata_utils.h"
#include "icamera_device.h"
#include "icamera_host_callback.h"
#include "cmd_common.h"

namespace OHOS::Camera {
CameraHostStub::CameraHostStub()
{
}

RetCode CameraHostStub::Init()
{
    cameraHost_ = CameraHost::CreateCameraHost();
    if (cameraHost_ == nullptr) {
        HDF_LOGE("%s: camera host service start failed", __func__);
        return RC_ERROR;
    }
    return RC_OK;
}

int32_t CameraHostStub::CameraHostStubSetCallback(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != CameraHostStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    bool flag = data.ReadBool();
    sptr<ICameraHostCallback> hostCallback = nullptr;
    if (flag) {
        sptr<IRemoteObject> remoteObj = data.ReadRemoteObject();
        hostCallback = OHOS::iface_cast<ICameraHostCallback>(remoteObj);
    }
    CamRetCode ret = cameraHost_->SetCallback(hostCallback);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraHostStub::CameraHostStubGetCameraIds(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (cameraHost_ == nullptr) {
        return HDF_FAILURE;
    }

    if (data.ReadInterfaceToken() != CameraHostStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }
    std::vector<std::string> cameraIds;
    CamRetCode ret = cameraHost_->GetCameraIds(cameraIds);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    if (!reply.WriteStringVector(cameraIds)) {
        HDF_LOGE("%s: write cameraIds failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraHostStub::CameraHostStubGetCameraAbility(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != CameraHostStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    const std::string cameraId = data.ReadString();
    if (cameraId.empty()) {
        HDF_LOGE("%s: read input param is empty", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    std::shared_ptr<CameraAbility> ability = nullptr;
    CamRetCode ret = cameraHost_->GetCameraAbility(cameraId, ability);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    bool bRet = CameraStandard::MetadataUtils::EncodeCameraMetadata(ability, reply);
    if (!bRet) {
        HDF_LOGE("%s: write ability failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraHostStub::CameraHostStubOpenCamera(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != CameraHostStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    const std::string cameraId = data.ReadString();
    if (cameraId.empty()) {
        HDF_LOGE("%s: read input param is empty", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    bool flag = data.ReadBool();
    OHOS::sptr<ICameraDeviceCallback> deviceCallback = nullptr;
    if (flag) {
        OHOS::sptr<IRemoteObject> remoteCallback = data.ReadRemoteObject();
        deviceCallback = OHOS::iface_cast<ICameraDeviceCallback>(remoteCallback);
    }

    OHOS::sptr<ICameraDevice> cameraDevice = nullptr;
    CamRetCode ret = cameraHost_->OpenCamera(cameraId, deviceCallback, cameraDevice);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: get stream operator failed", __func__);
        return HDF_FAILURE;
    }

    bool deviceFlag = (cameraDevice != nullptr);
    if (!reply.WriteBool(deviceFlag)) {
        HDF_LOGE("%s: write camera device flag failed", __func__);
        return HDF_FAILURE;
    }

    if (deviceFlag && !reply.WriteRemoteObject(cameraDevice->AsObject())) {
        HDF_LOGE("%s: write camera device failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraHostStub::CameraHostStubSetFlashlight(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != CameraHostStub::GetDescriptor()) {
        HDF_LOGE("%{public}s: invalid interface descriptor.", __func__);
        return INVALID_ARGUMENT;
    }

    if (cameraHost_ == nullptr) {
        HDF_LOGE("%s: camera host is null", __func__);
        return HDF_FAILURE;
    }

    std::string cameraId = data.ReadString();
    bool isEnable = data.ReadBool();
    CamRetCode ret = cameraHost_->SetFlashlight(cameraId, isEnable);
    if (!reply.WriteInt32(static_cast<int32_t>(ret))) {
        HDF_LOGE("%s: write retcode failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t CameraHostStub::CameraHostServiceStubOnRemoteRequest(int cmdId, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    switch (cmdId) {
        case CMD_CAMERA_HOST_SET_CALLBACK: {
            return CameraHostStubSetCallback(data, reply, option);
        }
        case CMD_CAMERA_HOST_GET_CAMERAID: {
            return CameraHostStubGetCameraIds(data, reply, option);
        }
        case CMD_CAMERA_HOST_GET_CAMERA_ABILITY: {
            return CameraHostStubGetCameraAbility(data, reply, option);
        }
        case CMD_CAMERA_HOST_OPEN_CAMERA: {
            return CameraHostStubOpenCamera(data, reply, option);
        }
        case CMD_CAMERA_HOST_SET_FLASH_LIGHT: {
            return CameraHostStubSetFlashlight(data, reply, option);
        }
        default: {
            HDF_LOGE("%s: not support cmd %d", __func__, cmdId);
            return HDF_ERR_INVALID_PARAM;
        }
    }
    return HDF_SUCCESS;
}
}

void *CameraHostStubInstance()
{
    OHOS::Camera::CameraHostStub *stub =
        new (std::nothrow) OHOS::Camera::CameraHostStub();
    if (stub == nullptr) {
        HDF_LOGE("%s: camera host stub create failed.", __func__);
        return nullptr;
    }

    OHOS::Camera::RetCode ret = stub->Init();
    if (ret != OHOS::Camera::RC_OK) {
        delete stub;
        stub = nullptr;
        return nullptr;
    }

    return reinterpret_cast<void*>(stub);
}

void DestroyCameraHostStub(void *stubObj)
{
    delete reinterpret_cast<OHOS::Camera::CameraHostStub *>(stubObj);
}

int32_t CameraHostServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    if (stub == nullptr) {
        HDF_LOGE("%s:stub is null", __func__);
        return HDF_FAILURE;
    }

    OHOS::Camera::CameraHostStub *cameraHostStub =
        reinterpret_cast<OHOS::Camera::CameraHostStub *>(stub);
    OHOS::MessageParcel *dataParcel = nullptr;
    OHOS::MessageParcel *replyParcel = nullptr;

    if (SbufToParcel(reply, &replyParcel) != HDF_SUCCESS) {
        HDF_LOGE("%s:invalid reply sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (SbufToParcel(data, &dataParcel) != HDF_SUCCESS) {
        HDF_LOGE("%s:invalid data sbuf object to dispatch", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OHOS::MessageOption option;
    return cameraHostStub->CameraHostServiceStubOnRemoteRequest(cmdId, *dataParcel, *replyParcel, option);
}
