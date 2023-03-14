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
#include "hc_log.h"
#include "hc_types.h"
#include "ipc_callback_proxy.h"
#include "ipc_callback_stub.h"
#include "ipc_dev_auth_proxy.h"
#include "ipc_dev_auth_stub.h"
#include "ipc_sdk.h"
#include "ipc_service.h"
#include "iservice_registry.h"
#include "securec.h"
#include "system_ability_definition.h"

using namespace std;
using namespace OHOS;
namespace {
    static const int32_t BUFF_MAX_SZ = 128;
    static const int32_t IPC_CALL_BACK_MAX_NODES = 64;
    static const int32_t IPC_CALL_BACK_STUB_NODES = 2;
}

static sptr<StubDevAuthCb> g_sdkCbStub[IPC_CALL_BACK_STUB_NODES] = { nullptr, nullptr };

typedef void (*CallbackStub)(uintptr_t, const IpcDataInfo *, int32_t, MessageParcel &);
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
} g_ipcCallBackList = {nullptr, 0};
static std::mutex g_cbListLock;

static void SetIpcCallBackNodeDefault(IpcCallBackNode &node)
{
    (void)memset_s(&node, sizeof(IpcCallBackNode), 0, sizeof(IpcCallBackNode));
    node.proxyId = -1;
    node.nodeIdx = -1;
    return;
}

int32_t InitIpcCallBackList(void)
{
    int32_t i;

    LOGI("initializing ...");
    if (g_ipcCallBackList.ctx != nullptr) {
        LOGI("has initialized");
        return HC_SUCCESS;
    }

    g_ipcCallBackList.ctx = new(std::nothrow) IpcCallBackNode[IPC_CALL_BACK_MAX_NODES];
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("initialized failed");
        return HC_ERROR;
    }
    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        SetIpcCallBackNodeDefault(g_ipcCallBackList.ctx[i]);
    }
    g_ipcCallBackList.nodeCnt = 0;
    LOGI("initialized successful");
    return HC_SUCCESS;
}

static void ResetIpcCallBackNode(IpcCallBackNode &node)
{
    char errStr[] = "invalid";
    char *appId = errStr;
    if ((node.appId[0] != 0) && (node.appId[sizeof(node.appId) - 1] == 0)) {
        appId = node.appId;
    }
    LOGI("appid is %s ", appId);
    ServiceDevAuth::ResetRemoteObject(node.proxyId);
    SetIpcCallBackNodeDefault(node);
    return;
}

void DeInitIpcCallBackList(void)
{
    int32_t i;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        return;
    }
    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        ResetIpcCallBackNode(g_ipcCallBackList.ctx[i]);
    }
    delete[] g_ipcCallBackList.ctx;
    g_ipcCallBackList.ctx = nullptr;
    return;
}

void ResetIpcCallBackNodeByNodeId(int32_t nodeIdx)
{
    LOGI("starting..., index %d", nodeIdx);
    if ((nodeIdx < 0) || (nodeIdx >= IPC_CALL_BACK_MAX_NODES)) {
        return;
    }
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        return;
    }
    ResetIpcCallBackNode(g_ipcCallBackList.ctx[nodeIdx]);
    g_ipcCallBackList.nodeCnt--;
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
    return nullptr;
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
    return nullptr;
}

static void SetCbDeathRecipient(int32_t type, int32_t objIdx, int32_t cbDataIdx)
{
    if ((type == CB_TYPE_DEV_AUTH) || (type == CB_TYPE_LISTENER)) {
        ServiceDevAuth::SetCbDeathRecipient(objIdx, cbDataIdx);
    }
    return;
}

void AddIpcCbObjByAppId(const char *appId, int32_t objIdx, int32_t type)
{
    IpcCallBackNode *node = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("list not inited");
        return;
    }

    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        LOGE("list is full");
        return;
    }

    node = GetIpcCallBackByAppId(appId, type);
    if (node != nullptr) {
        node->proxyId = objIdx;
        SetCbDeathRecipient(type, objIdx, node->nodeIdx);
        LOGI("ipc object add success, appid: %s, proxyId %d", appId, node->proxyId);
    }
    return;
}

int32_t AddIpcCallBackByAppId(const char *appId, const uint8_t *cbPtr, int32_t cbSz, int32_t type)
{
    IpcCallBackNode *node = nullptr;
    errno_t eno;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("list not inited");
        return HC_ERROR;
    }

    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        LOGE("list is full");
        return HC_ERROR;
    }

    node = GetIpcCallBackByAppId(appId, type);
    if (node != nullptr) {
        eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
        if (eno != EOK) {
            LOGE("callback context memory copy failed");
            return HC_ERROR;
        }
        if (node->proxyId >= 0) {
            ServiceDevAuth::ResetRemoteObject(node->proxyId);
            node->proxyId = -1;
        }
        LOGI("callback add success, appid: %s", appId);
        return HC_SUCCESS;
    }

    LOGI("new callback to add, appid: %s", appId);
    node = GetFreeIpcCallBackNode();
    if (node == nullptr) {
        LOGE("get free node failed");
        return HC_ERROR;
    }
    node->cbType = type;
    eno = memcpy_s(&(node->appId), sizeof(node->appId), appId, strlen(appId) + 1);
    if (eno != EOK) {
        ResetIpcCallBackNode(*node);
        LOGE("appid memory copy failed");
        return HC_ERROR;
    }
    eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
    if (eno != EOK) {
        ResetIpcCallBackNode(*node);
        LOGE("callback context memory copy failed");
        return HC_ERROR;
    }
    node->proxyId = -1;
    g_ipcCallBackList.nodeCnt++;
    LOGI("callback add success, appid: %s, type %d", node->appId, node->cbType);
    return HC_SUCCESS;
}

void DelIpcCallBackByAppId(const char *appId, int32_t type)
{
    IpcCallBackNode *node = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if ((g_ipcCallBackList.nodeCnt <= 0) || (g_ipcCallBackList.ctx == nullptr)) {
        return;
    }

    node = GetIpcCallBackByAppId(appId, type);
    if (node != nullptr) {
        ResetIpcCallBackNode(*node);
        g_ipcCallBackList.nodeCnt--;
    }
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
    return nullptr;
}

int32_t AddReqIdByAppId(const char *appId, int64_t reqId)
{
    IpcCallBackNode *node = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("ipc callback list not inited");
        return HC_ERROR;
    }

    node = GetIpcCallBackByAppId(appId, CB_TYPE_DEV_AUTH);
    if (node == nullptr) {
        LOGE("ipc callback node not found, appid: %s", appId);
        return HC_ERROR;
    }
    node->requestId = reqId;
    node->delOnFni = 0;
    LOGI("success, appid: %s, requestId: %lld", appId, (long long)reqId);
    return HC_SUCCESS;
}

void AddIpcCbObjByReqId(int64_t reqId, int32_t objIdx, int32_t type)
{
    IpcCallBackNode *node = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("list not inited");
        return;
    }

    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        LOGE("list is full");
        return;
    }

    node = GetIpcCallBackByReqId(reqId, type);
    if (node != nullptr) {
        node->proxyId = objIdx;
        LOGI("ipc object add success, request id %lld, type %d, proxy id %d",
            (long long)reqId, type, node->proxyId);
    }
    return;
}

int32_t AddIpcCallBackByReqId(int64_t reqId, const uint8_t *cbPtr, int32_t cbSz, int32_t type)
{
    IpcCallBackNode *node = nullptr;
    errno_t eno;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("list is full");
        return HC_ERROR;
    }

    if (g_ipcCallBackList.nodeCnt >= IPC_CALL_BACK_MAX_NODES) {
        LOGE("list is full");
        return HC_ERROR;
    }

    node = GetIpcCallBackByReqId(reqId, type);
    if (node != nullptr) {
        eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
        if (eno != EOK) {
            LOGE("callback context memory copy failed");
            return HC_ERROR;
        }
        if (node->proxyId >= 0) {
            ServiceDevAuth::ResetRemoteObject(node->proxyId);
            node->proxyId = -1;
        }
        LOGI("callback replaced success, request id %lld, type %d", (long long)reqId, type);
        return HC_SUCCESS;
    }

    LOGI("new callback to add, request id %lld, type %d", (long long)reqId, type);
    node = GetFreeIpcCallBackNode();
    if (node == nullptr) {
        LOGE("get free node failed");
        return HC_ERROR;
    }
    node->cbType = type;
    node->requestId = reqId;
    eno = memcpy_s(&(node->cbCtx), sizeof(node->cbCtx), cbPtr, cbSz);
    if (eno != EOK) {
        ResetIpcCallBackNode(*node);
        LOGE("callback context memory copy failed");
        return HC_ERROR;
    }
    node->delOnFni = 1;
    node->proxyId = -1;
    g_ipcCallBackList.nodeCnt++;
    LOGI("callback added success, request id %lld, type %d", (long long)reqId, type);
    return HC_SUCCESS;
}

static void DelCallBackByReqId(int64_t reqId, int32_t type)
{
    IpcCallBackNode *node = nullptr;

    if ((g_ipcCallBackList.nodeCnt <= 0) || (g_ipcCallBackList.ctx == nullptr)) {
        return;
    }

    node = GetIpcCallBackByReqId(reqId, type);
    if ((node != nullptr) && (node->delOnFni == 1)) {
        ResetIpcCallBackNode(*node);
        g_ipcCallBackList.nodeCnt--;
    }
    return;
}

void DelIpcCallBackByReqId(int64_t reqId, int32_t type, bool withLock)
{
    if (withLock) {
        std::lock_guard<std::mutex> autoLock(g_cbListLock);
        DelCallBackByReqId(reqId, type);
        return;
    }
    DelCallBackByReqId(reqId, type);
    return;
}

