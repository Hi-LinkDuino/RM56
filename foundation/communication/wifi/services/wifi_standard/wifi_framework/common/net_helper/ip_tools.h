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

#ifndef OHOS_IP_TOOLS_H
#define OHOS_IP_TOOLS_H

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace OHOS {
namespace Wifi {
static const int MIN_PREFIX_LEN = 0;
static const int MAX_PREFIX_LEN = 32;
static const int MAX_IPV6_PREFIX_LEN = 128;
static const int BIT_NUM_PER_BYTE = 8;
static const int IPV4_BYTE_NUM = 4;
static const unsigned int IPV4_DOT_NUM = 3;
static const int IPV6_BYTE_NUM = 16;
static const int IPV6_DIGIT_NUM_PER_SEG = 4;
static const int IPV6_COLON_NUM = 7;
static const int MAX_IPV4_MASK_BYTE = 255;
static const int POS_0 = 0;
static const int POS_1 = 1;
static const int POS_2 = 2;
static const int POS_3 = 3;
static const int HEX_BYTE_DIGIT_NUM = 2;
static const int HEX_FORM = 16;
static const int MIN_BYTE = 0;
static const int MAX_BYTE = 255;
static const unsigned int BIT_NUM_BYTE = 8;
static const int BITS_24 = 24;
static const int BITS_16 = 16;
static const int BITS_8 = 8;

class IpTools {
public:
    /**
     * @Description : Converts an IPv4 address in the int format to a string format.
     *
     * @param addressIpv4 - IPv4 address in the int format.[in]
     * @return std::string : IPv4 address in string format.
     */
    static std::string ConvertIpv4Address(unsigned int addressIpv4);

    /**
     * @Description : Converts an IPv4 address in string format to an int format.
     *
     * @param address - IPv4 address in string format.[in]
     * @return unsigned int : Specifies the IPv4 address in the int format. 0 is failure.
     */
    static unsigned int ConvertIpv4Address(const std::string &address);

    /**
     * @Description : Converts a 16-byte IPv6 address to a string.
     *
     * @param addressIpv6 - 16-byte IPv6 address.[in]
     * @return std::string : IPv6 address in string format
     */
    static std::string ConvertIpv6Address(const std::vector<unsigned char> &addressIpv6);

    /**
     * @Description : Converts the IPv6 address of a string into a 16-byte format.
     *
     * @param address - IPv6 address in string format.[in]
     * @param addressIpv6 - 16-byte IPv6 address.[in]
     */
    static void ConvertIpv6Address(const std::string &address, std::vector<unsigned char> &addressIpv6);

    /**
     * @Description : Converts the prefix length to an IPv4 mask.
     *
     * @param prefixLength - Prefix Length.[in]
     * @return std::string : IPv4 mask in string format.
     */
    static std::string ConvertIpv4Mask(int prefixLength);

    /**
     * @Description : Converts the prefix length in the int format to the string format.
     *
     * @param prefixLength - Prefix Length.[in]
     * @return std::string : Prefix length in string format.
     */
    static std::string ConvertIpv6Mask(int prefixLength);

    /**
     * @Description : Obtains the length based on the subnet mask.
     *
     * @param mask - The mask.[in]
     * @return int
     */
    static int GetMaskLength(std::string mask);

    /**
     * @Description : Resolving Web Sites Bypassing Agents.
     *
     * @param exclusionObjectList - Website list, separated by commas(,).[in]
     * @param exclusionList - Website List.[in]
     */
    static void GetExclusionObjectList(const std::string &exclusionObjectList, std::vector<std::string> &exclusionList);
};
}  // namespace Wifi
}  // namespace OHOS

#endif