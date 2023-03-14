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

#ifndef HOS_CAMERA_VI_CONTROLLER_H
#define HOS_CAMERA_VI_CONTROLLER_H

#include <map>
#include "icontroller.h"
#include "isensor.h"
#include "ivi_object.h"
#include "device_manager_adapter.h"
#include "mpi_adapter.h"

namespace OHOS {
namespace Camera {
class ViController : public IController {
public:
    ViController();
    explicit ViController(std::string hardwareName);
    virtual ~ViController();
    RetCode Init();
    std::shared_ptr<ISensor> GetSensor(std::string sensorName);
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    bool GetStartViState()
    {
        return startViState_;
    };
    RetCode GetFrameRateBySensor()
    {
        return RC_OK;
    };
    RetCode GetSizeBySensor()
    {
        return RC_OK;
    };
    RetCode StartCapture()
    {
        return RC_OK;
    };
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    RetCode StopVi();
    RetCode StartVi();
    RetCode ConfigVi();
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag);
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    bool GetMetaDataFlag();
    void SetMetaDataFlag(bool metaDataFlag);
    RetCode SetFlashlight(FlashMode flashMode, bool enable);

private:
    RetCode SendIspMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode SendAEMetaData(common_metadata_header_t &data);
    RetCode SendAWBMetaData(common_metadata_header_t &data);
    RetCode GetIspMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode GetAEMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode GetAWBMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    RetCode GetColorCorrectionGains(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    template<typename T>
    bool CheckNumequal(T oldnum, T num, int size)
    {
        if (oldnum == nullptr || num == nullptr) {
            CAMERA_LOGE("oldnum or num is nullptr");
            return false;
        }
        for (int i = 0; size > 0; i++, size--) {
            if (oldnum[i] != num[i]) {
                return false;
            }
        }
        return true;
    };
    std::mutex startVilock_;
    std::mutex metaDataFlaglock_;
    std::mutex metaDataSetlock_;
    bool metaDataFlag_ = false;
    bool startViState_ = false;
    std::shared_ptr<IViObject> viObject_ = nullptr;
    std::vector<int32_t> abilityMetaData_;
};
} // namespace Camera
} // namespace OHOS
#endif
