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
 * @file gatt_client_profile.h
 *
 * @brief Declares function of gatt client definition.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GATT_SERVER_PROFILE_CALLBACK_H
#define GATT_SERVER_PROFILE_CALLBACK_H

namespace OHOS {
namespace bluetooth {
class GattServerProfileCallback {
public:
    virtual void OnExchangeMtuEvent(uint16_t connectHandle, uint16_t rxMtu){};
    virtual void OnReadCharacteristicValueEvent(uint16_t connectHandle, uint16_t valueHandle){};
    virtual void OnReadUsingCharacteristicUuidEvent(uint16_t connectHandle, uint16_t valueHandle){};
    virtual void ReadBlobValueEvent(uint16_t connectHandle, uint16_t valueHandle){};
    virtual void OnMultipleCharacteristicValueEvent(uint16_t connectHandle, GattValue &value, size_t len){};
    virtual void OnWriteWithoutResponseEvent(
        uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len){};
    virtual void OnWriteCharacteristicValueEvent(
        uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len){};
    virtual void OnPrepareWriteValueEvent(
        uint16_t connectHandle, uint16_t valueHandle, uint16_t offset, GattValue &value, size_t len){};
    virtual void OnExecuteWriteValueEvent(uint16_t connectHandle, bool flag){};
    virtual void OnIndicationEvent(uint16_t connectHandle, uint16_t valueHandle, int result){};
    virtual void OnReliableWriteEvent(uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len){};
    virtual void OnExecuteWriteEvent(uint16_t connectHandle, uint16_t valueHandle, int state){};
    virtual void OnDescriptorReadEvent(uint16_t connectHandle, uint16_t valueHandle){};
    virtual void OnDescriptorWriteEvent(uint16_t connectHandle, uint16_t valueHandle, GattValue &value, size_t len){};

    virtual ~GattServerProfileCallback()
    {}
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // GATT_SERVER_PROFILE_CALLBACK_H
