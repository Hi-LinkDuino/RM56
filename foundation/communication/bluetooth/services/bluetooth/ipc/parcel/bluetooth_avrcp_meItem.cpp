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

#include "bluetooth_avrcp_meItem.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothAvrcpMeItem::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(itemType_)) {
        return false;
    }
    if (!parcel.WriteUint64(uid_)) {
        return false;
    }
    if (!parcel.WriteUint8(type_)) {
        return false;
    }
    if (!parcel.WriteUint8(playable_)) {
        return false;
    }
    if (!parcel.WriteString(name_)) {
        return false;
    }
    if (!parcel.WriteUint32(attributes_.size())) {
        return false;
    }
    for (auto &attribute : attributes_) {
        if (!parcel.WriteUint32(attribute)) {
            return false;
        }
    }
    if (!parcel.WriteUint32(values_.size())) {
        return false;
    }
    for (auto &value : values_) {
        if (!parcel.WriteString(value)) {
            return false;
        }
    }
    return true;
}

bool BluetoothAvrcpMeItem::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothAvrcpMeItem *BluetoothAvrcpMeItem::Unmarshalling(Parcel &parcel)
{
    BluetoothAvrcpMeItem* avrcpData = new BluetoothAvrcpMeItem();
    if (avrcpData != nullptr && !avrcpData->ReadFromParcel(parcel)) {
        delete avrcpData;
        avrcpData = nullptr;
    }
    return avrcpData;
}

bool BluetoothAvrcpMeItem::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadUint8(itemType_)) {
        return false;
    }
    if (!parcel.ReadUint64(uid_)) {
        return false;
    }
    if (!parcel.ReadUint8(type_)) {
        return false;
    }
    if (!parcel.ReadUint8(playable_)) {
        return false;
    }
    if (!parcel.ReadString(name_)) {
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        uint32_t attribute = 0;
        if (!parcel.ReadUint32(attribute)) {
            return false;
        }
        attributes_.push_back(attribute);
    }
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::string value;
        if (!parcel.ReadString(value)) {
            return false;
        }
        values_.push_back(value);
    }
    return true;
}

}  // namespace Bluetooth
}  // namespace OHOS