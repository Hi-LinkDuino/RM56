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
#ifndef OHOS_BLUETOOTH_BLUETOOTHHIDHOSTSTUB_H
#define OHOS_BLUETOOTH_BLUETOOTHHIDHOSTSTUB_H

#include <map>
#include <iremote_stub.h>
#include "i_bluetooth_hid_host.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHidHostStub : public IRemoteStub<IBluetoothHidHost> {
public:

    BluetoothHidHostStub();
    ~BluetoothHidHostStub();

    virtual int OnRemoteRequest(
        uint32_t code,
        MessageParcel& data,
        MessageParcel& reply,
        MessageOption& option) override;

private:
    ErrCode ConnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DisconnectInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDeviceStateInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetDevicesByStatesInner(MessageParcel &data, MessageParcel &reply);
    ErrCode RegisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterObserverInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HidHostVCUnplugInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HidHostSendDataInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HidHostSetReportInner(MessageParcel &data, MessageParcel &reply);
    ErrCode HidHostGetReportInner(MessageParcel &data, MessageParcel &reply);

    using BluetoothHidHostFunc = ErrCode (BluetoothHidHostStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, BluetoothHidHostFunc> memberFuncMap_;
    DISALLOW_COPY_AND_MOVE(BluetoothHidHostStub);
};
} // Bluetooth
} // OHOS

#endif // OHOS_BLUETOOTH_BLUETOOTHHIDHOSTSTUB_H

