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

#include "flash_manager.h"

namespace OHOS::Camera {
FlashManager::FlashManager() {}

FlashManager::FlashManager(ManagerId managerId) : IManager(managerId) {}

FlashManager::~FlashManager() {}

RetCode FlashManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    RetCode rc = RC_OK;
    if (controllerId == DM_C_FLASH) {
        if (!CheckCameraIdList(hardwareName)) {
            std::shared_ptr<FlashController> flash = std::make_shared<FlashController>(hardwareName);
            if (flash == nullptr) {
                return RC_ERROR;
            }
            rc = flash->Init();
            if (rc == RC_OK) {
            flashList_.push_back(flash);
            }
        }
    }
    return rc;
}

std::shared_ptr<IController> FlashManager::GetController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (flashList_.size() == 0) {
        return nullptr;
    }
    for (auto iter = flashList_.cbegin(); iter != flashList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return *iter;
        }
    }
    return nullptr;
}

void FlashManager::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    if (flashList_.size() == 0) {
        return;
    }
    for (auto iter = flashList_.cbegin(); iter != flashList_.cend(); iter++) {
        (*iter)->Configure(meta);
    }
};

RetCode FlashManager::SetFlashlight(FlashMode flashMode, bool enable, std::string hardwareName)
{
    for (auto iter = flashList_.cbegin(); iter != flashList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->SetFlashlight(flashMode, enable);
        }
    }
    return RC_OK;
}

bool FlashManager::CheckCameraIdList(std::string hardwareName)
{
    if (flashList_.size() == 0) {
        return false;
    }
    for (auto iter = flashList_.cbegin(); iter != flashList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return true;
        }
    }
    return false;
}

RetCode FlashManager::PowerUp(std::string hardwareName)
{
    for (auto iter = flashList_.cbegin(); iter != flashList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->PowerUp();
        }
    }
    return RC_ERROR;
}

RetCode FlashManager::PowerDown(std::string hardwareName)
{
    for (auto iter = flashList_.cbegin(); iter != flashList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->PowerDown();
        }
    }
    return RC_ERROR;
}
} // namespace OHOS::Camera