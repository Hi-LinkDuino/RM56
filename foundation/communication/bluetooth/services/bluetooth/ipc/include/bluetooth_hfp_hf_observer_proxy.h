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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_HF_OBSERVER_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_HFP_HF_OBSERVER_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_hfp_hf_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHfpHfObserverProxy : public IRemoteProxy<IBluetoothHfpHfObserver> {
public:
    explicit BluetoothHfpHfObserverProxy(const sptr<IRemoteObject> &impl) 
        : IRemoteProxy<IBluetoothHfpHfObserver>(impl) {};
    ~BluetoothHfpHfObserverProxy() {};

    void OnConnectionStateChanged(const BluetoothRawAddress &device, int state) override;
    void OnScoStateChanged(const BluetoothRawAddress &device,  int state) override;
    void OnCallChanged(const BluetoothRawAddress &device, const BluetoothHfpHfCall &call) override;
    void OnSignalStrengthChanged(const BluetoothRawAddress &device,  int signal) override;
    void OnRegistrationStatusChanged(const BluetoothRawAddress &device,  int status) override;
    void OnRoamingStatusChanged(const BluetoothRawAddress &device,  int status) override;
    void OnOperatorSelectionChanged(const BluetoothRawAddress &device, const std::string &name) override;
    void OnSubscriberNumberChanged(const BluetoothRawAddress &device, const std::string &number) override;
    void OnVoiceRecognitionStatusChanged(const BluetoothRawAddress &device, int status) override;
    void OnInBandRingToneChanged(const BluetoothRawAddress &device, int status) override;

private:
    static inline BrokerDelegator<BluetoothHfpHfObserverProxy> delegator_;
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_HF_OBSERVER_PROXY_H