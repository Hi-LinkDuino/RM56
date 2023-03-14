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

#ifndef HOS_CAMERA_FLASH_CONTROLLER_H
#define HOS_CAMERA_FLASH_CONTROLLER_H

#include "icontroller.h"
#include "device_manager_adapter.h"

namespace OHOS::Camera {
class FlashController : public IController {
public:
    FlashController();
    explicit FlashController(std::string hardwareName);
    virtual ~FlashController();
    RetCode Init();
    RetCode PowerUp();
    RetCode PowerDown();
    RetCode Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
    {
        (void)meta;
        return RC_OK;
    };
    RetCode SetFlashlight(FlashMode flashMode, bool enable);
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag);
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta);
private:
    std::mutex startVolock_;
    bool startVoState_ = false;
};
} // namespace OHOS::Camera
#endif