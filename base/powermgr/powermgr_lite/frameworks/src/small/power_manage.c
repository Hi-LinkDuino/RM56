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

#include <ohos_errno.h>
#include <ohos_types.h>
#include <pthread.h>
#include <registry.h>
#include <samgr_lite.h>
#include <securec.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "power_manage_interface.h"
#include "power_mgr.h"

#define MAX_DATA_LEN    1024

typedef struct {
    INHERIT_IUNKNOWNENTRY(PowerManageProxyInterface);
} PowerManageProxyEntry;

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static PowerManageProxyInterface *g_intf = NULL;

static int32_t AcquireRunningLockEntryProxy(IUnknown *iUnknown, RunningLockEntry *entry, int32_t timeoutMs);
static int32_t ReleaseRunningLockEntryProxy(IUnknown *iUnknown, RunningLockEntry *entry);
static BOOL IsAnyRunningLockHoldingProxy(IUnknown *iUnknown);
static void SuspendDeviceProxy(IUnknown *iUnknown, SuspendDeviceType reason, BOOL suspendImmed);
static void WakeupDeviceProxy(IUnknown *iUnknown, WakeupDeviceType reason, const char* details);

static void *CreatClient(const char *service, const char *feature, uint32_t size)
{
    (void)service;
    (void)feature;
    uint32_t len = size + sizeof(PowerManageProxyEntry);
    uint8_t *client = (uint8_t *)malloc(len);
    if (client == NULL) {
        POWER_HILOGE("Failed to allocate memory for power manage proxy entry");
        return NULL;
    }
    (void)memset_s(client, len, 0, len);
    PowerManageProxyEntry *entry = (PowerManageProxyEntry *)&client[size];
    entry->ver =  ((uint16)CLIENT_PROXY_VER | (uint16)DEFAULT_VERSION);
    entry->ref = 1;
    entry->iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    entry->iUnknown.AddRef = IUNKNOWN_AddRef;
    entry->iUnknown.Release = IUNKNOWN_Release;
    entry->iUnknown.Invoke = NULL;
    entry->iUnknown.AcquireRunningLockEntryFunc = AcquireRunningLockEntryProxy;
    entry->iUnknown.ReleaseRunningLockEntryFunc = ReleaseRunningLockEntryProxy;
    entry->iUnknown.IsAnyRunningLockHoldingFunc = IsAnyRunningLockHoldingProxy;
    entry->iUnknown.SuspendDeviceFunc = SuspendDeviceProxy;
    entry->iUnknown.WakeupDeviceFunc = WakeupDeviceProxy;
    return client;
}

static void DestroyClient(const char *service, const char *feature, void *iproxy)
{
    (void)service;
    (void)feature;
    free(iproxy);
}

static PowerManageProxyInterface *GetPowerManageProxyInterface(void)
{
    if (g_intf != NULL) {
        return g_intf;
    }
    pthread_mutex_lock(&g_mutex);
    if (g_intf != NULL) {
        pthread_mutex_unlock(&g_mutex);
        return g_intf;
    }

    SAMGR_RegisterFactory(POWER_MANAGE_SERVICE, POWER_MANAGE_FEATURE, CreatClient, DestroyClient);

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(POWER_MANAGE_SERVICE, POWER_MANAGE_FEATURE);
    if (iUnknown == NULL) {
        POWER_HILOGE("Failed to get power manage iUnknown");
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }

    int ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&g_intf);
    if ((ret != EC_SUCCESS) || (g_intf == NULL)) {
        POWER_HILOGE("Failed to query power manage interface");
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }
    pthread_mutex_unlock(&g_mutex);
    POWER_HILOGI("Succeed to get power manage proxy interface");
    return g_intf;
}

static int32_t AcquireReleaseCallback(IOwner owner, int32_t code, IpcIo *reply)
{
    if ((reply == NULL) || (owner == NULL)) {
        POWER_HILOGE("Invalid parameter");
        return EC_INVALID;
    }

    int32_t *ret = (int32_t *)owner;
    *ret = IpcIoPopInt32(reply);
    POWER_HILOGD("Running lock callback: %d, code: %d", *ret, code);
    return EC_SUCCESS;
}

static int32_t AcquireRunningLockEntryProxy(IUnknown *iUnknown, RunningLockEntry *entry, int32_t timeoutMs)
{
    if (IsValidRunningLockEntry(entry) == FALSE) {
        POWER_HILOGE("Invalid running lock entry");
        return EC_INVALID;
    }

    IpcIo request;
    char buffer[MAX_DATA_LEN];
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    IpcIoPushFlatObj(&request, entry, sizeof(RunningLockEntry));
    IpcIoPushInt32(&request, timeoutMs);

    int32_t ret;
    PowerManageProxyInterface *proxy = (PowerManageProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, POWERMANAGE_FUNCID_ACQUIRERUNNINGLOCK, &request, &ret, AcquireReleaseCallback);
    POWER_HILOGD("Acquire running lock done, name: %s, type: %d", entry->lock.name, entry->lock.type);

    return ret;
}

