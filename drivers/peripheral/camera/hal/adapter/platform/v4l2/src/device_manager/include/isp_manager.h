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

#ifndef HOS_CAMERA_ISP_MANAGER_H
#define HOS_CAMERA_ISP_MANAGER_H

#include "imanager.h"
#include "device_manager_adapter.h"
#include "isp_controller.h"

namespace OHOS::Camera {
class IspManager : public IManager {
public:
    IspManager();
    explicit IspManager(ManagerId managerId);
    virtual ~IspManager();
    RetCode CreateController(ControllerId controllerId, std::string hardwareName);
    std::shared_ptr<IController> GetController(ControllerId controllerId, std::string hardwareName);
    void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode Start(std::string hardwareName);
    RetCode Stop(std::string hardwareName);
    RetCode PowerUp(std::string hardwareName);
    RetCode PowerDown(std::string hardwareName);
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag, std::string hardwareName)
    {
        (void)abilityMetaDataTag;
        (void)hardwareName;
        return;
    }
private:
    bool CheckCameraIdList(std::string hardwareName);
    std::vector<std::shared_ptr<IspController>> ispList_;
};
} // namespace OHOS::Camera
#endif