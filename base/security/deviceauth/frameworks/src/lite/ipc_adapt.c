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

#include "ipc_adapt.h"
#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "hc_mutex.h"
#include "ipc_callback_stub.h"
#include "ipc_dev_auth_proxy.h"
#include "ipc_dev_auth_stub.h"
#include "ipc_sdk.h"
#include "ipc_service.h"
#include "liteipc_adapter.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUFF_MAX_SZ 128
#define IPC_CALL_BACK_MAX_NODES 64

typedef void (*CallbackStub)(uintptr_t, const IpcDataInfo *, int32_t, IpcIo *);
typedef struct {
    union {
        DeviceAuthCallback devAuth;
        DataChangeListener listener;
    } cbCtx;
    int64_t requestId;
    char appId[BUFF_MAX_SZ];
    int32_t cbType;
    int32_t delOnFni;
    int32_t methodId;
    int32_t proxyId;
    int32_t nodeIdx;
} IpcCallBackNode;

static struct {
    IpcCallBackNode *ctx;
    int32_t nodeCnt;
} g_ipcCallBackList = {NULL, 0};
static HcMutex g_cbListLock;

static StubDevAuthCb g_sdkCbStub;
static IClientProxy *g_proxyInstance = NULL;

static void SetIpcCallBackNodeDefault(IpcCallBackNode *node)
{
    (void)memset_s(node, sizeof(IpcCallBackNode), 0, sizeof(IpcCallBackNode));
    node->proxyId = -1;
    node->nodeIdx = -1;
    return;
}

int32_t InitIpcCallBackList(void)
{
    int32_t i;

    LOGI("initializing ...");
    if (g_ipcCallBackList.ctx != NULL) {
        LOGI("has initialized");
        return HC_SUCCESS;
    }

    g_ipcCallBackList.ctx = HcMalloc(sizeof(IpcCallBackNode) * IPC_CALL_BACK_MAX_NODES, 0);
    if (g_ipcCallBackList.ctx == NULL) {
        LOGE("initialized failed");
        return HC_ERROR;
    }
    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        SetIpcCallBackNodeDefault(g_ipcCallBackList.ctx + i);
    }
    (void)InitHcMutex(&g_cbListLock);
    g_ipcCallBackList.nodeCnt = 0;
    LOGI("initialized successful");
    return HC_SUCCESS;
}

static void ResetIpcCallBackNode(IpcCallBackNode *node)
{
    ResetRemoteObject(node->proxyId);
    SetIpcCallBackNodeDefault(node);
    return;
}

static void LockCallbackList(void)
{
    (void)g_cbListLock.lock(&g_cbListLock);
    return;
}

static void UnLockCallbackList(void)
{
    g_cbListLock.unlock(&g_cbListLock);
    return;
}

void DeInitIpcCallBackList(void)
{
    int32_t i;

    if (g_ipcCallBackList.ctx == NULL) {
        return;
    }
    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        ResetIpcCallBackNode(g_ipcCallBackList.ctx + i);
    }
    HcFree((void *)g_ipcCallBackList.ctx);
    g_ipcCallBackList.ctx = NULL;
    DestroyHcMutex(&g_cbListLock);
    return;
}

void ResetIpcCallBackNodeByNodeId(int32_t nodeIdx)
{
    LOGI("starting..., index %d", nodeIdx);
    if ((nodeIdx < 0) || (nodeIdx >= IPC_CALL_BACK_MAX_NODES)) {
        return;
    }
    if (g_ipcCallBackList.ctx == NULL) {
        return;
    }
    LockCallbackList();
    ResetIpcCallBackNode(g_ipcCallBackList.ctx + nodeIdx);
    UnLockCallbackList();
    LOGI("done, index %d", nodeIdx);
    return;
}

static IpcCallBackNode *GetIpcCallBackByAppId(const char *appId, int32_t type)
{
    int32_t i;
    int32_t ret;

    LOGI("appid: %s", appId);
    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].appId[0] == 0) {
            continue;
        }
        ret = strcmp(g_ipcCallBackList.ctx[i].appId, appId);
        if ((ret == 0) && (g_ipcCallBackList.ctx[i].cbType == type)) {
            return &g_ipcCallBackList.ctx[i];
        }
    }
    return NULL;
}

static IpcCallBackNode *GetFreeIpcCallBackNode(void)
{
    int32_t i;

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if ((g_ipcCallBackList.ctx[i].appId[0] == 0) && (g_ipcCallBackList.ctx[i].cbType == 0)) {
            g_ipcCallBackList.ctx[i].nodeIdx = i;
            return &g_ipcCallBackList.ctx[i];
        }
    }
    return NULL;
}

static void SetCbDeathRecipient(int32_t type, int32_t objIdx, int32_t cbDataIdx)
{
    if ((type == CB_TYPE_DEV_AUTH) || (type == CB_TYPE_LISTENER)) {
        AddCbDeathRecipient(objIdx, cbDataIdx);
    }
    return;
}

void AddIpcCbObjByAppId(const char *appId, int32_t objIdx, int32_t type)
{
    IpcCallBackNode *node = NULL;

    if (g_ipcCallBackList.ctx == NULL) {
        LOGE("list not inited");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        UnLockCallbackList();
        LOGE("list is full");
        return;
    }

    node = GetIpcCallBackByAppId(appId, type);
    if (node != NULL) {
        node->proxyId = objIdx;
        SetCbDeathRecipient(type, objIdx, node->nodeIdx);
        LOGI("ipc object add success, appid: %s, proxyId %d", appId, node->proxyId);
    }
    UnLockCallbackList();
    return;
}

