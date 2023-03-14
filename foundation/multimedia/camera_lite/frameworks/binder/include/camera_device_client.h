/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "camera_client.h"
#include "iproxy_client.h"
#include "media_log.h"
#include "liteipc_adapter.h"
#include "camera_type.h"

#include <cstdint>

namespace OHOS {
namespace Media {
class CameraImpl;
class CameraDeviceClient {
public:
    static CameraDeviceClient *GetInstance();
    static int Callback(void* owner, int code, IpcIo *reply);

    ~CameraDeviceClient();

    int32_t SetCameraConfig(CameraConfig &cc);
    int32_t TriggerLoopingCapture(FrameConfig &fc);
    void StopLoopingCapture(int32_t type);
    int32_t TriggerSingleCapture(FrameConfig &fc);
    void Release();

    void SetCameraId(std::string &cameraId);
    void SetCameraImpl(CameraImpl *cameraImpl);
    void SetCameraCallback();
    static int32_t SurfaceRequestHandler(const IpcContext* context, void *ipcMsg, IpcIo *io, void *arg);
    static int32_t DeviceClientCallback(const IpcContext* context, void *ipcMsg, IpcIo *io, void *arg);
private:
    CameraDeviceClient();
    CameraClient *cameraClient_ = nullptr;
    IClientProxy *proxy_ = nullptr;
    std::string cameraId_;
    CameraImpl *cameraImpl_ = nullptr;
    CallBackPara *para_ = nullptr;
    SvcIdentity sid_;
    int32_t ret_ = 0;
};
} // namespace Media
} // namespace OHOS
#endif // CAMERA_DEVICE_CLIENT_H