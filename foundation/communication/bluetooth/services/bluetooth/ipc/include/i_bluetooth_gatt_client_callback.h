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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_CALLBACK_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_CALLBACK_INTERFACE_H

#include "iremote_broker.h"
#include "../parcel/bluetooth_gatt_characteristic.h"
#include "../parcel/bluetooth_gatt_descriptor.h"
#include "../parcel/bluetooth_gatt_service.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothGattClientCallback : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothGattClientCallback");

    enum Code {
        BT_GATT_CLIENT_CALLBACK_CONNECT_STATE_CHANGE = 0,
        BT_GATT_CLIENT_CALLBACK_CHARACTER_CHANGE,
        BT_GATT_CLIENT_CALLBACK_CHARACTER_READ,
        BT_GATT_CLIENT_CALLBACK_CHARACTER_WRITE,
        BT_GATT_CLIENT_CALLBACK_DESCRIPTOR_READ,
        BT_GATT_CLIENT_CALLBACK_DESCRIPTOR_WRITE,
        BT_GATT_CLIENT_CALLBACK_MTU_UPDATE,
        BT_GATT_CLIENT_CALLBACK_SERVICES_DISCOVER,
        BT_GATT_CLIENT_CALLBACK_CONNECTION_PARA_CHANGE,
        BT_GATT_CLIENT_CALLBACK_SERVICES_CHANGED,
    };

    virtual void OnConnectionStateChanged(int32_t state, int32_t newState) = 0;
    virtual void OnCharacteristicChanged(const BluetoothGattCharacteristic &characteristic) = 0;
    virtual void OnCharacteristicRead(int32_t ret, const BluetoothGattCharacteristic &characteristic) = 0;
    virtual void OnCharacteristicWrite(int32_t ret, const BluetoothGattCharacteristic &characteristic) = 0;
    virtual void OnDescriptorRead(int32_t ret, const BluetoothGattDescriptor &descriptor) = 0;
    virtual void OnDescriptorWrite(int32_t ret, const BluetoothGattDescriptor &descriptor) = 0;
    virtual void OnMtuChanged(int32_t state, int32_t mtu) = 0;
    virtual void OnServicesDiscovered(int32_t status) = 0;
    virtual void OnConnectionParameterChanged(int32_t interval, int32_t latency, int32_t timeout, int32_t status) = 0;
    virtual void OnServicesChanged(std::vector<BluetoothGattService> &service) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_CALLBACK_INTERFACE_H