static int32_t ReleaseRunningLockEntryProxy(IUnknown *iUnknown, RunningLockEntry *entry)
{
    if (IsValidRunningLockEntry(entry) == FALSE) {
        POWER_HILOGE("Invalid running lock entry");
        return EC_INVALID;
    }

    IpcIo request;
    char buffer[MAX_DATA_LEN];
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    IpcIoPushFlatObj(&request, entry, sizeof(RunningLockEntry));

    int32_t ret;
    PowerManageProxyInterface *proxy = (PowerManageProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, POWERMANAGE_FUNCID_RELEASERUNNINGLOCK, &request, &ret, AcquireReleaseCallback);
    POWER_HILOGD("Release running lock done, name: %s, type: %d", entry->lock.name, entry->lock.type);

    return ret;
}

static int32_t IsAnyHoldingCallback(IOwner owner, int32_t code, IpcIo *reply)
{
    if ((reply == NULL) || (owner == NULL)) {
        POWER_HILOGE("Invalid parameter");
        return EC_INVALID;
    }

    BOOL *ret = (BOOL *)owner;
    *ret = IpcIoPopBool(reply) ? TRUE : FALSE;
    POWER_HILOGD("Any running lock holding: %d, code: %d", *ret, code);
    return EC_SUCCESS;
}

static BOOL IsAnyRunningLockHoldingProxy(IUnknown *iUnknown)
{
    BOOL ret;
    PowerManageProxyInterface *proxy = (PowerManageProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, POWERMANAGE_FUNCID_ISANYRUNNINGLOCKHOLDING, NULL, &ret, IsAnyHoldingCallback);
    return ret;
}

void InitIdentity(RunningLockEntry *entry)
{
    if (entry == NULL) {
        return;
    }
    entry->identity.pid = getpid();
    entry->identity.token = (uint64_t)(uintptr_t)entry;
}

BOOL AcquireRunningLockEntry(RunningLockEntry *entry, int32_t timeoutMs)
{
    int32_t ret = EC_FAILURE;
    PowerManageProxyInterface *intf = GetPowerManageProxyInterface();
    if ((intf != NULL) && (intf->AcquireRunningLockEntryFunc != NULL)) {
        ret = intf->AcquireRunningLockEntryFunc((IUnknown *)intf, entry, timeoutMs);
    }
    return (ret == EC_SUCCESS) ? TRUE : FALSE;
}

BOOL ReleaseRunningLockEntry(RunningLockEntry *entry)
{
    int32_t ret = EC_FAILURE;
    PowerManageProxyInterface *intf = GetPowerManageProxyInterface();
    if ((intf != NULL) && (intf->ReleaseRunningLockEntryFunc != NULL)) {
        ret = intf->ReleaseRunningLockEntryFunc((IUnknown *)intf, entry);
    }
    return (ret == EC_SUCCESS) ? TRUE : FALSE;
}

BOOL IsAnyRunningLockHolding()
{
    BOOL ret = FALSE;
    PowerManageProxyInterface *intf = GetPowerManageProxyInterface();
    if ((intf != NULL) && (intf->IsAnyRunningLockHoldingFunc != NULL)) {
        ret = intf->IsAnyRunningLockHoldingFunc((IUnknown *)intf);
    }
    return ret;
}

static void SuspendDeviceProxy(IUnknown *iUnknown, SuspendDeviceType reason, BOOL suspendImmed)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN];
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    IpcIoPushInt32(&request, reason);
    IpcIoPushBool(&request, (suspendImmed == TRUE));

    PowerManageProxyInterface *proxy = (PowerManageProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, POWERMANAGE_FUNCID_SUSPEND, &request, NULL, NULL);
    POWER_HILOGD("Suspend device done, reason: %{public}d", reason);
}

static void WakeupDeviceProxy(IUnknown *iUnknown, WakeupDeviceType reason, const char* details)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN];
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    IpcIoPushInt32(&request, reason);
    IpcIoPushString(&request, details);

    PowerManageProxyInterface *proxy = (PowerManageProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, POWERMANAGE_FUNCID_WAKEUP, &request, NULL, NULL);
    POWER_HILOGD("Wakeup device done, reason: %{public}d", reason);
}

void SuspendDevice(SuspendDeviceType reason, BOOL suspendImmed)
{
    PowerManageProxyInterface *intf = GetPowerManageProxyInterface();
    if ((intf != NULL) && (intf->SuspendDeviceFunc != NULL)) {
        intf->SuspendDeviceFunc((IUnknown *)intf, reason, suspendImmed);
    }
}

void WakeupDevice(WakeupDeviceType reason, const char* details)
{
    const char* detailReason = (details != NULL) ? details : "No details";
    PowerManageProxyInterface *intf = GetPowerManageProxyInterface();
    if ((intf != NULL) && (intf->WakeupDeviceFunc != NULL)) {
        intf->WakeupDeviceFunc((IUnknown *)intf, reason, detailReason);
    }
}
