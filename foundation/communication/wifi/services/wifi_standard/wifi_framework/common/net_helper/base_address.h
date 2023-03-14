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

#ifndef OHOS_BASE_ADDRESS_H
#define OHOS_BASE_ADDRESS_H

#include <arpa/inet.h>
#include <string>

namespace OHOS {
namespace Wifi {
class BaseAddress {
public:
    enum class FamilyType {
        FAMILY_UNSPEC = AF_UNSPEC, /* wrong type */
        FAMILY_INET = AF_INET,     /* ipv4 type */
        FAMILY_INET6 = AF_INET6,   /* ipv6 type */
    };

public:
    /**
     * @Description  delete default construct method
     *
     * @param None
     * @return None
     */
    BaseAddress() = delete;
    /**
     * @Description  unique construction method
     *
     * @param None
     * @return None
     */
    BaseAddress(const std::string &ip, size_t prefixLength, FamilyType family);

    /**
     * @Description  The == operator is reloaded to determine whether two
                     IP addresses are the same but not the prefix length.
                     (The string description of IPv6 in the code has only
                     one abbreviation.).
     *
     * @param None
     * @return None
     */
    bool operator==(const BaseAddress &) const;
    /**
     * @Description  destructor methods.
     *
     * @param None
     * @return None
     */
    virtual ~BaseAddress();

    /**
     * @Description  Check whether the current IP address is valid.
     *
     * @param None
     * @return true - legal       false - Illegal
     */
    virtual bool IsValid() const = 0;

    /**
     * @Description  Dump member data.
     *
     * @param None
     * @return None
     */
    virtual void Dump() const;

    /**
     * @Description  Obtains the IP address type.
     *
     * @param None
     * @return IPV4 - the current object is an IPv4 address.
               IPV6 - the current object is an IPv6 address.
     */
    inline FamilyType GetFamilyType() const
    {
        return family_;
    }

    /**
     * @Description  Obtain the address.
     *
     * @param None
     * @return string - Character description address in the current object
     */
    inline const std::string &GetAddressWithString() const
    {
        return ipAddress_;
    }

    /**
     * @Description  Obtains the prefix length of the current address.
     *
     * @param None
     * @return size_t - prefix length
     */
    inline size_t GetAddressPrefixLength() const
    {
        return prefixLength_;
    }

private:
    /**
     * Address family, which indicates the current IP version.
     * the length of the network prefix. The number of bits of IPv4 mask or
     * the number of IPv6 prefixes.
     */
    FamilyType family_;
    size_t prefixLength_;

    /* Description of an IPv4 or IPv6 address string */
    std::string ipAddress_;
};
}  // namespace Wifi
}  // namespace OHOS

#endif /* OHOS_BASE_ADDRESS_H */