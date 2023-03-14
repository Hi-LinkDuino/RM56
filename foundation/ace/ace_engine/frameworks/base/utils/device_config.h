/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_DEVICE_CONFIG_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_DEVICE_CONFIG_H

#include <string>
#include <vector>

#include "base/utils/device_type.h"

namespace OHOS::Ace {

enum class DeviceOrientation : int32_t {
    PORTRAIT,
    LANDSCAPE,
    ORIENTATION_UNDEFINED,
};

enum class ColorMode : int32_t {
    LIGHT = 0,
    DARK,
    COLOR_MODE_UNDEFINED,
};

struct DeviceConfig {
    DeviceOrientation orientation { DeviceOrientation::PORTRAIT };
    double density { 1.0 };
    DeviceType deviceType { DeviceType::PHONE };
    double fontRatio { 1.0 };
    ColorMode colorMode { ColorMode::LIGHT };
};

struct DeviceResourceInfo {
    DeviceConfig deviceConfig;
    std::vector<int64_t> resourcehandlers;
    std::string packagePath;
    int32_t themeId { -1 };
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_UTILS_DEVICE_CONFIG_H
