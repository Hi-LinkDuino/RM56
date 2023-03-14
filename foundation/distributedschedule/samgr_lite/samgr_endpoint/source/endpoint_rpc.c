/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "endpoint.h"

#include <log.h>
#include <ohos_errno.h>
#include <securec.h>
#include <service.h>
#include <stdlib.h>
#include <unistd.h>

#include "default_client.h"
#include "ipc_skeleton.h"
#include "iproxy_server.h"
#include "memory_adapter.h"
#include "policy_define.h"
#include "pthread.h"
#include "samgr_server.h"
#include "serializer.h"
#include "thread_adapter.h"

typedef struct RemoteRegister RemoteRegister;
struct RemoteRegister {
    MutexId mtx;
    Endpoint *endpoint;
    Vector clients;
};

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "Samgr"
#define LOG_DOMAIN 0xD001800

#ifdef LITE_LINUX_BINDER_IPC
#define MAX_STACK_SIZE 0x100000
#else
#define MAX_STACK_SIZE 0x1000
#endif
#define MAX_OBJECT_NUM 5
#define MAX_RETRY_TIMES 300
#define RETRY_INTERVAL (50 * 1000)
#define MAX_REGISTER_RETRY_TIMES 10
#define REGISTER_RETRY_INTERVAL 2
#define MAX_POLICY_NUM 8

#ifndef MAX_BUCKET_RATE
#define MAX_BUCKET_RATE 1000
#endif

#ifndef MAX_BURST_RATE
#define MAX_BURST_RATE (MAX_BUCKET_RATE + (MAX_BUCKET_RATE >> 1))
#endif

#define SAMGR_SERVICE "samgr"

typedef struct Router {
    SaName saName;
    Identity identity;
    IServerProxy *proxy;
    PolicyTrans *policy;
    uint32 policyNum;
} Router;

static int CompareIServerProxy(const IServerProxy *proxy1, const IServerProxy *proxy2);
static IServerProxy *GetIServerProxy(const Router *router);
static int Dispatch(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option);
static void HandleIpc(const Request *request, const Response *response);
static int OnSamgrServerExit(void *ipcMsg, IpcIo *data, void *argv);
static int RegisterRemoteFeatures(Endpoint *endpoint);
static void Listen(Endpoint *endpoint, int index, const char *service, const char *feature);
static boolean JudgePolicy(uid_t callingUid, const PolicyTrans *policy, uint32 policyNum);
static boolean SearchFixedPolicy(uid_t callingUid, PolicyTrans policy);
static int AddPolicyToRouter(const Endpoint *endpoint, const SvcIdentity *saInfo,
                             const PolicyTrans *policy, uint32 policyNum);
static int RegisterRemoteEndpoint(SvcIdentity *identity, int token, const char *service, const char *feature);
static int RegisterIdentity(const SaName *saName, SvcIdentity *saInfo,
                            PolicyTrans **policy, uint32 *policyNum);
static void GetRemotePolicy(IpcIo *reply, PolicyTrans **policy, uint32 *policyNum);

Endpoint *SAMGR_CreateEndpoint(const char *name, RegisterEndpoint registry)
{
    Endpoint *endpoint = SAMGR_Malloc(sizeof(Endpoint));
    if (endpoint == NULL) {
        return NULL;
    }
    endpoint->deadId = INVALID_INDEX;
    endpoint->boss = NULL;
    endpoint->routers = VECTOR_Make((VECTOR_Key)GetIServerProxy, (VECTOR_Compare)CompareIServerProxy);
    endpoint->name = name;
    endpoint->running = FALSE;
    endpoint->identity.handle = (uint32_t)INVALID_INDEX;
    endpoint->identity.token = (uint32_t)INVALID_INDEX;
    endpoint->identity.cookie = (uint32_t)INVALID_INDEX;
    endpoint->registerEP = (registry == NULL) ? RegisterRemoteEndpoint : registry;
    TB_InitBucket(&endpoint->bucket, MAX_BUCKET_RATE, MAX_BURST_RATE);
    return endpoint;
}

