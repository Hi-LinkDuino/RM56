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

#include "v4l2_device_manager.h"
#include "isp_manager.h"
#include "flash_manager.h"
#include "sensor_manager.h"
#include "enumerator_manager.h"
#include "project_hardware.h"

namespace OHOS::Camera {
IMPLEMENT_DEVICEMANAGER(V4L2DeviceManager)
V4L2DeviceManager::V4L2DeviceManager() {}

V4L2DeviceManager::~V4L2DeviceManager() {}

RetCode V4L2DeviceManager::Init()
{
    RetCode rc = RC_ERROR;

    std::vector<std::string> hardwareName;
    for (auto iter = hardware.cbegin(); iter != hardware.cend(); iter++) {
        if ((*iter).controllerId == DM_C_SENSOR) {
            hardwareName.push_back((*iter).hardwareName);
        }
    }
    rc = HosV4L2Dev::Init(hardwareName);
    if (rc == RC_ERROR) {
        CAMERA_LOGE("%s HosV4L2Dev Init fail", __FUNCTION__);
        return RC_ERROR;
    }

    for (auto iter = hardware.cbegin(); iter != hardware.cend(); iter++) {
        bool eraseHardwareNameFlage = true;
        for (auto iterName = hardwareName.cbegin(); iterName != hardwareName.cend(); iterName++) {
            if ((*iter).controllerId == DM_C_SENSOR && (*iterName) == (*iter).hardwareName) {
                eraseHardwareNameFlage = false;
            }
        }
        if (eraseHardwareNameFlage == true && (*iter).controllerId == DM_C_SENSOR) {
            hardware.erase(iter);
        }
    }
    for (auto iter = hardware.cbegin(); iter != hardware.cend(); iter++) {
        hardwareList_.push_back(*iter);
    }
    rc = CreateManager();
    enumeratorManager_ = std::make_shared<EnumeratorManager>();
    if (enumeratorManager_ == nullptr) {
        CAMERA_LOGE("%s Create EnumeratorManager fail", __FUNCTION__);
        return RC_ERROR;
    }
    rc = enumeratorManager_->Init();
    if (rc == RC_ERROR) {
        CAMERA_LOGE("%s EnumeratorManager Init fail", __FUNCTION__);
        return rc;
    }
    return rc;
}

std::vector<CameraId> V4L2DeviceManager::GetCameraId()
{
    std::vector<CameraId> sensor_list;
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).controllerId == DM_C_SENSOR) {
            sensor_list.push_back((*iter).cameraId);
        }
    }
    return sensor_list;
};

RetCode V4L2DeviceManager::PowerUp(CameraId cameraId)
{
    if (CheckCameraIdList(cameraId) == false) {
        return RC_ERROR;
    }
    RetCode rc = RC_OK;
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        rc = (*iter)->PowerUp(CameraIdToHardware(cameraId, (*iter)->GetManagerId()));
        if (rc == RC_ERROR) {
            return RC_ERROR;
        }
    }
    return rc;
}

RetCode V4L2DeviceManager::PowerDown(CameraId cameraId)
{
    if (CheckCameraIdList(cameraId) == false) {
        return RC_ERROR;
    }
    RetCode rc = RC_OK;
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        rc = (*iter)->PowerDown(CameraIdToHardware(cameraId, (*iter)->GetManagerId()));
        if (rc == RC_ERROR) {
            return RC_ERROR;
        }
    }
    return rc;
}

std::shared_ptr<ISensor> V4L2DeviceManager::GetSensor(CameraId cameraId)
{
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).controllerId == DM_C_SENSOR && (*iter).cameraId == cameraId) {
            return (std::static_pointer_cast<SensorManager>(GetManager(DM_M_SENSOR)))->GetSensor((*iter).hardwareName);
        }
    }
    return nullptr;
}

