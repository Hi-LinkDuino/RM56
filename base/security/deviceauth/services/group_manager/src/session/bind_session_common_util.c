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

#include "bind_session_common_util.h"
#include "channel_manager.h"
#include "dev_auth_module_manager.h"
#include "hc_log.h"
#include "hc_types.h"
#include "session_common.h"

static int32_t GenerateGroupErrorMsg(int32_t errorCode, const BindSession *session, CJson *errorData)
{
    if (AddIntToJson(errorData, FIELD_GROUP_ERROR_MSG, errorCode) != HC_SUCCESS) {
        LOGE("Failed to add errorCode to errorData!");
        return HC_ERR_JSON_ADD;
    }
    if (AddStringToJson(errorData, FIELD_APP_ID, session->appId) != HC_SUCCESS) {
        LOGE("Failed to add appId to errorData!");
        return HC_ERR_JSON_ADD;
    }
    if (AddInt64StringToJson(errorData, FIELD_REQUEST_ID, session->reqId) != HC_SUCCESS) {
        LOGE("Failed to add requestId to errorData!");
        return HC_ERR_JSON_ADD;
    }
    return HC_SUCCESS;
}

static char *GetDuplicateAppId(const CJson *params)
{
    const char *appId = GetStringFromJson(params, FIELD_APP_ID);
    if (appId == NULL) {
        LOGE("Failed to get appId from json!");
        return NULL;
    }
    int32_t appIdLen = HcStrlen(appId);
    char *copyAppId = (char *)HcMalloc(appIdLen + 1, 0);
    if (copyAppId == NULL) {
        LOGE("Failed to allocate copyAppId memory!");
        return NULL;
    }
    if (memcpy_s(copyAppId, appIdLen + 1, appId, appIdLen) != EOK) {
        LOGE("Failed to copy appId!");
        HcFree(copyAppId);
        return NULL;
    }
    return copyAppId;
}

BindSession *CreateBaseBindSession(int32_t sessionType, int32_t opCode, const CJson *params,
    const DeviceAuthCallback *callback, ProcessSessionFunc func)
{
    int64_t reqId = DEFAULT_REQUEST_ID;
    if (GetInt64FromJson(params, FIELD_REQUEST_ID, &reqId) != HC_SUCCESS) {
        LOGE("Failed to get reqId from json!");
        return NULL;
    }
    BindSession *session = (BindSession *)HcMalloc(sizeof(BindSession), 0);
    if (session == NULL) {
        LOGE("Failed to allocate session memory!");
        return NULL;
    }
    int32_t res = GenerateSessionOrTaskId(&session->base.sessionId);
    if (res != HC_SUCCESS) {
        LOGE("Failed to generate session id! res: %d", res);
        HcFree(session);
        return NULL;
    }
    session->appId = GetDuplicateAppId(params);
    if (session->appId == NULL) {
        HcFree(session);
        return NULL;
    }
    int32_t osAccountId = 0;
    (void)GetIntFromJson(params, FIELD_OS_ACCOUNT_ID, &osAccountId);
    session->base.type = sessionType;
    session->base.process = func;
    session->base.destroy = DestroyBindSession;
    session->base.callback = callback;
    session->osAccountId = osAccountId;
    session->curTaskId = 0;
    session->opCode = opCode;
    session->moduleType = DAS_MODULE;
    session->reqId = reqId;
    session->channelType = NO_CHANNEL;
    session->channelId = DEFAULT_CHANNEL_ID;
    session->isWaiting = false;
    session->params = NULL;
    session->onChannelOpened = NULL;
    session->onConfirmed = NULL;
    return session;
}

void DestroyBindSession(Session *session)
{
    if (session == NULL) {
        return;
    }
    BindSession *realSession = (BindSession *)session;
    DestroyTask(realSession->curTaskId, realSession->moduleType);
    HcFree(realSession->appId);
    realSession->appId = NULL;
    FreeJson(realSession->params);
    realSession->params = NULL;
    HcFree(realSession);
    realSession = NULL;
}

void InitClientChannel(const DeviceAuthCallback *callback, const CJson *params, BindSession *session)
{
    session->channelType = GetChannelType(callback, params);
}

