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

#ifndef BATTERY_MANAGE_FEATURE_H
#define BATTERY_MANAGE_FEATURE_H

#include "feature.h"
#include "message.h"
#include "service.h"    
#include "ibattery.h"
#include "battery_info.h"
#include "battery_mgr.h"
#include "battery_interface.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static IBattery *g_batteryDevice = NULL;

typedef struct {
    INHERIT_FEATURE;
    INHERIT_IUNKNOWNENTRY(BatteryInterface);
    Identity identity_;
} BatteryFeatureApi;


const char *GetFeatureName(Feature *feature);
void OnFeatureStop(Feature *feature, Identity identity);
BOOL OnFeatureMessage(Feature *feature, Request *request);
void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity);

int32_t BatterySocImpl(IUnknown *iUnknown);
BatteryChargeState ChargingStatusImpl(IUnknown *iUnknown);
BatteryHealthState HealthStatusImpl(IUnknown *iUnknown);
BatteryPluggedType PluggedTypeImpl(IUnknown *iUnknown);
int32_t VoltageImpl(IUnknown *iUnknown);
char* TechnologyImpl(IUnknown *iUnknown);

int32_t BatteryTemperatureImpl(IUnknown *iUnknown);

#define BATTERY_FEATURE_INTERFACE_IMPL                              \
    .GetName = GetFeatureName,                                      \
    .OnInitialize = OnFeatureInitialize,                            \
    .OnStop = OnFeatureStop,                                        \
    .OnMessage = OnFeatureMessage

#define BATTERY_INTERFACE_IMPL                                      \
    .GetBatSocFunc = BatterySocImpl,                                \
    .GetChargingStatusFunc = ChargingStatusImpl,                    \
    .GetHealthStatusFunc = HealthStatusImpl,                        \
    .GetPluggedTypeFunc = PluggedTypeImpl,                          \
    .GetBatVoltageFunc = VoltageImpl,                               \
    .GetBatTechnologyFunc = TechnologyImpl,                         \
    .GetBatTemperatureFunc = BatteryTemperatureImpl

BatteryFeatureApi *GetBatteryFeatureImpl(void);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // BATTERY_MANAGE_FEATURE_H