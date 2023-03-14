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

#include "venc_controller.h"

namespace OHOS {
namespace Camera {
VencController::VencController() {}

VencController::VencController(std::string hardwareName) : IController(hardwareName) {}

VencController::~VencController() {}

RetCode VencController::Init()
{
    vencObject_ = IVencObject::CreateVencObject();
    if (vencObject_ == nullptr) {
        CAMERA_LOGE("%{public}s Create VencObject fail", __FUNCTION__);
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode VencController::PowerUp(CameraId cameraId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    if (GetPowerOnState() == false) {
        SetPowerOnState(true);
        CAMERA_LOGI("%{public}s Venc Powerup", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode VencController::PowerDown(CameraId cameraId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    if (GetPowerOnState() == true) {
        SetPowerOnState(false);
        CAMERA_LOGI("%{public}s Venc PowerDown", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode VencController::ConfigVenc()
{
    uint32_t width = 1920;
    uint32_t height = 1080;
    vencObject_->ConfigVenc(width, height);
    CAMERA_LOGI("%{public}s Venc ConfigVenc", __FUNCTION__);
    return RC_OK;
};

RetCode VencController::StartVenc()
{
    std::lock_guard<std::mutex> l(startVenclock_);
    RetCode rc = RC_OK;
    if (startVencState_ == false) {
        vencObject_->StartVenc();
        startVencState_ = true;
    }
    CAMERA_LOGI("%{public}s Venc StartVenc", __FUNCTION__);
    return rc;
};

RetCode VencController::StopVenc()
{
    CAMERA_LOGI("%{public}s Venc StopVenc", __FUNCTION__);
    std::lock_guard<std::mutex> l(startVenclock_);
    RetCode rc = RC_OK;
    if (startVencState_ == false) {
        vencObject_->StopVenc();
        startVencState_ = true;
    }
    return rc;
};
} // namespace Camera
} // namespace OHOS
