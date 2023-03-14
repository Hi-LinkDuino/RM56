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

#ifndef HOS_CAMERA_VPSS_CONTROLLER_H
#define HOS_CAMERA_VPSS_CONTROLLER_H

#include "icontroller.h"
#include "ivpss_object.h"
#include "device_manager_adapter.h"
#include "mpi_adapter.h"

namespace OHOS {
namespace Camera {
class VpssController : public IController {
public:
    VpssController();
    explicit VpssController(std::string hardwareName);
    virtual ~VpssController();
    RetCode Init();
    bool GetStartVpssState()
    {
        return startVpssState_;
    };
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    RetCode ConfigVpss();
    RetCode StartVpss();
    RetCode StopVpss();
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        (void)abilityMetaDataTag;
        return;
    }
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        (void)meta;
        return RC_OK;
    };
    bool GetMetaDataFlag()
    {
        return false;
    };
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> camMeta)
    {
        (void)camMeta;
        return RC_OK;
    };
    void SetMetaDataFlag(bool metaDataFlag)
    {
        (void)metaDataFlag;
        return;
    }

private:
    std::shared_ptr<IVpssObject> vpssObject_ = nullptr;
    std::mutex startVpsslock_;
    bool startVpssState_ = false;
};
} // namespace Camera
} // namespace OHOS
#endif