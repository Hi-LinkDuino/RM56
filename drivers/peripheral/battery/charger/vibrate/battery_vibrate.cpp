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

#include "battery_vibrate.h"

#include <unistd.h>
#include "file_ex.h"
#include "battery_log.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
namespace {
const std::string VIBRATOR_PLAYMODE_PATH = "/sys/class/leds/vibrator/play_mode";
const std::string VIBRATOR_DURATIONMODE_PATH = "/sys/class/leds/vibrator/duration";
const std::string VIBRATOR_ACTIVATEMODE_PATH = "/sys/class/leds/vibrator/activate";
const std::string DURATION_MODE_DERECT = "direct";
const std::string DURATION_MODE_AUDIO = "audio";
constexpr int32_t VIBRATION_PLAYMODE = 0;
constexpr int32_t VIBRATION_DURATIONMODE = 1;
constexpr int32_t ACTIVE = 1;
constexpr int32_t DEACTIVE = 0;
constexpr int32_t VIBRATE_DELAY_MS = 5;
constexpr int32_t USEC_TO_MSEC = 1000;
}

bool BatteryVibrate::InitVibration()
{
    BATTERY_HILOGD(FEATURE_CHARGING, "start init vibrate");

    if (FileExists(VIBRATOR_PLAYMODE_PATH)) {
        BATTERY_HILOGI(FEATURE_CHARGING, "vibrate path is play mode path");
        vibrateMode_ = VIBRATION_PLAYMODE;
        return true;
    }

    if (FileExists(VIBRATOR_DURATIONMODE_PATH)) {
        BATTERY_HILOGI(FEATURE_CHARGING, "vibrate path is duration path");
        vibrateMode_ = VIBRATION_DURATIONMODE;
        return true;
    }

    BATTERY_HILOGW(FEATURE_CHARGING, "not support vibrate path");
    return false;
}

void BatteryVibrate::HandlePlayMode(int32_t time)
{
    SetPlayMode(DURATION_MODE_DERECT);
    SetDuration(time);
    ActivateVibration(true);
    usleep((time + VIBRATE_DELAY_MS) * USEC_TO_MSEC);
    SetPlayMode(DURATION_MODE_AUDIO);
}

void BatteryVibrate::HandleDurationMode(int32_t time)
{
    SetDuration(time);
    ActivateVibration(true);
    usleep((time + VIBRATE_DELAY_MS) * USEC_TO_MSEC);
    ActivateVibration(false);
}

void BatteryVibrate::HandleVibration(int32_t time) const
{
    switch (vibrateMode_) {
        case VIBRATION_PLAYMODE: {
            BATTERY_HILOGD(FEATURE_CHARGING, "vibrate play mode");
            HandlePlayMode(time);
            break;
        }
        case VIBRATION_DURATIONMODE: {
            BATTERY_HILOGD(FEATURE_CHARGING, "vibrate duration mode");
            HandleDurationMode(time);
            break;
        }
        default: {
            BATTERY_HILOGD(FEATURE_CHARGING, "vibrate unknown mode");
            break;
        }
    }
}

void BatteryVibrate::SetPlayMode(const std::string& modeName)
{
    FILE* file = fopen(VIBRATOR_PLAYMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "play mode path open failed.");
        return;
    }
    if (fprintf(file, "%s\n", modeName.c_str()) < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "play mode fprintf direct failed.");
    }
    if (fclose(file) < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "play mode fclose failed.");
    }
}

void BatteryVibrate::ActivateVibration(bool isActive)
{
    int32_t value = isActive ? ACTIVE : DEACTIVE;
    FILE* file = fopen(VIBRATOR_ACTIVATEMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "activate mode path open failed");
        return;
    }
    if (fprintf(file, "%d\n", value) < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "activate mode fprintf failed, value=%{public}d", value);
    }
    if (fclose(file) < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "activate mode fclose failed");
    }
}

void BatteryVibrate::SetDuration(int32_t time)
{
    FILE* file = fopen(VIBRATOR_DURATIONMODE_PATH.c_str(), "w");
    if (file == nullptr) {
        BATTERY_HILOGW(FEATURE_CHARGING, "duration mode path open failed");
        return;
    }
    if (fprintf(file, "%d\n", time) < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "duration mode fprintf time failed, time=%{public}d", time);
    }
    if (fclose(file) < 0) {
        BATTERY_HILOGW(FEATURE_CHARGING, "duration mode fclose failed.");
    }
}
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
