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

#include <stdint.h>
#include <stdlib.h>

#include <ohos_errno.h>
#include <pthread.h>
#include <registry.h>
#include <samgr_lite.h>
#include <securec.h>
#include <unistd.h>

#include "hilog_wrapper.h"
#include "power_mgr.h"
#include "screen_saver_interface.h"

#define MAX_DATA_LEN    1024

typedef struct {
    INHERIT_IUNKNOWNENTRY(ScreenSaverProxyInterface);
} ScreenSaverProxyEntry;

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static ScreenSaverProxyInterface *g_intf = NULL;

static int32_t SetScreenSaverStateProxy(IUnknown *iUnknown, BOOL enable);

static void *CreatClient(const char *service, const char *feature, uint32_t size)
{
    (void)service;
    (void)feature;
    uint32_t len = size + sizeof(ScreenSaverProxyEntry);
    uint8_t *client = (uint8_t *)malloc(len);
    if (client == NULL) {
        POWER_HILOGE("Failed to allocate memory for screen saver proxy entry");
        return NULL;
    }
    (void)memset_s(client, len, 0, len);
    ScreenSaverProxyEntry *entry = (ScreenSaverProxyEntry *)&client[size];
    entry->ver =  ((uint16)CLIENT_PROXY_VER | (uint16)DEFAULT_VERSION);
    entry->ref = 1;
    entry->iUnknown.QueryInterface = IUNKNOWN_QueryInterface;
    entry->iUnknown.AddRef = IUNKNOWN_AddRef;
    entry->iUnknown.Release = IUNKNOWN_Release;
    entry->iUnknown.Invoke = NULL;
    entry->iUnknown.SetScreenSaverStateFunc = SetScreenSaverStateProxy;
    return client;
}

static void DestroyClient(const char *service, const char *feature, void *iproxy)
{
    free(iproxy);
}

static ScreenSaverProxyInterface *GetScreenSaverProxyInterface(void)
{
    if (g_intf != NULL) {
        return g_intf;
    }
    pthread_mutex_lock(&g_mutex);
    if (g_intf != NULL) {
        pthread_mutex_unlock(&g_mutex);
        return g_intf;
    }

    SAMGR_RegisterFactory(POWER_MANAGE_SERVICE, POWER_SCREEN_SAVER_FEATURE, CreatClient, DestroyClient);

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(POWER_MANAGE_SERVICE, POWER_SCREEN_SAVER_FEATURE);
    if (iUnknown == NULL) {
        POWER_HILOGE("Failed to get screen saver iUnknown");
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }

    int ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&g_intf);
    if ((ret != EC_SUCCESS) || (g_intf == NULL)) {
        POWER_HILOGE("Failed to query screen saver interface");
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }
    pthread_mutex_unlock(&g_mutex);
    POWER_HILOGI("Succeed to get screen saver proxy interface");
    return g_intf;
}

static int32_t Callback(IOwner owner, int32_t code, IpcIo *reply)
{
    if ((reply == NULL) || (owner == NULL)) {
        POWER_HILOGE("Invalid parameter");
        return EC_INVALID;
    }

    int32_t *ret = (int32_t *)owner;
    *ret = IpcIoPopInt32(reply);
    POWER_HILOGD("Power screen saver callback: %d, code: %d", *ret, code);
    return EC_SUCCESS;
}

static int32_t SetScreenSaverStateProxy(IUnknown *iUnknown, BOOL enable)
{
    IpcIo request;
    char buffer[MAX_DATA_LEN];
    IpcIoInit(&request, buffer, MAX_DATA_LEN, 0);
    IpcIoPushBool(&request, enable == TRUE);

    int32_t ret;
    ScreenSaverProxyInterface *proxy = (ScreenSaverProxyInterface *)iUnknown;
    proxy->Invoke((IClientProxy *)proxy, SCREENSAVER_FUNCID_SETSTATE, &request, &ret, Callback);
    POWER_HILOGD("Set screen saver state done, enable: %d", (int32_t)enable);

    return ret;
}

BOOL SetScreenSaverState(BOOL enable)
{
    int32_t ret = EC_FAILURE;
    ScreenSaverProxyInterface *intf = GetScreenSaverProxyInterface();
    if ((intf != NULL) && (intf->SetScreenSaverStateFunc != NULL)) {
        ret = intf->SetScreenSaverStateFunc((IUnknown *)intf, enable);
    }
    return (ret == EC_SUCCESS) ? TRUE : FALSE;
}
