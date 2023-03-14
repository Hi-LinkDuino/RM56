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
#ifndef OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_SERVER_H
#define OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_SERVER_H

#include "bluetooth_ble_advertiser_stub.h"
#include "bluetooth_types.h"
#include "i_bluetooth_ble_advertiser.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleAdvertiserServer : public BluetoothBleAdvertiserStub {
public:
    BluetoothBleAdvertiserServer();
    ~BluetoothBleAdvertiserServer() override;

    virtual void RegisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback) override;
    virtual void DeregisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback) override;
    virtual void StartAdvertising(const BluetoothBleAdvertiserSettings &settings,
        const BluetoothBleAdvertiserData &advData, const BluetoothBleAdvertiserData &scanResponse,
        int32_t advHandle, bool isRawData) override;
    virtual void StopAdvertising(int32_t advHandle) override;
    virtual void Close(int32_t advHandle) override;
    virtual int32_t GetAdvertiserHandle() override;

private:
    BLUETOOTH_DECLARE_IMPL();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(BluetoothBleAdvertiserServer);
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_SERVER_H