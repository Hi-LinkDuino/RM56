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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_INTERFACE_H

#include "iremote_broker.h"
#include "i_bluetooth_gatt_client_callback.h"
#include "../parcel/bluetooth_gatt_characteristic.h"
#include "../parcel/bluetooth_gatt_descriptor.h"
#include "../parcel/bluetooth_gatt_device.h"
#include "../parcel/bluetooth_gatt_service.h"
#include "../parcel/bluetooth_raw_address.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothGattClient : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothGattClient");

    enum Code {
        BT_GATT_CLIENT_REGISTER_APP = 0,
        BT_GATT_CLIENT_DEREGISTER_APP,
        BT_GATT_CLIENT_CONNECT,
        BT_GATT_CLIENT_DIS_CONNECT,
        BT_GATT_CLIENT_DISCOVERY_SERVICES,
        BT_GATT_CLIENT_READ_CHARACTERISTIC,
        BT_GATT_CLIENT_WRITE_CHARACTERISTIC,
        BT_GATT_CLIENT_SIGNED_WRITE_CHARACTERISTIC,
        BT_GATT_CLIENT_READ_DESCRIPTOR,
        BT_GATT_CLIENT_WRITE_DESCRIPTOR,
        BT_GATT_CLIENT_REQUEST_EXCHANGE_MTU,
        BT_GATT_CLIENT_GET_ALL_DEVICE,
        BT_GATT_CLIENT_REQUEST_CONNECTION_PRIORITY,
        BT_GATT_CLIENT_GET_SERVICES,
    };

    virtual int RegisterApplication(
        const sptr<IBluetoothGattClientCallback> &callback, const BluetoothRawAddress &addr, int32_t transport) = 0;
    virtual int DeregisterApplication(int32_t appId) = 0;
    virtual int Connect(int32_t appId, bool autoConnect) = 0;
    virtual int Disconnect(int32_t appId) = 0;
    virtual int DiscoveryServices(int32_t appId) = 0;
    virtual int ReadCharacteristic(int32_t appId, const BluetoothGattCharacteristic &characteristic) = 0;
    virtual int WriteCharacteristic(
        int32_t appId, BluetoothGattCharacteristic *characteristic, bool withoutRespond) = 0;
    virtual int SignedWriteCharacteristic(int32_t appId, BluetoothGattCharacteristic *characteristic) = 0;
    virtual int ReadDescriptor(int32_t appId, const BluetoothGattDescriptor &descriptor) = 0;
    virtual int WriteDescriptor(int32_t appId, BluetoothGattDescriptor *descriptor) = 0;
    virtual int RequestExchangeMtu(int32_t appId, int32_t mtu) = 0;
    virtual void GetAllDevice(std::vector<BluetoothGattDevice> &device) = 0;
    virtual int RequestConnectionPriority(int32_t appId, int32_t connPriority) = 0;
    virtual void GetServices(int32_t appId, std::vector<BluetoothGattService> &service) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_INTERFACE_H