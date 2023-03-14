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

#include "bluetooth_bt_uuid.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
#define EIGHT 8
bool BluetoothUuid::Marshalling(Parcel &parcel) const
{
    uint64_t mostSigBits =
        ((((uint64_t)uuid_[0]) << EIGHT*7) | (((uint64_t)uuid_[1]) << EIGHT*6) | (((uint64_t)uuid_[2]) << EIGHT*5) |
            (((uint64_t)uuid_[3]) << EIGHT*4) | (((uint64_t)uuid_[4]) << EIGHT*3) | (((uint64_t)uuid_[5]) << EIGHT*2) |
            (((uint64_t)uuid_[6]) << EIGHT) | uuid_[7]);

    uint64_t leastSigBits =
        ((((uint64_t)uuid_[8]) << EIGHT*7) | (((uint64_t)uuid_[9]) << EIGHT*6) | (((uint64_t)uuid_[10]) << EIGHT*5) |
            (((uint64_t)uuid_[11]) << EIGHT*4) | (((uint64_t)uuid_[12]) << EIGHT*3) | (((uint64_t)uuid_[13]) << EIGHT*2) |
            (((uint64_t)uuid_[14]) << EIGHT) | uuid_[15]);

    bool ret = parcel.WriteUint64(mostSigBits);
    if (!ret) {
        HILOGE("ParcelBtUuid::WriteToParcel write mostSigBits error");
        return ret;
    }

    return parcel.WriteUint64(leastSigBits);
}

BluetoothUuid *BluetoothUuid::Unmarshalling(Parcel &parcel)
{
    uint64_t mostSigBits = parcel.ReadUint64();
    uint64_t leastSigBits = parcel.ReadUint64();
    BluetoothUuid *uuid = new BluetoothUuid(bluetooth::Uuid::ConvertFromMostAndLeastBit(mostSigBits, leastSigBits));
    return uuid;
}
}  // namespace Bluetooth
}  // namespace OHOS