static void OnTransmitStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    int64_t requestId = 0;
    int32_t inOutLen = 0;
    uint8_t *data = nullptr;
    uint32_t dataLen = 0u;
    bool bRet = false;
    bool (*onTransmitHook)(int64_t, uint8_t *, uint32_t) = nullptr;

    onTransmitHook = reinterpret_cast<bool (*)(int64_t, uint8_t *, uint32_t)>(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID,
        reinterpret_cast<uint8_t *>(&requestId), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum,
        PARAM_TYPE_COMM_DATA, (uint8_t *)&data, reinterpret_cast<int32_t *>(&dataLen));
    bRet = onTransmitHook(requestId, data, dataLen);
    (bRet == true) ? reply.WriteInt32(HC_SUCCESS) : reply.WriteInt32(HC_ERROR);
    return;
}

static void OnSessKeyStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    int64_t requestId = 0;
    int32_t inOutLen = 0;
    uint8_t *keyData = nullptr;
    uint32_t dataLen = 0u;
    void (*onSessKeyHook)(int64_t, uint8_t *, uint32_t) = nullptr;

    (void)reply;
    onSessKeyHook = reinterpret_cast<void (*)(int64_t, uint8_t *, uint32_t)>(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID,
        reinterpret_cast<uint8_t *>(&requestId), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_SESS_KEY,
        reinterpret_cast<uint8_t *>(&keyData), reinterpret_cast<int32_t *>(&dataLen));
    onSessKeyHook(requestId, keyData, dataLen);
    return;
}

static void OnFinishStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    int64_t requestId = 0;
    int32_t opCode = 0;
    int32_t inOutLen = 0;
    char *data = nullptr;
    void (*onFinishHook)(int64_t, int32_t, char *) = nullptr;

    (void)reply;
    onFinishHook = reinterpret_cast<void (*)(int64_t, int32_t, char *)>(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID,
        reinterpret_cast<uint8_t *>(&requestId), &inOutLen);
    inOutLen = sizeof(opCode);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_OPCODE,
        reinterpret_cast<uint8_t *>(&opCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_COMM_DATA,
        reinterpret_cast<uint8_t *>(&data), nullptr);
    onFinishHook(requestId, opCode, data);
    return;
}

static void OnErrorStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    int64_t requestId = 0;
    int32_t opCode = 0;
    int32_t errCode = 0;
    int32_t inOutLen = 0;
    char *errInfo = nullptr;
    void (*onErrorHook)(int64_t, int32_t, int32_t, char *) = nullptr;

    onErrorHook = reinterpret_cast<void (*)(int64_t, int32_t, int32_t, char *)>(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID,
        reinterpret_cast<uint8_t *>(&requestId), &inOutLen);
    inOutLen = sizeof(opCode);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_OPCODE,
        reinterpret_cast<uint8_t *>(&opCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_ERRCODE,
        reinterpret_cast<uint8_t *>(&errCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_ERR_INFO,
        reinterpret_cast<uint8_t *>(&errInfo), nullptr);
    onErrorHook(requestId, opCode, errCode, errInfo);
    return;
}

static void OnRequestStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    int64_t requestId = 0;
    int32_t opCode = 0;
    int32_t inOutLen = 0;
    char *reqParams = nullptr;
    char *reqResult = nullptr;
    char *(*onReqHook)(int64_t, int32_t, char *) = nullptr;

    (void)reply;
    onReqHook = reinterpret_cast<char *(*)(int64_t, int32_t, char *)>(cbHook);
    inOutLen = sizeof(requestId);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQID,
        reinterpret_cast<uint8_t *>(&requestId), &inOutLen);
    inOutLen = sizeof(opCode);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_OPCODE,
        reinterpret_cast<uint8_t *>(&opCode), &inOutLen);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_REQ_INFO,
        reinterpret_cast<uint8_t *>(&reqParams), nullptr);
    reqResult = onReqHook(requestId, opCode, reqParams);
    if (reqResult == nullptr) {
        reply.WriteInt32(HC_ERROR);
        return;
    }
    reply.WriteInt32(HC_SUCCESS);
    reply.WriteCString(const_cast<const char *>(reqResult));
    HcFree(reqResult);
    reqResult = nullptr;
    return;
}

static void OnGroupCreatedStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    const char *groupInfo = nullptr;
    void (*onGroupCreatedHook)(const char *) = nullptr;

    (void)reply;
    onGroupCreatedHook = reinterpret_cast<void (*)(const char *)>(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<uint8_t *>(&groupInfo), nullptr);
    onGroupCreatedHook(groupInfo);
    return;
}

