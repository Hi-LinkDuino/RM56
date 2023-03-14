/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
#include "remote_register.h"
#include <ohos_errno.h>
#include <ohos_init.h>
#include <liteipc_adapter.h>
#include <log.h>
#include "policy_define.h"
#include "samgr_lite.h"
#include "memory_adapter.h"
#include "thread_adapter.h"
#include "iproxy_client.h"
#include "default_client.h"

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "Samgr"
#define LOG_DOMAIN 0xD001800

#define RETRY_INTERVAL 2
#define MAX_RETRY_TIMES 10
#define ABILITY_UID_START 100
static void InitializeRegistry(void);
static RemoteRegister g_remoteRegister;
static BOOL g_isAbilityInited = FALSE;

int SAMGR_RegisterServiceApi(const char *service, const char *feature, const Identity *identity, IUnknown *iUnknown)
{
    if (service == NULL) {
        return EC_INVALID;
    }
    InitializeRegistry();
    MUTEX_Lock(g_remoteRegister.mtx);
    SaName saName = {service, feature};
    int32 token = SAMGR_AddRouter(g_remoteRegister.endpoint, &saName, identity, iUnknown);
    MUTEX_Unlock(g_remoteRegister.mtx);
    if (token < 0 || !g_remoteRegister.endpoint->running) {
        return token;
    }
    return SAMGR_ProcPolicy(g_remoteRegister.endpoint, &saName, token);
}

IUnknown *SAMGR_FindServiceApi(const char *service, const char *feature)
{
    if (service == NULL) {
        return NULL;
    }
    InitializeRegistry();
    SaName key = {service, feature};
    // the proxy already exits.
    int index = VECTOR_FindByKey(&g_remoteRegister.clients, &key);
    if (index != INVALID_INDEX) {
        return VECTOR_At(&g_remoteRegister.clients, index);
    }
    IUnknown *proxy = SAMGR_CreateIProxy(g_remoteRegister.endpoint->context, service, feature);
    if (proxy == NULL) {
        return NULL;
    }
    MUTEX_Lock(g_remoteRegister.mtx);
    index = VECTOR_FindByKey(&g_remoteRegister.clients, &key);
    if (index != INVALID_INDEX) {
        MUTEX_Unlock(g_remoteRegister.mtx);
        proxy->Release(proxy);
        return VECTOR_At(&g_remoteRegister.clients, index);
    }
    VECTOR_Add(&g_remoteRegister.clients, proxy);
    MUTEX_Unlock(g_remoteRegister.mtx);
    HILOG_INFO(HILOG_MODULE_SAMGR, "Create remote sa proxy<%s, %s>!", service, feature);
    return proxy;
}

int32 SAMGR_RegisterSystemCapabilityApi(const char *sysCap, BOOL isReg)
{
    InitializeRegistry();
    return SAMGR_AddSysCap(g_remoteRegister.endpoint, sysCap, isReg);
}

BOOL SAMGR_QuerySystemCapabilityApi(const char *sysCap)
{
    InitializeRegistry();
    BOOL isReg = FALSE;
    if (SAMGR_GetSysCap(g_remoteRegister.endpoint, sysCap, &isReg) != EC_SUCCESS) {
        return FALSE;
    }
    return isReg;
}

int32 SAMGR_GetSystemCapabilitiesApi(char sysCaps[MAX_SYSCAP_NUM][MAX_SYSCAP_NAME_LEN], int32 *size)
{
    InitializeRegistry();
    return SAMGR_GetSystemCapabilities(g_remoteRegister.endpoint, sysCaps, size);
}

static void ClearRegistry(void)
{
    if (g_remoteRegister.endpoint == NULL) {
        return;
    }
    HILOG_INFO(HILOG_MODULE_SAMGR, "Clear Client Registry!");
    SAMGR_Free(g_remoteRegister.mtx);
    g_remoteRegister.mtx = NULL;
    VECTOR_Clear(&(g_remoteRegister.clients));
    VECTOR_Clear(&(g_remoteRegister.endpoint->routers));
    CloseLiteIpc(g_remoteRegister.endpoint->context);
    SAMGR_Free(g_remoteRegister.endpoint);
    g_remoteRegister.endpoint = NULL;
}

static void InitializeRegistry(void)
{
    if (getuid() >= ABILITY_UID_START && !g_isAbilityInited) {
        ClearRegistry();
        g_isAbilityInited = TRUE;
    }
    if (g_remoteRegister.endpoint != NULL) {
        return;
    }
    HILOG_INFO(HILOG_MODULE_SAMGR, "Initialize Client Registry!");
    MUTEX_GlobalLock();
    if (g_remoteRegister.endpoint == NULL) {
        g_remoteRegister.mtx = MUTEX_InitValue();
        g_remoteRegister.clients = VECTOR_Make((VECTOR_Key)SAMGR_GetSAName, (VECTOR_Compare)SAMGR_CompareSAName);
        g_remoteRegister.endpoint = SAMGR_CreateEndpoint("ipc client", NULL);
    }
    MUTEX_GlobalUnlock();
}
