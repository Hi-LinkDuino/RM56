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
 * @brief Defines device class.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file bluetooth_device_class.h
 *
 * @brief Bluetooth device class.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef BLUETOOTH_DEVICE_CLASS_H
#define BLUETOOTH_DEVICE_CLASS_H

#include "bluetooth_def.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Represents device class.
 *
 * @since 1.0
 * @version 1.0
 */
class BLUETOOTH_API BluetoothDeviceClass {
public:
    /**
     * @brief A constructor used to create a <b>BluetoothDeviceClass</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    BluetoothDeviceClass();

    /**
     * @brief A constructor used to create a <b>BluetoothDeviceClass</b> instance.
     *
     * @param deviceClass Device class to create an <b>BluetoothDeviceClass</b> instance.
     * @since 1.0
     * @version 1.0
     */
    BluetoothDeviceClass(int deviceClass);

    /**
     * @brief A destructor used to delete the <b>BluetoothDeviceClass</b> instance.
     *
     * @since 1.0
     * @version 1.0
     */
    ~BluetoothDeviceClass();

    /**
     * @brief Get major class.
     *
     * @return Returns device major class.
     * @since 1.0
     * @version 1.0
     */
    int GetMajorClass() const;

    /**
     * @brief Get major minor class.
     *
     * @return Returns device major minor class.
     * @since 1.0
     * @version 1.0
     */
    int GetMajorMinorClass() const;

    /**
     * @brief Get class of device.
     *
     * @return Returns the class of device.
     * @since 1.0
     * @version 1.0
     */
    int GetClassOfDevice() const;

    /**
     * @brief Check if device supporte some profiles.
     *
     * @param profileId Profile Id.
     * @return Returns <b>true</b> if device supported this profile;
     *         returns <b>false</b> if device did not support this profile.
     * @since 1.0
     * @version 1.0
     */
    bool IsProfileSupported(int profileId) const;

    /**
     * @brief Check if device supporte some services.
     *
     * @param profileId Profile Id.
     * @return Returns <b>true</b> if device supported this service;
     *         returns <b>false</b> if device did not support this service.
     * @since 1.0
     * @version 1.0
     */
    bool IsServiceSupported(int service) const;

private:
    bool IsA2dpSupported() const;
    bool IsA2dpSinkSupported() const;
    bool IsHeadSetSupported() const;
    bool IsOppSupported() const;
    int class_;
};
} // namespace Bluetooth
} // namespace OHOS

#endif  // BLUETOOTH_DEVICE_CLASS_H
