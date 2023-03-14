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

#include "imx600.h"
#include <vector>

namespace OHOS::Camera {
IMPLEMENT_SENSOR(Imx600)
Imx600::Imx600() : ISensor("imx600") {}

Imx600::~Imx600() {}

void Imx600::InitPhysicalSize(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitPhysicalSize(camera_meta_data);
}

void Imx600::InitAntiBandingModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitAntiBandingModes(camera_meta_data);
}

void Imx600::InitAeFpsTarget(CameraStandard::CameraMetadata& camera_meta_data)
{
    std::vector<int32_t> availableAeFpsTarget = {
        15,
        30,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, availableAeFpsTarget.data(),
                              availableAeFpsTarget.size());
}

void Imx600::InitCompensationRange(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitCompensationRange(camera_meta_data);
}

void Imx600::InitSensitivityRange(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitSensitivityRange(camera_meta_data);
}

void Imx600::Init(CameraStandard::CameraMetadata& camera_meta_data)
{
    InitPhysicalSize(camera_meta_data);
    InitAntiBandingModes(camera_meta_data);

    std::vector<uint8_t> aeAvailableModes = {
        OHOS_CAMERA_AE_MODE_ON,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AE_AVAILABLE_MODES, aeAvailableModes.data(),
                              aeAvailableModes.size());

    InitAeFpsTarget(camera_meta_data);
    InitCompensationRange(camera_meta_data);

    const camera_rational_t aeCompensationStep[] = {{0, 1}};
    camera_meta_data.addEntry(OHOS_CONTROL_AE_COMPENSATION_STEP, aeCompensationStep, 1);

    std::vector<uint8_t> availableAwbModes = {
        OHOS_CAMERA_AWB_MODE_OFF,
    };
    camera_meta_data.addEntry(OHOS_CONTROL_AWB_AVAILABLE_MODES, availableAwbModes.data(),
                              availableAwbModes.size());

    InitSensitivityRange(camera_meta_data);

    uint8_t faceDetectMode = OHOS_CAMERA_FACE_DETECT_MODE_OFF;
    camera_meta_data.addEntry(OHOS_STATISTICS_FACE_DETECT_MODE, &faceDetectMode,
                              1);
}
} // namespace OHOS::Camera