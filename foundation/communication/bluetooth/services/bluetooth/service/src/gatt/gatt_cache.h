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

#ifndef GATT_CACHE_H
#define GATT_CACHE_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include "base_def.h"
#include "bt_uuid.h"
#include "gatt_data.h"

namespace OHOS {
namespace bluetooth {
class GattCache {
public:
    struct IncludeService {
        uint16_t handle_ = 0;
        uint16_t startHandle_ = 0;
        uint16_t endHandle_ = 0;
        Uuid uuid_ = {};

        explicit IncludeService(const Service &src)
            : handle_(src.handle_), startHandle_(src.startHandle_), endHandle_(src.endHandle_), uuid_(src.uuid_)
        {}
        IncludeService(uint16_t handle, uint16_t startHandle, uint16_t endHandle, const Uuid uuid)
            : handle_(handle), startHandle_(startHandle), endHandle_(endHandle), uuid_(uuid)
        {}
        IncludeService(uint16_t handle, uint16_t startHandle, uint16_t endHandle)
            : handle_(handle), startHandle_(startHandle), endHandle_(endHandle)
        {}
        explicit IncludeService(const Uuid uuid) : uuid_(uuid)
        {}
    };

    struct Descriptor {
        uint16_t handle_ = 0;
        Uuid uuid_ = {};

        Descriptor(uint16_t handle, const Uuid uuid) : handle_(handle), uuid_(uuid)
        {}
    };

    struct Characteristic {
        uint8_t properties_ = 0;
        uint16_t handle_ = 0;
        uint16_t valueHandle_ = 0;
        std::map<uint16_t, Descriptor> descriptors_ = {};
        Uuid uuid_ = {};

        Characteristic(uint16_t handle, uint8_t properties, uint16_t valueHandle, const Uuid uuid)
            : properties_(properties), handle_(handle), valueHandle_(valueHandle), uuid_(uuid)
        {}
    };

    struct Service {
        bool isPrimary_ = true;
        uint16_t handle_ = 0;
        uint16_t endHandle_ = 0;
        std::vector<IncludeService> includeServices_ = {};
        std::map<uint16_t, Characteristic> characteristics_ = {};
        Uuid uuid_ = {};

        Service()
        {}

        Service(bool isPrimary, uint16_t handle, uint16_t endHandle, const Uuid uuid)
            : isPrimary_(isPrimary), handle_(handle), endHandle_(endHandle), uuid_(uuid)
        {}
    };
    using Descriptors = std::pair<std::map<uint16_t, GattCache::Descriptor> *, uint16_t>;

    GattCache()
    {}
    ~GattCache()
    {}
    void Clear();
    void AddService(const Service &service);
    int AddIncludeService(uint16_t serviceHandle, const IncludeService &includeService);
    int AddCharacteristic(uint16_t serviceHandle, const Characteristic &characteristic);
    int AddDescriptor(uint16_t cccHandle, const Descriptor &descriptor);
    std::map<uint16_t, Service> &GetServices();
    std::vector<IncludeService> *GetIncludeServices(uint16_t serviceHandle);
    std::map<uint16_t, Characteristic> *GetCharacteristics(uint16_t serviceHandle);
    Descriptors GetDescriptors(uint16_t cccHandle);
    const GattCache::Characteristic *GetCharacteristic(int16_t valueHandle);
    const GattCache::Descriptor *GetDescriptor(int16_t valueHandle);
    uint16_t GetCharacteristicEndHandle(uint16_t serviceHandle, uint16_t cccHandle) const;

    int StoredToFile(const GattDevice& address) const;
    int LoadFromFile(const GattDevice& address);

    GattCache(GattCache &&src) = default;
    GattCache &operator=(GattCache &&src) = default;

private:
    struct StorageBlob {
        uint16_t handle_;
        Uuid type_;

        union {
            struct {
                Uuid uuid_;
                uint16_t endHandle_;
            } service_;

            struct {
                uint16_t handle_;
                uint16_t endHandle_;
                Uuid uuid_;
            } includeService_;

            struct {
                uint8_t properties_;
                uint16_t valueHandle_;
                Uuid uuid_;
            } characteristic_;
        } value_;
    };

    static const std::string GATT_STORAGE_PRIFIX;

    // service handle <-> struct Service
    std::map<uint16_t, Service> services_ = {};
    // value handle <-> (service handle, parent handle)
    // if value handle belong to descriptor, parent handle is characteristic handle witch descriptor belong to.
    // else parent handle is characteristic handle.
    std::map<uint16_t, std::pair<uint16_t, uint16_t>> valueHandleMap_ = {};

    static std::string GenerateGattCacheFileName(const GattDevice &address);
    int WriteStorageBlobToFile(const GattDevice& address, std::vector<StorageBlob> &blob) const;
    std::vector<StorageBlob> ReadStorageBlobFromFile(const GattDevice &address) const;

    BT_DISALLOW_COPY_AND_ASSIGN(GattCache);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // !GATT_CACHE_H