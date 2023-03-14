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

#include "isp_controller.h"
#include "camera_metadata_info.h"

namespace OHOS::Camera {
IspController::IspController() {}

IspController::IspController(std::string hardwareName) : IController(hardwareName) {}

IspController::~IspController() {}

RetCode IspController::Init()
{
    return RC_OK;
}

RetCode IspController::PowerUp()
{
    RetCode rc = RC_OK;
    if (GetPowerOnState() == false) {
        SetPowerOnState(true);
        CAMERA_LOGI("%s Isp Powerup", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode IspController::PowerDown()
{
    RetCode rc = RC_OK;
    if (GetPowerOnState() == true) {
        SetPowerOnState(false);
        CAMERA_LOGI("%s Isp PowerDown", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode IspController::Start()
{
    std::lock_guard<std::mutex> l(startIsplock_);
    RetCode rc = RC_OK;
    if (startIspState_ == false) {
        startIspState_ = true;
    }
    CAMERA_LOGI("%s Isp Start", __FUNCTION__);
    return rc;
}

RetCode IspController::Stop()
{
    std::lock_guard<std::mutex> l(startIsplock_);
    RetCode rc = RC_OK;
    if (startIspState_ == true) {
        startIspState_ = false;
    }
    CAMERA_LOGI("%s Isp Stop", __FUNCTION__);
    return rc;
}

RetCode IspController::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    return RC_OK;
}
} // namespace OHOS::Camera
