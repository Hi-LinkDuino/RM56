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

#ifndef OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_INTERFACE_H
#define OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_INTERFACE_H

#include "../parcel/bluetooth_ble_advertiser_data.h"
#include "../parcel/bluetooth_ble_advertiser_settings.h"
#include "bluetooth_ble_advertiser_data.h"
#include "bluetooth_ble_advertiser_settings.h"
#include "i_bluetooth_ble_advertise_callback.h"
#include "iremote_broker.h"

namespace OHOS {
namespace Bluetooth {
class IBluetoothBleAdvertiser : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.ipc.IBluetootBleAdvertiser");

    enum Code {
        BLE_REGISTER_BLE_ADVERTISER_CALLBACK = 0,
        BLE_DE_REGISTER_BLE_ADVERTISER_CALLBACK,
        BLE_START_ADVERTISING,
        BLE_START_ADVERTISING_WITH_RAWDATA,
        BLE_STOP_ADVERTISING,
        BLE_CLOSE,
        BLE_GET_ADVERTISER_HANDLE,
    };

    virtual void RegisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback) = 0;
    virtual void DeregisterBleAdvertiserCallback(const sptr<IBluetoothBleAdvertiseCallback> &callback) = 0;
    virtual void StartAdvertising(const BluetoothBleAdvertiserSettings &settings,
        const BluetoothBleAdvertiserData &advData, const BluetoothBleAdvertiserData &scanResponse,
        int32_t advHandle, bool isRawData) = 0;
    virtual void StopAdvertising(int32_t advHandle) = 0;
    virtual void Close(int32_t advHandle) = 0;
    virtual int32_t GetAdvertiserHandle() = 0;
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // OHOS_BLUETOOTH_STANDARD_BLE_ADVERTISER_INTERFACE_H