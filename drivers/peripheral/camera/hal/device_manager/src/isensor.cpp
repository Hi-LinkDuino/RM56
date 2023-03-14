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

#include "isensor.h"

namespace OHOS {
namespace Camera {
ISensor::ISensor() {}

ISensor::ISensor(std::string sensorName) : sensorName_(sensorName) {}

ISensor::~ISensor() {}

void ISensor::InitPhysicalSize(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<float> physicalSize = {
        2592,
        1944,
    };
    camera_meta_data.addEntry(OHOS_SENSOR_INFO_PHYSICAL_SIZE, physicalSize.data(),
        physicalSize.size());
}
void ISensor::InitAntiBandingModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<uint8_t> aeAvailableAntiBandingModes = {
        OHOS_CAMERA_AE_ANTIBANDING_MODE_OFF,
        OHOS_CAMERA_AE_ANTIBANDING_MODE_50HZ,
        OHOS_CAMERA_AE_ANTIBANDING_MODE_60HZ,
        OHOS_CAMERA_AE_ANTIBANDING_MODE_AUTO,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AE_AVAILABLE_ANTIBANDING_MODES, aeAvailableAntiBandingModes.data(),
                              aeAvailableAntiBandingModes.size());
}
void ISensor::InitAvailableModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<uint8_t> aeAvailableModes = {
        OHOS_CAMERA_AE_MODE_ON,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AE_AVAILABLE_MODES, aeAvailableModes.data(),
                              aeAvailableModes.size());
}
void ISensor::InitFpsTarget(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<int32_t> availableAeFpsTarget = {
        15,
        30,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, availableAeFpsTarget.data(),
                              availableAeFpsTarget.size());
}
void ISensor::InitCompensationRange(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<int32_t> aeCompensationRange = {
        0,
        0,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AE_COMPENSATION_RANGE, aeCompensationRange.data(),
                              aeCompensationRange.size());
}

void ISensor::InitAwbModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<uint8_t> availableAwbModes = {
        OHOS_CAMERA_AWB_MODE_OFF,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AWB_AVAILABLE_MODES, availableAwbModes.data(),
                              availableAwbModes.size());
}

void ISensor::InitSensitivityRange(CameraStandard::CameraMetadata& camera_meta_data)
{
    int32_t sensitivityRange[] = {
        32,
        2400,
    };
    camera_meta_data.addEntry(OHOS_SENSOR_INFO_SENSITIVITY_RANGE,
        &sensitivityRange,
        sizeof(sensitivityRange) / sizeof(int32_t));
}
} // namespace Camera
} // namespace OHOS