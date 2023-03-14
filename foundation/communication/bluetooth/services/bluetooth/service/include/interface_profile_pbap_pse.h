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

#ifndef INTERFACE_PROFILE_PBAP_PSE_H
#define INTERFACE_PROFILE_PBAP_PSE_H

#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief obsever for pbap server
 * when pbap server occur event, call these
 */
class IPbapPseObserver {
public:
    /**
     * @brief  ConnectionState Changed
     *
     * @param  device     bluetooth address
     * @param  state      changed status
     * @since 6
     */
    virtual void OnServiceConnectionStateChanged(const RawAddress &device, int state) = 0;

    /**
     * @brief connect permission confirm call back
     * @details when connect incoming, device is unknown call it
     * @param device remote device
     * @return void
     * @since 6
     */
    virtual void OnServicePermission(const RawAddress &remoteAddr) = 0;

    /**
     * @brief connect password input call back
     * @details when connect password input call  it
     * @param device remote device
     * @param description description bytes
     * @param charset description bytes's chartset
     * @param fullAccess fullAccess
     * @since 6
     */
    virtual void OnServicePasswordRequired(const RawAddress &remoteAddr, const std::vector<uint8_t> &description,
        uint8_t charset, bool fullAccess = true) = 0;

    /**
     * @brief deconstructor
     * @details deconstructor
     * @return
     * @since 6
     */
    virtual ~IPbapPseObserver(){};
};

/**
 * @brief pbap pce service
 * pbap pce service
 */
class IProfilePbapPse : public IProfile {
public:
    /**
     * @brief register observer
     * @details register observer for the service of phone book serve
     * @param observer the ref that point to a PbapPseObserver
     * @return void
     * @since 6
     */
    virtual void RegisterObserver(IPbapPseObserver &observer) = 0;

    /**
     * @brief deregister observer
     * @details deregister observer for the service of phone book serve
     * @param observer the ref that point to a PbapPseObserver
     * @return void
     * @since 6
     */
    virtual void DeregisterObserver(IPbapPseObserver &observer) = 0;

    /**
     * @brief get the remote devices
     * @details get the remote device with the specified states
     * @param states states
     * @return std::vector remote devices
     * @since 6
     */
    virtual std::vector<RawAddress> GetDevicesByStates(const std::vector<int> &states) const = 0;

    /**
     * @brief get the state of device
     * @details get the state with the specified remote device
     * @param device  remote device
     * @return int @c not -1 state of the specified remote device
     *             @c -1 device is not exist
     * @since 6
     */
    virtual int GetDeviceState(const RawAddress &device) const = 0;

    /**
     * @brief Set the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @param strategy Reference to the connection policy,
     *     @c UNKNOWN : the connection policy for unkown state.
     *     @c ALLOWED : the connection policy for allowed state.
     *     @c FORBIDDEN : the connection policy for forbidden state.
     * @return Returns true if the operation is successful;returns false if the operation fails.
     * @since 6
     */
    virtual int SetConnectionStrategy(const RawAddress &device, int strategy) = 0;

    /**
     * @brief Get the connection policy of the specified device.
     *
     * @param device Reference to the remote bluetooth device.
     * @return Returns the connection police of the specified bluetooth address.
     * @since 6
     */
    virtual int GetConnectionStrategy(const RawAddress &device) const = 0;

    /**
     * @brief Grant device's connect Permission. please call after OnServicePermission event.
     *
     * @param device device
     * @param allow allow
     * @param save save
     * @since 6
     */
    virtual void GrantPermission(const RawAddress &device, bool allow, bool save = false) = 0;

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
    virtual int SetDevicePassword(
        const RawAddress &device, const std::string &password, const std::string userId = "") = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_PBAP_PSE_H