static void OnGroupDeletedStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    const char *groupInfo = nullptr;
    void (*onDelGroupHook)(const char *) = nullptr;

    (void)reply;
    onDelGroupHook = reinterpret_cast<void (*)(const char *)>(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<uint8_t *>(&groupInfo), nullptr);
    onDelGroupHook(groupInfo);
    return;
}

static void OnDevBoundStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    const char *groupInfo = nullptr;
    const char *udid = nullptr;
    void (*onDevBoundHook)(const char *, const char *) = nullptr;

    (void)reply;
    onDevBoundHook = reinterpret_cast<void (*)(const char *, const char *)>(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, reinterpret_cast<uint8_t *>(&udid), nullptr);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<uint8_t *>(&groupInfo), nullptr);
    onDevBoundHook(udid, groupInfo);
    return;
}

static void OnDevUnboundStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    const char *groupInfo = nullptr;
    const char *udid = nullptr;
    void (*onDevUnBoundHook)(const char *, const char *) = nullptr;

    (void)reply;
    onDevUnBoundHook = reinterpret_cast<void (*)(const char *, const char *)>(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, reinterpret_cast<uint8_t *>(&udid), nullptr);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<uint8_t *>(&groupInfo), nullptr);
    onDevUnBoundHook(udid, groupInfo);
    return;
}

static void OnDevUnTrustStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    const char *udid = nullptr;
    void (*onDevUnTrustHook)(const char *) = nullptr;

    (void)reply;
    onDevUnTrustHook = reinterpret_cast<void (*)(const char *)>(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, reinterpret_cast<uint8_t *>(&udid), nullptr);
    onDevUnTrustHook(udid);
    return;
}

static void OnDelLastGroupStub(uintptr_t cbHook, const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    const char *udid = nullptr;
    int32_t groupType = 0;
    int32_t inOutLen = 0;
    void (*onDelLastGroupHook)(const char *, int32_t) = nullptr;

    (void)reply;
    onDelLastGroupHook = reinterpret_cast<void (*)(const char *, int32_t)>(cbHook);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_UDID, reinterpret_cast<uint8_t *>(&udid), nullptr);
    inOutLen = sizeof(groupType);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_GROUP_TYPE,
        reinterpret_cast<uint8_t *>(&groupType), &inOutLen);
    onDelLastGroupHook(udid, groupType);
    return;
}

static void OnTrustDevNumChangedStub(uintptr_t cbHook,
    const IpcDataInfo *cbDataCache, int32_t cacheNum, MessageParcel &reply)
{
    int32_t devNum = 0;
    int32_t inOutLen = 0;
    void (*onTrustDevNumChangedHook)(int32_t) = nullptr;

    (void)reply;
    onTrustDevNumChangedHook = reinterpret_cast<void (*)(int32_t)>(cbHook);
    inOutLen = sizeof(devNum);
    (void)GetIpcRequestParamByType(cbDataCache, cacheNum, PARAM_TYPE_DATA_NUM,
        reinterpret_cast<uint8_t *>(&devNum), &inOutLen);
    onTrustDevNumChangedHook(devNum);
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
    MessageParcel *reply = nullptr;

    reply = reinterpret_cast<MessageParcel *>(replyCtx);
    LOGI("Process call back hook, callback id %d", callbackId);
    if ((callbackId < CB_ID_ON_TRANS) || (callbackId > CB_ID_ON_TRUST_DEV_NUM_CHANGED)) {
        LOGE("Invalid call back id");
        return;
    }
    if (cbHook == 0x0) {
        LOGE("Invalid call back hook");
        return;
    }
    stubTable[callbackId - 1](cbHook, cbDataCache, cacheNum, *reply);
    LOGI("ProcCbHook done");
    return;
}

static uint32_t EncodeCallData(MessageParcel &dataParcel, int32_t type, const uint8_t *param, int32_t paramSz)
{
    const uint8_t *paramTmp = nullptr;
    int32_t zeroVal = 0;

    paramTmp = param;
    if ((param == nullptr) || (paramSz == 0)) {
        paramTmp = reinterpret_cast<const uint8_t *>(&zeroVal);
        paramSz = sizeof(zeroVal);
    }
    if (dataParcel.WriteInt32(type) && dataParcel.WriteInt32(paramSz) &&
        dataParcel.WriteBuffer(reinterpret_cast<const void *>(paramTmp), static_cast<size_t>(paramSz))) {
        return static_cast<uint32_t>(HC_SUCCESS);
    }
    return static_cast<uint32_t>(HC_ERROR);
}

