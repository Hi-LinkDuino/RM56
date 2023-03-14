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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_SERVER_H

#include <map>

#include "bluetooth_gatt_client_stub.h"
#include "bluetooth_types.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothGattClientServer : public BluetoothGattClientStub {
public:
    explicit BluetoothGattClientServer();
    ~BluetoothGattClientServer() override;

    int RegisterApplication(const sptr<IBluetoothGattClientCallback> &callback, const BluetoothRawAddress &addr,
        int32_t transport) override;
    int DeregisterApplication(int32_t appId) override;
    int Connect(int32_t appId, bool autoConnect) override;
    int Disconnect(int32_t appId) override;
    int DiscoveryServices(int32_t appId) override;
    int ReadCharacteristic(int32_t appId, const BluetoothGattCharacteristic &characteristic) override;
    int WriteCharacteristic(int32_t appId, BluetoothGattCharacteristic *characteristic, bool withoutRespond) override;
    int SignedWriteCharacteristic(int32_t appId, BluetoothGattCharacteristic *characteristic) override;
    int ReadDescriptor(int32_t appId, const BluetoothGattDescriptor &descriptor) override;
    int WriteDescriptor(int32_t appId, BluetoothGattDescriptor *descriptor) override;
    int RequestExchangeMtu(int32_t appId, int32_t mtu) override;
    void GetAllDevice(::std::vector<BluetoothGattDevice> &device) override;
    int RequestConnectionPriority(int32_t appId, int32_t connPriority) override;
    void GetServices(int32_t appId, ::std::vector<BluetoothGattService> &service) override;

private:
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothGattClientServer);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_SERVER_H