
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
#ifndef OHOS_BLUETOOTH_STANDARD_GATT_SERVER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_GATT_SERVER_INTERFACE_H

#include "i_bluetooth_gatt_server_callback.h"
#include "iremote_broker.h"
#include "../parcel/bluetooth_gatt_characteristic.h"
#include "../parcel/bluetooth_gatt_descriptor.h"
#include "../parcel/bluetooth_gatt_device.h"
#include "../parcel/bluetooth_gatt_service.h"
#include "gatt_data.h"
namespace OHOS {
namespace Bluetooth {
class IBluetoothGattServer : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothGattServer");

    enum Code {
        GATT_SERVER_CLEAR_SERVICES = 0,
        GATT_SERVER_ADD_SERVICE,
        GATT_SERVER_REGISTER,
        GATT_SERVER_DEREGISTER,
        GATT_SERVER_CANCEL_CONNECTION,
        GATT_SERVER_NOTIFY_CLIENT,
        GATT_SERVER_REMOVE_SERVICE,
        GATT_SERVER_RESPOND_CHARACTERISTIC_READ,
        GATT_SERVER_RESPOND_CHARACTERISTIC_WRITE,
        GATT_SERVER_RESPOND_DESCRIPTOR_READ,
        GATT_SERVER_RESPOND_DESCRIPTOR_WRITE,
    };

    virtual int AddService(int32_t appId, BluetoothGattService *services) = 0;
    virtual void ClearServices(int appId) = 0;
    virtual int RegisterApplication(const sptr<IBluetoothGattServerCallback> &callback) = 0;
    virtual int DeregisterApplication(int appId) = 0;
    virtual void CancelConnection(const BluetoothGattDevice &device) = 0;
    virtual int NotifyClient(
        const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, bool needConfirm) = 0;
    virtual int RemoveService(int32_t appId, const BluetoothGattService &services) = 0;
    virtual int RespondCharacteristicRead(
        const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, int32_t ret) = 0;
    virtual int RespondCharacteristicWrite(
        const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, int32_t ret) = 0;
    virtual int RespondDescriptorRead(
        const BluetoothGattDevice &device, BluetoothGattDescriptor *descriptor, int32_t ret) = 0;
    virtual int RespondDescriptorWrite(
        const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor, int32_t ret) = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_GATT_SERVER_INTERFACE_H
