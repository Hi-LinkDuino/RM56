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
#ifndef BLUETOOTH_PARCEL_BLE_ADVERTISER_DATA_H
#define BLUETOOTH_PARCEL_BLE_ADVERTISER_DATA_H

#include "ble_parcel_data.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothBleAdvertiserData : public Parcelable, public bluetooth::AdvertiserData {
public:
    explicit BluetoothBleAdvertiserData() = default;
    explicit BluetoothBleAdvertiserData(const bluetooth::AdvertiserData &other) : bluetooth::AdvertiserData(other)
    {}
    explicit BluetoothBleAdvertiserData(const BluetoothBleAdvertiserData &other) : bluetooth::AdvertiserData(other)
    {}
    BluetoothBleAdvertiserData& operator=(const bluetooth::AdvertiserData &other) = delete;
    BluetoothBleAdvertiserData& operator=(const BluetoothBleAdvertiserData &other) = delete;
    ~BluetoothBleAdvertiserData() = default;

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothBleAdvertiserData *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);

private:
    bool WriteServiceUuids(Parcel &parcel) const;
    bool ReadServiceUuids(std::vector<bluetooth::Uuid> &ser_Uuids, Parcel &parcel);

    bool WriteManufacturerData(Parcel &parcel) const;
    bool ReadManufacturerData(std::map<uint16_t, std::string> &manu, Parcel &parcel);

    bool WriteServiceData(Parcel &parcel) const;
    bool ReadServiceData(std::map<bluetooth::Uuid, std::string> &serviceData, Parcel &parcel);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_PARCEL_BLE_ADVERTISER_DATA_H