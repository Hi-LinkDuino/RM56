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

#include "ipc_callback_stub.h"
#include "common_defs.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "ipc_adapt.h"
#include "ipc_iface.h"
#include "liteipc_adapter.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

static void DoCallBack(int32_t callbackId, uintptr_t cbHook, IpcIo *data, IpcIo *reply)
{
    int32_t ret;
    int32_t i;
    IpcDataInfo cbDataCache[MAX_REQUEST_PARAMS_NUM] = {{0}};

    if (cbHook == 0x0) {
        LOGE("Invalid call back hook");
        return;
    }

    (void)IpcIoPopUint32(data); /* skip flat object length information */
    for (i = 0; i < MAX_REQUEST_PARAMS_NUM; i++) {
        ret = DecodeIpcData((uintptr_t)(data), &(cbDataCache[i].type),
            &(cbDataCache[i].val), &(cbDataCache[i].valSz));
        if (ret != HC_SUCCESS) {
            LOGE("decode failed, ret %d", ret);
            return;
        }
    }
    ProcCbHook(callbackId, cbHook, cbDataCache, MAX_REQUEST_PARAMS_NUM, (uintptr_t)(reply));
    return;
}

static int32_t CbStubOnRemoteReply(void *ipcMsg, IpcIo *reply)
{
    uint32_t flag = 0;
    int32_t ret = 0;
    IpcIo replyErr;
    uint8_t replyBuff[16] = {0}; /* length of reply buffer - 16 */

    GetFlag(ipcMsg, &flag);
    if (flag != 0) {
        LOGI("callback - async call(%u)", flag);
        return 0;
    }
    if (!IpcIoAvailable(reply)) {
        IpcIoInit(&replyErr, replyBuff, sizeof(replyBuff), 0);
        IpcIoPushInt32(&replyErr, HC_ERR_IPC_INTERNAL_FAILED);
        LOGI("callback - SendReply error code(%d)", HC_ERR_IPC_INTERNAL_FAILED);
        ret = SendReply(NULL, ipcMsg, &replyErr);
    } else {
        LOGI("callback - SendReply done");
        ret = SendReply(NULL, ipcMsg, reply);
    }
    return ret;
}

int32_t CbStubOnRemoteRequest(const IpcContext *ctx, void *ipcMsg, IpcIo *data, void *arg)
{
    uint32_t code = 0;
    int32_t callbackId;
    uintptr_t cbHook = 0x0;
    IpcIo reply;
    uint8_t replyBuff[1024] = {0}; /* length of reply buffer - 1024 */
    int32_t ret = 0;
    (void)ctx;
    (void)arg;

    LOGI("enter invoking callback...");
    if ((ipcMsg == NULL) || (data == NULL)) {
        LOGE("invalid param");
        return -1;
    }

    GetCode(ipcMsg, &code);
    LOGI("receive ipc transact code(%u)", code);
    IpcIoInit(&reply, replyBuff, sizeof(replyBuff), 0);
    switch (code) {
        case DEV_AUTH_CALLBACK_REQUEST:
            callbackId = IpcIoPopInt32(data);
            cbHook = IpcIoPopUintptr(data);
            DoCallBack(callbackId, cbHook, data, &reply);
            break;
        default:
            LOGE("Invoke callback cmd code(%u) error", code);
            break;
    }
    ret = CbStubOnRemoteReply(ipcMsg, &reply);
    LOGI("Invoke callback done, result(%d)", ret);
    return ret;
}

#ifdef __cplusplus
}
#endif
