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

#include "flash_controller.h"

namespace OHOS::Camera {
FlashController::FlashController() {}

FlashController::FlashController(std::string hardwareName) : IController(hardwareName) {}

FlashController::~FlashController() {}

RetCode FlashController::Init()
{
    return RC_OK;
}

RetCode FlashController::PowerUp()
{
    RetCode rc = RC_OK;
    if (GetPowerOnState() == false) {
        SetPowerOnState(true);
        CAMERA_LOGI("%s Flash Powerup", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode FlashController::PowerDown()
{
    RetCode rc = RC_OK;
    if (GetPowerOnState() == true) {
        SetPowerOnState(false);
        CAMERA_LOGI("%s Flash PowerDown", __FUNCTION__);
        return rc;
    }
    return rc;
}

RetCode FlashController::SetFlashlight(FlashMode flashMode, bool enable)
{
    return RC_OK;
}

void FlashController::SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag) {}

RetCode FlashController::GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    return RC_OK;
}
} // namespace OHOS::Camera