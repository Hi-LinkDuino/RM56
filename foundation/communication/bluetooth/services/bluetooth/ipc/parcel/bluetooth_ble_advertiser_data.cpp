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

#include "bluetooth_ble_advertiser_data.h"
#include "bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothBleAdvertiserData::Marshalling(Parcel &parcel) const
{
    if (!WriteServiceUuids(parcel)) {
        return false;
    }
    if (!WriteManufacturerData(parcel)) {
        return false;
    }
    if (!WriteServiceData(parcel)) {
        return false;
    }
    if (!parcel.WriteUint8(advFlag_)) {
        return false;
    }
    if (!parcel.WriteString(payload_)) {
        return false;
    }
    return true;
}

bool BluetoothBleAdvertiserData::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

BluetoothBleAdvertiserData *BluetoothBleAdvertiserData::Unmarshalling(Parcel &parcel)
{
    BluetoothBleAdvertiserData *advertiserData = new BluetoothBleAdvertiserData();
    if (advertiserData != nullptr && !advertiserData->ReadFromParcel(parcel)) {
        delete advertiserData;
        advertiserData = nullptr;
    }
    return advertiserData;
}

bool BluetoothBleAdvertiserData::ReadFromParcel(Parcel &parcel)
{
    if (!ReadServiceUuids(serviceUuids_, parcel)) {
        return false;
    }
    if (!ReadManufacturerData(manufacturerSpecificData_, parcel)) {
        return false;
    }
    if (!ReadServiceData(serviceData_, parcel)) {
        return false;
    }
    if (!parcel.ReadUint8(advFlag_)) {
        return false;
    }
    if (!parcel.ReadString(payload_)) {
        return false;
    }
    return true;
}

bool BluetoothBleAdvertiserData::WriteServiceUuids(Parcel &parcel) const
{
    if (!parcel.WriteInt32(serviceUuids_.size())) {
        return false;
    }
    for (auto &serviceUuids : serviceUuids_) {
        if (!parcel.WriteInt32(serviceUuids.ConvertTo32Bits())) {
            return false;
        }
    }
    return true;
}

bool BluetoothBleAdvertiserData::ReadServiceUuids(std::vector<bluetooth::Uuid> &serviceUuids, Parcel &parcel)
{
    int32_t serviceUuidSize;
    if (!parcel.ReadInt32(serviceUuidSize)) {
        return false;
    }
    for (int i = 0; i < serviceUuidSize; ++i) {
        uint32_t uuid;
        if (parcel.ReadUint32(uuid)) {
            serviceUuids.push_back(bluetooth::Uuid::ConvertFrom32Bits(uuid));
        } else {
            return false;
        }
    }
    return true;
}

bool BluetoothBleAdvertiserData::WriteManufacturerData(Parcel &parcel) const
{
    if (!parcel.WriteInt32(manufacturerSpecificData_.size())) {
        return false;
    }
    for (auto &manufacturer : manufacturerSpecificData_) {
        if (!parcel.WriteUint16(manufacturer.first)) {
            return false;
        }
        if (!parcel.WriteString(manufacturer.second)) {
            return false;
        }
    }
    return true;
}

bool BluetoothBleAdvertiserData::ReadManufacturerData(std::map<uint16_t, std::string> &manufacturerData, Parcel &parcel)
{
    int manufacturerSize;
    if (!parcel.ReadInt32(manufacturerSize)) {
        return false;
    }
    uint16_t manufacturerId;
    std::string manufacturerDataValue;
    for (int i = 0; i < manufacturerSize; ++i) {
        if (!parcel.ReadUint16(manufacturerId)) {
            return false;
        }
        if (!parcel.ReadString(manufacturerDataValue)) {
            return false;
        }
        manufacturerData.emplace(manufacturerId, manufacturerDataValue);
    }
    return true;
}

bool BluetoothBleAdvertiserData::WriteServiceData(Parcel &parcel) const
{
    if (!parcel.WriteInt32(serviceData_.size())) {
        return false;
    }
    for (auto &service : serviceData_) {
        if (!parcel.WriteUint32(service.first.ConvertTo32Bits())) {
            return false;
        }
        if (!parcel.WriteString(service.second)) {
            return false;
        }
    }
    return true;
}

bool BluetoothBleAdvertiserData::ReadServiceData(std::map<bluetooth::Uuid, std::string> &serviceData, Parcel &parcel)
{
    int serviceDataSize;
    if (!parcel.ReadInt32(serviceDataSize)) {
        return false;
    }
    bluetooth::Uuid serviceDataId;
    std::string serviceDataData;
    for (int i = 0; i < serviceDataSize; ++i) {
        uint32_t uuid;
        if (parcel.ReadUint32(uuid)) {
            serviceDataId = bluetooth::Uuid::ConvertFrom32Bits(uuid);
        } else {
            return false;
        }
        if (!parcel.ReadString(serviceDataData)) {
            return false;
        }
        serviceData.emplace(serviceDataId, serviceDataData);
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
