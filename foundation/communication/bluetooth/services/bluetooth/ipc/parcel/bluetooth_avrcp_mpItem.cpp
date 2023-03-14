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

#include "bluetooth_avrcp_mpItem.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothAvrcpMpItem::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(itemType_)) {
        return false;
    }
    if (!parcel.WriteUint16(playerId_)) {
        return false;
    }
    if (!parcel.WriteUint8(majorType_)) {
        return false;
    }
    if (!parcel.WriteUint32(subType_)) {
        return false;
    }
    if (!parcel.WriteUint8(playStatus_)) {
        return false;
    }
    if (!parcel.WriteUint32(features_.size())) {
        return false;
    }
    for (auto &feature : features_) {
        if (!parcel.WriteUint8(feature)) {
            return false;
        }
    }
    if (!parcel.WriteString(name_)) {
        return false;
    }
    return true;
}

bool BluetoothAvrcpMpItem::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothAvrcpMpItem *BluetoothAvrcpMpItem::Unmarshalling(Parcel &parcel)
{
    BluetoothAvrcpMpItem *avrcpData = new BluetoothAvrcpMpItem();
    if (avrcpData != nullptr && !avrcpData->ReadFromParcel(parcel)) {
        delete avrcpData;
        avrcpData = nullptr;
    }
    return avrcpData;
}

bool BluetoothAvrcpMpItem::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadUint8(itemType_)) {
        return false;
    }
    if (!parcel.ReadUint16(playerId_)) {
        return false;
    }
    if (!parcel.ReadUint8(majorType_)) {
        return false;
    }
    if (!parcel.ReadUint32(subType_)) {
        return false;
    }
    if (!parcel.ReadUint8(playStatus_)) {
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        uint8_t feature;
        if (!parcel.ReadUint8(feature)) {
            return false;
        }
        features_.push_back(feature);
    }
    if (!parcel.ReadString(name_)) {
        return false;
    }
    return true;
}

}  // namespace Bluetooth
}  // namespace OHOS
