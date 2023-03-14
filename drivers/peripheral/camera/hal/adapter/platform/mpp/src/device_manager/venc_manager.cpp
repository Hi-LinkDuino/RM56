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

#include "venc_manager.h"

namespace OHOS {
namespace Camera {
VencManager::VencManager() {}

VencManager::VencManager(ManagerId managerId) : IManager(managerId) {}

VencManager::~VencManager() {}

RetCode VencManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (venc_ == nullptr) {
        venc_ = std::make_shared<VencController>(hardwareName);
        if (venc_ == nullptr) {
            return RC_ERROR;
        }
    }
    return venc_->Init();
}

std::shared_ptr<IController> VencManager::GetController(ControllerId controllerId)
{
    if (controllerId == DM_C_VENC) {
        return venc_;
    } else {
        return nullptr;
    }
}

RetCode VencManager::PowerUp(CameraId cameraId)
{
    return venc_->PowerUp(cameraId);
}
RetCode VencManager::PowerDown(CameraId cameraId)
{
    return venc_->PowerDown(cameraId);
}
} // namespace Camera
} // namespace OHOS