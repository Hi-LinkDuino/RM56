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
#include "dhcpd_interface.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#include "network_interface.h"
#include "wifi_global_func.h"
#include "dhcpd_interface.h"
#include "wifi_logger.h"
#include "dhcp_service.h"

DEFINE_WIFILOG_DHCP_LABEL("WifiDhcpdInterface");

namespace OHOS {
namespace Wifi {
const int EU_I64_ADDR_LEN = 64;
const int GENE_V6_ADDR_LEN = 64; /* Generally, the prefix length cannot exceed 64 characters. */
const int IP_V6_ADDR_LEN = 128;
const int MAC_ADDR_MAX_LEN = 17;
const int DHCP_LEASE_FORMAT_SIZE = 5;
const int DHCP_LEASE_MAC_ADDR_POS = 1;
const int DHCP_LEASE_IP_ADDR_POS = 2;
const int DHCP_LEASE_HOSTNAME_POS = 3;

DhcpdInterface::DhcpdInterface()
    : mBindIpv4(Ipv4Address::INVALID_INET_ADDRESS), mBindIpv6(Ipv6Address::INVALID_INET6_ADDRESS)
{}

DhcpdInterface::~DhcpdInterface()
{
    if (mDhcpService != nullptr) {
        mDhcpService.reset(nullptr);
    }
}

bool DhcpdInterface::SetDhcpEventFunc(const std::string &ifaceName, IDhcpResultNotify* pResultNotify)
{
    if (ifaceName.empty() || pResultNotify == nullptr || mDhcpService == nullptr) {
        return false;
    }
    return (mDhcpService->GetDhcpSerProExit(ifaceName, pResultNotify) == DHCP_OPT_SUCCESS);
}

bool DhcpdInterface::StartDhcpServer(const std::string &ifaceName, Ipv4Address &ipv4, Ipv6Address &ipv6, bool isIpV4)
{
    if (mDhcpService == nullptr) {
        mDhcpService = std::make_unique<DhcpService>();
        if (mDhcpService == nullptr) {
            return false;
        }
    }

    std::vector<Ipv4Address> vecIpv4Addr;
    std::vector<Ipv6Address> vecIpv6Addr;
    if (!NetworkInterface::FetchIpAddress(std::string(""), vecIpv4Addr, vecIpv6Addr)) {
        WIFI_LOGW("Get ipaddress failed!");
    }

    // set ap interface to bind an ip address
    if (!AssignIpAddr(mBindIpv4, mBindIpv6, vecIpv4Addr, vecIpv6Addr, isIpV4)) {
        WIFI_LOGE("Assign ip address for interface failed!");
        return false;
    }
    ipv4 = mBindIpv4;
    ipv6 = mBindIpv6;
    if (!ApplyIpAddress(ifaceName, mBindIpv4, mBindIpv6)) {
        WIFI_LOGE("bind interface address failed!");
        return false;
    }

    if (!SetDhcpIpRange(ifaceName)) {
        WIFI_LOGE("Set dhcp range ip address failed!");
        return false;
    }
    if (mDhcpService->StartDhcpServer(ifaceName) != 0) {
        WIFI_LOGE("Start dhcp server failed!");
        return false;
    }

    return true;
}

bool DhcpdInterface::SetDhcpIpRange(const std::string &ifaceName)
{
    if (mDhcpService == nullptr) {
        WIFI_LOGE("SetDhcpIpRange mDhcpService is nullptr!");
        return false;
    }
    if (!mBindIpv4.IsValid()) { /* currently, we just support ipv4 */
        WIFI_LOGE("current interface does not bind ipv4!");
        return false;
    }
    std::string ipAddr = mBindIpv4.GetAddressWithString();
    std::string::size_type pos = ipAddr.rfind(".");
    if (pos == std::string::npos) {
        WIFI_LOGE("invalid ip address[%{private}s]!", ipAddr.c_str());
        return false;
    }
    std::string ipHead = ipAddr.substr(0, pos);
    DhcpRange range;
    range.iptype = IP_V4;
    range.strStartip = ipHead + ".3";
    range.strEndip = ipHead + ".254";
    range.strSubnet = "255.255.255.0";
    std::string tagName = ifaceName;
    transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
    if (tagName.find("p2p") != std::string::npos) {
        range.strTagName = "p2p";
        if (mDhcpService->PutDhcpRange(range.strTagName, range) != 0) {
            WIFI_LOGE("Put dhcp range failed! TagName[%{public}s].", range.strTagName.c_str());
            return false;
        }
        if (mDhcpService->SetDhcpRange(ifaceName, range.strTagName) != 0) {
            WIFI_LOGE("Set dhcp range failed! TagName[%{public}s].", range.strTagName.c_str());
            return false;
        }
    } else {
        range.strTagName = ifaceName;
        if (mDhcpService->SetDhcpRange(ifaceName, range) != 0) {
            WIFI_LOGE("Set dhcp range failed! TagName[%{public}s].", range.strTagName.c_str());
            return false;
        }
    }
    WIFI_LOGD("Set dhcp range : ifaceName[%{public}s] TagName[%{public}s] start ip[%s] end ip[%s]",
        ifaceName.c_str(),
        range.strTagName.c_str(),
        range.strStartip.c_str(),
        range.strEndip.c_str());
    return true;
}

bool DhcpdInterface::GetConnectedStationInfo(const std::string &ifaceName, std::map<std::string, StationInfo> &result)
{
    if (mDhcpService == nullptr) {
        return false;
    }
    std::vector<std::string> leaseInfo;
    if (mDhcpService->GetLeases(ifaceName, leaseInfo) != 0) {
        WIFI_LOGE("Get dhcp lease info failed!");
        return false;
    }
    /* currently, our dhcp server's lease message type is :
     * expire_time mac_address ip_address hostname client_identifier_info
     * and when dhcpv6 type, output a line begin with "duid", skip it.
     */
    for (auto str : leaseInfo) {
        std::vector<std::string> tmp;
        SplitString(str, " ", tmp);
        if (tmp.empty()) {
            continue;
        }
        if (tmp[0] == "duid") {
            break;
        }
        if (tmp.size() < DHCP_LEASE_FORMAT_SIZE) {
            continue;
        }
        /* mac: [00-][00:00:00:00:00:00] */
        std::string mac = tmp[DHCP_LEASE_MAC_ADDR_POS];
        if (mac.length() < MAC_ADDR_MAX_LEN) {
            continue;
        }
        if (mac.length() > MAC_ADDR_MAX_LEN) {
            mac = mac.substr(mac.length() - MAC_ADDR_MAX_LEN);
        }
        StationInfo info;
        info.deviceName = tmp[DHCP_LEASE_HOSTNAME_POS];
        info.bssid = mac;
        info.ipAddr = tmp[DHCP_LEASE_IP_ADDR_POS];
        result.insert(std::make_pair(info.bssid, info));
    }
    return true;
}

bool DhcpdInterface::StopDhcpServer(const std::string &ifaceName)
{
    if (ifaceName.empty() || mDhcpService == nullptr) {
        return false;
    }

    std::string rangeName;
    std::string tagName = ifaceName;
    transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
    if (tagName.find("p2p") != std::string::npos) {
        rangeName = "p2p";
    } else {
        rangeName = ifaceName;
    }

    if (mDhcpService->RemoveAllDhcpRange(rangeName) != 0) {
        WIFI_LOGW("failed to remove [%{public}s] dhcp range.", rangeName.c_str());
    }

    if (mDhcpService->StopDhcpServer(ifaceName) != 0) {
        WIFI_LOGE("Stop dhcp server failed!");
        return false;
    }
    if (!NetworkInterface::ClearAllIpAddress(ifaceName)) {
        WIFI_LOGW("Clear interface binding ip address failed!");
    }

    if (mDhcpService != nullptr) {
        mDhcpService.reset(nullptr);
    }
    return true;
}

bool DhcpdInterface::ApplyIpAddress(const std::string &ifaceName, const Ipv4Address &ipv4, const Ipv6Address &ipv6)
{
    bool ret = NetworkInterface::AddIpAddress(ifaceName, ipv4);
    if (ipv6.IsValid()) {
        ret |= NetworkInterface::AddIpAddress(ifaceName, ipv6);
    }
    return ret;
}

bool DhcpdInterface::AssignIpAddr(Ipv4Address &ipv4, Ipv6Address &ipv6, const std::vector<Ipv4Address> &vecIpv4Addr,
    const std::vector<Ipv6Address> &vecIpv6Addr, bool isIpV4)
{
    if (isIpV4) {
        ipv4 = AssignIpAddrV4(vecIpv4Addr, IP_V4_MASK);
        if (ipv4 == Ipv4Address::INVALID_INET_ADDRESS) {
            WIFI_LOGE("Failed to allocate the IP address.");
            return false;
        }
    } else {
        Ipv6Address apShareIp(Ipv6Address::INVALID_INET6_ADDRESS);
        for (auto iter = vecIpv6Addr.begin(); iter != vecIpv6Addr.end(); ++iter) {
            if (Ipv6Address::IsAddrLocallink(iter->GetIn6Addr()) || !iter->IsValid()) {
                continue;
            }
            apShareIp = *iter;
            break;
        }
        Ipv6Address prefixIp = (apShareIp.GetAddressPrefixLength() > (IP_V6_ADDR_LEN - EU_I64_ADDR_LEN))
                                   ? AssignIpAddrV6(vecIpv6Addr)
                                   : apShareIp;
        if (!prefixIp.IsValid()) {
            WIFI_LOGE("Failed to allocate the IP address.");
            return false;
        }

        ipv6 = Ipv6Address::Create(prefixIp.GetAddressWithString(), prefixIp.GetAddressPrefixLength(), 0);
    }
    return true;
}

bool DhcpdInterface::CompareSubNet(
    const std::vector<Ipv4Address> &vecIpAddr, const struct in_addr &input, const struct in_addr &mask) const
{
    /* Check whether the network ID is the same as the IP address in vecIpAddr. */
    for (auto address : vecIpAddr) {
        struct in_addr tmpAddr = {INADDR_ANY};
        if (inet_aton(address.GetAddressWithString().c_str(), &tmpAddr) == 0) {
            WIFI_LOGE("convert ipaddress %{private}s failed!", address.GetAddressWithString().c_str());
            return true;
        }
        if (CALC_SUBNET(tmpAddr.s_addr, mask.s_addr) == input.s_addr) {
            return true;
        }
    }
    return false;
}

Ipv4Address DhcpdInterface::AssignIpAddrV4(const std::vector<Ipv4Address> &vecIpAddr, const std::string &mask) const
{
    struct in_addr maskAddr = {INADDR_ANY};
    if (inet_aton(mask.c_str(), &maskAddr) == 0) {
        WIFI_LOGE("convert mask to ipaddress failed![%s].", mask.c_str());
        return Ipv4Address::INVALID_INET_ADDRESS;
    }
    struct in_addr initAddr = {INADDR_ANY};
    if (inet_aton(IP_V4_DEFAULT.c_str(), &initAddr) == 0) {
        WIFI_LOGE("convert default ipaddress failed![%s].", IP_V4_DEFAULT.c_str());
        return Ipv4Address::INVALID_INET_ADDRESS;
    }
    struct in_addr tmpAddr = {INADDR_ANY};
    while (true) {
        tmpAddr.s_addr = CALC_SUBNET(initAddr.s_addr, maskAddr.s_addr);
        if (!CompareSubNet(vecIpAddr, tmpAddr, maskAddr)) {
            return Ipv4Address::Create(initAddr, maskAddr);
        }
        /* For conflict, try to change the new network. */
        uint32_t cSubnet = ntohl(htonl(IN_CLASSC_NET & IN_CLASSB_HOST) & tmpAddr.s_addr) >> IN_CLASSC_NSHIFT;
        cSubnet++;
        if (cSubnet == 0xFF) {
            WIFI_LOGE("No available IPv4 address is found.\n");
            return Ipv4Address::INVALID_INET_ADDRESS;
        } else {
            tmpAddr.s_addr = (tmpAddr.s_addr & htonl(IN_CLASSB_NET)) | htonl(cSubnet << IN_CLASSC_NSHIFT);
            initAddr.s_addr = tmpAddr.s_addr | (initAddr.s_addr & htonl(IN_CLASSC_HOST));
        }
    }
    return Ipv4Address::INVALID_INET_ADDRESS;
}

Ipv6Address DhcpdInterface::AssignIpAddrV6(const std::vector<Ipv6Address> &vecIpAddr)
{
    struct in6_addr prefix = IN6ADDR_ANY_INIT;
    std::random_device rd;
    int loopNum = 10;
    while (loopNum > 0) {
        bool bFlag = true;
        prefix.s6_addr[0] = 0xFD;
        for (int i = 1; i < (GENE_V6_ADDR_LEN / CHAR_BIT); i++) {
            prefix.s6_addr[i] = std::abs((int)rd()) % CHAR_MAX;
        }
        for (auto address : vecIpAddr) {
            struct in6_addr tmpAddr = IN6ADDR_ANY_INIT;
            if (inet_pton(AF_INET6, address.GetAddressWithString().c_str(), &tmpAddr) <= 0) {
                WIFI_LOGI("IpAddr:bad ip:%s and inet_pton error.", address.GetAddressWithString().c_str());
                continue;
            }
            if (memcmp(&tmpAddr, &prefix, sizeof(in6_addr)) == 0) {
                bFlag = false;
                WIFI_LOGI("same IP: %x and %x.", tmpAddr.s6_addr32[0], tmpAddr.s6_addr32[1]);
                break;
            }
        }
        if (bFlag) {
            char retStr[256] = {0};
            if (inet_ntop(AF_INET6, &prefix, retStr, sizeof(retStr)) != nullptr) {
                return Ipv6Address::Create(std::string(retStr), GENE_V6_ADDR_LEN, 0);
            } else {
                WIFI_LOGE("inet_ntop convert ipv6 address failed!");
                return Ipv6Address::INVALID_INET6_ADDRESS;
            }
        }
        loopNum--;
    }
    WIFI_LOGE("Fatal error,can not generate valid ULA addr!");
    return Ipv6Address::INVALID_INET6_ADDRESS;
}
}  // namespace Wifi
}  // namespace OHOS