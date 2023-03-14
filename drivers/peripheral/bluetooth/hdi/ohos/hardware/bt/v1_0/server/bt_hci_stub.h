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

#ifndef OHOS_HARDWARE_BT_V1_0_BTHCISTUB_H
#define OHOS_HARDWARE_BT_V1_0_BTHCISTUB_H

#include <message_parcel.h>
#include <message_option.h>
#include <refbase.h>
#include "bt_hci_service.h"

namespace ohos {
namespace hardware {
namespace bt {
namespace v1_0 {
using namespace OHOS;

class BtHciStub {
public:
public:
    virtual ~BtHciStub() {}

    int32_t BtHciStubInit(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t BtHciStubSendHciPacket(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t BtHciStubClose(MessageParcel& data, MessageParcel& reply, MessageOption& option);

    int32_t BtHciStubOnRemoteRequest(int cmdId, MessageParcel& data, MessageParcel& reply,
        MessageOption& option);

private:
    BtHciService service_;
    sptr<IBtHciCallbacks> hciCallbacks_ {nullptr};
};
} // v1_0
} // bt
} // hardware
} // ohos

void *BtHciStubInstance();

void BtHciStubRelease(void *obj);

int32_t BtHciServiceOnRemoteRequest(void *stub, int cmdId, struct HdfSBuf* data, struct HdfSBuf* reply);

#endif // OHOS_HARDWARE_BT_V1_0_BTHCISTUB_H