int32_t AddIpcCallBackByAppId(const char *appId, const uint8_t *cbPtr, int32_t cbSz, int32_t type)
{
    IpcCallBackNode *node = NULL;
    errno_t eno;

    if (g_ipcCallBackList.ctx == NULL) {
        LOGE("list not inited");
        return HC_ERROR;
    }

    LockCallbackList();
    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        UnLockCallbackList();
        LOGE("list is full");
        return HC_ERROR;
    }

    node = GetIpcCallBackByAppId(appId, type);
    if (node != NULL) {
        eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
        if (eno != EOK) {
            UnLockCallbackList();
            LOGE("callback context memory copy failed");
            return HC_ERROR;
        }
        if (node->proxyId >= 0) {
            ResetRemoteObject(node->proxyId);
            node->proxyId = -1;
        }
        UnLockCallbackList();
        LOGI("callback add success, appid: %s", appId);
        return HC_SUCCESS;
    }

    LOGI("new callback to add, appid: %s", appId);
    node = GetFreeIpcCallBackNode();
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("get free node failed");
        return HC_ERROR;
    }
    node->cbType = type;
    eno = memcpy_s(&(node->appId), sizeof(node->appId), appId, strlen(appId) + 1);
    if (eno != EOK) {
        ResetIpcCallBackNode(node);
        UnLockCallbackList();
        LOGE("appid memory copy failed");
        return HC_ERROR;
    }
    eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
    if (eno != EOK) {
        ResetIpcCallBackNode(node);
        UnLockCallbackList();
        LOGE("callback context memory copy failed");
        return HC_ERROR;
    }
    node->proxyId = -1;
    g_ipcCallBackList.nodeCnt++;
    UnLockCallbackList();
    LOGI("callback add success, appid: %s, type %d", node->appId, node->cbType);
    return HC_SUCCESS;
}

void DelIpcCallBackByAppId(const char *appId, int32_t type)
{
    IpcCallBackNode *node = NULL;

    if ((g_ipcCallBackList.nodeCnt <= 0) || (g_ipcCallBackList.ctx == NULL)) {
        return;
    }

    LockCallbackList();
    node = GetIpcCallBackByAppId(appId, type);
    if (node != NULL) {
        ResetIpcCallBackNode(node);
        g_ipcCallBackList.nodeCnt--;
    }
    UnLockCallbackList();
    return;
}

static IpcCallBackNode *GetIpcCallBackByReqId(int64_t reqId, int32_t type)
{
    int32_t i;

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if ((reqId == g_ipcCallBackList.ctx[i].requestId) &&
            (g_ipcCallBackList.ctx[i].cbType == type)) {
            return &g_ipcCallBackList.ctx[i];
        }
    }
    return NULL;
}

int32_t AddReqIdByAppId(const char *appId, int64_t reqId)
{
    IpcCallBackNode *node = NULL;

    if (g_ipcCallBackList.ctx == NULL) {
        LOGE("ipc callback list not inited");
        return HC_ERROR;
    }

    LockCallbackList();
    node = GetIpcCallBackByAppId(appId, CB_TYPE_DEV_AUTH);
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("ipc callback node not found, appid: %s", appId);
        return HC_ERROR;
    }
    node->requestId = reqId;
    node->delOnFni = 0;
    UnLockCallbackList();
    LOGI("success, appid: %s, requestId: %lld", appId, reqId);
    return HC_SUCCESS;
}

void AddIpcCbObjByReqId(int64_t reqId, int32_t objIdx, int32_t type)
{
    IpcCallBackNode *node = NULL;

    if (g_ipcCallBackList.ctx == NULL) {
        LOGE("list not inited");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        UnLockCallbackList();
        LOGE("list is full");
        return;
    }

    node = GetIpcCallBackByReqId(reqId, type);
    if (node != NULL) {
        node->proxyId = objIdx;
        LOGI("ipc object add success, request id %lld, type %d, proxy id %d",
            reqId, type, node->proxyId);
    }
    UnLockCallbackList();
    return;
}

int32_t AddIpcCallBackByReqId(int64_t reqId, const uint8_t *cbPtr, int32_t cbSz, int32_t type)
{
    IpcCallBackNode *node = NULL;
    errno_t eno;

    if (g_ipcCallBackList.ctx == NULL) {
        LOGE("list is full");
        return HC_ERROR;
    }

    LockCallbackList();
    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        UnLockCallbackList();
        LOGE("list is full");
        return HC_ERROR;
    }

    node = GetIpcCallBackByReqId(reqId, type);
    if (node != NULL) {
        eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
        if (eno != EOK) {
            UnLockCallbackList();
            LOGE("callback context memory copy failed");
            return HC_ERROR;
        }
        if (node->proxyId >= 0) {
            ResetRemoteObject(node->proxyId);
            node->proxyId = -1;
        }
        UnLockCallbackList();
        LOGI("callback added success, request id %lld, type %d", reqId, type);
        return HC_SUCCESS;
    }

    LOGI("new callback to add, request id %lld, type %d", reqId, type);
    node = GetFreeIpcCallBackNode();
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("get free node failed");
        return HC_ERROR;
    }
    node->cbType = type;
    node->requestId = reqId;
    eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
    if (eno != EOK) {
        UnLockCallbackList();
        ResetIpcCallBackNode(node);
        LOGE("callback context memory copy failed");
        return HC_ERROR;
    }
    node->delOnFni = 1;
    node->proxyId = -1;
    g_ipcCallBackList.nodeCnt++;
    UnLockCallbackList();
    LOGI("callback added success, request id %lld, type %d", reqId, type);
    return HC_SUCCESS;
}

static void DelCallBackByReqId(int64_t reqId, int32_t type)
{
    IpcCallBackNode *node = NULL;

    if ((g_ipcCallBackList.nodeCnt <= 0) || (g_ipcCallBackList.ctx == NULL)) {
        return;
    }

    node = GetIpcCallBackByReqId(reqId, type);
    if ((node != NULL) && (node->delOnFni == 1)) {
        ResetIpcCallBackNode(node);
        g_ipcCallBackList.nodeCnt--;
    }
    return;
}

void DelIpcCallBackByReqId(int64_t reqId, int32_t type, bool withLock)
{
    if (withLock) {
        LockCallbackList();
        DelCallBackByReqId(reqId, type);
        UnLockCallbackList();
        return;
    }
    DelCallBackByReqId(reqId, type);
    return;
}

