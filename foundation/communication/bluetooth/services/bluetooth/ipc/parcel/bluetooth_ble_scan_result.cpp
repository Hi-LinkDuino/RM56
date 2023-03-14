/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "bluetooth_ble_scan_result.h"

namespace OHOS {
namespace Bluetooth {
bool BluetoothBleScanResult::Marshalling(Parcel &parcel) const
{
    if (!WirteServiceUuidsToParcel(parcel)) {
        return false;
    }
    if (!WirteManufacturerDataToParcel(parcel)) {
        return false;
    }
    if (!WirteServiceDataToParcel(parcel)) {
        return false;
    }
    if (!parcel.WriteString(addr_.GetAddress())) {
        return false;
    }
    if (!parcel.WriteInt8(rssi_)) {
        return false;
    }
    if (!parcel.WriteBool(connectable_)) {
        return false;
    }
    if (!parcel.WriteUint8(advertiseFlag_)) {
        return false;
    }
    if (!parcel.WriteString(payload_)) {
        return false;
    }
    return true;
}

BluetoothBleScanResult *BluetoothBleScanResult::Unmarshalling(Parcel &parcel)
{
    BluetoothBleScanResult *settings = new BluetoothBleScanResult();
    if (settings != nullptr && !settings->ReadFromParcel(parcel)) {
        delete settings;
        settings = nullptr;
    }
    return settings;
}

bool BluetoothBleScanResult::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothBleScanResult::ReadFromParcel(Parcel &parcel)
{
    if (!ReadServiceUuidsFromParcel(parcel)) {
        return false;
    }
    if (!ReadManufacturerDataFromParcel(parcel)) {
        return false;
    }
    if (!ReadServiceDataFromParcel(parcel)) {
        return false;
    }
    std::string address = "";
    if (parcel.ReadString(address)) {
        addr_ = bluetooth::RawAddress(address);
    } else {
        return false;
    }
    if (!parcel.ReadInt8(rssi_)) {
        return false;
    }
    if (!parcel.ReadBool(connectable_)) {
        return false;
    }
    if (!parcel.ReadUint8(advertiseFlag_)) {
        return false;
    }
    if (!parcel.ReadString(payload_)) {
        return false;
    }
    return true;
}

bool BluetoothBleScanResult::WirteServiceUuidsToParcel(Parcel &parcel) const
{
    if (!parcel.WriteInt32(serviceUuids_.size())) {
        return false;
    }
    for (auto iter : serviceUuids_) {
        if (!parcel.WriteUint32(iter.ConvertTo32Bits())) {
            return false;
        }
    }
    return true;
}

bool BluetoothBleScanResult::ReadServiceUuidsFromParcel(Parcel &parcel)
{
    int32_t uuidSize = 0;
    if (!parcel.ReadInt32(uuidSize)) {
        return false;
    }
    for (int i = 0; i < uuidSize; ++i) {
        uint32_t uuid;
        if (parcel.ReadUint32(uuid)) {
            serviceUuids_.push_back(bluetooth::Uuid::ConvertFrom32Bits(uuid));
        } else {
            return false;
        }
    }
    return true;
}

bool BluetoothBleScanResult::WirteManufacturerDataToParcel(Parcel &parcel) const
{
    if (!parcel.WriteInt32(manufacturerSpecificData_.size())) {
        return false;
    }
    for (auto iter = manufacturerSpecificData_.begin(); iter != manufacturerSpecificData_.end(); ++iter) {
        if (!parcel.WriteUint16(iter->first)) {
            return false;
        }
        if (!parcel.WriteString(iter->second)) {
            return false;
        }
    }
    return true;
}

bool BluetoothBleScanResult::ReadManufacturerDataFromParcel(Parcel &parcel)
{
    int32_t manuSize = 0;
    if (!parcel.ReadInt32(manuSize)) {
        return false;
    }
    for (int i = 0; i < manuSize; i++) {
        uint16_t manufacturerId = 0;
        std::string manufacturedData;
        if (!parcel.ReadUint16(manufacturerId)) {
            return false;
        }
        if (!parcel.ReadString(manufacturedData)) {
            return false;
        }
        manufacturerSpecificData_.emplace(manufacturerId, manufacturedData);
    }
    return true;
}

bool BluetoothBleScanResult::WirteServiceDataToParcel(Parcel &parcel) const
{
    if (!parcel.WriteInt32(serviceData_.size())) {
        return false;
    }
    for (auto iter = serviceData_.begin(); iter != serviceData_.end(); ++iter) {
        if (!parcel.WriteUint32(iter->first.ConvertTo32Bits())) {
            return false;
        }
        if (!parcel.WriteString(iter->second)) {
            return false;
        }
    }
    return true;
}

bool BluetoothBleScanResult::ReadServiceDataFromParcel(Parcel &parcel)
{
    int32_t serviceDataSize = 0;
    if (!parcel.ReadInt32(serviceDataSize)) {
        return false;
    }
    for (int i = 0; i < serviceDataSize; i++) {
        uint32_t serviceId;
        std::string serviceData;
        if (!parcel.ReadUint32(serviceId)) {
            return false;
        }
        if (!parcel.ReadString(serviceData)) {
            return false;
        }
        serviceData_.emplace(bluetooth::Uuid::ConvertFrom32Bits(serviceId), serviceData);
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS