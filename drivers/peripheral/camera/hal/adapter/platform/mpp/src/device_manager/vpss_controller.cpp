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

#include "vpss_controller.h"

namespace OHOS {
namespace Camera {
VpssController::VpssController() {}

VpssController::VpssController(std::string hardwareName) : IController(hardwareName) {}

VpssController::~VpssController() {}

RetCode VpssController::Init()
{
    vpssObject_ = IVpssObject::CreateVpssObject();
    if (vpssObject_ == nullptr) {
        CAMERA_LOGE("%{public}s Create VpssObject fail", __FUNCTION__);
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode VpssController::PowerUp(CameraId cameraId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    if (GetPowerOnState() == false) {
        SetPowerOnState(true);
        CAMERA_LOGI("%{public}s Vpss Powerup", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode VpssController::PowerDown(CameraId cameraId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    if (GetPowerOnState() == true) {
        SetPowerOnState(false);
        CAMERA_LOGI("%{public}s Vpss PowerDown", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode VpssController::ConfigVpss()
{
    std::vector<DeviceFormat> format;
    vpssObject_->ConfigVpss(format);
    CAMERA_LOGI("%{public}s Vpss ConfigVpss", __FUNCTION__);
    return RC_OK;
};

RetCode VpssController::StartVpss()
{
    std::lock_guard<std::mutex> l(startVpsslock_);
    RetCode rc = RC_OK;
    if (startVpssState_ == false) {
        vpssObject_->StartVpss();
        startVpssState_ = true;
    }
    CAMERA_LOGI("%{public}s Vpss StartVpss", __FUNCTION__);
    return rc;
};

RetCode VpssController::StopVpss()
{
    CAMERA_LOGI("%{public}s Vpss Stop Vpss enter", __FUNCTION__);
    std::lock_guard<std::mutex> l(startVpsslock_);
    RetCode rc = RC_OK;
    if (startVpssState_ == true) {
        vpssObject_->StopVpss();
        startVpssState_ = false;
    }

    CAMERA_LOGI("%{public}s Vpss Stop Vpss exit", __FUNCTION__);
    return rc;
};
} // namespace Camera
} // namespace OHOS
