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

#ifndef OHOS_BLUETOOTH_STANDARD_HOST_OBSERVER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_HOST_OBSERVER_STUB_H

#include <map>

#include "i_bluetooth_host_observer.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHostObserverStub : public IRemoteStub<IBluetoothHostObserver> {
public:
    BluetoothHostObserverStub();
    ~BluetoothHostObserverStub();

    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode OnStateChangedInner(MessageParcel &data, MessageParcel &reply);

    // ON_DIS_STA_CHANGE_CODE
    ErrCode OnDiscoveryStateChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnDiscoveryResultInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPairRequestedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnPairConfirmedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnScanModeChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnDeviceNameChangedInner(MessageParcel &data, MessageParcel &reply);
    ErrCode OnDeviceAddrChangedInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothObserverHostFunc = ErrCode (BluetoothHostObserverStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothObserverHostFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothHostObserverStub);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_HOST_OBSERVER_STUB_H