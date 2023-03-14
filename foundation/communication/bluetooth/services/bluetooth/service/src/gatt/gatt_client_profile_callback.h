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
 * @file gatt_client_profile_callback.h
 *
 * @brief Declares function definition of gatt client service callback.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GATT_CLIENT_PROFILE_CALLBACK_H
#define GATT_CLIENT_PROFILE_CALLBACK_H

#include "gatt_cache.h"
#include "gatt_defines.h"
namespace OHOS {
namespace bluetooth {
class GattClientProfileCallback {
public:
    virtual void OnExchangeMtuEvent(int reqId, uint16_t connectHandle, uint16_t rxMtu, bool status){};
    virtual void OnDiscoverAllPrimaryServicesEvent(
        int reqId, int result, uint16_t connectHandle, const std::map<uint16_t, GattCache::Service> &services){};
    virtual void OnDiscoverPrimaryServicesByServiceUUIDEvent(
        int reqId, int result, uint16_t connectHandle, const std::map<uint16_t, GattCache::Service> &services){};
    virtual void OnFindIncludedServicesEvent(int reqId, int result, uint16_t connectHandle, uint16_t serviceHandle,
        const std::vector<GattCache::IncludeService> &services){};
    virtual void OnDiscoverAllCharacteristicOfServiceEvent(int reqId, int result, uint16_t connectHandle,
        uint16_t serviceHandle, const std::map<uint16_t, GattCache::Characteristic> &characteristics){};
    virtual void OnDiscoverCharacteristicByUUIDEvent(int reqId, int result, uint16_t connectHandle,
        uint16_t serviceHandle, const std::map<uint16_t, GattCache::Characteristic> &characteristics){};
    virtual void OnDiscoverAllCharacteristicDescriptorsEvent(int reqId, int result, uint16_t serviceHandle,
        uint16_t characteristicHandle, const std::map<uint16_t, GattCache::Descriptor> &descriptors){};
    virtual void OnReadCharacteristicValueEvent(int reqId, uint16_t handle, GattValue &value, size_t len, int result){};
    virtual void OnWriteCharacteristicValueEvent(int reqId, uint16_t connectHandle, uint16_t handle, int result){};
    virtual void OnWriteLongCharacteristicValueEvent(int reqId, uint16_t connectHandle, uint16_t handle, int result){};
    virtual void OnWriteLongValueOverageDataEvent(
        int reqId, uint16_t connectHandle, uint16_t handle, GattValue &value, size_t len){};
    virtual void OnReadDescriptorValueEvent(int reqId, uint16_t handle, GattValue &value, size_t len, int result){};
    virtual void OnWriteDescriptorValueEvent(int reqId, uint16_t connectHandle, uint16_t handle, int result){};
    virtual void OnCharacteristicNotifyEvent(
        uint16_t connectHandle, uint16_t handle, GattValue &value, size_t len, bool needConfirm){};
    virtual void OnReliableWriteCharacteristicValueEvent(
        int reqId, uint16_t handle, GattValue &value, size_t len, int result){};
    virtual void OnExecuteWriteValueEvent(int reqId, uint16_t connectHandle, int result){};
    virtual ~GattClientProfileCallback()
    {}
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GATT_CLIENT_PROFILE_CALLBACK_H
