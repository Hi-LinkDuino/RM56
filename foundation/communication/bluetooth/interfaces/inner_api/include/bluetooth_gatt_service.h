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
 * @file bluetooth_gatt_service.h
 *
 * @brief gatt service interface.
 *
 * @since 6
 *
 */

#ifndef BLUETOOTH_GATT_SERVICE_H
#define BLUETOOTH_GATT_SERVICE_H

#include "bluetooth_gatt_characteristic.h"

namespace OHOS {
namespace Bluetooth {
/** A GATT-based Service type.
 *  Define GATT-based Service types.
 */
enum class GattServiceType : int {
    PRIMARY,   /**< primary service */
    SECONDARY, /**< secondary service */
};

/**
 * @brief Class for Gatt Service API.
 *
 * @since 6
 *
 */
class BLUETOOTH_API GattService {
public:
    /**
     * @brief The function to delete constructor of GattService.
     *
     * @since 6
     *
     */
    GattService() = delete;
    /**
     * @brief A constructor of GattService.
     *
     * @param uuid UUID of service.
     * @param type Type of service.
     * @since 6
     *
     */
    GattService(const UUID &uuid, const GattServiceType type);
    /**
     * @brief A constructor of GattService.
     *
     * @param uuid UUID of service.
     * @param handle Handle of service.
     * @param endHandle EndHandle of service.
     * @param type Type of service.
     * @since 6
     *
     */
    GattService(const UUID &uuid, uint16_t handle, uint16_t endHandle, const GattServiceType type);

    /**
     * @brief The function to add characteristic.
     *
     * @param characteristic Characteristic object to add.
     * @since 6
     *
     */
    void AddCharacteristic(const GattCharacteristic &characteristic);
    /**
     * @brief The function to add include service.
     *
     * @param characteristic Service object to add.
     * @since 6
     *
     */
    void AddService(GattService &service);
    /**
     * @brief The function to get characteristic by UUID.
     *
     * @param uuid UUID of characteristic.
     * @return characteristic.
     * @since 6
     *
     */
    GattCharacteristic *GetCharacteristic(const UUID &uuid);
    /**
     * @brief The function to get all characteristics.
     *
     * @return list of characteristics.
     * @since 6
     *
     */
    std::vector<GattCharacteristic> &GetCharacteristics();
    /**
     * @brief The function to get include services.
     *
     * @return list of include services.
     * @since 6
     *
     */
    const std::vector<std::reference_wrapper<GattService>> &GetIncludedServices();
    /**
     * @brief The function to get service's handle.
     *
     * @return handle.
     * @since 6
     *
     */
    uint16_t GetHandle() const;
    /**
     * @brief The function to get service's type.
     *
     * @return bool   primary or not.
     * @since 6
     *
     */
    bool IsPrimary() const;
    /**
     * @brief The function to get service's UUID.
     *
     * @return UUID.
     * @since 6
     *
     */
    const UUID &GetUuid() const;

    GattService(const GattService &);
    GattService &operator=(const GattService &) = default;

    GattService(GattService &&);
    GattService &operator=(GattService &&) = default;

private:
    /**
     * @brief The handle of service.
     *
     * @since 6
     *
     */
    uint16_t handle_;
    /**
     * @brief The endHandle of service.
     *
     * @since 6
     *
     */
    uint16_t endHandle_;
    /**
     * @brief The type of service.
     *
     * @since 6
     *
     */
    GattServiceType serviceType_;
    /**
     * @brief The list of current service's include services.
     *
     * @since 6
     *
     */
    std::vector<std::reference_wrapper<GattService>> includeServices_;
    /**
     * @brief The characteristics of service.
     *
     * @since 6
     *
     */
    std::vector<GattCharacteristic> characteristics_;
    /**
     * @brief The UUID of service.
     *
     * @since 6
     *
     */
    UUID uuid_;
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_GATT_SERVICE_H
