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
#include "bluetooth_gatt_device.h"
#include "bluetooth_log.h"
#include "bluetooth_raw_address.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothGattDevice::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteBool(isEncryption_)) {
        return false;
    }
    if (!parcel.WriteUint8(transport_)) {
        return false;
    }
    if (!parcel.WriteUint8(addressType_)) {
        return false;
    }
    if (!parcel.WriteInt32(connectState_)) {
        return false;
    }
    if (!parcel.WriteString(addr_.GetAddress())) {
        return false;
    }
    return true;
}

BluetoothGattDevice *BluetoothGattDevice::Unmarshalling(Parcel &parcel)
{
    BluetoothGattDevice *gattDevice = new BluetoothGattDevice();
    if (gattDevice != nullptr && !gattDevice->ReadFromParcel(parcel))  {
        delete gattDevice;
        gattDevice = nullptr;
    }
    return gattDevice;
}

bool BluetoothGattDevice::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothGattDevice::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadBool(isEncryption_)) {
        return false;
    }
    if (!parcel.ReadUint8(transport_)) {
        return false;
    }
    if (!parcel.ReadUint8(addressType_)) {
        return false;
    }
    if (!parcel.ReadInt32(connectState_)) {
        return false;
    }
    std::string addr;
    if (!parcel.ReadString(addr)) {
        return false;
    }
    addr_ = bluetooth::RawAddress(addr);
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
