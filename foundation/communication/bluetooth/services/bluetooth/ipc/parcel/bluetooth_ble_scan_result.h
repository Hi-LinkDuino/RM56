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

#ifndef BLUETOOTH_PARCEL_BLE_SCAN_RESULT_H
#define BLUETOOTH_PARCEL_BLE_SCAN_RESULT_H

#include "ble_parcel_data.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleScanResult : public Parcelable, public bluetooth::ScanResult {
public:
    explicit BluetoothBleScanResult() = default;
    BluetoothBleScanResult(const bluetooth::ScanResult &other) : bluetooth::ScanResult(other)
    {}
    BluetoothBleScanResult(const BluetoothBleScanResult &other) : bluetooth::ScanResult(other)
    {}
    ~BluetoothBleScanResult() = default;

    bool Marshalling(Parcel &parcel) const override;
    static BluetoothBleScanResult *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel);
    bool ReadFromParcelInt(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);

private:
    bool WirteServiceUuidsToParcel(Parcel &parcel) const;
    bool ReadServiceUuidsFromParcel(Parcel &parcel);
    bool WirteManufacturerDataToParcel(Parcel &parcel) const;
    bool ReadManufacturerDataFromParcel(Parcel &parcel);
    bool WirteServiceDataToParcel(Parcel &parcel) const;
    bool ReadServiceDataFromParcel(Parcel &parcel);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_PARCEL_BLE_SCAN_RESULT_H