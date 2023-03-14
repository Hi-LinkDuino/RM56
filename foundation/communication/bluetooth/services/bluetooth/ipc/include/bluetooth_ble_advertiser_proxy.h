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

#ifndef OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_PROXY_H

#include "i_bluetooth_ble_advertiser.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleAdvertiserProxy : public IRemoteProxy<IBluetoothBleAdvertiser> {
public:
    BluetoothBleAdvertiserProxy() = delete;
    explicit BluetoothBleAdvertiserProxy(const sptr<IRemoteObject> &impl);
    ~BluetoothBleAdvertiserProxy() override;
    DISALLOW_COPY_AND_MOVE(BluetoothBleAdvertiserProxy);

    void RegisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback) override;
    void DeregisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback) override;
    void StartAdvertising(const BluetoothBleAdvertiserSettings &settings, const BluetoothBleAdvertiserData &advData,
        const BluetoothBleAdvertiserData &scanResponse, int32_t advHandle, bool isRawData) override;
    void StopAdvertising(int32_t advHandle) override;
    void Close(int32_t advHandle) override;
    int32_t GetAdvertiserHandle() override;

private:
    ErrCode InnerTransact(uint32_t code, MessageOption &flags, MessageParcel &data, MessageParcel &reply);
    static inline BrokerDelegator<BluetoothBleAdvertiserProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif