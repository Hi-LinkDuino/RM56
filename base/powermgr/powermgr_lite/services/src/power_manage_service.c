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

#include <ohos_init.h>
#include <samgr_lite.h>
#include <service.h>

#include <unistd.h>
#include <pthread.h>

#include "hilog_wrapper.h"
#include "power_mgr.h"

#define STACK_SIZE      0x800
#define QUEUE_SIZE      20

typedef struct {
    INHERIT_SERVICE;
    Identity identity;
} PowerMgrService;

static const char *GetName(Service *service);
static BOOL Initialize(Service *service, Identity identity);
static BOOL MessageHandle(Service *service, Request *request);
static TaskConfig GetTaskConfig(Service *service);

static PowerMgrService g_service = {
    .GetName = GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    .identity = { -1, -1, NULL }
};

static const char *GetName(Service *service)
{
    (void)service;
    return POWER_MANAGE_SERVICE;
}

static BOOL Initialize(Service *service, Identity identity)
{
    if (service == NULL) {
        POWER_HILOGE("Invalid service");
        return FALSE;
    }
    PowerMgrService *pms = (PowerMgrService *)(service);
    pms->identity = identity;
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *request)
{
    (void)service;
    return request != NULL;
}

static TaskConfig GetTaskConfig(Service *service)
{
    (void)service;
    TaskConfig config = { LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK };
    return config;
}

static void Init(void)
{
    SAMGR_GetInstance()->RegisterService((Service *)&g_service);
    POWER_HILOGI("Succeed to init power manager service");
}
SYS_SERVICE_INIT(Init);