static void OnTransmitStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    int64_t requestId = 0;
    int32_t inOutLen;
    uint8_t *data = NULL;
    uint32_t dataLen = 0u;
    bool bRet = false;
    bool (*onTransmitHook)(int64_t, uint8_t *, uint32_t) = NULL;

    onTransmitHook = (bool (*)(int64_t, uint8_t *, uint32_t))(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID, (uint8_t *)(&requestId), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum,
        PARAM_TYPE_COMM_DATA, (uint8_t *)&data, (int32_t *)(&dataLen));
    bRet = onTransmitHook(requestId, data, dataLen);
    (bRet == true) ? IpcIoPushInt32(reply, HC_SUCCESS) : IpcIoPushInt32(reply, HC_ERROR);
    return;
}

static void OnSessKeyStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    int64_t requestId = 0;
    int32_t inOutLen;
    uint8_t *keyData = NULL;
    uint32_t dataLen = 0u;
    void (*onSessKeyHook)(int64_t, uint8_t *, uint32_t) = NULL;

    onSessKeyHook = (void (*)(int64_t, uint8_t *, uint32_t))(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID, (uint8_t *)(&requestId), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_SESS_KEY,
        (uint8_t *)(&keyData), (int32_t *)(&dataLen));
    onSessKeyHook(requestId, keyData, dataLen);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnFinishStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    int64_t requestId = 0;
    int32_t opCode = 0;
    int32_t inOutLen;
    char *data = NULL;
    void (*onFinishHook)(int64_t, int32_t, char *) = NULL;

    onFinishHook = (void (*)(int64_t, int32_t, char *))(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID, (uint8_t *)(&requestId), &inOutLen);
    inOutLen = sizeof(opCode);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_OPCODE, (uint8_t *)(&opCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_COMM_DATA, (uint8_t *)(&data), NULL);
    onFinishHook(requestId, opCode, data);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnErrorStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    int64_t requestId = 0;
    int32_t opCode = 0;
    int32_t errCode = 0;
    int32_t inOutLen;
    char *errInfo = NULL;
    void (*onErrorHook)(int64_t, int32_t, int32_t, char *) = NULL;

    onErrorHook = (void (*)(int64_t, int32_t, int32_t, char *))(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID, (uint8_t *)(&requestId), &inOutLen);
    inOutLen = sizeof(opCode);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_OPCODE, (uint8_t *)(&opCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_ERRCODE, (uint8_t *)(&errCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_ERR_INFO, (uint8_t *)(&errInfo), NULL);
    onErrorHook(requestId, opCode, errCode, errInfo);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnRequestStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    int64_t requestId = 0;
    int32_t opCode = 0;
    int32_t inOutLen;
    char *reqParams = NULL;
    char *reqResult = NULL;
    char *(*onReqHook)(int64_t, int32_t, char *) = NULL;

    onReqHook = (char *(*)(int64_t, int32_t, char *))(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID, (uint8_t *)(&requestId), &inOutLen);
    inOutLen = sizeof(opCode);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_OPCODE, (uint8_t *)(&opCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQ_INFO, (uint8_t *)(&reqParams), NULL);
    reqResult = onReqHook(requestId, opCode, reqParams);
    if (reqResult == NULL) {
        IpcIoPushInt32(reply, HC_ERROR);
        return;
    }
    IpcIoPushInt32(reply, HC_SUCCESS);
    IpcIoPushString(reply, (const char *)(reqResult));
    HcFree(reqResult);
    reqResult = NULL;
    return;
}

static void OnGroupCreatedStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    const char *groupInfo = NULL;
    void (*onGroupCreatedHook)(const char *) = NULL;

    onGroupCreatedHook = (void (*)(const char *))(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO, (uint8_t *)(&groupInfo), NULL);
    onGroupCreatedHook(groupInfo);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnGroupDeletedStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    const char *groupInfo = NULL;
    void (*onDelGroupHook)(const char *) = NULL;

    onDelGroupHook = (void (*)(const char *))(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO, (uint8_t *)(&groupInfo), NULL);
    onDelGroupHook(groupInfo);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnDevBoundStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    const char *groupInfo = NULL;
    const char *udid = NULL;
    void (*onDevBoundHook)(const char *, const char *) = NULL;

    onDevBoundHook = (void (*)(const char *, const char *))(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, (uint8_t *)(&udid), NULL);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO, (uint8_t *)(&groupInfo), NULL);
    onDevBoundHook(udid, groupInfo);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnDevUnboundStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    const char *groupInfo = NULL;
    const char *udid = NULL;
    void (*onDevUnBoundHook)(const char *, const char *) = NULL;

    onDevUnBoundHook = (void (*)(const char *, const char *))(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, (uint8_t *)(&udid), NULL);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO, (uint8_t *)(&groupInfo), NULL);
    onDevUnBoundHook(udid, groupInfo);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnDevUnTrustStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    const char *udid = NULL;
    void (*onDevUnTrustHook)(const char *) = NULL;

    onDevUnTrustHook = (void (*)(const char *))(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, (uint8_t *)(&udid), NULL);
    onDevUnTrustHook(udid);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnDelLastGroupStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    const char *udid = NULL;
    int32_t groupType = 0;
    int32_t inOutLen;
    void (*onDelLastGroupHook)(const char *, int32_t) = NULL;

    onDelLastGroupHook = (void (*)(const char *, int32_t))(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, (uint8_t *)(&udid), NULL);
    inOutLen = sizeof(groupType);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_TYPE, (uint8_t *)(&groupType), &inOutLen);
    onDelLastGroupHook(udid, groupType);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

static void OnTrustDevNumChangedStub(uintptr_t cbHook,
    const IpcDataInfo *cbDataCache, int32_t cacheNum, IpcIo *reply)
{
    int32_t devNum = 0;
    int32_t inOutLen;
    void (*onTrustDevNumChangedHook)(int32_t) = NULL;

    onTrustDevNumChangedHook = (void (*)(int32_t))(cbHook);
    inOutLen = sizeof(devNum);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_DATA_NUM, (uint8_t *)(&devNum), &inOutLen);
    onTrustDevNumChangedHook(devNum);
    IpcIoPushInt32(reply, HC_SUCCESS);
    return;
}

