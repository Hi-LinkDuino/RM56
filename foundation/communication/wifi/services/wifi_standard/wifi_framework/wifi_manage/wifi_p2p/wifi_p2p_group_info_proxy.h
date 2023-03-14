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
#ifndef OHOS_P2P_WIFI_P2P_GROUP_INFO_PROXY_H
#define OHOS_P2P_WIFI_P2P_GROUP_INFO_PROXY_H

#include <mutex>
#include "wifi_p2p_msg.h"

namespace OHOS {
namespace Wifi {
/**
 * This class solves the problem caused by frequent operations on the WifiP2pGroupInfo object in the following
   scenarios:
 *
 *  1. Frequent SET/GET-related WifiP2pGroupInfo attributes exist.
 *  2. Multi-thread access, which has thread safety issues.
 *  3. Due to the existence of std::vector<WifiP2pDevice>, if you copy a modification and then set it back each time,
       the performance will be lost. (In most cases, only general types are modified)
 *
 *  Realization principle:
 *  1. Support for thread safety.
 *  2. For common attribute modification, only the attribute itself is modified to avoid excessive copying.
 *  3. The operation (interface) is consistent with that of the agent object.
 */
class WifiP2pGroupInfoProxy {
public:
    WifiP2pGroupInfoProxy() = default;

    bool operator==(const WifiP2pGroupInfo &group);
    bool operator!=(const WifiP2pGroupInfo &group);

    void SetIsGroupOwner(bool isGroupOwner);
    bool IsGroupOwner() const;

    void SetOwner(const WifiP2pDevice &owner);
    WifiP2pDevice GetOwner() const;

    void SetPassphrase(const std::string &passphrase);
    std::string GetPassphrase() const;

    void SetInterface(const std::string &interface);
    std::string GetInterface() const;

    void SetGroupName(const std::string &newGroupName);
    std::string GetGroupName() const;

    void SetFrequency(int frequency);
    int GetFrequency() const;

    void SetIsPersistent(bool isPersistent);
    bool IsPersistent() const;

    void SetP2pGroupStatus(P2pGroupStatus newGroupStatus);
    P2pGroupStatus GetP2pGroupStatus() const;

    void SetNetworkId(int nwId);
    int GetNetworkId() const;

    void SetGoIpAddress(const std::string &ipAddr);
    const std::string &GetGoIpAddress() const;

    void AddClientDevice(const WifiP2pDevice &clientDevice);
    void RemoveClientDevice(const WifiP2pDevice &clientDevice);

    bool IsContainsDevice(const WifiP2pDevice &clientDevice) const;
    bool IsClientDevicesEmpty() const;

    std::vector<WifiP2pDevice> GetClientDevices() const;
    void SetClientDevices(const std::vector<WifiP2pDevice> &devices);
    void ClearClientDevices();

public:
    ~WifiP2pGroupInfoProxy() = default;
    WifiP2pGroupInfoProxy(const WifiP2pGroupInfoProxy &) = delete;
    WifiP2pGroupInfoProxy &operator=(const WifiP2pGroupInfoProxy &) = delete;
    /**
     * @Description - Set the code object so that the proxy interface is consistent with the real object interface.
     * @return - WifiP2pGroupInfoProxy& return the modified proxy itself
     */
    WifiP2pGroupInfoProxy &operator=(const WifiP2pGroupInfo &);
    /**
     * @Description - Conversion function, which is used to ensure that the proxy interface is consistent with the real
                      object interface.
     * @return - WifiP2pGroupInfo converted object
     */
    operator WifiP2pGroupInfo() const;

private:
    WifiP2pGroupInfo groupsInfo;         /* proxy group object */
    mutable std::mutex proxyMutex;  /* mutex */
};
}  // namespace Wifi
}  // namespace OHOS

#endif  // OHOS_P2P_WIFI_P2P_GROUP_INFO_PROXY_H