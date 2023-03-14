/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_HARDWARE_BT_V1_0_BTHCICALLBACKSSTUB_H
#define OHOS_HARDWARE_BT_V1_0_BTHCICALLBACKSSTUB_H

#include <message_parcel.h>
#include <message_option.h>
#include <refbase.h>
#include <iremote_stub.h>
#include "ibt_hci_callbacks.h"

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
using namespace OHOS;

class BtHciCallbacksStub : public IRemoteStub<IBtHciCallbacks> {
public:
    virtual ~BtHciCallbacksStub() {}

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    int32_t BtHciCallbacksStubOnInited(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t BtHciCallbacksStubOnReceivedHciPacket(MessageParcel& data, MessageParcel& reply, MessageOption& option);
};
} // v1_0
} // bt
} // hardware
} // ohos


#endif // OHOS_HARDWARE_BT_V1_0_BTHCICALLBACKSSTUB_H

