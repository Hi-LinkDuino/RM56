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

#include "bind_session_server_lite.h"
#include "bind_session_common_util.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "session_manager.h"

static int32_t RequestConfirmation(const BindSession *session, char **returnStr)
{
    char *returnDataStr = ProcessRequestCallback(session->reqId, session->opCode, NULL,
        session->base.callback);
    if (returnDataStr == NULL) {
        LOGE("The OnRequest callback is fail!");
        return HC_ERR_REQ_REJECTED;
    }
    *returnStr = returnDataStr;
    LOGI("The server receives the data returned by the service!");
    return HC_SUCCESS;
}

static int32_t CombineInputData(const CJson *returnData, CJson *jsonParams)
{
    if (AddObjToJson(jsonParams, FIELD_RETURN_DATA, returnData) != HC_SUCCESS) {
        LOGE("Failed to add returnData to jsonParams!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static int32_t LitePrepareServer(BindSession *session, CJson *returnData, bool *isNeedInform)
{
    if ((session->isWaiting) && (!IsAcceptRequest(returnData))) {
        LOGE("The service rejects the request!");
        return HC_ERR_REQ_REJECTED;
    }
    int32_t result = CombineInputData(returnData, session->params);
    /* Release the memory in advance to reduce the memory usage. */
    DeleteAllItem(returnData);
    session->isWaiting = false;
    if (result != HC_SUCCESS) {
        return result;
    }
    return LitePrepareAndSendData(session, isNeedInform);
}

static int32_t JudgeConfirmation(CJson *returnData, CJson *jsonParams, BindSession *session, bool *isNeedInform)
{
    uint32_t confirmation = REQUEST_ACCEPTED;
    if (GetUnsignedIntFromJson(returnData, FIELD_CONFIRMATION, &confirmation) != HC_SUCCESS) {
        LOGE("Failed to get confirmation from returnData!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t result;
    if (confirmation == REQUEST_WAITING) {
        LOGI("The service wants us to wait for its signal!");
        result = LiteSaveReceivedData(session, jsonParams);
        session->isWaiting = true;
        return result;
    } else if (confirmation == REQUEST_ACCEPTED) {
        LOGI("The service accepts the request!");
        result = LiteSaveReceivedData(session, jsonParams);
        if (result != HC_SUCCESS) {
            return result;
        }
        /* Release the memory in advance to reduce the memory usage. */
        DeleteAllItem(jsonParams);
        return LitePrepareServer(session, returnData, isNeedInform);
    } else {
        LOGE("The service rejects the request! [Confirmation]: %u", confirmation);
        return HC_ERR_REQ_REJECTED;
    }
}

static int32_t HandleRequest(CJson *jsonParams, BindSession *session, bool *isNeedInform)
{
    char *returnDataStr = NULL;
    int32_t result = RequestConfirmation(session, &returnDataStr);
    if (result != HC_SUCCESS) {
        return result;
    }

    CJson *returnData = CreateJsonFromString(returnDataStr);
    FreeJsonString(returnDataStr);
    if (returnData == NULL) {
        LOGE("Failed to create returnData from string!");
        return HC_ERR_JSON_FAIL;
    }
    result = JudgeConfirmation(returnData, jsonParams, session, isNeedInform);
    FreeJson(returnData);
    return result;
}

static void OnLiteBindConfirmationReceived(Session *session, CJson *returnData)
{
    if ((session == NULL) || (returnData == NULL)) {
        LOGE("The input session or returnData is NULL!");
        return;
    }
    BindSession *realSession = (BindSession *)session;

    bool isNeedInform = true;
    int32_t result = LitePrepareServer(realSession, returnData, &isNeedInform);
    if (result != HC_SUCCESS) {
        InformPeerGroupErrorIfNeed(isNeedInform, result, realSession);
        ProcessErrorCallback(realSession->reqId, realSession->opCode, result, NULL, realSession->base.callback);
        CloseChannel(realSession->channelType, realSession->channelId);
        DestroySession(realSession->reqId);
    }
}

Session *CreateLiteServerBindSession(CJson *jsonParams, const DeviceAuthCallback *callback)
{
    int opCode = OP_BIND;
    (void)(GetIntFromJson(jsonParams, FIELD_OPERATION_CODE, &opCode));

    BindSession *session = CreateBaseBindSession(TYPE_SERVER_BIND_SESSION_LITE, opCode,
        jsonParams, callback, ProcessLiteBindSession);
    if (session == NULL) {
        return NULL;
    }
    InitServerChannel(jsonParams, session);
    InitModuleType(jsonParams, session);
    /* The server may receive the confirm request message. */
    session->onConfirmed = OnLiteBindConfirmationReceived;

    bool isNeedInform = true;
    int32_t result = HandleRequest(jsonParams, session, &isNeedInform);
    if (result != HC_SUCCESS) {
        InformPeerGroupErrorIfNeed(isNeedInform, result, session);
        ProcessErrorCallback(session->reqId, session->opCode, result, NULL, session->base.callback);
        CloseChannel(session->channelType, session->channelId);
        DestroyBindSession((Session *)session);
        return NULL;
    }
    return (Session *)session;
}