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
#ifndef OHOS_BLUETOOTH_BLUETOOTHOPPSTUB_H
#define OHOS_BLUETOOTH_BLUETOOTHOPPSTUB_H

#include <map>
#include <iremote_stub.h>
#include "i_bluetooth_opp.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothOppStub : public IRemoteStub<IBluetoothOpp> {
public:
    BluetoothOppStub();
    ~BluetoothOppStub();

    virtual int OnRemoteRequest(
        uint32_t code,
        MessageParcel& data,
        MessageParcel& reply,
        MessageOption& option) override;

private:
    ErrCode SendFileInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetIncomingFileConfirmationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetCurrentTransferInformationInner(MessageParcel &data, MessageParcel &reply);
    ErrCode CancelTransferInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothOppFunc = ErrCode (BluetoothOppStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothOppFunc> memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothOppStub);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_BLUETOOTHOPPSTUB_H
