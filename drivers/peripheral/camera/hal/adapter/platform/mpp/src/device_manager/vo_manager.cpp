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

#include "vo_manager.h"

namespace OHOS {
namespace Camera {
VoManager::VoManager() {}

VoManager::VoManager(ManagerId managerId) : IManager(managerId) {}

VoManager::~VoManager() {}

RetCode VoManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (vo_ == nullptr) {
        vo_ = std::make_shared<VoController>(hardwareName);
        if (vo_ == nullptr) {
            return RC_ERROR;
        }
    }
    return vo_->Init();
}

std::shared_ptr<IController> VoManager::GetController(ControllerId controllerId)
{
    if (controllerId == DM_C_VO) {
        return vo_;
    } else {
        return nullptr;
    }
}

RetCode VoManager::PowerUp(CameraId cameraId)
{
    return vo_->PowerUp(cameraId);
}

RetCode VoManager::PowerDown(CameraId cameraId)
{
    return vo_->PowerDown(cameraId);
}
} // namespace Camera
} // namespace OHOS