std::shared_ptr<IManager> V4L2DeviceManager::GetManager(ManagerId managerId)
{
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        if ((*iter)->GetManagerId() == managerId) {
            return (*iter);
        }
    }
    return nullptr;
}
RetCode V4L2DeviceManager::CreateManager()
{
    RetCode rc = RC_OK;
    std::shared_ptr<IManager> manager = nullptr;
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if (CheckManagerList((*iter).managerId) == false) {
            switch ((*iter).managerId) {
                case DM_M_SENSOR:
                    manager = std::make_shared<SensorManager>(DM_M_SENSOR);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController((*iter).cameraId, manager, DM_M_SENSOR);
                    break;
                case DM_M_FLASH:
                    manager = std::make_shared<FlashManager>(DM_M_FLASH);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController((*iter).cameraId, manager, DM_M_FLASH);
                    break;
                case DM_M_ISP:
                    manager = std::make_shared<IspManager>(DM_M_ISP);
                    CHECK_IF_PTR_NULL_RETURN_VALUE(manager, RC_ERROR);
                    rc = CreateController((*iter).cameraId, manager, DM_M_ISP);
                    break;
                default:
                    break;
            }
            if (rc == RC_ERROR) {
                return RC_ERROR;
            } else {
                managerList_.push_back(manager);
        }
        }
    }
    return rc;
}
RetCode V4L2DeviceManager::DestroyManager()
{
    return RC_OK;
}
std::shared_ptr<IController> V4L2DeviceManager::GetController(CameraId cameraId, ManagerId managerId,
    ControllerId controllerId)
{
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        if ((*iter)->GetManagerId() == managerId) {
            return (*iter)->GetController(controllerId, CameraIdToHardware(cameraId, (*iter)->GetManagerId()));
        }
    }
    return nullptr;
}
RetCode V4L2DeviceManager::CreateController(CameraId cameraId, std::shared_ptr<IManager> manager, ManagerId managerId)
{
    RetCode rc = RC_OK;
    (void)cameraId;
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).managerId == managerId) {
            switch (managerId) {
                case DM_M_SENSOR:
                    rc = manager->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                case DM_M_FLASH:
                    rc = manager->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                case DM_M_ISP:
                    rc = manager->CreateController((*iter).controllerId, (*iter).hardwareName);
                    break;
                default:
                    break;
            }
        }
        if (RC_ERROR == rc) {
            return RC_ERROR;
        }
    }
    return rc;
}
RetCode V4L2DeviceManager::DestroyController()
{
    return RC_OK;
}

bool V4L2DeviceManager::CheckCameraIdList(CameraId cameraId)
{
    if (hardwareList_.size() == 0) {
        return false;
    }
    for (auto it = hardwareList_.cbegin(); it != hardwareList_.cend(); it++) {
        if ((*it).cameraId == cameraId) {
            return true;
        }
    }
    return false;
}

bool V4L2DeviceManager::CheckManagerList(ManagerId managerId)
{
    if (managerList_.size() == 0) {
        return false;
    }
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        if ((*iter)->GetManagerId() == managerId) {
            return true;
        }
    }
    return false;
}

void V4L2DeviceManager::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    if (managerList_.size() != 0) {
        for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
            (*iter)->Configure(meta);
        }
    }
}

RetCode V4L2DeviceManager::SetFlashlight(FlashMode flashMode, bool enable, CameraId cameraId)
{
    if (cameraId == CAMERA_MAX) {
        return std::static_pointer_cast<FlashController>(
            GetController(CAMERA_FIRST, DM_M_FLASH, DM_C_FLASH))->SetFlashlight(flashMode, enable);
    } else {
        return std::static_pointer_cast<FlashController>(
            GetController(cameraId, DM_M_FLASH, DM_C_FLASH))->SetFlashlight(flashMode, enable);
    }
}

void V4L2DeviceManager::SetMetaDataCallBack(const MetaDataCb cb, CameraId cameraId)
{
    if (managerList_.size() == 0) {
        return;
    }
    for (auto iter = managerList_.cbegin(); iter != managerList_.cend(); iter++) {
        if ((*iter)->GetManagerId() == DM_M_SENSOR) {
            if (cameraId == CAMERA_MAX) {
                (std::static_pointer_cast<SensorManager>(*iter))->SetMetaDataCallBack(cb,
                    CameraIdToHardware(CAMERA_FIRST, DM_M_SENSOR));
            } else {
                (std::static_pointer_cast<SensorManager>(*iter))->SetMetaDataCallBack(cb,
                    CameraIdToHardware(cameraId, DM_M_SENSOR));
            }
        }
    }
}

