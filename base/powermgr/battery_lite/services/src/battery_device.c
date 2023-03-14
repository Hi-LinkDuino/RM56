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

#include <securec.h>
#include "battery_device.h"

static BatInfo battInfo = {
    80,
    10,
    20,
    60,
    CHARGE_STATE_ENABLE,
    PLUGGED_TYPE_WIRELESS,
    "Ternary_Lithium",
    HEALTH_STATE_GOOD,
};

static BatteeryDeviceFeatureApi *g_batteryFeatureHandle = NULL;
static IBattery g_device;
static IBattery *g_ibattery = NULL;

static const char *BATTERY_GetName(Service *service)
{
    (void)service;
    return BATTERY_DEVICE;
}

static BOOL Initialize(Service *service, Identity identity)
{
    (void)service;
    (void)identity;
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *msg)
{
    (void)service;
    (void)msg;
    return TRUE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, TASK_CONFIG_STACK_SIZE, TASK_CONFIG_QUEUE_SIZE, SHARED_TASK};
    return config;
}

static int32_t Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    (void)iProxy;
    (void)funcId;
    (void)origin;
    (void)req;
    (void)reply;
    return BATTERY_OK;
}

int32_t GetSocImpl(void)
{
    return battInfo.batSoc;
}
BatteryChargeState GetChargingStatusImpl(void)
{
    return battInfo.chargingStatus;
}
BatteryHealthState GetHealthStatusImpl(void)
{
    return battInfo.healthStatus;
}
BatteryPluggedType GetPluggedTypeImpl(void)
{
    return battInfo.pluggedType;
}
int32_t GetVoltageImpl(void)
{
    return battInfo.batVoltage;
}
char *GetTechnologyImpl(void)
{
    return battInfo.BatTechnology;
}
int32_t GetTemperatureImpl(void)
{
    return battInfo.BatTemp;
}
int TurnOnLedImpl(int red, int green, int blue)
{
    (void)red;
    (void)green;
    (void)blue;
    return BATTERY_OK;
}
int TurnOffLedImpl(void)
{
    return BATTERY_OK;
}
int SetLedColorImpl(int red, int green, int blue)
{
    (void)red;
    (void)green;
    (void)blue;
    return BATTERY_OK;
}
int GetLedColorImpl(int *red, int *green, int *blue)
{
    (void)red;
    (void)green;
    (void)blue;
    return BATTERY_OK;
}
void ShutDownImpl(void)
{
}
void UpdateBatInfoImpl(BatInfo *battery)
{
    if (battery == NULL) {
        return;
    }
    if (strcpy_s(battery->BatTechnology, BATTECHNOLOGY_LEN, battInfo.BatTechnology) != EOK) {
        return;
    }
    battery->batSoc = battInfo.batSoc;
    battery->batVoltage = battInfo.batVoltage;
    battery->BatTemp = battInfo.BatTemp;
    battery->batCapacity = battInfo.batCapacity;
    battery->chargingStatus = battInfo.chargingStatus;
    battery->pluggedType = battInfo.pluggedType;
    battery->healthStatus = battInfo.healthStatus;
}

static BatteryDevice g_batteryDevice = {
    .GetName = BATTERY_GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    .GetSoc = GetSocImpl,
    .GetChargingStatus = GetChargingStatusImpl,
    .GetHealthStatus = GetHealthStatusImpl,
    .GetPluggedType =  GetPluggedTypeImpl,
    .GetVoltage = GetVoltageImpl,
    .GetTechnology = GetTechnologyImpl,
    .GetTemperature = GetTemperatureImpl,
    .TurnOnLed = TurnOnLedImpl,
    .TurnOffLed = TurnOffLedImpl,
    .SetLedColor = SetLedColorImpl,
    .GetLedColor = GetLedColorImpl,
    .ShutDown = ShutDownImpl,
    .UpdateBatInfo = UpdateBatInfoImpl,
    IPROXY_END,
};

static void ChargingApiGet(void)
{
    if (g_batteryFeatureHandle != NULL) {
        return;
    }
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(BATTERY_DEVICE);
    if (iUnknown == NULL) {
        return;
    }
    int ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)(&g_batteryFeatureHandle));
    if (ret != BATTERY_OK) {
        return;
    }
}

static void Init(void)
{
    BOOL result = SAMGR_GetInstance()->RegisterService((Service *)&g_batteryDevice);
    if (result == FALSE) {
        return;
    }
    BOOL apiResult = SAMGR_GetInstance()->RegisterDefaultFeatureApi(BATTERY_DEVICE, GET_IUNKNOWN(g_batteryDevice));
    if (apiResult == FALSE) {
        return;
    }
}
SYSEX_SERVICE_INIT(Init);


static IBattery *GetBatteryDeviceMethods(void)
{
    if (g_batteryFeatureHandle == NULL) {
        return NULL;
    }
    g_device.GetSoc = g_batteryFeatureHandle->GetSoc;
    g_device.GetChargingStatus = g_batteryFeatureHandle->GetChargingStatus;
    g_device.GetHealthStatus = g_batteryFeatureHandle->GetHealthStatus;
    g_device.GetPluggedType = g_batteryFeatureHandle->GetPluggedType;
    g_device.GetVoltage = g_batteryFeatureHandle->GetVoltage;
    g_device.GetTechnology = g_batteryFeatureHandle->GetTechnology;
    g_device.GetTemperature = g_batteryFeatureHandle->GetTemperature;
    g_device.TurnOnLed = g_batteryFeatureHandle->TurnOnLed;
    g_device.TurnOffLed = g_batteryFeatureHandle->TurnOffLed;
    g_device.SetLedColor = g_batteryFeatureHandle->SetLedColor;
    g_device.GetLedColor = g_batteryFeatureHandle->GetLedColor;
    g_device.ShutDown = g_batteryFeatureHandle->ShutDown;
    g_device.UpdateBatInfo = g_batteryFeatureHandle->UpdateBatInfo;
    return &g_device;
}

IBattery *NewBatterInterfaceInstance(void)
{
    if (g_batteryFeatureHandle == NULL) {
        ChargingApiGet();
    }

    g_ibattery = GetBatteryDeviceMethods();
    if (g_ibattery == NULL) {
        return NULL;
    }
    return g_ibattery;
}

int32_t FreeBatterInterfaceInstance(void)
{
    return 0;
}
