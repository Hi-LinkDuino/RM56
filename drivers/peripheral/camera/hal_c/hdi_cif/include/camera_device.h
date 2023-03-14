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

#include "camera_device_c_if.h"
#include "camera_metadata_c_if.h"
#include "camera_types_c_if.h"
#include "icamera_device.h"
#include "camera_device_stub.h"

namespace OHOS::Camera {
class CameraDevice : public CameraDeviceStub {
public:
    CameraDevice();
    virtual ~CameraDevice();
    CameraDevice(const CameraDevice& other) = delete;
    CameraDevice(CameraDevice&& other) = delete;
    CameraDevice& operator=(const CameraDevice& other) = delete;
    CameraDevice& operator=(CameraDevice&& other) = delete;

public:
    void Init(CameraDeviceCIF* device);
    virtual CamRetCode GetStreamOperator(const OHOS::sptr<IStreamOperatorCallback>& callback,
                                         OHOS::sptr<IStreamOperator>& streamOperator) override;
    virtual CamRetCode UpdateSettings(const std::shared_ptr<CameraSetting>& settings) override;
    virtual CamRetCode SetResultMode(const ResultCallbackMode& mode) override;
    virtual CamRetCode GetEnabledResults(std::vector<MetaType>& results) override;
    virtual CamRetCode EnableResult(const std::vector<MetaType>& results) override;
    virtual CamRetCode DisableResult(const std::vector<MetaType>& results) override;
    virtual void Close() override;

private:
    CameraDeviceCIF* device_ = nullptr;
};
} // end namespace OHOS::Camera
#endif // CAMERA_DEVICE_CAMERA_DEVICE_H
