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

#ifndef HOS_CAMERA_ICONTROLLER_H
#define HOS_CAMERA_ICONTROLLER_H

#include "device_manager_adapter.h"
#include <mutex>
#include <string>

namespace OHOS {
namespace Camera {
class IController {
public:
    IController();
    explicit IController(std::string hardwareName);
    virtual ~IController();
    virtual RetCode Init() = 0;
    std::string GetName()
    {
        return hardwareName_;
    };
    bool GetPowerOnState();
    void SetPowerOnState(bool powerOnState);
    virtual void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag) = 0;
    virtual RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta) = 0;
    virtual bool GetMetaDataFlag()
    {
        return false;
    };
    virtual void SetMetaDataFlag(bool metaDataFlag);
    virtual RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta) = 0;

private:
    std::mutex powerOnStatelock_;
    std::string hardwareName_;
    bool powerOnState_ = false;
};
} // namespace Camera
} // namespace OHOS
#endif