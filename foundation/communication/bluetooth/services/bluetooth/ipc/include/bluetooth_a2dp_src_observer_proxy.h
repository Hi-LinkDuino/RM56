/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_BLUETOOTH_STANDARD_A2DP_SRC_OBSERVER_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_A2DP_SRC_OBSERVER_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_a2dp_src_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothA2dpSrcObserverProxy : public IRemoteProxy<IBluetoothA2dpSourceObserver> {
public:
    explicit BluetoothA2dpSrcObserverProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IBluetoothA2dpSourceObserver>(impl)
    {}
    ~BluetoothA2dpSrcObserverProxy()
    {}

    void OnConnectionStateChanged(const RawAddress &device, int state) override;
    void OnPlayingStatusChanged(const RawAddress &device, int playingState, int error) override;
    void OnConfigurationChanged(const RawAddress &device, const BluetoothA2dpCodecInfo &info, int error) override;

private:
    ErrCode InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply);
    static inline BrokerDelegator<BluetoothA2dpSrcObserverProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_A2DP_SRC_OBSERVER_PROXY_H