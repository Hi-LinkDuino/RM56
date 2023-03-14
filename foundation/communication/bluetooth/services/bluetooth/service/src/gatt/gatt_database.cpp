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


#include "gatt_database.h"
#include <algorithm>
#include <functional>
#include <optional>
#include "bt_def.h"
#include "gatt_defines.h"
#include "openssl/md4.h"

namespace OHOS {
namespace bluetooth {
using GattAttributeEntity = std::optional<std::reference_wrapper<GattDatabase::AttributeEntity>>;

GattDatabase::GattDatabase()
{
    availableHandles_.emplace_front(MIN_ATTRIBUTE_HANDLE, MAX_ATTRIBUTE_HANDLE);
}

int GattDatabase::AddService(bluetooth::Service &service)
{
    std::pair<uint16_t, uint16_t> availableHandlePair = CalculateAndAssignHandle(service);
    if (availableHandlePair == std::pair<uint16_t, uint16_t>(0, 0)) {
        return GattStatus::NOT_ENOUGH_HANDLES;
    }

    uint32_t currentHandle = availableHandlePair.first;
    service.handle_ = currentHandle++;
    service.endHandle_ = availableHandlePair.second;
    // copy service
    Service dbService(service);

    // copy includeService
    for (auto &includeService : service.includeServices_) {
        includeService.handle_ = currentHandle++;
        dbService.includeServices_.push_back(GattDatabase::IncludeService(includeService));
    }

    for (auto &ccc : service.characteristics_) {
        ccc.handle_ = currentHandle++;
        ccc.valueHandle_ = currentHandle++;
        // copy characteristic
        Characteristic dbCharacteristic(ccc);
        AttributeEntity cccAttributeValue(ccc.permissions_, ccc.valueHandle_, ccc.uuid_);
        cccAttributeValue.SetValue(ccc.value_.get(), ccc.length_);

        attributes_.emplace(ccc.valueHandle_, std::move(cccAttributeValue));
        valueHandleMap_.emplace(ccc.valueHandle_, std::make_pair(service.handle_, ccc.handle_));

        for (auto &descriptor : ccc.descriptors_) {
            descriptor.handle_ = currentHandle++;
            // copy descriptor
            Descriptor dbDescriptor(descriptor);
            AttributeEntity descAttributeValue(descriptor.permissions_, descriptor.handle_, descriptor.uuid_);
            descAttributeValue.SetValue(descriptor.value_.get(), descriptor.length_);

            attributes_.emplace(descriptor.handle_, std::move(descAttributeValue));
            valueHandleMap_.emplace(descriptor.handle_, std::make_pair(service.handle_, ccc.handle_));
            dbCharacteristic.descriptors_.emplace(dbDescriptor.handle_, std::move(dbDescriptor));
        }

        dbCharacteristic.endHandle_ = currentHandle - 1;
        dbService.characteristics_.emplace(dbCharacteristic.handle_, std::move(dbCharacteristic));
    }

    services_.emplace(dbService.handle_, std::move(dbService));
    return GattStatus::GATT_SUCCESS;
}

int GattDatabase::DeleteService(uint16_t handle)
{
    auto sIt = services_.find(handle);
    if (sIt == services_.end()) {
        return GattStatus::INVALID_PARAMETER;
    }
    if (IsReferenced(handle)) {
        return GattStatus::REFERENCED_BY_OTHER_SERVICE;
    }
    // release handles
    ReleaseHandle(sIt->second);
    // delete attribute and handle map
    for (auto &ccc : sIt->second.characteristics_) {
        attributes_.erase(ccc.second.valueHandle_);
        valueHandleMap_.erase(ccc.second.valueHandle_);
        for (auto &descriptor : ccc.second.descriptors_) {
            attributes_.erase(descriptor.second.handle_);
            valueHandleMap_.erase(descriptor.second.handle_);
        }
    }
    // delete service
    services_.erase(sIt);
    return GattStatus::GATT_SUCCESS;
}

void GattDatabase::RemoveAllServices()
{
    availableHandles_.clear();
    availableHandles_.emplace_front(MIN_ATTRIBUTE_HANDLE, MAX_ATTRIBUTE_HANDLE);
    services_.clear();
    attributes_.clear();
    valueHandleMap_.clear();
    restrictedGattBasedService_.clear();
}

void GattDatabase::ReleaseHandle(GattDatabase::Service &service)
{
    if (availableHandles_.empty()) {
        availableHandles_.emplace_front(service.handle_, service.endHandle_);
        return;
    }
    auto item = availableHandles_.begin();
    while (item != availableHandles_.end()) {
        auto currentNode = item;
        auto nextNode = ++item;

        //     service                  current
        // [start handle, end handle][first,second]
        if (currentNode->first > service.endHandle_) {
            //              release service           current
            // before   [start handle, end handle][c.first, c.second]
            // after    [start handle,                      c.second]
            if (currentNode->first == service.endHandle_ + 1) {
                availableHandles_.emplace(currentNode, service.handle_, currentNode->second);
                availableHandles_.erase(currentNode);
                //              release service           current
                // before   [start handle, end handle][c.first, c.second]
                // after    [start handle, end handle][c.first, c.second]
            } else {
                availableHandles_.emplace(currentNode, service.handle_, service.endHandle_);
            }

            break;
        }
        //   current           service
        // [first,second][start handle, end handle]
        if (nextNode == availableHandles_.end()) {
            //              current           release service
            // before   [c.first, c.second][start handle, end handle]
            // after    [c.first,                         end handle]
            if (service.handle_ == currentNode->second + 1) {
                availableHandles_.emplace(currentNode, currentNode->first, service.endHandle_);
                availableHandles_.erase(currentNode);
                //              current           release service
                // before   [c.first, c.second][start handle, end handle]
                // after    [c.first, c.second][start handle, end handle]
            } else {
                availableHandles_.emplace(nextNode, service.handle_, service.endHandle_);
            }

            break;
        } else {
            //   current            Next           service
            // [first,second][first,second][start handle, end handle]
            if (service.handle_ > nextNode->second) {
                continue;
            }
            //   current           service                Next
            // [first,second][start handle, end handle][first,second]

            //              current           release service              Next
            // before   [c.first, c.second][start handle, end handle][n.first, n.second]
            // after    [c.first,                         end handle][n.first, n.second]
            if (service.handle_ == currentNode->second + 1 && service.endHandle_ + 1 != nextNode->first) {
                availableHandles_.emplace(currentNode, currentNode->first, service.endHandle_);
                availableHandles_.erase(currentNode);
                //              current           release service           Next
                // before   [c.first, c.second][start handle, end handle][n.first, n.second]
                // after    [c.first, c.second][start handle,                      n.second]
            } else if (service.handle_ != currentNode->second + 1 && service.endHandle_ + 1 == nextNode->first) {
                availableHandles_.emplace(nextNode, service.handle_, nextNode->second);
                availableHandles_.erase(nextNode);
                //              current           release service            Next
                // before   [c.first, c.second][start handle, end handle][n.first, n.second]
                // after    [c.first,                                              n.second]
            } else if (service.handle_ == currentNode->second + 1 && service.endHandle_ + 1 == nextNode->first) {
                availableHandles_.emplace(nextNode, currentNode->first, nextNode->second);
                availableHandles_.erase(currentNode);
                availableHandles_.erase(nextNode);
                //              current           release service            Next
                // before   [c.first, c.second][start handle, end handle][n.first, n.second]
                // after    [c.first, c.second][start handle, end handle][n.first, n.second]
            } else {
                availableHandles_.emplace(nextNode, service.handle_, service.endHandle_);
            }

            break;
        }
    }
}

bool GattDatabase::IsReferenced(uint16_t handle) const
{
    return std::any_of(services_.begin(), services_.end(), [&handle](auto &svc) {
        return std::any_of(svc.second.includeServices_.begin(),
            svc.second.includeServices_.end(),
            [&handle](auto &iSvc) { return handle == iSvc.startHandle_;
        });
    });
}

const std::map<uint16_t, GattDatabase::Service> &GattDatabase::GetServices() const
{
    return services_;
}

const std::vector<GattDatabase::IncludeService> *GattDatabase::GetIncludeServices(uint16_t serviceHandle)
{
    auto service = services_.find(serviceHandle);
    if (service != services_.end()) {
        return &service->second.includeServices_;
    }
    return nullptr;
}

const GattDatabase::IncludeService *GattDatabase::GetIncludeService(uint16_t serviceHandle)
{
    for (auto &svc : services_) {
        auto it = std::find_if(svc.second.includeServices_.begin(),
            svc.second.includeServices_.end(),
            [&serviceHandle](auto &iSvc) { return iSvc.handle_ == serviceHandle; });

        if (it != svc.second.includeServices_.end()) {
            return it.base();
        }
    }
    return nullptr;
}

const std::map<uint16_t, GattDatabase::Characteristic> *GattDatabase::GetCharacteristics(uint16_t serviceHandle)
{
    auto service = services_.find(serviceHandle);
    if (service != services_.end()) {
        return &service->second.characteristics_;
    }
    return nullptr;
}

const GattDatabase::Service *GattDatabase::GetService(uint16_t handle)
{
    auto service = services_.find(handle);
    if (service != services_.end()) {
        return &service->second;
    }
    return nullptr;
}

GattDatabase::Characteristic *GattDatabase::GetCharacteristic(uint16_t valueHandle)
{
    auto it = valueHandleMap_.find(valueHandle);
    if (it != valueHandleMap_.end() && it->second.second == valueHandle - 1) {
        auto service = services_.find(it->second.first);
        if (service != services_.end()) {
            auto ccc = service->second.characteristics_.find(it->second.second);
            if (ccc != service->second.characteristics_.end()) {
                return &ccc->second;
            }
        }
    }
    return nullptr;
}

const GattDatabase::Descriptor *GattDatabase::GetDescriptor(uint16_t valueHandle)
{
    auto it = valueHandleMap_.find(valueHandle);
    if (it == valueHandleMap_.end()) {
        return nullptr;
    }
    auto service = services_.find(it->second.first);
    if (service == services_.end()) {
        return nullptr;
    }
    auto ccc = service->second.characteristics_.find(it->second.second);
    if (ccc == service->second.characteristics_.end()) {
        return nullptr;
    }
    auto descriptor = ccc->second.descriptors_.find(valueHandle);
    if (descriptor == ccc->second.descriptors_.end()) {
        return nullptr;
    }
    return &descriptor->second;
}

const std::map<uint16_t, GattDatabase::Descriptor> *GattDatabase::GetDescriptors(uint16_t cccHandle)
{
    for (auto &service : services_) {
        auto it = service.second.characteristics_.find(cccHandle);
        if (it != service.second.characteristics_.end()) {
            return &(it->second.descriptors_);
        }
    }
    return nullptr;
}

int GattDatabase::SetValueByHandle(const uint16_t handle, AttributeValue &value)
{
    auto it = attributes_.find(handle);
    if (it == attributes_.end()) {
        return GattStatus::HANDLE_NOT_FOUND;
    }
    it->second.value_ = std::move(value);
    return GattStatus::GATT_SUCCESS;
}

GattAttributeEntity GattDatabase::GetValueByHandle(const uint16_t handle)
{
    auto it = attributes_.find(handle);
    if (it == attributes_.end()) {
        return std::nullopt;
    }
    return std::ref(it->second);
}

int GattDatabase::CheckDescriptorsLegality(const bluetooth::Characteristic &characteristic) const
{
    for (auto &desc : characteristic.descriptors_) {
        if (desc.value_ == nullptr || desc.length_ == 0) {
            return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR_DATA;
        }

        if (desc.permissions_ < 0 || desc.permissions_ > 0x10) {
            return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR;
        }
    }

    if ((characteristic.properties_ & CHARACTERISTIC_PROPERTIE_BROADCAST) &&
        CountDescriptorByUuid(characteristic.descriptors_,
            Uuid::ConvertFrom16Bits(UUID_SERVER_CHARACTERISTIC_CONFIGURATION)) != 1) {
        return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR;
    }

    if (((characteristic.properties_ & CHARACTERISTIC_PROPERTIE_NOTIFY) ||
        (characteristic.properties_ & CHARACTERISTIC_PROPERTIE_INDICATE)) &&
        CountDescriptorByUuid(characteristic.descriptors_,
            Uuid::ConvertFrom16Bits(UUID_CLIENT_CHARACTERISTIC_CONFIGURATION)) != 1) {
        return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR;
    }

    if ((characteristic.properties_ & CHARACTERISTIC_PROPERTIE_EXTENDED_PROPERTIES) &&
        CountDescriptorByUuid(characteristic.descriptors_,
            Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC_EXTENDED_PROPERTIES)) != 1) {
        return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR;
    }

