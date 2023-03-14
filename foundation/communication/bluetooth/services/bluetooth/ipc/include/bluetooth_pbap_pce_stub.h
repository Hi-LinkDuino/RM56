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

#ifndef OHOS_BLUETOOTH_STANDARD_PBAP_PCE_STUB_H
#define OHOS_BLUETOOTH_STANDARD_PBAP_PCE_STUB_H
#include <map>

#include "iremote_stub.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_pbap_pce.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothPbapPceStub : public IRemoteStub<IBluetoothPbapPce> {
public:
    BluetoothPbapPceStub();
    virtual ~BluetoothPbapPceStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode PullPhoneBookInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetPhoneBookInner(MessageParcel &data, MessageParcel &reply);
    ErrCode PullvCardListingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode PullvCardEntryInner(MessageParcel &data, MessageParcel &reply);
    ErrCode IsDownloadingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode AbortDownloadingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetDevicePasswordInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetConnectionStrategyInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothPbapPceFunc = ErrCode (BluetoothPbapPceStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothPbapPceFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(BluetoothPbapPceStub);
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_PBAP_PCE_STUB_H