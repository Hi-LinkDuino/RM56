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

#ifndef BLUETOOTH_STANDARD_BLUETOOTH_BLE_PERIPHERAL_OBSERVER_STUB_H
#define BLUETOOTH_STANDARD_BLUETOOTH_BLE_PERIPHERAL_OBSERVER_STUB_H

#include "i_bluetooth_ble_peripheral_observer.h"

#include "iremote_stub.h"
#include <map>

namespace OHOS {
namespace Bluetooth {
class BluetoothBlePeripheralObserverStub : public IRemoteStub<IBluetoothBlePeripheralObserver> {
public:
    BluetoothBlePeripheralObserverStub();
    ~BluetoothBlePeripheralObserverStub() override;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode OnReadRemoteRssiEventInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPairStatusChangedInner(MessageParcel &data, MessageParcel &reply);

    std::map<uint32_t,
        ErrCode (BluetoothBlePeripheralObserverStub::*)(MessageParcel &data, MessageParcel &reply)>
        memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothBlePeripheralObserverStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_STANDARD_BLUETOOTH_BLE_PERIPHERAL_OBSERVER_STUB_H