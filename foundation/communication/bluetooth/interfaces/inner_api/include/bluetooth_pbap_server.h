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
 * @brief The framework interface and callback function of pbap server are defined.
 *
 * @since 6
 */

/**
 * @file bluetooth_pbap_server.h
 *
 * @brief pbap server interface.
 *
 * @since 6
 */

#ifndef BLUETOOTH_PBAP_SERVER_H
#define BLUETOOTH_PBAP_SERVER_H

#include <memory>
#include <string>
#include <vector>
#include "bluetooth_remote_device.h"
#include "bluetooth_types.h"
#include "bluetooth_def.h"

namespace OHOS {
namespace Bluetooth {
/**
 * @brief obsever for pbap server
 * when pbap server occur event, call these
 */
class PbapObserver {
public:
    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     * @since 6
     */
    virtual ~PbapObserver() = default;
    /**
     * @brief  ConnectionState Changed
     *
     * @param  device     bluetooth address
     * @param  state      changed status
     * @since 6
     */
    virtual void OnServiceConnectionStateChanged(const BluetoothRemoteDevice &device, int state)
    {}
    /**
     * @brief connect permission event call back
     * @details when connect permission require call it
     * @param device remote device
     * @return void
     * @since 6
     */
    virtual void OnServicePermission(const BluetoothRemoteDevice &device)
    {}
    /**
     * @brief connect password input call back
     * @details when connect password input call  it
     * @param device remote device
     * @param description description bytes
     * @param charset description bytes's chartset
     * @param fullAccess fullAccess
     * @since 6
     */
    virtual void OnServicePasswordRequired(const BluetoothRemoteDevice &device,
        const std::vector<uint8_t> &description, uint8_t charset, bool fullAccess = true)
    {}
};

/**
 * @brief pbap server
 * pbap server
 */
class BLUETOOTH_API PbapServer {
public:
    /**
     * @brief get PbapServer
     * @details get PbapServer instance
     * @return PbapServer instance
     * @since 6
     */
    static PbapServer *GetProfile();

    /**
     * @brief regist observer
     * @details regist observer for the service of phone book server
     * @param observer the pointer that point to a PbapObserver
     * @return void
     * @since 6
     */
    void RegisterObserver(PbapObserver *observer);

    /**
     * @brief remove observer
     * @details remove observer for the service of phone book server
     * @param observer the pointer that point to a PbapObserver
     * @return void
     * @since 6
     */
    void DeregisterObserver(PbapObserver *observer);

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return std::vector remote devices
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetDevicesByStates(const std::vector<int> &states);

    /**
     * @brief get connected devices
     * @details get the remote device with the connected states
     * @param states states
     * @return std::vector remote devices
     * @since 6
     */
    std::vector<BluetoothRemoteDevice> GetConnectedDevices();

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return int @c not -1 state of the specified remote device
     *             @c -1 device is not exist
     * @since 6
     */
    int GetDeviceState(const BluetoothRemoteDevice &device);

    /**
     * @brief disconnect device
     * @details disconnect from remote device
     * @param device  remote device
     * @return Returns true if the operation is successful;returns false if the operation fails.
     * @since 6
     */
    bool Disconnect(const BluetoothRemoteDevice &device);
    /**
     * @brief Set the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @param strategy Reference to the connection policy,
     *     @c 0 : the connection policy for unkown state.
     *     @c 1 : the connection policy for allowed state.
     *     @c 2 : the connection policy for forbidden state.
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
     * @param save Always accept/reject connection from this device.
     * @since 6
     */
    void GrantPermission(const BluetoothRemoteDevice &device, bool allow, bool save = false);
    /**
     * @brief Set device's password. please call after OnServicePasswordRequired event.
     *
     * @param device device
     * @param password device's password
     * @param userId device's userId
     * @return int @c 0 ok
     *             @c -1 fail
     * @since 6
     */
    int SetDevicePassword(const BluetoothRemoteDevice &device, const std::string &password, std::string userId = "");

private:
    /**
     * @brief constructor
     * @details constructor
     */
    PbapServer();

    /**
     * @brief deconstructor
     * @details deconstructor
     */
    ~PbapServer();

    BLUETOOTH_DISALLOW_COPY_AND_ASSIGN(PbapServer);
    BLUETOOTH_DECLARE_IMPL();
};
}  // namespace Bluetooth
}  // namespace OHOS
#endif  // BLUETOOTH_PBAP_SERVER_H
