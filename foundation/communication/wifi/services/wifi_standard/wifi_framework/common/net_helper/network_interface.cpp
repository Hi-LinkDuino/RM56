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
#include "network_interface.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <string>
#include <net/if.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <typeinfo>
#include <unistd.h>
#include <fcntl.h>

#include "securec.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("WifiNetworkInterface");

namespace OHOS {
namespace Wifi {
bool NetworkInterface::IsValidInterfaceName(const std::string &interfaceName)
{
    size_t len = interfaceName.length();
    if (len == 0 || len > IF_NAMESIZE) {
        return false;
    }

    if (!isalnum(interfaceName[0])) {
        return false;
    }

    for (size_t i = 1; i < len; i++) {
        char ch = interfaceName[i];
        if (!isalnum(ch) && (ch != '_' && ch != '-' && ch != ':')) {
            return false;
        }
    }

    return true;
}


void NetworkInterface::Dump(const std::string &interfaceName)
{
    WIFI_LOGI("InterfaceName  [%{private}s]", interfaceName.c_str());

    std::vector<Ipv4Address> vecIPv4;
    std::vector<Ipv6Address> vecIPv6;

    bool ret = FetchInterfaceConfig(interfaceName, vecIPv4, vecIPv6);
    if (!ret) {
        WIFI_LOGI("Fetch Interface [%{private}s] failed.", interfaceName.c_str());
    }

    WIFI_LOGI("\tIPv4  size   [%{private}zu]", vecIPv4.size());
    for (const auto &item : vecIPv4) {
        item.Dump();
    }

    WIFI_LOGI("\tIPv6  size   [%{private}zu]", vecIPv6.size());
    for (const auto &item : vecIPv6) {
        item.Dump();
    }
}

bool NetworkInterface::FetchInterfaceConfig(
    const std::string &interfaceName, std::vector<Ipv4Address> &vecIPv4, std::vector<Ipv6Address> &vecIPv6)
{
    if (!FetchIpAddress(interfaceName, vecIPv4, vecIPv6)) {
        WIFI_LOGE("interface [%{public}s] Fetch IP address failed.", interfaceName.c_str());
        return false;
    }
    return true;
}

bool NetworkInterface::GetIpv4Address(const std::string &interfaceName, std::vector<Ipv4Address> &vecIPv4)
{
    std::vector<Ipv6Address> vecIPv6;
    if (!FetchIpAddress(interfaceName, vecIPv4, vecIPv6)) {
        WIFI_LOGE("interface [%{public}s] Fetch IP address failed.", interfaceName.c_str());
        return false;
    }
    /*
     * The ipv4 address is not set for the network interface. In this case, the
     * ipv4 address is not updated.
     */
    return !(vecIPv4.empty());
}

bool NetworkInterface::GetAllIpv6Address(const std::string &interfaceName, std::vector<Ipv6Address> &vecIPv6)
{
    std::vector<Ipv4Address> vecIPv4;
    if (!FetchIpAddress(interfaceName, vecIPv4, vecIPv6)) {
        WIFI_LOGE("interface [%{public}s] Fetch IP address failed.", interfaceName.c_str());
        return false;
    }
    return true;
}

bool NetworkInterface::IsExistAddressForInterface(const std::string &interfaceName, const BaseAddress &address)
{
    std::vector<Ipv4Address> vecIPv4;
    std::vector<Ipv6Address> vecIPv6;
    if (!FetchIpAddress(interfaceName, vecIPv4, vecIPv6)) {
        WIFI_LOGE("interface [%{public}s] Fetch IP address failed.", interfaceName.c_str());
        return false;
    }

    for (const auto &iter : vecIPv4) {
        if (iter == address) {
            return true;
        }
    }

    for (const auto &iter : vecIPv6) {
        if (iter == address) {
            return true;
        }
    }

    return false;
}

bool NetworkInterface::AddIpAddress(const std::string &interfaceName, const BaseAddress &ipAddress)
{
    if (!ipAddress.IsValid()) {
        WIFI_LOGE("Add IP address [%{private}s] is not valid.", ipAddress.GetAddressWithString().c_str());
        return false;
    }

    /* Avoid repeated add. */
    if (IsExistAddressForInterface(interfaceName, ipAddress)) {
        WIFI_LOGI("In interface [%{public}s], the address [%{private}s] is exist.",
            interfaceName.c_str(),
            ipAddress.GetAddressWithString().c_str());
        return true;
    }

    if (!IpAddressChange(interfaceName, ipAddress, true)) {
        WIFI_LOGE("Interface [%{public}s] add address [%{private}s] failed.",
            interfaceName.c_str(),
            ipAddress.GetAddressWithString().c_str());
        return false;
    }
    return true;
}

bool NetworkInterface::DelIpAddress(const std::string &interfaceName, const BaseAddress &ipAddress)
{
    if (!ipAddress.IsValid()) {
        WIFI_LOGE("Del IP address [%{private}s] is not valid.", ipAddress.GetAddressWithString().c_str());
        return false;
    }

    if (!IsExistAddressForInterface(interfaceName, ipAddress)) {
        WIFI_LOGI("In interface [%{public}s], the address [%{private}s] is not exist.",
            interfaceName.c_str(),
            ipAddress.GetAddressWithString().c_str());
        return true;
    }
    if (!IpAddressChange(interfaceName, ipAddress, false)) {
        WIFI_LOGE("Interface [%{public}s] del address [%{private}s] failed.",
            interfaceName.c_str(),
            ipAddress.GetAddressWithString().c_str());
        return false;
    }
    return true;
}

bool NetworkInterface::ClearAllIpAddress(const std::string &interfaceName)
{
    std::vector<Ipv4Address> vecIPv4;
    std::vector<Ipv6Address> vecIPv6;
    bool ret = true;
    if (!FetchIpAddress(interfaceName, vecIPv4, vecIPv6)) {
        return false;
    }
    for (auto ip4 : vecIPv4) {
        ret &= DelIpAddress(interfaceName, ip4);
    }
    for (auto ip6 : vecIPv6) {
        ret &= DelIpAddress(interfaceName, ip6);
    }
    if (!ret) {
        WIFI_LOGW("Some ip del failed.");
    }
    return true;
}

bool NetworkInterface::SaveIpAddress(
    const struct ifaddrs &ifa, std::vector<Ipv4Address> &vecIPv4, std::vector<Ipv6Address> &vecIPv6)
{
    int ret = 0;
    char host[NI_MAXHOST] = {0}; /* IP address storage */
    char mask[NI_MAXHOST] = {0}; /* mask storage */
    int family = ifa.ifa_addr->sa_family;
    /* For an AF_INET* interface address, display the address */
    if (family == AF_INET || family == AF_INET6) {
        ret = getnameinfo(ifa.ifa_addr,
            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
            host,
            NI_MAXHOST,
            nullptr,
            0,
            NI_NUMERICHOST);
        if (ret != 0) {
            WIFI_LOGE("getnameinfo() failed: %{public}s\n", gai_strerror(ret));
            return false;
        }
        ret = getnameinfo(ifa.ifa_netmask,
            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
            mask,
            NI_MAXHOST,
            nullptr,
            0,
            NI_NUMERICHOST);
        if (ret != 0) {
            WIFI_LOGE("getnameinfo() failed: %{public}s\n", gai_strerror(ret));
            return false;
        }
        /* For an IPv6 address, the suffix %wlan0 exists. */
        char *sepNum = strchr(host, '%');
        if (sepNum != nullptr) {
            *sepNum = '\0';
        }
        if (family == AF_INET) {
            vecIPv4.push_back(Ipv4Address::Create(host, mask));
        } else if (family == AF_INET6) {
            vecIPv6.push_back(Ipv6Address::Create(host, mask));
        }
    }
    return true;
}

bool NetworkInterface::FetchIpAddress(
    const std::string &interfaceName, std::vector<Ipv4Address> &vecipv4, std::vector<Ipv6Address> &vecIPv6)
{
    struct ifaddrs *ifaddr = nullptr;
    struct ifaddrs *ifa = nullptr;
    bool ret = false;
    int n = 0;

    if (getifaddrs(&ifaddr) == -1) {
        WIFI_LOGE("getifaddrs failed, error is %{public}d", errno);
        return false;
    }

    for (ifa = ifaddr, n = 0; ifa != nullptr; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }
        /*
         * Display interface name and family (including symbolic
         * form of the latter for the common families)
         */
        if (strncmp(interfaceName.c_str(), ifa->ifa_name, IF_NAMESIZE) != 0 && !interfaceName.empty()) {
            continue;
        }
        ret |= SaveIpAddress(*ifa, vecipv4, vecIPv6);
    }

