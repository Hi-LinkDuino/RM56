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

#include "ipc_dev_auth_stub.h"

#include "common_defs.h"
#include "hc_log.h"
#include "ipc_adapt.h"
#include "ipc_callback_stub.h"
#include "ipc_sdk.h"
#include "permission_adapter.h"
#include "securec.h"
#include "system_ability_definition.h"

using namespace std;
namespace OHOS {
static std::mutex g_cBMutex;

struct CbStubInfo {
    sptr<IRemoteObject> cbStub;
    bool inUse;
};
static struct CbStubInfo g_cbStub[MAX_CBSTUB_SIZE];
static bool g_cbStubInited = false;

ServiceDevAuth::ServiceDevAuth()
{}

ServiceDevAuth::~ServiceDevAuth()
{
    maxCallMapSz = MAX_CALLMAP_SIZE;
    if (callMapTable != nullptr) {
        delete[] callMapTable;
        callMapTable = nullptr;
    }
    callMapElemNum = 0;
}

IpcServiceCall ServiceDevAuth::GetCallMethodByMethodId(int32_t methodId)
{
    int32_t i;

    if (callMapTable == nullptr) {
        return nullptr;
    }

    for (i = 0; i < maxCallMapSz; i++) {
        if ((callMapTable[i].methodId == methodId) && (callMapTable[i].method != nullptr)) {
            return callMapTable[i].method;
        }
    }
    return nullptr;
}

static int32_t DecodeCallRequest(MessageParcel &data, IpcDataInfo *paramsCache, int32_t cacheNum, int32_t &inParamNum)
{
    int32_t dataLen = 0;
    int32_t i;
    int32_t ret;

    if (data.GetReadableBytes() == 0) {
        return HC_SUCCESS;
    }

    data.ReadInt32(dataLen);
    if (dataLen > static_cast<int32_t>(data.GetReadableBytes())) {
        return HC_ERR_IPC_BAD_MESSAGE_LENGTH;
    }

    data.ReadInt32(inParamNum);
    if ((inParamNum < 0) || (inParamNum > cacheNum)) {
        LOGE("param number invalid, inParamNum - %d", inParamNum);
        return HC_ERR_IPC_BAD_PARAM_NUM;
    }
    LOGI("param number: %d", inParamNum);

    for (i = 0; i < inParamNum; i++) {
        ret = DecodeIpcData(reinterpret_cast<uintptr_t>(&data), &(paramsCache[i].type),
            &(paramsCache[i].val), &(paramsCache[i].valSz));
        if (ret != HC_SUCCESS) {
            LOGE("decode failed, ret %d", ret);
            return ret;
        }
        LOGI("decode success, param type %d, val size %d", paramsCache[i].type, paramsCache[i].valSz);
    }
    return HC_SUCCESS;
}

static int32_t GetMethodId(MessageParcel &data, int32_t &methodId)
{
    if (data.GetDataSize() < sizeof(int32_t)) {
        return HC_ERR_IPC_CALL_DATA_LENGTH;
    }
    methodId = data.ReadInt32();
    LOGI("GetMethodId, id code %d", methodId);
    return HC_SUCCESS;
}

static void WithObject(int32_t methodId, MessageParcel &data, IpcDataInfo &ipcData, int32_t &cnt)
{
    if (IsCallbackMethod(methodId)) {
        ipcData.type = data.ReadInt32();
        ipcData.valSz = sizeof(StubDevAuthCb);
        sptr<IRemoteObject> tmp = data.ReadRemoteObject();
        if (!tmp) {
            LOGE("should with remote object, but read failed");
            return;
        }
        ipcData.idx = ServiceDevAuth::SetRemoteObject(tmp);
        if (ipcData.idx >= 0) {
            ipcData.val = reinterpret_cast<uint8_t *>(&(ipcData.idx));
            LOGI("object trans success, set id %d", ipcData.idx);
            cnt++;
        }
    }
    return;
}

static void InitCbStubTable()
{
    int32_t i;
    if (g_cbStubInited) {
        return;
    }
    std::lock_guard<std::mutex> autoLock(g_cBMutex);
    if (g_cbStubInited) { /* for first init at the same time */
        return;
    }
    for (i = 0; i < MAX_CBSTUB_SIZE; i++) {
        g_cbStub[i].inUse = false;
    }
    g_cbStubInited = true;
    return;
}

int32_t ServiceDevAuth::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        LOGE("The client interface token is invalid!");
        return -1;
    }
    if (CheckPermission() != HC_SUCCESS) {
        return -1;
    }
    int32_t ret = HC_ERR_IPC_UNKNOW_OPCODE;
    int32_t dataLen;
    int32_t methodId = 0;
    int32_t reqParamNum = 0;
    MessageParcel replyCache;
    IpcDataInfo reqParams[MAX_REQUEST_PARAMS_NUM] = {{0}};
    IpcServiceCall serviceCall = nullptr;

    LOGI("request code %u", code);
    switch (code) {
        case DEV_AUTH_CALL_REQUEST:
            ret = GetMethodId(data, methodId);
            if (ret != HC_SUCCESS) {
                break;
            }
            serviceCall = GetCallMethodByMethodId(methodId);
            if (serviceCall == nullptr) {
                ret = HC_ERR_IPC_METHOD_ID_INVALID;
                break;
            }
            ret = DecodeCallRequest(data, reqParams, MAX_REQUEST_PARAMS_NUM, reqParamNum);
            if (ret != HC_SUCCESS) {
                break;
            }
            if (reqParamNum < (MAX_REQUEST_PARAMS_NUM - 1)) {
                InitCbStubTable();
                WithObject(methodId, data, reqParams[reqParamNum], reqParamNum);
            }
            ret = serviceCall(reqParams, reqParamNum, reinterpret_cast<uintptr_t>(&replyCache));
            break;
        default:
            break;
    }
    reply.WriteInt32(ret);
    dataLen = replyCache.GetDataSize();
    if (dataLen > 0) {
        reply.WriteInt32(dataLen);
        reply.WriteBuffer(reinterpret_cast<const void *>(replyCache.GetData()), dataLen);
    }
    LOGI("done, request code %u, method id %d, call result %d", code, methodId, ret);
    return 0;
}

