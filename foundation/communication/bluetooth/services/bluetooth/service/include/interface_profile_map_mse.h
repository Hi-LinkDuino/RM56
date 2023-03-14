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

#ifndef INTERFACE_PROFILE_MAP_MSE_H
#define INTERFACE_PROFILE_MAP_MSE_H

#include "interface_profile.h"
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Callback function api of Map service, including connection, disconnection and authorization.
 *
 * @since 6
 */
class IMapMseObserver {
public:
    /**
     * @brief A destructor used to delete the Map Service Observer instance.
     *
     * @since 6
     */
    virtual ~IMapMseObserver() = default;

    /**
     * @brief Callback function when the connection state changes.
     *
     * @param device  the remote bluetooth device.
     * @param state   the connection status.
     *     @c BTConnectState::CONNECTING : the state is connecting.
     *     @c BTConnectState::CONNECTED : the state is connected.
     *     @c BTConnectState::DISCONNECTING : the state is disconnecting.
     *     @c BTConnectState::DISCONNECTED : the state is disconnected.
     * @since 6
     */
    virtual void OnConnectionStateChanged(const RawAddress &remoteAddr, int state){};

    /**
     * @brief Called to validate if a connection to the bluetooth device should be accepted.
     *
     * @param remoteAddr  the connecting bluetooth device.
     * @since 6
     */
    virtual void OnPermission(const RawAddress &remoteAddr){};
};

/**
 * @brief Map service API.
 *
 * @since 6
 */
class IProfileMapMse : public IProfile {
public:
    /**
     * @brief A destructor used to delete the Map service Observer instance.
     *
     * @since 6
     */
    virtual ~IProfileMapMse() = default;

    /**
     * @brief Register the callback of map service.
     *
     * @param mapMseObserver  Reference to the map service observer.
     * @since 6
     */
    virtual void RegisterObserver(IMapMseObserver &mapMseObserver) = 0;

    /**
     * @brief Unregister the callback of map service.
     *
     * @param mapMseObserver  Reference to the map service observer.
     * @since 6
     */
    virtual void DeregisterObserver(IMapMseObserver &mapMseObserver) = 0;

    /**
     * @brief Get the connection status of the server.
     *
     * @return Returns connection state of map profile.
     * @since 6
     */
    virtual int GetState(void) = 0;

    /**
     * @brief Disconnect bluetooth connection service.
     *
     * @param device  Reference to the remote bluetooth device.
     * @return Returns true if the operation is successful;returns false if the operation fails.
     * @since 6
     */
    virtual int Disconnect(const RawAddress &device) = 0;

    /**
     * @brief Get whether bluetooth device is connected.
     *
     * @param device  Reference to the remote bluetooth device.
     * @return Returns true if the connection is successful;returns false if the connection fails.
     * @since 6
     */
    virtual bool IsConnected(const RawAddress &device) = 0;

    /**
     * @brief Get the device list through the connection status.
     *
     * @param  states  Reference to the connection status.
     *     @c  BTConnectState::CONNECTING : the state is connecting.
     *     @c  BTConnectState::CONNECTED : the state is connected.
     *     @c  BTConnectState::DISCONNECTING : the state is disconnecting.
     *     @c  BTConnectState::DISCONNECTED : the state is disconnected.
     * @return Returns the bluetooth address in the specified status.
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) = 0;

    /**
     * @brief Get the connection status of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @return Returns the connection status of the specified bluetooth address.
     * @since 6
     */
    virtual int GetConnectionState(const RawAddress &device) = 0;

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
    virtual bool SetConnectionStrategy(const RawAddress &device, int strategy) = 0;

    /**
     * @brief Get the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @return Returns the connection police of the specified bluetooth address.
     * @since 6
     */
    virtual int GetConnectionStrategy(const RawAddress &device) = 0;

    /**
     * @brief Set whether to authorize the connection.
     *
     * @param device Reference to the remote bluetooth device.
     * @param allow Reference to grant authorization.
     *     @c true : accept connection.
     *     @c false : reject connection.
     * @since 6
     */
    virtual void GrantPermission(const RawAddress &device, bool allow, bool save = false) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_MAP_MSE_H