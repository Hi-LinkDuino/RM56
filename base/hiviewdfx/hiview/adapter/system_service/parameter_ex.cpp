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
#include "parameter_ex.h"
#include <cstdint>
namespace OHOS {
namespace HiviewDFX {
namespace Parameter {
bool IsBetaVersion()
{
    auto userType = GetInteger(KEY_HIVIEW_USER_TYPE, UserType::COMMERCIAL);
    return ((userType == UserType::BETA) || (userType == UserType::OVERSEAS_BETA));
}

DeviceType GetDeviceType()
{
    std::string deviceType = GetString(KEY_BUILD_CHARACTER, "unknown");
    if (deviceType == "phone" || deviceType == "default") {
        return DeviceType::PHONE;
    } else if (deviceType == "watch") {
        return DeviceType::WATCH;
    } else if (deviceType == "tv") {
        return DeviceType::TV;
    } else if (deviceType == "car") {
        return DeviceType::IVI;
    } else {
        return DeviceType::UNKNOWN;
    }
}
} // namespace Parameter
} // namespace HiviewDFX
} // namespace OHOS
