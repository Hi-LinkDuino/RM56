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

#include "sensor_manager.h"
#include "create_sensor_factory.h"

namespace OHOS::Camera {
SensorManager::SensorManager() {}

SensorManager::SensorManager(ManagerId managerId) : IManager(managerId) {}

SensorManager::~SensorManager() {}

RetCode SensorManager::CreateController(ControllerId controllerId, std::string hardwareName)
{
    RetCode rc = RC_OK;
    if (controllerId == DM_C_SENSOR) {
        if (!CheckCameraIdList(hardwareName)) {
            std::shared_ptr<SensorController> sensor = std::make_shared<SensorController>(hardwareName);
            if (sensor == nullptr) {
                return RC_ERROR;
            }
            rc = sensor->Init();
            if (rc == RC_OK) {
                sensorList_.push_back(sensor);
            }
        }
    }
    return rc;
}

std::shared_ptr<IController> SensorManager::GetController(ControllerId controllerId, std::string hardwareName)
{
    (void)controllerId;
    if (sensorList_.size() == 0) {
        return nullptr;
    }
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return *iter;
        }
    }
    return nullptr;
}

void SensorManager::Configure(std::shared_ptr<CameraStandard::CameraMetadata> meta)
{
    if (sensorList_.size() == 0) {
        return;
    }
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        (*iter)->Configure(meta);
    }
}

RetCode SensorManager::PowerUp(std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->PowerUp();
        }
    }
    return RC_ERROR;
}

RetCode SensorManager::PowerDown(std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->PowerDown();
        }
    }
    return RC_ERROR;
}

RetCode SensorManager::Start(std::string hardwareName, int buffCont, DeviceFormat& format)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->Start(buffCont, format);
        }
    }
    return RC_ERROR;
}

RetCode SensorManager::Stop(std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->Stop();
        }
    }
    return RC_ERROR;
}

RetCode SensorManager::SendFrameBuffer(std::shared_ptr<FrameSpec> buffer, std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return (*iter)->SendFrameBuffer(buffer);
        }
    }
    return RC_ERROR;
}

void SensorManager::SetAbilityMetaDataTag(std::vector<int32_t> abilityMetaDataTag, std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            (*iter)->SetAbilityMetaDataTag(abilityMetaDataTag);
        }
    }
}

std::shared_ptr<ISensor> SensorManager::GetSensor(std::string sensorName)
{
    CAMERA_LOGI("%s GetSensor %s", __FUNCTION__, sensorName.c_str());
    return GetSensorFactory.GetSensorByName(sensorName);
}

bool SensorManager::CheckCameraIdList(std::string hardwareName)
{
    if (sensorList_.size() == 0) {
        return false;
    }
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            return true;
        }
    }
    return false;
}

void SensorManager::SetNodeCallBack(const NodeBufferCb cb, std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            (*iter)->SetNodeCallBack(cb);
        }
    }
}

void SensorManager::SetMetaDataCallBack(const MetaDataCb cb, std::string hardwareName)
{
    for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
        if ((*iter)->GetName() == hardwareName) {
            (*iter)->SetMetaDataCallBack(cb);
        }
    }
}

RetCode SensorManager::DestroyController(ControllerId controllerId, std::string hardwareName)
{
    if (controllerId == DM_C_SENSOR) {
        for (auto iter = sensorList_.cbegin(); iter != sensorList_.cend(); iter++) {
            if ((*iter)->GetName() == hardwareName) {
                sensorList_.erase(iter);
                return RC_OK;
            }
        }
    }
    return RC_ERROR;
}
} // namespace OHOS::Camera