/* group auth callback adapter */
static bool GaCbOnTransmitWithType(int64_t requestId, const uint8_t *data, uint32_t dataLen, int32_t type)
{
    int32_t ret = -1;
    uint32_t uRet;
    MessageParcel dataParcel;
    MessageParcel reply;
    IpcCallBackNode *node = nullptr;

    LOGI("starting ... request id: %lld, type %d", (long long)requestId, type);
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == nullptr) {
        LOGE("onTransmit hook is null, request id %lld", (long long)requestId);
        return false;
    }
    uRet = EncodeCallData(dataParcel, PARAM_TYPE_REQID,
        reinterpret_cast<const uint8_t *>(&requestId), sizeof(requestId));
    uRet |= EncodeCallData(dataParcel, PARAM_TYPE_COMM_DATA, data, dataLen);
    if (uRet != HC_SUCCESS) {
        LOGE("build trans data failed");
        return false;
    }
    ServiceDevAuth::ActCallback(node->proxyId, CB_ID_ON_TRANS, true,
        reinterpret_cast<uintptr_t>(node->cbCtx.devAuth.onTransmit), dataParcel, reply);
    LOGI("process done, request id: %lld", (long long)requestId);
    if (reply.ReadInt32(ret) && (ret == HC_SUCCESS)) {
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
    MessageParcel dataParcel;
    MessageParcel reply;
    IpcCallBackNode *node = nullptr;

    LOGI("starting ... request id: %lld, type %d", (long long)requestId, type);
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == nullptr) {
        LOGE("onSessionKeyReturned hook is null, request id %lld", (long long)requestId);
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, reinterpret_cast<uint8_t *>(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_SESS_KEY, sessKey, sessKeyLen);
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }
    ServiceDevAuth::ActCallback(node->proxyId, CB_ID_SESS_KEY_DONE, false,
        reinterpret_cast<uintptr_t>(node->cbCtx.devAuth.onSessionKeyReturned), dataParcel, reply);
    LOGI("process done, request id: %lld", (long long)requestId);
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
    MessageParcel dataParcel;
    MessageParcel reply;
    IpcCallBackNode *node = nullptr;

    LOGI("starting ... request id: %lld, type %d", (long long)requestId, type);
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == nullptr) {
        LOGE("onFinish hook is null, request id %lld", (long long)requestId);
        return;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, reinterpret_cast<uint8_t *>(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_OPCODE,
        reinterpret_cast<uint8_t *>(&operationCode), sizeof(operationCode));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_COMM_DATA,
        reinterpret_cast<const uint8_t *>(returnData), strlen(returnData) + 1);
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }
    ServiceDevAuth::ActCallback(node->proxyId, CB_ID_ON_FINISH, false,
        reinterpret_cast<uintptr_t>(node->cbCtx.devAuth.onFinish), dataParcel, reply);
    /* delete request id */
    DelIpcCallBackByReqId(requestId, type, false);
    LOGI("process done, request id: %lld", (long long)requestId);
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
    MessageParcel dataParcel;
    MessageParcel reply;
    IpcCallBackNode *node = nullptr;

    LOGI("starting ... request id: %lld, type %d", (long long)requestId, type);
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == nullptr) {
        LOGE("onError hook is null, request id %lld", (long long)requestId);
        return;
    }
    ret = EncodeCallData(dataParcel, PARAM_TYPE_REQID, reinterpret_cast<uint8_t *>(&requestId), sizeof(requestId));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_OPCODE,
        reinterpret_cast<uint8_t *>(&operationCode), sizeof(operationCode));
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_ERRCODE, reinterpret_cast<uint8_t *>(&errorCode), sizeof(errorCode));
    if (errorReturn != nullptr) {
        ret |= EncodeCallData(dataParcel, PARAM_TYPE_ERR_INFO,
            reinterpret_cast<const uint8_t *>(errorReturn), strlen(errorReturn) + 1);
    }
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }
    ServiceDevAuth::ActCallback(node->proxyId, CB_ID_ON_ERROR, false,
        reinterpret_cast<uintptr_t>(node->cbCtx.devAuth.onError), dataParcel, reply);
    /* delete request id */
    DelIpcCallBackByReqId(requestId, type, false);
    LOGI("process done, request id: %lld", (long long)requestId);
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
    int32_t ret = -1;
    uint32_t uRet;
    MessageParcel dataParcel;
    MessageParcel reply;
    const char *dPtr = nullptr;
    IpcCallBackNode *node = nullptr;

    LOGI("starting ... request id: %lld, type %d", (long long)requestId, type);
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    node = GetIpcCallBackByReqId(requestId, type);
    if (node == nullptr) {
        LOGE("onRequest hook is null, request id %lld", (long long)requestId);
        return nullptr;
    }

    uRet = EncodeCallData(dataParcel, PARAM_TYPE_REQID, reinterpret_cast<uint8_t *>(&requestId), sizeof(requestId));
    uRet |= EncodeCallData(dataParcel, PARAM_TYPE_OPCODE,
        reinterpret_cast<uint8_t *>(&operationCode), sizeof(operationCode));
    if (reqParams != nullptr) {
        uRet |= EncodeCallData(dataParcel, PARAM_TYPE_REQ_INFO,
            reinterpret_cast<const uint8_t *>(reqParams), strlen(reqParams) + 1);
    }
    if (uRet != HC_SUCCESS) {
        LOGE("build trans data failed");
        return nullptr;
    }

    ServiceDevAuth::ActCallback(node->proxyId, CB_ID_ON_REQUEST, true,
        reinterpret_cast<uintptr_t>(node->cbCtx.devAuth.onRequest), dataParcel, reply);
    if (reply.ReadInt32(ret) && (ret == HC_SUCCESS)) {
        if (reply.GetReadableBytes() == 0) {
            LOGE("onRequest has no data, but success");
            return nullptr;
        }
        dPtr = reply.ReadCString();
        LOGI("process done, request id: %lld, %s string",
             (long long)requestId, (dPtr != nullptr) ? "valid" : "invalid");
        return (dPtr != nullptr) ? strdup(dPtr) : nullptr;
    }
    return nullptr;
}

