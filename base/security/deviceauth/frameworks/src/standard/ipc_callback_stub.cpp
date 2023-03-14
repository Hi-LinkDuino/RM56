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

#include "ipc_callback_stub.h"
#include "common_defs.h"
#include "hc_log.h"
#include "ipc_adapt.h"
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
StubDevAuthCb::StubDevAuthCb()
{}

StubDevAuthCb::~StubDevAuthCb()
{}

void StubDevAuthCb::DoCallBack(int32_t callbackId, uintptr_t cbHook,
    MessageParcel &dataParcel, MessageParcel &reply, MessageOption &option)
{
    int32_t ret;
    int32_t i;
    MessageParcel retParcel;
    IpcDataInfo cbDataCache[MAX_REQUEST_PARAMS_NUM] = {{0}};

    if (cbHook == 0x0) {
        LOGE("Invalid call back hook");
        return;
    }

    for (i = 0; i < MAX_REQUEST_PARAMS_NUM; i++) {
        ret = DecodeIpcData(reinterpret_cast<uintptr_t>(&dataParcel), &(cbDataCache[i].type),
            &(cbDataCache[i].val), &(cbDataCache[i].valSz));
        if (ret != HC_SUCCESS) {
            LOGE("decode failed, ret %d", ret);
            return;
        }
    }
    ProcCbHook(callbackId, cbHook, cbDataCache, MAX_REQUEST_PARAMS_NUM, reinterpret_cast<uintptr_t>(&reply));
    return;
}

int32_t StubDevAuthCb::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t callbackId;
    uintptr_t cbHook = 0x0;

    LOGI("enter invoking callback...");
    switch (code) {
        case DEV_AUTH_CALLBACK_REQUEST:
            callbackId = data.ReadInt32();
            cbHook = data.ReadPointer();
            StubDevAuthCb::DoCallBack(callbackId, cbHook, data, reply, option);
            break;
        default:
            LOGE("Invoke call back cmd id error, %u", code);
            break;
    }
    LOGI("Invoke callback done");
    return 0;
}
}