void ProcCbHook(int32_t callbackId, uintptr_t cbHook,
    const IpcDataInfo *cbDataCache, int32_t cacheNum, uintptr_t replyCtx)
{
    CallbackStub stubTable[] = {
        OnTransmitStub, OnSessKeyStub, OnFinishStub, OnErrorStub,
        OnRequestStub, OnGroupCreatedStub, OnGroupDeletedStub, OnDevBoundStub,
        OnDevUnboundStub, OnDevUnTrustStub, OnDelLastGroupStub, OnTrustDevNumChangedStub
    };
    IpcIo *reply = NULL;

    reply = (IpcIo *)(replyCtx);
    LOGI("Process call back hook, callback id %d", callbackId);
    if ((callbackId < CB_ID_ON_TRANS) || (callbackId > CB_ID_ON_TRUST_DEV_NUM_CHANGED)) {
        LOGE("Invalid call back id");
        return;
    }
    if (cbHook == 0x0) {
        LOGE("Invalid call back hook");
        return;
    }
    stubTable[callbackId - 1](cbHook, cbDataCache, cacheNum, reply);
    LOGI("ProcCbHook done");
    return;
}

static uint32_t EncodeCallData(IpcIo *dataParcel, int32_t type, const uint8_t *param, int32_t paramSz)
{
    const uint8_t *paramTmp = NULL;
    int32_t zeroVal = 0;

    paramTmp = param;
    if ((param == NULL) || (paramSz == 0)) {
        paramTmp = (const uint8_t *)(&zeroVal);
        paramSz = sizeof(zeroVal);
    }
    IpcIoPushInt32(dataParcel, type);
    IpcIoPushFlatObj(dataParcel, (const void *)(paramTmp), (uint32_t)paramSz);
    if (!IpcIoAvailable(dataParcel)) {
        return (uint32_t)(HC_ERROR);
    }
    return (uint32_t)(HC_SUCCESS);
}

/* group auth callback adapter */
static bool GaCbOnTransmitWithType(int64_t requestId, const uint8_t *data, uint32_t dataLen, int32_t type)
{
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    IpcIo reply;
    uint8_t dataBuf[IPC_STACK_BUFF_SZ] = {0};
    IpcCallBackNode *node = NULL;

    LOGI("starting ... request id: %lld, type %d", requestId, type);
    IpcIoInit(&reply, (void *)dataBuf, sizeof(dataBuf), 0);
    LockCallbackList();
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("onTransmit hook is null, request id %lld", requestId);
        return false;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return false;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, (const uint8_t *)(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_COMM_DATA, data, dataLen);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return false;
    }
    ActCallback(node->proxyId, CB_ID_ON_TRANS, (uintptr_t)(node->cbCtx.devAuth.onTransmit), dataParcel, &reply);
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    LOGI("process done, request id: %lld", requestId);
    if (IpcIoAvailable(&reply) && (IpcIoPopInt32(&reply) == HC_SUCCESS)) {
        return true;
    }
    return false;
}

static bool IpcGaCbOnTransmit(int64_t requestId, const uint8_t *data, uint32_t dataLen)
{
    return GaCbOnTransmitWithType(requestId, data, dataLen, CB_TYPE_DEV_AUTH);
}

static bool TmpIpcGaCbOnTransmit(int64_t requestId, const uint8_t *data, uint32_t dataLen)
{
    return GaCbOnTransmitWithType(requestId, data, dataLen, CB_TYPE_TMP_DEV_AUTH);
}

static void GaCbOnSessionKeyRetWithType(int64_t requestId, const uint8_t *sessKey, uint32_t sessKeyLen, int32_t type)
{
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    IpcCallBackNode *node = NULL;

    LOGI("starting ... request id: %lld, type %d", requestId, type);
    LockCallbackList();
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("onSessionKeyReturned hook is null, request id %lld", requestId);
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, (uint8_t *)(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_SESS_KEY, sessKey, sessKeyLen);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }
    ActCallback(node->proxyId, CB_ID_SESS_KEY_DONE,
        (uintptr_t)(node->cbCtx.devAuth.onSessionKeyReturned), dataParcel, NULL);
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    LOGI("process done, request id: %lld", requestId);
    return;
}

static void IpcGaCbOnSessionKeyReturned(int64_t requestId, const uint8_t *sessKey, uint32_t sessKeyLen)
{
    GaCbOnSessionKeyRetWithType(requestId, sessKey, sessKeyLen, CB_TYPE_DEV_AUTH);
    return;
}

static void TmpIpcGaCbOnSessionKeyReturned(int64_t requestId, const uint8_t *sessKey, uint32_t sessKeyLen)
{
    GaCbOnSessionKeyRetWithType(requestId, sessKey, sessKeyLen, CB_TYPE_TMP_DEV_AUTH);
    return;
}

static void GaCbOnFinishWithType(int64_t requestId, int32_t operationCode, const char *returnData, int32_t type)
{
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    IpcCallBackNode *node = NULL;

    LOGI("starting ... request id: %lld, type %d", requestId, type);
    LockCallbackList();
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("onFinish hook is null, request id %lld", requestId);
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, (uint8_t *)(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_OPCODE, (uint8_t *)(&operationCode), sizeof(operationCode));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_COMM_DATA, (const uint8_t *)(returnData), strlen(returnData) + 1);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }
    ActCallback(node->proxyId, CB_ID_ON_FINISH, (uintptr_t)(node->cbCtx.devAuth.onFinish), dataParcel, NULL);
    /* delete request id */
    DelIpcCallBackByReqId(requestId, type, false);
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    LOGI("process done, request id: %lld", requestId);
    return;
}

static void IpcGaCbOnFinish(int64_t requestId, int32_t operationCode, const char *returnData)
{
    GaCbOnFinishWithType(requestId, operationCode, returnData, CB_TYPE_DEV_AUTH);
    return;
}

