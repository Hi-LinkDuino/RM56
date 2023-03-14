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

#include "camera_device_impl.h"
#include "ipipeline_core.h"
#include "idevice_manager.h"

namespace OHOS::Camera {
std::shared_ptr<CameraDevice> CameraDevice::CreateCameraDevice(const std::string &cameraId)
{
    // create pipelineCore
    std::shared_ptr<IPipelineCore> pipelineCore = IPipelineCore::Create();
    if (pipelineCore == nullptr) {
        CAMERA_LOGW("create pipeline core failed. [cameraId = %{public}s]", cameraId.c_str());
        return nullptr;
    }

    RetCode rc = pipelineCore->Init();
    if (rc != RC_OK) {
        CAMERA_LOGW("pipeline core init failed. [cameraId = %{public}s]", cameraId.c_str());
        return nullptr;
    }

    std::shared_ptr<CameraDeviceImpl> device = std::make_shared<CameraDeviceImpl>(cameraId, pipelineCore);
    if (device == nullptr) {
        CAMERA_LOGW("create camera device failed. [cameraId = %{public}s]", cameraId.c_str());
        return nullptr;
    }
    CAMERA_LOGD("create camera device success. [cameraId = %{public}s]", cameraId.c_str());

    // set deviceManager metadata & dev status callback
    std::shared_ptr<IDeviceManager> deviceManager = IDeviceManager::GetInstance();
    if (deviceManager != nullptr) {
        deviceManager->SetMetaDataCallBack([device](const std::shared_ptr<CameraStandard::CameraMetadata> &metadata) {
            std::static_pointer_cast<CameraDevice>(device)->OnMetadataChanged(metadata);
        });
        deviceManager->SetDevStatusCallBack([device]() {
            std::static_pointer_cast<CameraDevice>(device)->OnDevStatusErr();
        });
    }

    return device;
}
} // end namespace OHOS::Camera

