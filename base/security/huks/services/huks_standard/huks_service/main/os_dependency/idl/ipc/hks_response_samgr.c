/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include <unistd.h>

#include "hks_response.h"
#include "hks_samgr_server.h"

void HksSendResponse(const uint8_t *context, int32_t result, const struct HksBlob *response)
{
    if (context == NULL) {
        HKS_LOG_E("SendResponse NULL Pointer");
        return;
    }
    HksIpcContext *ipcContext = (HksIpcContext *)context;
    IpcIo *reply = ipcContext->reply;
    IpcIoPushInt32(reply, result);
    if (response == NULL) {
        IpcIoPushBool(reply, true);
        return;
    }
    IpcIoPushBool(reply, false);
    uint32_t len = response->size;
    uint8_t *dataBuff = (uint8_t *)HksMalloc(len);
    if (dataBuff == NULL) {
        HKS_LOG_E("malloc fail.");
        return;
    }
    if (memcpy_s(dataBuff, len, response->data, len) != EOK) {
        HKS_FREE_PTR(dataBuff);
        return;
    }
    BuffPtr responseBuff = {
        .buffSz = len,
        .buff = (void *)dataBuff
    };
    IpcIoPushDataBuffWithFree(reply, &responseBuff, HksFree);
}

int32_t HksGetProcessNameForIPC(const uint8_t *context, struct HksBlob *processName)
{
    if ((context == NULL) || (processName == NULL)) {
        HKS_LOG_D("Don't need get process name.");
        return HKS_SUCCESS;
    }

    HksIpcContext *ipcContext = (HksIpcContext *)context;
    uint32_t callingUid = (uint32_t)(ipcContext->callingUid);
    uint8_t *name = (uint8_t *)HksMalloc(sizeof(callingUid));
    if (name == NULL) {
        HKS_LOG_E("GetProcessName malloc failed.");
        return HKS_ERROR_MALLOC_FAIL;
    }

    if (memcpy_s(name, sizeof(callingUid), &callingUid, sizeof(callingUid)) != EOK) {
        HKS_FREE_PTR(name);
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }
    processName->size = sizeof(callingUid);
    processName->data = name;
    return HKS_SUCCESS;
}
