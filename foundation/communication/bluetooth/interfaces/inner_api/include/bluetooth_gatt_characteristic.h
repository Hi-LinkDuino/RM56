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
 * @file bluetooth_gatt_characteristic.h
 *
 * @brief Bluetooth gatt characteristic interface.
 *
 * @since 6
 *
 */

#ifndef BLUETOOTH_GATT_CHARACTERISTIC_H
#define BLUETOOTH_GATT_CHARACTERISTIC_H

#include <cstddef>
#include <stdint.h>
#include "bluetooth_def.h"
#include "bluetooth_gatt_descriptor.h"
#include "cstdint"
#include <list>
#include "memory"
#include "uuid.h"
#include "vector"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for GattService functions.
 *
 * @since 6
 *
 */
class GattService;
/**
 * @brief GATT-based Characteristic class
 * A characteristic is a value used in a service along with properties and configuration information about
 * how the value is accessed and information about how the value is displayed or represented.
 */
class BLUETOOTH_API GattCharacteristic {
public:

    /** A GATT characteristic value permission.
     *  Define GATT characteristic permission.
     */
    enum Permission {
        READABLE = 0x01,
        WRITEABLE = 0x02,
    };


    /** A GATT characteristic value write type.
     *  Define GATT characteristic value write types.
     */
    enum WriteType {
        DEFAULT,
        NO_RESPONSE,
        SIGNED,
    };

    /** A GATT characteristic propertie.
     *  Define GATT characteristic properties.
     */
    enum Propertie {
        BROADCAST = 0x01, /**< readable */
        READ = 0x02,
        WRITE_WITHOUT_RESPONSE = 0x04,
        WRITE = 0x08,
        NOTIFY = 0x10,
        INDICATE = 0x20,
        AUTHENTICATED_SIGNED_WRITES = 0x40,
        EXTENDED_PROPERTIES = 0x80
    };

    /**
     * @brief The function to delete constructor of GattCharacteristic.
     *
     * @since 6
     *
     */
    GattCharacteristic() = delete;

    /**
     * @brief A constructor of GattCharacteristic.
     *
     * @param uuid Uuid of Gatt Characteristic.
     * @param permissions permissions of Gatt Characteristic.
     * @param properties properties of Gatt Characteristic.
     * @since 6
     *
     */
    GattCharacteristic(const UUID uuid, int permissions, int properties);

    /**
     * @brief A constructor of GattCharacteristic.
     *
     * @param uuid Uuid of Gatt Characteristic.
     * @param handle handle of Gatt Characteristic.
     * @param properties properties of Gatt Characteristic.
     * @param properties properties of Gatt Characteristic.
     * @since 6
     *
     */
    GattCharacteristic(const UUID uuid, uint16_t handle, const int permissions, const int properties);
    GattCharacteristic(const GattCharacteristic &);
    GattCharacteristic &operator=(const GattCharacteristic &);
    GattCharacteristic(GattCharacteristic &&);
    GattCharacteristic &operator=(GattCharacteristic &&) = default;

    /**
     * @brief The function to add descriptor.
     *
     * @param descriptor Descriptor object to add.
     * @since 6
     *
     */
    void AddDescriptor(const GattDescriptor &descriptor);

    /**
     * @brief The function to get descriptor by UUID.
     *
     * @param uuid Uuid of Gatt Descriptor.
     * @return descriptor or nullptr.
     * @since 6
     *
     */
    GattDescriptor *GetDescriptor(const UUID &uuid);

    /**
     * @brief The function to get descriptors.
     *
     * @return list of descriptors.
     * @since 6
     *
     */
    std::vector<GattDescriptor> &GetDescriptors();

    /**
     * @brief The function to get handle.
     *
     * @return uint16_t  handle.
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
     * @brief The function to get properties.
     *
     * @return properties.
     * @since 6
     *
     */
    int GetProperties() const;

    /**
     * @brief The function to get service.
     *
     * @return service which characteristic belong to.
     * @since 6
     *
     */
    GattService *GetService() const;

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
     * @return value pointer.
     * @since 6
     *
     */
    const std::unique_ptr<uint8_t[]> &GetValue(size_t *size) const;

    /**
     * @brief The function to get write type.
     *
     * @return write type.
     * @since 6
     *
     */
    int GetWriteType() const;

    /**
     * @brief The function to set write type.
     *
     * @param type type of set write type.
     * @return result of #GATT_STATUS.
     * @since 6
     *
     */
    int SetWriteType(int type);

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
     * @brief The writeType of characteristic.
     *
     * @since 6
     *
     */
    uint8_t writeType_;

    /**
     * @brief The handle of characteristic.
     *
     * @since 6
     *
     */
    uint16_t handle_;

    /**
     * @brief The permissions of characteristic.
     *
     * @since 6
     *
     */
    int permissions_;

    /**
     * @brief The properties of characteristic.
     *
     * @since 6
     *
     */
    int properties_;

    /**
     * @brief The service of characteristic.
     *
     * @since 6
     *
     */
    GattService *service_;

    /**
     * @brief The value of characteristic.
     *
     * @since 6
     *
     */
    std::unique_ptr<uint8_t[]> value_;

    /**
     * @brief The length of characteristic.
     *
     * @since 6
     *
     */
    size_t length_;

    /**
     * @brief The descriptors of characteristic.
     *
     * @since 6
     *
     */
    std::vector<GattDescriptor> descriptors_;

    /**
     * @brief The uuid of characteristic.
     *
     * @since 6
     *
     */
    UUID uuid_;

    friend class GattService;
};

} // namespace Bluetooth
} // namespace OHOS

#endif  // BLUETOOTH_GATT_CHARACTERISTIC_H
