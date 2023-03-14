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

#include "bind_peer.h"

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

static int32_t GetOpCodeWhenLite(const CJson *params)
{
    int32_t opCode = OP_BIND;
    if (GetIntFromJson(params, FIELD_OPERATION_CODE, &opCode) != HC_SUCCESS) {
        LOGW("Failed to get operationCode from params!");
    }
    return opCode;
}

static int32_t GetModuleType(const CJson *jsonParams)
{
    bool isAccountBind = false;
    (void)GetBoolFromJson(jsonParams, FIELD_IS_ACCOUNT_BIND, &isAccountBind);
    return isAccountBind ? ACCOUNT_MODULE : DAS_MODULE;
}

static int32_t AddLiteDataToReceivedData(CJson *receivedData, int64_t requestId, const char *appId)
{
    int groupOp = ACCOUNT_BIND;
    (void)GetIntFromJson(receivedData, FIELD_GROUP_OP, &groupOp);
    if (groupOp == ACCOUNT_BIND) {
        if (AddBoolToJson(receivedData, FIELD_IS_ACCOUNT_BIND, true) != HC_SUCCESS) {
            LOGE("Failed to add isAccountBind to json!");
            return HC_ERR_JSON_FAIL;
        }
        return AddBindParamsToJson(OP_BIND, requestId, appId, receivedData);
    }
    return AddServerParamsToJson(false, requestId, appId, receivedData);
}

static void DoClientTask(HcTaskBase *baseTask)
{
    if (baseTask == NULL) {
        LOGE("The input task is NULL!");
        return;
    }
    GroupManagerTask *task = (GroupManagerTask *)baseTask;
    int32_t res = CreateSession(task->reqId, TYPE_CLIENT_BIND_SESSION_LITE, task->params, task->cb);
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
        ((CheckMsgRepeatability(task->params, GetModuleType(task->params))) != HC_SUCCESS)) {
        return;
    }
    int32_t res = CreateSession(task->reqId, TYPE_SERVER_BIND_SESSION_LITE, task->params, task->cb);
    if (res != HC_SUCCESS) {
        if (res != HC_ERR_CREATE_SESSION_FAIL) {
            ProcessErrorCallback(task->reqId, task->opCode, res, NULL, task->cb);
        }
    }
}

int32_t RequestBindPeer(int64_t requestId, const char *appId, const char *bindParams)
{
    if ((appId == NULL) || (bindParams == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    LOGI("[Start]: RequestBindPeer! [AppId]: %s, [ReqId]: %" PRId64, appId, requestId);
    CJson *params = CreateJsonFromString(bindParams);
    if (params == NULL) {
        LOGE("Failed to create json from string!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddClientParamsToJson(true, OP_BIND, requestId, appId, params) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_JSON_FAIL;
    }
    if (InitAndPushGMTask(OP_BIND, requestId, params, DoClientTask) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("[End]: RequestBindPeer!");
    return HC_SUCCESS;
}

int32_t RequestUnbindPeer(int64_t requestId, const char *appId, const char *unbindParams)
{
    if ((appId == NULL) || (unbindParams == NULL)) {
        LOGE("The input parameters contains NULL value!");
        return HC_ERR_INVALID_PARAMS;
    }
    LOGI("[Start]: RequestUnbindPeer! [AppId]: %s, [ReqId]: %" PRId64, appId, requestId);
    CJson *params = CreateJsonFromString(unbindParams);
    if (params == NULL) {
        LOGE("Failed to create json from string!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddClientParamsToJson(true, OP_UNBIND, requestId, appId, params) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_JSON_FAIL;
    }
    if (InitAndPushGMTask(OP_UNBIND, requestId, params, DoClientTask) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("[End]: RequestUnbindPeer!");
    return HC_SUCCESS;
}

int32_t RequestProcessLiteData(int64_t requestId, const char *appId, const uint8_t *data, uint32_t dataLen)
{
    if ((appId == NULL) || (data == NULL) || (dataLen == 0) || (dataLen > MAX_DATA_BUFFER_SIZE)) {
        LOGE("Invalid input parameters!");
        return HC_ERR_INVALID_PARAMS;
    }
    LOGI("[Start]: RequestProcessLiteData! [AppId]: %s, [ReqId]: %" PRId64, appId, requestId);
    CJson *params = CreateJsonFromString((const char *)data);
    if (params == NULL) {
        LOGE("Failed to create json from string!");
        return HC_ERR_JSON_FAIL;
    }
    if (AddLiteDataToReceivedData(params, requestId, appId) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_JSON_FAIL;
    }
    if (InitAndPushGMTask(GetOpCodeWhenLite(params), requestId, params, DoServerTask) != HC_SUCCESS) {
        FreeJson(params);
        return HC_ERR_INIT_TASK_FAIL;
    }
    LOGI("[End]: RequestProcessLiteData!");
    return HC_SUCCESS;
}