int SAMGR_AddRouter(Endpoint *endpoint, const SaName *saName, const Identity *id, IUnknown *proxy)
{
    if (endpoint == NULL || id == NULL || proxy == NULL || saName == NULL) {
        return EC_INVALID;
    }
    IServerProxy *serverProxy = NULL;
    proxy->QueryInterface(proxy, SERVER_PROXY_VER, (void *)&serverProxy);
    int index = VECTOR_FindByKey(&endpoint->routers, proxy);
    if (index != INVALID_INDEX) {
        serverProxy->Release((IUnknown *)serverProxy);
    }
    Router *router = SAMGR_Malloc(sizeof(Router));
    if (router == NULL) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Memory is not enough! Identity<%d, %d>",
                    id->serviceId, id->featureId);
        return EC_NOMEMORY;
    }
    router->saName = *saName;
    router->identity = *id;
    router->proxy = serverProxy;
    router->policy = NULL;
    router->policyNum = 0;
    index = VECTOR_Add(&endpoint->routers, router);
    if (index == INVALID_INDEX) {
        SAMGR_Free(router);
        return EC_FAILURE;
    }
    Listen(endpoint, index, saName->service, saName->feature);
    return index;
}

int32 SAMGR_AddSysCap(const Endpoint *endpoint, const char *sysCap, BOOL isReg)
{
    if (endpoint == NULL) {
        return EC_INVALID;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SAMGR_AddSysCap begin");
    IpcIo req;
    uint8 data[MAX_DATA_LEN];
    IpcIoInit(&req, data, MAX_DATA_LEN, 0);
    WriteUint32(&req, RES_SYSCAP);
    WriteUint32(&req, OP_PUT);
    WriteBool(&req, sysCap);
    WriteBool(&req, isReg);

    IpcIo reply;
    void *replyBuf = NULL;
    SvcIdentity samgr = {SAMGR_HANDLE, SAMGR_TOKEN, SAMGR_COOKIE};
    MessageOption option;
    MessageOptionInit(&option);
    int ret = SendRequest(samgr, INVALID_INDEX, &req, &reply,
                          option, (uintptr_t *)&replyBuf);
    ret = -ret;
    int32_t ipcRet = ret;
    if (ret == EC_SUCCESS) {
        ReadInt32(&reply, &ipcRet);
    }

    if (replyBuf != NULL) {
        FreeBuffer(replyBuf);
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SAMGR_AddSysCap ret = %d", ipcRet);

    return ipcRet;
}

int32 SAMGR_GetSysCap(const Endpoint *endpoint, const char *sysCap, BOOL *isReg)
{
    if (endpoint == NULL) {
        return EC_INVALID;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SAMGR_GetSysCap begin");
    IpcIo req;
    uint8 data[MAX_DATA_LEN];
    IpcIoInit(&req, data, MAX_DATA_LEN, 0);
    WriteUint32(&req, RES_SYSCAP);
    WriteUint32(&req, OP_GET);
    WriteBool(&req, sysCap);

    IpcIo reply;
    void *replyBuf = NULL;
    SvcIdentity samgr = {SAMGR_HANDLE, SAMGR_TOKEN, SAMGR_COOKIE};
    MessageOption option;
    MessageOptionInit(&option);
    int ret = SendRequest(samgr, INVALID_INDEX, &req, &reply,
                          option, (uintptr_t *)&replyBuf);
    ret = -ret;
    *isReg = FALSE;
    int32_t ipcRet = ret;
    if (ret == EC_SUCCESS) {
        (void)ReadInt32(&reply, &ipcRet);
    }
    if (ipcRet == EC_SUCCESS) {
        (void)ReadBool(&reply, (bool *)isReg);
    }
    if (replyBuf != NULL) {
        FreeBuffer(replyBuf);
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SAMGR_GetSysCap ret = %d", ipcRet);
    return ipcRet;
}

static int SendGetAllSysCapsRequest(const Endpoint *endpoint, uint32 startIdx, IpcIo *reply, void **replyBuf)
{
    IpcIo req;
    uint8 data[MAX_DATA_LEN];
    IpcIoInit(&req, data, MAX_DATA_LEN, 0);
    WriteUint32(&req, RES_SYSCAP);
    WriteUint32(&req, OP_ALL);
    WriteUint32(&req, startIdx);
    SvcIdentity samgr = {SAMGR_HANDLE, SAMGR_TOKEN, SAMGR_COOKIE};
    MessageOption option;
    MessageOptionInit(&option);
    int ret = SendRequest(samgr, INVALID_INDEX, &req, reply,
                          option, (uintptr_t *)replyBuf);
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SendGetAllSysCapsRequest startIdx:%u, ret:%d!", startIdx, ret);
    return -ret;
}

static int32 ParseGetAllSysCapsReply(IpcIo *reply, char sysCaps[MAX_SYSCAP_NUM][MAX_SYSCAP_NAME_LEN],
                                     int32 *sysCapNum, BOOL *isEnd, uint32 *nextRequestIdx)
{
    int32_t ret;
    if (ReadInt32(reply, &ret)) {
        if (ret != EC_SUCCESS) {
            *isEnd = TRUE;
            return ret;
        }
    } else {
        *isEnd = TRUE;
        return EC_INVALID;
    }

    (void)ReadBool(reply, (bool *)isEnd);
    (void)ReadUint32(reply, nextRequestIdx);
    uint32 size;
    (void)ReadUint32(reply, &size);
    size = ((size > MAX_SYSCAP_NUM) ? MAX_SYSCAP_NUM : size);
    int cnt = *sysCapNum;
    for (uint32 i = 0; i < size; i++) {
        uint32 len = 0;
        char *sysCap = (char *)ReadString(reply, (size_t *)&len);
        if (sysCap == NULL || len == 0) {
            continue;
        }
        if (strcpy_s(sysCaps[cnt], sizeof(sysCaps[cnt]), sysCap) != EC_SUCCESS) {
            continue;
        }
        cnt++;
    }
    *sysCapNum = cnt;

    return ret;
}

int32 SAMGR_GetSystemCapabilities(const Endpoint *endpoint,
                                  char sysCaps[MAX_SYSCAP_NUM][MAX_SYSCAP_NAME_LEN], int32 *sysCapNum)
{
    if (sysCapNum == NULL) {
        return EC_INVALID;
    }
    *sysCapNum = 0;
    if (endpoint == NULL) {
        return EC_INVALID;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SAMGR_GetSystemCapabilities begin");
    IpcIo reply;
    void *replyBuf = NULL;
    uint32 startIdx = 0;
    BOOL isEnd = TRUE;
    int ret;
    do {
        ret = SendGetAllSysCapsRequest(endpoint, startIdx, &reply, &replyBuf);
        if (ret == EC_SUCCESS) {
            ret = ParseGetAllSysCapsReply(&reply, sysCaps, sysCapNum, &isEnd, &startIdx);
        }
        if (replyBuf != NULL) {
            FreeBuffer(replyBuf);
        }
    } while (isEnd == FALSE && ret == EC_SUCCESS);
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "SAMGR_GetSystemCapabilities ret = %d", ret);
    return ret;
}

int SAMGR_ProcPolicy(const Endpoint *endpoint, const SaName *saName, int token)
{
    if (endpoint == NULL || saName == NULL || token == INVALID_INDEX) {
        return EC_INVALID;
    }
    // retry until success or 20 seconds.
    int ret = EC_INVALID;
    uint8 retry = 0;
    SvcIdentity saInfo = {INVALID_INDEX, token, INVALID_INDEX};
    while (retry < MAX_REGISTER_RETRY_TIMES) {
        ++retry;
        PolicyTrans *policy = NULL;
        uint32 policyNum = 0;
        ret = RegisterIdentity(saName,  &saInfo, &policy, &policyNum);
        if (ret != EC_SUCCESS || policy == NULL) {
            SAMGR_Free(policy);
            continue;
        }
        HILOG_INFO(HILOG_MODULE_SAMGR, "Register server sa<%s, %s> id<%lu, %u> retry:%d ret:%d!",
                   saName->service, saName->feature, saInfo.handle, saInfo.token, retry, ret);
        ret = AddPolicyToRouter(endpoint, &saInfo, policy, policyNum);
        SAMGR_Free(policy);
        if (ret == EC_SUCCESS) {
            break;
        }
        sleep(REGISTER_RETRY_INTERVAL);
    }
    return ret;
}

static void Listen(Endpoint *endpoint, int index, const char *service, const char *feature)
{
    endpoint->registerEP(&endpoint->identity, index, service, feature);
}

static int AddPolicyToRouter(const Endpoint *endpoint, const SvcIdentity *saInfo,
                             const PolicyTrans *policy, uint32 policyNum)
{
    if (endpoint == NULL || saInfo == NULL || policy == NULL) {
        return EC_INVALID;
    }

    Router *router = VECTOR_At((Vector *)&endpoint->routers, saInfo->token);
    if (router == NULL) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Router <%s, %u> is NULL", endpoint->name, saInfo->token);
        return EC_INVALID;
    }

    if (router->policy != NULL) {
        return EC_SUCCESS;
    }
    router->policyNum = policyNum;
    if (policyNum == 0) {
        return EC_INVALID;
    }
    router->policy = (PolicyTrans *)SAMGR_Malloc(sizeof(PolicyTrans) * policyNum);
    if (router->policy == NULL) {
        return EC_NOMEMORY;
    }
    if (memcpy_s(router->policy, sizeof(PolicyTrans) * policyNum, policy,
                 sizeof(PolicyTrans) * policyNum) != EOK) {
        SAMGR_Free(router->policy);
        router->policy = NULL;
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Add Policy <%s, %s, %s> Failed!",
                    endpoint->name, router->saName.service, router->saName.feature);
        return EC_FAILURE;
    }
    HILOG_DEBUG(HILOG_MODULE_SAMGR, "Add Policy <%s, %s, %s> Success",
                endpoint->name, router->saName.service, router->saName.feature);
    return EC_SUCCESS;
}

static int Dispatch(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option)
{
    extern RemoteRegister g_remoteRegister;
    Endpoint *endpoint = g_remoteRegister.endpoint;
    uintptr_t saId = 0;
    ReadInt32(data, &saId);
    SaNode *saNode = GetSaNodeBySaId(saId);
    if (saNode == NULL) {
        HILOG_WARN(HILOG_MODULE_SAMGR, "get sa node by sa id %d is NULL", saId);
        goto ERROR;
    }
    if (TB_CheckMessage(&endpoint->bucket) == BUCKET_BUSY) {
        HILOG_WARN(HILOG_MODULE_SAMGR, "Flow Control <%u> is NULL", saNode->token);
        goto ERROR;
    }

    Router *router = VECTOR_At(&endpoint->routers, saNode->token);
    if (router == NULL) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Router <%s, %u> is NULL", endpoint->name, saNode->token);
        goto ERROR;
    }
    Response resp = {0};
    resp.data = endpoint;
    Request request = {0};
    request.msgId = saNode->token;
    request.data = data;
    resp.reply = reply;
    request.msgValue = code;
    uint32 *ref = NULL;
    int ret = SAMGR_SendSharedDirectRequest(&router->identity, &request, &resp, &ref, HandleIpc);
    if (ret != EC_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Router[%u] Service<%d, %d> is busy",
                    saNode->token, router->identity.serviceId, router->identity.featureId);
        goto ERROR;
    }

    return EC_SUCCESS;
ERROR:
    return EC_INVALID;
}

