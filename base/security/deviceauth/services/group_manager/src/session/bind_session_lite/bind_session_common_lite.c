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

#include "bind_session_common_lite.h"
#include "bind_session_common_util.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "hc_log.h"
#include "securec.h"
#include "session_common.h"

static int32_t AddGroupOpToSendDataIfNeed(const BindSession *session, CJson *sendData)
{
    if ((session->moduleType != ACCOUNT_MODULE) || (session->opCode != OP_BIND)) {
        return HC_SUCCESS;
    }
    if (AddIntToJson(sendData, FIELD_GROUP_OP, ACCOUNT_BIND) != HC_SUCCESS) {
        LOGE("Failed to add groupOp to json!");
        return HC_ERR_JSON_ADD;
    }
    return HC_SUCCESS;
}

static int32_t OnSessionFinish(const BindSession *session, CJson *out)
{
    int32_t res;
    CJson *sendData = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    /* The last packet may need to be sent */
    if (sendData != NULL) {
        res = AddGroupOpToSendDataIfNeed(session, sendData);
        if (res != HC_SUCCESS) {
            return res;
        }
        res = SendBindSessionData(session, sendData);
        if (res != HC_SUCCESS) {
            return res;
        }
    }
    uint8_t sessionKey[DEFAULT_RETURN_KEY_LENGTH] = { 0 };
    if (GetByteFromJson(out, FIELD_SESSION_KEY, sessionKey, DEFAULT_RETURN_KEY_LENGTH) == HC_SUCCESS) {
        ProcessSessionKeyCallback(session->reqId, sessionKey, DEFAULT_RETURN_KEY_LENGTH, session->base.callback);
        (void)memset_s(sessionKey, DEFAULT_RETURN_KEY_LENGTH, 0, DEFAULT_RETURN_KEY_LENGTH);
        ClearSensitiveStringInJson(out, FIELD_SESSION_KEY);
    }
    char *returnDataStr = NULL;
    const CJson *returnData = GetObjFromJson(out, FIELD_SEND_TO_SELF);
    if (returnData != NULL) {
        returnDataStr = PackJsonToString(returnData);
    }
    ProcessFinishCallback(session->reqId, session->opCode, returnDataStr, session->base.callback);
    FreeJsonString(returnDataStr);
    LOGI("The session completed successfully! [ReqId]: %" PRId64, session->reqId);
    NotifyBindResult(session->channelType, session->channelId);
    CloseChannel(session->channelType, session->channelId);
    return HC_SUCCESS;
}

static int32_t ProcessLiteBindSessionInner(BindSession *session, CJson *jsonParams, int32_t *status, bool *isNeedInform)
{
    int32_t res = CheckPeerStatus(jsonParams, isNeedInform);
    if (res != HC_SUCCESS) {
        return res;
    }

    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to allocate out memory!");
        return HC_ERR_JSON_FAIL;
    }

    res = ProcessModule(session, jsonParams, out, status);
    DeleteAllItem(jsonParams);
    if (res != HC_SUCCESS) {
        *isNeedInform = false;
        InformPeerModuleError(out, session);
        FreeJson(out);
        return res;
    }

    if (*status == IGNORE_MSG) {
        FreeJson(out);
        return HC_SUCCESS;
    } else if (*status == CONTINUE) {
        CJson *sendData = DetachItemFromJson(out, FIELD_SEND_TO_PEER);
        FreeJson(out);
        if (sendData == NULL) {
            LOGE("Failed to get sendToPeer from out!");
            return HC_ERR_JSON_GET;
        }
        res = AddGroupOpToSendDataIfNeed(session, sendData);
        if (res != HC_SUCCESS) {
            FreeJson(sendData);
            return res;
        }
        res = SendBindSessionData(session, sendData);
        FreeJson(sendData);
        return res;
    }

    res = OnSessionFinish(session, out);
    FreeJson(out);
    return res;
}

static int32_t LitePrepareData(BindSession *session, CJson **sendData, bool *isNeedInform)
{
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to allocate out memory!");
        return HC_ERR_JSON_FAIL;
    }

    int32_t result = CreateAndProcessModule(session, session->params, out);
    if (result != HC_SUCCESS) {
        *isNeedInform = false;
        InformPeerModuleError(out, session);
        FreeJson(out);
        return result;
    }

    DeleteAllItem(session->params);
    *sendData = DetachItemFromJson(out, FIELD_SEND_TO_PEER);
    FreeJson(out);
    if (*sendData == NULL) {
        LOGE("Failed to get sendToPeer from out!");
        return HC_ERR_JSON_GET;
    }
    return AddGroupOpToSendDataIfNeed(session, *sendData);
}

void InitModuleType(const CJson *jsonParams, BindSession *session)
{
    bool isAccountBind = false;
    (void)GetBoolFromJson(jsonParams, FIELD_IS_ACCOUNT_BIND, &isAccountBind);
    session->moduleType = (isAccountBind) ? ACCOUNT_MODULE : DAS_MODULE;
}

int32_t ProcessLiteBindSession(Session *session, CJson *jsonParams)
{
    if ((session == NULL) || (jsonParams == NULL)) {
        LOGE("The input session or jsonParams is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    BindSession *realSession = (BindSession *)session;
    LOGI("Start to process bind session successfully! [ReqId]: %" PRId64, realSession->reqId);

    bool isNeedInform = true;
    int32_t status = CONTINUE;
    int32_t result = ProcessLiteBindSessionInner(realSession, jsonParams, &status, &isNeedInform);
    if (result != HC_SUCCESS) {
        InformPeerGroupErrorIfNeed(isNeedInform, result, realSession);
        ProcessErrorCallback(realSession->reqId, realSession->opCode, result, NULL, realSession->base.callback);
        CloseChannel(realSession->channelType, realSession->channelId);
        return result;
    }
    if (status == FINISH) {
        return status;
    }
    return result;
}

int32_t LitePrepareAndSendData(BindSession *session, bool *isNeedInform)
{
    CJson *sendData = NULL;
    int32_t result = LitePrepareData(session, &sendData, isNeedInform);
    if (result != HC_SUCCESS) {
        return result;
    }

    result = SendBindSessionData(session, sendData);
    FreeJson(sendData);
    return result;
}

int32_t LiteSaveReceivedData(BindSession *session, const CJson *jsonParams)
{
    if (session->params == NULL) {
        session->params = CreateJson();
        if (session->params == NULL) {
            LOGE("Failed to allocate session params memory!");
            return HC_ERR_ALLOC_MEMORY;
        }
    }
    if (AddObjToJson(session->params, FIELD_RECEIVED_DATA, jsonParams) != HC_SUCCESS) {
        LOGE("Failed to add received data to session params!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}
