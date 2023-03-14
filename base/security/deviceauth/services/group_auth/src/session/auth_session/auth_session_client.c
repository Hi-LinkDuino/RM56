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

#include "auth_session_client.h"
#include "auth_session_common.h"
#include "auth_session_util.h"
#include "dev_auth_module_manager.h"
#include "hc_log.h"
#include "hc_types.h"
#include "json_utils.h"
#include "session_common.h"
#include "string_util.h"

static int32_t ProcessClientAuthSession(Session *session, CJson *in);

int32_t CheckInputAuthParams(const CJson *authParam)
{
    int32_t keyLen = DEFAULT_RETURN_KEY_LENGTH;
    (void)GetIntFromJson(authParam, FIELD_KEY_LENGTH, &keyLen);
    if ((keyLen < MIN_KEY_LENGTH) || (keyLen > MAX_KEY_LENGTH)) {
        LOGE("The key length is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    const char *pkgName = GetStringFromJson(authParam, FIELD_SERVICE_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Pkg name is null, the input is invalid!");
        return HC_ERR_INVALID_PARAMS;
    }
    return HC_SUCCESS;
}

static int32_t ProcessClientAuthTask(AuthSession *session, int32_t moduleType, CJson *in, CJson *out)
{
    int32_t status = 0;
    CJson *paramInSession = (session->paramsList).get(&(session->paramsList), session->currentIndex);
    if (paramInSession == NULL) {
        LOGE("Failed to get param in session!");
        return HC_ERR_NULL_PTR;
    }
    int32_t res = ProcessTask(session->curTaskId, in, out, &status, moduleType);
    DeleteItemFromJson(in, FIELD_PAYLOAD);
    if (res != HC_SUCCESS) {
        DestroyTask(session->curTaskId, moduleType);
        return InformAuthError(session, out, res);
    }
    return ProcessTaskStatusForAuth(session, paramInSession, out, status);
}

static int32_t StartClientAuthTask(AuthSession *session)
{
    CJson *paramInSession = (session->paramsList).get(&(session->paramsList), session->currentIndex);
    if (paramInSession == NULL) {
        LOGE("Failed to get param in session!");
        return HC_ERR_NULL_PTR;
    }
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json!");
        InformLocalAuthError(paramInSession, session->base.callback);
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t status = 0;
    int32_t res = CreateAndProcessTask(session, paramInSession, out, &status);
    if (res != HC_SUCCESS) {
        res = InformAuthError(session, out, res);
        FreeJson(out);
        LOGD("Start process client auth task, res = %d.", res);
        return res;
    }
    res = ProcessTaskStatusForAuth(session, paramInSession, out, status);
    FreeJson(out);
    return res;
}

int32_t CheckClientGroupAuthMsg(AuthSession *session, const CJson *in)
{
    int32_t groupErrMsg = 0;
    if (GetIntFromJson(in, FIELD_GROUP_ERROR_MSG, &groupErrMsg) != HC_SUCCESS) {
        return HC_SUCCESS;
    }
    CJson *outData = CreateJson();
    if (outData == NULL) {
        LOGE("Failed to malloc for outData!");
        return HC_ERR_ALLOC_MEMORY;
    }
    if (AddIntToJson(outData, FIELD_GROUP_ERROR_MSG, groupErrMsg) != HC_SUCCESS) {
        LOGE("Failed to add info to outData!");
        FreeJson(outData);
        return HC_ERR_JSON_FAIL;
    }
    if (InformAuthError(session, outData, HC_ERR_PEER_ERROR) != HC_SUCCESS) {
        LOGE("Failed to inform auth error!");
    }
    FreeJson(outData);
    return HC_ERR_PEER_ERROR;
}

static void PrintErrorInputInfo(const CJson *param)
{
    const char *peerUdid = GetStringFromJson(param, FIELD_PEER_CONN_DEVICE_ID);
    const char *peerAuthId = GetStringFromJson(param, FIELD_PEER_AUTH_ID);
    char *anonyPeerUdid = NULL;
    char *anonyPeerAuthId = NULL;
    ConvertToAnonymousStr(peerUdid, &anonyPeerUdid);
    ConvertToAnonymousStr(peerAuthId, &anonyPeerAuthId);
    LOGE("[PrintErrorInputInfo] [peerUdid]: %s", ((anonyPeerUdid == NULL) ? "NULL" : anonyPeerUdid));
    LOGE("[PrintErrorInputInfo] [peerAuthId]: %s", ((anonyPeerAuthId == NULL) ? "NULL" : anonyPeerAuthId));
    HcFree(anonyPeerUdid);
    HcFree(anonyPeerAuthId);
}