static void HandleIpc(const Request *request, const Response *response)
{
    Endpoint *endpoint = (Endpoint *)response->data;
    Router *router = VECTOR_At(&endpoint->routers, request->msgId);

    router->proxy->Invoke(router->proxy, request->msgValue, NULL, request->data, response->reply);
}

static int CompareIServerProxy(const IServerProxy *proxy1, const IServerProxy *proxy2)
{
    if (proxy1 == proxy2) {
        return 0;
    }
    return (proxy1 > proxy2) ? 1 : -1;
}

static IServerProxy *GetIServerProxy(const Router *router)
{
    if (router == NULL) {
        return NULL;
    }
    return router->proxy;
}

static int RegisterIdentity(const SaName *saName, SvcIdentity *saInfo,
                            PolicyTrans **policy, uint32 *policyNum)
{
    IpcIo req;
    uint8 data[MAX_DATA_LEN];
    IpcIoInit(&req, data, MAX_DATA_LEN, 0);
    WriteUint32(&req, RES_FEATURE);
    WriteUint32(&req, OP_PUT);
    WriteBool(&req, saName->service);
    WriteBool(&req, saName->feature == NULL);
    if (saName->feature != NULL) {
        WriteBool(&req, saName->feature);
    }
    WriteUint32(&req, saInfo->token);
    IpcIo reply;
    void *replyBuf = NULL;
    SvcIdentity samgr = {SAMGR_HANDLE, SAMGR_TOKEN, SAMGR_COOKIE};
    MessageOption option;
    MessageOptionInit(&option);
    int ret = SendRequest(samgr, INVALID_INDEX, &req, &reply, option,
                          (uintptr_t *)&replyBuf);
    ret = -ret;
    int32_t ipcRet = EC_FAILURE;
    if (ret == EC_SUCCESS) {
        ret = ReadInt32(&reply, &ipcRet);
    }
    if (ipcRet == EC_SUCCESS) {
        SvcIdentity target;
        (void)ReadRemoteObject(&reply, &target);
        GetRemotePolicy(&reply, policy, policyNum);
    }
    if (replyBuf != NULL) {
        FreeBuffer(replyBuf);
    }
    return ret;
}

