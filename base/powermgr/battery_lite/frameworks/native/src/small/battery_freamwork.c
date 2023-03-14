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

#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>
#include <registry.h>
#include <samgr_lite.h>
#include <securec.h>

#include "battery_framework.h"
#include "battery_info.h"
#include "battery_interface.h"
#include "ibattery.h"

#define MAX_DATA_LEN    1024

typedef struct {
    INHERIT_IUNKNOWNENTRY(BatteryProxyInterface);
} BatteryProxyEntry;

static int32_t BatteryCallbackInt(IOwner owner, int32_t code, IpcIo *reply)
{
    (void)code;
    if ((reply == NULL) || (owner == NULL)) {
        return EC_INVALID;
    }

    int32_t *ret = (int32_t *)owner;
    *ret = IpcIoPopInt32(reply);
    return EC_SUCCESS;
}

static int32_t GetBatSocProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    int32_t ret = 0;

    if (iUnknown == NULL) {
        return EC_INVALID;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETSOC, &request, &ret, BatteryCallbackInt);

    return ret;
}
static BatteryChargeState GetChargingStatusProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    int32_t ret = 0;

    if (iUnknown == NULL) {
        return EC_INVALID;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETCHARGING, &request, &ret, BatteryCallbackInt);

    return ret;
}
static BatteryHealthState GetHealthStatusProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    int32_t ret = 0;

    if (iUnknown == NULL) {
        return EC_INVALID;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETHEALTH, &request, &ret, BatteryCallbackInt);

    return ret;
}
static BatteryPluggedType GetPluggedTypeProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    int32_t ret = 0;

    if (iUnknown == NULL) {
        return EC_INVALID;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETPLUGTYPE, &request, &ret, BatteryCallbackInt);

    return ret;
}
static int32_t GetBatVoltageProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    int32_t ret = 0;

    if (iUnknown == NULL) {
        return EC_INVALID;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETVOLTAGE, &request, &ret, BatteryCallbackInt);

    return ret;
}

static int32_t BatteryCallbackBuff(IOwner owner, int32_t code, IpcIo *reply)
{
    size_t len = 0;
    (void)code;
    if ((reply == NULL) || (owner == NULL)) {
        return EC_INVALID;
    }

    char **strBuff = (char **)owner;
    *strBuff = (char *)IpcIoPopString(reply, &len);
    if (*strBuff == NULL || len == 0) {
        return EC_INVALID;
    }

    return EC_SUCCESS;
}

static char *GetBatTechnologyProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    char *string = NULL;

    if (iUnknown == NULL) {
        return string;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETTECHNOLONY, &request, &string, BatteryCallbackBuff);
    return string;
}

static int32_t GetBatTemperatureProxy(IUnknown *iUnknown)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN] = { 0 };
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    int32_t ret = 0;

    if (iUnknown == NULL) {
        return EC_INVALID;
    }

    BatteryProxyInterface *proxy = (BatteryProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, BATTERY_FUNCID_GETTEMPERATURE, &request, &ret, BatteryCallbackInt);

    return ret;
}

static void *CreatClient(const char *service, const char *feature, uint32_t size)
{
    (void)service;
    (void)feature;
    uint32_t len = size + sizeof(BatteryProxyEntry);
    uint8_t *client = (uint8_t *)malloc(len);
    if (client == NULL) {
        return NULL;
    }

    (void)memset_s(client, len, 0, len);
    BatteryProxyEntry *entry = (BatteryProxyEntry *)&client[size];
    entry->ver =  ((uint16)CLIENT_PROXY_VER | (uint16)DEFAULT_VERSION);
    entry->ref = 1;
    entry->iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    entry->iUnknown.AddRef = IUNKNOWN_AddRef;
    entry->iUnknown.Release = IUNKNOWN_Release;
    entry->iUnknown.Invoke = NULL;
    entry->iUnknown.GetBatSocFunc = GetBatSocProxy;
    entry->iUnknown.GetChargingStatusFunc = GetChargingStatusProxy;
    entry->iUnknown.GetHealthStatusFunc = GetHealthStatusProxy;
    entry->iUnknown.GetPluggedTypeFunc = GetPluggedTypeProxy;
    entry->iUnknown.GetBatVoltageFunc = GetBatVoltageProxy;
    entry->iUnknown.GetBatTechnologyFunc = GetBatTechnologyProxy;
    entry->iUnknown.GetBatTemperatureFunc = GetBatTemperatureProxy;
    return client;
}

static void DestroyClient(const char *service, const char *feature, void *iproxy)
{
    (void)service;
    (void)feature;
    if (iproxy != NULL) {
        free(iproxy);
    }
}

static BatteryProxyInterface *GetBatteryInterface(void)
{
    static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
    static BatteryProxyInterface *g_intf = NULL;

    pthread_mutex_lock(&g_mutex);
    if (g_intf != NULL) {
        pthread_mutex_unlock(&g_mutex);
        return g_intf;
    }
    SAMGR_RegisterFactory(BATTERY_SERVICE, BATTERY_INNER, CreatClient, DestroyClient);

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
    BatteryProxyInterface *intf = GetBatteryInterface();

    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        ret = intf->GetBatSocFunc((IUnknown *)intf);
    }
    return ret;
}

BatteryChargeState GetChargingStatus(void)
{
    BatteryChargeState state = CHARGE_STATE_NONE;
    BatteryProxyInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        state = intf->GetChargingStatusFunc((IUnknown *)intf);
    }
    return state;
}

BatteryHealthState GetHealthStatus(void)
{
    BatteryHealthState state = HEALTH_STATE_UNKNOWN;
    BatteryProxyInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        state = intf->GetHealthStatusFunc((IUnknown *)intf);
    }
    return state;
}

BatteryPluggedType GetPluggedType(void)
{
    BatteryPluggedType state = PLUGGED_TYPE_NONE;
    BatteryProxyInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        state = intf->GetPluggedTypeFunc((IUnknown *)intf);
    }
    return state;
}

int32_t GetBatVoltage(void)
{
    int32_t ret = EC_FAILURE;
    BatteryProxyInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        ret = intf->GetBatVoltageFunc((IUnknown *)intf);
    }
    return ret;
}

char *GetBatTechnology(void)
{
    char *strBuff = NULL;
    BatteryProxyInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        strBuff = intf->GetBatTechnologyFunc((IUnknown *)intf);
    }
    return strBuff;
}

int32_t GetBatTemperature(void)
{
    int32_t ret = EC_FAILURE;
    BatteryProxyInterface *intf = GetBatteryInterface();
    if ((intf != NULL) && (intf->GetBatSocFunc != NULL)) {
        ret = intf->GetBatTemperatureFunc((IUnknown *)intf);
    }
    return ret;
}
