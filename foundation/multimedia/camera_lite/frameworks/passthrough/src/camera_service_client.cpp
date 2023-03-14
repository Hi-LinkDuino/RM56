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
#include "camera_service_client.h"

#include "media_log.h"
#include "camera_type.h"
#include "camera_manager.h"
#include "meta_data.h"

using namespace std;
namespace OHOS {
namespace Media {
CameraServiceClient *CameraServiceClient::GetInstance()
{
    static CameraServiceClient client;
    return &client;
}

CameraServiceClient::CameraServiceClient()
{
    cameraService_ = CameraService::GetInstance();
}

CameraServiceClient::~CameraServiceClient() {}

void CameraServiceClient::InitCameraServiceClient(CameraServiceCallback *callback)
{
    cameraServiceCb_ = callback;
    cameraService_->Initialize();
    list<string> cameraList = cameraService_->GetCameraIdList();
    cameraServiceCb_->OnCameraServiceInitialized(cameraList);
}

list<string> CameraServiceClient::GetCameraIdList()
{
    return cameraService_->GetCameraIdList();
}

uint8_t CameraServiceClient::GetCameraModeNum()
{
    return cameraService_->GetCameraModeNum();
}

CameraAbility *CameraServiceClient::GetCameraAbility(string &cameraId)
{
    return cameraService_->GetCameraAbility(cameraId);
}

CameraInfo *CameraServiceClient::GetCameraInfo(string &cameraId)
{
    return cameraService_->GetCameraInfo(cameraId);
}

int32_t CameraServiceClient::SetCameraMode(uint8_t modeIndex)
{
    return cameraService_->SetCameraMode(modeIndex);
}

void CameraServiceClient::CreateCamera(string cameraId)
{
    CameraServiceCallback::CameraStauts cameraStatus =
        static_cast<CameraServiceCallback::CameraStauts>(cameraService_->CreateCamera(cameraId));
    cameraServiceCb_->OnCameraStatusChange(cameraId, cameraStatus);
}
} // namespace Media
} // namespace OHOS
