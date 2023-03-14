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

#ifndef BATTERY_VIBRATE_H
#define BATTERY_VIBRATE_H

#include <fstream>

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class BatteryVibrate {
public:
    bool InitVibration();
    void HandleVibration(int32_t time) const;
private:
    static void HandlePlayMode(int32_t time);
    static void HandleDurationMode(int32_t time);
    static void SetPlayMode(const std::string& modeName);
    static void ActivateVibration(bool isActive);
    static void SetDuration(int32_t time);
    int32_t vibrateMode_ = -1;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif
