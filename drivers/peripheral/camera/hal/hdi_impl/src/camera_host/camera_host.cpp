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

#include "camera_host_impl.h"

namespace OHOS::Camera {
std::shared_ptr<CameraHost> CameraHost::CreateCameraHost()
{
    std::shared_ptr<CameraHostImpl> cameraHost = std::make_shared<CameraHostImpl>();
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
} // end namespace OHOS::Camera
