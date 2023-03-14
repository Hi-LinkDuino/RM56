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

#ifndef BLUETOOTH_PARCEL_A2DP_A2DPCODECSTATUS_H
#define BLUETOOTH_PARCEL_A2DP_A2DPCODECSTATUS_H

#include "a2dp_parcel_codec.h"
#include "bluetooth_a2dp_a2dpCodecInfo.h"
#include "parcel.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothA2dpCodecStatus : public Parcelable,
                                 public bluetooth::CodecStatus {
public:
    BluetoothA2dpCodecStatus() = default;
    BluetoothA2dpCodecStatus(const bluetooth::CodecStatus& other)
        : bluetooth::CodecStatus(other) {} // NOLINT(implicit)
    BluetoothA2dpCodecStatus(const BluetoothA2dpCodecStatus& other)
        : bluetooth::CodecStatus(other) {} // NOLINT(implicit)
    ~BluetoothA2dpCodecStatus() = default;

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothA2dpCodecStatus *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif // BLUETOOTH_PARCEL_A2DP_A2DPCODECSTATUS_H