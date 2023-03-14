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

#ifndef IPC_DEV_AUTH_PROXY_H
#define IPC_DEV_AUTH_PROXY_H

#include "ipc_iface.h"
#include "iremote_proxy.h"
#include "ipc_callback_stub.h"

namespace OHOS {
class ProxyDevAuth : public IRemoteProxy<IMethodsIpcCall> {
public:
    explicit ProxyDevAuth(const sptr<IRemoteObject> &impl);
    ~ProxyDevAuth();
    int32_t DoCallRequest(MessageParcel &dataParcel, MessageParcel &replyParcel, bool withSync);
    static bool ServiceRunning(void);

private:
    static inline BrokerDelegator<ProxyDevAuth> delegator_;
};

class ProxyDevAuthData {
public:
    ProxyDevAuthData() = default;
    virtual ~ProxyDevAuthData() = default;
    int32_t EncodeCallRequest(int32_t type, const uint8_t *param, int32_t paramSz);
    int32_t FinalCallRequest(int32_t methodId);
    int32_t ActCall(bool withSync);
    void SetCallbackStub(sptr<IRemoteObject> cbRemote);
    MessageParcel *GetReplyParcel(void);

public:
    MessageParcel replyParcel;
private:
    MessageParcel dataParcel;
    MessageParcel tmpDataParcel;
    sptr<IRemoteObject> cbStub = nullptr;
    int32_t paramCnt = 0;
    sptr<ProxyDevAuth> GetProxy() const;
    bool withCallback = false;
};
}
#endif
