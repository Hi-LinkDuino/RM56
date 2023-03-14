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

#ifndef OHOS_MOCK_NETWORK_INTERFACE_H
#define OHOS_MOCK_NETWORK_INTERFACE_H

#include <gmock/gmock.h>

#include "ipv4_address.h"
#include "ipv6_address.h"
#include "mac_address.h"
#include <vector>
#include <string>

namespace OHOS {
namespace Wifi {
class MockNetworkInterface {
public:
    MOCK_METHOD1(IsValidInterfaceName, bool(const std::string &interfaceName));
    MOCK_METHOD1(IsInterfaceUp, bool(const std::string &interfaceName));
    MOCK_METHOD1(Dump, void(const std::string &interfaceName));

    MOCK_METHOD4(FetchInterfaceConfig,
        bool(const std::string &interfaceName, MacAddress &Mac, Ipv4Address &IPv4, std::vector<Ipv6Address> &vecIPv6));
    MOCK_METHOD2(GetIpv4Address, bool(const std::string &interfaceName, Ipv4Address &IPv4));
    MOCK_METHOD2(GetAllIpv6Address, bool(const std::string &interfaceName, std::vector<Ipv6Address> &vecIPv6));
    MOCK_METHOD2(IsExistAddressForInterface, bool(const std::string &interfaceName, const BaseAddress &address));
    MOCK_METHOD2(AddIpAddress, bool(const std::string &interfaceName, const BaseAddress &IpAddress));
    MOCK_METHOD2(DelIpAddress, bool(const std::string &interfaceName, const BaseAddress &IpAddress));
    MOCK_METHOD1(ClearAllIpAddress, bool(const std::string &interfaceName));
    MOCK_METHOD3(FetchIpAddress,
        bool(const std::string &interfaceName, std::vector<Ipv4Address> &vecIPv4, std::vector<Ipv6Address> &vecIPv6));
    MOCK_METHOD3(SaveIpAddress, bool(struct ifaddrs *ifa, Ipv4Address &ipv4, std::vector<Ipv6Address> &vecIPv6));
    MOCK_METHOD4(
        IpAddressChange, bool(const std::string &interface, const BaseAddress &IpAddress, bool Action, bool Dad));
    static MockNetworkInterface &GetInstance(void);

private:
    MockNetworkInterface() {}
    ~MockNetworkInterface() {}
};
} // namespace Wifi
} // namespace OHOS

#endif /* OHOS_NETWORK_INTERFACE_H */