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

/**
 * @addtogroup Graphic
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file gatt_client_profile.h
 *
 * @brief This file contains all the functions for gatt client.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GATT_CLIENT_PROFILE_H
#define GATT_CLIENT_PROFILE_H

#include "dispatcher.h"
#include "gatt_client_profile_callback.h"

namespace OHOS {
namespace bluetooth {
class GattClientProfile {
public:
    explicit GattClientProfile(GattClientProfileCallback *pClientCallbackFunc, utility::Dispatcher *dispatcher);
    ~GattClientProfile();
    void Enable() const;
    void Disable() const;
    void ExchangeMtu(int reqId, uint16_t connectHandle, uint16_t mtu) const;
    void DiscoverAllPrimaryServices(
        int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const;
    void DiscoverAllPrimaryServicesInter(
        int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const;
    void DiscoverPrimaryServicesByServiceUuid(int reqId, uint16_t connectHandle, const Uuid &uuid) const;
    void FindIncludedServices(
        int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const;
    void DiscoverAllCharacteristicOfService(
        int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const;
    void DiscoverCharacteristicByUuid(
        int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle, const Uuid &uuid) const;
    void DiscoverAllCharacteristicDescriptors(
        int reqId, uint16_t connectHandle, uint16_t startHandle, uint16_t endHandle) const;
    void ReadCharacteristicValue(int reqId, uint16_t connectHandle, uint16_t handle) const;
    void ReadUsingCharacteristicByUuid(int reqId, uint16_t connectHandle, const Uuid &uuid) const;
    void ReadLongCharacteristicValue(int reqId, uint16_t connectHandle, uint16_t handle) const;
    void ReadDescriptorValue(int reqId, uint16_t connectHandle, uint16_t handle) const;
    void ReadLongCharacteristicDescriptor(int reqId, uint16_t connectHandle, uint16_t handle) const;
    void ReadMultipleCharacteristicValue(
        int reqId, uint16_t connectHandle, const GattValue &value, size_t len) const;
    void WriteWithoutResponse(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void SignedWriteWithoutResponse(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void WriteCharacteristicValue(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void WriteLongCharacteristicValue(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void WriteDescriptorValue(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void WriteLongCharacteristicDescriptor(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void ReliableWriteCharacteristicValue(
        int reqId, uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void ExecuteWriteRequest(int reqId, uint16_t connectHandle, uint8_t flag) const;
    void HandleValueConfirmation(uint16_t connectHandle) const;
    void ClearCacheMap(uint16_t connectHandle) const;
    std::map<uint16_t, GattCache::Service> *GetServices(uint16_t connectHandle) const;
    uint16_t GetCharacteristicEndHandle(uint16_t connectHandle, uint16_t svcHandle, uint16_t handle) const;
    const GattCache::Service *GetService(uint16_t connectHandle, int16_t handle) const;
    const GattCache::Characteristic *GetCharacteristic(uint16_t connectHandle, int16_t valueHandle) const;
    const GattCache::Descriptor *GetDescriptor(uint16_t connectHandle, int16_t valueHandle) const;
    BT_DISALLOW_COPY_AND_ASSIGN(GattClientProfile);

private:
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GATT_CLIENT_PROFILE_H
