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

#include <feature.h>
#include <samgr_lite.h>
#include "ibattery.h"
#include "battery_manage_feature.h"
#include "battery_device.h"

static const int BATT_INT_VALUE = 0;

const char *GetFeatureName(Feature *feature)
{
    (void)feature;
    return BATTERY_INNER;
}

void OnFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    (void)feature;
    (void)identity;
}

void OnFeatureStop(Feature *feature, Identity identity)
{
    (void)feature;
    (void)identity;
}

BOOL OnFeatureMessage(Feature *feature, Request *request)
{
    return ((feature != NULL) && (request != NULL)) ? TRUE : FALSE;
}

int32_t BatterySocImpl(IUnknown *iUnknown)
{
    int32_t soc = BATT_INT_VALUE;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return soc;
    }
    soc = g_batteryDevice->GetSoc();
    return soc;
}
BatteryChargeState ChargingStatusImpl(IUnknown *iUnknown)
{
    BatteryChargeState chargingstate = CHARGE_STATE_BUTT;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return chargingstate;
    }
    chargingstate = g_batteryDevice->GetChargingStatus();
    return chargingstate;
}
BatteryHealthState HealthStatusImpl(IUnknown *iUnknown)
{
    BatteryHealthState healthState = HEALTH_STATE_BUTT;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return healthState;
    }
    healthState = g_batteryDevice->GetHealthStatus();
    return healthState;
}
BatteryPluggedType PluggedTypeImpl(IUnknown *iUnknown)
{
    BatteryPluggedType pluggedType = PLUGGED_TYPE_BUTT;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return pluggedType;
    }
    pluggedType = g_batteryDevice->GetPluggedType();
    return pluggedType;
}
int32_t VoltageImpl(IUnknown *iUnknown)
{
    int32_t voltage = BATT_INT_VALUE;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return voltage;
    }
    voltage = g_batteryDevice->GetVoltage();
    return voltage;
}
char* TechnologyImpl(IUnknown *iUnknown)
{
    char* technology = NULL;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return technology;
    }
    technology = g_batteryDevice->GetTechnology();
    return technology;
}
int32_t BatteryTemperatureImpl(IUnknown *iUnknown)
{
    int32_t temperature = BATT_INT_VALUE;
    g_batteryDevice = NewBatterInterfaceInstance();
    if (g_batteryDevice == NULL) {
        return temperature;
    }
    temperature = g_batteryDevice->GetTemperature();
    return temperature;
}

static void GInit()
{
    BatteryFeatureApi *feature = GetBatteryFeatureImpl();
    if (feature == NULL) {
        return;
    }

    BOOL result = SAMGR_GetInstance()->RegisterFeature(BATTERY_SERVICE, (Feature *)feature);
    if (result == FALSE) {
        return;
    }
    BOOL apiResult = SAMGR_GetInstance()->RegisterFeatureApi(BATTERY_SERVICE, BATTERY_INNER, GET_IUNKNOWN(*feature));
    if (apiResult == FALSE) {
        return;
    }
}
SYSEX_FEATURE_INIT(GInit);