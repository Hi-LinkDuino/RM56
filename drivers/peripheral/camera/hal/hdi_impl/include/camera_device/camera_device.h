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

#ifndef CAMERA_DEVICE_CAMERA_DEVICE_H
#define CAMERA_DEVICE_CAMERA_DEVICE_H

#include "icamera_device.h"
#include "camera_device_stub.h"
#include "camera.h"

namespace OHOS::Camera {
class StreamOperator;
class IPipelineCore;
class CameraDevice : public CameraDeviceStub {
public:
    static std::shared_ptr<CameraDevice> CreateCameraDevice(const std::string &cameraId);
    CameraDevice() = default;
    virtual ~CameraDevice() = default;
    CameraDevice(const CameraDevice &other) = delete;
    CameraDevice(CameraDevice &&other) = delete;
    CameraDevice& operator=(const CameraDevice &other) = delete;
    CameraDevice& operator=(CameraDevice &&other) = delete;

public:
    virtual std::shared_ptr<IPipelineCore> GetPipelineCore() const = 0;
    virtual CamRetCode SetCallback(const OHOS::sptr<ICameraDeviceCallback> &callback) = 0;
    virtual ResultCallbackMode GetMetaResultMode() const = 0;
    /* RC_OK metadata changed；RC_ERROR metadata unchanged； */
    virtual RetCode GetMetadataResults(std::shared_ptr<CameraStandard::CameraMetadata> &metadata) = 0;
    virtual void ResultMetadata() = 0;
    virtual void GetCameraId(std::string &cameraId) const = 0;
    virtual bool IsOpened() const = 0;
    virtual void SetStatus(bool isOpened) = 0;

protected:
    virtual void OnMetadataChanged(const std::shared_ptr<CameraStandard::CameraMetadata> &metadata) = 0;
    virtual void OnDevStatusErr() = 0;
};
} // end namespace OHOS::Camera
#endif // CAMERA_DEVICE_CAMERA_DEVICE_H
