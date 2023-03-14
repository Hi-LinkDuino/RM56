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

#include "auth_session_client_lite.h"
#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "json_utils.h"

static int32_t StartClientAuthLiteTask(AuthSessionLite *session)
{
    CJson *out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json!");
        InformLocalAuthErrorLite(session->authParams, session->base.callback);
        return HC_ERR_ALLOC_MEMORY;
    }
    const char *pkgName = GetStringFromJson(session->authParams, FIELD_SERVICE_PKG_NAME);
    if (pkgName == NULL) {
        LOGE("Pkg name is null, the input is invalid!");
        FreeJson(out);
        InformLocalAuthErrorLite(session->authParams, session->base.callback);
        return HC_ERR_INVALID_PARAMS;
    }
    if (AddStringToJson(session->authParams, FIELD_PKG_NAME, pkgName) != HC_SUCCESS) {
        LOGE("Failed to add pkg name for client!");
        FreeJson(out);
        InformLocalAuthErrorLite(session->authParams, session->base.callback);
        return HC_ERR_JSON_FAIL;
    }

    int32_t status = 0;
    int32_t res = CreateAndProcessLiteTask(session, out, &status);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process for client auth session lite!");
        InformAuthErrorLite(session->authParams, session->base.callback, out, res);
        FreeJson(out);
        return res;
    }
    res = ProcessLiteTaskStatusForAuth(session, out, status);
    FreeJson(out);
    return res;
}

static Session *CreateClientAuthSessionLiteInner(const CJson *in, const DeviceAuthCallback *callback)
{
    AuthSessionLite *session = InitAuthSessionLite(in, callback);
    if (session == NULL) {
        LOGE("Failed to initial session!");
        InformLocalAuthErrorLite(in, callback);
        return NULL;
    }
    int32_t res = StartClientAuthLiteTask(session);
    if (res != HC_SUCCESS) {
        DestroyAuthSessionLite((Session *)session);
        return NULL;
    }
    return (Session *)session;
}

Session *CreateClientAuthSessionLite(CJson *in, const DeviceAuthCallback *callback)
{
    if (AddBoolToJson(in, FIELD_IS_CLIENT, true) != HC_SUCCESS) {
        LOGE("Failed to add isClient to json!");
        InformLocalAuthErrorLite(in, callback);
        return NULL;
    }
    if (AddIntToJson(in, FIELD_OPERATION_CODE, AUTHENTICATE) != HC_SUCCESS) {
        LOGE("Failed to add operation code to json!");
        InformLocalAuthErrorLite(in, callback);
        return NULL;
    }
    Session *session = CreateClientAuthSessionLiteInner(in, callback);
    if (session == NULL) {
        LOGE("Failed to create lite client auth session!");
        return NULL;
    }
    return session;
}
