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

#include "auth_session_common_lite.h"
#include "common_defs.h"
#include "das_module_defines.h"
#include "dev_auth_module_manager.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "session_common.h"

static void ReturnFinishDataLite(const CJson *in, const DeviceAuthCallback *callback, const CJson *out);

static int32_t ProcessAuthTaskLite(AuthSessionLite *session, int32_t moduleType, CJson *in, CJson *out)
{
    int32_t status = 0;
    int32_t res = ProcessTask(session->curTaskId, in, out, &status, moduleType);
    if (res != HC_SUCCESS) {
        InformAuthErrorLite(in, session->base.callback, out, res);
        return res;
    }
    return ProcessLiteTaskStatusForAuth(session, out, status);
}

static int32_t GetAuthModuleTypeLite(const CJson *in)
{
    int32_t authForm = AUTH_FORM_INVALID_TYPE;
    if (GetIntFromJson(in, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to get auth form!");
        return INVALID_MODULE_TYPE;
    }
    if (authForm == AUTH_FORM_ACCOUNT_UNRELATED) {
        return DAS_MODULE;
    } else if ((authForm == AUTH_FORM_IDENTICAL_ACCOUNT) || (authForm == AUTH_FORM_ACROSS_ACCOUNT)) {
        return ACCOUNT_MODULE;
    }
    LOGE("Invalid authForm for repeated payload check in auth session lite!");
    return INVALID_MODULE_TYPE;
}

static int ProcessAuthSessionLite(Session *session, CJson *in)
{
    LOGD("Begin process authSession lite.");
    if ((session == NULL) || (in == NULL)) {
        LOGE("Invalid input params!");
        return HC_ERR_INVALID_PARAMS;
    }
    CJson *out = NULL;
    AuthSessionLite *realSession = (AuthSessionLite *)session;
    out = CreateJson();
    if (out == NULL) {
        LOGE("Failed to create json!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = ProcessAuthTaskLite(realSession, GetAuthModuleTypeLite(realSession->authParams), in, out);
    FreeJson(out);
    LOGI("End process authSession lite, res = %d.", res);
    return res;
}

static void ReturnErrorDataLite(const CJson *in, const DeviceAuthCallback *callback, const CJson *out, int errorCode)
{
    int64_t requestId = 0;
    int32_t authForm = 0;
    if (GetByteFromJson(in, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to add request id!");
        return;
    }
    if (GetIntFromJson(in, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to add auth form!");
        return;
    }
    char *returnStr = PackJsonToString(out);
    if (returnStr == NULL) {
        LOGE("Failed to convert json: out to string!");
        return;
    }
    if ((callback != NULL) && (callback->onError != NULL)) {
        LOGE("Begin to invoke onError for lite auth!");
        callback->onError(requestId, authForm, errorCode, returnStr);
    }
    FreeJsonString(returnStr);
}

static int32_t ReturnSessionKeyLite(int64_t requestId, const CJson *authParams,
    const CJson *out, const DeviceAuthCallback *callback)
{
    int32_t keyLen = DEFAULT_RETURN_KEY_LENGTH;
    (void)GetIntFromJson(authParams, FIELD_KEY_LENGTH, &keyLen);
    uint8_t *sessionKey = (uint8_t *)HcMalloc(keyLen, 0);
    if (sessionKey == NULL) {
        LOGE("Failed to allocate memory for sessionKey!");
        return HC_ERR_ALLOC_MEMORY;
    }
    int32_t res = HC_SUCCESS;
    do {
        if (GetByteFromJson(out, FIELD_SESSION_KEY, sessionKey, keyLen) != HC_SUCCESS) {
            LOGE("Failed to get sessionKey!");
            res = HC_ERR_JSON_GET;
            break;
        }
        if ((callback == NULL) || (callback->onSessionKeyReturned == NULL)) {
            LOGE("The callback of onSessionKeyReturned is null!");
            res = HC_ERR_JSON_GET;
            break;
        }
        LOGD("Begin to invoke onSessionKeyReturned for lite auth.");
        callback->onSessionKeyReturned(requestId, sessionKey, keyLen);
    } while (0);
    (void)memset_s(sessionKey, keyLen, 0, keyLen);
    HcFree(sessionKey);
    sessionKey = NULL;
    return res;
}

static void ReturnFinishDataLiteInner(int64_t requestId, const CJson *in, const DeviceAuthCallback *callback,
    const CJson *out)
{
    int32_t authForm = AUTH_FORM_INVALID_TYPE;
    if (GetIntFromJson(in, FIELD_AUTH_FORM, &authForm) != HC_SUCCESS) {
        LOGE("Failed to get auth type!");
        return;
    }
    const CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer != NULL) {
        char *sendToPeerStr = PackJsonToString(sendToPeer);
        if (sendToPeerStr == NULL) {
            LOGE("Failed to pack sendToPeerStr for onTransmit!");
            return;
        }
        if ((callback != NULL) && (callback->onTransmit != NULL)) {
            LOGD("Begin to invoke onTransmit for lite auth!");
            if (!callback->onTransmit(requestId, (uint8_t *)sendToPeerStr, (uint32_t)strlen(sendToPeerStr) + 1)) {
                LOGE("Failed to transmit data to peer!");
                FreeJsonString(sendToPeerStr);
                return;
            }
            LOGD("End to invoke onTransmit for lite auth!");
        }
        FreeJsonString(sendToPeerStr);
    }
    CJson *sendToSelf = GetObjFromJson(out, FIELD_SEND_TO_SELF);
    if (sendToSelf == NULL) {
        return;
    }
    char *returnStr = PackJsonToString(sendToSelf);
    if (returnStr == NULL) {
        LOGE("Failed to pack returnStr for onFinish!");
        return;
    }
    if ((callback != NULL) && (callback->onFinish != NULL)) {
        LOGD("Begin to invoke onFinish for lite auth.");
        callback->onFinish(requestId, authForm, returnStr);
    }
    ClearAndFreeJsonString(returnStr);
}

static void ReturnFinishDataLite(const CJson *in, const DeviceAuthCallback *callback, const CJson *out)
{
    if (out == NULL) {
        LOGE("No data for finish return!");
        return;
    }
    int64_t requestId = 0;
    if (GetByteFromJson(in, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return;
    }
    if (ReturnSessionKeyLite(requestId, in, out, callback) != HC_SUCCESS) {
        LOGE("Failed to return session key!");
        return;
    }
    ReturnFinishDataLiteInner(requestId, in, callback, out);
}

int32_t ReturnTransmitDataLite(const AuthSessionLite *session, CJson *out)
{
    CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer == NULL) {
        LOGI("The transmit data to peer is null in lite auth session!");
        return HC_ERR_JSON_GET;
    }
    int32_t ret = HC_SUCCESS;
    const DeviceAuthCallback *callback = session->base.callback;
    int64_t requestId = 0;
    if (GetByteFromJson(session->authParams, FIELD_REQUEST_ID,
        (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return HC_ERR_JSON_GET;
    }
    char *outStr = PackJsonToString(sendToPeer);
    if (outStr == NULL) {
        LOGE("Failed to pack outStr for onTransmit!");
        return HC_ERR_ALLOC_MEMORY;
    }
    do {
        if ((callback == NULL) || (callback->onTransmit == NULL)) {
            LOGE("The callback for transmit is null!");
            ret = HC_ERR_NULL_PTR;
            break;
        }
        LOGD("Begin to invoke onTransmit for lite auth.");
        if (!callback->onTransmit(requestId, (uint8_t *)outStr, (uint32_t)strlen(outStr) + 1)) {
            LOGE("Failed to transmit data to peer!");
            ret = HC_ERR_TRANSMIT_FAIL;
        }
        LOGD("End to invoke onTransmit for lite auth.");
    } while (0);
    FreeJsonString(outStr);
    return ret;
}

void InformLocalAuthErrorLite(const CJson *authParam, const DeviceAuthCallback *callback)
{
    int64_t requestId = 0;
    if (GetByteFromJson(authParam, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return;
    }
    if ((callback != NULL) && (callback->onError != NULL)) {
        LOGE("Begin to invoke onError for lite auth.");
        callback->onError(requestId, AUTH_FORM_INVALID_TYPE, HC_ERR_CREATE_SESSION_FAIL, NULL);
    }
}

void InformPeerAuthErrorLite(const CJson *in, const DeviceAuthCallback *callback)
{
    bool isClient = false;
    (void)GetBoolFromJson(in, FIELD_IS_CLIENT, &isClient);
    if (isClient) {
        return;
    }
    int32_t message = 0;
    (void)GetIntFromJson(in, FIELD_MESSAGE, &message);
    if (message == ERR_MESSAGE) {
        LOGI("Peer send error msg, no need to let peer device process error twice!");
        return;
    }
    int64_t requestId = 0;
    if (GetByteFromJson(in, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request ID!");
        return;
    }
    CJson *errorToPeer = CreateJson();
    if (errorToPeer == NULL) {
        LOGE("Failed to allocate memory!");
        return;
    }
    if (AddIntToJson(errorToPeer, FIELD_MESSAGE, ERR_MESSAGE) != HC_SUCCESS) {
        LOGE("Failed to add errMessage to json!");
        FreeJson(errorToPeer);
        return;
    }
    char *errorToPeerStr = PackJsonToString(errorToPeer);
    FreeJson(errorToPeer);
    if (errorToPeerStr == NULL) {
        LOGE("Failed to pack errorToPeer to string!");
        return;
    }
    if ((callback != NULL) && (callback->onTransmit != NULL)) {
        LOGD("Begin to invoke onTransmit for lite auth.");
        (void)callback->onTransmit(requestId, (uint8_t *)errorToPeerStr, (uint32_t)strlen(errorToPeerStr) + 1);
    }
    FreeJsonString(errorToPeerStr);
}

void InformAuthErrorLite(const CJson *in, const DeviceAuthCallback *callback, const CJson *out, int errorCode)
{
    bool isClient = false;
    (void)GetBoolFromJson(in, FIELD_IS_CLIENT, &isClient);
    const CJson *sendToSelf = GetObjFromJson(out, FIELD_SEND_TO_SELF);
    if (sendToSelf != NULL) {
        ReturnErrorDataLite(in, callback, sendToSelf, errorCode);
    }
    const CJson *sendToPeer = GetObjFromJson(out, FIELD_SEND_TO_PEER);
    if (sendToPeer == NULL) {
        return;
    }
    int64_t requestId = 0;
    if (GetByteFromJson(in, FIELD_REQUEST_ID, (uint8_t *)&requestId, sizeof(int64_t)) != HC_SUCCESS) {
        LOGE("Failed to get request id!");
        return;
    }
    char *sendToPeerStr = PackJsonToString(sendToPeer);
    if (sendToPeerStr == NULL) {
        LOGE("Failed to pack json to string!");
        return;
    }
    if ((callback != NULL) && (callback->onTransmit != NULL)) {
        LOGD("Begin to invoke onTransmit for lite auth in InformAuthErrorLite.");
        (void)callback->onTransmit(requestId, (uint8_t *)sendToPeerStr, (uint32_t)strlen(sendToPeerStr) + 1);
    }
    FreeJsonString(sendToPeerStr);
}

int32_t CreateAndProcessLiteTask(AuthSessionLite *session, CJson *out, int32_t *status)
{
    int32_t moduleType = GetAuthModuleTypeLite(session->authParams);
    session->curTaskId = 0;
    int32_t res = CreateTask(&(session->curTaskId), session->authParams, out, moduleType);
    if (res != HC_SUCCESS) {
        LOGE("Failed to create task for auth!");
        return res;
    }
    res = ProcessTask(session->curTaskId, session->authParams, out, status, moduleType);
    if (res != HC_SUCCESS) {
        LOGE("Failed to process task for auth!");
        return res;
    }
    return HC_SUCCESS;
}

int32_t ProcessLiteTaskStatusForAuth(const AuthSessionLite *session, CJson *out, int32_t status)
{
    int32_t res = HC_SUCCESS;
    switch (status) {
        case IGNORE_MSG:
            LOGD("Ignore this msg.");
            break;
        case CONTINUE:
            res = ReturnTransmitDataLite(session, out);
            if (res != HC_SUCCESS) {
                LOGE("Failed to transmit lite auth data to peer!");
                InformLocalAuthErrorLite(session->authParams, session->base.callback);
            }
            break;
        case FINISH:
            ReturnFinishDataLite(session->authParams, session->base.callback, out);
            ClearSensitiveStringInJson(out, FIELD_SESSION_KEY);
            res = FINISH;
            break;
        default:
            LOGE("Invalid status after process task!");
            res = HC_ERR_INVALID_PARAMS;
            break;
    }
    return res;
}

void DestroyAuthSessionLite(Session *session)
{
    if (session == NULL) {
        return;
    }
    AuthSessionLite *realSession = (AuthSessionLite *)session;
    FreeJson(realSession->authParams);
    HcFree(realSession);
    realSession = NULL;
}

AuthSessionLite *InitAuthSessionLite(const CJson *in, const DeviceAuthCallback *callback)
{
    AuthSessionLite *session = (AuthSessionLite *)HcMalloc(sizeof(AuthSessionLite), 0);
    if (session == NULL) {
        LOGE("Failed to allocate memory for session!");
        return NULL;
    }
    session->base.process = ProcessAuthSessionLite;
    session->base.destroy = DestroyAuthSessionLite;
    session->base.callback = callback;
    session->authParams = DuplicateJson(in);
    if (session->authParams == NULL) {
        LOGE("Failed to duplicate param!");
        DestroyAuthSessionLite((Session *)session);
        return NULL;
    }
    int32_t res = GenerateSessionOrTaskId(&session->base.sessionId);
    if (res != HC_SUCCESS) {
        LOGE("Failed to generate session id!");
        DestroyAuthSessionLite((Session *)session);
        return NULL;
    }
    return session;
}