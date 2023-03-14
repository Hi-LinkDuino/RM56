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
#ifndef BLUETOOTH_PBAP_PSE_SERVER_H
#define BLUETOOTH_PBAP_PSE_SERVER_H

#include "i_bluetooth_pbap_pse.h"
#include "bluetooth_pbap_pse_stub.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothPbapPseServer : public BluetoothPbapPseStub {
public:
    /**
     * @brief constructor
     * @details constructor
     */
    explicit BluetoothPbapPseServer();

    /**
     * @brief deconstructor
     * @details deconstructor
     */
    ~BluetoothPbapPseServer() override;

    // IBluetoothPbapPse overrides:

    /**
     * @brief register observer
     * @details register observer for the service of phone book
     * @param observer the pointer that point to a IBluetoothPbapPseObserver
     * @return Status
     */
    void RegisterObserver(const sptr<IBluetoothPbapPseObserver>& observer) override;

    /**
     * @brief deregister observer
     * @details deregister observer for the service of phone book
     * @param observer the pointer that point to a IBluetoothPbapPseObserver
     * @return Status
     */
    void DeregisterObserver(const sptr<IBluetoothPbapPseObserver>& observer) override;

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return Status
     */
    void GetDevicesByStates(
                const ::std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices) override;

    /**
     * @brief get the connected remote devices
     * @details get the remote device with the connected states
     * @return Status
     */
    std::vector<BluetoothRawAddress> GetConnectedDevices() override;

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return Status
     */
    int GetDeviceState(const BluetoothRawAddress &device) override;

    /**
     * @brief disconnect device
     * @details disconnect from remote device
     * @param device  remote device
     * @return Status
     */
    int Disconnect(const BluetoothRawAddress &device) override;
    int SetConnectionStrategy(const BluetoothRawAddress &device, int32_t strategy) override;
    int GetConnectionStrategy(const BluetoothRawAddress &device) override;
    // /**
    //  * @brief grant device's connect permission
    //  * @details grant device's connect permission
    //  * @param device  remote device
    //  * @return Status
    //  */
    void GrantPermission(const BluetoothRawAddress& device, bool allow, bool save) override;
    int SetDevicePassword(
        const BluetoothRawAddress& device, const std::string &password, const std::string &userId) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothPbapPseServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PSE_SERVER_H