static bool CanFindCbByReqId(int64_t requestId)
{
    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    IpcCallBackNode *node = GetIpcCallBackByReqId(requestId, CB_TYPE_DEV_AUTH);
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
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    if (groupInfo == nullptr) {
        LOGE("IpcOnGroupCreated, params error");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<const uint8_t *>(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        LOGE("IpcGaCbOnRequest, build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onGroupCreated == nullptr) {
                LOGE("onGroupCreated hook is invalid");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_GROUP_CREATED,
                false, reinterpret_cast<uintptr_t>(listener->onGroupCreated), dataParcel, reply);
        }
    }
    return;
}

void IpcOnGroupDeleted(const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    if (groupInfo == nullptr) {
        LOGE("IpcOnGroupDeleted, params error");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<const uint8_t *>(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        LOGE("IpcGaCbOnRequest, build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onGroupDeleted == nullptr) {
                LOGE("onGroupDeleted hook is null");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_GROUP_DELETED,
                false, reinterpret_cast<uintptr_t>(listener->onGroupDeleted), dataParcel, reply);
        }
    }
    return;
}

void IpcOnDeviceBound(const char *peerUdid, const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    if ((peerUdid == nullptr) || (groupInfo == nullptr)) {
        LOGE("params error");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID,
        reinterpret_cast<const uint8_t *>(peerUdid), strlen(peerUdid) + 1);
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<const uint8_t *>(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onDeviceBound == nullptr) {
                LOGE("onDeviceBound hook is null");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_DEV_BOUND,
                false, reinterpret_cast<uintptr_t>(listener->onDeviceBound), dataParcel, reply);
        }
    }
    return;
}

void IpcOnDeviceUnBound(const char *peerUdid, const char *groupInfo)
{
    int32_t i;
    uint32_t ret;
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    if ((peerUdid == nullptr) || (groupInfo == nullptr)) {
        LOGE("params error");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID,
        reinterpret_cast<const uint8_t *>(peerUdid), strlen(peerUdid) + 1);
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_GROUP_INFO,
        reinterpret_cast<const uint8_t *>(groupInfo), strlen(groupInfo) + 1);
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onDeviceUnBound == nullptr) {
                LOGE("onDeviceUnBound hook is null");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_DEV_UNBOUND,
                false, reinterpret_cast<uintptr_t>(listener->onDeviceUnBound), dataParcel, reply);
        }
    }
    return;
}

void IpcOnDeviceNotTrusted(const char *peerUdid)
{
    int32_t i;
    uint32_t ret;
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    if (peerUdid == nullptr) {
        LOGE("params error");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID,
        reinterpret_cast<const uint8_t *>(peerUdid), strlen(peerUdid) + 1);
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onDeviceNotTrusted == nullptr) {
                LOGE("onDeviceNotTrusted hook is null");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_DEV_UNTRUSTED,
                false, reinterpret_cast<uintptr_t>(listener->onDeviceNotTrusted), dataParcel, reply);
        }
    }
    return;
}

void IpcOnLastGroupDeleted(const char *peerUdid, int32_t groupType)
{
    int32_t i;
    uint32_t ret;
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    if (peerUdid == nullptr) {
        LOGE("params error");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_UDID,
        reinterpret_cast<const uint8_t *>(peerUdid), strlen(peerUdid) + 1);
    ret |= EncodeCallData(dataParcel, PARAM_TYPE_GROUP_TYPE,
        reinterpret_cast<const uint8_t *>(&groupType), sizeof(groupType));
    if (ret != HC_SUCCESS) {
        LOGE("build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onLastGroupDeleted == nullptr) {
                LOGE("onLastGroupDeleted hook is null");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_LAST_GROUP_DELETED,
                false, reinterpret_cast<uintptr_t>(listener->onLastGroupDeleted), dataParcel, reply);
        }
    }
    return;
}

