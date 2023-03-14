/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HOS_CAMERA_DEVICE_MANAGER_ADAPTER_H
#define HOS_CAMERA_DEVICE_MANAGER_ADAPTER_H

#include "camera.h"
#include "ibuffer.h"
#include <memory>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <type_traits>
#include <vector>
#include "camera_metadata_info.h"
#include "stream.h"
#include <functional>

namespace OHOS {
namespace Camera {
enum ManagerId {
    DM_M_ISP,
    DM_M_SENSOR,
    DM_M_FLASH,
    DM_M_VENC,
    DM_M_VI,
    DM_M_VO,
    DM_M_VPSS,
    DM_M_MAX
};

enum ControllerId : uint32_t{
    DM_C_ISP,
    DM_C_SENSOR,
    DM_C_FLASH,
    DM_C_VCM,
    DM_C_OIS,
    DM_C_VENC,
    DM_C_VI,
    DM_C_VO,
    DM_C_VPSS,
    DM_C_MAX
};

enum CameraId : int32_t {
    CAMERA_FIRST = 0,
    CAMERA_SECOND,
    CAMERA_THIRD,
    CAMERA_FOURTH,
    CAMERA_FIFTH,
    CAMERA_SIXTH,
    CAMERA_MAX
};

struct HardwareConfiguration {
    CameraId cameraId;
    ManagerId managerId;
    ControllerId controllerId;
    std::string hardwareName;
};
using HardwareConfiguration = struct HardwareConfiguration;

struct camera_metadata_rational {
    int32_t numerator;
    int32_t denominator;
};
using camera_rational_t = struct camera_metadata_rational;

using NodeBufferCb = std::function<void(std::shared_ptr<FrameSpec>)>;
using MetaDataCb = std::function<void(std::shared_ptr<CameraStandard::CameraMetadata>)>;
using HotplugDevCb = std::function<void(std::shared_ptr<CameraStandard::CameraMetadata>, bool, CameraId)>;
using DeviceStatusCb = std::function<void()>;

} // namespace Camera
} // namespace OHOS
#endif
