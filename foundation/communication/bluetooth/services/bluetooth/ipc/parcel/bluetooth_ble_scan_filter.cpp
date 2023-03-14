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

#include "bluetooth_ble_scan_filter.h"
#include "bluetooth_bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothBleScanFilter::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(deviceId_)) {
        return false;
    }
    if (!parcel.WriteString(name_)) {
        return false;
    }
    if (!parcel.WriteBool(hasServiceUuid_)) {
        return false;
    }
    if (!parcel.WriteBool(hasServiceUuidMask_)) {
        return false;
    }
    if (!parcel.WriteBool(hasSolicitationUuid_)) {
        return false;
    }
    if (!parcel.WriteBool(hasSolicitationUuidMask_)) {
        return false;
    }
    BluetoothUuid serviceUuid = BluetoothUuid(serviceUuid_);
    if (!parcel.WriteParcelable(&serviceUuid)) {
        return false;
    }
    BluetoothUuid serviceUuidMask = BluetoothUuid(serviceUuidMask_);
    if (!parcel.WriteParcelable(&serviceUuidMask)) {
        return false;
    }
    BluetoothUuid serviceSolicitationUuid = BluetoothUuid(serviceSolicitationUuid_);
    if (!parcel.WriteParcelable(&serviceSolicitationUuid)) {
        return false;
    }
    BluetoothUuid serviceSolicitationUuidMask = BluetoothUuid(serviceSolicitationUuidMask_);
    if (!parcel.WriteParcelable(&serviceSolicitationUuidMask)) {
        return false;
    }
    if (!parcel.WriteUInt8Vector(serviceData_)) {
        return false;
    }
    if (!parcel.WriteUInt8Vector(serviceDataMask_)) {
        return false;
    }
    if (!parcel.WriteUint16(manufacturerId_)) {
        return false;
    }
    if (!parcel.WriteUInt8Vector(manufactureData_)) {
        return false;
    }
    if (!parcel.WriteUInt8Vector(manufactureDataMask_)) {
        return false;
    }
    return true;
}

BluetoothBleScanFilter *BluetoothBleScanFilter::Unmarshalling(Parcel &parcel)
{
    BluetoothBleScanFilter *filter = new BluetoothBleScanFilter();
    if (filter != nullptr && !filter->ReadFromParcel(parcel)) {
        delete filter;
        filter = nullptr;
    }
    return filter;
}

bool BluetoothBleScanFilter::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothBleScanFilter::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(deviceId_)) {
        return false;
    }
    if (!parcel.ReadString(name_)) {
        return false;
    }
    if (!parcel.ReadBool(hasServiceUuid_)) {
        return false;
    }
    if (!parcel.ReadBool(hasServiceUuidMask_)) {
        return false;
    }
    if (!parcel.ReadBool(hasSolicitationUuid_)) {
        return false;
    }
    if (!parcel.ReadBool(hasSolicitationUuidMask_)) {
        return false;
    }
    if (!ReadUuidFromParcel(parcel)) {
        return false;
    }
    if (!parcel.ReadUInt8Vector(&serviceDataMask_)) {
        return false;
    }
    if (!parcel.ReadUint16(manufacturerId_)) {
        return false;
    }
    if (!parcel.ReadUInt8Vector(&manufactureData_)) {
        return false;
    }
    if (!parcel.ReadUInt8Vector(&manufactureDataMask_)) {
        return false;
    }
    return true;
}

bool BluetoothBleScanFilter::ReadUuidFromParcel(Parcel &parcel)
{
    std::shared_ptr<BluetoothUuid> serviceUuid(parcel.ReadParcelable<BluetoothUuid>());
    if (!serviceUuid) {
        return false;
    }
    serviceUuid_ = BluetoothUuid(*serviceUuid);
    std::shared_ptr<BluetoothUuid> serviceUuidMask(parcel.ReadParcelable<BluetoothUuid>());
    if (!serviceUuidMask) {
        return false;
    }
    serviceUuidMask_ = BluetoothUuid(*serviceUuidMask);
    std::shared_ptr<BluetoothUuid> serviceSolicitationUuid(parcel.ReadParcelable<BluetoothUuid>());
    if (!serviceSolicitationUuid) {
        return false;
    }
    serviceSolicitationUuid_ = BluetoothUuid(*serviceSolicitationUuid);
    std::shared_ptr<BluetoothUuid> serviceSolicitationUuidMask(parcel.ReadParcelable<BluetoothUuid>());
    if (!serviceSolicitationUuidMask) {
        return false;
    }
    serviceSolicitationUuidMask_ = BluetoothUuid(*serviceSolicitationUuidMask);
    if (!parcel.ReadUInt8Vector(&serviceData_)) {
        return false;
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
