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

#include "bluetooth_a2dp_a2dpCodecInfo.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothA2dpCodecInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(codecPriority)) {
        return false;
    }
    if (!parcel.WriteUint8(codecType)) {
        return false;
    }
    if (!parcel.WriteUint32(sampleRate)) {
        return false;
    }
    if (!parcel.WriteUint8(bitsPerSample)) {
        return false;
    }
    if (!parcel.WriteUint8(channelMode)) {
        return false;
    }
    if (!parcel.WriteUint64(codecSpecific1)) {
        return false;
    }
    if (!parcel.WriteUint64(codecSpecific2)) {
        return false;
    }
    if (!parcel.WriteUint64(codecSpecific3)) {
        return false;
    }
    if (!parcel.WriteUint64(codecSpecific4)) {
        return false;
    }
    return true;
}

bool BluetoothA2dpCodecInfo::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothA2dpCodecInfo *BluetoothA2dpCodecInfo::Unmarshalling(Parcel &parcel)
{
    BluetoothA2dpCodecInfo *codecData = new BluetoothA2dpCodecInfo();
    if (codecData != nullptr && !codecData->ReadFromParcel(parcel)) {
        delete codecData;
        codecData = nullptr;
    }
    return codecData;
}

bool BluetoothA2dpCodecInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadUint8(codecPriority)) {
        return false;
    }
    if (!parcel.ReadUint8(codecType)) {
        return false;
    }
    if (!parcel.ReadUint32(sampleRate)) {
        return false;
    }
    if (!parcel.ReadUint8(bitsPerSample)) {
        return false;
    }
    if (!parcel.ReadUint8(channelMode)) {
        return false;
    }
    if (!parcel.ReadUint64(codecSpecific1)) {
        return false;
    }
    if (!parcel.ReadUint64(codecSpecific2)) {
        return false;
    }
    if (!parcel.ReadUint64(codecSpecific3)) {
        return false;
    }
    if (!parcel.ReadUint64(codecSpecific4)) {
        return false;
    }
    return true;
}

}  // namespace Bluetooth
}  // namespace OHOS
