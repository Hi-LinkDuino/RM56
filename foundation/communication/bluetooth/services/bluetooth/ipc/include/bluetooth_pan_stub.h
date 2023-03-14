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
#ifndef OHOS_BLUETOOTH_BLUETOOTHPANSTUB_H
#define OHOS_BLUETOOTH_BLUETOOTHPANSTUB_H

#include <map>
#include <iremote_stub.h>
#include "i_bluetooth_pan.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothPanStub : public IRemoteStub<IBluetoothPan> {
public:
    BluetoothPanStub();
    ~BluetoothPanStub();

    virtual int OnRemoteRequest(
        uint32_t code,
        MessageParcel& data,
        MessageParcel& reply,
        MessageOption& option) override;

private:
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode SetTetheringInner(MessageParcel &data, MessageParcel &reply);
    ErrCode IsTetheringOnInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothPanFunc = ErrCode (BluetoothPanStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothPanFunc> memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothPanStub);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_BLUETOOTHPANSTUB_H
