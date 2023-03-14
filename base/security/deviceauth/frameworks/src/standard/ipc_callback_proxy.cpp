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

#include "ipc_callback_proxy.h"
#include "hc_log.h"
#include "ipc_adapt.h"
#include "system_ability_definition.h"

namespace OHOS {
ProxyDevAuthCb::ProxyDevAuthCb(const sptr<IRemoteObject> &impl) : IRemoteProxy<ICommIpcCallback>(impl)
{}

ProxyDevAuthCb::~ProxyDevAuthCb()
{}

void ProxyDevAuthCb::DoCallBack(int32_t callbackId, uintptr_t cbHook,
    MessageParcel &dataParcel, MessageParcel &reply, MessageOption &option)
{
    int32_t ret;
    MessageParcel data;

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        LOGE("Proxy DoCallBack Remote() is null");
        return;
    }
    (void)data.WriteInt32(callbackId);
    (void)data.WritePointer(cbHook);
    if (dataParcel.GetDataSize() > 0) {
        (void)data.WriteBuffer(reinterpret_cast<const void *>(dataParcel.GetData()), dataParcel.GetDataSize());
    }
    ret = remote->SendRequest(static_cast<uint32_t>(DEV_AUTH_CALLBACK_REQUEST), data, reply, option);
    if (ret != NO_ERROR) {
        LOGE("SendRequest is failed, error code: %d", ret);
    }
    return;
}
}