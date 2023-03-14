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

#ifndef IPC_IFACE_H
#define IPC_IFACE_H

#include <cstdint>
#include "iremote_broker.h"
#include "message_parcel.h"

namespace OHOS {
class IMethodsIpcCall : public IRemoteBroker {
public:
    enum {
        DEV_AUTH_CALL_REQUEST = 1,
    };
    DECLARE_INTERFACE_DESCRIPTOR(u"deviceauth.IMethodsIpcCall");
};

class ICommIpcCallback : public IRemoteBroker {
public:
    enum {
        DEV_AUTH_CALLBACK_REQUEST = 1,
    };
    DECLARE_INTERFACE_DESCRIPTOR(u"deviceauth.ICommIpcCallback");
    virtual void DoCallBack(int32_t callbackId, uintptr_t cbHook,
        MessageParcel &dataParcel, MessageParcel &reply, MessageOption &option) = 0;
};

const int32_t DEV_AUTH_CALL_WAIT_TIME = 30; /* second */
}

#endif