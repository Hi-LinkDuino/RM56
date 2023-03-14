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

#ifndef CAMERA_DEVICE_CLIENT_H
#define CAMERA_DEVICE_CLIENT_H

#include "camera_config.h"
#include "camera_impl.h"
#include "camera_service.h"

#include <cstdint>

namespace OHOS {
namespace Media {
class CameraDeviceClient {
public:
    static CameraDeviceClient *GetInstance();
    CameraDeviceClient();
    ~CameraDeviceClient();

    int32_t SetCameraConfig(CameraConfig &cc);
    int32_t TriggerLoopingCapture(FrameConfig &fc);
    void StopLoopingCapture(int32_t type);
    int32_t TriggerSingleCapture(FrameConfig &fc);
    void Release();

    void SetCameraId(std::string cameraId);
    void SetCameraImpl(CameraImpl *cameraImpl);
    void SetCameraCallback();
private:
    std::string cameraId_;
    CameraImpl *cameraImpl_ = nullptr;
    CameraConfig *cc_ = nullptr;
    CameraService *cameraService_ = nullptr;
};
} // namespace Media
} // namespace OHOS
#endif // CAMERA_DEVICE_CLIENT_H
