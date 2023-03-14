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

#ifndef A2DP_SERVICE_CONNECTION_H
#define A2DP_SERVICE_CONNECTION_H

#include <cstdint>

#include "a2dp_service_device.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
class A2dpConnectManager {
public:
    /**
     * @brief A constructor used to create an <b>A2dpConnectManager</b> instance.
     *
     * @param role The a2dp profile role of local device.
     * @since 6.0
     */
    explicit A2dpConnectManager(uint8_t role);

    /**
     * @brief A constructor used to create an <b>A2dpConnectManager</b> instance.
     *
     * @since 6.0
     */
    A2dpConnectManager() = default;

    /**
     * @brief A destructor used to delete the <b>A2dpConnectManager</b> instance.
     *
     * @since 6.0
     */
    ~A2dpConnectManager(void) = default;

    /**
     * @brief Connect to the peer bluetooth device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>true</b> if the operation is successful.
     *         Returns <b>false</b> if the operation fails.
     * @since 6.0
     */
    bool A2dpConnect(const RawAddress &device);

    /**
     * @brief Disconnect with the peer bluetooth service.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>true</b> if the operation is successful.
     *         Returns <b>false</b> if the operation fails.
     * @since 6.0
     */
    bool A2dpDisconnect(const RawAddress &device);

    /**
     * @brief Add the peer bluetooth service to local device management list.
     *
     * @param device The address of the peer bluetooth device.
     * @param state The connection state of the peer bluetooth device.
     * @return The device pointer of peer device
     * @since 6.0
     */
    A2dpDeviceInfo *AddDevice(const RawAddress &device, int state);

    /**
     * @brief Delete the peer bluetooth service from local device management list.
     *
     * @param device The address of the peer bluetooth device.
     * @return void
     * @since 6.0
     */
    void DeleteDevice(const RawAddress &device);
    /**
     * @brief Get local device's a2dp profile role.
     *
     * @return Returns <b>A2DP_ROLE_SOURCE</b> if local device is a2dp source role.
     *         Returns <b>A2DP_ROLE_SINK</b> if local device is a2dp sink role.
     * @since 6.0
     */
    uint8_t GetRole() const
    {
        return role_;
    }

    /**
     * @brief Judge whether the number of connections is the maximum.
     *
     * @return Returns <b>true</b> if the number of connections is less than the maximum.
     *         Returns <b>false</b> if the number of connections is greater than or equal to the maximum.
     * @since 6.0
     */
    bool JudgeConnectedNum() const;

    /**
     * @brief Judge whether the current device have connected peer as another role.
     *
     * @param device The address of the peer bluetooth device.
     * @param role The local device's profile role.
     * @return Returns <b>true</b> if the current device have connected peer as another role.
     *         Returns <b>false</b> if the current device have not connected peer as another role.
     * @since 6.0
     */
    static bool JudgeConnectExit(const RawAddress &device, uint8_t role);

private:
    // The a2dp profile role of local device.
    uint8_t role_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_SERVICE_CONNECTION_H