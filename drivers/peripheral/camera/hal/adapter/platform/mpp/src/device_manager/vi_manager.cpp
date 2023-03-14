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

#include "vi_manager.h"

namespace OHOS {
namespace Camera {
ViManager::ViManager() {}

ViManager::ViManager(ManagerId managerId) : IManager(managerId) {}

ViManager::~ViManager() {}

RetCode ViManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (vi_ == nullptr) {
        vi_ = std::make_shared<ViController>(hardwareName);
        if (vi_ == nullptr) {
            return RC_ERROR;
        }
    }
    return vi_->Init();
}

std::shared_ptr<ISensor> ViManager::GetSensor(std::string sensorName)
{
    if (vi_ != nullptr) {
        return vi_->GetSensor(sensorName);
    }
    return nullptr;
}

std::shared_ptr<IController> ViManager::GetController(ControllerId controllerId)
{
    if (controllerId == DM_C_VI) {
        return vi_;
    } else {
        return nullptr;
    }
}

RetCode ViManager::PowerUp(CameraId cameraId)
{
    return vi_->PowerUp(cameraId);
}

RetCode ViManager::PowerDown(CameraId cameraId)
{
    return vi_->PowerDown(cameraId);
}

CameraStandard::CameraMetadata ViManager::GetSensorInfo(CameraId cameraId)
{
    (void)cameraId;
    CameraStandard::CameraMetadata meta(0, 0);
    return meta;
}
} // namespace Camera
} // namespace OHOS
