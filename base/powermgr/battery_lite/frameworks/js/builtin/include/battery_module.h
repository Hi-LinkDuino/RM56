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

#ifndef BATTERY_MODULE_H
#define BATTERY_MODULE_H

#include <stdint.h>
#include "jsi/jsi.h"

namespace OHOS {
namespace ACELite {
class BatteryModule {
public:
    BatteryModule() = default;
    ~BatteryModule() = default;
    static JSIValue GetBatterySOC(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetChargingStatus(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetHealthStatus(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetPluggedType(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetVoltage(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetTechnology(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
    static JSIValue GetTemperature(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);
};
}
}

#endif // BATTERY_MODULE_H