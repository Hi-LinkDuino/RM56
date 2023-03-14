/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
 * @brief Defines pan service interface, including observer and api functions.
 *
 */

/**
 * @file interface_profile_pan.h
 *
 * @brief pan interface.
 *
 */

#ifndef INTERFACE_PROFILE_PAN_H
#define INTERFACE_PROFILE_PAN_H

#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief pan observer for framework api
 *
 */
class IPanObserver {
public:
    /**
     * @brief Destroy the IPanObserver object
     *
     */
    virtual ~IPanObserver() = default;
    /**
     * @brief  ConnectionState Changed
     *
     * @param  deviceAddress     bluetooth address
     * @param  state            changed status
     */
    virtual void OnConnectionStateChanged(const RawAddress &deviceAddress, int state) {}
};

class IProfilePan : public IProfile {
public:
    /**
     * @brief  register observer
     *
     * @param  observer         function pointer
     */
    virtual void RegisterObserver(IPanObserver &observer) = 0;
    /**
     * @brief  deregister observer
     *
     * @param  observer         function pointer
     */
    virtual void DeregisterObserver(IPanObserver &observer) = 0;
    /**
     * @brief Get remote AG device list which are in the specified states.
     *
     * @param states List of remote device states.
     * @return Returns the list of devices.
     */
    virtual std::vector<RawAddress> GetDevicesByStates(std::vector<int> states) = 0;

    /**
     * @brief Get the connection state of the specified remote device.
     *
     * @param device Remote device object.
     * @return Returns the connection state of the remote device.
     */
    virtual int GetDeviceState(const RawAddress &device) = 0;

    virtual bool SetTethering(const bool enable) = 0;
    virtual bool IsTetheringOn() = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_PAN_H