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

#ifndef NAPI_BLUETOOTH_BLE_ADVERTISE_CALLBACK_H
#define NAPI_BLUETOOTH_BLE_ADVERTISE_CALLBACK_H

#include "bluetooth_ble_advertiser.h"

namespace OHOS {
namespace Bluetooth {
class NapiBluetoothBleAdvertiseCallback : public BleAdvertiseCallback {
public:
    NapiBluetoothBleAdvertiseCallback() = default;
    virtual ~NapiBluetoothBleAdvertiseCallback() = default;

    void OnStartResultEvent(int result) override;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // NAPI_BLUETOOTH_BLE_ADVERTISE_CALLBACK_H
