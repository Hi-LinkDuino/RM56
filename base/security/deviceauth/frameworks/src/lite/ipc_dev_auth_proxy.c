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

#include "ipc_dev_auth_proxy.h"

#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "liteipc_adapter.h"
#include "ipc_adapt.h"
#include "ipc_sdk.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

const IClientProxy *GetProxyInstance(const char *serviceName)
{
    IClientProxy *clientProxy = NULL;
    IUnknown *iUnknown = NULL;
    int ret;

    iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(serviceName);
    if (iUnknown == NULL) {
        LOGE("GetDefaultFeatureApi failed");
        return NULL;
    }
    ret = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&clientProxy);
    if ((ret != 0) || (clientProxy == NULL)) {
        LOGE("QueryInterface failed, result %d", ret);
        clientProxy = NULL;
    }
    return (const IClientProxy *)clientProxy;
}

int32_t EncodeCallRequest(ProxyDevAuthData *dataCtx, int32_t type, const uint8_t *param, int32_t paramSz)
{
    IpcIo *ioPtr = NULL;
    LOGI("type %d, paramSz %d", type, paramSz);
    ioPtr = dataCtx->tmpData;
    IpcIoPushInt32(ioPtr, type);
    IpcIoPushFlatObj(ioPtr, param, paramSz);
    if (!IpcIoAvailable(ioPtr)) {
        LOGE("request data serialized failed");
        return HC_ERROR;
    }
    dataCtx->paramCnt++;
    return HC_SUCCESS;
}

int32_t FinalCallRequest(ProxyDevAuthData *dataCtx, int32_t methodId)
{
    int32_t dataLen;
    IpcIo *ioPtr = NULL;

    dataLen = GetIpcIoDataLength((const IpcIo *)(dataCtx->tmpData));
    if (dataLen <= 0) {
        LOGE("data invalid, dataLen %d", dataLen);
        return HC_ERROR;
    }
    ioPtr = dataCtx->data;
    LOGI("method id %d, param num %d, data length %d, flag %u, io offset %d",
        methodId, dataCtx->paramCnt, dataLen, ioPtr->flag, dataCtx->ioBuffOffset);
    /* request data length = number of params + params information */
    IpcIoPushInt32(ioPtr, methodId);
    IpcIoPushInt32(ioPtr, dataLen + sizeof(int32_t));
    IpcIoPushInt32(ioPtr, dataCtx->paramCnt);
    IpcIoPushFlatObj(ioPtr, (const uint8_t *)(dataCtx->tmpData->bufferBase + dataCtx->ioBuffOffset), dataLen);
    if (dataCtx->withCallback) {
        SvcIdentity badSvc = {0};
        ShowIpcSvcInfo(&(dataCtx->cbSvc));
        if ((sizeof(dataCtx->cbSvc) != sizeof(badSvc)) ||
            !memcmp(&(dataCtx->cbSvc), &badSvc, sizeof(badSvc))) {
            LOGE("ipc call with callback, but stub object invalid");
            dataCtx->withCallback = false;
            return HC_ERROR;
        }
        IpcIoPushInt32(ioPtr, PARAM_TYPE_CB_OBJECT);
        IpcIoPushSvc(ioPtr, &(dataCtx->cbSvc));
        LOGI("ipc call with callback, data flag %u", ioPtr->flag);
    }
    dataCtx->withCallback = false;
    if (!IpcIoAvailable(ioPtr)) {
        LOGE("request data serialized failed");
        return HC_ERROR;
    }
    return HC_SUCCESS;
}

static int32_t CliInvokeRetCallback(IOwner owner, int32_t code, IpcIo *reply)
{
    IpcIo *dstReply = NULL;
    errno_t eno;

    LOGI("starting...");
    if ((reply == NULL) || (owner == NULL)) {
        LOGE("invalid params");
        return -1;
    }
    if (!IpcIoAvailable(reply)) {
        LOGE("invalid reply data");
        return -1;
    }
    dstReply = (IpcIo *)owner;
    eno = memcpy_s(dstReply->bufferCur, dstReply->bufferLeft, reply->bufferCur, reply->bufferLeft);
    if (eno != EOK) {
        LOGE("data copy failed");
        dstReply->flag = 0;
        dstReply->bufferLeft = 0;
        return -1;
    }
    dstReply->bufferLeft = reply->bufferLeft;
    LOGI("done, reply data length %zu", dstReply->bufferLeft);
    return 0;
}

int32_t ActCall(const IClientProxy *clientInst, ProxyDevAuthData *dataCtx)
{
    int32_t ret;
    int32_t ipcRet;
    if (clientInst == NULL) {
        LOGE("proxy invalid");
        return HC_ERROR;
    }
    LOGI("start to invoke ipc call...");
    ipcRet = clientInst->Invoke((IClientProxy *)clientInst, DEV_AUTH_CALL_REQUEST,
        dataCtx->data, (IOwner)(dataCtx->reply), CliInvokeRetCallback);
    LOGI("invoke call done, ipc result(%d)", ipcRet);
    ret = HC_ERROR;
    if (IpcIoAvailable(dataCtx->reply)) {
        ret = IpcIoPopInt32(dataCtx->reply);
        LOGI("service call result(%d)", ret);
    }
    return ((ipcRet == 0) && (ret == HC_SUCCESS)) ? HC_SUCCESS : HC_ERR_IPC_INTERNAL_FAILED;
}

void SetCallbackStub(ProxyDevAuthData *dataCtx, const SvcIdentity *cbSvc)
{
    (void)memset_s(&(dataCtx->cbSvc), sizeof(SvcIdentity), 0, sizeof(SvcIdentity));
    if (cbSvc != NULL) {
        (void)memcpy_s(&(dataCtx->cbSvc), sizeof(SvcIdentity), cbSvc, sizeof(SvcIdentity));
    }
    dataCtx->withCallback = true;
    return;
}

#ifdef __cplusplus
}
#endif
