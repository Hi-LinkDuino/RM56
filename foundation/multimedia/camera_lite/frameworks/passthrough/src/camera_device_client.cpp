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

#include "camera_device_client.h"
#include "camera_impl.h"
#include "media_log.h"
#include "camera_config.h"
#include "frame_config.h"

using namespace std;
namespace OHOS {
namespace Media {
CameraDeviceClient *CameraDeviceClient::GetInstance()
{
    static CameraDeviceClient client;
    return &client;
}

CameraDeviceClient::CameraDeviceClient()
{
    cameraService_ = CameraService::GetInstance();
}

CameraDeviceClient::~CameraDeviceClient() {}

void CameraDeviceClient::SetCameraId(string cameraId)
{
    cameraId_ = cameraId;
}

void CameraDeviceClient::SetCameraImpl(CameraImpl *cameraImpl)
{
    cameraImpl_ = cameraImpl;
}
void CameraDeviceClient::SetCameraCallback() {}

int32_t CameraDeviceClient::SetCameraConfig(CameraConfig &cc)
{
    CameraDevice *device_ = cameraService_->GetCameraDevice(cameraId_);
    if (device_ == nullptr) {
        MEDIA_ERR_LOG("device_ is null");
        return -1;
    }
    cc_ = &cc;
    int32_t ret = device_->SetCameraConfig();
    cameraImpl_->OnConfigured(ret, *cc_);
    return ret;
}


int32_t CameraDeviceClient::TriggerLoopingCapture(FrameConfig &fc)
{
    CameraDevice *device_ = cameraService_->GetCameraDevice(cameraId_);
    if (device_ == nullptr) {
        MEDIA_ERR_LOG("device_ is null");
        return -1;
    }
    uint32_t streamId = 0;
    return device_->TriggerLoopingCapture(fc, &streamId);
}

int32_t CameraDeviceClient::TriggerSingleCapture(FrameConfig &fc)
{
    CameraDevice *device_ = cameraService_->GetCameraDevice(cameraId_);
    if (device_ == nullptr) {
        MEDIA_ERR_LOG("device_ is null");
        return -1;
    }
    uint32_t streamId = 0;
    int32_t ret = device_->TriggerSingleCapture(fc, &streamId);
    cameraImpl_->OnFrameFinished(ret, fc);
    return ret;
}

void CameraDeviceClient::StopLoopingCapture(int32_t type)
{
    CameraDevice *device_ = cameraService_->GetCameraDevice(cameraId_);
    if (device_ == nullptr) {
        MEDIA_ERR_LOG("device_ is null");
        return;
    }
    return device_->StopLoopingCapture(type);
}

void CameraDeviceClient::Release()
{
    cameraService_->CloseCamera(cameraId_);
}
}
}