static int RegisterRemoteFeatures(Endpoint *endpoint)
{
    int nums = 0;
    int size = VECTOR_Size(&endpoint->routers);
    int i;
    SvcIdentity identity;
    for (i = 0; i < size; ++i) {
        Router *router = VECTOR_At(&endpoint->routers, i);
        if (router == NULL) {
            continue;
        }

        identity.handle = endpoint->identity.handle;
        identity.token = i;
        int ret = RegisterIdentity(&(router->saName), &identity, &(router->policy),
                                   &(router->policyNum));
        if (ret == EC_SUCCESS) {
            ++nums;
        }
        HILOG_DEBUG(HILOG_MODULE_SAMGR, "RegisterRemoteFeatures<%s, %s> ret:%d",
                    router->saName.service, router->saName.feature,  ret);
    }
    return VECTOR_Num(&endpoint->routers) - nums;
}

static int RegisterRemoteEndpoint(SvcIdentity *identity, int token, const char *service, const char *feature)
{
    IpcObjectStub *objectStubOne = (IpcObjectStub *)calloc(1, sizeof(IpcObjectStub));
    if (objectStubOne == NULL) {
        return -1;
    }
    objectStubOne->func = Dispatch;
    objectStubOne->isRemote = true;
    // handle is used by rpc, should be bigger than 0
    identity->handle = token + 1;
    identity->cookie = objectStubOne;
    // token is used by router index, should be itself, and save in SaNode
    identity->token = token;
    extern int AddEndpoint(SvcIdentity identity, const char *service, const char *feature);
    AddEndpoint(*identity, service, feature);
    return EC_SUCCESS;
}

