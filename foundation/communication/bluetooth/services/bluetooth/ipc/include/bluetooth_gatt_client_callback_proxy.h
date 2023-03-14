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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_CALLBACK_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_gatt_client_callback.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothGattClientCallbackProxy : public IRemoteProxy<IBluetoothGattClientCallback> {
public:
    explicit BluetoothGattClientCallbackProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IBluetoothGattClientCallback>(impl)
    {}
    ~BluetoothGattClientCallbackProxy()
    {}

    void OnConnectionStateChanged(int32_t state, int32_t newState) override;
    void OnCharacteristicChanged(const BluetoothGattCharacteristic &characteristic) override;
    void OnCharacteristicRead(int32_t ret, const BluetoothGattCharacteristic &characteristic) override;
    void OnCharacteristicWrite(int32_t ret, const BluetoothGattCharacteristic &characteristic) override;
    void OnDescriptorRead(int32_t ret, const BluetoothGattDescriptor &descriptor) override;
    void OnDescriptorWrite(int32_t ret, const BluetoothGattDescriptor &descriptor) override;
    void OnMtuChanged(int32_t state, int32_t mtu) override;
    void OnServicesDiscovered(int32_t status) override;
    void OnConnectionParameterChanged(int32_t interval, int32_t latency, int32_t timeout, int32_t status) override;
    void OnServicesChanged(std::vector<BluetoothGattService> &service) override;

private:
    static inline BrokerDelegator<BluetoothGattClientCallbackProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_CLIENT_CALLBACK_PROXY_H