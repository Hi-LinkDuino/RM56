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
#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MSE_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MSE_SERVER_H

#include <string>
#include <vector>

#include "i_bluetooth_map_mse.h"
#include "bluetooth_map_mse_stub.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMseServer : public BluetoothMapMseStub {
public:
    explicit BluetoothMapMseServer();
    ~BluetoothMapMseServer() override;

    // IBluetoothMapMse overrides:
    void RegisterObserver(
        const sptr<IBluetoothMapMseObserver> &observer) override;
    void DeregisterObserver(
        const sptr<IBluetoothMapMseObserver> &observer) override;
    void GetState(int32_t &ret) override;
    void Disconnect(
        const BluetoothRawAddress &device, int32_t &ret) override;
    void IsConnected(
        const BluetoothRawAddress &device, bool &ret) override;
    void GetConnectedDevices(
        std::vector<BluetoothRawAddress> &devices) override;
    void GetDevicesByStates(const std::vector<int32_t> &states,
        std::vector<BluetoothRawAddress> &devices) override;
    void GetConnectionState(
        const BluetoothRawAddress &device, int32_t &ret) override;
    void SetConnectionStrategy(
        const BluetoothRawAddress &device, int32_t strategy, bool &ret) override;
    void GetConnectionStrategy(
        const BluetoothRawAddress &device, int32_t &ret) override;
    void GrantPermission(
        const BluetoothRawAddress &device, bool allow, bool save) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothMapMseServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MSE_SERVER_H
