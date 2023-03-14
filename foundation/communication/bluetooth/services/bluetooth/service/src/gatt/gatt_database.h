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

#ifndef GATT_DATABASE_H
#define GATT_DATABASE_H

#include <cstddef>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "base_def.h"
#include "bt_uuid.h"
#include "gatt_data.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
class GattDatabase {
public:
    struct AttributeValue {
        std::unique_ptr<uint8_t[]> value_;
        size_t length_;

        AttributeValue() : value_(nullptr), length_(0)
        {}

        AttributeValue(uint8_t **value, size_t length) : value_(nullptr), length_(length)
        {
            value_.reset(*value);
            *value = nullptr;
        }

        AttributeValue(const AttributeValue &src) = delete;

        AttributeValue &operator=(const AttributeValue &src)
        {
            if (this != &src) {
                auto tmp = std::make_unique<uint8_t[]>(src.length_);
                if (tmp) {
                    length_ = src.length_;
                    value_ = std::move(tmp);
                    (void)memcpy_s(value_.get(), length_, src.value_.get(), length_);
                }
            }
            return *this;
        }

        AttributeValue(AttributeValue &&src) noexcept : value_(std::move(src.value_)), length_(src.length_)
        {
            src.length_ = 0;
        }

        AttributeValue &operator=(AttributeValue &&src) noexcept
        {
            if (this != &src) {
                value_ = std::move(src.value_);
                length_ = src.length_;
                src.length_ = 0;
            }
            return *this;
        }

        bool SetValue(const uint8_t *value, size_t length)
        {
            length_ = length;
            value_ = std::make_unique<uint8_t[]>(length_);
            if (memcpy_s(value_.get(), length_, value, length_) != EOK) {
                return false;
            }
            return true;
        }

        ~AttributeValue()
        {}
    };

    struct AttributeEntity {
        uint8_t permissions_;
        uint16_t handle_;
        AttributeValue value_;
        Uuid type_;
        AttributeEntity(uint8_t permissions, uint16_t handle, const Uuid &uuid)
            : permissions_(permissions), handle_(handle), value_(), type_(uuid)
        {}

        bool SetValue(const uint8_t *value, size_t length)
        {
            return value_.SetValue(value, length);
        }

        AttributeEntity(const AttributeEntity &) = delete;
        AttributeEntity &operator=(const AttributeEntity &) = default;
        AttributeEntity(AttributeEntity &&) = default;
        AttributeEntity &operator=(AttributeEntity &&) = default;
    };

    struct Descriptor {
        uint8_t permissions_;
        uint16_t handle_;
        AttributeValue value_;
        Uuid uuid_;
        explicit Descriptor(const bluetooth::Descriptor &src)
            : permissions_(src.permissions_), handle_(src.handle_), value_(), uuid_(src.uuid_)
        {}

        Descriptor(const Descriptor &) = delete;
        Descriptor &operator=(const Descriptor &) = default;
        Descriptor(Descriptor &&) = default;
        Descriptor &operator=(Descriptor &&) = default;
    };

    struct Characteristic {
        uint8_t properties_;
        uint16_t handle_;
        uint16_t valueHandle_;
        uint16_t endHandle_;
        std::map<uint16_t, Descriptor> descriptors_;
        Uuid uuid_;

        explicit Characteristic(const bluetooth::Characteristic &src)
            : properties_(src.properties_),
              handle_(src.handle_),
              valueHandle_(src.valueHandle_),
              endHandle_(),
              descriptors_(),
              uuid_(src.uuid_)
        {}

        Characteristic(const Characteristic &) = delete;
        Characteristic &operator=(const Characteristic &) = default;
        Characteristic(Characteristic &&) = default;
        Characteristic &operator=(Characteristic &&) = default;
    };

    struct IncludeService {
        uint16_t handle_;
        uint16_t startHandle_;
        uint16_t endHandle_;
        Uuid uuid_;