static int OnSamgrServerExit(void *ipcMsg, IpcIo *data, void *argv)
{
    (void)data;
    HILOG_ERROR(HILOG_MODULE_SAMGR, "Disconnect to samgr server!");
    Endpoint *endpoint = (Endpoint *)argv;
    if (endpoint == NULL || endpoint->registerEP == NULL) {
        return EC_FAILURE;
    }
    if (ipcMsg != NULL) {
        FreeBuffer(ipcMsg);
    }
    int size = VECTOR_Size(&endpoint->routers);
    int i;
    for (i = 0; i < size; i++) {
        Router *router = VECTOR_At(&endpoint->routers, i);
        if (router == NULL) {
            continue;
        }
        SAMGR_Free(router->policy);
        router->policy = NULL;
        router->policyNum = 0;
    }

    SvcIdentity old = endpoint->identity;
    while (endpoint->registerEP(&endpoint->identity, 0, "", "") != EC_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Reconnect to samgr server failed!");
        usleep(RETRY_INTERVAL);
    }
    SvcIdentity new = endpoint->identity;
    if (old.handle != new.handle || old.cookie != new.cookie || old.token != new.token) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Samgr server identity error!");
        exit(-1);
    }

    SvcIdentity identity = {SAMGR_HANDLE, SAMGR_TOKEN, SAMGR_COOKIE};
    (void)RemoveDeathRecipient(identity, endpoint->deadId);
    (void)AddDeathRecipient(identity, OnSamgrServerExit, endpoint, &endpoint->deadId);
    int remain = RegisterRemoteFeatures(endpoint);
    HILOG_INFO(HILOG_MODULE_SAMGR, "Reconnect and register finished! remain<%d> iunknown!", remain);
    return EC_SUCCESS;
}

