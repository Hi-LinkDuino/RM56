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

#ifndef NAPI_BLUETOOTH_HFP_HF_OBSERVER_H_
#define NAPI_BLUETOOTH_HFP_HF_OBSERVER_H_

#include "bluetooth_hfp_hf.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
const std::string STR_BT_HANDS_FREE_UNIT_OBSERVER_CONNECTION_STATE_CHANGE = "connectionStateChange";
const std::string STR_BT_HANDS_FREE_UNIT_OBSERVER_SCO_STATE_CHANGE = "scoStateChange";

class NapiHandsFreeUnitObserver : public HandsFreeUnitObserver{
public:
    void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) override;
    void OnScoStateChanged(const BluetoothRemoteDevice &device, int state) override;

    void OnCallChanged(const BluetoothRemoteDevice &device, const HandsFreeUnitCall &call) override{}
    void OnSignalStrengthChanged(const BluetoothRemoteDevice &device, int signal) override{}
    void OnRegistrationStatusChanged(const BluetoothRemoteDevice &device, int status) override{}
    void OnRoamingStatusChanged(const BluetoothRemoteDevice &device, int status) override{}
    void OnOperatorSelectionChanged(const BluetoothRemoteDevice &device, const std::string &name) override{}
    void OnSubscriberNumberChanged(const BluetoothRemoteDevice &device, const std::string &number) override{}
    void OnVoiceRecognitionStatusChanged(const BluetoothRemoteDevice &device, int status) override{}
    void OnInBandRingToneChanged(const BluetoothRemoteDevice &device, int status) override{}

    NapiHandsFreeUnitObserver() = default;
    virtual ~NapiHandsFreeUnitObserver() = default;

    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> callbackInfos_ = {};
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_HFP_HF_OBSERVER_H_ */