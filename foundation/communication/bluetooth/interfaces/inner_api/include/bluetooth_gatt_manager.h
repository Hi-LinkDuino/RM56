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
 * @file bluetooth_gatt_manager.h
 *
 * @brief gatt manager interface.
 *
 * @since 6
 *
 */

#ifndef BLUETOOTH_GATT_MANAGER_H
#define BLUETOOTH_GATT_MANAGER_H

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Class for Gatt Manager.
 *
 * @since 6
 *
 */
class BLUETOOTH_API GattManager {
public:
    static constexpr uint8_t GATT_CONNECTION_STATE_NUM = 4;
    /**
     * @brief The function to get devices by state.
     *
     * @param states State of devices.
     * @return list of devices.
     * @since 6
     *
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::array<int, GATT_CONNECTION_STATE_NUM> &states);

    /**
     * @brief The function to get devices by state.
     *
     * @return list of devices.
     * @since 6
     *
     */
    std::vector<BluetoothRemoteDevice> GetConnectedDevices();

    /**
     * @brief A constructor used to create a GattManager instance.
     *
     * @since 6
     *
     */
    GattManager();
    /**
     * @brief A destructor of GattManager.
     *
     * @since 6
     *
     */
    ~GattManager();

private:
    BLUETOOTH_DECLARE_IMPL();
};
} // namespace Bluetooth
} // namespace OHOS
#endif  // BLUETOOTH_GATT_MANAGER_H
