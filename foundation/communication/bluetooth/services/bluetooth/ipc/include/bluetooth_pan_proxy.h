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
#ifndef OHOS_BLUETOOTH_BLUETOOTHPANPROXY_H
#define OHOS_BLUETOOTH_BLUETOOTHPANPROXY_H

#include <iremote_proxy.h>
#include "i_bluetooth_pan.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothPanProxy : public IRemoteProxy<IBluetoothPan> {
public:
    explicit BluetoothPanProxy(const sptr<IRemoteObject>& remote) : IRemoteProxy<IBluetoothPan>(remote)
    {}
    virtual ~BluetoothPanProxy()
    {}

    ErrCode Disconnect(const BluetoothRawAddress &device, bool& result) override;
    ErrCode GetDeviceState(const BluetoothRawAddress &device, int& result) override;
    ErrCode GetDevicesByStates(const std::vector<int32_t> &states, std::vector<BluetoothRawAddress>& result) override;
    ErrCode RegisterObserver(const sptr<IBluetoothPanObserver> observer) override;
    ErrCode DeregisterObserver(const sptr<IBluetoothPanObserver> observer) override;
    ErrCode SetTethering(const bool value, bool& result) override;
    ErrCode IsTetheringOn(bool& result) override;

private:
    static inline BrokerDelegator<BluetoothPanProxy> delegator_;
    bool WriteParcelableInt32Vector(const std::vector<int32_t> &parcelableVector, Parcel &reply);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // OHOS_BLUETOOTH_BLUETOOTHPANPROXY_H