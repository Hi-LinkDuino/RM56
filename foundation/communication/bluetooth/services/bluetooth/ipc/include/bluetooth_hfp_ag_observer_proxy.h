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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_AG_OBSERVER_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_HFP_AG_OBSERVER_PROXY_H

#include "i_bluetooth_hfp_ag_observer.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHfpAgObserverProxy : public IRemoteProxy<IBluetoothHfpAgObserver> {
public:
    explicit BluetoothHfpAgObserverProxy(const sptr<IRemoteObject> &impl) 
        : IRemoteProxy<IBluetoothHfpAgObserver>(impl) {};
    ~BluetoothHfpAgObserverProxy() {};

    void OnConnectionStateChanged(const BluetoothRawAddress &device, int state) override;
    void OnScoStateChanged(const BluetoothRawAddress &device, int state) override;
    void OnActiveDeviceChanged(const BluetoothRawAddress &device) override;
    void OnHfEnhancedDriverSafetyChanged(const BluetoothRawAddress &device, int indValue) override;

private:
    static inline BrokerDelegator<BluetoothHfpAgObserverProxy> delegator_;
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_AG_OBSERVER_PROXY_H