static int32_t ProcessClientAuthSession(Session *session, CJson *in)
{
    LOGI("Begin process client authSession.");
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
    ProcessDeviceLevel(in, paramInSession);
    int32_t res = CheckClientGroupAuthMsg(realSession, in);
    if (res != HC_SUCCESS) {
        LOGE("Peer device's group has error, so we stop client auth session!");
        return res;
    }
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json for out!");
        InformPeerAuthError(paramInSession, realSession->base.callback);
        InformLocalAuthError(paramInSession, realSession->base.callback);
        return HC_ERR_ALLOC_MEMORY;
    }
    res = ProcessClientAuthTask(realSession, GetAuthModuleType(paramInSession), in, out);
    ClearSensitiveStringInJson(out, FIELD_SESSION_KEY);
    FreeJson(out);
    if (res == FINISH) {
        LOGD("End process client authSession.");
    }
    return res;
}

static Session *CreateClientAuthSessionInner(int32_t osAccountId, CJson *param, const DeviceAuthCallback *callback)
{
    ParamsVec authParamsVec;
    CreateAuthParamsVec(&authParamsVec);
    int32_t res = GetAuthParamsList(osAccountId, param, &authParamsVec);
    if ((res != HC_SUCCESS) || (authParamsVec.size(&authParamsVec) == 0)) {
        LOGE("Failed to get auth param list, candidate group = %u!", authParamsVec.size(&authParamsVec));
        DestroyAuthParamsVec(&authParamsVec);
        InformLocalAuthError(param, callback);
        PrintErrorInputInfo(param);
        return NULL;
    }

    AuthSession *session = (AuthSession *)HcMalloc(sizeof(AuthSession), 0);
    if (session == NULL) {
        LOGE("Failed to allocate memory for session!");
        DestroyAuthParamsVec(&authParamsVec);
        InformLocalAuthError(param, callback);
        return NULL;
    }
    session->base.process = ProcessClientAuthSession;
    session->base.destroy = DestroyAuthSession;
    session->base.callback = callback;
    session->currentIndex = 0;
    session->paramsList = authParamsVec;
    res = GenerateSessionOrTaskId(&session->base.sessionId);
    if (res != HC_SUCCESS) {
        LOGE("Failed to generate session id!");
        DestroyAuthSession((Session *)session);
        InformLocalAuthError(param, callback);
        return NULL;
    }

    res = StartClientAuthTask(session);
    if (res != HC_SUCCESS) {
        DestroyAuthSession((Session *)session);
        return NULL;
    }
    return (Session *)session;
}

Session *CreateClientAuthSession(CJson *param, const DeviceAuthCallback *callback)
{
    LOGD("Begin create client authSession.");
    Session *session = NULL;
    if (CheckInputAuthParams(param) != HC_SUCCESS) {
        LOGE("Invalid input params!");
        InformLocalAuthError(param, callback);
        return NULL;
    }
    if (AddIntToJson(param, FIELD_OPERATION_CODE, AUTHENTICATE) != HC_SUCCESS) {
        LOGE("Failed to add operation code to json!");
        InformLocalAuthError(param, callback);
        return NULL;
    }
    int32_t osAccountId = INVALID_OS_ACCOUNT;
    if (GetIntFromJson(param, FIELD_OS_ACCOUNT_ID, &osAccountId) != HC_SUCCESS) {
        LOGE("Failed to get osAccountId for create client session!");
        InformLocalAuthError(param, callback);
        return NULL;
    }
    session = CreateClientAuthSessionInner(osAccountId, param, callback);
    if (session == NULL) {
        LOGE("Failed to create client auth session!");
        return NULL;
    }
    LOGD("End create client authSession.");
    return session;
}
