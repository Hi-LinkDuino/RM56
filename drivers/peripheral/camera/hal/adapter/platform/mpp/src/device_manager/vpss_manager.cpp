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

#include "vpss_manager.h"

namespace OHOS {
namespace Camera {
VpssManager::VpssManager() {}

VpssManager::VpssManager(ManagerId managerId) : IManager(managerId) {}

VpssManager::~VpssManager() {}

RetCode VpssManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (vpss_ == nullptr) {
        vpss_ = std::make_shared<VpssController>(hardwareName);
        if (vpss_ == nullptr) {
            return RC_ERROR;
        }
    }
    return vpss_->Init();
}

std::shared_ptr<IController> VpssManager::GetController(ControllerId controllerId)
{
    if (controllerId == DM_C_VPSS) {
        return vpss_;
    } else {
        return nullptr;
    }
}

RetCode VpssManager::PowerUp(CameraId cameraId)
{
    return vpss_->PowerUp(cameraId);
}

RetCode VpssManager::PowerDown(CameraId cameraId)
{
    return vpss_->PowerDown(cameraId);
}
} // namespace Camera
} // namespace OHOS