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

#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PSE_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PSE_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_pbap_pse.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothPbapPseProxy : public IRemoteProxy<IBluetoothPbapPse> {
public:
    explicit BluetoothPbapPseProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothPbapPse>(impl) {};
    ~BluetoothPbapPseProxy() {};

    int GetDeviceState(const BluetoothRawAddress& device) override;
    void GetDevicesByStates(
                const ::std::vector<int32_t> tmpStates, std::vector<BluetoothRawAddress> &rawDevices) override;
    int Disconnect(const BluetoothRawAddress& device) override;
    int SetConnectionStrategy(const BluetoothRawAddress& device, int32_t strategy) override;
    int GetConnectionStrategy(const BluetoothRawAddress& device) override;
    void GrantPermission(const BluetoothRawAddress& device, bool allow, bool save) override;
    int SetDevicePassword(
        const BluetoothRawAddress& device, const std::string &password, const std::string &userId) override;
    void RegisterObserver(const sptr<IBluetoothPbapPseObserver>& observer) override;
    void DeregisterObserver(const sptr<IBluetoothPbapPseObserver>& observer) override;
    std::vector<BluetoothRawAddress> GetConnectedDevices() override
    {
        std::vector<BluetoothRawAddress> rawDevices;
        return rawDevices;
    }

private:
    static inline BrokerDelegator<BluetoothPbapPseProxy> delegator_;
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PSE_PROXY_H