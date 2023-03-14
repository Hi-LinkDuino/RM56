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
#include "ipv4_address.h"
#include <cstdlib>
#include <cstdio>
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_UTIL_Ipv4Address"

namespace OHOS {
namespace Wifi {
const int MAX_IPV4_PREFIX_LENGTH = 32;
const int MAX_IPV4_STRING_LENGTH = 64;
const Ipv4Address Ipv4Address::INVALID_INET_ADDRESS("255.255.255.255", MAX_IPV4_PREFIX_LENGTH);
bool Ipv4Address::IsValidIPv4(const std::string &ipv4)
{
    struct in_addr ipv4Addr = {INADDR_ANY};
    if (inet_pton(AF_INET, ipv4.c_str(), (void *)&ipv4Addr) != 1 ||
        ipv4 == INVALID_INET_ADDRESS.GetAddressWithString()) {
        return false;
    } else {
        return true;
    }
}

Ipv4Address Ipv4Address::Create(const std::string &ipv4, size_t prefixLength)
{
    if (!IsValidIPv4(ipv4) || prefixLength > MAX_IPV4_PREFIX_LENGTH - 1) {
        return INVALID_INET_ADDRESS;
    }
    return Ipv4Address(ipv4, prefixLength);
}

Ipv4Address Ipv4Address::Create(const std::string &ipv4, const std::string &mask)
{
    size_t prefixLength = 0;
    struct in_addr maskAddr = {INADDR_ANY};
    if ((inet_aton(mask.c_str(), &maskAddr) != 1) || (!IsValidIPv4(ipv4))) {
        return INVALID_INET_ADDRESS;
    }
    for (int i = 0; i < MAX_MASK_LENGTH; i++) {
        if (maskAddr.s_addr != 0) {
            maskAddr.s_addr = maskAddr.s_addr >> 1;
            ++prefixLength;
        }
    }
    return Ipv4Address(ipv4, prefixLength);
}

Ipv4Address Ipv4Address::Create(const in_addr &ipv4, const in_addr &mask)
{
    size_t prefixLength = 0;
    struct in_addr maskAddr = mask;
    for (int i = 0; i < MAX_MASK_LENGTH; i++) {
        if (maskAddr.s_addr != 0) {
            maskAddr.s_addr = maskAddr.s_addr >> 1;
            ++prefixLength;
        }
    }
    char ipStr[MAX_IPV4_STRING_LENGTH] = {0};
    if (inet_ntop(AF_INET, &ipv4, ipStr, sizeof(ipStr)) == nullptr) {
        return INVALID_INET_ADDRESS;
    }
    return Ipv4Address(ipStr, prefixLength);
}

Ipv4Address::Ipv4Address(const std::string &ipv4, size_t prefixLength)
    : BaseAddress(ipv4, prefixLength, FamilyType::FAMILY_INET)
{}

bool Ipv4Address::IsValid() const
{
    return IsValidIPv4(GetAddressWithString());
}

in_addr Ipv4Address::GetAddressWithInet() const
{
    struct in_addr ipv4Addr = {INADDR_ANY};
    if (inet_aton(GetAddressWithString().c_str(), &ipv4Addr) == 0) {
        LOGI("inet_aton error");
    }
    return ipv4Addr;
}

std::string Ipv4Address::GetMaskWithString() const
{
    char ipStr[MAX_IPV4_STRING_LENGTH] = {0};
    in_addr ipAddr = GetMaskWithInet();
    if (inet_ntop(AF_INET, &ipAddr, ipStr, sizeof(ipStr)) == nullptr) {
        return "";
    }
    return std::string(ipStr);
}

in_addr Ipv4Address::GetMaskWithInet() const
{
    struct in_addr mask4Addr = {INADDR_ANY};
    size_t num = GetAddressPrefixLength();
    size_t i = 0xffffffff;
    i = i >> (MAX_MASK_LENGTH - num);
    mask4Addr.s_addr = i;
    return mask4Addr;
}

std::string Ipv4Address::GetNetworkAddressWithString() const
{
    char ipStr[MAX_IPV4_STRING_LENGTH] = {0};
    in_addr ipAddr = GetNetworkAddressWithInet();
    if (inet_ntop(AF_INET, &ipAddr, ipStr, sizeof(ipStr)) == nullptr) {
        return "";
    }
    return std::string(ipStr);
}

in_addr Ipv4Address::GetNetworkAddressWithInet() const
{
    struct in_addr networkAddress = {INADDR_ANY};
    networkAddress.s_addr = GetMaskWithInet().s_addr & GetAddressWithInet().s_addr;
    return networkAddress;
}

std::string Ipv4Address::GetHostAddressWithString() const
{
    char ipStr[MAX_IPV4_STRING_LENGTH] = {0};
    in_addr ipAddr = GetHostAddressWithInet();
    if (inet_ntop(AF_INET, &ipAddr, ipStr, sizeof(ipStr)) == nullptr) {
        return "";
    }
    return std::string(ipStr);
}

in_addr Ipv4Address::GetHostAddressWithInet() const
{
    struct in_addr hostAddress = {INADDR_ANY};
    hostAddress.s_addr = (~(GetMaskWithInet().s_addr)) & GetAddressWithInet().s_addr;
    return hostAddress;
}

std::string Ipv4Address::GetNetwork() const
{
    std::string network = GetAddressWithString() + "/" + std::to_string(GetAddressPrefixLength());
    return network;
}
}  // namespace Wifi
}  // namespace OHOS