/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mock_network_interface.h"

namespace OHOS {
namespace Wifi {
namespace NetworkInterface {
bool IsValidInterfaceName(const std::string &interfaceName)
{
    return MockNetworkInterface::GetInstance().IsValidInterfaceName(interfaceName);
}

bool IsInterfaceUp(const std::string &interfaceName)
{
    return MockNetworkInterface::GetInstance().IsInterfaceUp(interfaceName);
}

void Dump(const std::string &interfaceName)
{
    MockNetworkInterface::GetInstance().Dump(interfaceName);
}

bool FetchInterfaceConfig(
    const std::string &interfaceName, MacAddress &Mac, Ipv4Address &IPv4, std::vector<Ipv6Address> &vecIPv6)
{
    return MockNetworkInterface::GetInstance().FetchInterfaceConfig(interfaceName, Mac, IPv4, vecIPv6);
}

bool GetIpv4Address(const std::string &interfaceName, Ipv4Address &IPv4)
{
    return MockNetworkInterface::GetInstance().GetIpv4Address(interfaceName, IPv4);
}

bool GetAllIpv6Address(const std::string &interfaceName, std::vector<Ipv6Address> &vecIPv6)
{
    return MockNetworkInterface::GetInstance().GetAllIpv6Address(interfaceName, vecIPv6);
}

bool IsExistAddressForInterface(const std::string &interfaceName, const BaseAddress &address)
{
    return MockNetworkInterface::GetInstance().IsExistAddressForInterface(interfaceName, address);
}

bool AddIpAddress(const std::string &interfaceName, const BaseAddress &IpAddress)
{
    return MockNetworkInterface::GetInstance().AddIpAddress(interfaceName, IpAddress);
}

bool DelIpAddress(const std::string &interfaceName, const BaseAddress &IpAddress)
{
    return MockNetworkInterface::GetInstance().DelIpAddress(interfaceName, IpAddress);
}

bool ClearAllIpAddress(const std::string &interfaceName)
{
    return MockNetworkInterface::GetInstance().ClearAllIpAddress(interfaceName);
}

bool FetchIpAddress(
    const std::string &interfaceName, std::vector<Ipv4Address> &vecIPv4, std::vector<Ipv6Address> &vecIPv6)
{
    return MockNetworkInterface::GetInstance().FetchIpAddress(interfaceName, vecIPv4, vecIPv6);
}

bool SaveIpAddress(struct ifaddrs *ifa, Ipv4Address &ipv4, std::vector<Ipv6Address> &vecIPv6)
{
    return MockNetworkInterface::GetInstance().SaveIpAddress(ifa, ipv4, vecIPv6);
}

bool IpAddressChange(const std::string &interface, const BaseAddress &IpAddress, bool Action, bool Dad)
{
    return MockNetworkInterface::GetInstance().IpAddressChange(interface, IpAddress, Action, Dad);
}
} /* namespace NetworkInterface */

MockNetworkInterface &MockNetworkInterface::GetInstance()
{
    static MockNetworkInterface gMockNetworkInterface;
    return gMockNetworkInterface;
};
} // namespace Wifi
} // namespace OHOS