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
#include "securec.h"
#include <cstddef>
#include <cstdint>
#include "bluetooth_gatt_descriptor.h"
#include "bluetooth_log.h"
#include "cstdint"
#include "memory"
#include "uuid.h"

namespace OHOS {
namespace Bluetooth {
GattDescriptor::GattDescriptor(const UUID uuid, const int permissions)
    : handle_(0), permissions_(permissions), characteristic_(nullptr), value_(nullptr), length_(0), uuid_(uuid)
{}

GattDescriptor::GattDescriptor(const UUID uuid, uint16_t handle, const int permissions)
    : handle_(handle), permissions_(permissions), characteristic_(nullptr), value_(nullptr), length_(0), uuid_(uuid)
{}

GattDescriptor::GattDescriptor(const GattDescriptor &src)
    : handle_(src.handle_),
      permissions_(src.permissions_),
      characteristic_(src.characteristic_),
      value_(nullptr),
      length_(src.length_),
      uuid_(src.uuid_)
{
    if (length_ != 0 && src.value_ != nullptr) {
        value_ = std::make_unique<uint8_t[]>(length_);
        (void)memcpy_s(value_.get(), length_, src.value_.get(), length_);
    } else {
        value_.reset(nullptr);
        length_ = 0;
    }
}

GattDescriptor &GattDescriptor::operator=(const GattDescriptor &src)
{
    if (this != &src) {
        uuid_ = src.uuid_;
        permissions_ = src.permissions_;
        handle_ = src.handle_;
        characteristic_ = src.characteristic_;
        length_ = src.length_;

    if (length_ != 0 && src.value_ != nullptr) {
            value_ = std::make_unique<uint8_t[]>(length_);
            (void)memcpy_s(value_.get(), length_, src.value_.get(), length_);
        } else {
            value_.reset(nullptr);
            length_ = 0;
        }
    }
    return *this;
}

GattCharacteristic *GattDescriptor::GetCharacteristic() const
{
    return characteristic_;
}

int GattDescriptor::GetPermissions() const
{
    return permissions_;
}

const UUID &GattDescriptor::GetUuid() const
{
    return uuid_;
}

const std::unique_ptr<uint8_t[]> &GattDescriptor::GetValue(size_t *size) const
{
    *size = length_;
    return value_;
}

void GattDescriptor::SetValue(const uint8_t *values, const size_t length)
{
    if (length == 0 || values == nullptr) {
        HILOGI("value is nullptr, or length is 0");
        return;
    }
    value_ = std::make_unique<uint8_t[]>(length);
    length_ = length;
    (void)memcpy_s(value_.get(), length, values, length);
}

uint16_t GattDescriptor::GetHandle() const
{
    return handle_;
}
} // namespace Bluetooth
} // namespace OHOS
