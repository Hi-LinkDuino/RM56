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

#ifndef BATTERY_LED_H
#define BATTERY_LED_H

#include "battery_config.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class BatteryLed {
public:
    int32_t InitLedsSysfs();
    static void TurnOffLed();
    static void WriteLedInfoToSys(int32_t  redBrightness, int32_t  greenBrightness, int32_t  blueBrightness);
    void UpdateLedColor(int32_t chargeState, int32_t capacity);
private:
    static void InitMockLedFile(std::string& redPath, std::string& greenPath, std::string& bluePath) ;
    static void TraversalNode();
    static void CreateMockLedPath(const std::string& mockPath) ;
    static void CreateMockLedFile(const std::string& path, const std::string& content) ;
    static void WriteLedNode(std::string& ledPath, int32_t brightness);
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif
