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
#ifndef NAPI_BLUETOOTH_GATT_SERVER_CALLBACK_H_
#define NAPI_BLUETOOTH_GATT_SERVER_CALLBACK_H_

#include "bluetooth_gatt_server.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
const std::string STR_BT_GATT_SERVER_CALLBACK_CHARACTERISTIC_READ = "characteristicRead";
const std::string STR_BT_GATT_SERVER_CALLBACK_CHARACTERISTIC_WRITE = "characteristicWrite";
const std::string STR_BT_GATT_SERVER_CALLBACK_DESCRIPTOR_READ = "descriptorRead";
const std::string STR_BT_GATT_SERVER_CALLBACK_DESCRIPTOR_WRITE = "descriptorWrite";
const std::string STR_BT_GATT_SERVER_CALLBACK_CONNECT_STATE_CHANGE = "connectStateChange";

class NapiGattServerCallback : public GattServerCallback {
public:
    void OnConnectionStateUpdate(const BluetoothRemoteDevice &device, int state) override;
    void OnServiceAdded(GattService *Service, int ret) override {}
    void OnCharacteristicReadRequest(const BluetoothRemoteDevice &device,
        GattCharacteristic &characteristic, int requestId) override;
    void OnCharacteristicWriteRequest(const BluetoothRemoteDevice &device,
        GattCharacteristic &characteristic, int requestId) override;
    void OnDescriptorReadRequest(const BluetoothRemoteDevice &device,
        GattDescriptor &descriptor, int requestId) override;
    void OnDescriptorWriteRequest(const BluetoothRemoteDevice &device,
        GattDescriptor &descriptor, int requestId) override;
    void OnMtuUpdate(const BluetoothRemoteDevice &device, int mtu) override {}
    void OnNotificationCharacteristicChanged(const BluetoothRemoteDevice &device,
        int result) override {}
    void OnConnectionParameterChanged(const BluetoothRemoteDevice &device,
        int interval, int latency, int timeout, int status) override {}

    void SetCallbackInfo(const std::string &type, std::shared_ptr<BluetoothCallbackInfo> callbackInfo)
    {
        HILOGI("enter, type: %{public}s", type.c_str());
        callbackInfos_[type] = callbackInfo;
    }

    NapiGattServerCallback() = default;
    virtual ~NapiGattServerCallback() = default;

private:
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> callbackInfos_ = {};
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_GATT_SERVER_CALLBACK_H_ */