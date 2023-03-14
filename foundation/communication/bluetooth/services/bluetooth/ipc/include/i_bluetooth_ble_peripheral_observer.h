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

#ifndef OHOS_BLUETOOTH_STANDARD_BLE_PERIPHERAL_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_BLE_PERIPHERAL_OBSERVER_INTERFACE_H

#include "../../../../services/bluetooth/ipc/parcel/bluetooth_raw_address.h"

#include "iremote_broker.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothBlePeripheralObserver : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothBlePeripheralObserver");

    enum Code {
        BLE_ON_READ_REMOTE_RSSI_EVENT = 0,
        BLE_PAIR_STATUS_CHANGED,
    };

    virtual void OnReadRemoteRssiEvent(const BluetoothRawAddress &device, int rssi, int status) = 0;
    virtual void OnPairStatusChanged(const int32_t transport, const BluetoothRawAddress &device, int status) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_BLE_PERIPHERAL_OBSERVER_INTERFACE_H