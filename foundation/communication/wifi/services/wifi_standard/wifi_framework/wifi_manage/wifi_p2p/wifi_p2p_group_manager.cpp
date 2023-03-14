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
#include "wifi_p2p_group_manager.h"
#include <algorithm>
#include "wifi_settings.h"
#include "wifi_logger.h"

    DEFINE_WIFILOG_P2P_LABEL("WifiP2pGroupManager");

namespace OHOS {
namespace Wifi {

WifiP2pGroupManager::WifiP2pGroupManager() : groupsInfo(), currentGroup(), groupMutex(), p2pConnInfo()
{}

void WifiP2pGroupManager::Initialize()
{
    std::unique_lock<std::mutex> lock(groupMutex);
    WifiSettings::GetInstance().GetWifiP2pGroupInfo(groupsInfo);
    WifiSettings::GetInstance().GetP2pInfo(p2pConnInfo);
}

void WifiP2pGroupManager::StashGroups()
{
    std::unique_lock<std::mutex> lock(groupMutex);
    RefreshGroupsFromCurrentGroup();
    WifiSettings::GetInstance().SetWifiP2pGroupInfo(groupsInfo);
}

bool WifiP2pGroupManager::AddGroup(const WifiP2pGroupInfo &group)
{
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        if (*it == group) {
            return true;
        }
    }
    groupsInfo.push_back(group);
    return true;
}
bool WifiP2pGroupManager::RemoveGroup(const WifiP2pGroupInfo &group)
{
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        if (*it == group) {
            groupsInfo.erase(it);
            return true;
        }
    }
    return false;
}
int WifiP2pGroupManager::ClearAll()
{
    std::unique_lock<std::mutex> lock(groupMutex);
    int groupSize = groupsInfo.size();
    groupsInfo.clear();
    return groupSize;
}

void WifiP2pGroupManager::UpdateWpaGroup(const WifiP2pGroupInfo &group)
{
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        if (*it == group) {
            WifiP2pDevice owner = it->GetOwner();
            owner.SetDeviceAddress(group.GetOwner().GetDeviceAddress());
            it->SetOwner(owner);
            it->SetGroupName(group.GetGroupName());
            return;
        }
    }

    groupsInfo.push_back(group);
}

int WifiP2pGroupManager::RemoveClientFromGroup(int networkId, const std::string &deviceAddress)
{
    bool isRemoveSucceed = false;
    WifiP2pDevice device;
    device.SetDeviceAddress(deviceAddress);

    std::unique_lock<std::mutex> lock(groupMutex);
    auto it = groupsInfo.begin();
    for (; it != groupsInfo.end(); ++it) {
        if (networkId == it->GetNetworkId()) {
            if (it->IsContainsDevice(device)) {
                it->RemoveClientDevice(device);
                isRemoveSucceed = true;
            }
            break;
        }
    }

    if (it != groupsInfo.end()) {
        const std::vector<WifiP2pDevice> &clients = it->GetClientDevices();
        return clients.size();
    } else {
        return -1;
    }
}

const std::vector<WifiP2pGroupInfo> &WifiP2pGroupManager::GetGroups()
{
    std::unique_lock<std::mutex> lock(groupMutex);
    RefreshGroupsFromCurrentGroup();
    return groupsInfo;
}

int WifiP2pGroupManager::GetNetworkIdFromClients(const WifiP2pDevice &device)
{
    std::string deviceAddr = device.GetDeviceAddress();
    std::transform(deviceAddr.begin(), deviceAddr.end(), deviceAddr.begin(), ::tolower);

    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        const std::vector<WifiP2pDevice> &clients = it->GetClientDevices();
        for (auto client = clients.begin(); client != clients.end(); client++) {
            std::string clientAddr = client->GetDeviceAddress();
            std::transform(clientAddr.begin(), clientAddr.end(), clientAddr.begin(), ::tolower);
            if (clientAddr == deviceAddr) {
                return it->GetNetworkId();
            }
        }
    }

    return -1;
}

int WifiP2pGroupManager::GetGroupNetworkId(const WifiP2pDevice &device)
{
    std::string deviceMac = device.GetDeviceAddress();
    std::transform(deviceMac.begin(), deviceMac.end(), deviceMac.begin(), ::tolower);
    std::string ownerMac;
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        ownerMac = it->GetOwner().GetDeviceAddress();
        std::transform(ownerMac.begin(), ownerMac.end(), ownerMac.begin(), ::tolower);
        if (deviceMac == ownerMac) {
            return it->GetNetworkId();
        }
    }
    return -1;
}
int WifiP2pGroupManager::GetGroupNetworkId(const WifiP2pDevice &device, const std::string &ssid)
{
    std::string deviceMac = device.GetDeviceAddress();
    std::transform(deviceMac.begin(), deviceMac.end(), deviceMac.begin(), ::tolower);
    std::string ownerMac;
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        ownerMac = it->GetOwner().GetDeviceAddress();
        std::transform(ownerMac.begin(), ownerMac.end(), ownerMac.begin(), ::tolower);
        if (deviceMac == ownerMac && ssid == it->GetGroupName()) {
            return it->GetNetworkId();
        }
    }
    return -1;
}
std::string WifiP2pGroupManager::GetGroupOwnerAddr(int netId)
{
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        if (netId == it->GetNetworkId()) {
            return it->GetOwner().GetDeviceAddress();
        }
    }
    return "";
}
bool WifiP2pGroupManager::IsInclude(int netId)
{
    std::unique_lock<std::mutex> lock(groupMutex);
    for (auto it = groupsInfo.begin(); it != groupsInfo.end(); ++it) {
        if (netId == it->GetNetworkId()) {
            return true;
        }
    }
    return false;
}

void WifiP2pGroupManager::RefreshGroupsFromCurrentGroup()
{
    for (auto &group : groupsInfo) {
        if (group == currentGroup) {
            group = currentGroup;
            break;
        }
    }
}

void WifiP2pGroupManager::RefreshCurrentGroupFromGroups()
{
    for (auto &group : groupsInfo) {
        if (group == currentGroup) {
            currentGroup.SetClientDevices(group.GetClientDevices());
            break;
        }
    }
}

void WifiP2pGroupManager::SaveP2pInfo(const WifiP2pLinkedInfo &linkedInfo)
{
    p2pConnInfo = linkedInfo;
}

const WifiP2pLinkedInfo &WifiP2pGroupManager::GetP2pInfo() const
{
    return p2pConnInfo;
}

void WifiP2pGroupManager::UpdateGroupsNetwork(std::map<int, WifiP2pGroupInfo> wpaGroups)
{
    std::unique_lock<std::mutex> lock(groupMutex);
    bool found = false;
    auto group = groupsInfo.begin();
    while (group != groupsInfo.end()) {
        found = false;
        for (auto wpaGroup = wpaGroups.begin(); wpaGroup != wpaGroups.end(); ++wpaGroup) {
            if (group->GetGroupName() == wpaGroup->second.GetGroupName() &&
                group->GetOwner().GetDeviceAddress() == wpaGroup->second.GetOwner().GetDeviceAddress()) {
                group->SetNetworkId(wpaGroup->second.GetNetworkId());
                found = true;
                break;
            }
        }
        /**
         * If the corresponding group cannot be found, the group has been deleted by the WPA.
         */
        if (!found) {
            WIFI_LOGI("The group has been deleted by the WPA.");
            group = groupsInfo.erase(group);
        } else {
            group++;
        }
    }
}
}  // namespace Wifi
}  // namespace OHOS