void InitServerChannel(const CJson *params, BindSession *session)
{
    int64_t channelId = DEFAULT_CHANNEL_ID;
    if (GetByteFromJson(params, FIELD_CHANNEL_ID, (uint8_t *)&channelId, sizeof(int64_t)) == HC_SUCCESS) {
        session->channelType = SOFT_BUS;
        session->channelId = channelId;
    } else {
        session->channelType = SERVICE_CHANNEL;
    }
}

bool IsAcceptRequest(const CJson *params)
{
    uint32_t confirmation = REQUEST_REJECTED;
    if (GetUnsignedIntFromJson(params, FIELD_CONFIRMATION, &confirmation) != HC_SUCCESS) {
        LOGE("Failed to get confimation from json!");
        return false;
    }
    return (confirmation == REQUEST_ACCEPTED);
}

int32_t CheckPeerStatus(const CJson *params, bool *isNeedInform)
{
    int32_t errorCode = HC_SUCCESS;
    if (GetIntFromJson(params, FIELD_GROUP_ERROR_MSG, &errorCode) == HC_SUCCESS) {
        LOGE("An error occurs in the peer device! [ErrorCode]: %d", errorCode);
        *isNeedInform = false;
        return errorCode;
    }
    return HC_SUCCESS;
}

int32_t SendBindSessionData(const BindSession *session, const CJson *sendData)
{
    char *sendDataStr = PackJsonToString(sendData);
    if (sendDataStr == NULL) {
        LOGE("An error occurred when converting json to string!");
        return HC_ERR_PACKAGE_JSON_TO_STRING_FAIL;
    }
    int32_t res = HcSendMsg(session->channelType, session->reqId, session->channelId,
        session->base.callback, sendDataStr);
    FreeJsonString(sendDataStr);
    if (res != HC_SUCCESS) {
        LOGE("Failed to send msg to peer device! res: %d", res);
        return res;
    }
    return HC_SUCCESS;
}

int32_t CreateAndProcessModule(BindSession *session, const CJson *in, CJson *out)
{
    int32_t status = 0;
    LOGI("Start to create and process module task! [ModuleType]: %d", session->moduleType);
    int32_t res = CreateTask(&(session->curTaskId), in, out, session->moduleType);
    if (res != HC_SUCCESS) {
        LOGE("Failed to create module task! res: %d", res);
        return res;
    }
    res = ProcessTask(session->curTaskId, in, out, &status, session->moduleType);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process module task! res: %d", res);
        return res;
    }
    LOGI("Create and process module task successfully!");
    return HC_SUCCESS;
}

int32_t ProcessModule(const BindSession *session, const CJson *in, CJson *out, int32_t *status)
{
    LOGI("Start to process module task! [ModuleType]: %d", session->moduleType);
    int32_t res = ProcessTask(session->curTaskId, in, out, status, session->moduleType);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process module task! res: %d", res);
        return res;
    }
    LOGI("Process module task successfully!");
    return HC_SUCCESS;
}

void InformPeerGroupErrorIfNeed(bool isNeedInform, int32_t errorCode, const BindSession *session)
{
    if (!isNeedInform) {
        return;
    }
    CJson *errorData = CreateJson();
    if (errorData == NULL) {
        LOGE("Failed to allocate errorData memory!");
        return;
    }
    int32_t res = GenerateGroupErrorMsg(errorCode, session, errorData);
    if (res != HC_SUCCESS) {
        FreeJson(errorData);
        return;
    }
    res = SendBindSessionData(session, errorData);
    FreeJson(errorData);
    if (res != HC_SUCCESS) {
        return;
    }
    LOGI("Notify the peer device that an error occurred at the local end successfully!");
}

void InformPeerModuleError(CJson *out, const BindSession *session)
{
    CJson *errorData = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (errorData == NULL) {
        return;
    }
    if (AddInt64StringToJson(errorData, FIELD_REQUEST_ID, session->reqId) != HC_SUCCESS) {
        LOGE("Failed to add requestId to errorData!");
        return;
    }
    int32_t result = SendBindSessionData(session, errorData);
    if (result != HC_SUCCESS) {
        LOGE("An error occurred when notifying the peer service!");
        return;
    }
    LOGI("Succeeded in notifying the peer device that an error occurred at the local end!");
}
