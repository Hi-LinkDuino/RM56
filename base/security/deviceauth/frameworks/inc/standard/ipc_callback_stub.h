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

#ifndef IPC_CALLBACK_STUB_H
#define IPC_CALLBACK_STUB_H

#include "ipc_iface.h"
#include "iremote_stub.h"

namespace OHOS {
class StubDevAuthCb : public IRemoteStub<ICommIpcCallback> {
public:
    StubDevAuthCb();
    ~StubDevAuthCb();
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option) override;
    virtual void DoCallBack(int32_t callbackId, uintptr_t cbHook,
        MessageParcel &dataParcel, MessageParcel &reply, MessageOption &option) override;
};
}

#endif