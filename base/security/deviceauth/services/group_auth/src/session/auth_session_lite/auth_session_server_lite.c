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

#include "auth_session_server_lite.h"
#include "auth_session_common_util.h"
#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "json_utils.h"

static int32_t StartServerAuthLiteTask(AuthSessionLite *session)
{
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json!");
        InformLocalAuthErrorLite(session->authParams, session->base.callback);
        InformPeerAuthErrorLite(session->authParams, session->base.callback);
        return HC_ERR_ALLOC_MEMORY;
    }

    const char *pkgName = GetStringFromJson(session->authParams, FIELD_SERVICE_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Pkg name is null, the input is invalid!");
        FreeJson(out);
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddStringToJson(session->authParams, FIELD_PKG_NAME, pkgName) != HC_SUCCESS) {
        LOGE("Failed to add pkg name for server!");
        FreeJson(out);
        InformPeerAuthErrorLite(session->authParams, session->base.callback);
        InformLocalAuthErrorLite(session->authParams, session->base.callback);
        return HC_ERR_JSON_FAIL;
    }

    int32_t status = 0;
    int32_t res = CreateAndProcessLiteTask(session, out, &status);
    DeleteItemFromJson(session->authParams, FIELD_PAYLOAD);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process for server auth lite session!");
        InformAuthErrorLite(session->authParams, session->base.callback, out, res);
        FreeJson(out);
        return res;
    }
    res = ProcessLiteTaskStatusForAuth(session, out, status);
    FreeJson(out);
    return res;
}

static int32_t CombineServerParamsLite(const CJson *confirmationJson, CJson *dataFromClient)
{
    const char *pkgName = GetStringFromJson(confirmationJson, FIELD_SERVICE_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Failed to get pkgName from confirmation data!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(dataFromClient, FIELD_SERVICE_PKG_NAME, pkgName) != HC_SUCCESS) {
        LOGE("Failed to combine server param with pkgName!");
        return HC_ERR_JSON_FAIL;
    }
    const char *selfId = GetStringFromJson(confirmationJson, FIELD_SELF_AUTH_ID);
    if (selfId != NULL) {
        LOGD("Begin to add selfId for server auth!");
        if (AddStringToJson(dataFromClient, FIELD_SELF_AUTH_ID, selfId) != HC_SUCCESS) {
            LOGE("Failed to combine server param with selfId!");
            return HC_ERR_JSON_FAIL;
        }
    }

    int32_t selfType;
    if (GetIntFromJson(confirmationJson, FIELD_SELF_TYPE, &selfType) == HC_SUCCESS) {
        LOGD("Begin to add selfId for server auth!");
        if (AddIntToJson(dataFromClient, FIELD_SELF_TYPE, selfType) != HC_SUCCESS) {
            LOGE("Failed to combine server param with selfType!");
            return HC_ERR_JSON_FAIL;
        }
    }

    const char *serviceType = GetStringFromJson(confirmationJson, FIELD_SERVICE_TYPE);
    if (serviceType == NULL) {
        LOGE("Failed to get serviceType from confirmation data!");
        return HC_ERR_JSON_GET;
    }
    if (AddStringToJson(dataFromClient, FIELD_SERVICE_TYPE, serviceType) != HC_SUCCESS) {
        LOGE("Failed to combine server param with serviceType!");
        return HC_ERR_JSON_FAIL;
    }
    return HC_SUCCESS;
}

static char *StartServerRequestLite(const CJson *in, const DeviceAuthCallback *callback)
{
    CJson *reqParam = CreateJson();
    if (reqParam == NULL) {
        LOGE("Failed to create reqParam json!");
        return NULL;
    }
    int32_t res = GetGeneralReqParams(in, reqParam);
    if (res != HC_SUCCESS) {
        LOGE("Failed to get request params!");
        FreeJson(reqParam);
        return NULL;
    }
    char *confirmation = GetServerConfirmation(in, reqParam, callback);
    FreeJson(reqParam);
    return confirmation;
}

static int32_t AddAuthParamByRequestLite(CJson *in, const DeviceAuthCallback *callback)
{
    char *confirmation = StartServerRequestLite(in, callback);
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
    int serverConfirm = REQUEST_ACCEPTED;
    (void)GetIntFromJson(confirmationJson, FIELD_CONFIRMATION, &serverConfirm);
    if ((uint32_t)serverConfirm == REQUEST_REJECTED) {
        LOGE("Server reject to response!");
        FreeJson(confirmationJson);
        return HC_ERR_REQ_REJECTED;
    }
    int32_t res = CombineServerParamsLite(confirmationJson, in);
    FreeJson(confirmationJson);
    if (res != HC_SUCCESS) {
        LOGE("Failed to combine server params!");
        return res;
    }
    return res;
}

static Session *CreateServerAuthSessionLiteInner(CJson *in, const DeviceAuthCallback *callback)
{
    int32_t res = AddAuthParamByRequestLite(in, callback);
    if (res != HC_SUCCESS) {
        LOGE("Failed to add auth params by request!");
        return NULL;
    }
    AuthSessionLite *session = InitAuthSessionLite(in, callback);
    if (session == NULL) {
        LOGE("Failed to initial session!");
        InformLocalAuthErrorLite(in, callback);
        InformPeerAuthErrorLite(in, callback);
        return NULL;
    }
    res = StartServerAuthLiteTask(session);
    if (res != HC_SUCCESS) {
        DestroyAuthSessionLite((Session *)session);
        return NULL;
    }
    return (Session *)session;
}

Session *CreateServerAuthSessionLite(CJson *in, const DeviceAuthCallback *callback)
{
    if (AddBoolToJson(in, FIELD_IS_CLIENT, false) != HC_SUCCESS) {
        LOGE("Failed to add isClient to json for server auth!");
        InformLocalAuthErrorLite(in, callback);
        InformPeerAuthErrorLite(in, callback);
        return NULL;
    }
    if (AddIntToJson(in, FIELD_OPERATION_CODE, AUTHENTICATE) != HC_SUCCESS) {
        LOGE("Failed to add operation code to json!");
        InformLocalAuthErrorLite(in, callback);
        InformPeerAuthErrorLite(in, callback);
        return NULL;
    }
    Session *session = CreateServerAuthSessionLiteInner(in, callback);
    if (session == NULL) {
        LOGE("Failed to create lite server auth session!");
        return NULL;
    }
    return session;
}