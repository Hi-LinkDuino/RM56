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

#include "battery_framework.h"
#include "battery_info.h"
#include "battery_interface.h"
#include "ibattery.h"


static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static BatteryInterface *g_intf = NULL;

static BatteryInterface *GetBatteryInterface(void)
{
    pthread_mutex_lock(&g_mutex);
    if (g_intf != NULL) {
        pthread_mutex_unlock(&g_mutex);
        return g_intf;
    }
    IUnknown *iUnknown = GetBatteryIUnknown();
    if (iUnknown == NULL) {
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }

    int ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&g_intf);
    if ((ret != EC_SUCCESS) || (g_intf == NULL)) {
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }
    pthread_mutex_unlock(&g_mutex);
    return g_intf;
}

int32_t GetBatSoc(void)
{
    int32_t ret = EC_FAILURE;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        ret = intf->GetBatSocFunc((IUnknown *)intf);
    }
    return ret;
}

BatteryChargeState GetChargingStatus(void)
{
    BatteryChargeState state = CHARGE_STATE_NONE;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        state = intf->GetChargingStatusFunc((IUnknown *)intf);
    }
    return state;
}

BatteryHealthState GetHealthStatus(void)
{
    BatteryHealthState state = HEALTH_STATE_UNKNOWN;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        state = intf->GetHealthStatusFunc((IUnknown *)intf);
    }
    return state;
}

BatteryPluggedType GetPluggedType(void)
{
    BatteryPluggedType state = PLUGGED_TYPE_NONE;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        state = intf->GetPluggedTypeFunc((IUnknown *)intf);
    }
    return state;
}

int32_t GetBatVoltage(void)
{
    int32_t ret = EC_FAILURE;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        ret = intf->GetBatVoltageFunc((IUnknown *)intf);
    }
    return ret;
}

char *GetBatTechnology(void)
{
    char *strBuff = NULL;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        strBuff = intf->GetBatTechnologyFunc((IUnknown *)intf);
    }
    return strBuff;
}

int32_t GetBatTemperature(void)
{
    int32_t ret = EC_FAILURE;
    BatteryInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        ret = intf->GetBatTemperatureFunc((IUnknown *)intf);
    }
    return ret;
}
