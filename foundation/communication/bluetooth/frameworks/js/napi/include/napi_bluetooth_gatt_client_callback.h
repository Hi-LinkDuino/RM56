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
#ifndef NAPI_BLUETOOTH_GATT_CLIENT_CALLBACK_H_
#define NAPI_BLUETOOTH_GATT_CLIENT_CALLBACK_H_

#include <mutex>
#include "bluetooth_gatt_client.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
static std::mutex g_gattClientCallbackInfosMutex;
const std::string STR_BT_GATT_CLIENT_CALLBACK_BLE_CHARACTERISTIC_CHANGE = "BLECharacteristicChange";
const std::string STR_BT_GATT_CLIENT_CALLBACK_BLE_CONNECTIION_STATE_CHANGE = "BLEConnectionStateChange";
class NapiGattClient;

class NapiGattClientCallback : public GattClientCallback {
public:
    void OnConnectionStateChanged(int connectionState, int ret) override;
    void OnCharacteristicChanged(const GattCharacteristic &characteristic) override;
    void OnCharacteristicReadResult(const GattCharacteristic &characteristic, int ret)  override;
    void OnCharacteristicWriteResult(const GattCharacteristic &characteristic, int ret) override {}
    void OnDescriptorReadResult(const GattDescriptor &descriptor, int ret) override;
    void OnDescriptorWriteResult(const GattDescriptor &descriptor, int ret) override {}
    void OnMtuUpdate(int mtu, int ret) override {}
    void OnServicesDiscovered(int status) override;
    void OnConnectionParameterChanged(int interval, int latency, int timeout, int status) override {}
    void OnSetNotifyCharacteristic(int status) override {}

    void SetCallbackInfo(const std::string &type, std::shared_ptr<BluetoothCallbackInfo> callbackInfo)
    {
        callbackInfos_[type] = callbackInfo;
    }
    void SetClient(NapiGattClient *client)
    {
        client_ = client;
    }
    std::shared_ptr<BluetoothCallbackInfo> GetCallbackInfo(const std::string &type)
    {
        return callbackInfos_[type];
    }
    NapiGattClientCallback() = default;
    virtual ~NapiGattClientCallback() = default;

private:
    std::map<std::string, std::shared_ptr<BluetoothCallbackInfo>> callbackInfos_ = {};
    NapiGattClient *client_ = nullptr;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_GATT_CLIENT_CALLBACK_H_ */