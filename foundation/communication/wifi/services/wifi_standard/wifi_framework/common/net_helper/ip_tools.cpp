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

#include "ip_tools.h"

namespace OHOS {
namespace Wifi {
std::string IpTools::ConvertIpv4Address(unsigned int addressIpv4)
{
    std::string address;
    if (addressIpv4 == 0) {
        return address;
    }

    std::ostringstream stream;
    stream<<((addressIpv4>>BITS_24) & 0xFF)<<"."<<((addressIpv4>>BITS_16) & 0xFF)<<"."
    <<((addressIpv4>>BITS_8) & 0xFF)<<"."<<(addressIpv4 & 0xFF);
    address = stream.str();

    return address;
}

unsigned int IpTools::ConvertIpv4Address(const std::string &address)
{
    std::string tmpAddress = address;
    unsigned int addrInt = 0;
    unsigned int i = 0;
    for (i = 0; i < IPV4_DOT_NUM; i++) {
        std::string::size_type npos = tmpAddress.find(".");
        if (npos == std::string::npos) {
            break;
        }
        std::string value = tmpAddress.substr(0, npos);
        unsigned int tmp = std::atoi(value.c_str());
        if ((tmp < MIN_BYTE) || (tmp > MAX_BYTE)) {
            break;
        }
        addrInt += tmp << ((IPV4_DOT_NUM - i) * BIT_NUM_BYTE);
        tmpAddress = tmpAddress.substr(npos + 1);
    }

    if (i != IPV4_DOT_NUM) {
        return 0;
    }
    int tmp = std::atoi(tmpAddress.c_str());
    if ((tmp < MIN_BYTE) || (tmp > MAX_BYTE)) {
        return 0;
    }
    addrInt += tmp;

    return addrInt;
}

std::string IpTools::ConvertIpv6Address(const std::vector<unsigned char> &addressIpv6)
{
    std::string address;
    if (addressIpv6.size() != IPV6_BYTE_NUM) {
        return address;
    }

    std::ostringstream stream;
    stream << std::hex << std::setw(POS_2) << std::setfill('0') << static_cast<int>(addressIpv6[0]);
    stream << std::hex << std::setw(POS_2) << std::setfill('0') << static_cast<int>(addressIpv6[1]);
    for (int i = POS_2; i < IPV6_BYTE_NUM; i += POS_2) {
        stream << ":";
        stream << std::hex << std::setw(POS_2) << std::setfill('0') << static_cast<int>(addressIpv6[i]);
        stream << std::hex << std::setw(POS_2) << std::setfill('0') << static_cast<int>(addressIpv6[i + 1]);
    }
    address = stream.str();

    return address;
}

void IpTools::ConvertIpv6Address(const std::string &address, std::vector<unsigned char> &addressIpv6)
{
    std::string tmpAddress = address;
    addressIpv6.clear();
    std::vector<unsigned char> ipv6;
    int i = 0;
    for (i = 0; i < IPV6_COLON_NUM; i++) {
        std::string::size_type npos = tmpAddress.find(":");
        if (npos == std::string::npos) {
            break;
        }

        std::string value = tmpAddress.substr(0, npos);
        if (value.size() != IPV6_DIGIT_NUM_PER_SEG) {
            break;
        }
        ipv6.push_back(std::stoi(value.substr(POS_0, HEX_BYTE_DIGIT_NUM), nullptr, HEX_FORM));
        ipv6.push_back(std::stoi(value.substr(POS_2, HEX_BYTE_DIGIT_NUM), nullptr, HEX_FORM));
        tmpAddress = tmpAddress.substr(npos + 1);
    }

    if (i != IPV6_COLON_NUM) {
        return;
    }
    if (tmpAddress.size() != IPV6_DIGIT_NUM_PER_SEG) {
        return;
    }
    ipv6.push_back(std::stoi(tmpAddress.substr(POS_0, HEX_BYTE_DIGIT_NUM), nullptr, HEX_FORM));
    ipv6.push_back(std::stoi(tmpAddress.substr(POS_2, HEX_BYTE_DIGIT_NUM), nullptr, HEX_FORM));

    addressIpv6.assign(ipv6.begin(), ipv6.end());
    return;
}

std::string IpTools::ConvertIpv4Mask(int prefixLength)
{
    std::string netMask;
    if (prefixLength < MIN_PREFIX_LEN || prefixLength > MAX_PREFIX_LEN) {
        return netMask;
    }

    int mask[IPV4_BYTE_NUM] = {0, 0, 0, 0};
    int quot = prefixLength / BIT_NUM_PER_BYTE;
    unsigned int remain = prefixLength % BIT_NUM_PER_BYTE;
    for (int i = 0; i < quot; i++) {
        mask[i] = MAX_IPV4_MASK_BYTE;
    }
    if (quot < IPV4_BYTE_NUM) {
        mask[quot] = (MAX_BYTE + 1) - (1 << (BIT_NUM_PER_BYTE - remain));
    }
    std::ostringstream stream;
    stream << mask[POS_0] << "." << mask[POS_1] << "." << mask[POS_2] << "." << mask[POS_3];
    netMask = stream.str();

    return netMask;
}

std::string IpTools::ConvertIpv6Mask(int prefixLength)
{
    std::string netMask;
    if (prefixLength < MIN_PREFIX_LEN || prefixLength > MAX_IPV6_PREFIX_LEN) {
        return netMask;
    }

    std::ostringstream stream;
    stream << prefixLength;
    netMask = stream.str();

    return netMask;
}

int IpTools::GetMaskLength(std::string mask)
{
    int netMask = 0;
    const int constMask = 0x80000000;
    unsigned int maskTmp = ntohl(static_cast<int>(inet_addr(mask.c_str())));
    while (maskTmp & constMask) {
        netMask++;
        maskTmp = (maskTmp << 1);
    }

    return netMask;
}

void IpTools::GetExclusionObjectList(const std::string &exclusionObjectList, std::vector<std::string> &exclusionList)
{
    std::string tmpExclusionList = exclusionObjectList;
    std::vector<std::string> list;
    int listNum = count(tmpExclusionList.begin(), tmpExclusionList.end(), ',');
    int i = 0;
    for (i = 0; i < listNum; ++i) {
        std::string::size_type npos = tmpExclusionList.find(",");
        if (npos == std::string::npos) {
            break;
        }

        std::string exclusionOne = tmpExclusionList.substr(0, npos);
        /* Do you need to check whether the format of this website is correct? */
        list.push_back(exclusionOne);
        tmpExclusionList = tmpExclusionList.substr(npos + 1);
    }
    if (i != listNum) {
        return;
    }
    list.push_back(tmpExclusionList);
    exclusionList.assign(list.begin(), list.end());
    return;
}
}  // namespace Wifi
}  // namespace OHOS