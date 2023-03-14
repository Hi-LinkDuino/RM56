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

#ifndef HOS_CAMERA_V4L2_DEVICE_MANAGER_H
#define HOS_CAMERA_V4L2_DEVICE_MANAGER_H

#include <iostream>
#include <vector>
#include "create_devicemanager_factory.h"
#include "icontroller.h"
#include "idevice_manager.h"
#include "enumerator_manager.h"
#include "imanager.h"
#include "v4l2_dev.h"
#include "device_manager_adapter.h"

namespace OHOS::Camera {
class V4L2DeviceManager : public IDeviceManager {
    DECLARE_DEVICEMANAGER(V4L2DeviceManager)
public:
    V4L2DeviceManager();
    virtual ~V4L2DeviceManager();
    std::shared_ptr<IManager> GetManager(ManagerId managerId);
    std::shared_ptr<ISensor> GetSensor(CameraId cameraId);
    RetCode DestroyManager();
    RetCode Init();
    std::shared_ptr<IController> GetController(CameraId cameraId, ManagerId managerId, ControllerId controllerId);
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    std::vector<CameraId> GetCameraId();
    RetCode SetFlashlight(FlashMode flashMode, bool enable, CameraId cameraId = CAMERA_MAX);
    void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    void SetMetaDataCallBack(const MetaDataCb cb, CameraId cameraId = CAMERA_MAX);
    void SetHotplugDevCallBack(HotplugDevCb cb);

private:
    void UvcCallBack(const std::string hardwareName, std::vector<DeviceControl>& deviceControl,
        std::vector<DeviceFormat>& deviceFormat, bool uvcState);
    void UvcinfoToMetaData();
    RetCode CreateController(CameraId cameraId, std::shared_ptr<IManager> manager, ManagerId managerId);
    RetCode DestroyController();
    bool CheckManagerList(ManagerId managerId);
    bool CheckCameraIdList(CameraId cameraId);
    RetCode CreateManager();
    std::string CameraIdToHardware(CameraId cameraId, ManagerId managerId);
    CameraId ReturnEnableCameraId(std::string hardwareName);

private:
    HotplugDevCb uvcCb_ = nullptr;
    std::shared_ptr<EnumeratorManager> enumeratorManager_;
    std::vector<HardwareConfiguration> hardwareList_;
    std::vector<std::shared_ptr<IManager>> managerList_;
};
} // namespace OHOS::Camera
#endif
