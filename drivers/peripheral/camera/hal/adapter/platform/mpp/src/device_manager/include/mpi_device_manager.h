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

#ifndef HOS_CAMERA_MPI_DEVICE_MANAGER_H
#define HOS_CAMERA_MPI_DEVICE_MANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include "create_devicemanager_factory.h"
#include "icontroller.h"
#include "idevice_manager.h"
#include "imanager.h"
#include "isys_object.h"
#include "device_manager_adapter.h"

namespace OHOS {
namespace Camera {
class MpiDeviceManager : public IDeviceManager {
    DECLARE_DEVICEMANAGER(MpiDeviceManager)
public:
    MpiDeviceManager();
    virtual ~MpiDeviceManager();
    std::shared_ptr<IManager> GetManager(ManagerId managerId);
    std::shared_ptr<ISensor> GetSensor(CameraId cameraId);
    RetCode DestroyManager();
    RetCode Init();
    std::shared_ptr<IController> GetController(ManagerId managerId, ControllerId controllerId);
    RetCode PowerUp(CameraId cameraId);
    RetCode PowerDown(CameraId cameraId);
    std::vector<CameraId> GetCameraId();
    RetCode Connect(std::string controller,
                    std::string portNum,
                    std::string connectController,
                    std::string connectPortNum);
    RetCode UnConnect(std::string controller,
                      std::string portNum,
                      std::string connectController,
                      std::string connectPortNum);
    void SetSendflag(bool flag);
    void BufferCallback(std::shared_ptr<FrameSpec> buffer);
    void SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag);
    RetCode SendFrameBuffer(std::shared_ptr<FrameSpec> buffer, CameraId cameraId = CAMERA_MAX);
    void SetNodeCallBack(const NodeBufferCb cb, CameraId cameraId = CAMERA_MAX);
    void SetMetaDataCallBack(const MetaDataCb cb, CameraId cameraId = CAMERA_MAX);
    void SetDevStatusCallBack(const DeviceStatusCb cb);
    RetCode SetFlashlight(FlashMode flashMode, bool enable, CameraId cameraId = CAMERA_MAX);
    void Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta);
    virtual RetCode PreConfig(const ModeMeta& meta, const std::vector<DeviceStreamSetting>& settings) override;
    virtual RetCode Flush(int32_t streamId) override;
    virtual RetCode StartRecvFrame(int32_t streamId) override;
    virtual RetCode StopRecvFrame(int32_t streamId) override;

private:
    RetCode CreateController(std::shared_ptr<IManager> manager, ManagerId managerId);
    RetCode DestroyController();
    bool CheckManagerList(ManagerId managerId);
    bool CheckCameraIdList(CameraId cameraId);
    ControllerId CheckControllerId(std::string controllerName);
    RetCode CreateManager();
    RetCode StartNode(ControllerId controllerId);
    int32_t CheckNodePortNum(std::string controllerName);
    RetCode GetAbilityMetaData(std::shared_ptr<CameraStandard::CameraMetadata> meta, bool &sendflag);

private:
    std::vector<HardwareConfiguration> hardwareList_;
    std::vector<std::shared_ptr<IManager>> managerList_;
    std::shared_ptr<ISysObject> sysObject_ = nullptr;
    NodeBufferCb nodeBufferCb_;
    MetaDataCb metaDataCb_;
    DeviceStatusCb deviceStatusCb_;
    bool sysInitFlag;
    bool metaSendflag_ = false;
};
} // namespace Camera
} // namespace OHOS
#endif
