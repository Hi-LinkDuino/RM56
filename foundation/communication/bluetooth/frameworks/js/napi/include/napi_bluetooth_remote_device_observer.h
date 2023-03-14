/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NAPI_BLUETOOTH_REMOTE_DEVICE_OBSERVER_H
#define NAPI_BLUETOOTH_REMOTE_DEVICE_OBSERVER_H

#include "bluetooth_host.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {
class NapiBluetoothRemoteDeviceObserver : public BluetoothRemoteDeviceObserver {
public:
    NapiBluetoothRemoteDeviceObserver() = default;
    virtual ~NapiBluetoothRemoteDeviceObserver() = default;

    void OnPairStatusChanged(const BluetoothRemoteDevice &device, int status) override;
    void OnRemoteUuidChanged(const BluetoothRemoteDevice &device, const std::vector<ParcelUuid> &uuids) override;
    void OnRemoteNameChanged(const BluetoothRemoteDevice &device, const std::string &deviceName) override;
    void OnRemoteAliasChanged(const BluetoothRemoteDevice &device, const std::string &alias) override;
    void OnRemoteCodChanged(const BluetoothRemoteDevice &device, const BluetoothDeviceClass &cod) override;
    void OnRemoteBatteryLevelChanged(const BluetoothRemoteDevice &device, int batteryLevel) override;
    void OnReadRemoteRssiEvent(const BluetoothRemoteDevice &device, int rssi, int status) override;

private:
    void DealPairStatus(const int &status, int &boneStatus);
    void UvQueueWorkOnPairStatusChanged(uv_work_t *work, std::pair<std::string, int> &data);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // NAPI_BLUETOOTH_REMOTE_DEVICE_OBSERVER_H
