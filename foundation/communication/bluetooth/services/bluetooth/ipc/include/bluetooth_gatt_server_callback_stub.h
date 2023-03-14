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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_SERVER_CALLBACK_STUB_H
#define OHOS_BLUETOOTH_STANDARD_GATT_SERVER_CALLBACK_STUB_H

#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_gatt_server_callback.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothGattServerCallbackStub : public IRemoteStub<IBluetoothGattServerCallback> {
public:
    BluetoothGattServerCallbackStub();
    virtual ~BluetoothGattServerCallbackStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode OnCharacteristicReadRequestInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnAddServiceInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnCharacteristicWriteRequestInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnDescriptorReadRequestInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnDescriptorWriteRequestInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnMtuChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnNotifyConfirmInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnConnectionParameterChangedInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothGattServerCallbackFunc = ErrCode (BluetoothGattServerCallbackStub::*)(
        MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothGattServerCallbackFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothGattServerCallbackStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_SERVER_STUB_H