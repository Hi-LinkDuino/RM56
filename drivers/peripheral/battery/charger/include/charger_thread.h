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

#ifndef CHARGER_THREAD_H
#define CHARGER_THREAD_H

#include <thread>

#include <linux/input.h>
#include "input_type.h"
#include "battery_thread.h"
#include "battery_config.h"
#include "battery_vibrate.h"
#include "battery_backlight.h"
#include "battery_led.h"
#include "animation_label.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
class ChargerThread : public BatteryThread {
public:
    friend class BatteryThreadTest;
private:
    void Init();
    static void UpdateAnimation(const int32_t& capacity);
    void Run(void* service) override;
    void UpdateBatteryInfo(void* arg, char* msg) override;
    void UpdateBatteryInfo(void* arg) override;
    void HandleChargingState();
    void HandleScreenState();
    void HandleTemperature(const int32_t& temperature);
    void HandleCapacity(const int32_t& capacity);
    void HandleStates() override;
    int32_t UpdateWaitInterval() override;
    void CycleMatters() override;
    static void AnimationInit();
    static void LoadImages(AnimationLabel* animationLabel);
    void SetKeyWait(struct KeyState& key, int64_t timeout);
    static void SetKeyState(int32_t code, int32_t value, int64_t now);
    static void InitInput();
    static void EventPkgCallback(const EventPackage** pkgs, uint32_t count, uint32_t devIndex);
    void HandlePowerKeyState();
    void HandlePowerKey(int32_t keycode, int64_t now);
    static void HandleInputEvent(const struct input_event* iev);
    std::unique_ptr<BatteryConfig> batteryConfig_ = nullptr;
    std::unique_ptr<PowerSupplyProvider> provider_ = nullptr;
    std::unique_ptr<BatteryVibrate> vibrate_ = nullptr;
    std::unique_ptr<BatteryBacklight> backlight_ = nullptr;
    std::unique_ptr<BatteryLed> led_ = nullptr;

    static const int32_t INVALID = -1;
    int64_t pluginWait_ = INVALID;
    int64_t keyWait_ = INVALID;
    int64_t backlightWait_ = INVALID;
    int32_t capacity_ = INVALID;
    int32_t chargeState_ = PowerSupplyProvider::BatteryChargeState::CHARGE_STATE_RESERVED;
    bool started_ = false;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS
#endif
