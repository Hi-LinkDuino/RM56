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
#include "bluetooth_gatt_characteristic.h"
#include "bluetooth_gatt_service.h"
#include "bluetooth_log.h"
#include "parcel_bt_uuid.h"

namespace OHOS {
namespace Bluetooth {
const uint32_t GATT_SERVICE_PARCEL_SIZE_MAX = 0x100;
bool BluetoothGattService::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteBool(isPrimary_)) {
        return false;
    }
    if (!parcel.WriteUint16(handle_)) {
        return false;
    }
    if (!parcel.WriteUint16(startHandle_)) {
        return false;
    }
    if (!parcel.WriteUint16(endHandle_)) {
        return false;
    }
    BluetoothUuid uuid(uuid_);
    if (!parcel.WriteParcelable(&uuid)) {
        return false;
    }
    uint32_t size = includeServices_.size();
    if (!parcel.WriteUint32(size)) {
        return false;
    }
    for (auto serv : includeServices_) {
        BluetoothGattService service = BluetoothGattService(serv);
        if (!parcel.WriteParcelable(&service)) {
            return false;
        }
    }
    size = characteristics_.size();
    if (!parcel.WriteUint32(size)) {
        return false;
    }
    for (auto character: characteristics_) {
        BluetoothGattCharacteristic characteristic = BluetoothGattCharacteristic(character);
        if (!parcel.WriteParcelable(&characteristic)) {
            return false;
        }
    }
    return true;
}

BluetoothGattService *BluetoothGattService::Unmarshalling(Parcel &parcel)
{
    BluetoothGattService *service = new BluetoothGattService();
    if (service != nullptr && !service->ReadFromParcel(parcel)) {
        delete service;
        service = nullptr;
    }
    return service;
}

bool BluetoothGattService::WriteToParcel(Parcel &parcel)
{
    return Marshalling(parcel);
}

bool BluetoothGattService::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadBool(isPrimary_)) {
        return false;
    }
    if (!parcel.ReadUint16(handle_)) {
        return false;
    }
    if (!parcel.ReadUint16(startHandle_)) {
        return false;
    }
    if (!parcel.ReadUint16(endHandle_)) {
        return false;
    }
    std::shared_ptr<BluetoothUuid> uuid(parcel.ReadParcelable<BluetoothUuid>());
    if (!uuid) {
        return false;
    }
    uuid_ = BluetoothUuid(*uuid);
    uint32_t size = 0;
    if (!parcel.ReadUint32(size) || size > GATT_SERVICE_PARCEL_SIZE_MAX) {
        HILOGE("readfailed size value:%{public}u", size);
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<BluetoothGattService> service(parcel.ReadParcelable<BluetoothGattService>());
        if (!service) {
            return false;
        }
        includeServices_.push_back(*service);
    }
    if (!parcel.ReadUint32(size) || size > GATT_SERVICE_PARCEL_SIZE_MAX) {
        HILOGE("readfailed size value:%{public}u", size);
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<BluetoothGattCharacteristic> characteristic(
            parcel.ReadParcelable<BluetoothGattCharacteristic>());
        if (!characteristic) {
            return false;
        }
        characteristics_.push_back(*characteristic);
    }
    return true;
}
}  // namespace Bluetooth
}  // namespace OHOS