static void GetRemotePolicy(IpcIo *reply, PolicyTrans **policy, uint32 *policyNum)
{
    if (reply == NULL) {
        return;
    }
    uint32 i;
    uint32 j;
    ReadUint32(reply, policyNum);
    if (*policyNum > MAX_POLICY_NUM) {
        *policyNum = MAX_POLICY_NUM;
    }
    SAMGR_Free(*policy);
    if (*policyNum == 0) {
        *policy = NULL;
        return;
    }
    *policy = (PolicyTrans *)SAMGR_Malloc(sizeof(PolicyTrans) * (*policyNum));
    if (*policy == NULL) {
        return;
    }
    for (i = 0; i < *policyNum; i++) {
        if (ReadInt32(reply, &(*policy)[i].type)) {
            switch ((*policy)[i].type) {
                case RANGE:
                    ReadInt32(reply, &((*policy)[i].uidMin));
                    ReadInt32(reply, &((*policy)[i].uidMax));
                    break;
                case FIXED:
                    for (j = 0; j < UID_SIZE; j++) {
                        ReadInt32(reply, &((*policy)[i].fixedUid[j]));
                    }
                    break;
                case BUNDLENAME:
                    ReadInt32(reply, &((*policy)[i].fixedUid[0]));
                    break;
                default:
                    break;
            }
        }
    }
}

static boolean JudgePolicy(uid_t callingUid, const PolicyTrans *policy, uint32 policyNum)
{
    if (policy == NULL) {
        HILOG_ERROR(HILOG_MODULE_SAMGR, "Policy is NULL! Num is %u", policyNum);
        return FALSE;
    }

    uint32 i;
    for (i = 0; i < policyNum; i++) {
        if (policy[i].type == RANGE && callingUid >= policy[i].uidMin && callingUid <= policy[i].uidMax) {
            return TRUE;
        }
        if (policy[i].type == FIXED && SearchFixedPolicy(callingUid, policy[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

static boolean SearchFixedPolicy(uid_t callingUid, PolicyTrans policy)
{
    int i;
    for (i = 0; i < UID_SIZE; i++) {
        if (callingUid == policy.fixedUid[i]) {
            return TRUE;
        }
    }
    return FALSE;
}
