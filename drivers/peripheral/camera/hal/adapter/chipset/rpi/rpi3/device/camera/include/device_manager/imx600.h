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

#ifndef HOS_CAMERA_IMX600_H
#define HOS_CAMERA_IMX600_H

#include "isensor.h"
#include "create_sensor_factory.h"
#include "device_manager_adapter.h"

namespace OHOS::Camera {
class Imx600 : public ISensor {
    DECLARE_SENSOR(Imx600)
public:
    Imx600();
    virtual ~Imx600();
    void Init(CameraStandard::CameraMetadata& camera_meta_data);
    void InitPhysicalSize(CameraStandard::CameraMetadata& camera_meta_data);
    void InitAntiBandingModes(CameraStandard::CameraMetadata& camera_meta_data);
    void InitAeFpsTarget(CameraStandard::CameraMetadata& camera_meta_data);
    void InitCompensationRange(CameraStandard::CameraMetadata& camera_meta_data);
    void InitSensitivityRange(CameraStandard::CameraMetadata& camera_meta_data);
};
} // namespace OHOS::Camera
#endif