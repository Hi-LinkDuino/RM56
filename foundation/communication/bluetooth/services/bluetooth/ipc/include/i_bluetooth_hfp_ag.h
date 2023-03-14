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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_AG_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_HFP_AG_INTERFACE_H

#include <string>
#include <vector>
#include "i_bluetooth_hfp_ag_observer.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothHfpAg : public OHOS::IRemoteBroker {
public:

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothHfpAg");

    enum Code {
        BT_HFP_AG_GET_CONNECT_DEVICES = 0,
        BT_HFP_AG_GET_DEVICES_BY_STATES,
        BT_HFP_AG_GET_DEVICE_STATE,
        BT_HFP_AG_CONNECT,
        BT_HFP_AG_DISCONNECT,
        BT_HFP_AG_GET_SCO_STATE,
        BT_HFP_AG_CONNECT_SCO,
        BT_HFP_AG_DISCONNECT_SCO,
        BT_HFP_AG_PHONE_STATE_CHANGED,
        BT_HFP_AG_CLCC_RESPONSE,
        BT_HFP_AG_OPEN_VOICE_RECOGNITION,
        BT_HFP_AG_CLOSE_VOICE_RECOGNITION,
        BT_HFP_AG_SET_ACTIVE_DEVICE,
        BT_HFP_AG_GET_ACTIVE_DEVICE,
        BT_HFP_AG_REGISTER_OBSERVER,
        BT_HFP_AG_DEREGISTER_OBSERVER,
        BT_HFP_AG_SEND_NO_CARRIER,
        BT_HFP_AG_INTO_MOCK,
    };

    virtual int GetConnectDevices(std::vector<BluetoothRawAddress> &devices) = 0;
    virtual int GetDevicesByStates(const std::vector<int> &states, std::vector<BluetoothRawAddress> &devices) = 0;
    virtual int GetDeviceState(const BluetoothRawAddress &device) = 0;
    virtual int Connect(const BluetoothRawAddress &device) = 0;
    virtual int Disconnect(const BluetoothRawAddress &device) = 0;
    virtual int GetScoState(const BluetoothRawAddress &device) = 0;
    virtual bool ConnectSco() = 0;
    virtual bool DisconnectSco() = 0;
    virtual void PhoneStateChanged(int numActive, int numHeld, int callState, const std::string &number, int type,
        const std::string &name) = 0;
    virtual void ClccResponse(int index, int direction, int status, int mode, bool mpty, const std::string &number, 
        int type) = 0;
    virtual bool OpenVoiceRecognition(const BluetoothRawAddress &device) = 0;
    virtual bool CloseVoiceRecognition(const BluetoothRawAddress &device) = 0;
    virtual bool SetActiveDevice(const BluetoothRawAddress &device) = 0;
    virtual bool IntoMock(const BluetoothRawAddress &device, int state) = 0;
    virtual bool SendNoCarrier(const BluetoothRawAddress &device) = 0;
    virtual std::string GetActiveDevice() = 0;
    virtual void RegisterObserver(const sptr<IBluetoothHfpAgObserver> &observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothHfpAgObserver> &observer) = 0;
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_AG_INTERFACE_H