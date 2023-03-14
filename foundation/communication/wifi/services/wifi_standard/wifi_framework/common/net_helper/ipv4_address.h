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
#ifndef OHOS_ADDRESS_IPV4_H
#define OHOS_ADDRESS_IPV4_H

#include "base_address.h"

constexpr int MAX_MASK_LENGTH = 32;

namespace OHOS {
namespace Wifi {
class Ipv4Address : public BaseAddress {
public:
    /**
     * @Description  Check whether an IPv4 address is a valid IPv4 address.
     *
     * @param None
     * @return true - legal    false - illegal
     */
    static bool IsValidIPv4(const std::string &ipv4);

    /**
     * @Description  Factory method for generating an object from Ipv4Address
     *
     * @param IPv4 - Character description, for example, 192.168.1.1 [input]
     * @param prefixLength - Number of bits 1 in the mask. [input]
                For example, if the mask is 255.255.255.0, the value is 24.
     * @return Ipv4Address - Create the generated IPv4Address object.
     */
    static Ipv4Address Create(const std::string &ipv4, size_t prefixLength);

    /**
     * @Description  Factory method for generating an object from Ipv4Address
     *
     * @param IPv4 - Character description, for example, 192.168.1.1 [input]
     * @param Mask - IPv4 mask, for example, 255.255.255.0 [input]
     * @return Ipv4Address - Create the generated IPv4Address object.
     */
    static Ipv4Address Create(const std::string &ipv4, const std::string &mask);

    /**
     * @Description  Factory method for generating an object from Ipv4Address
     *
     * @param IPv4 - IPv4 address description of the in_addr type [input]
     * @param Mask - IPv4 mask description of the in_addr type [input]
     * @return Ipv4Address - Create the generated IPv4Address object.
     */
    static Ipv4Address Create(const in_addr &ipv4, const in_addr &mask);

public:
    static const Ipv4Address INVALID_INET_ADDRESS; /* Invalid IPv4 object constant. */

public:
    /**
     * @Description  Use Default Destruction
     *
     * @param None
     * @return None
     */
    virtual ~Ipv4Address() = default;

    /**
     * @Description  Check whether the current IP address is valid.
     *
     * @param None
     * @return true - legal       false - illegal
     */
    bool IsValid() const override;
    /**
     * @Description  Obtains an IPv4 address of the in_addr type.
     *
     * @param None
     * @return in_addr - IPv4 address of the in_addr type
     */
    in_addr GetAddressWithInet() const;

    /**
     * @Description  Obtains the subnet mask of the IPv4 character description.
     *
     * @param None
     * @return string - Subnet mask described by IPv4 characters
     */
    std::string GetMaskWithString() const;

    /**
     * @Description  Obtains the subnet mask of the IPv4 in_addr type.
     *
     * @param None
     * @return in_addr - Subnet mask described by IPv4 characters
     */
    in_addr GetMaskWithInet() const;

    /**
     * @Description  Obtains the network ID of the current IP address.
     *
     * @param None
     * @return string - Description of the string type of the network code.
     */
    std::string GetNetworkAddressWithString() const;

    /**
     * @Description  Obtains the network ID of the in_addr type of the current IP address.
     *
     * @param None
     * @return in_addr - Description of the in_addr type of the network code
     */
    in_addr GetNetworkAddressWithInet() const;

    /**
     * @Description  Obtains the host ID of the string type of the current IP address.
     *
     * @param None
     * @return string - Description of the string type of the host ID
     */
    std::string GetHostAddressWithString() const;

    /**
     * @Description  Obtains the host ID of the in_addr type of the current IP address.
     *
     * @param None
     * @return in_addr - Description of the in_addr type of the host ID
     */
    in_addr GetHostAddressWithInet() const;

    /**
     * @Description  Obtain the description of the network where the current IP address is located. For
                     example, if the description is 192.168.1.1/24, the description is 192.168.1.0/24.
     *
     * @param None
     * @return string - Description of the in_addr type of the host ID
     */
    std::string GetNetwork() const;

private:
    /**
     * @Description  Delete Default Construct
     *
     * @param None
     * @return None
     */
    Ipv4Address() = delete;

    /**
     * @Description Unique constructor, private. Use the factory mode create.
     *
     * @param IPv4 - IPv4 address in character format [input]
     * @param prefixLength - Number of bits whose mask is 1, that is, the prefix length [input]
     * @return Ipv4Address object
     */
    Ipv4Address(const std::string &ipv4, size_t prefixLength);
};
}  // namespace Wifi
}  // namespace OHOS

#endif /* OHOS_ADDRESS_IPV4_H */