void IpcOnTrustedDeviceNumChanged(int32_t curTrustedDeviceNum)
{
    int32_t i;
    uint32_t ret;
    MessageParcel dataParcel;
    MessageParcel reply;
    DataChangeListener *listener = nullptr;

    std::lock_guard<std::mutex> autoLock(g_cbListLock);
    if (g_ipcCallBackList.ctx == nullptr) {
        LOGE("IpcCallBackList un-initialized");
        return;
    }

    ret = EncodeCallData(dataParcel, PARAM_TYPE_DATA_NUM,
        reinterpret_cast<const uint8_t *>(&curTrustedDeviceNum), sizeof(curTrustedDeviceNum));
    if (ret != HC_SUCCESS) {
        LOGE("IpcOnTrustedDeviceNumChanged, build trans data failed");
        return;
    }

    for (i = 0; i < IPC_CALL_BACK_MAX_NODES; i++) {
        if (g_ipcCallBackList.ctx[i].cbType == CB_TYPE_LISTENER) {
            listener = &(g_ipcCallBackList.ctx[i].cbCtx.listener);
            if (listener->onTrustedDeviceNumChanged == nullptr) {
                LOGE("onTrustedDeviceNumChanged hook is null");
                continue;
            }
            ServiceDevAuth::ActCallback(g_ipcCallBackList.ctx[i].proxyId, CB_ID_ON_TRUST_DEV_NUM_CHANGED,
                false, reinterpret_cast<uintptr_t>(listener->onTrustedDeviceNumChanged), dataParcel, reply);
        }
    }
    return;
}

