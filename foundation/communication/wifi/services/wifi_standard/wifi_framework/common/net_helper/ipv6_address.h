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

#ifndef OHOS_ADDRESS_IPV6_H
#define OHOS_ADDRESS_IPV6_H

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <random>
#include "base_address.h"
#include "mac_address.h"

namespace OHOS {
namespace Wifi {
class Ipv6Address : public BaseAddress {
public:
    /**
     * @Description  Check whether the IPv6 address is a legal IPv6 address.
     *
     * @param ipv6 - IPv6 address. [input]
     * @return true - Yes    false - No.
     */
    static bool IsValidIPv6(const std::string &ipv6);
    /**
     * @Description  Check whether the IPv6 address is a local IPv6 address.
     *
     * @param ipv6 - IPv6 address. [input]
     * @return true - Yes    false - No.
     */
    static bool IsAddrLocallink(const struct in6_addr &ip6Addr);
    /**
     * @Description  Obtains the prefix of a specified IPv6 address.
     *
     * @param ip6Addr - IPv6 address [input]
     * @param prefixLength - Prefix Length [input]
     * @return Parameter invalid, IN6ADDR_ANY_INIT is returned.
               Otherwise, in6_addr is returned.
     */
    static struct in6_addr GetIpv6Prefix(struct in6_addr &ip6Addr, size_t prefixLength);
    /**
     * @Description  Obtains the IPv6 address mask with a specified prefix length.
     *
     * @param prefixLength - prefix length. [input]
     * @return Parameter invalid, IN6ADDR_ANY_INIT is returned.
               Otherwise, in6_addr is returned.
     */
    static struct in6_addr GetIpv6Mask(size_t prefixLength);

    /**
     * @Description  Create an IPv6Address object based on the IPv6
                     address with a prefix length.
     *
     * @param ipv6 - IPv6 abbreviation, string type such as fe80::0/64. [input]
     * @return Parameter invalid, INVALID_INET6_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static Ipv6Address Create(std::string ipv6);

    /**
     * @Description  method of construct an IPv6, generate an EUI-64 address
                     based on the input MAC address, and obtain the prefix
                     address to generate an IPv6 address.
     *
     * @param ipv6Prefix - IPv6 prefix address [input]
     * @param mac - MacAddress type of a MAC address [input]
     * @param prefixLength - Prefix length. The default value is 64. [input]
     * @return Parameter invalid, INVALID_INET6_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static Ipv6Address Create(const std::string &ipv6Prefix, MacAddress &mac, const size_t prefixLength = 64);

    /**
     * @Description  method of construct an IPv6, generate a random IPv6
                     address in a random way.
     *
     * @param ipv6Prefix - IPv6 prefix address [input]
     * @param prefixLength - prefix length [input]
     * @param rndSeed - Random number seed. [input]
     * @return Parameter invalid, INVALID_INET6_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static Ipv6Address Create(const std::string &ipv6Prefix, const size_t prefixLength, unsigned int rndSeed);

    /**
     * @Description  method of construct an IPv6, generate an IPv6 address
                     based on input in6_addr structure.
     *
     * @param i6_addr - in6_addr structure of IPv6 [input]
     * @param prefixLength - Prefix length. The default value is 128. [input]
     * @return Parameter invalid, INVALID_INET6_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static Ipv6Address Create(const struct in6_addr &i6Addr, const size_t prefixLength = 128);

    /**
     * @Description  method of construct an IPv6.
     *
     * @param ipv6 - string of IPv6 address. [input]
     * @param prefixLength - prefix length. [input]
     * @return Parameter invalid, INVALID_INET6_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static Ipv6Address Create(std::string ipv6, const size_t prefixLength);

    /**
     * @Description  method of construct an IPv6.
     *
     * @param ipv6 - string of IPv6 address. [input]
     * @param mask - string of Mask. [input]
     * @return Parameter invalid, INVALID_INET6_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static Ipv6Address Create(std::string ipv6, const std::string &mask);

public:
    static const Ipv6Address INVALID_INET6_ADDRESS; /* Invalid IPv6 object constant. */

public:
    /**
     * @Description  Supports default destructor methods.
     *
     * @param None
     * @return None
     */
    virtual ~Ipv6Address() = default;

    /**
     * @Description  Check whether the current IP address is valid.
     *
     * @param None
     * @return true - legal       false - illegal
     */
    bool IsValid() const override;

    /**
     * @Description  obtain the IPv6 address in the in6_addr format.
     *
     * @param None
     * @return in6_addr structure
     */
    struct in6_addr GetIn6Addr() const;

    /**
     * @Description  obtain the IPv6 address prefix in character format.
     *
     * @param None
     * @return string of theIPv6 address prefix.
     */
    std::string GetPrefix() const;

    /**
     * @Description  Obtain the description of the network where the
                     current IP address is located, for example,
                     fe80:1234::1/10.
     *
     * @param None
     * @return string of network.
     */
    std::string GetNetwork() const;

private:
    /**
     * @Description  Construction method.
     *
     * @param ipv6 - string of ipv6 address [input]
     * @param prefixLength - prefix length [input]
     * @return Ipv6Address object
     */
    Ipv6Address(const std::string &ipv6, const size_t prefixLength);
    /**
     * @Description  Generates random IPv6 addresses based on prefixes.
     *
     * @param ipv6Prefix - prefix of IPv6 [input]
     * @param prefixLength - prefix length [input]
     * @return string of random IPv6 address
     */
    static std::string GetRandomAddr(const std::string &ipv6Prefix, int prefixLength);
    /**
     * @Description  Converts four binary strings to a hexadecimal string.
     *
     * @param strBin - four binary strings [input]
     * @return a hexadecimal string
     */
    static std::string BinToHex(const std::string &strBin);
    /**
     * @Description  Converts a hexadecimal string to four binary strings.
     *
     * @param strHex - a hexadecimal string [input]
     * @return four binary strings
     */
    static std::string HexToBin(const std::string &strHex);
    /**
     * @Description  generating EUI-64 addresses based on MAC addresses.
     *
     * @param mac - MacAddress of a MAC address [input]
     * @return string of EUI-64 address
     */
    static std::string MacToEui64addr(MacAddress &mac);
};
}  // namespace Wifi
}  // namespace OHOS

#endif /* OHOS_ADDRESS_IPV6_H */