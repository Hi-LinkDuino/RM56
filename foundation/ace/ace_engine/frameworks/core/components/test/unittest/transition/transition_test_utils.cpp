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

#include "base/utils/system_properties.h"

namespace OHOS::Ace {
namespace {
DeviceType g_deviceType = DeviceType::TV;
}

bool SystemProperties::traceEnabled_ = false;
bool SystemProperties::rosenBackendEnabled_ = true;
bool SystemProperties::windowAnimationEnabled_ = true;

void SystemProperties::InitDeviceType(DeviceType type)
{
    // Treat all other device type as phone.
    g_deviceType = type;
}

DeviceType SystemProperties::GetDeviceType()
{
    return g_deviceType;
}

float SystemProperties::GetFontWeightScale()
{
    return 1.0f;
}
} // namespace OHOS::Ace