    if (CountDescriptorByUuid(
        characteristic.descriptors_, Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC_USER_DESCRIPTION)) > 1) {
        return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR;
    }

    if ((CountDescriptorByUuid(characteristic.descriptors_, Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC_FROMAT)) > 1) &&
        (CountDescriptorByUuid(characteristic.descriptors_,
            Uuid::ConvertFrom16Bits(UUID_CHARACTERISTIC_AGGREGATE_FROMAT)) != 1)) {
        return GattStatus::INVALID_CHARACTERISTIC_DESCRIPTOR;
    }

    return GattStatus::GATT_SUCCESS;
}

int GattDatabase::CheckCharacteristicsLegality(const bluetooth::Service &service) const
{
    for (auto &ccc : service.characteristics_) {
        if (ccc.value_ == nullptr || ccc.length_ == 0) {
            return GattStatus::INVALID_CHARACTERISTIC_DATA;
        }

        if (ccc.properties_ > 0xFF || ccc.properties_ < 0 || ccc.permissions_ < 0 || ccc.permissions_ > 0x10) {
            return GattStatus::INVALID_CHARACTERISTIC;
        }

        if (((ccc.properties_ & CHARACTERISTIC_PROPERTIE_READ) &&
            !(ccc.permissions_ & static_cast<int>(GattPermission::READABLE))) ||
            ((ccc.properties_ & CHARACTERISTIC_PROPERTIE_WRITE) &&
            !(ccc.permissions_ & static_cast<int>(GattPermission::WRITABLE)))) {
            return GattStatus::INVALID_CHARACTERISTIC;
        }

        int result = CheckDescriptorsLegality(ccc);
        if (GattStatus::GATT_SUCCESS != result) {
            return result;
        }
    }

    return GattStatus::GATT_SUCCESS;
}

int GattDatabase::CheckIncludeServicesLegality(bluetooth::Service &service) const
{
    for (auto &includeService : service.includeServices_) {
        auto sIt = services_.find(includeService.startHandle_);
        if (sIt == services_.end()) {
            return GattStatus::INCLUDE_SERVICE_NOT_FOUND;
        } else {
            includeService.endHandle_ = sIt->second.endHandle_;
            includeService.uuid_ = sIt->second.uuid_;
            includeService.isPrimary_ = sIt->second.isPrimary_;
        }
    }

    return GattStatus::GATT_SUCCESS;
}

int GattDatabase::CheckLegalityOfServiceDefinition(bluetooth::Service &service)
{
    if (!CheckRestrictedGattBasedService(service)) {
        return GattStatus::REQUEST_NOT_SUPPORT;
    }

    int result = CheckIncludeServicesLegality(service);
    if (GattStatus::GATT_SUCCESS != result) {
        return result;
    }

    result = CheckCharacteristicsLegality(service);
    if (GattStatus::GATT_SUCCESS != result) {
        return result;
    }

    return result;
}

bool GattDatabase::CheckRestrictedGattBasedService(const bluetooth::Service &service)
{
    if (service.uuid_.ConvertTo16Bits() == 0x180A) { // DIS
        if (restrictedGattBasedService_.count(0x180A) == 0) {
            restrictedGattBasedService_.emplace(0x180A);
        } else {
            return false;
        }
    } else if (service.uuid_.ConvertTo16Bits() == 0x1800) { // GAS
        if (restrictedGattBasedService_.count(0x1800) == 0) {
            restrictedGattBasedService_.emplace(0x1800);
        } else {
            return false;
        }
    } else if (service.uuid_.ConvertTo16Bits() == 0x1801) { // GATTS
        if (restrictedGattBasedService_.count(0x1801) == 0) {
            restrictedGattBasedService_.emplace(0x1801);
        } else {
            return false;
        }
    }

    return true;
}

