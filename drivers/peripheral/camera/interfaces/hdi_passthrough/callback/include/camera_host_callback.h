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

#ifndef HDI_CAMERA_HOST_CALLBACK_H
#define HDI_CAMERA_HOST_CALLBACK_H

#include "icamera_host_callback.h"

namespace OHOS::Camera {
class CameraHostCallback : public ICameraHostCallback {
public:
    CameraHostCallback() = default;
    virtual ~CameraHostCallback() = default;

public:
    virtual void OnCameraStatus(const std::string &cameraId, CameraStatus status) override;
    virtual void OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status) override;
    virtual void OnCameraEvent(const std::string &cameraId, CameraEvent event) override;
};
}
#endif // HDI_CAMERA_HOST_CALLBACK_H
