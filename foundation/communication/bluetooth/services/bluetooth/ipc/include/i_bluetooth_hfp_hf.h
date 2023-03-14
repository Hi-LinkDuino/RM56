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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_HF_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_HFP_HF_INTERFACE_H

#include <string>
#include <vector>
#include "i_bluetooth_hfp_hf_observer.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothHfpHf : public OHOS::IRemoteBroker {
public:

    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetoothHfpHf");

    enum Code {
        BT_HFP_HF_CONNECT_SCO = 0,
        BT_HFP_HF_DISCONNECT_SCO,
        BT_HFP_HF_GET_DEVICES_BY_STATES,
        BT_HFP_HF_GET_DEVICE_STATE,
        BT_HFP_HF_GET_SCO_STATE,
        BT_HFP_HF_SEND_DTMF_TONE,
        BT_HFP_HF_CONNECT,
        BT_HFP_HF_DISCONNECT,
        BT_HFP_HF_OPEN_VOICE_RECOGNITION,
        BT_HFP_HF_CLOSE_VOICE_RECOGNITION,
        BT_HFP_HF_GET_CURRENT_CALL_LIST,
        BT_HFP_HF_ACCEPT_INCOMING_CALL,
        BT_HFP_HF_HOLD_ACTIVE_CALL,
        BT_HFP_HF_REJECT_INCOMING_CALL,
        BT_HFP_HF_SEND_KEY_PRESSED,
        BT_HFP_HF_HANDLE_INCOMING_CALL,
        BT_HFP_HF_HANDLE_MULLTI_CALL,
        BT_HFP_HF_DIAL_LAST_NUMBER,
        BT_HFP_HF_DIAL_MEMORY,
        BT_HFP_HF_SEND_VOICE_TAG,
        BT_HFP_HF_FINISH_ATIVE_CALL,
        BT_HFP_HF_START_DIAL,
        BT_HFP_HF_REGISTER_OBSERVER,
        BT_HFP_HF_DEREGISTER_OBSERVER,
    };

    virtual bool ConnectSco(const BluetoothRawAddress &device) = 0;
    virtual bool DisconnectSco(const BluetoothRawAddress &device) = 0;
    virtual int GetDevicesByStates(const std::vector<int> &states, std::vector<BluetoothRawAddress> &devices) = 0;
    virtual int GetDeviceState(const BluetoothRawAddress &device) = 0;
    virtual int GetScoState(const BluetoothRawAddress &device) = 0;
    virtual bool SendDTMFTone(const BluetoothRawAddress &device, uint8_t code) = 0;
    virtual int Connect(const BluetoothRawAddress &device) = 0;
    virtual int Disconnect(const BluetoothRawAddress &device) = 0;
    virtual bool OpenVoiceRecognition(const BluetoothRawAddress &device) = 0;
    virtual bool CloseVoiceRecognition(const BluetoothRawAddress &device) = 0;
    virtual int GetCurrentCallList(const BluetoothRawAddress &device, std::vector<BluetoothHfpHfCall> &calls) = 0;
    virtual bool AcceptIncomingCall(const BluetoothRawAddress &device, int flag) = 0;
    virtual bool HoldActiveCall(const BluetoothRawAddress &device) = 0;
    virtual bool RejectIncomingCall(const BluetoothRawAddress &device) = 0;
    virtual bool SendKeyPressed(const BluetoothRawAddress &device) = 0;
    virtual bool HandleIncomingCall(const BluetoothRawAddress &device, int flag) = 0;
    virtual bool HandleMultiCall(const BluetoothRawAddress &device, int flag, int index) = 0;
    virtual bool DialLastNumber(const BluetoothRawAddress &device) = 0;
    virtual bool DialMemory(const BluetoothRawAddress &device, int index) = 0;
    virtual bool SendVoiceTag(const BluetoothRawAddress &device, int index) = 0;
    virtual bool FinishActiveCall(const BluetoothRawAddress &device, const BluetoothHfpHfCall &call) = 0;
    virtual int StartDial(const BluetoothRawAddress &device, const std::string &number, BluetoothHfpHfCall &call) = 0;
    virtual void RegisterObserver(const sptr<IBluetoothHfpHfObserver> &observer) = 0;
    virtual void DeregisterObserver(const sptr<IBluetoothHfpHfObserver> &observer) = 0;

};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_HF_INTERFACE_H