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

#ifndef OHOS_BLUETOOTH_STANDARD_BLUETOOTH_BLE_ADVERTISER_CALLBACK_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_BLUETOOTH_BLE_ADVERTISER_CALLBACK_PROXY_H

#include "i_bluetooth_ble_advertise_callback.h"
#include "iremote_proxy.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothBleAdvertiseCallbackProxy : public IRemoteProxy<IBluetoothBleAdvertiseCallback> {
public:
    BluetoothBleAdvertiseCallbackProxy() = delete;
    explicit BluetoothBleAdvertiseCallbackProxy(const sptr<IRemoteObject> &impl);
    ~BluetoothBleAdvertiseCallbackProxy() override;
    DISALLOW_COPY_AND_MOVE(BluetoothBleAdvertiseCallbackProxy);

    void OnAutoStopAdvEvent(int32_t advHandle) override;
    void OnStartResultEvent(int32_t result, int32_t advHandle, int32_t opcode) override;

private:
    ErrCode InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply);
    static inline BrokerDelegator<BluetoothBleAdvertiseCallbackProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif