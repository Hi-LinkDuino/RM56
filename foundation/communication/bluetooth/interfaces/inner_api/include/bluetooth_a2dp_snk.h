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
 * @brief The framework interface and callback function of a2dp sink are defined.
 *
 * @since 6
 */

#ifndef BLUETOOTH_A2DP_SNK_H
#define BLUETOOTH_A2DP_SNK_H

#include <cstdio>
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief A2dp sink API callback function.
 *
 * @since 6.0
 */
class A2dpSinkObserver {
public:
    /**
     * @brief A destructor used to delete the a2dp sink Observer instance.
     *
     * @since 6.0
     */
    virtual ~A2dpSinkObserver() = default;

    /**
     * @brief ConnectionState Changed observer
     * @param device bluetooth device address
     * @param state Event Reported
     * @since 6.0
     */
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
    {};
};

/**
 * @brief A2dp sink API.
 *
 * @since 6.0
 */
class BLUETOOTH_API A2dpSink {
public:
    /**
     * @brief Get a2dp sink instance.
     *
     * @return Returns an instance of a2dp sink.
     * @since 6.0
     */
    static A2dpSink *GetProfile();

    /**
     * @brief Get devices by connection states.
     *
     * @param states The connection states of the bluetooth device.
     * @return Returns devices that match the connection states.
     * @since 6.0
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states) const;

    /**
     * @brief Get device connection state by address.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>A2DP_DISCONNECTED</b> if device connect state is disconnected;
     *         Returns <b>A2DP_DISCONNECTING</b> if device connect state is disconnecting;
     *         Returns <b>A2DP_CONNECTED</b> if device connect state is connected;
     *         Returns <b>A2DP_CONNECTING</b> if device connect state is connecting;
     *         Returns <b>A2DP_INVALID_STATUS</b> if can not find peer device.
     * @since 6.0
     */
    int GetDeviceState(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Get device playing state by address when peer device is on connected.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>1</b> if device is on playing;
     *         Returns <b>0</b> if device is not on playing.
     * @since 6.0
     */
    int GetPlayingState(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Connect to the peer bluetooth device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>true</b> Perform normal connection processing.
     *         Returns <b>false</b> Target device is on connected,or connecting,
                                    or device is not allowed to connect,or the connection fails.
     * @since 6.0
     */
    bool Connect(const BluetoothRemoteDevice &device);

    /**
     * @brief Disconnect with the peer bluetooth service.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>true</b> if perform normal disconnection processing.
     *         Returns <b>false</b> if target device is on disconnected,or disconnecting,or disconnection fails.
     * @since 6.0
     */
    bool Disconnect(const BluetoothRemoteDevice &device);

    /**
     * @brief Set connection strategy for peer bluetooth device.
     *        If peer device is connected and the policy is set not allowed,then perform disconnect operation.
     *        If peer device is disconnected and the policy is set allowed,then perform connect operation.
     *
     * @param device The address of the peer bluetooth device.
     * @param strategy The device connect strategy.
     * @return Returns <b>true</b> if the operation is successful.
     *         Returns <b>false</b> if the operation fails.
     * @since 6.0
     */
    bool SetConnectStrategy(const BluetoothRemoteDevice &device, int strategy);

    /**
     * @brief Get connection strategy of peer bluetooth device.
     *
     * @param device The address of the peer bluetooth device.
     * @return Returns <b>A2DP_CONNECT_POLICY_AGREE</b> if the peer device is allowed to connect.
     *         Returns <b>A2DP_CONNECT_POLICY_DISAGREE</b> if the peer device is not allowed to connect.
     * @since 6.0
     */
    int GetConnectStrategy(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Send delay reporting.
     *
     * @param device The address of the peer bluetooth device.
     * @param delayValue The delay value.
     * @return Returns <b>true</b> Set delay reporting success.
     *         Returns <b>false</b> Set delay reporting failed,
     */
    bool SendDelay(const BluetoothRemoteDevice &device, uint16_t delayValue);

    /**
     * @brief Register callback function of framework.
     *
     * @param observer Reference to the a2dp sink observer.
     * @since 6.0
     */
    void RegisterObserver(A2dpSinkObserver *observer);

    /**
     * @brief Deregister callback function of framework.
     *
     * @param observer Reference to the a2dp sink observer.
     * @since 6.0
     */
    void DeregisterObserver(A2dpSinkObserver *observer);

private:
    /**
     * @brief A constructor used to create a a2dp sink instance.
     *
     * @since 6.0
     */
    A2dpSink(void);

    /**
     * @brief A destructor used to delete the a2dp sink instance.
     *
     * @since 6.0
     */
    ~A2dpSink(void);
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(A2dpSink);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_A2DP_SNK_H