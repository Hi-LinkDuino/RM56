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

#ifndef OHOS_HDI_BATTERY_V1_0_TYPES_H
#define OHOS_HDI_BATTERY_V1_0_TYPES_H

#include <string>
#include <message_parcel.h>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
using namespace OHOS;

enum BatteryHealthState {
    BATTERY_HEALTH_UNKNOWN = 0,
    BATTERY_HEALTH_GOOD,
    BATTERY_HEALTH_OVERHEAT,
    BATTERY_HEALTH_OVERVOLTAGE,
    BATTERY_HEALTH_COLD,
    BATTERY_HEALTH_DEAD,
    BATTERY_HEALTH_RESERVED,
};

enum BatteryChargeState {
    CHARGE_STATE_NONE = 0,
    CHARGE_STATE_ENABLE,
    CHARGE_STATE_DISABLE,
    CHARGE_STATE_FULL,
    CHARGE_STATE_RESERVED,
};

enum BatteryPluggedType {
    PLUGGED_TYPE_NONE = 0,
    PLUGGED_TYPE_AC,
    PLUGGED_TYPE_USB,
    PLUGGED_TYPE_WIRELESS,
    PLUGGED_TYPE_BUTT,
};

struct BatteryInfo {
    int32_t capacity;
    int32_t voltage;
    int32_t temperature;
    int32_t healthState;
    int32_t pluggedType;
    int32_t pluggedMaxCurrent;
    int32_t pluggedMaxVoltage;
    int32_t chargeState;
    int32_t chargeCounter;
    int32_t totalEnergy;
    int32_t curAverage;
    int32_t curNow;
    int32_t remainEnergy;
    int8_t present;
    std::string technology;
};

bool BatteryInfoBlockMarshalling(OHOS::MessageParcel &data, const BatteryInfo& dataBlock);

bool BatteryInfoBlockUnmarshalling(OHOS::MessageParcel &data, BatteryInfo& dataBlock);
} // V1_0
} // Battery
} // HDI
} // OHOS

#endif // OHOS_HDI_BATTERY_V1_0_TYPES_H
