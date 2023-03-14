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

#include "session_manager.h"
#include "auth_session_client.h"
#include "auth_session_client_lite.h"
#include "auth_session_server.h"
#include "auth_session_server_lite.h"
#include "bind_session_client.h"
#include "bind_session_client_lite.h"
#include "bind_session_server.h"
#include "bind_session_server_lite.h"
#include "common_defs.h"
#include "device_auth.h"
#include "device_auth_defines.h"
#include "hc_dev_info.h"
#include "hc_log.h"
#include "hc_time.h"
#include "hc_vector.h"
#include "key_agree_session_client.h"
#include "key_agree_session_server.h"

typedef struct {
    int64_t requestId;
    int32_t type;
    int64_t sessionId;
} RequestInfo;

DECLARE_HC_VECTOR(SessionManagerVec, void *);
IMPLEMENT_HC_VECTOR(SessionManagerVec, void *, 1)

DECLARE_HC_VECTOR(RequestInfoVec, RequestInfo);
IMPLEMENT_HC_VECTOR(RequestInfoVec, RequestInfo, 1)

static SessionManagerVec g_sessionManagerVec;
static RequestInfoVec g_requestVec;

typedef Session *(*CreateSessionFunc)(CJson *, const DeviceAuthCallback *);

typedef struct SessionManagerInfoStruct {
    SessionTypeValue sessionType;
    int requestType; /* only BIND_TYPE or AUTH_TYPE currently */
    CreateSessionFunc createSessionFunc;
} SessionManagerInfo;

static const SessionManagerInfo SESSION_MANAGER_INFO[] = {
    { TYPE_CLIENT_BIND_SESSION, BIND_TYPE, CreateClientBindSession },
    { TYPE_SERVER_BIND_SESSION, BIND_TYPE, CreateServerBindSession },
    { TYPE_CLIENT_AUTH_SESSION, AUTH_TYPE, CreateClientAuthSession },
    { TYPE_SERVER_AUTH_SESSION, AUTH_TYPE, CreateServerAuthSession },
    { TYPE_CLIENT_BIND_SESSION_LITE, BIND_TYPE, CreateLiteClientBindSession },
    { TYPE_SERVER_BIND_SESSION_LITE, BIND_TYPE, CreateLiteServerBindSession },
    { TYPE_CLIENT_AUTH_SESSION_LITE, AUTH_TYPE, CreateClientAuthSessionLite },
    { TYPE_SERVER_AUTH_SESSION_LITE, AUTH_TYPE, CreateServerAuthSessionLite },
    { TYPE_CLIENT_KEY_AGREE_SESSION, BIND_TYPE, CreateClientKeyAgreeSession },
    { TYPE_SERVER_KEY_AGREE_SESSION, BIND_TYPE, CreateServerKeyAgreeSession }
};

static int32_t GetSessionId(int64_t requestId, int64_t *sessionId)
{
    uint32_t index;
    RequestInfo *requestInfo = NULL;
    FOR_EACH_HC_VECTOR(g_requestVec, index, requestInfo) {
        if ((requestInfo != NULL) && (requestInfo->requestId == requestId)) {
            *sessionId = requestInfo->sessionId;
            return HC_SUCCESS;
        }
    }
    return HC_ERR_REQUEST_NOT_FOUND;
}

static int32_t GetSessionIdByType(int64_t requestId, int32_t type, int64_t *sessionId)
{
    uint32_t index;
    RequestInfo *requestInfo = NULL;
    FOR_EACH_HC_VECTOR(g_requestVec, index, requestInfo) {
        if ((requestInfo != NULL) && (requestInfo->requestId == requestId)) {
            if (requestInfo->type != type) {
                LOGE("RequestId is match but type not match");
                return HC_ERR_REQUEST_NOT_FOUND;
            }
            *sessionId = requestInfo->sessionId;
            return HC_SUCCESS;
        }
    }
    return HC_ERR_REQUEST_NOT_FOUND;
}

static void DestroyRequest(int64_t requestId)
{
    uint32_t index;
    RequestInfo *request = NULL;
    FOR_EACH_HC_VECTOR(g_requestVec, index, request) {
        if ((request != NULL) && (request->requestId == requestId)) {
            RequestInfo tempRequest;
            HC_VECTOR_POPELEMENT(&g_requestVec, &tempRequest, index);
            return;
        }
    }
}