std::string V4L2DeviceManager::CameraIdToHardware(CameraId cameraId, ManagerId managerId)
{
    for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
        if ((*iter).managerId == managerId && (*iter).cameraId == cameraId) {
            return (*iter).hardwareName;
        }
    }
    return nullptr;
}

void V4L2DeviceManager::SetHotplugDevCallBack(HotplugDevCb cb)
{
    uvcCb_ = cb;
    enumeratorManager_->SetCallBack([&](const std::string hardwareName, std::vector<DeviceControl>& deviceControl,
        std::vector<DeviceFormat>& deviceFormat, bool uvcState) {
        UvcCallBack(hardwareName, deviceControl, deviceFormat, uvcState);
    });
}

void V4L2DeviceManager::UvcCallBack(const std::string hardwareName, std::vector<DeviceControl>& deviceControl,
    std::vector<DeviceFormat>& deviceFormat, bool uvcState)
{
    if (uvcState) {
        if (deviceControl.empty() || deviceFormat.empty()) {
            CAMERA_LOGI("V4L2DeviceManager::UvcCallBack %{public}s is empty", hardwareName.c_str());
            return;
        }
        CAMERA_LOGI("uvc plug in %{public}s begin", hardwareName.c_str());
        CameraId id = ReturnEnableCameraId("");
        CHECK_IF_EQUAL_RETURN_VOID(id, CAMERA_MAX);

        RetCode rc = GetManager(DM_M_SENSOR)->CreateController(DM_C_SENSOR, hardwareName);
        CHECK_IF_EQUAL_RETURN_VOID(rc, RC_ERROR);

        HardwareConfiguration hardware;
        hardware.cameraId = id;
        hardware.managerId = DM_M_SENSOR;
        hardware.controllerId = DM_C_SENSOR;
        hardware.hardwareName = hardwareName;
        hardwareList_.push_back(hardware);
        std::vector<float> physicalSize;
        for (auto iter = deviceFormat.cbegin(); iter != deviceFormat.cend(); iter++) {
            physicalSize.push_back((*iter).fmtdesc.width);
            physicalSize.push_back((*iter).fmtdesc.height);
        }
        std::shared_ptr<CameraStandard::CameraMetadata> meta = std::make_shared<CameraStandard::CameraMetadata>(30,
            2000);
        meta->addEntry(OHOS_SENSOR_INFO_PHYSICAL_SIZE, physicalSize.data(), physicalSize.size());
        CHECK_IF_PTR_NULL_RETURN_VOID(uvcCb_);

        uvcCb_(meta, uvcState, id);
        CAMERA_LOGI("uvc plug in %{public}s end", hardwareName.c_str());
    } else {
        CAMERA_LOGI("uvc plug out %{public}s begin", hardwareName.c_str());
        CameraId id = ReturnEnableCameraId(hardwareName);
        CHECK_IF_EQUAL_RETURN_VOID(id, CAMERA_MAX);
        CHECK_IF_PTR_NULL_RETURN_VOID(uvcCb_);

        for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
            if ((*iter).hardwareName == hardwareName) {
                std::shared_ptr<CameraStandard::CameraMetadata> meta =
                    std::make_shared<CameraStandard::CameraMetadata>(30, 2000);
                uvcCb_(meta, uvcState, id);
                hardwareList_.erase(iter);
                break;
            }
        }
        CAMERA_LOGI("uvc plug out %{public}s end", hardwareName.c_str());
    }
}

CameraId V4L2DeviceManager::ReturnEnableCameraId(std::string hardwareName)
{
    if (hardwareName.size() == 0) {
        bool enable = true;
        for (CameraId id = CAMERA_FIRST; id <= CAMERA_MAX - 0; id = (CameraId)(id + 1)) {
            for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
                if ((*iter).cameraId == id) {
                    enable = false;
                    break;
                } else {
                    enable = true;
                }
            }
            if (enable == true) {
                return id;
            }
        }
        if (enable == false) {
            return CAMERA_MAX;
        }
    } else {
        for (auto iter = hardwareList_.cbegin(); iter != hardwareList_.cend(); iter++) {
            if (hardwareName == (*iter).hardwareName) {
                return (*iter).cameraId;
            }
        }
        return CAMERA_MAX;
    }
    return CAMERA_MAX;
}
} // namespace OHOS::Camera
