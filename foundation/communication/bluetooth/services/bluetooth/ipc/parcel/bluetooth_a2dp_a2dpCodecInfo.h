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

#ifndef BLUETOOTH_PARCEL_A2DP_A2DPCODECINFO_H
#define BLUETOOTH_PARCEL_A2DP_A2DPCODECINFO_H

#include "a2dp_parcel_codec.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothA2dpCodecInfo : public Parcelable,
                               public bluetooth::CodecInfo {
 public:
    BluetoothA2dpCodecInfo() = default;
    BluetoothA2dpCodecInfo(const bluetooth::CodecInfo& other)
        : bluetooth::CodecInfo(other) {} // NOLINT(implicit)
    BluetoothA2dpCodecInfo(const BluetoothA2dpCodecInfo& other)
        : bluetooth::CodecInfo(other) {} // NOLINT(implicit)
    ~BluetoothA2dpCodecInfo() = default;

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothA2dpCodecInfo *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif // BLUETOOTH_PARCEL_A2DP_A2DPCODECINFO_H