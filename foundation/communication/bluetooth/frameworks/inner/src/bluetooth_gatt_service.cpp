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

#include "bluetooth_gatt_service.h"

#include <cstdint>
#include "__functional_base"
#include "bluetooth_gatt_characteristic.h"
#include "type_traits"
#include "uuid.h"
#include "vector"

namespace OHOS {
namespace Bluetooth {
void GattService::AddCharacteristic(const GattCharacteristic &characteristic)
{
    characteristics_.insert(characteristics_.end(), characteristic)->service_ = this;
}

GattService::GattService(const UUID &uuid, const GattServiceType type)
    : handle_(0), endHandle_(0), serviceType_(type), includeServices_(), characteristics_(), uuid_(uuid)
{}

GattService::GattService(const UUID &uuid, uint16_t handle, uint16_t endHandle, const GattServiceType type)
    : handle_(handle), endHandle_(endHandle), serviceType_(type), includeServices_(), characteristics_(), uuid_(uuid)
{}

GattService::GattService(const GattService &src)
    : handle_(src.handle_),
      endHandle_(src.endHandle_),
      serviceType_(src.serviceType_),
      includeServices_(),
      characteristics_(),
      uuid_(src.uuid_)
{
    includeServices_ = src.includeServices_;
    for (auto &characteristic : src.characteristics_) {
        AddCharacteristic(characteristic);
    }
}

GattService::GattService(GattService &&src)
    : handle_(src.handle_),
      endHandle_(src.endHandle_),
      serviceType_(src.serviceType_),
      includeServices_(),
      characteristics_(),
      uuid_(src.uuid_)
{
    includeServices_ = std::move(src.includeServices_);
    for (auto &characteristic : src.characteristics_) {
        characteristics_.insert(characteristics_.end(), std::move(characteristic))->service_ = this;
    }
}

void GattService::AddService(GattService &service)
{
    includeServices_.push_back(std::ref(service));
}

GattCharacteristic *GattService::GetCharacteristic(const UUID &uuid)
{
    for (auto &characteristic : characteristics_) {
        if (characteristic.GetUuid().Equals(uuid)) {
            return &characteristic;
        }
    }
    return nullptr;
}

std::vector<GattCharacteristic> &GattService::GetCharacteristics()
{
    return characteristics_;
}

const std::vector<std::reference_wrapper<GattService>> &GattService::GetIncludedServices()
{
    return includeServices_;
}

uint16_t GattService::GetHandle() const
{
    return handle_;
}

bool GattService::IsPrimary() const
{
    return (serviceType_ == GattServiceType::PRIMARY);
}

const UUID &GattService::GetUuid() const
{
    return uuid_;
}
}  // namespace Bluetooth
}  // namespace OHOS
