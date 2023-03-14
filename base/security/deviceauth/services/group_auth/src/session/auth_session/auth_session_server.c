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

#include "auth_session_server.h"
#include "auth_session_common.h"
#include "auth_session_common_util.h"
#include "auth_session_util.h"
#include "base_group_auth.h"
#include "common_defs.h"
#include "dev_auth_module_manager.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "os_account_adapter.h"
#include "session_common.h"

static int32_t ProcessServerAuthSession(Session *session, CJson *in);

static int32_t CombineServerParams(const CJson *confirmationJson, CJson *dataFromClient)
{
    int32_t authForm = AUTH_FORM_INVALID_TYPE;
    if (GetIntFromJson(dataFromClient, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to get auth form in data sent by client for the first time!");
        return HC_ERR_JSON_GET;
    }
    int32_t osAccountId = ANY_OS_ACCOUNT;
    (void)GetIntFromJson(confirmationJson, FIELD_OS_ACCOUNT_ID, &osAccountId);
    osAccountId = DevAuthGetRealOsAccountLocalId(osAccountId);
    if (osAccountId == INVALID_OS_ACCOUNT) {
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddIntToJson(dataFromClient, FIELD_OS_ACCOUNT_ID, osAccountId) != HC_SUCCESS) {
        LOGE("Failed to add os accountId for server in onRequest confirm!");
        return HC_ERR_JSON_ADD;
    }
    int32_t groupAuthType = GetGroupAuthType(authForm);
    BaseGroupAuth *groupAuthHandle = GetGroupAuth(groupAuthType);
    if (groupAuthHandle == NULL) {
        LOGE("Failed to get group auth handle!");
        return HC_ERR_NOT_SUPPORT;
    }
    int32_t res = groupAuthHandle->combineServerConfirmParams(confirmationJson, dataFromClient);
    if (res != HC_SUCCESS) {
        LOGE("Failed to combine server confirm params!");
    }
    return res;
}

static int32_t GetAuthInfoForServer(CJson *dataFromClient, ParamsVec *authParamsVec)
{
    int32_t authForm = AUTH_FORM_INVALID_TYPE;
    if (GetIntFromJson(dataFromClient, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to get auth form!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t groupAuthType = GetGroupAuthType(authForm);
    BaseGroupAuth *groupAuthHandle = GetGroupAuth(groupAuthType);
    if (groupAuthHandle == NULL) {
        LOGE("Failed to get group auth handle!");
        return HC_ERR_NOT_SUPPORT;
    }
    return groupAuthHandle->getAuthParamForServer(dataFromClient, authParamsVec);
}

static char *StartServerRequest(const CJson *dataFromClient, const DeviceAuthCallback *callback)
{
    CJson *reqParam = CreateJson();
    if (reqParam == NULL) {
        LOGE("Failed to create reqParam json!");
        return NULL;
    }
    char *confirmation = NULL;
    do {
        int32_t authForm = AUTH_FORM_INVALID_TYPE;
        if (GetIntFromJson(dataFromClient, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
            LOGE("Failed to get auth form!");
            break;
        }
        int32_t groupAuthType = GetGroupAuthType(authForm);
        BaseGroupAuth *groupAuthHandle = GetGroupAuth(groupAuthType);
        if (groupAuthHandle == NULL) {
            LOGE("Failed to get group auth handle!");
            break;
        }
        int32_t res = groupAuthHandle->getReqParams(dataFromClient, reqParam);
        if (res != HC_SUCCESS) {
            LOGE("Failed to get request params!");
            break;
        }
        confirmation = GetServerConfirmation(dataFromClient, reqParam, callback);
    } while (0);
    FreeJson(reqParam);
    return confirmation;
}

static int32_t AddAuthParamByRequest(CJson *dataFromClient, const DeviceAuthCallback *callback,
    ParamsVec *authParamsVec)
{
    char *confirmation = StartServerRequest(dataFromClient, callback);
    if (confirmation == NULL) {
        LOGE("Failed to get confirmation from server!");
        return HC_ERR_SERVER_CONFIRM_FAIL;
    }
    CJson *confirmationJson = CreateJsonFromString(confirmation);
    FreeJsonString(confirmation);
    if (confirmationJson == NULL) {
        LOGE("Failed to create json from string!");
        return HC_ERR_JSON_FAIL;
    }
    int32_t serverConfirm = REQUEST_ACCEPTED;
    (void)GetIntFromJson(confirmationJson, FIELD_CONFIRMATION, &serverConfirm);
    if ((uint32_t)serverConfirm == REQUEST_REJECTED) {
        LOGE("Server reject to response.");
        FreeJson(confirmationJson);
        return HC_ERR_REQ_REJECTED;
    }

    int32_t res = CombineServerParams(confirmationJson, dataFromClient);
    FreeJson(confirmationJson);
    if (res != HC_SUCCESS) {
        LOGE("Failed to combine server params!");
        return res;
    }

    res = GetAuthInfoForServer(dataFromClient, authParamsVec);
    if (res != HC_SUCCESS) {
        LOGE("Failed to fill device auth info for server!");
    }
    return res;
}

static int32_t ProcessServerAuthTask(AuthSession *session, int32_t moduleType, CJson *in, CJson *out)
{
    int32_t status = 0;
    CJson *paramInSession = (session->paramsList).get(&(session->paramsList), session->currentIndex);
    if (paramInSession == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    int32_t res = ProcessTask(session->curTaskId, in, out, &status, moduleType);
    DeleteItemFromJson(in, FIELD_PAYLOAD);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process task, res = %d!", res);
        if (InformAuthError(session, out, res) != HC_SUCCESS) {
            LOGE("Failed to inform auth error!");
        }
        return res;
    }
    res = ProcessTaskStatusForAuth(session, paramInSession, out, status);
    return res;
}

static int32_t StartServerAuthTask(AuthSession *session, const CJson *receivedData)
{
    CJson *paramInSession = (session->paramsList).get(&(session->paramsList), session->currentIndex);
    if (paramInSession == NULL) {
        LOGE("The json data in session is null!");
        return HC_ERR_NULL_PTR;
    }
    ProcessDeviceLevel(receivedData, paramInSession);
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json!");
        InformPeerAuthError(receivedData, session->base.callback);
        InformLocalAuthError(receivedData, session->base.callback);
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t status = 0;
    int32_t res = CreateAndProcessTask(session, paramInSession, out, &status);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process server auth task, res = %d!", res);
        if (InformAuthError(session, out, res) != HC_SUCCESS) {
            LOGE("Failed to inform auth error!");
        }
        FreeJson(out);
        return res;
    }
    res = ProcessTaskStatusForAuth(session, receivedData, out, status);
    FreeJson(out);
    return res;
}

static int32_t CheckServerGroupAuthMsg(const CJson *in, const CJson *paramInSession, const DeviceAuthCallback *callback)
{
    int32_t groupErrMsg = 0;
    if (GetIntFromJson(in, FIELD_GROUP_ERROR_MSG, &groupErrMsg) != HC_SUCCESS) {
        return HC_SUCCESS;
    }
    InformLocalAuthError(paramInSession, callback);
    return HC_ERR_PEER_ERROR;
}

static int32_t ProcessServerAuthSession(Session *session, CJson *in)
{
    LOGI("Begin process server authSession.");
    if ((session == NULL) || (in == NULL)) {
        LOGE("Invalid input params!");
        return HC_ERR_INVALID_PARAMS;
    }
    AuthSession *realSession = (AuthSession *)session;
    CJson *paramInSession = (realSession->paramsList).get(&(realSession->paramsList), realSession->currentIndex);
    if (paramInSession == NULL) {
        LOGE("Failed to get param in session!");
        return HC_ERR_NULL_PTR;
    }
    int32_t res = CheckServerGroupAuthMsg(in, paramInSession, realSession->base.callback);
    if (res != HC_SUCCESS) {
        LOGE("Peer device's group has error, so we stop server auth session!");
        return res;
    }
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json for out!");
        InformPeerAuthError(paramInSession, realSession->base.callback);
        InformLocalAuthError(paramInSession, realSession->base.callback);
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t moduleType = GetAuthModuleType(paramInSession);
    res = ProcessServerAuthTask(realSession, moduleType, in, out);
    FreeJson(out);
    if (res == FINISH) {
        LOGI("End process server authSession.");
    }
    return res;
}

static AuthSession *CreateServerAuthSessionInner(CJson *param, const DeviceAuthCallback *callback)
{
    ParamsVec authVec;
    CreateAuthParamsVec(&authVec);
    int32_t res = AddAuthParamByRequest(param, callback, &authVec);
    DeleteCachedData(param);
    if ((res != HC_SUCCESS) || (authVec.size(&authVec) == 0)) {
        LOGE("Failed to add auth param for server, res = %d, candidate auth group = %u!", res, authVec.size(&authVec));
        DestroyAuthParamsVec(&authVec);
        InformPeerAuthError(param, callback);
        InformLocalAuthError(param, callback);
        return NULL;
    }

    AuthSession *session = (AuthSession *)HcMalloc(sizeof(AuthSession), 0);
    if (session == NULL) {
        LOGE("Failed to malloc memory for session!");
        DestroyAuthParamsVec(&authVec);
        InformPeerAuthError(param, callback);
        InformLocalAuthError(param, callback);
        return NULL;
    }
    session->base.process = ProcessServerAuthSession;
    session->base.destroy = DestroyAuthSession;
    session->base.callback = callback;
    session->currentIndex = 0;
    session->paramsList = authVec;
    res = GenerateSessionOrTaskId(&session->base.sessionId);
    if (res != HC_SUCCESS) {
        LOGE("Failed to generate session id!");
        DestroyAuthSession((Session *)session);
        InformPeerAuthError(param, callback);
        InformLocalAuthError(param, callback);
        return NULL;
    }

    res = StartServerAuthTask(session, param);
    if (res != HC_SUCCESS) {
        LOGE("Failed to start server auth task!");
        DestroyAuthSession((Session *)session);
        return NULL;
    }
    return (AuthSession *)session;
}

Session *CreateServerAuthSession(CJson *param, const DeviceAuthCallback *callback)
{
    AuthSession *session = CreateServerAuthSessionInner(param, callback);
    if (session == NULL) {
        LOGE("Failed to create server auth session!");
        return NULL;
    }
    return (Session *)session;
}
