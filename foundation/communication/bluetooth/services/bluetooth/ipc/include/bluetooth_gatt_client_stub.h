/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_STUB_H
#define OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_gatt_client.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothGattClientStub : public IRemoteStub<IBluetoothGattClient> {
public:
    BluetoothGattClientStub();
    virtual ~BluetoothGattClientStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode RegisterApplicationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterApplicationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DiscoveryServicesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ReadCharacteristicInner(MessageParcel &data, MessageParcel &reply);
    ErrCode WriteCharacteristicInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SignedWriteCharacteristicInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ReadDescriptorInner(MessageParcel &data, MessageParcel &reply);
    ErrCode WriteDescriptorInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RequestExchangeMtuInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetAllDeviceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RequestConnectionPriorityInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetServicesInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothHostFunc = ErrCode (BluetoothGattClientStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothHostFunc> memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothGattClientStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_STUB_H