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

#ifndef OHOS_MAC_ADDRESS_H
#define OHOS_MAC_ADDRESS_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

constexpr int ZERO = 0;
constexpr int ONE = 1;
constexpr int TWO = 2;
constexpr int THREE = 3;
constexpr int FOUR = 4;
constexpr int FIVE = 5;
constexpr int SIX = 6;
constexpr int SEVEN = 7;
constexpr int ETH_ALEN = 6;

/* ETH_ALEN   Size of the MAC address binary data */
constexpr size_t MAC_STRING_LENGTH = ETH_ALEN * 2 + (ETH_ALEN - 1); /* length of the string of mac address */

#ifndef MAC_LEN
#define MAC_LEN 6
#endif

namespace OHOS {
namespace Wifi {
class MacAddress {
public:
    /**
     * @Description  Check whether the MAC address is valid.
     *
     * @param mac - string of mac address [input]
     * @return true - legal    false - illegal
     */
    static bool IsValidMac(const std::string &mac);

    /**
     * @Description  factory method
     *
     * @param mac - string of mac address [input]
     * @return Parameter invalid, the invalid object INVALID_MAC_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static MacAddress Create(const std::string &mac);

    /**
     * @Description  factory method
     *
     * @param hwAddr - sockaddr structure of mac address [input]
     * @return Parameter invalid, the invalid object INVALID_MAC_ADDRESS is returned.
               Otherwise, the successful object is returned.
     */
    static MacAddress Create(const sockaddr &hwAddr);

    /**
     * @Description  Obtaining the MAC address by interface name
     *
     * @param ifName - interface name
     * @param macAddr - Array for storing returned mac data
     * @return true - success    false - fail
     */
    static bool GetMacAddr(const std::string& ifName, unsigned char macAddr[MAC_LEN]);

    static const MacAddress INVALID_MAC_ADDRESS; /* Invalid MAC Address Object Constant */

    /**
     * @Description  The == operator is overloaded to determine whether
                     two MAC addresses represent the same MAC address.
     *
     * @param MacAddress of the compared MAC address. [input]
     * @return true - same mac    false - different mac.
     */
    bool operator==(const MacAddress &) const;

    /**
     * @Description  The == operator is overloaded to determine whether
                     two MAC addresses represent the same MAC address.
     *
     * @param sockaddr structure of the compared MAC address. [input]
     * @return true - same mac    false - different mac.
     */
    bool operator==(const struct sockaddr &) const;
    /**
     * @Description  Supports default destructor methods.
     *
     * @param None
     * @return None
     */
    virtual ~MacAddress() = default;

    /**
     * @Description  Check whether the current mac address is valid.
     *
     * @param None
     * @return true - legal       false - illegal
     */
    bool IsValid() const;

    /**
     * @Description  Print the MAC address.
     *
     * @param None
     * @return None
     */
    void Dump() const;

    /**
     * @Description  obtain the string of mac address
     *
     * @param None
     * @return the string of mac address
     */
    const std::string &GetMacAddressWifiString() const;

    /**
     * @Description  obtain the sockaddr structure of mac address
     *
     * @param None
     * @return the sockaddr structure of mac address
     */
    struct sockaddr GetMacAddressWifiSockaddr() const;

private:
    /**
     * @Description  Delete Default Construct
     *
     * @param None
     * @return None
     */
    MacAddress() = delete;

    /**
     * @Description  construction method
     *
     * @param mac - the string of mac address [input]
     * @return MacAddress object
     */
    explicit MacAddress(const std::string &mac);

private:
    std::string mac_;
};
}  // namespace Wifi
}  // namespace OHOS

#endif /* OHOS_MAC_ADDRESS_H */