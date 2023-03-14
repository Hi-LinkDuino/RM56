/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines remote device battery level observer.
 *
 */

/**
 * @file classic_battery_observer.h
 *
 * @brief Classic remote device battery level observer functions.
 *
 */

#ifndef CLASSIC_BATTERY_OBSERVER
#define CLASSIC_BATTERY_OBSERVER

#include "base_def.h"
#include "dispatcher.h"
#include "interface_profile_hfp_ag.h"
#include "interface_profile_hfp_hf.h"

namespace OHOS {
namespace bluetooth {
class ClassicBatteryObserverHf : public HfpHfServiceObserver {
public:
    explicit ClassicBatteryObserverHf(utility::Dispatcher &dispatcher);
    ~ClassicBatteryObserverHf();
    void OnBatteryLevelChanged(const RawAddress &device, int batteryLevel);
    void SetBatteryLevel(const RawAddress &device, int batteryLevel) const;

private:
    utility::Dispatcher *dispatcher_ = nullptr;
    BT_DISALLOW_COPY_AND_ASSIGN(ClassicBatteryObserverHf);
};

class ClassicBatteryObserverAg : public HfpAgServiceObserver {
public:
    explicit ClassicBatteryObserverAg(utility::Dispatcher &dispatcher);
    ~ClassicBatteryObserverAg();
    void OnHfBatteryLevelChanged(const RawAddress &device, int indValue);
    void SetBatteryLevel(const RawAddress &device, int batteryLevel) const;

private:
    utility::Dispatcher *dispatcher_ = nullptr;

    BT_DISALLOW_COPY_AND_ASSIGN(ClassicBatteryObserverAg);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // CLASSIC_BATTERY_OBSERVER