void InitSessionManager(void)
{
    g_sessionManagerVec = CREATE_HC_VECTOR(SessionManagerVec);
    g_requestVec = CREATE_HC_VECTOR(RequestInfoVec);
}

void DestroySessionManager(void)
{
    uint32_t index;
    void **session = NULL;
    FOR_EACH_HC_VECTOR(g_sessionManagerVec, index, session) {
        if (session != NULL && (*session != NULL)) {
            Session *temp = (Session *)(*session);
            temp->destroy(temp);
        }
    }
    DESTROY_HC_VECTOR(SessionManagerVec, &g_sessionManagerVec);
    DESTROY_HC_VECTOR(RequestInfoVec, &g_requestVec);
}

bool IsRequestExist(int64_t requestId)
{
    int64_t sessionId = 0;
    return (GetSessionId(requestId, &sessionId) == HC_SUCCESS) ? true : false;
}

static void InformTimeOutAndDestroyRequest(const DeviceAuthCallback *callback, int64_t sessionId)
{
    if (callback == NULL || callback->onError == NULL) {
        LOGD("Callback is null, can't inform timeout");
        return;
    }
    int64_t requestId = 0;
    uint32_t index;
    RequestInfo *requestInfo = NULL;
    FOR_EACH_HC_VECTOR(g_requestVec, index, requestInfo) {
        if ((requestInfo != NULL) && (requestInfo->sessionId == sessionId)) {
            requestId = requestInfo->requestId;
            RequestInfo tempRequest;
            HC_VECTOR_POPELEMENT(&g_requestVec, &tempRequest, index);
            break;
        }
    }
    LOGI("Begin to inform time out, requestId :%" PRId64, requestId);
    callback->onError(requestId, AUTH_FORM_INVALID_TYPE, HC_ERR_TIME_OUT, NULL);
}

static void RemoveOverTimeSession(void)
{
    uint32_t index = 0;
    void **session = NULL;
    while (index < g_sessionManagerVec.size(&(g_sessionManagerVec))) {
        session = g_sessionManagerVec.getp(&(g_sessionManagerVec), index);
        if (session == NULL || (*session == NULL)) {
            index++;
            continue;
        }
        Session *ptr = (Session *)(*session);
        if (HcGetIntervalTime(ptr->createTime) < TIME_OUT_VALUE) {
            index++;
        } else {
            InformTimeOutAndDestroyRequest(ptr->callback, ptr->sessionId);
            ptr->destroy(ptr);
            g_sessionManagerVec.eraseElement(&(g_sessionManagerVec), session, index);
        }
        session = g_sessionManagerVec.getp(&(g_sessionManagerVec), index);
    }
}

int32_t ProcessSession(int64_t requestId, int32_t type, CJson *in)
{
    RemoveOverTimeSession();
    int64_t sessionId = 0;
    int32_t result = GetSessionIdByType(requestId, type, &sessionId);
    if (result != HC_SUCCESS) {
        LOGE("The corresponding session is not found!");
        return result;
    }
    uint32_t index;
    void **session = NULL;
    FOR_EACH_HC_VECTOR(g_sessionManagerVec, index, session) {
        if (session != NULL && (*session != NULL)) {
            Session *ptr = (Session *)(*session);
            if (ptr->sessionId == sessionId) {
                return ptr->process(ptr, in);
            }
        }
    }
    return HC_ERR_SESSION_NOT_EXIST;
}

static int32_t CheckForCreateSession(int64_t requestId, CJson *params, const DeviceAuthCallback *callback)
{
    if ((params == NULL) && (callback == NULL)) {
        LOGE("The input params or callback is NULL!");
        return HC_ERR_INVALID_PARAMS;
    }
    if (IsRequestExist(requestId)) {
        LOGE("A request with the request ID already exists!");
        return HC_ERR_REQUEST_EXIST;
    }
    return HC_SUCCESS;
}

