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

#include <hdf_log.h>
#include "camera_host_callback.h"

namespace OHOS::Camera {
void CameraHostCallback::OnCameraStatus(const std::string &cameraId, CameraStatus status)
{
    HDF_LOGV("%{public}s, enter.", __func__);
}

void CameraHostCallback::OnFlashlightStatus(const std::string &cameraId, FlashlightStatus status)
{
    HDF_LOGV("%{public}s, enter. cameraId = %s, status = %d",
        __func__, cameraId.c_str(), static_cast<int>(status));
}

void CameraHostCallback::OnCameraEvent(const std::string &cameraId, CameraEvent event)
{
    HDF_LOGV("%{public}s, enter. cameraId = %s, event = %d",
        __func__, cameraId.c_str(), static_cast<int>(event));
}
}
