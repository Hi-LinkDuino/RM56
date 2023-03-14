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

#include "bind_session_client_lite.h"
#include "bind_session_common_util.h"
#include "callback_manager.h"
#include "channel_manager.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "session_manager.h"

static int32_t DoubleCheckChannelId(int64_t channelId, int64_t oldChannelId)
{
    if (oldChannelId != channelId) {
        /* If the two channelIds are different, the soft bus channel must be used. */
        LOGE("The channelId returned by the soft bus are inconsistent, causing a channel error!");
        return HC_ERR_CHANNEL_NOT_EXIST;
    }
    return HC_SUCCESS;
}

static void OnLiteBindChannelOpened(Session *session, int64_t channelId, int64_t requestId)
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

    bool isNeedInform = true;
    result = LitePrepareAndSendData(realSession, &isNeedInform);
    if (result != HC_SUCCESS) {
        ProcessErrorCallback(requestId, realSession->opCode, result, NULL, realSession->base.callback);
        CloseChannel(realSession->channelType, realSession->channelId);
        DestroySession(requestId);
    }
}

static int32_t LitePrepareClient(const CJson *jsonParams, BindSession *session)
{
    int32_t result = LiteSaveReceivedData(session, jsonParams);
    if (result != HC_SUCCESS) {
        return result;
    }
    return OpenChannel(session->channelType, jsonParams, session->reqId, &session->channelId);
}

Session *CreateLiteClientBindSession(CJson *jsonParams, const DeviceAuthCallback *callback)
{
    int opCode = OP_BIND;
    if (GetIntFromJson(jsonParams, FIELD_OPERATION_CODE, &opCode) != HC_SUCCESS) {
        LOGE("Failed to get opCode from jsonParams!");
        return NULL;
    }

    BindSession *session = CreateBaseBindSession(TYPE_CLIENT_BIND_SESSION_LITE, opCode,
        jsonParams, callback, ProcessLiteBindSession);
    if (session == NULL) {
        return NULL;
    }
    InitClientChannel(callback, jsonParams, session);
    InitModuleType(jsonParams, session);
    /* The client bind session needs to receive a message indicating that the channel is open. */
    session->onChannelOpened = OnLiteBindChannelOpened;

    int32_t result = LitePrepareClient(jsonParams, session);
    if (result != HC_SUCCESS) {
        ProcessErrorCallback(session->reqId, session->opCode, result, NULL, session->base.callback);
        DestroyBindSession((Session *)session);
        return NULL;
    }
    return (Session *)session;
}