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

#ifndef OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_map_mce_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothMapMceObserverProxy : public IRemoteProxy<IBluetoothMapMceObserver> {
public:
    explicit BluetoothMapMceObserverProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothMapMceObserver>(impl)
    {}
    ~BluetoothMapMceObserverProxy()
    {}

    void OnMapActionCompleted(const BluetoothRawAddress &device, const BluetoothIProfileMapAction &action,
        int32_t status) override;
    void OnConnectionStateChanged(const BluetoothRawAddress &device, int32_t status) override;
    void OnMapEventReported(const BluetoothRawAddress &device,
        const BluetoothIProfileMapEventReport &report) override;
    void OnBmessageCompleted(const BluetoothRawAddress &device, const BluetoothIProfileBMessage &bmsg,
        int32_t status) override;
    void OnMessagesListingCompleted(const BluetoothRawAddress &device,
        const BluetoothIProfileMessagesListing &listing, int32_t status) override;
    void OnConversationListingCompleted(const BluetoothRawAddress &device,
        const BluetoothIProfileConversationListing &listing, int32_t status) override;

private:
    static inline BrokerDelegator<BluetoothMapMceObserverProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_MAP_MCE_OBSERVER_PROXY_H
