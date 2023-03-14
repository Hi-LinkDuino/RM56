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

#include "key_agree.h"

#include "callback_manager.h"
#include "channel_manager.h"
#include "dev_auth_module_manager.h"
#include "group_manager_common.h"
#include "hc_log.h"
#include "session_manager.h"
#include "task_manager.h"

static int32_t AddClientParamsToJson(bool isClient, int32_t opCode, int64_t reqId, const char *appId, CJson *jsonParams)
{
    if (AddBoolToJson(jsonParams, FIELD_IS_CLIENT, isClient) != HC_SUCCESS) {
        LOGE("Failed to add isClient to json!");
        return HC_ERR_JSON_FAIL;
    }
    return AddBindParamsToJson(opCode, reqId, appId, jsonParams);
}

static int32_t AddServerParamsToJson(bool isClient, int64_t reqId, const char *appId, CJson *jsonParams)
{
    if (AddBoolToJson(jsonParams, FIELD_IS_CLIENT, isClient) != HC_SUCCESS) {
        LOGE("Failed to add isClient to json!");
        return HC_ERR_JSON_FAIL;
    }
    return AddReqInfoToJson(reqId, appId, jsonParams);
}

static void DoClientTask(HcTaskBase *baseTask)
{
    if (baseTask == NULL) {
        LOGE("The input task is NULL!");
        return;
    }
    GroupManagerTask *task = (GroupManagerTask *)baseTask;
    int32_t res = CreateSession(task->reqId, TYPE_CLIENT_KEY_AGREE_SESSION, task->params, task->cb);
    if (res != HC_SUCCESS) {
        if (res != HC_ERR_CREATE_SESSION_FAIL) {
            ProcessErrorCallback(task->reqId, task->opCode, res, NULL, task->cb);
        }
        return;
    }
    /**
     * If service open the channel by itself,
     * a channel opened message needs to be triggered to unify the channel usage policy.
     */
    if (GetChannelType(task->cb, task->params) == SERVICE_CHANNEL) {
        /* Release the memory in advance to reduce the memory usage. */
        DeleteAllItem(task->params);
        OnChannelOpened(task->reqId, DEFAULT_CHANNEL_ID);
    }
}

static void DoServerTask(HcTaskBase *baseTask)
{
    if (baseTask == NULL) {
        LOGE("The input task is NULL!");
        return;
    }
    GroupManagerTask *task = (GroupManagerTask *)baseTask;
    if (IsRequestExist(task->reqId)) {
        int32_t ret = ProcessSession(task->reqId, BIND_TYPE, task->params);
        if (ret != CONTINUE) {
            DestroySession(task->reqId);
        }
        return;
    }
    if ((BindCallbackToTask(task, task->params) != HC_SUCCESS) ||
        ((CheckMsgRepeatability(task->params, DAS_MODULE)) != HC_SUCCESS)) {
        return;
    }
    int32_t res = CreateSession(task->reqId, TYPE_SERVER_KEY_AGREE_SESSION, task->params, task->cb);
    if (res != HC_SUCCESS) {
        if (res != HC_ERR_CREATE_SESSION_FAIL) {
            ProcessErrorCallback(task->reqId, task->opCode, res, NULL, task->cb);
        }
    }
}

int32_t RequestAuthKeyAgree(int64_t requestId, const char *appId, const char *agreeParams)
{
    if ((appId == NULL) || (agreeParams == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    LOGI("[Start]: RequestAuthKeyAgree! [AppId]: %s, [ReqId]: %" PRId64, appId, requestId);
    CJson *params = CreateJsonFromString(agreeParams);
    if (params == NULL) {
        LOGE("Failed to create json from string!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddClientParamsToJson(true, AUTH_KEY_AGREEMENT, requestId, appId, params) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_JSON_FAIL;
    }
    if (InitAndPushGMTask(AUTH_KEY_AGREEMENT, requestId, params, DoClientTask) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("[End]: RequestAuthKeyAgree!");
    return HC_SUCCESS;
}

int32_t RequestProcessKeyAgreeData(int64_t requestId, const char *appId, const uint8_t *data, uint32_t dataLen)
{
    if ((appId == NULL) || (data == NULL) || (dataLen == 0) || (dataLen > MAX_DATA_BUFFER_SIZE)) {
        LOGE("Invalid input parameters!");
        return HC_ERR_INVALID_PARAMS;
    }
    LOGI("[Start]: RequestProcessKeyAgreeData! [AppId]: %s, [ReqId]: %" PRId64, appId, requestId);
    CJson *params = CreateJsonFromString((const char *)data);
    if (params == NULL) {
        LOGE("Failed to create json from string!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddServerParamsToJson(false, requestId, appId, params) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_JSON_FAIL;
    }
    if (InitAndPushGMTask(AUTH_KEY_AGREEMENT, requestId, params, DoServerTask) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("[End]: RequestProcessKeyAgreeData!");
    return HC_SUCCESS;
}