/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_P2P_DEVICE_MANAGER_H
#define OHOS_P2P_DEVICE_MANAGER_H

#include <mutex>
#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
class WifiP2pDeviceManager {
    friend class P2pStateMachine;
    friend class AuthorizingNegotiationRequestState;
    friend class GroupFormedState;
    friend class GroupNegotiationState;
    friend class InvitationReceivedState;
    friend class InvitationRequestState;
    friend class P2pDefaultState;
    friend class P2pDisabledState;
    friend class P2pDisablingState;
    friend class P2pEnabledState;
    friend class P2pEnablingState;
    friend class P2pGroupFormationState;
    friend class P2pGroupJoinState;
    friend class P2pGroupOperatingState;
    friend class P2pIdleState;
    friend class P2pInvitingState;
    friend class ProvisionDiscoveryState;
    friend class WifiP2pService;
public:
    /**
     * @Description Destroy the Wifi P2p Device Manager object.
     */
    virtual ~WifiP2pDeviceManager() = default;
    /**
     * @Description - Initialize the device manager.
     */
    virtual void Initialize();
    /**
     * @Description - Add a P2P device.
     * @param  device - P2P device to be added
     * @return - bool true:success   false:failed
     */
    virtual bool AddDevice(const WifiP2pDevice &device);
    /**
     * @Description - Remove a P2P device.
     * @param  deviceAddress - MAC of the P2P device to be removed
     * @return - bool true:success    false:failed
     */
    virtual bool RemoveDevice(const std::string &deviceAddress);
    /**
     * @Description - Remove a P2P device.
     * @param  device - P2P device to be removed
     * @return - bool true:success    false:failed
     */
    virtual bool RemoveDevice(const WifiP2pDevice &device);
    /**
     * @Description - Clear all P2P devices.
     * @return - int number of cleared P2P devices
     */
    virtual int ClearAll();
    /**
     * @Description - Obtain all P2P devices.
     * @param  devices - object for storing P2P devices
     * @return - int the number of obtained
     */
    virtual int GetDevicesList(std::vector<WifiP2pDevice> &devices);
    /**
     * @Description - Updates the supplicant information of a P2P device based on the device address.
     * @param  devices - device that need to be updated
     * @return - bool true:success    false:failed
     */
    virtual bool UpdateDeviceSupplicantInf(const WifiP2pDevice &device);
    /**
     * @Description - Updates all information about the P2P device based on the device address.
                      If the device does not exist, add the device.
     * @param  devices - device that need to be updated
     * @return - bool true:success    false:failed
     */
    virtual bool UpdateDevice(const WifiP2pDevice &device);
    /**
     * @Description - Update the group capability of a device.
     * @param  deviceAddress - device address to be updated
     * @param  cap - group capability
     * @return - bool
     */
    virtual bool UpdateDeviceGroupCap(const std::string &deviceAddress, uint32_t cap);
    /**
     * @Description - Update the group capability of a device.
     * @param  device - device that need to be updated
     * @return - bool true:success    false:failed
     */
    virtual bool UpdateDeviceGroupCap(const WifiP2pDevice &device);
    /**
     * @Description - Update the status of a P2P device based on the device address.
     * @param  deviceAddress - device address to be updated
     * @param  status - status that needs to be updated
     * @return - bool true:success    false:failed
     */
    virtual bool UpdateDeviceStatus(const std::string &deviceAddress, P2pDeviceStatus status);
    /**
     * @Description - Updates the status of a P2P device based on the device information.
     * @param  devices - device that need to be updated
     * @return - bool true:success    false:failed
     */
    virtual bool UpdateDeviceStatus(const WifiP2pDevice &device);
    /**
     * @Description - Obtain information about a P2P device based on the device address.
     * @param  devices - MAC of the device
     * @return - WifiP2pDevice device information that obtained
     */
    virtual WifiP2pDevice GetDevices(const std::string &deviceAddress);
    /**
     * @Description Obtain the device name based on the device address.
     * @param  deviceAddress - address of the device
     * @return - const std::string device name
     */
    virtual const std::string GetDeviceName(const std::string &deviceAddress);

private:
    /**
     * @Description Get own device information.
     * @return - WifiP2pDevice a reference to the own device
     */
    inline WifiP2pDevice &GetThisDevice()
    {
        return thisDevice;
    }

private:
    WifiP2pDevice thisDevice;            /* own device */
    std::vector<WifiP2pDevice> p2pDevices;  /* all scanned devices */
    std::mutex deviceMutex;
};
}  // namespace Wifi
}  // namespace OHOS

#endif  // OHOS_P2P_DEVICE_MANAGER_H
