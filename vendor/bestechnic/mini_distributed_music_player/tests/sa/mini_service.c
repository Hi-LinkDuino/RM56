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

#include "ohos_init.h"

#include "common.h"
#include "iproxy_server.h"
#include "mini_service.h"
#include "samgr_lite.h"
#include "service.h"
#include "stdio.h"

#define MINI_SERVICE "mini_sa_rpc"
static const char *GetName(Service *service);
static BOOL Initialize(Service *service, Identity identity);
static TaskConfig GetTaskConfig(Service *service);
static BOOL MessageHandle(Service *service, Request *request);
static int32_t FeatureInvoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply);

static MiniService g_miniService = {
        .GetName = GetName,
        .Initialize = Initialize,
        .MessageHandle = MessageHandle,
        .GetTaskConfig = GetTaskConfig,
        SERVER_IPROXY_IMPL_BEGIN,
        .Invoke = FeatureInvoke,
        IPROXY_END,
};
extern void CallOnPlayRemote(int32_t data);
static int32_t FeatureInvoke(IServerProxy *iProxy, int32_t funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    int32_t data;
    ReadInt32(req, &data);
    CallOnPlayRemote(data);
    return 0;
}

static void Init(void)
{
    SAMGR_GetInstance()->RegisterService((Service *)&g_miniService);
    SAMGR_GetInstance()->RegisterDefaultFeatureApi(MINI_SERVICE, GET_IUNKNOWN(g_miniService));
}
SYS_SERVICE_INIT(Init);

static const char *GetName(Service *service)
{
    (void)service;
    return MINI_SERVICE;
}

static BOOL IsDistributed(void)
{
    return TRUE;
}

static BOOL Initialize(Service *service, Identity identity)
{
    MiniService *miniSa = NULL;

    if (service == NULL) {
        return FALSE;
    }
    miniSa = (MiniService *)service;
    miniSa->identity = identity;
    /* The communication of task can be use after the service is running. */
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *request)
{
    (void)service;
    (void)request;
    return TRUE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = { LEVEL_LOW, PRI_NORMAL, 0x2000, 10, SINGLE_TASK };
    return config;
}
