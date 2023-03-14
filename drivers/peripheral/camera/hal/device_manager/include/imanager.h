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

#ifndef HOS_CAMERA_IMANAGER_H
#define HOS_CAMERA_IMANAGER_H

#include "icontroller.h"
#include "device_manager_adapter.h"

namespace OHOS {
namespace Camera {
class IManager {
public:
    IManager();
    explicit IManager(ManagerId managerId);
    virtual ~IManager();
    virtual void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta) = 0;
    ManagerId GetManagerId()
    {
        return managerID_;
    };
    virtual std::shared_ptr<IController> GetController(ControllerId controllerId, std::string hardwareName)
    {
        (void)controllerId;
        (void)hardwareName;
        return nullptr;
    };
    virtual std::shared_ptr<IController> GetController(ControllerId controllerId)
    {
        (void)controllerId;
        return nullptr;
    };
    virtual RetCode CreateController(CameraId cameraId, ControllerId controllerId, std::string hardwareName)
    {
        (void)cameraId;
        (void)controllerId;
        (void)hardwareName;
        return RC_OK;
    };
    virtual RetCode CreateController(ControllerId controllerId, std::string hardwareName)
    {
        (void)controllerId;
        (void)hardwareName;
        return RC_OK;
    };
    virtual RetCode PowerUp(CameraId cameraId)
    {
        (void)cameraId;
        return RC_OK;
    };
    virtual RetCode PowerDown(CameraId cameraId)
    {
        (void)cameraId;
        return RC_OK;
    };
    virtual RetCode PowerUp(std::string hardwareName)
    {
        (void)hardwareName;
        return RC_OK;
    };
    virtual RetCode PowerDown(std::string hardwareName)
    {
        (void)hardwareName;
        return RC_OK;
    };
    virtual void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        (void)abilityMetaDataTag;
    }
    virtual void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag, std::string hardwareName)
    {
        (void)abilityMetaDataTag;
        (void)hardwareName;
    }
    virtual RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        (void)meta;
        return RC_OK;
    };
    virtual RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta, std::string hardwareName)
    {
        (void)meta;
        (void)hardwareName;
        return RC_OK;
    };
    virtual bool GetMetaDataFlag()
    {
        return false;
    };
    virtual void SetMetaDataFlag(bool metaDataFlag)
    {
        (void)metaDataFlag;
        return;
    }

private:
    ManagerId managerID_;
};
} // namespace Camera
} // namespace OHOS
#endif