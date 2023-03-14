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
#ifndef CAMERA_SERVICE_CLIENT_H
#define CAMERA_SERVICE_CLIENT_H

#include "liteipc_adapter.h"
#include "serializer.h"
#include "camera_ability.h"
#include "camera_info_impl.h"
#include "iproxy_client.h"
#include "camera_service_callback.h"
#include "camera_client.h"
#include "camera_type.h"

namespace OHOS {
namespace Media {
class CameraServiceClient {
public:
    ~CameraServiceClient();
    static CameraServiceClient *GetInstance();
    static int Callback(void* owner, int code, IpcIo *reply);
    void InitCameraServiceClient(CameraServiceCallback *callback);
    std::list<std::string> GetCameraIdList();
    CameraAbility *GetCameraAbility(std::string &cameraId);
    CameraInfo *GetCameraInfo(std::string &cameraId);
    void CreateCamera(std::string cameraId);
    static int32_t ServiceClientCallback(const IpcContext* context, void *ipcMsg, IpcIo *io, void *arg);
    uint8_t GetCameraModeNum();
    int32_t SetCameraMode(uint8_t modeIndex);

private:
    CameraServiceClient();
    IClientProxy* proxy_ = nullptr;
    CameraClient *cameraClient_ = nullptr;
    std::list<std::string> list_;
    std::string cameraIdForAbility;
    std::map<std::string, CameraAbility*> deviceAbilityMap_;
    std::string cameraIdForInfo;
    std::map<std::string, CameraInfo*> deviceInfoMap_;
    CameraServiceCallback *cameraServiceCb_ = nullptr;
    SvcIdentity sid_;
    CallBackPara *para_ = nullptr;
    uint8_t cameraModeNum = 0;
    int32_t ret_ = 0;
};
} // namespace Media
} // namespace OHOS

#endif // CAMERA_SERVICE_CLIENT_H