        explicit IncludeService(const bluetooth::Service &src)
            : handle_(src.handle_), startHandle_(src.startHandle_), endHandle_(src.endHandle_), uuid_(src.uuid_)
        {}
    };

    struct Service {
        bool isPrimary_;
        uint16_t handle_;
        uint16_t endHandle_;
        std::vector<IncludeService> includeServices_;
        std::map<uint16_t, Characteristic> characteristics_;
        Uuid uuid_;

        Service(uint16_t handle, uint16_t endHandle, const Uuid uuid)
            : isPrimary_(false),
              handle_(handle),
              endHandle_(endHandle),
              includeServices_(),
              characteristics_(),
              uuid_(uuid)
        {}

        explicit Service(const bluetooth::Service &src)
            : isPrimary_(src.isPrimary_),
              handle_(src.handle_),
              endHandle_(src.endHandle_),
              includeServices_(),
              characteristics_(),
              uuid_(src.uuid_)
        {}

        Service(const Service &) = delete;
        Service &operator=(const Service &) = default;
        Service(Service &&) = default;
        Service &operator=(Service &&) = default;
    };

    using GattAttributeEntity = std::optional<std::reference_wrapper<GattDatabase::AttributeEntity>>;

    GattDatabase();
    ~GattDatabase()
    {}
    int AddService(bluetooth::Service &service);
    int DeleteService(uint16_t handle);
    void RemoveAllServices();
    int SetValueByHandle(uint16_t handle, AttributeValue &value);
    const std::map<uint16_t, Service> &GetServices() const;
    const std::vector<IncludeService> *GetIncludeServices(uint16_t serviceHandle);
    const GattDatabase::IncludeService *GetIncludeService(uint16_t serviceHandle);
    const std::map<uint16_t, Characteristic> *GetCharacteristics(uint16_t serviceHandle);
    const std::map<uint16_t, Descriptor> *GetDescriptors(uint16_t cccHandle);
    const GattDatabase::Service *GetService(uint16_t handle);
    GattDatabase::Characteristic *GetCharacteristic(uint16_t valueHandle);
    const GattDatabase::Descriptor *GetDescriptor(uint16_t valueHandle);
    GattAttributeEntity GetValueByHandle(const uint16_t handle);
    int CheckLegalityOfServiceDefinition(bluetooth::Service &service);

private:
    static int CountDescriptorByUuid(const std::vector<bluetooth::Descriptor> &descriptors, const Uuid &uuid);
    std::pair<uint16_t, uint16_t> CalculateAndAssignHandle(const bluetooth::Service &service);
    void ReleaseHandle(Service &service);
    bool IsReferenced(uint16_t handle) const;
    int CheckIncludeServicesLegality(bluetooth::Service &service) const;
    int CheckCharacteristicsLegality(const bluetooth::Service &service) const;
    int CheckDescriptorsLegality(const bluetooth::Characteristic &characteristic) const;
    bool CheckRestrictedGattBasedService(const bluetooth::Service &service);

    // available handle [handle, handle]
    std::list<std::pair<uint16_t, uint16_t>> availableHandles_ = {};
    // service handle <-> Service entity
    std::map<uint16_t, Service> services_ = {};
    // value handle <-> Attribute entity
    std::map<uint16_t, AttributeEntity> attributes_ = {};
    // value handle <-> (service handle, parent handle)
    // if value handle belong to descriptor, parent handle is characteristic handle witch descriptor belong to.
    // else parent handle is characteristic handle.
    std::map<uint16_t, std::pair<uint16_t, uint16_t>> valueHandleMap_ = {};
    std::set<uint16_t> restrictedGattBasedService_ = {};

    BT_DISALLOW_COPY_AND_ASSIGN(GattDatabase);
    BT_DISALLOW_MOVE_AND_ASSIGN(GattDatabase);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !GATT_DATABASE_H
