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

#include "gatt_data.h"
#include "bt_def.h"
#include "memory"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
GattDevice::GattDevice(const RawAddress& addr, uint8_t type, uint8_t transport)
    : isEncryption_((transport == GATT_TRANSPORT_TYPE_CLASSIC) ? true : false),
    transport_(transport), addressType_(type), connectState_(0), addr_(addr)
{
}

GattDevice::GattDevice(const RawAddress& addr, uint8_t type, uint8_t transport, int state)
    : isEncryption_((transport == GATT_TRANSPORT_TYPE_CLASSIC) ? true : false),
    transport_(transport), addressType_(type), connectState_(state), addr_(addr)
{
}

GattDevice::GattDevice(const RawAddress& addr, uint8_t transport)
    : isEncryption_((transport == GATT_TRANSPORT_TYPE_CLASSIC) ? true : false),
    transport_(transport), addressType_(0), connectState_(0), addr_(addr) {}

Descriptor::Descriptor(const Uuid &uuid, uint16_t handle, int permissions, const uint8_t *value, size_t length)
    : handle_(handle), permissions_(permissions), value_(nullptr), length_(length), uuid_(uuid)
    {
        if (value != nullptr && length != 0) {
            value_ = std::make_unique<uint8_t[]>(length);
            (void)memcpy_s(value_.get(), length_, value, length_);
        } else {
            value_ = nullptr;
            length_ = 0;
        }
    }

Descriptor::Descriptor(const Descriptor& src)
    : handle_(src.handle_), permissions_(src.permissions_), value_(nullptr), length_(src.length_), uuid_(src.uuid_)
{
    if (src.value_ != nullptr && src.length_ != 0) {
        value_ = std::make_unique<uint8_t[]>(length_);
        (void)memcpy_s(value_.get(), length_, src.value_.get(), length_);
    } else {
        value_ = nullptr;
        length_ = 0;
    }
}

Descriptor::Descriptor(uint16_t handle, const uint8_t *value, size_t length)
    : handle_(handle), permissions_(0), value_(nullptr), length_(length), uuid_()
{
    value_ = std::make_unique<uint8_t[]>(length_);
    (void)memcpy_s(value_.get(), length_, value, length_);
}

Characteristic::Characteristic(
    const Uuid& uuid, uint16_t handle, int properties, int permissions, const uint8_t *value, size_t length)
    : handle_(handle), endHandle_(0), valueHandle_(handle + 1), properties_(properties), permissions_(permissions),
    value_(nullptr), length_(length), uuid_(uuid), descriptors_()
{
    if (value != nullptr && length != 0) {
        value_ = std::make_unique<uint8_t[]>(length_);
        (void)memcpy_s(value_.get(), length_, value, length_);
    } else {
        value_ = nullptr;
        length_ = 0;
    }
}

Characteristic::Characteristic(uint16_t handle, const uint8_t *value, size_t length)
    : handle_(handle), endHandle_(0), valueHandle_(handle + 1), properties_(0), permissions_(0),
    value_(std::make_unique<uint8_t[]>(length)), length_(length), uuid_(), descriptors_()
{
    (void)memcpy_s(value_.get(), length_, value, length_);
}

Characteristic::Characteristic(const Characteristic& src)
    : handle_(src.handle_), endHandle_(src.endHandle_), valueHandle_(src.handle_ + 1),
    properties_(src.properties_), permissions_(src.permissions_), value_(nullptr), length_(src.length_),
    uuid_(src.uuid_), descriptors_(src.descriptors_)
{
    if (src.value_ != nullptr && src.length_ != 0) {
        value_ = std::make_unique<uint8_t[]>(length_);
        (void)memcpy_s(value_.get(), length_, src.value_.get(), length_);
    } else {
        value_ = nullptr;
        length_ = 0;
    }
}

void Characteristic::SetValue(const uint8_t *value, size_t length)
{
    if (value_ != nullptr) {
        value_.reset(nullptr);
    }

    length_ = length;
    value_ = std::make_unique<uint8_t[]>(length_);
    (void)memcpy_s(value_.get(), length_, value, length_);
}
}  // namespace bluetooth
}  // namespace OHOS