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

#include "gatt_cache.h"
#include "bt_def.h"
#include "gatt_defines.h"

namespace OHOS {
namespace bluetooth {
using Descriptors = std::pair<std::map<uint16_t, GattCache::Descriptor> *, uint16_t>;

void GattCache::AddService(const Service &service)
{
    auto it = services_.emplace(service.handle_, service);
    if (!it.second) {
        it.first->second.endHandle_ = service.endHandle_;
        it.first->second.uuid_ = service.uuid_;
    }
}

void GattCache::Clear()
{
    services_.clear();
}

int GattCache::AddIncludeService(uint16_t serviceHandle, const IncludeService &includeService)
{
    auto it = services_.find(serviceHandle);
    if (it != services_.end()) {
        it->second.includeServices_.push_back(includeService);

        return GattStatus::GATT_SUCCESS;
    }

    return GattStatus::INVALID_PARAMETER;
}

int GattCache::AddCharacteristic(uint16_t serviceHandle, const Characteristic &characteristic)
{
    auto it = services_.find(serviceHandle);
    if (it != services_.end()) {
        auto result = it->second.characteristics_.emplace(characteristic.handle_, characteristic);
        if (result.second) {
            valueHandleMap_.emplace(characteristic.valueHandle_, std::make_pair(serviceHandle, characteristic.handle_));
        } else {
            result.first->second.properties_ = characteristic.properties_;
            result.first->second.uuid_ = characteristic.uuid_;
        }
        return GattStatus::GATT_SUCCESS;
    }
    return GattStatus::INVALID_PARAMETER;
}

int GattCache::AddDescriptor(uint16_t cccHandle, const Descriptor &descriptor)
{
    for (auto &sIt : services_) {
        auto cIt = sIt.second.characteristics_.find(cccHandle);
        if (cIt != sIt.second.characteristics_.end()) {
            cIt->second.descriptors_.emplace(descriptor.handle_, descriptor);
            valueHandleMap_.emplace(descriptor.handle_, std::make_pair(sIt.second.handle_, cIt->second.handle_));
            return GattStatus::GATT_SUCCESS;
        }
    }

    return GattStatus::INVALID_PARAMETER;
}

const GattCache::Characteristic *GattCache::GetCharacteristic(int16_t valueHandle)
{
    auto it = valueHandleMap_.find(valueHandle);
    if (it != valueHandleMap_.end()) {
        auto svc = services_.find(it->second.first);
        if (svc != services_.end()) {
            auto ccc = svc->second.characteristics_.find(it->second.second);
            if (ccc != svc->second.characteristics_.end()) {
                return &ccc->second;
            }
        }
    }
    return nullptr;
}

const GattCache::Descriptor *GattCache::GetDescriptor(int16_t valueHandle)
{
    auto it = valueHandleMap_.find(valueHandle);
    if (it == valueHandleMap_.end()) {
        return nullptr;
    }
    auto svc = services_.find(it->second.first);
    if (svc == services_.end()) {
        return nullptr;
    }
    auto ccc = svc->second.characteristics_.find(it->second.second);
    if (ccc == svc->second.characteristics_.end()) {
        return nullptr;
    }
    auto descriptor = ccc->second.descriptors_.find(valueHandle);
    if (descriptor != ccc->second.descriptors_.end()) {
        return nullptr;
    }
    return &descriptor->second;
}

uint16_t GattCache::GetCharacteristicEndHandle(uint16_t serviceHandle, uint16_t cccHandle) const
{
    auto svc = services_.find(serviceHandle);
    if (svc == services_.end()) {
        return INVALID_ATTRIBUTE_HANDLE;
    }

    auto ccc = svc->second.characteristics_.find(cccHandle);
    if (ccc == svc->second.characteristics_.end()) {
        return INVALID_ATTRIBUTE_HANDLE;
    }

    if (++ccc != svc->second.characteristics_.end()) {
        return ccc->second.handle_ - MIN_ATTRIBUTE_HANDLE;
    }

    return svc->second.endHandle_;
}

std::map<uint16_t, GattCache::Service> &GattCache::GetServices()
{
    return services_;
}

std::vector<GattCache::IncludeService> *GattCache::GetIncludeServices(uint16_t serviceHandle)
{
    auto service = services_.find(serviceHandle);
    if (service != services_.end()) {
        return &service->second.includeServices_;
    }
    return nullptr;
}

std::map<uint16_t, GattCache::Characteristic> *GattCache::GetCharacteristics(uint16_t serviceHandle)
{
    auto service = services_.find(serviceHandle);
    if (service != services_.end()) {
        return &service->second.characteristics_;
    }
    return nullptr;
}

Descriptors GattCache::GetDescriptors(uint16_t cccHandle)
{
    for (auto &service : services_) {
        auto it = service.second.characteristics_.find(cccHandle);
        if (it != service.second.characteristics_.end()) {
            return std::make_pair(&it->second.descriptors_, service.second.handle_);
        }
    }
    return std::make_pair(nullptr, 0);
}

const std::string GattCache::GATT_STORAGE_PRIFIX = "gatt_storage_cache_";

int GattCache::StoredToFile(const GattDevice& address) const
{
    std::vector<StorageBlob> storage;

    for (auto &svc : services_) {
        StorageBlob svcBlob = {};
        svcBlob.handle_ = svc.second.handle_;
        svcBlob.type_ = (svc.second.isPrimary_ ? Uuid::ConvertFrom16Bits(UUID_PRIMARY_SERVICE)
                                               : Uuid::ConvertFrom16Bits(UUID_SECONDARY_SERVICE));
        svcBlob.value_.service_.uuid_ = svc.second.uuid_;
        svcBlob.value_.service_.endHandle_ = svc.second.endHandle_;
        storage.push_back(svcBlob);

        for (auto &isvc : svc.second.includeServices_) {
            StorageBlob isvcBlob = {};
            isvcBlob.handle_ = isvc.handle_;
            isvcBlob.type_ = Uuid::ConvertFrom16Bits(UUID_INCLUDE_SERVICE);
            isvcBlob.value_.includeService_.handle_ = isvc.startHandle_;
            isvcBlob.value_.includeService_.endHandle_ = isvc.endHandle_;
            isvcBlob.value_.includeService_.uuid_ = isvc.uuid_;
            storage.push_back(isvcBlob);
        }

        for (auto &ccc : svc.second.characteristics_) {
            StorageBlob cccBlob = {};
            cccBlob.handle_ = ccc.second.handle_;
            cccBlob.type_ = Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC);
            cccBlob.value_.characteristic_.properties_ = ccc.second.properties_;
            cccBlob.value_.characteristic_.valueHandle_ = ccc.second.valueHandle_;
            cccBlob.value_.characteristic_.uuid_ = ccc.second.uuid_;
            storage.push_back(cccBlob);

            for (auto &desc : ccc.second.descriptors_) {
                StorageBlob descBlob = {};
                descBlob.handle_ = desc.second.handle_;
                descBlob.type_ = desc.second.uuid_;
                storage.push_back(descBlob);
            }
        }
    }

