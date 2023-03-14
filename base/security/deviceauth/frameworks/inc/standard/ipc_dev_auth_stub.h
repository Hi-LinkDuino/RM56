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

#ifndef IPC_DEV_AUTH_STUB_H
#define IPC_DEV_AUTH_STUB_H

#include "ipc_iface.h"
#include "ipc_adapt.h"
#include "iremote_stub.h"

namespace OHOS {
typedef struct {
    IpcServiceCall method;
    int32_t methodId;
} IpcServiceCallMap;
const int32_t MAX_CALLMAP_SIZE = 64;
const int32_t MAX_CBSTUB_SIZE = 64;

class ServiceDevAuth : public IRemoteStub<IMethodsIpcCall> {
public:
    ServiceDevAuth();
    ~ServiceDevAuth();
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data,
        MessageParcel &reply, MessageOption &option) override;
    void ResetCallMap(void);
    int32_t SetCallMap(IpcServiceCall method, int32_t methodId);
    static int32_t SetRemoteObject(sptr<IRemoteObject> &object);
    static void SetCbDeathRecipient(int32_t cbStubIdx, int32_t cbDataIdx);
    static void ResetRemoteObject(int32_t idx);
    static void ActCallback(int32_t objIdx, int32_t callbackId, bool sync,
        uintptr_t cbHook, MessageParcel &dataParcel, MessageParcel &reply);

private:
    int32_t maxCallMapSz = MAX_CALLMAP_SIZE;
    IpcServiceCallMap *callMapTable = nullptr;
    int32_t callMapElemNum = 0;
    IpcServiceCall GetCallMethodByMethodId(int32_t methodId);
};

class DevAuthDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    DevAuthDeathRecipient() {}
    explicit DevAuthDeathRecipient(int32_t cbIdx);
    ~DevAuthDeathRecipient() {}
    virtual void OnRemoteDied(const wptr<IRemoteObject>& remoteObject) override;
private:
    int32_t callbackIdx;
};
}
#endif