void InitDeviceAuthCbCtx(DeviceAuthCallback *ctx, int32_t type)
{
    if (ctx == nullptr) {
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
    if (ctx == nullptr) {
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
    ProxyDevAuthData *dataCache = nullptr;

    (void)cbCtx;
    if (callCtx == nullptr) {
        return HC_ERR_INVALID_PARAMS;
    }

    dataCache = new(std::nothrow) ProxyDevAuthData();
    if (dataCache == nullptr) {
        LOGE("call context alloc failed");
        return HC_ERR_ALLOC_MEMORY;
    }
    *callCtx = reinterpret_cast<uintptr_t>(dataCache);
    return HC_SUCCESS;
}

void DestroyCallCtx(uintptr_t *callCtx, uintptr_t *cbCtx)
{
    ProxyDevAuthData *dataCache = nullptr;

    (void)cbCtx;
    if ((callCtx != nullptr) && (*callCtx != 0)) {
        dataCache = reinterpret_cast<ProxyDevAuthData *>(*callCtx);
        delete dataCache;
        *callCtx = 0;
    }
    return;
}

void SetCbCtxToDataCtx(uintptr_t callCtx, int32_t cbIdx)
{
    ProxyDevAuthData *dataCache = nullptr;
    sptr<IRemoteObject> remote = g_sdkCbStub[cbIdx];
    dataCache = reinterpret_cast<ProxyDevAuthData *>(callCtx);
    dataCache->SetCallbackStub(remote);
    return;
}

int32_t SetCallRequestParamInfo(uintptr_t callCtx, int32_t type, const uint8_t *param, int32_t paramSz)
{
    ProxyDevAuthData *dataCache = reinterpret_cast<ProxyDevAuthData *>(callCtx);

    return dataCache->EncodeCallRequest(type, param, paramSz);
}

int32_t DoBinderCall(uintptr_t callCtx, int32_t methodId, bool withSync)
{
    int32_t ret;
    ProxyDevAuthData *dataCache = reinterpret_cast<ProxyDevAuthData *>(callCtx);

    LOGI("proc method %d", methodId);
    ret = dataCache->FinalCallRequest(methodId);
    if (ret != HC_SUCCESS) {
        return ret;
    }
    return dataCache->ActCall(withSync);
}

/* ipc service process adapter */
uint32_t SetIpcCallMap(uintptr_t ipcInstance, IpcServiceCall method, int32_t methodId)
{
    sptr<ServiceDevAuth> service = nullptr;
    if ((method == nullptr) || (methodId <= 0)) {
        return static_cast<uint32_t>(HC_ERR_INVALID_PARAMS);
    }

    service = reinterpret_cast<ServiceDevAuth *>(ipcInstance);
    return static_cast<uint32_t>(service->SetCallMap(method, methodId));
}

int32_t CreateServiceInstance(uintptr_t *ipcInstance)
{
    ServiceDevAuth *service = nullptr;
    service = new(std::nothrow) ServiceDevAuth();
    if (service == nullptr) {
        return HC_ERR_ALLOC_MEMORY;
    }
    *ipcInstance = reinterpret_cast<uintptr_t>(service);
    return HC_SUCCESS;
}

void DestroyServiceInstance(uintptr_t *ipcInstance)
{
    sptr<ServiceDevAuth> service = nullptr;
    if (ipcInstance == nullptr) {
        return;
    }
    service = reinterpret_cast<ServiceDevAuth *>(*ipcInstance);
    if (service == nullptr) {
        return;
    }
    delete service;
    *ipcInstance = 0x0;
    return;
}

int32_t AddDevAuthServiceToManager(uintptr_t *serviceCtx)
{
    int32_t ret = ERR_OK;
    ServiceDevAuth *sPtr = nullptr;

    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysMgr == nullptr) {
        return HC_ERR_IPC_GET_SERVICE;
    }
    sPtr = new(std::nothrow) ServiceDevAuth();
    if (sPtr == nullptr) {
        return HC_ERR_ALLOC_MEMORY;
    }
    ret = sysMgr->AddSystemAbility(DEVICE_AUTH_SERVICE_ID, sPtr);
    if (ret != ERR_OK) {
        LOGE("add service failed");
        delete sPtr;
        return HC_ERROR;
    }
    *serviceCtx = reinterpret_cast<uintptr_t>(sPtr);
    LOGI("AddSystemAbility to SA manager success");
    return HC_SUCCESS;
}

int32_t IpcEncodeCallReplay(uintptr_t replayCache, int32_t type, const uint8_t *result, int32_t resultSz)
{
    int32_t errCnt = 0;
    MessageParcel *replyParcel = nullptr;
    unsigned long valZero = 0ul;

    replyParcel = reinterpret_cast<MessageParcel *>(replayCache);
    errCnt += replyParcel->WriteInt32(type) ? 0 : 1;
    errCnt += replyParcel->WriteInt32(resultSz) ? 0 : 1;
    if ((result != nullptr) && (resultSz > 0)) {
        errCnt += replyParcel->WriteBuffer(
            reinterpret_cast<const void *>(result), static_cast<size_t>(resultSz)) ? 0 : 1;
    } else {
        errCnt += replyParcel->WriteBuffer(
            reinterpret_cast<const void *>(&valZero), sizeof(unsigned long)) ? 0 : 1;
    }
    LOGI("reply type %d, %s", type, (errCnt == 0) ? "success" : "failed");
    return (errCnt == 0) ? HC_SUCCESS : HC_ERROR;
}

int32_t DecodeIpcData(uintptr_t data, int32_t *type, uint8_t **val, int32_t *valSz)
{
    MessageParcel *dataPtr = nullptr;

    dataPtr = reinterpret_cast<MessageParcel *>(data);
    if (dataPtr->GetReadableBytes() == 0) {
        return HC_SUCCESS;
    }
    if (sizeof(int32_t) > dataPtr->GetReadableBytes()) {
        return HC_ERR_IPC_BAD_MESSAGE_LENGTH;
    }
    *type = dataPtr->ReadInt32();
    *valSz = dataPtr->ReadInt32();
    if (*valSz > static_cast<int32_t>(dataPtr->GetReadableBytes())) {
        return HC_ERR_IPC_BAD_VAL_LENGTH;
    }
    *val = const_cast<uint8_t *>(dataPtr->ReadUnpadBuffer(*valSz));
    return HC_SUCCESS;
}

void DecodeCallReply(uintptr_t callCtx, IpcDataInfo *replyCache, int32_t cacheNum)
{
    int32_t dataLen = 0;
    int32_t i;
    int32_t ret;

    ProxyDevAuthData *dataCache = reinterpret_cast<ProxyDevAuthData *>(callCtx);
    MessageParcel *tmpParcel = dataCache->GetReplyParcel();
    dataLen = tmpParcel->ReadInt32();
    if ((dataLen <= 0) || (dataLen != static_cast<int32_t>(tmpParcel->GetReadableBytes()))) {
        LOGE("decode failed, data length %d", dataLen);
        return;
    }

    for (i = 0; i < cacheNum; i++) {
        ret = DecodeIpcData(reinterpret_cast<uintptr_t>(tmpParcel),
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
            if ((ipcParams[i].val == nullptr) || (ipcParams[i].valSz <= 0)) {
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
    return ProxyDevAuth::ServiceRunning();
}

void UnInitProxyAdapt(void)
{
    g_sdkCbStub[IPC_CALL_BACK_STUB_AUTH_ID] = nullptr;
    g_sdkCbStub[IPC_CALL_BACK_STUB_BIND_ID] = nullptr;
    return;
}

int32_t InitProxyAdapt(void)
{
    g_sdkCbStub[IPC_CALL_BACK_STUB_AUTH_ID] = new(std::nothrow) StubDevAuthCb;
    g_sdkCbStub[IPC_CALL_BACK_STUB_BIND_ID] = new(std::nothrow) StubDevAuthCb;
    if (!g_sdkCbStub[IPC_CALL_BACK_STUB_AUTH_ID] || !g_sdkCbStub[IPC_CALL_BACK_STUB_BIND_ID]) {
        LOGE("alloc callback stub object failed");
        UnInitProxyAdapt();
        return HC_ERR_ALLOC_MEMORY;
    }
    LOGI("init callback stub object success");
    return HC_SUCCESS;
}
