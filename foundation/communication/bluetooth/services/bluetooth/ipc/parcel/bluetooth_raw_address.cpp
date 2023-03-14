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

#include "bluetooth_log.h"
#include "bluetooth_raw_address.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothRawAddress::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(address_)) {
        return false;
    }
    return true;
}

BluetoothRawAddress *BluetoothRawAddress::Unmarshalling(Parcel &parcel)
{
    BluetoothRawAddress *rawAddress = new BluetoothRawAddress();
    if (rawAddress != nullptr && !rawAddress->ReadFromParcel(parcel)) {
        delete rawAddress;
        rawAddress = nullptr;
    }
    return rawAddress;
}

bool BluetoothRawAddress::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothRawAddress::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(address_)) {
        return false;
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