    freeifaddrs(ifaddr);
    return ret;
}

bool NetworkInterface::IpAddressChange(
    const std::string &interface, const BaseAddress &ipAddress, bool action, bool dad)
{
    if (!ipAddress.IsValid() || ipAddress.GetFamilyType() == BaseAddress::FamilyType::FAMILY_INET6) {
        WIFI_LOGE("bad input parameter [%{public}s][%{private}s]/[%{private}zu]to change ip.", interface.c_str(),
            ipAddress.GetAddressWithString().c_str(), ipAddress.GetAddressPrefixLength());
        return false;
    }
    std::string IpAddr;
    std::string Mask;
    if (action) {
        Ipv4Address ipv4 = Ipv4Address::Create(ipAddress.GetAddressWithString(), ipAddress.GetAddressPrefixLength());
        IpAddr = ipv4.GetAddressWithString();
        Mask = ipv4.GetMaskWithString();
    } else {
        IpAddr = "0.0.0.0";
        Mask = "0.0.0.0";
    }

    struct ifreq ifr;
    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK ||
        strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), interface.c_str(), interface.length()) != EOK) {
        return false;
    }

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return false;
    }

    struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    if (inet_aton(IpAddr.c_str(), &(sin->sin_addr)) < 0) {
        close(fd);
        return false;
    }
    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        close(fd);
        WIFI_LOGE("ioctl set ip address failed, error is: %{public}d.", errno);
        return false;
    }
    if (inet_aton(Mask.c_str(), &(sin->sin_addr)) < 0) {
        close(fd);
        return false;
    }
    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0) {
        close(fd);
        WIFI_LOGE("ioctl set mask address failed, error is: %{public}d.", errno);
        return false;
    }
    close(fd);
    return true;
}

bool NetworkInterface::WriteDataToFile(const std::string &fileName, const std::string &content)
{
    int fd = open(fileName.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd < 0) {
        WIFI_LOGE("open %{public}s fail, error: %{public}d", fileName.c_str(), errno);
        return false;
    }

    if (static_cast<size_t>(write(fd, content.c_str(), content.length())) != content.length()) {
        WIFI_LOGE("write content [%{public}s] to file [%{public}s] failed. error: %{public}d.",
            content.c_str(), fileName.c_str(), errno);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}
}  // namespace Wifi
} // namespace OHOS
