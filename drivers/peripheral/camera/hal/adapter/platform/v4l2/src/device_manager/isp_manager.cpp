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

#include "isp_manager.h"

namespace OHOS::Camera {
IspManager::IspManager() {}

IspManager::IspManager(ManagerId managerId) : IManager(managerId) {}

IspManager::~IspManager() {}

RetCode IspManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    RetCode rc = RC_OK;
    if (controllerId == DM_C_ISP) {
        if (!CheckCameraIdList(hardwareName)) {
            std::shared_ptr<IspController> isp = std::make_shared<IspController>(hardwareName);
            if (isp == nullptr) {
                return RC_ERROR;
            }
            rc = isp->Init();
            if (rc == RC_OK) {
                ispList_.push_back(isp);
            }
        }
    }
    return rc;
}

std::shared_ptr<IController> IspManager::GetController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (ispList_.size() == 0) {
        return nullptr;
    }
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return *iter;
        }
    }
    return nullptr;
}

RetCode IspManager::PowerUp(std::string hardwareName)
{
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->PowerUp();
        }
    }
    return RC_ERROR;
}

RetCode IspManager::PowerDown(std::string hardwareName)
{
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->PowerDown();
        }
    }
    return RC_ERROR;
}

void IspManager::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    if (ispList_.size() == 0) {
        return;
    }
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        (*iter)->Configure(meta);
    }
}

RetCode IspManager::Start(std::string hardwareName)
{
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->Start();
        }
    }
    return RC_ERROR;
}

RetCode IspManager::Stop(std::string hardwareName)
{
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->Stop();
        }
    }
    return RC_ERROR;
}

bool IspManager::CheckCameraIdList(std::string hardwareName)
{
    if (ispList_.size() == 0) {
        return false;
    }
    for (auto iter = ispList_.cbegin(); iter != ispList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return true;
        }
    }
    return false;
}
} // namespace OHOS::Camera