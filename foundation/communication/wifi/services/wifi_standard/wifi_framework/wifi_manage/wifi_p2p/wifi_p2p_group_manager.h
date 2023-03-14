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

#ifndef OHOS_P2P_GROUP_MANAGER_H
#define OHOS_P2P_GROUP_MANAGER_H

#include <vector>
#include <mutex>
#include "wifi_msg.h"
#include "wifi_p2p_msg.h"
#include "p2p_macro.h"
#include "wifi_p2p_group_info_proxy.h"

namespace OHOS {
namespace Wifi {
const int CREDENTIAL_MAX_NUM = 32;
class WifiP2pGroupManager {
FRIEND_GTEST(WifiP2pGroupManager);
public:
    /**
     * @Description  Construct a new WifiP2pGroupManager object.
     * @param None
     * @return None
     */
    WifiP2pGroupManager();

    /**
     * @Description  Destroy the WifiP2pGroupManager object.
     * @param None
     * @return None
     */
    virtual ~WifiP2pGroupManager() = default;

    /**
     * @Description - Reads the stored group information during initialization.
     * @param None
     * @return None
     */
    virtual void Initialize();

    /**
     * @Description - Stores and serializes the current group information.
     */
    virtual void StashGroups();

    /**
     * @Description - Adding a P2P group.
     * @param group - P2P group to be added
     * @return true: adding succeeded    false: adding failed
     */
    virtual bool AddGroup(const WifiP2pGroupInfo &group);

    /**
     * @Description - Remove a P2P group.
     * @param group - P2P group to be removed
     * @return true: delete successfully    false: delete failed
     */
    virtual bool RemoveGroup(const WifiP2pGroupInfo &group);

    /**
     * @Description - Clear all P2P groups.
     * @param None
     * @return int number of P2P groups to be deleted
     */
    virtual int ClearAll();

    /**
     * @Description - Update the group information (except the current temporary group)
                      in the latest WPA to the manager.
     * @param  group - group recorded in WPA
     */
    virtual void UpdateWpaGroup(const WifiP2pGroupInfo &group);

    /**
     * @Description - Delete a client from a specified group.
     * @param  networkId - specified group ID
     * @param  deviceAddress - client address to be deleted
     * @return - int number of clients in the group
     */
    virtual int RemoveClientFromGroup(int networkId, const std::string &deviceAddress);

    /**
     * @Description - Obtaining all groups.
     * @param None
     * @return const std::vector<WifiP2pGroupInfo> &
     */
    virtual const std::vector<WifiP2pGroupInfo> &GetGroups();

    /**
     * @Description Query the first GC of the group to which the device belongs and returns the network ID of the group.
     * @param  device - device corresponding to the GC of the group
     * @return - int -1:not found    >=0：network ID of the corresponding group
     */
    virtual int GetNetworkIdFromClients(const WifiP2pDevice &device);

    /**
     * @Description  Get the network id of the group owner profile with the specified p2p device address.
     * @param device - P2P devices to be matched
     * @return int -1: not found    >=0: network ID of the P2P device
     */
    virtual int GetGroupNetworkId(const WifiP2pDevice &device);

    /**
     * @Description  Get the network id of the group owner profile with the
                     specified p2p device address and the ssid.
     * @param device - P2P devices to be matched
     * @param ssid - specified network name
     * @return int -1: not found    >=0: network ID of the P2P device
     */
    virtual int GetGroupNetworkId(const WifiP2pDevice &device, const std::string &ssid);

    /**
     * @Description  Get the group owner mac address with the specified networkId.
     * @param netId - specifies the network ID of the group
     * @return std::string not null: MAC address of the group owner    null: not found
     */
    virtual std::string GetGroupOwnerAddr(int netId);

    /**
     * @Description  Specifies whether the group with networkId is included in the group list.
     * @param netId - specifies the network ID of a group
     * @return true:contains    false:not contains
     */
    virtual bool IsInclude(int netId);

    /**
     * @Description Store P2P group connection information.
     *
     * @param linkedInfo - group connection information
     */
    virtual void SaveP2pInfo(const WifiP2pLinkedInfo &linkedInfo);

    /**
     * @Description Get P2P group connection information.
     *
     * @return const WifiP2pLinkedInfo& group connection information
     */
    virtual const WifiP2pLinkedInfo &GetP2pInfo() const;

    /**
     * @Description Obtain the group information from the WPA, match and update the network ID.
     *
     * @param wpaGroups - groups information from WPA
     */
    virtual void UpdateGroupsNetwork(std::map<int, WifiP2pGroupInfo> wpaGroups);

public:
    /**
     * @Description - Update the current group client.
     * @param  device - clients to be updated
     */
    inline void UpdateCurrGroupClient(const WifiP2pDevice &device)
    {
        currentGroup.AddClientDevice(device);
    }

    inline void RemoveCurrGroupClient(const WifiP2pDevice &device)
    {
        currentGroup.RemoveClientDevice(device);
    }

    inline void SetCurrentGroup(const WifiP2pGroupInfo &group)
    {
        currentGroup = group;
        RefreshCurrentGroupFromGroups();
    }

    inline const WifiP2pGroupInfoProxy &GetCurrentGroup() const
    {
        return currentGroup;
    }

private:
    /**
     * @Description - Synchronize from the current group to all registry groups.
     */
    void RefreshGroupsFromCurrentGroup();

    /**
     * @Description - From all record groups, update to the current group in use.
     */
    void RefreshCurrentGroupFromGroups();

private:
    std::vector<WifiP2pGroupInfo> groupsInfo;
    WifiP2pGroupInfoProxy currentGroup;
    std::mutex groupMutex;
    WifiP2pLinkedInfo p2pConnInfo;      /* group connection information */
};
}  // namespace Wifi
}  // namespace OHOS

#endif  // OHOS_P2P_GROUP_MANAGER_H
