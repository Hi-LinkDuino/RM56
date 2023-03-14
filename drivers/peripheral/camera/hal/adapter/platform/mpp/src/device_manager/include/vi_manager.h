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

#ifndef HOS_CAMERA_VI_MANAGER_H
#define HOS_CAMERA_VI_MANAGER_H

#include "imanager.h"
#include "device_manager_adapter.h"
#include "vi_controller.h"

namespace OHOS {
namespace Camera {
class ViManager : public IManager {
public:
    ViManager();
    explicit ViManager(ManagerId managerId);
    virtual ~ViManager();
    RetCode CreateController(ControllerId controllerId, std::string hardwareName);
    std::shared_ptr<IController> GetController(ControllerId controllerId);
    std::shared_ptr<ISensor> GetSensor(std::string sensorName);
    void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        vi_->Configure(meta);
    };
    RetCode ConfigVi()
    {
        return vi_->ConfigVi();
    };
    RetCode StartVi()
    {
        return vi_->StartVi();
    };
    RetCode StopVi()
    {
        return vi_->StopVi();
    };
    RetCode GetFrameRateBySensor(int32_t CameraId, int32_t* pu32FrameRate)
    {
        (void)CameraId;
        (void)pu32FrameRate;
        return RC_OK;
    };
    CameraStandard::CameraMetadata GetSensorInfo(CameraId cameraIdt);
    RetCode StartCapture(int32_t snapCnt)
    {
        (void)snapCnt;
        return RC_OK;
    };
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        vi_->SetAbilityMetaDataTag(abilityMetaDataTag);
    };
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        return vi_->GetAbilityMetaData(meta);
    };
    bool GetMetaDataFlag()
    {
        return vi_->GetMetaDataFlag();
    };
    void SetMetaDataFlag(bool metaDataFlag)
    {
        vi_->SetMetaDataFlag(metaDataFlag);
    };
    RetCode SetFlashlight(FlashMode flashMode, bool enable)
    {
        return vi_->SetFlashlight(flashMode, enable);
    };

private:
    std::shared_ptr<ViController> vi_ = nullptr;
};
} // namespace Camera
} // namespace OHOS
#endif
