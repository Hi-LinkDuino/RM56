/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ipc_service_init.h"

#include "device_auth_defines.h"
#include "hc_log.h"
#include "ipc_dev_auth_stub.h"
#include "liteipc_adapter.h"
#include "ohos_init.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char *GetName(Service *service)
{
    (void)service;
    return DEV_AUTH_SERVICE_NAME;
}

static BOOL Initialize(Service *service, Identity identity)
{
    if (service == NULL) {
        LOGW("invalid param");
        return FALSE;
    }

    DevAuthService *mgrService = (DevAuthService *)service;
    mgrService->identity = identity;
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *request)
{
    if ((service == NULL) || (request == NULL)) {
        LOGE("invalid param");
        return FALSE;
    }
    return TRUE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    const uint16_t stackSz = 0x1000; /* task stack size 4096 for task configuration */
    const uint16_t queSize = 32; /* task queue size 32 for task configuration */
    (void)service;
    TaskConfig config = { LEVEL_HIGH, PRI_BELOW_NORMAL, stackSz, queSize, SHARED_TASK };
    return config;
}

void ServiceInit(void)
{
    static DevAuthService service = {
        .GetName = GetName,
        .Initialize = Initialize,
        .MessageHandle = MessageHandle,
        .GetTaskConfig = GetTaskConfig,
        SERVER_IPROXY_IMPL_BEGIN,
        .Invoke = OnRemoteRequest,
        IPROXY_END,
    };

#ifdef __LINUX__
    sleep(1); /* delay 1 second on boot */
#endif
    if (!SAMGR_GetInstance()->RegisterService((Service *)&service)) {
        LOGE("%s, RegisterService failed", DEV_AUTH_SERVICE_NAME);
        return;
    }
    if (!SAMGR_GetInstance()->RegisterDefaultFeatureApi(DEV_AUTH_SERVICE_NAME, GET_IUNKNOWN(service))) {
        LOGE("%s, RegisterDefaultFeatureApi failed", DEV_AUTH_SERVICE_NAME);
        return;
    }
    LOGI("%s, init success", DEV_AUTH_SERVICE_NAME);
    return;
}
SYS_SERVICE_INIT(ServiceInit);

#ifdef __cplusplus
}
#endif

