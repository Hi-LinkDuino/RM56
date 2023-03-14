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
#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PCE_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PCE_SERVER_H

#include "i_bluetooth_pbap_pce.h"
#include "bluetooth_pbap_pce_stub.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"
#include "bluetooth_types.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothPbapPceServer : public BluetoothPbapPceStub {
public:
    /**
     * @brief constructor
     * @details constructor
     */
    explicit BluetoothPbapPceServer();

    /**
     * @brief deconstructor
     * @details deconstructor
     */
    ~BluetoothPbapPceServer() override;

    // IBluetoothPbapPce overrides:

    /**
     * @brief register observer
     * @details register observer for the service of phone book
     * @param observer the pointer that point to a IBluetoothPbapPceObserver
     * @return Status
     */
    void RegisterObserver(const sptr<IBluetoothPbapPceObserver> &observer) override;

    // /**
    //  * @brief deregister observer
    //  * @details deregister observer for the service of phone book
    //  * @param observer the pointer that point to a IBluetoothPbapPceObserver
    //  * @return Status
    //  */
    void DeregisterObserver(const sptr<IBluetoothPbapPceObserver> &observer) override;

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return Status
     */
    void GetDevicesByStates(const ::std::vector<int32_t> states, std::vector<BluetoothRawAddress> &rawDevices) override;

    /**
     * @brief get the connected remote devices
     * @details get the remote device with the connected states
     * @return Status
     */
    ::std::vector<BluetoothRawAddress> GetConnectedDevices() override;

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return Status
     */
    int GetDeviceState(const BluetoothRawAddress &device) override;

    /**
     * @brief set the strategy of device
     * @details set the strategy with the specified remote device
     * @param device  remote device
     * @param strategy  specified strategy
     * @return Status
     */
    int SetConnectionStrategy(const BluetoothRawAddress &device, int32_t strategy) override;

    /**
     * @brief get the strategy of device
     * @details get the strategy with the specified remote device
     * @param device  remote device
     * @return Status
     */
    int GetConnectionStrategy(const BluetoothRawAddress &device) override;

    /**
     * @brief connect to server
     * @details connect to phone book server
     * @param device  remote device
     * @return Status
     */
    int Connect(const BluetoothRawAddress &device) override;

    /**
     * @brief disconnect device
     * @details disconnect from remote device
     * @param device  remote device
     * @return Status
     */
    int Disconnect(const BluetoothRawAddress &device) override;
    int PullPhoneBook(const BluetoothRawAddress &device, const BluetoothIPbapPullPhoneBookParam &param) override;
    int SetPhoneBook(const BluetoothRawAddress &device, const std::u16string &name, int32_t flag) override;
    int PullvCardListing(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardListingParam &param) override;
    int PullvCardEntry(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardEntryParam &param) override;
    bool IsDownloading(const BluetoothRawAddress &device) override;
    int AbortDownloading(const BluetoothRawAddress &device) override;
    int SetDevicePassword(const BluetoothRawAddress &device, const std::string &password, const std::string &userId) override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothPbapPceServer);
};
    
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PCE_SERVER_H