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
 * @brief The framework interface and callback function of map server are defined.
 *
 * @since 6
 */

/**
 * @file bluetooth_map_mse.h
 *
 * @brief map server interface.
 *
 * @since 6
 */
#ifndef BLUETOOTH_MAP_MSE_H
#define BLUETOOTH_MAP_MSE_H

#include "bluetooth_def.h"
#include "bluetooth_types.h"
#include "bluetooth_remote_device.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief Map server API callback function, including connection, disconnection and authorization.
 *
 * @since 6
 */
class MapServerObserver {
public:
    /**
     * @brief A destructor used to delete the Map Server Observer instance.
     *
     * @since 6
     */
    virtual ~MapServerObserver() = default;

    /**
     * @brief Callback function when the connection state changes.
     *
     * @param device  the remote bluetooth device.
     * @param state   the connection status.
     *     @c  BTConnectState::CONNECTING : the state is connecting.
     *     @c  BTConnectState::CONNECTED : the state is connected.
     *     @c  BTConnectState::DISCONNECTING : the state is disconnecting.
     *     @c  BTConnectState::DISCONNECTED : the state is disconnected.
     * @since 6
     */
    virtual void OnConnectionStateChanged(const BluetoothRemoteDevice &device, int state) {};

    /**
     * @brief Called to validate if a connection to the bluetooth device should be accepted.
     *
     * @param device  the connecting bluetooth device.
     * @since 6
     */
    virtual void OnPermission(const BluetoothRemoteDevice &device) {};
};

/**
 * @brief Map server API.
 *
 * @since 6
 */
class BLUETOOTH_API MapServer {
public:
    /**
     * @brief Get map server instance.
     *
     * @return Returns an instance of map server.
     * @since 6
     */
    static MapServer *GetProfile();

    /**
     * @brief Register the callback of map server.
     *
     * @param observer  Reference to the map server observer.
     * @since 6
     */
    void RegisterObserver(MapServerObserver &observer);

    /**
     * @brief Unregister the callback of map server.
     *
     * @param observer  Reference to the map server observer.
     * @since 6
     */
    void DeregisterObserver(MapServerObserver &observer);

    /**
     * @brief Get the connection status of the server.
     *
     * @return Returns connection state of map profile.
     * @since 6
     */
    int GetState() const;

    /**
     * @brief Disconnect bluetooth connection service.
     *
     * @param device  Reference to the remote bluetooth device.
     * @return Returns true if the operation is successful;returns false if the operation fails.
     * @since 6
     */
    bool Disconnect(const BluetoothRemoteDevice &device);

    /**
     * @brief Get whether bluetooth device is connected.
     *
     * @param device  Reference to the remote bluetooth device.
     * @return Returns true if the connection is successful;returns false if the connection fails.
     * @since 6
     */
    bool IsConnected(const BluetoothRemoteDevice &device);

    /**
     * @brief Get the list of connected bluetooth devices.
     *
     * @return Returns the bluetooth address list of the map profile.
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetConnectedDevices() const;

    /**
     * @brief Get the device list through the connection status.
     *
     * @param states  Reference to the connection status.
     *     @c BTConnectState::CONNECTING : the state is connecting.
     *     @c BTConnectState::CONNECTED : the state is connected.
     *     @c BTConnectState::DISCONNECTING : the state is disconnecting.
     *     @c BTConnectState::DISCONNECTED : the state is disconnected.
     * @return Returns the bluetooth address in the specified status.
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(std::vector<int> states) const;

    /**
     * @brief Get the connection status of the specified device.
     *
     * @param  device Reference to the remote bluetooth device.
     * @return Returns the connection status of the specified bluetooth address.
     * @since 6
     */
    int GetConnectionState(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Set the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @param strategy Reference to the connection policy,
     *     @c BTStrategyType::CONNECTION_UNKNOWN : the connection policy for unkown state.
     *     @c BTStrategyType::CONNECTION_ALLOWED : the connection policy for allowed state.
     *     @c BTStrategyType::CONNECTION_FORBIDDEN : the connection policy for forbidden state.
     * @return Returns true if the operation is successful;returns false if the operation fails.
     * @since 6
     */
    bool SetConnectionStrategy(const BluetoothRemoteDevice &device, int strategy);

    /**
     * @brief Get the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @return Returns the connection police of the specified bluetooth address.
     * @since 6
     */
    int GetConnectionStrategy(const BluetoothRemoteDevice &device) const;

    /**
     * @brief Set whether to authorize the connection.
     *
     * @param device Reference to the remote bluetooth device.
     * @param allow Reference to grant authorization.
     *     @c true : accept connection.
     *     @c false : reject connection.
     * @param save Save authorization.
     *     @c true : always accept.
     *     @c false : always reject.
     * @since 6
     */
    void GrantPermission(const BluetoothRemoteDevice &device, bool allow, bool save = false);

private:
    /**
     * @brief A constructor used to create a Map Server instance.
     *
     * @since 6
     */
    MapServer();

    /**
     * @brief A destructor used to delete the Map Server Observer instance.
     *
     * @since 6
     */
    ~MapServer();
    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(MapServer);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS

#endif  // BLUETOOTH_MAP_MSE_H