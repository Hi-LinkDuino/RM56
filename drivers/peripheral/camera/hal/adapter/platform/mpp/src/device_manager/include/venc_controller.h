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

#ifndef HOS_CAMERA_VENC_CONTROLLER_H
#define HOS_CAMERA_VENC_CONTROLLER_H

#include "icontroller.h"
#include "ivenc_object.h"
#include "device_manager_adapter.h"
#include "mpi_adapter.h"

namespace OHOS {
namespace Camera {
class VencController : public IController {
public:
    VencController();
    explicit VencController(std::string hardwareName);
    virtual ~VencController();
    RetCode Init();
    bool GetStartVencState()
    {
        return startVencState_;
    };
    RetCode StartGetStream()
    {
        return RC_OK;
    };
    RetCode StopGetStream()
    {
        return RC_OK;
    };
    RetCode SnapProcess()
    {
        return RC_OK;
    };
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    RetCode StopVenc();
    RetCode StartVenc();
    RetCode ConfigVenc();
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> camMeta)
    {
        (void)camMeta;
        return RC_OK;
    };
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag)
    {
        (void)abilityMetaDataTag;
    }
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        (void)meta;
        return RC_OK;
    };
    bool GetMetaDataFlag()
    {
        return false;
    };
    void SetMetaDataFlag(bool dataFlag)
    {
        (void)dataFlag;
    }
private:
    std::mutex startVenclock_;
    bool startVencState_ = false;
    std::shared_ptr<IVencObject> vencObject_ = nullptr;
};
} // namespace Camera
} // namespace OHOS
#endif
