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

#ifndef OHOS_BLUETOOTH_STANDARD_REMOTE_DEVICE_OBSERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_REMOTE_DEVICE_OBSERVER_INTERFACE_H

#include "../parcel/bluetooth_raw_address.h"
#include "bt_uuid.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothRemoteDeviceObserver : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothRemoteDeviceObserver");

    enum Code {
        BT_REMOTE_DEVICE_OBSERVER_PSIR_STATUS = 0,
        BT_REMOTE_DEVICE_OBSERVER_REMOTE_UUID,
        BT_REMOTE_DEVICE_OBSERVER_REMOTE_NAME,
        BT_REMOTE_DEVICE_OBSERVER_ALIAS_CHANGED,
        BT_REMOTE_DEVICE_OBSERVER_REMOTE_COD,
        BT_REMOTE_DEVICE_OBSERVER_REMOTE_BATTERY_LEVEL,
    };

    virtual void OnPairStatusChanged(const int32_t transport, const BluetoothRawAddress &device, int32_t status) = 0;
    virtual void OnRemoteUuidChanged(const BluetoothRawAddress &device, const std::vector<bluetooth::Uuid> uuids) = 0;
    virtual void OnRemoteNameChanged(const BluetoothRawAddress &device, const std::string deviceName) = 0;
    virtual void OnRemoteAliasChanged(const BluetoothRawAddress &device, const std::string alias) = 0;
    virtual void OnRemoteCodChanged(const BluetoothRawAddress &device, int32_t cod) = 0;
    virtual void OnRemoteBatteryLevelChanged(const BluetoothRawAddress &device, int32_t batteryLevel) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_REMOTE_DEVICE_OBSERVER_INTERFACE_H