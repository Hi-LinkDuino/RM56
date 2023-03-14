/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef CAMERA_SERVICE_CLIENT_H
#define CAMERA_SERVICE_CLIENT_H

#include "camera_ability.h"
#include "camera_info_impl.h"
#include "camera_service.h"

namespace OHOS {
namespace Media {
class CameraServiceClient {
public:
    ~CameraServiceClient();
    static CameraServiceClient *GetInstance();
    std::list<std::string> GetCameraIdList();
    void InitCameraServiceClient(CameraServiceCallback *callback);
    CameraAbility *GetCameraAbility(std::string &cameraId);
    CameraInfo *GetCameraInfo(std::string &cameraId);
    void CreateCamera(std::string cameraId);
    uint8_t GetCameraModeNum();
    int32_t SetCameraMode(uint8_t modeIndex);

private:
    CameraServiceClient();
    CameraService *cameraService_ = nullptr;
    CameraServiceCallback *cameraServiceCb_ = nullptr;
};
} // namespace Media
} // namespace OHOS

#endif // CAMERA_SERVICE_CLIENT_H