static void TmpIpcGaCbOnFinish(int64_t requestId, int32_t operationCode, const char *returnData)
{
    GaCbOnFinishWithType(requestId, operationCode, returnData, CB_TYPE_TMP_DEV_AUTH);
    return;
}

static void GaCbOnErrorWithType(int64_t requestId, int32_t operationCode,
    int32_t errorCode, const char *errorReturn, int32_t type)
{
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    IpcCallBackNode *node = NULL;

    LOGI("starting ... request id: %lld, type %d", requestId, type);
    LockCallbackList();
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("onError hook is null, request id %lld", requestId);
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, (uint8_t *)(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_OPCODE, (uint8_t *)(&operationCode), sizeof(operationCode));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_ERRCODE, (uint8_t *)(&errorCode), sizeof(errorCode));
    if (errorReturn != NULL) {
        ret |= EncodeCallData(dataParcel, PARAM_TYPE_ERR_INFO, (const uint8_t *)(errorReturn), strlen(errorReturn) + 1);
    }
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }
    ActCallback(node->proxyId, CB_ID_ON_ERROR, (uintptr_t)(node->cbCtx.devAuth.onError), dataParcel, NULL);
    /* delete request id */
    DelIpcCallBackByReqId(requestId, type, false);
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    LOGI("process done, request id: %lld", requestId);
    return;
}

static void IpcGaCbOnError(int64_t requestId, int32_t operationCode, int32_t errorCode, const char *errorReturn)
{
    GaCbOnErrorWithType(requestId, operationCode, errorCode, errorReturn, CB_TYPE_DEV_AUTH);
    return;
}

static void TmpIpcGaCbOnError(int64_t requestId, int32_t operationCode, int32_t errorCode, const char *errorReturn)
{
    GaCbOnErrorWithType(requestId, operationCode, errorCode, errorReturn, CB_TYPE_TMP_DEV_AUTH);
    return;
}

static char *GaCbOnRequestWithType(int64_t requestId, int32_t operationCode, const char *reqParams, int32_t type)
{
    int32_t ret;
    uint32_t uRet;
    IpcIo *dataParcel = NULL;
    IpcIo reply;
    uint8_t dataBuf[IPC_STACK_BUFF_SZ] = {0};
    const char *dPtr = NULL;
    IpcCallBackNode *node = NULL;

    LOGI("starting ... request id: %lld, type %d", requestId, type);
    IpcIoInit(&reply, (void *)dataBuf, sizeof(dataBuf), 0);
    LockCallbackList();
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == NULL) {
        UnLockCallbackList();
        LOGE("onRequest hook is null, request id %lld", requestId);
        return NULL;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return NULL;
    }
    uRet = EncodeCallData(dataParcel, PARAM_TYPE_REQID, (uint8_t *)(&requestId), sizeof(requestId));
    uRet |= EncodeCallData(dataParcel, PARAM_TYPE_OPCODE, (uint8_t *)(&operationCode), sizeof(operationCode));
    if (reqParams != NULL) {
        uRet |= EncodeCallData(dataParcel, PARAM_TYPE_REQ_INFO, (const uint8_t *)(reqParams), strlen(reqParams) + 1);
    }
    if (uRet != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return NULL;
    }

    ActCallback(node->proxyId, CB_ID_ON_REQUEST, (uintptr_t)(node->cbCtx.devAuth.onRequest), dataParcel, &reply);
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    if (IpcIoAvailable(&reply)) {
        ret = IpcIoPopInt32(&reply);
        if (ret == HC_SUCCESS) {
            dPtr = (const char *)IpcIoPopString(&reply, NULL);
        }
    }
    LOGI("process done, request id: %lld, %s result", requestId, (dPtr != NULL) ? "valid" : "invalid");
    return (dPtr != NULL) ? strdup(dPtr) : NULL;
}

static bool CanFindCbByReqId(int64_t requestId)
{
    LockCallbackList();
    IpcCallBackNode *node = GetIpcCallBackByReqId(requestId, CB_TYPE_DEV_AUTH);
    UnLockCallbackList();
    return (node != NULL) ? true : false;
}

static char *IpcGaCbOnRequest(int64_t requestId, int32_t operationCode, const char *reqParams)
{
    if (!CanFindCbByReqId(requestId)) {
        CJson *reqParamsJson = CreateJsonFromString(reqParams);
        if (reqParamsJson == NULL) {
            LOGE("failed to create json from string!");
            return NULL;
        }
        const char *callerAppId = GetStringFromJson(reqParamsJson, FIELD_APP_ID);
        if (callerAppId == NULL) {
            LOGE("failed to get appId from json object!");
            FreeJson(reqParamsJson);
            return NULL;
        }
        int32_t ret = AddReqIdByAppId(callerAppId, requestId);
        FreeJson(reqParamsJson);
        if (ret != HC_SUCCESS) {
            return NULL;
        }
    }
    return GaCbOnRequestWithType(requestId, operationCode, reqParams, CB_TYPE_DEV_AUTH);
}

static char *TmpIpcGaCbOnRequest(int64_t requestId, int32_t operationCode, const char *reqParams)
{
    return GaCbOnRequestWithType(requestId, operationCode, reqParams, CB_TYPE_TMP_DEV_AUTH);
}

