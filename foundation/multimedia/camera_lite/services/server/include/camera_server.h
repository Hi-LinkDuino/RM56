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
#ifndef CAMERA_SERVER_H
#define CAMERA_SERVER_H

#include "camera_service.h"
#include "liteipc_adapter.h"
#include "serializer.h"
#include "camera_ability.h"

namespace OHOS {
namespace Media {
class CameraServer {
public:
    static CameraServer *GetInstance();
    ~CameraServer() {}

    static void CameraServerRequestHandle(int funcId, void *origin, IpcIo *req, IpcIo *reply);
    void InitCameraServer();
    void GetCameraAbility(IpcIo *req, IpcIo *reply);
    void GetCameraInfo(IpcIo *req, IpcIo *reply);
    void GetCameraIdList(IpcIo *req, IpcIo *reply);
    void CreateCamera(IpcIo *req, IpcIo *reply);
    void CloseCamera(IpcIo *req, IpcIo *reply);
    void SetCameraConfig(IpcIo *req, IpcIo *reply);
    void SetFrameConfig(IpcIo *req, IpcIo *reply);
    void TriggerLoopingCapture(IpcIo *req, IpcIo *reply);
    void StopLoopingCapture(IpcIo *req, IpcIo *reply);
    void TriggerSingleCapture(IpcIo *req, IpcIo *reply);
    void SetCameraCallback(IpcIo *req, IpcIo *reply);
    void OnTriggerSingleCaptureFinished(int32_t ret);
    void OnTriggerLoopingCaptureFinished(int32_t ret, int32_t streamId);
    void OnCameraStatusChange(int32_t ret, SvcIdentity *sid);
    void OnCameraConfigured(int32_t ret);
    void GetCameraModeNum(IpcIo *req, IpcIo *reply);
    void SetCameraMode(IpcIo *req, IpcIo *reply);
private:
    CameraServer()
    {
        sid_.cookie = 0;
    }
    SvcIdentity sid_;
};
} // namespace Media
} // namespace OHOS
#endif // CAMERA_SERVER_H
