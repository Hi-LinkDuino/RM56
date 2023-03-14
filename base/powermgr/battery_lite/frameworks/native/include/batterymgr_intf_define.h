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
 
#ifndef BATTERY_INTF_DEFINE_H
#define BATTERY_INTF_DEFINE_H

#include <iunknown.h>
#include "battery_info.h"
 
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define INHERIT_BATTERY_INTERFACE                                                  \
    int32_t (*GetBatSocFunc)(IUnknown *iUnknown);                                  \
    BatteryChargeState (*GetChargingStatusFunc)(IUnknown *iUnknown);               \
    BatteryHealthState (*GetHealthStatusFunc)(IUnknown *iUnknown);                 \
    BatteryPluggedType (*GetPluggedTypeFunc)(IUnknown *iUnknown);                  \
    int32_t (*GetBatVoltageFunc)(IUnknown *iUnknown);                              \
    char* (*GetBatTechnologyFunc)(IUnknown *iUnknown);                             \
    int32_t (*GetBatTemperatureFunc)(IUnknown *iUnknown)

#ifdef __cplusplus
}
#endif // __cplusplus 
#endif // BATTERYMGR_INTF_DEFINE_H