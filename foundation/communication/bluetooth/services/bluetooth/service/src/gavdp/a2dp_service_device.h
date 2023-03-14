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

#ifndef A2DP_SERVICE_DEVICE_H
#define A2DP_SERVICE_DEVICE_H

#include <cstdint>
#include <map>

#include "a2dp_service_state_machine.h"
#include "bt_def.h"
#include "btstack.h"
#include "interface_profile_a2dp_src.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Single device information management,including device address,playing state,
 *        connection state,state machine,etc.
 *
 * @since 6.0
 */
class A2dpDeviceInfo {
public:
    /**
     * @brief A constructor used to create an <b>A2dpDeviceInfo</b> instance.
     *
     * @since 6.0
     */
    explicit A2dpDeviceInfo(const RawAddress &device);

    /**
     * @brief A destructor used to delete the <b>A2dpDeviceInfo</b> instance.
     *
     * @since 6.0
     */
    ~A2dpDeviceInfo();

    /**
     * @brief Save codec status in Device information.
     *
     * @param codecStatusInfo The codec information.
     * @since 6.0
     */
    void SetCodecStatus(A2dpSrcCodecStatus codecStatusInfo);

    /**
     * @brief Save playing state in Device information.
     *
     * @param state The playing state of device.
     * @since 6.0
     */
    void SetPlayingState(bool state);

    /**
     * @brief Save connection state in Device information..
     *
     * @param state The connection state of device.
     * @since 6.0
     */
    void SetConnectState(int state);

    /**
     * @brief Save device handle in Device information.
     *
     * @param handleInfo The handle of device.
     * @since 6.0
     */
    void SetHandle(uint16_t handleInfo);

    /**
     * @brief Get device from Device information.
     *
     * @return Return the device address
     * @since 6.0
     */
    BtAddr GetDevice() const;

    /**
     * @brief Get state machine from Device information.
     *
     * @return Returns the device statemachine
     * @since 6.0
     */
    A2dpStateManager *GetStateMachine();

    /**
     * @brief Get codec status from Device information.
     *
     * @return Returns the device code information
     * @since 6.0
     */
    A2dpSrcCodecStatus GetCodecStatus() const;

    /**
     * @brief Get playing state from Device information.
     *
     * @return Returns <b>true</b> if device is on playing;
     *         Returns <b>false</b> if device is not on playing.
     * @since 6.0
     */
    bool GetPlayingState() const;

    /**
     * @brief Get connect state from Device information.
     *
     * @return Returns <b>DISCONNECTED</b> if device connect state is disconnected;
     *         Returns <b>DISCONNECTING</b> if device connect state is disconnecting;
     *         Returns <b>CONNECTED</b> if device connect state is connected;
     *         Returns <b>CONNECTING</b> if device connect state is connecting;
     * @since 6.0
     */
    int GetConnectState() const;

    /**
     * @brief Get device handle from Device information.
     *
     * @return Returns device handle value
     * @since 6.0
     */
    uint16_t GetHandle() const;

private:
    A2dpDeviceInfo() = delete;
    // The handle of device.
    uint16_t handle_ = 0;
    // The address of the bluetooth device.
    BtAddr peerAddress_ {};
    // The codec status information.
    A2dpSrcCodecStatus codecStatus_ {};
    // The playing state of device.
    bool isPlaying_ = false;
    // The connection state of device.
    int currentConnectState_ = static_cast<int>(BTConnectState::DISCONNECTED);
    // The pointer of device's state machine.
    A2dpStateManager state_ {};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // A2DP_SERVICE_DEVICE_H