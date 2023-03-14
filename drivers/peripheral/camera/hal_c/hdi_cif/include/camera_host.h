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

#ifndef CAMERA_HOST_CAMERA_HOST_H
#define CAMERA_HOST_CAMERA_HOST_H

#include "camera_host_c_if.h"
#include "camera_device_c_if.h"
#include "icamera_host_callback.h"
#include "icamera_device_callback.h"
#include "icamera_device.h"

namespace OHOS::Camera {
class CameraHost {
public:
    static std::shared_ptr<CameraHost> CreateCameraHost();

    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback);
    virtual CamRetCode GetCameraIds(std::vector<std::string> &cameraIds);
    virtual CamRetCode GetCameraAbility(const std::string &cameraId,
        std::shared_ptr<CameraAbility> &ability);
    virtual CamRetCode OpenCamera(const std::string &cameraId,
        const OHOS::sptr<ICameraDeviceCallback> &callback,
        OHOS::sptr<ICameraDevice> &pDevice);
    virtual CamRetCode SetFlashlight(const std::string &cameraId,  bool &isEnable);


public:
    CameraHost();
    virtual ~CameraHost();
    CameraHost(const CameraHost& other) = delete;
    CameraHost(CameraHost&& other) = delete;
    CameraHost& operator=(const CameraHost& other) = delete;
    CameraHost& operator=(CameraHost&& other) = delete;

private:
    CamRetCode Init();

private:
    void* handler_ = nullptr;
    CameraHostCIF* host_ = nullptr;
};
} // end namespace OHOS::Camera
#endif // CAMERA_HOST_CAMERA_HOST_H
