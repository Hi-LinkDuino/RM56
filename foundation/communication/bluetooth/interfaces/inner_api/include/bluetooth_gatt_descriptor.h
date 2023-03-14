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
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines a bluetooth system that provides basic bluetooth connection and profile functions,
 *        including A2DP, AVRCP, BLE, GATT, HFP, MAP, PBAP, and SPP, etc.
 *
 * @since 6
 *
 */
/**
 * @file bluetooth_gatt_descriptor.h
 *
 * @brief Bluetooth gatt descriptor interface.
 *
 * @since 6
 *
 */

#ifndef BLUETOOTH_GATT_DESCRIPTOR_H
#define BLUETOOTH_GATT_DESCRIPTOR_H

#include <memory>

#include "bluetooth_def.h"
#include "bluetooth_types.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for GattCharacteristic functions.
 *
 * @since 6
 *
 */
class GattCharacteristic;
/**
 * @brief GATT-based Descriptor class
 * Descriptors describe the value or permit configuration of the server with respect to the characteristic.
 */
class BLUETOOTH_API GattDescriptor {
public:
    /**
     * @brief The function to delete constructor of GattDescriptor.
     *
     * @since 6
     *
     */
    GattDescriptor() = delete;

    /**
     * @brief A constructor of GattDescriptor.
     *
     * @param uuid Uuid of Gatt Descriptor.
     * @param permissions permissions of Gatt Descriptor.
     * @since 6
     *
     */
    GattDescriptor(const UUID uuid, const int permissions);

    /**
     * @brief A constructor of GattDescriptor.
     *
     * @param uuid Uuid of Gatt Descriptor.
     * @param handle handle of Gatt Descriptor.
     * @param permissions permissions of Gatt Descriptor.
     * @since 6
     *
     */
    GattDescriptor(const UUID uuid, uint16_t handle, const int permissions);
    explicit GattDescriptor(const GattDescriptor &);
    GattDescriptor &operator=(const GattDescriptor &);
    GattDescriptor(GattDescriptor &&) = default;
    GattDescriptor &operator=(GattDescriptor &&) = default;

    /**
     * @brief The function to get characteristic.
     *
     * @since 6
     *
     */
    GattCharacteristic *GetCharacteristic() const;

    /**
     * @brief The function to get handle.
     *
     * @return handle.
     * @since 6
     *
     */
    uint16_t GetHandle() const;

    /**
     * @brief The function to get permissions.
     *
     * @return permissions.
     * @since 6
     *
     */
    int GetPermissions() const;

    /**
     * @brief The function to get uuid.
     *
     * @return UUID.
     * @since 6
     *
     */
    const UUID &GetUuid() const;

    /**
     * @brief The function to get value.
     *
     * @param size size of get value.
     * @return value.
     * @since 6
     *
     */
    const std::unique_ptr<uint8_t[]> &GetValue(size_t *size) const;

    /**
     * @brief The function to set value.
     *
     * @param values values of set value.
     * @param length length of set value.
     * @since 6
     *
     */
    void SetValue(const uint8_t *values, const size_t length);

private:
    /**
     * @brief The handle of descriptor.
     *
     * @since 6
     *
     */
    uint16_t handle_;

    /**
     * @brief The permissions of descriptor.
     *
     * @since 6
     *
     */
    int permissions_;

    /**
     * @brief The characteristic of descriptor.
     *
     * @since 6
     *
     */
    GattCharacteristic *characteristic_;

    /**
     * @brief The value of descriptor.
     *
     * @since 6
     *
     */
    std::unique_ptr<uint8_t[]> value_;

    /**
     * @brief The length of descriptor.
     *
     * @since 6
     *
     */
    size_t length_;

    /**
     * @brief The uuid of descriptor.
     *
     * @since 6
     *
     */
    UUID uuid_;
    friend class GattCharacteristic;
};
} // namespace Bluetooth
} // namespace OHOS

#endif  // BLUETOOTH_GATT_DESCRIPTOR_H