    return WriteStorageBlobToFile(address, storage);
}

int GattCache::LoadFromFile(const GattDevice& address)
{
    std::vector<StorageBlob> storage = ReadStorageBlobFromFile(address);

    uint16_t currentSvcHandle = 0;
    uint16_t currentCccHandle = 0;
    for (auto &item : storage) {
        if (item.type_ == Uuid::ConvertFrom16Bits(UUID_PRIMARY_SERVICE) ||
            item.type_ == Uuid::ConvertFrom16Bits(UUID_SECONDARY_SERVICE)) {
            AddService(
                GattCache::Service((item.type_ == Uuid::ConvertFrom16Bits(UUID_PRIMARY_SERVICE)) ? true : false,
                    item.handle_,
                    item.value_.service_.endHandle_,
                    item.value_.service_.uuid_));
            currentSvcHandle = item.handle_;
        } else if (item.type_ == Uuid::ConvertFrom16Bits(UUID_INCLUDE_SERVICE)) {
            AddIncludeService(currentSvcHandle,
                GattCache::IncludeService(item.handle_,
                    item.value_.includeService_.handle_,
                    item.value_.includeService_.endHandle_,
                    item.value_.includeService_.uuid_));
        } else if (item.type_ == Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC)) {
            AddCharacteristic(currentSvcHandle,
                GattCache::Characteristic(item.handle_,
                    item.value_.characteristic_.properties_,
                    item.value_.characteristic_.valueHandle_,
                    item.value_.characteristic_.uuid_));
            currentCccHandle = item.handle_;
        } else {
            AddDescriptor(currentCccHandle, GattCache::Descriptor(item.handle_, item.type_));
        }
    }

    return GattStatus::GATT_SUCCESS;
}

std::string GattCache::GenerateGattCacheFileName(const GattDevice &address)
{
    return (GATT_STORAGE_PRIFIX + address.addr_.GetAddress() + "_" +
           ((address.transport_ == GATT_TRANSPORT_TYPE_CLASSIC) ? "CLASSIC" : "LE"));
}

int GattCache::WriteStorageBlobToFile(const GattDevice& address, std::vector<StorageBlob> &blob) const
{
    FILE* fd = fopen(GenerateGattCacheFileName(address).c_str(), "wb");
    if (fd == nullptr) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    uint16_t blobSize = blob.size();
    if (fwrite(&blobSize, sizeof(uint16_t), 1, fd) != 1) {
        fclose(fd);
        return GattStatus::INTERNAL_ERROR;
    }

    if (fwrite(blob.data(), sizeof(StorageBlob), blobSize, fd) != blobSize) {
        fclose(fd);
        return GattStatus::INTERNAL_ERROR;
    }

    fclose(fd);

    return GattStatus::GATT_SUCCESS;
}

std::vector<GattCache::StorageBlob> GattCache::ReadStorageBlobFromFile(const GattDevice &address) const
{
    FILE* fd = fopen(GenerateGattCacheFileName(address).c_str(), "rb");
    if (fd == nullptr) {
        return std::vector<StorageBlob>();
    }

    uint16_t blobSize = 0;
    if (fread(&blobSize, sizeof(uint16_t), 1, fd) != 1) {
        fclose(fd);
        return std::vector<StorageBlob>();
    }

    std::vector<StorageBlob> blob(blobSize, {0, {}, {}});
    if (fread(blob.data(), sizeof(StorageBlob), blobSize, fd) != blobSize) {
        fclose(fd);
        return std::vector<StorageBlob>();
    }

    fclose(fd);
    return blob;
}
}  // namespace bluetooth
}  // namespace OHOS