/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PCE_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PCE_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_pbap_pce.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothPbapPceProxy : public IRemoteProxy<IBluetoothPbapPce> {
public:
    explicit BluetoothPbapPceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothPbapPce>(impl) {};
    ~BluetoothPbapPceProxy() {};

    int GetDeviceState(const BluetoothRawAddress &device) override;
    int Connect(const BluetoothRawAddress &device) override;
    int PullPhoneBook(const BluetoothRawAddress &device, const BluetoothIPbapPullPhoneBookParam &param) override;
    int SetPhoneBook(const BluetoothRawAddress &device, const std::u16string &name, int32_t flag) override;
    int PullvCardListing(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardListingParam &param) override;
    int PullvCardEntry(const BluetoothRawAddress &device, const BluetoothIPbapPullvCardEntryParam &param) override;
    bool IsDownloading(const BluetoothRawAddress &device) override;
    int AbortDownloading(const BluetoothRawAddress &device) override;
    int SetDevicePassword(const BluetoothRawAddress &device, const std::string &password, const std::string &userId) override;
    int Disconnect(const BluetoothRawAddress &device) override;
    int SetConnectionStrategy(const BluetoothRawAddress &device, int32_t strategy) override;
    int GetConnectionStrategy(const BluetoothRawAddress &device) override;
    void GetDevicesByStates(const std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices) override;
    void RegisterObserver(const sptr<IBluetoothPbapPceObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothPbapPceObserver> &observer) override;
    ::std::vector<BluetoothRawAddress> GetConnectedDevices() override
    {
        std::vector<BluetoothRawAddress> rawDevices;
        return rawDevices;
    }

private:
    static inline BrokerDelegator<BluetoothPbapPceProxy> delegator_;
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PCE_PROXY_H