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
#ifndef OHOS_BLUETOOTH_STANDARD_HFP_HF_OBSERVER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_HFP_HF_OBSERVER_STUB_H

#include <map>
#include "iremote_stub.h"
#include "i_bluetooth_hfp_hf_observer.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHfpHfObserverStub : public IRemoteStub<IBluetoothHfpHfObserver> {
    
public:
    BluetoothHfpHfObserverStub();
    virtual ~BluetoothHfpHfObserverStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:

    ErrCode OnConnectionStateChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnScoStateChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnCallChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnSignalStrengthChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnRegistrationStatusChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnRoamingStatusChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnOperatorSelectionChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnSubscriberNumberChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnVoiceRecognitionStatusChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnInBandRingToneChangedInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothHfpHfObserverFunc = ErrCode (BluetoothHfpHfObserverStub::*)(MessageParcel &data, 
        MessageParcel &reply);
    std::map<uint32_t, BluetoothHfpHfObserverFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothHfpHfObserverStub);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_STANDARD_HFP_HF_OBSERVER_STUB_H