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

#include "vo_controller.h"

namespace OHOS {
namespace Camera {
VoController::VoController() {}

VoController::VoController(std::string hardwareName) : IController(hardwareName) {}

VoController::~VoController() {}

RetCode VoController::Init()
{
    voObject_ = IVoObject::CreateVoObject();
    if (voObject_ == nullptr) {
        CAMERA_LOGE("%{public}s Create VoObject fail", __FUNCTION__);
        return RC_ERROR;
    }
    return RC_OK;
}

RetCode VoController::PowerUp(CameraId cameraId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    if (GetPowerOnState() == false) {
        SetPowerOnState(true);
        CAMERA_LOGI("%{public}s Vo Powerup", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode VoController::PowerDown(CameraId cameraId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    if (GetPowerOnState() == true) {
        SetPowerOnState(false);
        CAMERA_LOGI("%{public}s Vo PowerDown", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode VoController::ConfigVo()
{
    std::vector<DeviceFormat> format;
    voObject_->ConfigVo(format);
    CAMERA_LOGI("%{public}s Vo ConfigVenc", __FUNCTION__);
    return RC_OK;
};

RetCode VoController::StartVo()
{
    std::lock_guard<std::mutex> l(startVolock_);
    RetCode rc = RC_OK;
    if (startVoState_ == false) {
        voObject_->StartVo();
        startVoState_ = true;
    }
    CAMERA_LOGI("%{public}s Vo StartVo", __FUNCTION__);
    return rc;
};

RetCode VoController::StopVo()
{
    CAMERA_LOGI("%{public}s Vo StopVo", __FUNCTION__);
    std::lock_guard<std::mutex> l(startVolock_);
    RetCode rc = RC_OK;
    if (startVoState_ == true) {
        voObject_->StopVo();
        startVoState_ = false;
    }
    return rc;
};
} // namespace Camera
} // namespace OHOS
