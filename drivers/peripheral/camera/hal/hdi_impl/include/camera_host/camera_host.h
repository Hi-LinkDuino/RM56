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

#include "icamera_host_callback.h"
#include "icamera_device_callback.h"
#include "camera.h"
#include "types.h"

namespace OHOS::Camera {
class ICameraDevice;
class ICameraHostCallback;
class ICameraDeviceCallback;
class CameraHost {
public:
    static std::shared_ptr<CameraHost> CreateCameraHost();

    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraHostCallback> &callback) = 0;
    virtual CamRetCode GetCameraIds(std::vector<std::string> &cameraIds) = 0;
    virtual CamRetCode GetCameraAbility(const std::string &cameraId,
        std::shared_ptr<CameraAbility> &ability) = 0;
    virtual CamRetCode OpenCamera(const std::string &cameraId,
        const OHOS::sptr<ICameraDeviceCallback> &callback,
        OHOS::sptr<ICameraDevice> &pDevice) = 0;
    virtual CamRetCode SetFlashlight(const std::string &cameraId,  bool &isEnable) = 0;

public:
    CameraHost() = default;
    virtual ~CameraHost() = default;
    CameraHost(const CameraHost &other) = delete;
    CameraHost(CameraHost &&other) = delete;
    CameraHost& operator=(const CameraHost &other) = delete;
    CameraHost& operator=(CameraHost &&other) = delete;
};
} // end namespace OHOS::Camera
#endif // CAMERA_HOST_CAMERA_HOST_H
