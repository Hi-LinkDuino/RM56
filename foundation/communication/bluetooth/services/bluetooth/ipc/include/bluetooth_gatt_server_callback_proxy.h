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

#ifndef OHOS_BLUETOOTH_STANDARD_GATT_SERVER_CALLBACK_PROXY_H
#define OHOS_BLUETOOTH_STANDARD_GATT_SERVER_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "i_bluetooth_host.h"
#include "i_bluetooth_gatt_server_callback.h"
namespace OHOS {
namespace Bluetooth {
class BluetoothGattServerCallbackProxy : public IRemoteProxy<IBluetoothGattServerCallback> {
public:
    explicit BluetoothGattServerCallbackProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IBluetoothGattServerCallback>(impl){};
    ~BluetoothGattServerCallbackProxy(){};

    void OnCharacteristicReadRequest(
        const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic) override;
    void OnConnectionStateChanged(const BluetoothGattDevice &device, int32_t ret, int32_t state) override;
    void OnAddService(int32_t ret, const BluetoothGattService &service) override;
    void OnCharacteristicWriteRequest(const BluetoothGattDevice &device,
        const BluetoothGattCharacteristic &characteristic, bool needRespones) override;
    void OnDescriptorReadRequest(const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor) override;
    void OnDescriptorWriteRequest(
        const BluetoothGattDevice &device, const BluetoothGattDescriptor &descriptor) override;
    void OnMtuChanged(const BluetoothGattDevice &device, int32_t mtu) override;
    void OnNotifyConfirm(
        const BluetoothGattDevice &device, const BluetoothGattCharacteristic &characteristic, int result) override;
    void OnConnectionParameterChanged(
        const BluetoothGattDevice &device, int32_t interval, int32_t latency, int32_t timeout, int32_t status) override;

private:
    static inline BrokerDelegator<BluetoothGattServerCallbackProxy> delegator_;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_GATT_SERVER_CALLBACK_PROXY_H