int32_t CreateSession(int64_t requestId, SessionTypeValue sessionType, CJson *params,
    const DeviceAuthCallback *callback)
{
    RemoveOverTimeSession();
    int32_t res = CheckForCreateSession(requestId, params, callback);
    if (res != HC_SUCCESS) {
        return res;
    }
    uint32_t vecSize = g_sessionManagerVec.size(&g_sessionManagerVec);
    LOGI("Current session num: %d", vecSize);
    if (vecSize >= MAX_SESSION_COUNT) {
        LOGE("Session vector is full.");
        return HC_ERR_SESSION_IS_FULL;
    }
    RequestInfo requestInfo;
    requestInfo.requestId = requestId;
    Session *session = NULL;
    for (uint32_t i = 0; i < sizeof(SESSION_MANAGER_INFO) / sizeof(SessionManagerInfo); i++) {
        if (SESSION_MANAGER_INFO[i].sessionType == sessionType) {
            session = SESSION_MANAGER_INFO[i].createSessionFunc(params, callback);
            requestInfo.type = SESSION_MANAGER_INFO[i].requestType;
            break;
        }
    }
    if (session == NULL) {
        LOGE("Failed to create session! Session Type: %d", sessionType);
        return HC_ERR_CREATE_SESSION_FAIL;
    }

    requestInfo.sessionId = session->sessionId;
    g_sessionManagerVec.pushBackT(&g_sessionManagerVec, (void *)session);
    g_requestVec.pushBack(&g_requestVec, &requestInfo);
    session->createTime = HcGetCurTime();
    if (session->createTime <= 0) {
        session->createTime = 0;
        LOGE("Failed to get cur time.");
    }
    return HC_SUCCESS;
}

void DestroySession(int64_t requestId)
{
    int64_t sessionId = 0;
    if (GetSessionId(requestId, &sessionId) != HC_SUCCESS) {
        LOGI("The corresponding session is not found. Therefore, the destruction operation is not required!");
        return;
    }
    uint32_t index;
    void **session = NULL;
    FOR_EACH_HC_VECTOR(g_sessionManagerVec, index, session) {
        if (session != NULL && (*session != NULL)) {
            if (((Session *)(*session))->sessionId == sessionId) {
                ((Session *)(*session))->destroy(((Session *)(*session)));
                *session = NULL;
                HC_VECTOR_POPELEMENT(&g_sessionManagerVec, session, index);
                break;
            }
        }
    }
    DestroyRequest(requestId);
}

void OnChannelOpened(int64_t requestId, int64_t channelId)
{
    int64_t sessionId = 0;
    if (GetSessionIdByType(requestId, BIND_TYPE, &sessionId) != HC_SUCCESS) {
        LOGE("The corresponding session is not found!");
        return;
    }
    uint32_t index;
    void **session = NULL;
    FOR_EACH_HC_VECTOR(g_sessionManagerVec, index, session) {
        if (session == NULL || (*session == NULL) || (((Session *)(*session))->sessionId != sessionId)) {
            continue;
        }
        int sessionType = ((Session *)(*session))->type;
        if ((sessionType == TYPE_CLIENT_BIND_SESSION) ||
            (sessionType == TYPE_CLIENT_BIND_SESSION_LITE) ||
            (sessionType == TYPE_CLIENT_KEY_AGREE_SESSION)) {
            BindSession *realSession = (BindSession *)(*session);
            realSession->onChannelOpened(*session, channelId, requestId);
            return;
        }
        LOGE("The type of the found session is not as expected!");
        return;
    }
}

void OnConfirmed(int64_t requestId, CJson *returnData)
{
    int64_t sessionId = 0;
    if (GetSessionIdByType(requestId, BIND_TYPE, &sessionId) != HC_SUCCESS) {
        LOGE("The corresponding session is not found!");
        return;
    }
    uint32_t index;
    void **session = NULL;
    FOR_EACH_HC_VECTOR(g_sessionManagerVec, index, session) {
        if ((session == NULL) || (*session == NULL) || (((Session *)(*session))->sessionId != sessionId)) {
            continue;
        }
        int sessionType = ((Session *)(*session))->type;
        if ((sessionType == TYPE_SERVER_BIND_SESSION) ||
            (sessionType == TYPE_SERVER_BIND_SESSION_LITE) ||
            (sessionType == TYPE_SERVER_KEY_AGREE_SESSION)) {
            BindSession *realSession = (BindSession *)(*session);
            if (!realSession->isWaiting) {
                LOGE("The found session is not in the waiting state!");
                return;
            }
            realSession->onConfirmed(*session, returnData);
            return;
        }
        LOGE("The type of the found session is not as expected!");
        return;
    }
}