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

#ifndef HOS_CAMERA_VO_CONTROLLER_H
#define HOS_CAMERA_VO_CONTROLLER_H

#include "icontroller.h"
#include "ivo_object.h"
#include "device_manager_adapter.h"
#include "mpi_adapter.h"

namespace OHOS {
namespace Camera {
class VoController : public IController {
public:
    VoController();
    explicit VoController(std::string hardwareName);
    virtual ~VoController();
    RetCode Init();
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    RetCode StopVo();
    RetCode StartVo();
    RetCode ConfigVo();
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> metadata)
    {
        (void)metadata;
        return RC_OK;
    };
    bool GetStartVoState()
    {
        return startVoState_;
    };
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        (void)abilityMetaDataTag;
        return;
    }
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> cameraMeta)
    {
        (void)cameraMeta;
        return RC_OK;
    };
    bool GetMetaDataFlag()
    {
        return false;
    };
    void SetMetaDataFlag(bool metaDataFlag)
    {
        (void)metaDataFlag;
    }

private:
    std::mutex startVolock_;
    bool startVoState_ = false;
    std::shared_ptr<IVoObject> voObject_ = nullptr;
};
} // namespace Camera
} // namespace OHOS
#endif