int32_t ServiceDevAuth::SetCallMap(IpcServiceCall method, int32_t methodId)
{
    int32_t len;
    errno_t eno;
    IpcServiceCallMap *callMapTmp = nullptr;

    if ((1 + callMapElemNum) > maxCallMapSz) {
        maxCallMapSz += MAX_CALLMAP_SIZE;
        if (callMapTable != nullptr) {
            callMapTmp = callMapTable;
            callMapTable = nullptr;
        }
    }
    if (callMapTable == nullptr) {
        callMapTable = new(std::nothrow) IpcServiceCallMap[maxCallMapSz];
        if (callMapTable == nullptr) {
            return HC_ERR_ALLOC_MEMORY;
        }
        len = sizeof(IpcServiceCallMap) * maxCallMapSz;
        (void)memset_s(callMapTable, len, 0, len);
        if (callMapTmp != nullptr) {
            eno = memcpy_s(callMapTable, len, callMapTmp, (sizeof(IpcServiceCallMap) * callMapElemNum));
            if (eno != EOK) {
                delete[] callMapTable;
                callMapTable = callMapTmp;
                maxCallMapSz -= MAX_CALLMAP_SIZE;
                return HC_ERR_MEMORY_COPY;
            }
            delete[] callMapTmp;
            callMapTmp = nullptr;
        }
    }
    callMapTable[callMapElemNum].method = method;
    callMapTable[callMapElemNum].methodId = methodId;
    callMapElemNum++;
    return HC_SUCCESS;
}

int32_t ServiceDevAuth::SetRemoteObject(sptr<IRemoteObject> &object)
{
    int32_t idx = -1;
    int32_t i;

    std::lock_guard<std::mutex> autoLock(g_cBMutex);
    for (i = 0; i < MAX_CBSTUB_SIZE; i++) {
        if (!g_cbStub[i].inUse) {
            idx = i;
            break;
        }
    }
    LOGI("remote object cache index %d", idx);
    if (idx == -1) {
        return -1;
    }
    g_cbStub[idx].cbStub = object;
    g_cbStub[idx].inUse = true;
    return idx;
}

void ServiceDevAuth::SetCbDeathRecipient(int32_t objIdx, int32_t cbDataIdx)
{
    bool bRet = false;
    if ((objIdx < 0) || (objIdx >= MAX_CBSTUB_SIZE) || (!g_cbStub[objIdx].inUse)) {
        return;
    }

    std::lock_guard<std::mutex> autoLock(g_cBMutex);
    bRet = g_cbStub[objIdx].cbStub->AddDeathRecipient(new(std::nothrow) DevAuthDeathRecipient(cbDataIdx));
    LOGI("AddDeathRecipient %s, callback stub idx %d", bRet ? "success" : "failed", objIdx);
    return;
}

void ServiceDevAuth::ResetRemoteObject(int32_t idx)
{
    if ((idx >= 0) && (idx < MAX_CBSTUB_SIZE)) {
        LOGI("remote object used done, idx %d", idx);
        std::lock_guard<std::mutex> autoLock(g_cBMutex);
        g_cbStub[idx].inUse = false;
    }
    return;
}

void ServiceDevAuth::ActCallback(int32_t objIdx, int32_t callbackId, bool sync,
    uintptr_t cbHook, MessageParcel &dataParcel, MessageParcel &reply)
{
    if ((objIdx < 0) || (objIdx >= MAX_CBSTUB_SIZE) || (!g_cbStub[objIdx].inUse)) {
        LOGW("nothing to do, callback id %d, remote object id %d", callbackId, objIdx);
        return;
    }
    MessageOption option(MessageOption::TF_SYNC);
    option.SetWaitTime(DEV_AUTH_CALL_WAIT_TIME);
    if (!sync) {
        option.SetFlags(MessageOption::TF_ASYNC);
        option.SetWaitTime(0);
    }
    std::lock_guard<std::mutex> autoLock(g_cBMutex);
    sptr<ICommIpcCallback> proxy = iface_cast<ICommIpcCallback>(g_cbStub[objIdx].cbStub);
    proxy->DoCallBack(callbackId, cbHook, dataParcel, reply, option);
    return;
}

DevAuthDeathRecipient::DevAuthDeathRecipient(int32_t cbIdx)
{
    callbackIdx = cbIdx;
}

void DevAuthDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
{
    LOGI("remote is not actively, to reset local resource");
    ResetIpcCallBackNodeByNodeId(callbackIdx);
}
}