int GattDatabase::CountDescriptorByUuid(const std::vector<bluetooth::Descriptor> &descriptors, const Uuid &uuid)
{
    return std::count_if(descriptors.begin(), descriptors.end(), [&uuid](auto &item) { return uuid == item.uuid_; });
}

std::pair<uint16_t, uint16_t> GattDatabase::CalculateAndAssignHandle(const bluetooth::Service &service)
{
    // calculate service
    uint16_t count = 1;
    std::pair<uint16_t, uint16_t> handlePair(0, 0);

    // calculate Service
    count += service.includeServices_.size();

    // calculate Characteristics
    count += service.characteristics_.size() * 0x2;
    // calculate Descripors
    for (auto &ccc : service.characteristics_) {
        count += ccc.descriptors_.size();
    }

    // assign handles
    for (auto item = availableHandles_.begin(); item != availableHandles_.end(); item++) {
        auto availableLength = item->second - item->first + 1;
        if (availableLength >= count) {
            handlePair = std::pair<uint16_t, uint16_t>(item->first, item->first + count - 1);
            if (handlePair.second < item->second) {
                availableHandles_.insert(item, std::pair<uint16_t, uint16_t>(handlePair.second + 1, item->second));
            }
            availableHandles_.erase(item);
            break;
        }
    }

    return handlePair;
}
}  // namespace bluetooth
}  // namespace OHOS
