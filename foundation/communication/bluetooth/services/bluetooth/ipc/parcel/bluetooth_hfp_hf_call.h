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

#ifndef BLUETOOTH_PARCEL_HFP_HF_CALL_H
#define BLUETOOTH_PARCEL_HFP_HF_CALL_H

#include "parcel.h"
#include "hands_free_unit_calls.h"

namespace OHOS {
namespace Bluetooth {
class BluetoothHfpHfCall : public Parcelable, public bluetooth::HandsFreeUnitCalls {
public:
    BluetoothHfpHfCall() = default;
    BluetoothHfpHfCall(const bluetooth::HandsFreeUnitCalls& other): bluetooth::HandsFreeUnitCalls(other) {}
    BluetoothHfpHfCall(const BluetoothHfpHfCall& other): bluetooth::HandsFreeUnitCalls(other) {}
    ~BluetoothHfpHfCall() = default;

    bool Marshalling(Parcel &parcel) const override;

    static BluetoothHfpHfCall *Unmarshalling(Parcel &parcel);

    bool WriteToParcel(Parcel &parcel) const;

    bool ReadFromParcel(Parcel &parcel);
};

}  // namespace Bluetooth
}  // namespace OHOS

#endif // BLUETOOTH_PARCEL_HFP_HF_CALL_H