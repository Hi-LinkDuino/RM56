/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_HDI_THERMAL_V1_0_THERMALTYPES_H
#define OHOS_HDI_THERMAL_V1_0_THERMALTYPES_H

#include <string>
#include <vector>
#include <message_parcel.h>

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
using namespace OHOS;

struct ThermalZoneInfo {
    std::string type;
    int32_t temp;
};

struct HdfThermalCallbackInfo {
    std::vector<ThermalZoneInfo> info;
};

bool ThermalZoneInfoBlockMarshalling(OHOS::MessageParcel &data, const ThermalZoneInfo& dataBlock);

bool ThermalZoneInfoBlockUnmarshalling(OHOS::MessageParcel &data, ThermalZoneInfo& dataBlock);

bool HdfThermalCallbackInfoBlockMarshalling(OHOS::MessageParcel &data, const HdfThermalCallbackInfo& dataBlock);

bool HdfThermalCallbackInfoBlockUnmarshalling(OHOS::MessageParcel &data, HdfThermalCallbackInfo& dataBlock);
} // V1_0
} // Thermal
} // HDI
} // OHOS

#endif // OHOS_HDI_THERMAL_V1_0_THERMALTYPES_H
