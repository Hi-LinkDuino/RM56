/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MCE_STUB_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MCE_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_map_mce.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMceStub : public IRemoteStub<IBluetoothMapMce> {
public:
    BluetoothMapMceStub();
    virtual ~BluetoothMapMceStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode IsConnectedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectDevicesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectionStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetUnreadMessagesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetSupportedFeaturesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SendMessageInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetNotificationFilterInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetMessagesListingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetMessageInner(MessageParcel &data, MessageParcel &reply);
    ErrCode UpdateInboxInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConversationListingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetMessageStatusInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetOwnerStatusInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetOwnerStatusInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetMasInstanceInfoInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothHostFunc = ErrCode (BluetoothMapMceStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothHostFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothMapMceStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MCE_STUB_H
