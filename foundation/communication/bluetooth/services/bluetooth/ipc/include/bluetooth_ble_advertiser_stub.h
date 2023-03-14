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

#ifndef OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_STUB_H
#define OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_STUB_H

#include <map>

#include "i_bluetooth_ble_advertiser.h"
#include "iremote_stub.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleAdvertiserStub : public IRemoteStub<IBluetoothBleAdvertiser> {
public:
    BluetoothBleAdvertiserStub();
    ~BluetoothBleAdvertiserStub() override;
    DISALLOW_COPY_AND_MOVE(BluetoothBleAdvertiserStub);

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    static const std::map<uint32_t,
        std::function<ErrCode(BluetoothBleAdvertiserStub *, MessageParcel &, MessageParcel &)>>
        interfaces_;

    ErrCode RegisterBleAdvertiserCallbackInner(MessageParcel &data, MessageParcel &reply);
    ErrCode DeregisterBleAdvertiserCallbackInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StartAdvertisingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode StopAdvertisingInner(MessageParcel &data, MessageParcel &reply);
    ErrCode CloseInner(MessageParcel &data, MessageParcel &reply);
    ErrCode GetAdvertiserHandleInner(MessageParcel &data, MessageParcel &reply);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif