/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "group_manager_common.h"

#include "hc_log.h"
#include "common_defs.h"
#include "callback_manager.h"
#include "task_manager.h"

static int32_t InitGroupManagerTask(GroupManagerTask *task, GMTaskParams *taskParams, TaskFunc func)
{
    task->base.doAction = func;
    task->base.destroy = DestroyGroupManagerTask;
    task->osAccountId = taskParams->osAccountId;
    task->opCode = taskParams->opCode;
    task->reqId = taskParams->reqId;
    task->params = taskParams->params;
    return BindCallbackToTask(task, taskParams->params);
}

int32_t AddReqInfoToJson(int64_t requestId, const char *appId, CJson *jsonParams)
{
    if (AddInt64StringToJson(jsonParams, FIELD_REQUEST_ID, requestId) != HC_SUCCESS) {
        LOGE("Failed to add requestId to json!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddStringToJson(jsonParams, FIELD_APP_ID, appId) != HC_SUCCESS) {
        LOGE("Failed to add appId to json!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

int32_t BindCallbackToTask(GroupManagerTask *task, const CJson *jsonParams)
{
    const char *appId = GetStringFromJson(jsonParams, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from jsonParams!");
        return HC_ERR_JSON_GET;
    }
    task->cb = GetGMCallbackByAppId(appId);
    if (task->cb == NULL) {
        LOGE("Failed to find callback by appId! [AppId]: %s", appId);
        return HC_ERR_CALLBACK_NOT_FOUND;
    }
    return HC_SUCCESS;
}

void DestroyGroupManagerTask(HcTaskBase *task)
{
    if (task == NULL) {
        LOGE("The input task is NULL!");
        return;
    }
    FreeJson(((GroupManagerTask *)task)->params);
}

int32_t AddBindParamsToJson(int operationCode, int64_t requestId, const char *appId, CJson *jsonParams)
{
    if (AddIntToJson(jsonParams, FIELD_OPERATION_CODE, operationCode) != HC_SUCCESS) {
        LOGE("Failed to add operationCode to json!");
        return HC_ERR_JSON_FAIL;
    }
    return AddReqInfoToJson(requestId, appId, jsonParams);
}

int32_t InitAndPushGMTask(int32_t osAccountId, int32_t opCode, int64_t reqId, CJson *params, TaskFunc func)
{
    GroupManagerTask *task = (GroupManagerTask *)HcMalloc(sizeof(GroupManagerTask), 0);
    if (task == NULL) {
        LOGE("Failed to allocate task memory!");
        return HC_ERR_ALLOC_MEMORY;
    }
    GMTaskParams taskParams;
    taskParams.osAccountId = osAccountId;
    taskParams.opCode = opCode;
    taskParams.reqId = reqId;
    taskParams.params = params;
    if (InitGroupManagerTask(task, &taskParams, func) != HC_SUCCESS) {
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    if (PushTask((HcTaskBase *)task) != HC_SUCCESS) {
        HcFree(task);
        return HC_ERR_INIT_TASK_FAIL;
    }
    return HC_SUCCESS;
}