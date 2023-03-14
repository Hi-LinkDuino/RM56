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

#ifndef GATT_DATA_H
#define GATT_DATA_H

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include "bt_def.h"
#include "bt_uuid.h"
#include "cstdint"
#include "raw_address.h"
#include "type_traits"


namespace OHOS {
namespace bluetooth {
struct Descriptor {
    Descriptor() : handle_(0), permissions_(0), value_(nullptr), length_(0), uuid_() {}

    Descriptor(const Uuid& uuid, const int permissions)
        : handle_(0), permissions_(permissions), value_(nullptr), length_(0), uuid_(uuid)
    {
    }

    Descriptor(uint16_t handle, const Uuid& uuid, int permissions)
        : handle_(handle), permissions_(permissions), value_(nullptr), length_(0), uuid_(uuid)
    {
    }

    Descriptor(const Uuid &uuid, uint16_t handle, int permissions, const uint8_t *value, size_t length);

    Descriptor(const Descriptor &src);

    Descriptor(Descriptor&& src)
        : handle_(src.handle_), permissions_(src.permissions_), value_(std::move(src.value_)), length_(src.length_),
        uuid_(src.uuid_)
    {
        src.value_ = nullptr;
        src.length_ = 0;
    }

    explicit Descriptor(uint16_t handle) : handle_(handle), permissions_(0), value_(nullptr), length_(0), uuid_() {}

    Descriptor(uint16_t handle, const uint8_t *value, size_t length);

    ~Descriptor(){}

    Descriptor &operator=(const Descriptor &src) = delete;
    Descriptor &operator=(Descriptor &&src) = delete;

    uint16_t handle_;
    int permissions_;
    std::unique_ptr<uint8_t[]> value_;
    size_t length_;
    Uuid uuid_;
};

struct Characteristic {
    Characteristic()
        : handle_(0), endHandle_(0), valueHandle_(0), properties_(0),
          permissions_(0), value_(nullptr), length_(0), uuid_(), descriptors_() {}

    Characteristic(const Uuid& uuid, uint16_t handle, int properties)
        : handle_(handle), endHandle_(0), valueHandle_(handle + 1), properties_(properties), permissions_(0),
        value_(nullptr),  length_(0), uuid_(uuid), descriptors_()
    {
    }

    Characteristic(
        const Uuid &uuid, uint16_t handle, int properties, int permissions, const uint8_t *value, size_t length);

    Characteristic(uint16_t handle, const uint8_t *value, size_t length);

    void SetValue(const uint8_t *value, size_t length);

    explicit Characteristic(uint16_t handle)
        : handle_(handle), endHandle_(0), valueHandle_(handle + 1), properties_(0), permissions_(0),
        value_(nullptr), length_(0), uuid_(), descriptors_() {}

    Characteristic(uint16_t handle, uint16_t endHandle)
        : handle_(handle), endHandle_(endHandle), valueHandle_(handle + 1), properties_(0), permissions_(0),
        value_(nullptr), length_(0), uuid_(), descriptors_()
    {
    }

    ~Characteristic(){}

    Characteristic(const Characteristic &src);

    Characteristic(Characteristic&& src)
        : handle_(src.handle_), endHandle_(src.endHandle_), valueHandle_(src.handle_ + 1),
        properties_(src.properties_), permissions_(src.permissions_), value_(std::move(src.value_)),
        length_(src.length_), uuid_(src.uuid_), descriptors_(std::move(src.descriptors_))
    {
        src.value_ = nullptr;
        src.length_ = 0;
    }

    Characteristic &operator=(const Characteristic &src) = delete;
    Characteristic &operator=(Characteristic &&src) = delete;

    uint16_t handle_;
    uint16_t endHandle_;
    uint16_t valueHandle_;
    int properties_;
    int permissions_;
    std::unique_ptr<uint8_t[]> value_;
    size_t length_;
    Uuid uuid_;
    std::vector<Descriptor> descriptors_;
};

struct Service {
    Service()
        :isPrimary_(false), handle_(0), startHandle_(0), endHandle_(0), uuid_(), includeServices_(), characteristics_()
    {
    }

    Service(const Uuid& uuid, uint16_t handle, uint16_t starthandle, uint16_t endHandle)
        : isPrimary_(true), handle_(handle), startHandle_(starthandle), endHandle_(endHandle), uuid_(uuid),
          includeServices_(), characteristics_()
    {
    }

    explicit Service(uint16_t handle)
        : isPrimary_(false), handle_(handle), startHandle_(handle), endHandle_(0), uuid_(), includeServices_(),
        characteristics_()
    {
    }

    Service(uint16_t handle, uint16_t endHandle)
        : isPrimary_(false), handle_(handle), startHandle_(handle), endHandle_(endHandle), uuid_(), includeServices_(),
        characteristics_()
    {
    }

    Service(const Service&) = default;
    Service(Service&&) = default;

    Service &operator=(const Service &src) = delete;
    Service &operator=(Service &&src) = delete;

    bool isPrimary_;
    uint16_t handle_;
    uint16_t startHandle_;
    uint16_t endHandle_;
    Uuid uuid_;
    std::vector<Service> includeServices_;
    std::vector<Characteristic> characteristics_;
};

struct GattDevice {
    GattDevice() : isEncryption_(false), transport_(0), addressType_(0), connectState_(0), addr_() {}
    GattDevice(const RawAddress &addr, uint8_t type, uint8_t transport);

    GattDevice(const RawAddress &addr, uint8_t type, uint8_t transport, int state);

    GattDevice(const RawAddress &addr, uint8_t transport);

    bool isEncryption_ = false;
    uint8_t role_ = GATT_ROLE_INVALID;
    uint8_t transport_ = 0;
    uint8_t addressType_ = 0;
    int connectState_ = 0;
    RawAddress addr_ = {};

    bool operator==(const GattDevice& rhs) const
    {
        return ((addr_ == rhs.addr_) && (transport_ == rhs.transport_));
    }

    bool operator<(const GattDevice& rhs) const
    {
        return ((addr_ < rhs.addr_) && (transport_ == rhs.transport_));
    }
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // GATT_DATA_H
