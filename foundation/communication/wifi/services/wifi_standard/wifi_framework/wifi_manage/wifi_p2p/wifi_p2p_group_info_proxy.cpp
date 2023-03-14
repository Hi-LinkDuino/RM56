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
#include "wifi_p2p_group_info_proxy.h"

namespace OHOS {
namespace Wifi {
bool WifiP2pGroupInfoProxy::operator==(const WifiP2pGroupInfo &group)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return group == groupsInfo;
}

bool WifiP2pGroupInfoProxy::operator!=(const WifiP2pGroupInfo &group)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return group != groupsInfo;
}

void WifiP2pGroupInfoProxy::SetIsGroupOwner(bool isGroupOwner)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetIsGroupOwner(isGroupOwner);
}

bool WifiP2pGroupInfoProxy::IsGroupOwner() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.IsGroupOwner();
}

void WifiP2pGroupInfoProxy::SetOwner(const WifiP2pDevice &owner)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetOwner(owner);
}

WifiP2pDevice WifiP2pGroupInfoProxy::GetOwner() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetOwner();
}

void WifiP2pGroupInfoProxy::SetPassphrase(const std::string &passphrase)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetPassphrase(passphrase);
}

std::string WifiP2pGroupInfoProxy::GetPassphrase() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetPassphrase();
}

void WifiP2pGroupInfoProxy::SetInterface(const std::string &interface)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetInterface(interface);
}

std::string WifiP2pGroupInfoProxy::GetInterface() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetInterface();
}

void WifiP2pGroupInfoProxy::SetGroupName(const std::string &newGroupName)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetGroupName(newGroupName);
}

std::string WifiP2pGroupInfoProxy::GetGroupName() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetGroupName();
}

void WifiP2pGroupInfoProxy::SetFrequency(int frequency)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetFrequency(frequency);
}

int WifiP2pGroupInfoProxy::GetFrequency() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetFrequency();
}

void WifiP2pGroupInfoProxy::SetIsPersistent(bool isPersistent)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetIsPersistent(isPersistent);
}

bool WifiP2pGroupInfoProxy::IsPersistent() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.IsPersistent();
}

void WifiP2pGroupInfoProxy::SetP2pGroupStatus(P2pGroupStatus newGroupStatus)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetP2pGroupStatus(newGroupStatus);
}

P2pGroupStatus WifiP2pGroupInfoProxy::GetP2pGroupStatus() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetP2pGroupStatus();
}

void WifiP2pGroupInfoProxy::SetNetworkId(int nwId)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetNetworkId(nwId);
}

int WifiP2pGroupInfoProxy::GetNetworkId() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetNetworkId();
}

void WifiP2pGroupInfoProxy::SetGoIpAddress(const std::string &ipAddr)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetGoIpAddress(ipAddr);
}
const std::string &WifiP2pGroupInfoProxy::GetGoIpAddress() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetGoIpAddress();
}

void WifiP2pGroupInfoProxy::AddClientDevice(const WifiP2pDevice &clientDevice)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.AddClientDevice(clientDevice);
}

void WifiP2pGroupInfoProxy::RemoveClientDevice(const WifiP2pDevice &clientDevice)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.RemoveClientDevice(clientDevice);
}

bool WifiP2pGroupInfoProxy::IsContainsDevice(const WifiP2pDevice &clientDevice) const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.IsContainsDevice(clientDevice);
}

bool WifiP2pGroupInfoProxy::IsClientDevicesEmpty() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.IsClientDevicesEmpty();
}

std::vector<WifiP2pDevice> WifiP2pGroupInfoProxy::GetClientDevices() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo.GetClientDevices();
}

void WifiP2pGroupInfoProxy::SetClientDevices(const std::vector<WifiP2pDevice> &devices)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.SetClientDevices(devices);
}

void WifiP2pGroupInfoProxy::ClearClientDevices()
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo.ClearClientDevices();
}

WifiP2pGroupInfoProxy &WifiP2pGroupInfoProxy::operator=(const WifiP2pGroupInfo &group)
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    groupsInfo = group;
    return *this;
}

WifiP2pGroupInfoProxy::operator WifiP2pGroupInfo() const
{
    std::unique_lock<std::mutex> lock(proxyMutex);
    return groupsInfo;
}
}  // namespace Wifi
}  // namespace OHOS