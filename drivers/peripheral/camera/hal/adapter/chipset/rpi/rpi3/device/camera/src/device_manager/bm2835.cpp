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

#include "bm2835.h"
#include <vector>

namespace OHOS::Camera {
IMPLEMENT_SENSOR(Bm2835)
Bm2835::Bm2835() : ISensor("bm2835") {}

Bm2835::~Bm2835() {}
void Bm2835::InitPhysicalSize(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitPhysicalSize(camera_meta_data);
}
void Bm2835::InitAntiBandingModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitAntiBandingModes(camera_meta_data);
}
void Bm2835::InitAvailableModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitAvailableModes(camera_meta_data);
}
void Bm2835::InitFpsTarget(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitFpsTarget(camera_meta_data);
}
void Bm2835::InitCompensationRange(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitCompensationRange(camera_meta_data);
}

void Bm2835::InitAwbModes(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitAwbModes(camera_meta_data);
}

void Bm2835::InitSensitivityRange(CameraStandard::CameraMetadata& camera_meta_data)
{
    ISensor::InitSensitivityRange(camera_meta_data);
}

void Bm2835::Init(CameraStandard::CameraMetadata& camera_metaData)
{
    InitPhysicalSize(camera_metaData);
    InitAntiBandingModes(camera_metaData);
    InitAvailableModes(camera_metaData);
    InitFpsTarget(camera_metaData);
    InitCompensationRange(camera_metaData);

    const camera_rational_t aeCompensationStep[] = {{0, 1}};
    camera_metaData.addEntry(OHOS_CONTROL_AE_COMPENSATION_STEP, aeCompensationStep, 1);

    InitAwbModes(camera_metaData);
    InitSensitivityRange(camera_metaData);

    uint8_t faceDetectMode = OHOS_CAMERA_FACE_DETECT_MODE_OFF;
    camera_metaData.addEntry(OHOS_STATISTICS_FACE_DETECT_MODE, &faceDetectMode, 1);
}
} // namespace OHOS::Camera