void IpcOnGroupCreated(const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    if (groupInfo == NULL) {
        LOGE("IpcOnGroupCreated, params error");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO, (const uint8_t *)(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("IpcGaCbOnRequest, build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onGroupCreated == NULL) {
                LOGE("onGroupCreated hook is invalid");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_GROUP_CREATED,
                (uintptr_t)(listener->onGroupCreated), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void IpcOnGroupDeleted(const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    if (groupInfo == NULL) {
        LOGE("IpcOnGroupDeleted, params error");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO, (const uint8_t *)(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("IpcGaCbOnRequest, build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onGroupDeleted == NULL) {
                LOGE("onGroupDeleted hook is null");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_GROUP_DELETED,
                (uintptr_t)(listener->onGroupDeleted), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void IpcOnDeviceBound(const char *peerUdid, const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    if ((peerUdid == NULL) || (groupInfo == NULL)) {
        LOGE("params error");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID, (const uint8_t *)(peerUdid), strlen(peerUdid) + 1);
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO, (const uint8_t *)(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onDeviceBound == NULL) {
                LOGE("onDeviceBound hook is null");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_DEV_BOUND,
                (uintptr_t)(listener->onDeviceBound), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void IpcOnDeviceUnBound(const char *peerUdid, const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    if ((peerUdid == NULL) || (groupInfo == NULL)) {
        LOGE("params error");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID, (const uint8_t *)(peerUdid), strlen(peerUdid) + 1);
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO, (const uint8_t *)(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onDeviceUnBound == NULL) {
                LOGE("onDeviceUnBound hook is null");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_DEV_UNBOUND,
                (uintptr_t)(listener->onDeviceUnBound), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void IpcOnDeviceNotTrusted(const char *peerUdid)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    if (peerUdid == NULL) {
        LOGE("params error");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID, (const uint8_t *)(peerUdid), strlen(peerUdid) + 1);
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onDeviceNotTrusted == NULL) {
                LOGE("onDeviceNotTrusted hook is null");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_DEV_UNTRUSTED,
                (uintptr_t)(listener->onDeviceNotTrusted), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void IpcOnLastGroupDeleted(const char *peerUdid, int32_t groupType)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    if (peerUdid == NULL) {
        LOGE("params error");
        return;
    }

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }
    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID, (const uint8_t *)(peerUdid), strlen(peerUdid) + 1);
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_GROUP_TYPE, (const uint8_t *)(&groupType), sizeof(groupType));
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onLastGroupDeleted == NULL) {
                LOGE("onLastGroupDeleted hook is null");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_LAST_GROUP_DELETED,
                (uintptr_t)(listener->onLastGroupDeleted), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void IpcOnTrustedDeviceNumChanged(int32_t curTrustedDeviceNum)
{
    int32_t i;
    uint32_t ret;
    IpcIo *dataParcel = NULL;
    DataChangeListener *listener = NULL;

    LockCallbackList();
    if (g_ipcCallBackList.ctx == NULL) {
        UnLockCallbackList();
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    dataParcel = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if (dataParcel == NULL) {
        UnLockCallbackList();
        return;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_DATA_NUM,
        (const uint8_t *)(&curTrustedDeviceNum), sizeof(curTrustedDeviceNum));
    if (ret != HC_SUCCESS) {
        UnLockCallbackList();
        HcFree((void *)dataParcel);
        LOGE("IpcOnTrustedDeviceNumChanged, build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onTrustedDeviceNumChanged == NULL) {
                LOGE("onTrustedDeviceNumChanged hook is null");
                continue;
            }
            ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_TRUST_DEV_NUM_CHANGED,
                (uintptr_t)(listener->onTrustedDeviceNumChanged), dataParcel, NULL);
        }
    }
    UnLockCallbackList();
    HcFree((void *)dataParcel);
    return;
}

void InitDeviceAuthCbCtx(DeviceAuthCallback *ctx, int32_t type)
{
    if (ctx == NULL) {
        return;
    }
    if (type == CB_TYPE_DEV_AUTH) {
        ctx->onTransmit = IpcGaCbOnTransmit;
        ctx->onSessionKeyReturned = IpcGaCbOnSessionKeyReturned;
        ctx->onFinish = IpcGaCbOnFinish;
        ctx->onError = IpcGaCbOnError;
        ctx->onRequest = IpcGaCbOnRequest;
    }
    if (type == CB_TYPE_TMP_DEV_AUTH) {
        ctx->onTransmit = TmpIpcGaCbOnTransmit;
        ctx->onSessionKeyReturned = TmpIpcGaCbOnSessionKeyReturned;
        ctx->onFinish = TmpIpcGaCbOnFinish;
        ctx->onError = TmpIpcGaCbOnError;
        ctx->onRequest = TmpIpcGaCbOnRequest;
    }
    return;
}

void InitDevAuthListenerCbCtx(DataChangeListener *ctx)
{
    if (ctx == NULL) {
        return;
    }
    ctx->onGroupCreated = IpcOnGroupCreated;
    ctx->onGroupDeleted = IpcOnGroupDeleted;
    ctx->onDeviceBound = IpcOnDeviceBound;
    ctx->onDeviceUnBound = IpcOnDeviceUnBound;
    ctx->onDeviceNotTrusted = IpcOnDeviceNotTrusted;
    ctx->onLastGroupDeleted = IpcOnLastGroupDeleted;
    ctx->onTrustedDeviceNumChanged = IpcOnTrustedDeviceNumChanged;
    return;
}

/* ipc client process adapter */
int32_t CreateCallCtx(uintptr_t *callCtx, uintptr_t *cbCtx)
{
    ProxyDevAuthData *dataCache = NULL;

    (void)cbCtx;
    if (callCtx == NULL) {
        return HC_ERR_INVALID_PARAMS;
    }

    dataCache = (ProxyDevAuthData *)HcMalloc(sizeof(ProxyDevAuthData), 0);
    if (dataCache == NULL) {
        LOGE("call context alloc failed");
        return HC_ERR_ALLOC_MEMORY;
    }
    dataCache->data = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    dataCache->tmpData = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    dataCache->reply = InitIpcDataCache(IPC_DATA_BUFF_MAX_SZ);
    if ((dataCache->data == NULL) || (dataCache->tmpData == NULL) || (dataCache->reply == NULL)) {
        DestroyCallCtx((uintptr_t *)(dataCache), NULL);
        return HC_ERROR;
    }
    /* linux lite, ipc io init with : token + SvcIdentity */
    dataCache->ioBuffOffset = IpcIoBufferOffset();
    *callCtx = (uintptr_t)(dataCache);
    return HC_SUCCESS;
}

void DestroyCallCtx(uintptr_t *callCtx, uintptr_t *cbCtx)
{
    ProxyDevAuthData *dataCache = NULL;

    (void)cbCtx;
    if ((callCtx != NULL) && (*callCtx != 0)) {
        dataCache = (ProxyDevAuthData *)(*callCtx);
        if (dataCache->data != NULL) {
            HcFree((void *)dataCache->data);
            dataCache->data = NULL;
        }
        if (dataCache->tmpData != NULL) {
            HcFree((void *)dataCache->tmpData);
            dataCache->tmpData = NULL;
        }
        if (dataCache->reply != NULL) {
            HcFree((void *)dataCache->reply);
            dataCache->reply = NULL;
        }
        HcFree((void *)dataCache);
        *callCtx = 0;
    }
    return;
}

void SetCbCtxToDataCtx(uintptr_t callCtx, int32_t cbIdx)
{
    ProxyDevAuthData *dataCache = NULL;
    const SvcIdentity *stubInfo = &g_sdkCbStub.stubIdentity;
    (void)cbIdx;
    if (!g_sdkCbStub.registered) {
        LOGW("SDK callback stub un-registered");
        return;
    }
    ShowIpcSvcInfo(stubInfo);
    dataCache = (ProxyDevAuthData *)(callCtx);
    SetCallbackStub(dataCache, stubInfo);
    return;
}

int32_t SetCallRequestParamInfo(uintptr_t callCtx, int32_t type, const uint8_t *param, int32_t paramSz)
{
    ProxyDevAuthData *dataCache = (ProxyDevAuthData *)(callCtx);

    return EncodeCallRequest(dataCache, type, param, paramSz);
}

int32_t DoBinderCall(uintptr_t callCtx, int32_t methodId, bool withSync)
{
    int32_t ret;
    ProxyDevAuthData *dataCache = (ProxyDevAuthData *)(callCtx);

    LOGI("proc method %d", methodId);
    ret = FinalCallRequest(dataCache, methodId);
    if (ret != HC_SUCCESS) {
        return ret;
    }
    return ActCall(g_proxyInstance, dataCache);
}

/* ipc service process adapter */
uint32_t SetIpcCallMap(uintptr_t ipcInstance, IpcServiceCall method, int32_t methodId)
{
    if ((method == NULL) || (methodId <= 0)) {
        return HC_ERR_INVALID_PARAMS;
    }

    return (uint32_t)SetCallMap(method, methodId);
}

int32_t CreateServiceInstance(uintptr_t *ipcInstance)
{
    *ipcInstance = 0x0;
    return HC_SUCCESS;
}

void DestroyServiceInstance(uintptr_t *ipcInstance)
{
    *ipcInstance = 0x0;
    return;
}

int32_t AddDevAuthServiceToManager(uintptr_t *serviceCtx)
{
    (void)CreateServiceInstance(serviceCtx);
    SAMGR_Bootstrap();
    InitCbStubTable();
    LOGI("AddSystemAbility to SA manager success");
    return HC_SUCCESS;
}

int32_t IpcEncodeCallReplay(uintptr_t replayCache, int32_t type, const uint8_t *result, int32_t resultSz)
{
    int32_t ret = HC_SUCCESS;
    IpcIo *replyParcel = NULL;
    unsigned long valZero = 0ul;

    replyParcel = (IpcIo *)(replayCache);
    IpcIoPushInt32(replyParcel, type);
    if ((result != NULL) && (resultSz > 0)) {
        IpcIoPushFlatObj(replyParcel, (const void *)result, (uint32_t)resultSz);
    } else {
        IpcIoPushFlatObj(replyParcel, (const void *)(&valZero), sizeof(valZero));
    }
    if (!IpcIoAvailable(replyParcel)) {
        ret = HC_ERROR;
    }
    LOGI("reply type %d, %s", type, (ret == HC_SUCCESS) ? "success" : "failed");
    return ret;
}

int32_t DecodeIpcData(uintptr_t data, int32_t *type, uint8_t **val, int32_t *valSz)
{
    IpcIo *dataPtr = NULL;

    dataPtr = (IpcIo *)(data);
    if (dataPtr->bufferLeft <= 0) {
        return HC_SUCCESS;
    }
    *type = IpcIoPopInt32(dataPtr);
    *val = (uint8_t *)IpcIoPopFlatObj(dataPtr, (uint32_t *)valSz);
    if (!IpcIoAvailable(dataPtr)) {
        return HC_ERROR;
    }
    return HC_SUCCESS;
}

void DecodeCallReply(uintptr_t callCtx, IpcDataInfo *replyCache, int32_t cacheNum)
{
    int32_t i;
    int32_t ret;
    uint32_t replyLen;

    ProxyDevAuthData *dataCache = (ProxyDevAuthData *)(callCtx);
    replyLen = IpcIoPopUint32(dataCache->reply);
    LOGI("to decode data length %u", replyLen);
    if (replyLen == 0) {
        return;
    }

    for (i = 0; i < cacheNum; i++) {
        ret = DecodeIpcData((uintptr_t)(dataCache->reply),
            &(replyCache[i].type), &(replyCache[i].val), &(replyCache[i].valSz));
        if (ret != HC_SUCCESS) {
            return;
        }
        LOGI("decode success, type %d", replyCache[i].type);
    }
    return;
}

static bool IsTypeForSettingPtr(int32_t type)
{
    int32_t typeList[] = {
        PARAM_TYPE_APPID, PARAM_TYPE_DEV_AUTH_CB, PARAM_TYPE_LISTERNER, PARAM_TYPE_CREATE_PARAMS,
        PARAM_TYPE_GROUPID, PARAM_TYPE_UDID, PARAM_TYPE_ADD_PARAMS, PARAM_TYPE_DEL_PARAMS,
        PARAM_TYPE_BIND, PARAM_TYPE_UNBIND, PARAM_TYPE_CREDENTIAL, PARAM_TYPE_MGR_APPID,
        PARAM_TYPE_FRIEND_APPID, PARAM_TYPE_QUERY_PARAMS, PARAM_TYPE_COMM_DATA, PARAM_TYPE_REQ_CFM,
        PARAM_TYPE_SESS_KEY, PARAM_TYPE_REQ_INFO, PARAM_TYPE_GROUP_INFO, PARAM_TYPE_AUTH_PARAMS
    };
    int32_t i;
    int32_t n = sizeof(typeList) / sizeof(typeList[0]);
    for (i = 0; i < n; i++) {
        if (typeList[i] == type) {
            return true;
        }
    }
    return false;
}

static bool IsTypeForCpyData(int32_t type)
{
    int32_t typeList[] = {
        PARAM_TYPE_REQID, PARAM_TYPE_GROUP_TYPE, PARAM_TYPE_OPCODE, PARAM_TYPE_ERRCODE, PARAM_TYPE_OS_ACCOUNT_ID
    };
    int32_t i;
    int32_t n = sizeof(typeList) / sizeof(typeList[0]);
    for (i = 0; i < n; i++) {
        if (typeList[i] == type) {
            return true;
        }
    }
    return false;
}

int32_t GetIpcRequestParamByType(const IpcDataInfo *ipcParams, int32_t paramNum,
    int32_t type, uint8_t *paramCache, int32_t *cacheLen)
{
    int32_t i;
    int32_t ret = HC_ERR_IPC_BAD_MSG_TYPE;
    errno_t eno;

    for (i = 0; i < paramNum; i++) {
        if (ipcParams[i].type != type) {
            continue;
        }
        ret = HC_SUCCESS;
        if (IsTypeForSettingPtr(type)) {
            *(uint8_t **)paramCache = ipcParams[i].val;
            if (cacheLen != NULL) {
                *cacheLen = ipcParams[i].valSz;
            }
            break;
        }
        if (IsTypeForCpyData(type)) {
            if ((ipcParams[i].val == NULL) || (ipcParams[i].valSz <= 0)) {
                ret = HC_ERR_INVALID_PARAMS;
                break;
            }
            eno = memcpy_s(paramCache, *cacheLen, ipcParams[i].val, ipcParams[i].valSz);
            if (eno != EOK) {
                ret = HC_ERR_MEMORY_COPY;
            }
            *cacheLen = ipcParams[i].valSz;
            break;
        }
        if ((type == PARAM_TYPE_CB_OBJECT) && (*(uint32_t *)cacheLen >= sizeof(ipcParams[i].idx))) {
            *(int32_t *)paramCache = ipcParams[i].idx;
        }
        break;
    }
    LOGI("type %d, result 0x%x", type, ret);
    return ret;
}

bool IsCallbackMethod(int32_t methodId)
{
    if ((methodId == IPC_CALL_ID_REG_CB) || (methodId == IPC_CALL_ID_REG_LISTENER) ||
        (methodId == IPC_CALL_ID_GA_PROC_DATA) || (methodId == IPC_CALL_ID_AUTH_DEVICE)) {
        return true;
    }
    return false;
}

bool IsServiceRunning(void)
{
    LOGI("service activity check");
    return g_proxyInstance != NULL ? true : false;
}

IpcIo *InitIpcDataCache(uint32_t buffSz)
{
    IpcIo *ioPtr = NULL;
    uint8_t *buf = NULL;
    uint32_t len;

    if (buffSz == 0) {
        LOGE("invalid param");
        return NULL;
    }
    len = sizeof(IpcIo) + buffSz;
    ioPtr = (IpcIo *)HcMalloc(len, 0);
    if (ioPtr == NULL) {
        LOGE("alloc memory failed");
        return NULL;
    }
    buf = (uint8_t *)ioPtr + sizeof(IpcIo);
    /* ipcio inited with 4 svc objects */
    IpcIoInit(ioPtr, (void *)buf, buffSz, 4);
    if (!IpcIoAvailable(ioPtr)) {
        LOGE("IpcIoInit failed");
        HcFree((void *)ioPtr);
        return NULL;
    }
    return ioPtr;
}

int32_t GetIpcIoDataLength(const IpcIo *io)
{
    uintptr_t beginPos;
    uintptr_t endPos;

    if ((io == NULL) || !IpcIoAvailable((IpcIo *)io)) {
        return 0;
    }
    beginPos = (uintptr_t)(io->bufferBase + IpcIoBufferOffset());
    endPos = (uintptr_t)(io->bufferCur);
    return (endPos <= beginPos) ? 0 : (int32_t)(endPos - beginPos);
}

void ShowIpcSvcInfo(const SvcIdentity *svc)
{
    LOGI("svc information - handle(%u), token(%u), cookie(%u)",
        svc->handle, svc->token, svc->cookie);
}

int32_t InitProxyAdapt(void)
{
    int32_t ret;

    if (g_proxyInstance == NULL) {
        g_proxyInstance = (IClientProxy *)GetProxyInstance(DEV_AUTH_SERVICE_NAME);
        if (g_proxyInstance == NULL) {
            LOGE("get proxy instance failed");
            return HC_ERR_IPC_INIT;
        }
        LOGI("get proxy instance success");
    }

    if (!g_sdkCbStub.registered) {
        ret = RegisterIpcCallback(CbStubOnRemoteRequest, 0, IPC_WAIT_FOREVER, &(g_sdkCbStub.stubIdentity), NULL);
        if (ret != 0) {
            LOGE("register ipc cb failed");
            return HC_ERR_IPC_INIT;
        }
        ShowIpcSvcInfo(&(g_sdkCbStub.stubIdentity));
        LOGI("register ipc cb success");
        g_sdkCbStub.registered = true;
    }
    return HC_SUCCESS;
}

void UnInitProxyAdapt(void)
{
    g_proxyInstance = NULL;
    if (UnregisterIpcCallback(g_sdkCbStub.stubIdentity)) {
        LOGW("un-register ipc cb failed");
    }
    g_sdkCbStub.registered = false;
    return;
}

int32_t IpcIoBufferOffset(void)
{
    int8_t buf[64]; /* 64 buffer size */
    IpcIo ioCache;

    IpcIoInit(&ioCache, (void *)buf, sizeof(buf), 0);
    if (ioCache.bufferCur <= ioCache.bufferBase) {
        return 0;
    }
    return (int32_t)((uintptr_t)(ioCache.bufferCur) - (uintptr_t)(ioCache.bufferBase));
}

#ifdef __cplusplus
}
#endif

