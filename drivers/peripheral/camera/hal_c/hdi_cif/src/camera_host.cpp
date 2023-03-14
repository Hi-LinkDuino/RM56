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

#include "camera_host.h"
#include "camera.h"
#include "camera_device.h"
#include "camera_device_callback_wrapper.h"
#include "camera_host_callback_wrapper.h"
#include <dlfcn.h>

namespace OHOS::Camera {
std::shared_ptr<CameraHost> CameraHost::CreateCameraHost()
{
    std::shared_ptr<CameraHost> cameraHost = std::make_shared<CameraHost>();
    if (cameraHost == nullptr) {
        CAMERA_LOGE("new camera host implement failed.");
        return nullptr;
    }

    CamRetCode rc = cameraHost->Init();
    if (rc != NO_ERROR) {
        CAMERA_LOGE("camera host init failed.");
        return nullptr;
    }

    return cameraHost;
}

CameraHost::CameraHost() {}

CameraHost::~CameraHost()
{
    if (host_ != nullptr) {
        delete host_;
        host_ = nullptr;
    }

    if (handler_ != nullptr) {
        ::dlclose(handler_);
    }
}

CamRetCode CameraHost::SetCallback(const OHOS::sptr<ICameraHostCallback>& callback)
{
    CameraHostCallbackCIF cb;
    BindCameraHostCallback(callback);

    cb.OnCameraStatus = HostCBOnCameraStatus;
    cb.OnFlashlightStatus = HostCBOnFlashlightStatus;

    host_->SetCallback(cb);

    return NO_ERROR;
}

CamRetCode CameraHost::GetCameraIds(std::vector<std::string>& cameraIds)
{
    char** ids = nullptr;
    int size = 0;
    int ret = host_->GetCameraIds(&ids, &size);
    if (ret != NO_ERROR) {
        return static_cast<CamRetCode>(ret);
    }
    for (int i = 0; i < size; i++) {
        cameraIds.push_back(ids[i]);
    }

    return NO_ERROR;
}

CamRetCode CameraHost::GetCameraAbility(const std::string& cameraId, std::shared_ptr<CameraAbility>& ability)
{
    CameraAbilityCIF* meta = nullptr;
    int ret = host_->GetCameraAbility(cameraId.c_str(), meta);
    ability = std::make_shared<CameraAbility>(0, 0);
    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraHost::OpenCamera(const std::string& cameraId,
                                  const OHOS::sptr<ICameraDeviceCallback>& callback,
                                  OHOS::sptr<ICameraDevice>& pDevice)
{
    CameraDeviceCallbackCIF cb;
    BindCameraDeviceCallback(callback);

    cb.OnError = DeviceCBOnError;
    cb.OnResult = DeviceCBOnResult;

    CameraDeviceCIF* device = nullptr;
    const char* id = cameraId.c_str();
    int ret = host_->OpenCamera(id, cb, device);

    auto cd = new CameraDevice;
    if (cd == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }
    cd->Init(device);
    pDevice = cd;
    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraHost::SetFlashlight(const std::string& cameraId, bool& isEnable)
{
    if (host_ == nullptr) {
        return INSUFFICIENT_RESOURCES;
    }

    const char* id = cameraId.c_str();
    int ret = host_->SetFlashlight(id, reinterpret_cast<int*>(&isEnable));
    return static_cast<CamRetCode>(ret);
}

CamRetCode CameraHost::Init()
{
    host_ = new CameraHostCIF();
    if (host_ == nullptr) {
        CAMERA_LOGE("%{public}s, create camera host failed.", __FUNCTION__);
        return INSUFFICIENT_RESOURCES;
    }

    handler_ = ::dlopen(CAMERA_C_HAL_LIB_PATH, RTLD_NOW);
    if (handler_ == nullptr) {
        CAMERA_LOGE("%{public}s, dlopen %{public}s failed, %{public}s",
            __FUNCTION__, CAMERA_C_HAL_LIB_PATH, ::dlerror());
        return INSUFFICIENT_RESOURCES;
    }

    host_->SetCallback = reinterpret_cast<HOST_C_FUNC_SETCALLBACK>(::dlsym(handler_, "SetCallback"));
    if (host_->SetCallback == nullptr) {
        CAMERA_LOGE("%{public}s, can't get symbol of function SetCallback, %{public}s", __FUNCTION__, ::dlerror());
        return INSUFFICIENT_RESOURCES;
    }

    host_->GetCameraIds = reinterpret_cast<HOST_C_FUNC_GETCAMERAIDS>(::dlsym(handler_, "GetCameraIds"));
    if (host_->GetCameraIds == nullptr) {
        CAMERA_LOGE("%{public}s, can't get symbol of function GetCameraIds, %{public}s", __FUNCTION__, ::dlerror());
        return INSUFFICIENT_RESOURCES;
    }

    host_->GetCameraAbility = reinterpret_cast<HOST_C_FUNC_GETCAMERAABILITY>(::dlsym(handler_, "GetCameraAbility"));
    if (host_->GetCameraAbility == nullptr) {
        CAMERA_LOGE("%{public}s, can't get symbol of function GetCameraAbility, %{public}s", __FUNCTION__, ::dlerror());
        return INSUFFICIENT_RESOURCES;
    }

    host_->OpenCamera = reinterpret_cast<HOST_C_FUNC_OPENCAMERA>(::dlsym(handler_, "OpenCamera"));
    if (host_->OpenCamera == nullptr) {
        CAMERA_LOGE("%{public}s, can't get symbol of function OpenCamera, %{public}s", __FUNCTION__, ::dlerror());
        return INSUFFICIENT_RESOURCES;
    }

    host_->SetFlashlight = reinterpret_cast<HOST_C_FUNC_SETFLASHLIGHT>(::dlsym(handler_, "SetFlashlight"));
    if (host_->SetFlashlight == nullptr) {
        CAMERA_LOGE("%{public}s, can't get symbol of function SetFlashlight, %{public}s", __FUNCTION__, ::dlerror());
        return INSUFFICIENT_RESOURCES;
    }

    return NO_ERROR;
}
} // namespace OHOS::Camera
