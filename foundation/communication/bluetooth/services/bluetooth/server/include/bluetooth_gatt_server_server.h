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
#ifndef OHOS_BLUETOOTH_STANDARD_GATT_SERVER_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_GATT_SERVER_SERVER_H

#include "i_bluetooth_gatt_server.h"
#include "bluetooth_gatt_server_stub.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "bluetooth_types.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothGattServerServer : public BluetoothGattServerStub {
public:
    BluetoothGattServerServer();
    ~BluetoothGattServerServer() override;

    int AddService(int32_t appId, BluetoothGattService *services) override;
    void ClearServices(int appId) override;
    void CancelConnection(const BluetoothGattDevice &device) override;
    int RegisterApplication(const sptr<IBluetoothGattServerCallback> &callback) override;
    int DeregisterApplication(int32_t appId) override;
    int NotifyClient(
        const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, bool needConfirm) override;
    int RemoveService(int32_t appId, const BluetoothGattService &services) override;
    int RespondCharacteristicRead(
        const BluetoothGattDevice &device, BluetoothGattCharacteristic *characteristic, int32_t ret) override;
    int RespondCharacteristicWrite(
        const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, int32_t ret) override;
    int RespondDescriptorRead(
        const BluetoothGattDevice &device, BluetoothGattDescriptor *descriptor, int32_t ret) override;
    int RespondDescriptorWrite(
        const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor, int32_t ret) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothGattServerServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SOURCE_SERVER_H