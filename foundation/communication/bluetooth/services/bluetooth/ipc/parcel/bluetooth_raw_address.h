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

#ifndef OHOS_BLUETOOTH_STANDARD_BLUETOOTH_PARCEL_RAW_ADDRESS_H
#define OHOS_BLUETOOTH_STANDARD_BLUETOOTH_PARCEL_RAW_ADDRESS_H

#include "parcel.h"
#include "raw_address.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothRawAddress : public Parcelable, public bluetooth::RawAddress {
public:
    BluetoothRawAddress() = default;
    BluetoothRawAddress(const bluetooth::RawAddress &other) : bluetooth::RawAddress(other)
    {}
    BluetoothRawAddress(const BluetoothRawAddress &other) : bluetooth::RawAddress(other)
    {}
    BluetoothRawAddress(const std::string &address) : bluetooth::RawAddress(address)
    {}
    ~BluetoothRawAddress() = default;

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothRawAddress *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel);

    bool ReadFromParcel(Parcel &parcel);
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_PARCEL_RAW_ADDRESS_H