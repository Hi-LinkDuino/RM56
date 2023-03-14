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

#ifndef HOS_CAMERA_VENC_MANAGER_H
#define HOS_CAMERA_VENC_MANAGER_H

#include "imanager.h"
#include "device_manager_adapter.h"
#include "venc_controller.h"

namespace OHOS {
namespace Camera {
class VencManager : public IManager {
public:
    VencManager();
    explicit VencManager(ManagerId managerId);
    virtual ~VencManager();
    RetCode CreateController(ControllerId controllerId, std::string hardwareName);
    std::shared_ptr<IController> GetController(ControllerId controllerId);

    void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        venc_->Configure(meta);
    };
    RetCode ConfigVenc()
    {
        return venc_->ConfigVenc();
    };
    RetCode StartVenc()
    {
        return venc_->StartVenc();
    };
    RetCode StopVenc()
    {
        return venc_->StopVenc();
    };
    RetCode StartGetStream(int32_t s32Cnt)
    {
        (void)s32Cnt;
        return RC_OK;
    };
    RetCode StopGetStream()
    {
        return RC_OK;
    };
    RetCode SnapProcess(int32_t SnapCnt, bool bSaveJpg, bool bSaveThm)
    {
        (void)SnapCnt;
        (void)bSaveJpg;
        (void)bSaveThm;
        return RC_OK;
    };
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        venc_->SetAbilityMetaDataTag(abilityMetaDataTag);
    };
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        return venc_->GetAbilityMetaData(meta);
    };
    bool GetMetaDataFlag()
    {
        return venc_->GetMetaDataFlag();
    };
    void SetMetaDataFlag(bool metaDataFlag)
    {
        venc_->SetMetaDataFlag(metaDataFlag);
    };

private:
    std::shared_ptr<VencController> venc_ = nullptr;
};
} // namespace Camera
} // namespace OHOS
#endif