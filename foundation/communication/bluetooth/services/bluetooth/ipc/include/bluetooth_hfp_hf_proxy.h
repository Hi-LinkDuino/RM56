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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_HF_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_HFP_HF_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_hfp_hf.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHfpHfProxy : public IRemoteProxy<IBluetoothHfpHf> {
public:
    explicit BluetoothHfpHfProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IBluetoothHfpHf>(impl) {};
    ~BluetoothHfpHfProxy() {};

    bool ConnectSco(const BluetoothRawAddress &device) override;
    bool DisconnectSco(const BluetoothRawAddress &device) override;
    int GetDevicesByStates(const std::vector<int> &states, std::vector<BluetoothRawAddress> &devices) override;
    int GetDeviceState(const BluetoothRawAddress &device) override;
    int GetScoState(const BluetoothRawAddress &device) override;
    bool SendDTMFTone(const BluetoothRawAddress &device, uint8_t code) override;
    int Connect(const BluetoothRawAddress &device) override;
    int Disconnect(const BluetoothRawAddress &device) override;
    bool OpenVoiceRecognition(const BluetoothRawAddress &device) override;
    bool CloseVoiceRecognition(const BluetoothRawAddress &device) override;
    int GetCurrentCallList(const BluetoothRawAddress &device, std::vector<BluetoothHfpHfCall> &calls) override;
    bool AcceptIncomingCall(const BluetoothRawAddress &device, int flag) override;
    bool HoldActiveCall(const BluetoothRawAddress &device) override;
    bool RejectIncomingCall(const BluetoothRawAddress &device) override;
    bool SendKeyPressed(const BluetoothRawAddress &device) override;
    bool HandleIncomingCall(const BluetoothRawAddress &device, int flag) override;
    bool HandleMultiCall(const BluetoothRawAddress &device, int flag, int index) override;
    bool DialLastNumber(const BluetoothRawAddress &device) override;
    bool DialMemory(const BluetoothRawAddress &device, int index) override;
    bool SendVoiceTag(const BluetoothRawAddress &device, int index) override;
    bool FinishActiveCall(const BluetoothRawAddress &device, const BluetoothHfpHfCall &call) override;
    int StartDial(const BluetoothRawAddress &device, const std::string &number, BluetoothHfpHfCall &call) override;
    void RegisterObserver(const sptr<IBluetoothHfpHfObserver> &observer) override;
    void DeregisterObserver(const sptr<IBluetoothHfpHfObserver> &observer) override;

private:
    static inline BrokerDelegator<BluetoothHfpHfProxy> delegator_;
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_HF_PROXY_H