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

#include "bind_session_client.h"
#include "bind_session_common_util.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "group_operation_common.h"
#include "hc_log.h"
#include "session_manager.h"

static int32_t GenerateClientModuleParams(BindSession *session, CJson *moduleParams)
{
    if (AddIntToJson(moduleParams, FIELD_OPERATION_CODE,
        ((session->opCode == MEMBER_DELETE) ? OP_UNBIND : OP_BIND)) != HC_SUCCESS) {
        LOGE("Failed to add operationCode to moduleParams!");
        return HC_ERR_JSON_FAIL;
    }
    return GenerateBasicModuleParams(true, session, moduleParams);
}

static int32_t GetClientModuleReturnData(BindSession *session, CJson *out)
{
    CJson *moduleParams = CreateJson();
    if (moduleParams == NULL) {
        LOGE("Failed to allocate moduleParams memory!");
        return HC_ERR_JSON_FAIL;
    }

    int32_t result = GenerateClientModuleParams(session, moduleParams);
    if (result != HC_SUCCESS) {
        LOGE("Failed to generate all params sent to the module!");
        FreeJson(moduleParams);
        return result;
    }

    result = CreateAndProcessModule(session, moduleParams, out);
    FreeJson(moduleParams);
    if (result != HC_SUCCESS) {
        return result;
    }
    return HC_SUCCESS;
}

static int32_t PrepareData(BindSession *session, CJson **sendData)
{
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to allocate out memory!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t result = GetClientModuleReturnData(session, out);
    if (result != HC_SUCCESS) {
        FreeJson(out);
        return result;
    }

    *sendData = DetachItemFromJson(out, FIELD_SEND_TO_PEER);
    FreeJson(out);
    if (*sendData == NULL) {
        LOGE("Failed to get sendToPeer from out!");
        return HC_ERR_JSON_GET;
    }

    result = AddInfoToSendData(false, session, *sendData);
    if (result != HC_SUCCESS) {
        LOGE("Failed to add information to sendData!");
        FreeJson(*sendData);
        *sendData = NULL;
        return result;
    }
    return HC_SUCCESS;
}

static int32_t PrepareAndSendData(BindSession *session)
{
    CJson *sendData = NULL;
    int32_t result = PrepareData(session, &sendData);
    if (result != HC_SUCCESS) {
        return result;
    }

    result = SendBindSessionData(session, sendData);
    FreeJson(sendData);
    return result;
}

static int32_t DoubleCheckChannelId(int64_t channelId, int64_t oldChannelId)
{
    if (oldChannelId != channelId) {
        /* If the two channelIds are different, the soft bus channel must be used. */
        LOGE("The channelId returned by the soft bus are inconsistent, causing a channel error!");
        return HC_ERR_CHANNEL_NOT_EXIST;
    }
    return HC_SUCCESS;
}

static void OnBindChannelOpened(Session *session, int64_t channelId, int64_t requestId)
{
    if (session == NULL) {
        LOGE("The input session is NULL!");
        return;
    }

    BindSession *realSession = (BindSession *)session;
    /* Double check channelId. If the two channelIds are different, the channel fails to be established. */
    int32_t result = DoubleCheckChannelId(channelId, realSession->channelId);
    if (result != HC_SUCCESS) {
        ProcessErrorCallback(requestId, realSession->opCode, result, NULL, realSession->base.callback);
        DestroySession(requestId);
        return;
    }

    result = PrepareAndSendData(realSession);
    if (result != HC_SUCCESS) {
        LOGI("An error occurs before the client send data to the server. We need to notify the service!");
        if ((!NeedForceDelete(realSession)) || (ForceUnbindDevice(realSession) != HC_SUCCESS)) {
            ProcessErrorCallback(requestId, realSession->opCode, result, NULL, realSession->base.callback);
        }
        CloseChannel(realSession->channelType, realSession->channelId);
        DestroySession(requestId);
    }
}

static int32_t PrepareClient(const CJson *jsonParams, BindSession *session)
{
    int32_t result = GenerateBindParams(session->osAccountId, CLIENT, jsonParams, session);
    if (result != HC_SUCCESS) {
        return result;
    }
    return OpenChannel(session->channelType, jsonParams, session->reqId, &session->channelId);
}

Session *CreateClientBindSession(CJson *jsonParams, const DeviceAuthCallback *callback)
{
    int32_t opCode = MEMBER_INVITE;
    if (GetIntFromJson(jsonParams, FIELD_OPERATION_CODE, &opCode) != HC_SUCCESS) {
        LOGE("Failed to get opCode from json!");
        return NULL;
    }
    /*
     * If service want to join the peer group,
     * the identity key pair of the corresponding group needs to be generated here.
     */
    int32_t result;
    if (NeedCreateGroup(CLIENT, opCode)) {
        const char *groupId = GetStringFromJson(jsonParams, FIELD_GROUP_ID);
        if (groupId == NULL) {
            LOGE("Failed to get groupId from jsonParams!");
            return NULL;
        }
        result = ProcessKeyPair(CREATE_KEY_PAIR, jsonParams, groupId);
        if (result != HC_SUCCESS) {
            return NULL;
        }
    }

    BindSession *session = CreateBaseBindSession(TYPE_CLIENT_BIND_SESSION, opCode,
        jsonParams, callback, ProcessBindSession);
    if (session == NULL) {
        return NULL;
    }
    InitClientChannel(callback, jsonParams, session);
    /* The client bind session needs to receive a message indicating that the channel is open. */
    session->onChannelOpened = OnBindChannelOpened;

    result = PrepareClient(jsonParams, session);
    if (result != HC_SUCCESS) {
        ProcessErrorCallback(session->reqId, session->opCode, result, NULL, session->base.callback);
        DestroyBindSession((Session *)session);
        return NULL;
    }
    return (Session *)session;
}
