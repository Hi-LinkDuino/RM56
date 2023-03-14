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

#ifndef OHOS_NETWORK_INTERFACE_H
#define OHOS_NETWORK_INTERFACE_H

#include <ifaddrs.h>
#include <string>
#include <vector>
#include <ifaddrs.h>

#include "ipv4_address.h"
#include "ipv6_address.h"

namespace OHOS {
namespace Wifi {
namespace NetworkInterface {
/**
 * @Description  Check whether the network interface name is valid.
 *
 * @param interfaceName - Network Interface Name
 * @return true - legal       false - illegal
 */
bool IsValidInterfaceName(const std::string &interfaceName);

/**
 * @Description  Print all members in an object
 *
 * @param interfaceName - Network Interface Name
 * @return None
 */
void Dump(const std::string &interfaceName);

/**
 * @Description  Obtain the current configuration from the network interface name.
 *
 * @param interfaceName - Network Interface Name [input]
 * @param ipv4 - ipv4 address of network interface [output]
 * @param vecIPv6 - the vector of ipv6 addresses of network [output]
 * @return true - fetch success     false - fetch failed
 */
bool FetchInterfaceConfig(
    const std::string &interfaceName, std::vector<Ipv4Address> &vecIPv4, std::vector<Ipv6Address> &vecIPv6);

/**
 * @Description  Obtains the IPv4 address object of the current network interface.
 *
 * @param interfaceName - Network Interface Name [input]
 * @param vecIPv4 - the vector of address of the network interface interface. If
                 the IPv4 address fails to be obtained, an invalid IPv4
                 ddress is returned. [output]
 * @return true - success   false - failed
 */
bool GetIpv4Address(const std::string &interfaceName, std::vector<Ipv4Address> &ipv4);

/**
 * @Description  Obtains all IPv6 addresses of a network interface.
 *
 * @param interfaceName - Network Interface Name [input]
 * @param vecIPv6 - All IPv6 addresses configured for the current
                    network interface [output]
 * @return true - success   false - failed
 */
bool GetAllIpv6Address(const std::string &interfaceName, std::vector<Ipv6Address> &vecIPv6);

/**
 * @Description  Whether an address exists in the network interface.
 *
 * @param interfaceName - Network Interface Name [input]
 * @param address - Specifies the IPv4 or IPv6 address to be queried. [input]
 * @return true - exist    false - no exist
 */
bool IsExistAddressForInterface(const std::string &interfaceName, const BaseAddress &address);

/**
 * @Description  Set the IPv4 or IPv6 address to the network interface.
 *
 * @param interfaceName - Network Interface Name [input]
 * @param ipAddress - Specifies the IPv4 or IPv6 address to be added. [input]
 * @return true - success    false - failed
 */
bool AddIpAddress(const std::string &interfaceName, const BaseAddress &ipAddress);

/**
 * @Description  Deletes information about a specified IPv4 or IPv6
                 address from a network interface.
 *
 * @param interfaceName - NetworkInterface Name [input]
 * @param ipAddress - Specifies the IPv4 or IPv6 address to be deleted. [input]
 * @return true - success    false - failed
 */
bool DelIpAddress(const std::string &interfaceName, const BaseAddress &ipAddress);

/**
 * @Description  Clears all IPv4 or IPv6 addresses except local link
                 addresses starting with "fe80:"
 *
 * @param interfaceName - NetworkInterface Name [input]
 * @return true - clear success   false - clear failed
 */
bool ClearAllIpAddress(const std::string &interfaceName);

/**
 * @Description  obtain IPv4 and IPv6 address from interfaceName
 *
 * @param interfaceName - Network Interface Name [input]
 * @param ipv4 - IPv4 address. [output]
 * @param vecIPv6 - IPv6 address. [output]
 * @return true - success    false - failed
 */
bool FetchIpAddress(
    const std::string &interfaceName, std::vector<Ipv4Address> &vecipv4, std::vector<Ipv6Address> &vecIPv6);

/**
 * @Description  Saves the IP address obtained through ifaddrs.
 *
 * @param ifa - Source address. [input]
 * @param vecIPv4 - IPv4 address. [output]
 * @param vecIPv6 - IPv6 address. [output]
 * @return true - success    false - failed
 */
bool SaveIpAddress(const struct ifaddrs &ifa, std::vector<Ipv4Address> &vecIPv4, std::vector<Ipv6Address> &vecIPv6);

/**
 * @Description  obtain IPv4 and IPv6 address from interfaceName
 *
 * @param interfaceName - Network Interface Name. [input]
 * @param ipAddress - IP address. [input]
 * @param action - true - add ip   flase - del app [input]
 * @param dad - duplicate address detection,default disable [input]
 * @return true - success    false - fail
 */
bool IpAddressChange(const std::string &interface, const BaseAddress &ipAddress, bool action, bool dad = false);
/**
 * @Description  Write data to a file.
 * @param fileName - file name
 * @param content - Character data to be written
 * @return true: success     false: fail
 */
bool WriteDataToFile(const std::string &fileName, const std::string &content);
}; /* namespace NetworkInterface */
}  // namespace Wifi
}  // namespace OHOS

#endif /* OHOS_NETWORK_INTERFACE_H */