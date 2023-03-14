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
 * @brief Declares function of gatt client definition.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GATT_SERVER_PROFILE_H
#define GATT_SERVER_PROFILE_H

#include "dispatcher.h"
#include "gatt_database.h"
#include "gatt_profile_defines.h"
#include "gatt_server_profile_callback.h"

namespace OHOS {
namespace bluetooth {
class GattServerProfile {
public:
    explicit GattServerProfile(
        GattServerProfileCallback *pServerCallbackFunc, utility::Dispatcher *dispatcher, uint16_t maxMtu);
    ~GattServerProfile();
    void Enable() const;
    void Disable() const;
    int AddService(Service &service) const;
    int CheckLegalityOfServiceDefinition(Service &service) const;
    int RemoveService(uint16_t serviceHandle) const;
    const std::string GetDBHash() const;
    const std::map<uint16_t, GattDatabase::Service> &GetServices() const;
    const GattDatabase::Service *GetService(uint16_t serviceHandle) const;
    GattDatabase::Characteristic *GetCharacteristic(uint16_t valueHandle) const;
    const GattDatabase::Descriptor *GetDescriptor(uint16_t valueHandle) const;
    void SetAttributeValue(uint16_t valueHandle, GattDatabase::AttributeValue &value) const;
    Buffer *GetAttributeValue(uint16_t handle) const;
    const std::optional<std::reference_wrapper<GattDatabase::AttributeEntity>> GetAttributeEntity(
        uint16_t handle) const;
    void SendNotification(uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void SendIndication(uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len) const;
    void SendReadCharacteristicValueResp(
        uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const;
    void SendReadUsingCharacteristicValueResp(
        uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const;
    void SendReadBlobValueResp(
        uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const;
    void SendWriteCharacteristicValueResp(uint16_t connectHandle, uint16_t handle, int result) const;
    void SendReadDescriptorResp(
        uint16_t connectHandle, uint16_t handle, const GattValue &value, size_t len, int result) const;
    void SendWriteDescriptorResp(uint16_t connectHandle, uint16_t handle, int result) const;
    void SendPrepareWriteValueResp(
        PrepareWriteParam param, const GattValue &value, size_t len, int result) const;
    static void SendExecuteWriteValueResp(uint16_t connectHandle);
    static int ConvertResponseErrorCode(int errorCode);
    BT_DISALLOW_COPY_AND_ASSIGN(GattServerProfile);

private:
    DECLARE_IMPL();
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GATT_SERVER_PROFILE_H
