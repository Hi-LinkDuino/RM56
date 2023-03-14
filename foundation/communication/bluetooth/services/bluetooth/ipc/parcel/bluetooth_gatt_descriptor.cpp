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

#include "securec.h"
#include "bluetooth_bt_uuid.h"
#include "bluetooth_gatt_descriptor.h"
#include "bluetooth_log.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothGattDescriptor::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint16(handle_)) {
        return false;
    }
    if (!parcel.WriteInt32(permissions_)) {
        return false;
    }
    if (!parcel.WriteUint32(length_)) {
        return false;
    }
    for (size_t i = 0; i < length_; i++) {
        if (!parcel.WriteUint8(value_[i])) {
            return false;
        }
    }
    BluetoothUuid uuid(uuid_);
    if (!parcel.WriteParcelable(&uuid)) {
        return false;
    }
    return true;
}

BluetoothGattDescriptor *BluetoothGattDescriptor::Unmarshalling(Parcel &parcel)
{
    BluetoothGattDescriptor *descriptor = new BluetoothGattDescriptor();
    if (descriptor != nullptr && !descriptor->ReadFromParcel(parcel)) {
        delete descriptor;
        descriptor = nullptr;
    }
    return descriptor;
}

bool BluetoothGattDescriptor::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothGattDescriptor::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadUint16(handle_)) {
        return false;
    }
    if (!parcel.ReadInt32(permissions_)) {
        return false;
    }
    uint32_t length;
    if (!parcel.ReadUint32(length)) {
        return false;
    }
    length_ = length;
    uint8_t value[length_];
    for (size_t i = 0; i < length_; i++) {
        if (!parcel.ReadUint8(value[i])) {
            return false;
        }
    }
    if (length != 0) {
        value_ = std::make_unique<uint8_t[]>(length);
        if (memcpy_s(value_.get(), length, value, length_) != EOK) {
            HILOGE("BluetoothGattDescriptor::ReadFromParcel error");
            return false;
        }
    }

    std::shared_ptr<BluetoothUuid> uuid(parcel.ReadParcelable<BluetoothUuid>());
    if (!uuid) {
        return false;
    }
    uuid_ = bluetooth::Uuid(*uuid);
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
