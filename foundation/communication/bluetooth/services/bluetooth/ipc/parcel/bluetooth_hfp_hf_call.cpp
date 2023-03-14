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

#include <string>
#include "bluetooth_hfp_hf_call.h"
#include "bluetooth_bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
using namespace std;

bool BluetoothHfpHfCall::Marshalling(Parcel &parcel) const 
{
    return WriteToParcel(parcel);
}

BluetoothHfpHfCall *BluetoothHfpHfCall::Unmarshalling(Parcel &parcel) 
{
    BluetoothHfpHfCall *call = new BluetoothHfpHfCall();
    if (call!= nullptr && !call->ReadFromParcel(parcel)) {
        delete call;
        call = nullptr;
    }
    return call;
}

bool BluetoothHfpHfCall::WriteToParcel(Parcel &parcel) const 
{
    if (!parcel.WriteString(device_)) {
        return false;
    }
    if (!parcel.WriteInt32(id_)) {
        return false;
    }
    if (!parcel.WriteInt32(state_)) {
        return false;
    }
    if (!parcel.WriteString(number_)) {
        return false;
    }
    BluetoothUuid uuid = BluetoothUuid(uuid_);
    if (!parcel.WriteParcelable(&uuid)) {
        return false;
    }
    if (!parcel.WriteBool(multiParty_)) {
        return false;
    }
    if (!parcel.WriteBool(outgoing_)) {
        return false;
    }
    if (!parcel.WriteBool(inBandRing_)) {
        return false;
    }
    if (!parcel.WriteInt64(creationTime_)) {
        return false;
    }
    return true;
}

bool BluetoothHfpHfCall::ReadFromParcel(Parcel &parcel) {
    if (!parcel.ReadString(device_)) {
        return false;
    }
    if (!parcel.ReadInt32(id_)) {
        return false;
    }
    if (!parcel.ReadInt32(state_)) {
        return false;
    }
    if (!parcel.ReadString(number_)) {
        return false;
    }
    std::shared_ptr<BluetoothUuid> uuid(parcel.ReadParcelable<BluetoothUuid>());
    if (!uuid) {
        return false;
    }
    uuid_ = BluetoothUuid(*uuid);
    if (!parcel.ReadBool(multiParty_)) {
        return false;
    }
    if (!parcel.ReadBool(outgoing_)) {
        return false;
    }
    if (!parcel.ReadBool(inBandRing_)) {
        return false;
    }
    int64_t time;
    if (!parcel.ReadInt64(time)) {
        return false;
    }
    creationTime_ = time;
    return true;
}

}  // namespace Bluetooth
}  // namespace OHOS