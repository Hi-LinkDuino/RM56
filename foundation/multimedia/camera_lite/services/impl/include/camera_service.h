/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#ifndef OHOS_CAMERA_SERVICE_H
#define OHOS_CAMERA_SERVICE_H

#include "camera_device.h"
#include "camera_service_callback.h"
#include "camera_ability.h"
#include "camera_info_impl.h"

namespace OHOS {
namespace Media {
class CameraService {
public:
    ~CameraService();
    static CameraService *GetInstance();

    void Initialize();
    CameraAbility *GetCameraAbility(std::string &cameraId);
    CameraInfo *GetCameraInfo(std::string &cameraId);
    CameraDevice *GetCameraDevice(std::string &cameraId);
    int32_t CreateCamera(std::string cameraId);
    int32_t CloseCamera(string cameraId);
    list<string> GetCameraIdList();
    uint8_t GetCameraModeNum();
    int32_t SetCameraMode(uint8_t modeIndex);
private:
    CameraService();
    int32_t InitCameraDevices();
    std::map<string, CameraDevice*> deviceMap_;
    std::map<string, CameraAbility*> deviceAbilityMap_;
    std::map<string, CameraInfo*> deviceInfoMap_;
    CameraServiceCallback *cameraServiceCb_ = nullptr;
};
} // namespace Media
} // namespace OHOS

#endif // OHOS_CAMERA_SERVICE_H