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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_HF_STUB_H
#define OHOS_BLUETOOTH_STANDARD_HFP_HF_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_bluetooth_hfp_hf.h"

namespace OHOS {
namespace Bluetooth {
    
class BluetoothHfpHfStub : public IRemoteStub<IBluetoothHfpHf> {
 
public:
    BluetoothHfpHfStub();
    ~BluetoothHfpHfStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:

    ErrCode ConnectScoInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectScoInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetScoStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SendDTMFToneInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OpenVoiceRecognitionInner(MessageParcel &data, MessageParcel &reply);
    ErrCode CloseVoiceRecognitionInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetCurrentCallListInner(MessageParcel &data, MessageParcel &reply);
    ErrCode AcceptIncomingCallInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HoldActiveCallInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RejectIncomingCallInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SendKeyPressedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleIncomingCallInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HandleMultiCallInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DialLastNumberInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DialMemoryInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SendVoiceTagInner(MessageParcel &data, MessageParcel &reply);
    ErrCode FinishActiveCallInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StartDialInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothHfpHfFunc = ErrCode (BluetoothHfpHfStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothHfpHfFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothHfpHfStub);

};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_HF_STUB_H