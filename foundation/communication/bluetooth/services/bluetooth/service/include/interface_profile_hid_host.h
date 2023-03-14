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
 * @brief Defines hid host service interface, including observer and api functions.
 *
 */

/**
 * @file interface_profile_hid_host.h
 *
 * @brief hid host interface.
 *
 */

#ifndef INTERFACE_PROFILE_HID_HOST_H
#define INTERFACE_PROFILE_HID_HOST_H

#include "interface_profile.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief hid host observer for framework api
 *
 */
class IHidHostObserver {
public:
    /**
     * @brief Destroy the IHidHostObserver object
     *
     */
    virtual ~IHidHostObserver() = default;
    /**
     * @brief  ConnectionState Changed
     *
     * @param  deviceAddress     bluetooth address
     * @param  state            changed status
     */
    virtual void OnConnectionStateChanged(const RawAddress &deviceAddress, int state) {};
};

class IProfileHidHost : public IProfile {
public:
    /**
     * @brief  register observer
     *
     * @param  observer         function pointer
     */
    virtual void RegisterObserver(IHidHostObserver &observer) = 0;
    /**
     * @brief  deregister observer
     *
     * @param  observer         function pointer
     */
    virtual void DeregisterObserver(IHidHostObserver &observer) = 0;
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

    /**
     * @brief Get Hid Host VCUnplug.
     *
     */
    virtual int HidHostVCUnplug(std::string device, uint8_t id, uint16_t size, uint8_t type) = 0;

    /**
     * @brief Hid Host Send Data.
     *
     */
    virtual int HidHostSendData(std::string device, uint8_t id, uint16_t size, uint8_t type) = 0;

    /**
     * @brief Hid Host Set Report.
     *
     */
    virtual int HidHostSetReport(std::string device, uint8_t type, uint16_t size, const uint8_t* report) = 0;

    /**
     * @brief Hid Host Get Report.
     *
     */
    virtual int HidHostGetReport(std::string device, uint8_t id, uint16_t size, const uint8_t type) = 0;
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // INTERFACE_PROFILE_HID_HOST_H