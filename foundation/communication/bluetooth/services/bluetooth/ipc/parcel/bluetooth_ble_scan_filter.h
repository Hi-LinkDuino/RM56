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

#ifndef BLUETOOTH_PARCEL_BLE_SCAN_FILTER_H
#define BLUETOOTH_PARCEL_BLE_SCAN_FILTER_H

#include "ble_parcel_data.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleScanFilter : public Parcelable, public bluetooth::ScanFilter {
public:
    BluetoothBleScanFilter() = default;
    ~BluetoothBleScanFilter() = default;

    bool Marshalling(Parcel &parcel) const override;
    static BluetoothBleScanFilter *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);

private:
    bool ReadUuidFromParcel(Parcel &parcel);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_PARCEL_BLE_SCAN_FILTER_H
