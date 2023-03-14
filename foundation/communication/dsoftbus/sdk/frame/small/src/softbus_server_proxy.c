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

#include "iproxy_client.h"
#include "liteipc_adapter.h"
#include "samgr_lite.h"
#include "softbus_adapter_file.h"
#include "softbus_adapter_timer.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_ipc_def.h"
#include "softbus_log.h"
#include "softbus_server_proxy.h"


#define WAIT_SERVER_READY_INTERVAL_COUNT 50

static IClientProxy *g_serverProxy = NULL;

static int ClientSimpleResultCb(IOwner owner, int code, IpcIo *reply)
{
    *(int *)owner = IpcIoPopInt32(reply);
    SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_INFO, "retvalue:%d", *(int *)owner);
    return EC_SUCCESS;
}

static IClientProxy *GetServerProxy(void)
{
    IClientProxy *clientProxy = NULL;

    SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_INFO, "start get client proxy");
    int32_t proxyInitCount = 0;
    while (clientProxy == NULL) {
        proxyInitCount++;
        if (proxyInitCount == WAIT_SERVER_READY_INTERVAL_COUNT) {
            SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "frame get server proxy error");
            return NULL;
        }
        IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(SOFTBUS_SERVICE);
        if (iUnknown == NULL) {
            SoftBusSleepMs(WAIT_SERVER_READY_INTERVAL);
            continue;
        }

        int32_t ret = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&clientProxy);
        if (ret != EC_SUCCESS || clientProxy == NULL) {
            SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "QueryInterface failed [%d]", ret);
            SoftBusSleepMs(WAIT_SERVER_READY_INTERVAL);
            continue;
        }
    }

    SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_INFO, "frame get client proxy ok");
    return clientProxy;
}

int32_t RegisterService(const char *name, const struct CommonScvId *svcId)
{
    SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_INFO, "server register service client push.");
    if ((svcId == NULL) || (name == NULL)) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    uint8_t data[MAX_SOFT_BUS_IPC_LEN] = {0};

    IpcIo request = {0};
    IpcIoInit(&request, data, MAX_SOFT_BUS_IPC_LEN, 1);
    IpcIoPushString(&request, name);

    SvcIdentity svc = {0};
    svc.handle = svcId->handle;
    svc.token = svcId->token;
    svc.cookie = svcId->cookie;
#ifdef __LINUX__
    svc.ipcContext = svcId->ipcCtx;
#endif
    IpcIoPushSvc(&request, &svc);

    int ret = SOFTBUS_ERR;
    if (g_serverProxy->Invoke(g_serverProxy, MANAGE_REGISTER_SERVICE, &request, &ret,
        ClientSimpleResultCb) != EC_SUCCESS) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_INFO, "Call back ret(%d)", ret);
        return SOFTBUS_ERR;
    }
    return ret;
}

void __attribute__((weak)) HOS_SystemInit(void)
{
    SAMGR_Bootstrap();
    return;
}

int32_t ServerProxyInit(void)
{
    HOS_SystemInit();
    g_serverProxy = GetServerProxy();
    if (g_serverProxy == NULL) {
        SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_ERROR, "get ipc client proxy failed");
        return SOFTBUS_ERR;
    }
    SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_INFO, "ServerProvideInterfaceInit ok");
    return SOFTBUS_OK;
}
