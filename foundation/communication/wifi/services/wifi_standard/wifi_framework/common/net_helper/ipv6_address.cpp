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
#include "ipv6_address.h"
#include <climits>
#include "securec.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "OHWIFI_UTIL_Ipv6Address"

namespace OHOS {
namespace Wifi {
constexpr int MAX_IPV6_PREFIX_LENGTH = 128;
constexpr int MAX_BIN = 2;
constexpr int BIN_TO_HEX = 4;
constexpr int IPV6_INTERVAL = 5;
constexpr int HALF_IN6ADDR = 8;
constexpr int MAX_DEC = 10;
constexpr int MAX_HEX = 16;
constexpr int BUFFER_SIZE = 32;
constexpr int HALF_PREFIX_LENGTH = 64;

const Ipv6Address Ipv6Address::INVALID_INET6_ADDRESS(std::string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"),
    MAX_IPV6_PREFIX_LENGTH);

bool Ipv6Address::IsValidIPv6(const std::string &ipv6)
{
    struct in6_addr ipv6Addr = IN6ADDR_ANY_INIT;
    if (inet_pton(AF_INET6, ipv6.c_str(), &ipv6Addr) != 1 ||
        ipv6 == INVALID_INET6_ADDRESS.GetAddressWithString()) {
        return false;
    } else {
        return true;
    }
}

bool Ipv6Address::IsAddrLocallink(const struct in6_addr &ip6Addr)
{
    return IN6_IS_ADDR_LINKLOCAL(&ip6Addr);
}

Ipv6Address Ipv6Address::Create(std::string ipv6)
{
    std::transform(ipv6.begin(), ipv6.end(), ipv6.begin(), ::tolower);
    std::size_t index = ipv6.find("/");
    std::string ip;
    size_t prefixLength = 0;

    if (index == std::string::npos) {
        ip = ipv6;
        prefixLength = MAX_IPV6_PREFIX_LENGTH;
    } else {
        ip = std::string(ipv6.begin(), ipv6.begin() + index);
        prefixLength = std::stoi(ipv6.substr(index + 1));
    }

    if (!IsValidIPv6(ip)) {
        return INVALID_INET6_ADDRESS;
    }
    return {ip, prefixLength};
}

Ipv6Address Ipv6Address::Create(const std::string &ipv6Prefix, MacAddress &mac, const size_t prefixLength)
{
    if (prefixLength > HALF_PREFIX_LENGTH || !IsValidIPv6(ipv6Prefix)) {
        return INVALID_INET6_ADDRESS;
    }
    std::string eui64Addr = std::string("::") + MacToEui64addr(mac);
    struct in6_addr eui64 = IN6ADDR_ANY_INIT;
    inet_pton(AF_INET6, eui64Addr.c_str(), &eui64);
    struct in6_addr ipv6Buf = IN6ADDR_ANY_INIT;
    inet_pton(AF_INET6, ipv6Prefix.c_str(), &ipv6Buf);

    struct in6_addr ipv6 = GetIpv6Prefix(ipv6Buf, prefixLength);
    for (int n = HALF_IN6ADDR; n < MAX_IPV6_PREFIX_LENGTH / CHAR_BIT; ++n) {
        ipv6.s6_addr[n] = eui64.s6_addr[n];
    }
    char ip[INET6_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET6, &ipv6, ip, INET6_ADDRSTRLEN);
    return Ipv6Address(ip, prefixLength);
}

Ipv6Address Ipv6Address::Create(const std::string &ipv6Prefix, const size_t prefixLength, unsigned int rndSeed)
{
    if (prefixLength > MAX_IPV6_PREFIX_LENGTH || !IsValidIPv6(ipv6Prefix)) {
        return INVALID_INET6_ADDRESS;
    }
    LOGD("Create use rand seed, %{public}u", rndSeed);
    std::string rndAddr = GetRandomAddr(ipv6Prefix, prefixLength);
    return Ipv6Address(rndAddr, prefixLength);
}

Ipv6Address Ipv6Address::Create(const struct in6_addr &i6Addr, const size_t prefixLength)
{
    if (prefixLength > MAX_IPV6_PREFIX_LENGTH) {
        return INVALID_INET6_ADDRESS;
    }
    char ipv6Buf[INET6_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET6, &i6Addr, ipv6Buf, INET6_ADDRSTRLEN) == nullptr) {
        return INVALID_INET6_ADDRESS;
    }
    return Ipv6Address(ipv6Buf, prefixLength);
}

Ipv6Address Ipv6Address::Create(std::string ipv6, const size_t prefixLength)
{
    if ((prefixLength > MAX_IPV6_PREFIX_LENGTH) || (!IsValidIPv6(ipv6))) {
        return INVALID_INET6_ADDRESS;
    }
    std::transform(ipv6.begin(), ipv6.end(), ipv6.begin(), ::tolower);
    return Ipv6Address(ipv6, prefixLength);
}

Ipv6Address Ipv6Address::Create(std::string ipv6, const std::string &mask)
{
    if (!IsValidIPv6(ipv6)) {
        return INVALID_INET6_ADDRESS;
    }
    std::transform(ipv6.begin(), ipv6.end(), ipv6.begin(), ::tolower);
    std::string copyMask = mask;
    unsigned int n = 0;
    while (n < copyMask.size()) {
        if (copyMask.at(n) == ':') {
            copyMask.erase(n, 1);
        } else {
            ++n;
        }
    }

    std::string maskBin;
    std::string binBuf;
    for (unsigned int n = 0; n < copyMask.size(); ++n) {
        binBuf = copyMask.substr(n, 1);
        maskBin += HexToBin(binBuf);
    }

    size_t count = 0;
    for (unsigned int n = 0; n < maskBin.size(); ++n) {
        if (maskBin.at(n) == '1') {
            ++count;
        }
    }
    return Ipv6Address(ipv6, count);
}

Ipv6Address::Ipv6Address(const std::string &ipv6, const size_t prefixLength)
    : BaseAddress(ipv6, prefixLength, FamilyType::FAMILY_INET6)
{}

bool Ipv6Address::IsValid() const
{
    return IsValidIPv6(GetAddressWithString());
}

struct in6_addr Ipv6Address::GetIn6Addr() const
{
    struct in6_addr ipv6Addr = IN6ADDR_ANY_INIT;
    inet_pton(AF_INET6, GetAddressWithString().c_str(), &ipv6Addr);
    return ipv6Addr;
}

std::string Ipv6Address::GetPrefix() const
{
    struct in6_addr ipv6Prefix = IN6ADDR_ANY_INIT;
    struct in6_addr ipv6AddrBuf = GetIn6Addr();
    ipv6Prefix = GetIpv6Prefix(ipv6AddrBuf, GetAddressPrefixLength());
    char ipv6PrefixBuf[INET6_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET6, &ipv6Prefix, ipv6PrefixBuf, INET6_ADDRSTRLEN);
    std::string prefixStr = ipv6PrefixBuf;
    return prefixStr;
}

struct in6_addr Ipv6Address::GetIpv6Prefix(struct in6_addr &ip6Addr, size_t prefixLength)
{
    struct in6_addr addr = IN6ADDR_ANY_INIT;
    if (prefixLength > MAX_IPV6_PREFIX_LENGTH) {
        return addr;
    }
    char buf[INET6_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET6, &ip6Addr, buf, INET6_ADDRSTRLEN) == nullptr) {
        return addr;
    }

    unsigned int o = prefixLength / CHAR_BIT;
    unsigned int b = prefixLength & 0x7;
    if (memset_s(addr.s6_addr, sizeof(addr.s6_addr), 0, sizeof(addr.s6_addr)) != EOK ||
        memcpy_s(addr.s6_addr, sizeof(addr.s6_addr), &ip6Addr, o) != EOK) {
        return IN6ADDR_ANY_INIT;
    }
    if ((b != 0) && (o < INET_ADDRSTRLEN)) {
        addr.s6_addr[o] = ip6Addr.s6_addr[o] & (0xff00 >> b);
    }
    return addr;
}

struct in6_addr Ipv6Address::GetIpv6Mask(size_t prefixLength)
{
    struct in6_addr addr = IN6ADDR_ANY_INIT;
    if (prefixLength > MAX_IPV6_PREFIX_LENGTH) {
        return addr;
    } else if (prefixLength == MAX_IPV6_PREFIX_LENGTH) {
        for (unsigned int count = 0; count < prefixLength / CHAR_BIT; ++count) {
            addr.s6_addr[count] = UCHAR_MAX;
        }
        return addr;
    }

    unsigned int count = 0;
    for (; count < prefixLength / CHAR_BIT; ++count) {
        addr.s6_addr[count] = UCHAR_MAX;
    }
    addr.s6_addr[count] = 0;
    for (int n = CHAR_BIT - prefixLength % CHAR_BIT; n < CHAR_BIT; ++n) {
        addr.s6_addr[count] |= (0x01 << n);
    }
    ++count;
    for (; count < MAX_IPV6_PREFIX_LENGTH / CHAR_BIT; ++count) {
        addr.s6_addr[count] = 0;
    }
    return addr;
}

std::string Ipv6Address::GetRandomAddr(const std::string &ipv6Prefix, int prefixLength)
{
    int rndLength = MAX_IPV6_PREFIX_LENGTH - prefixLength;
    std::random_device rd;
    std::string resBin;
    int rndnum = 0;
    for (int n = 0; n < rndLength; ++n) {
        rndnum = std::abs(static_cast<int>(rd()));
        switch (rndnum % MAX_BIN) {
            case 0:
                resBin += "0";
                break;
            default:
                resBin += "1";
                break;
        }
    }
    for (int n = rndLength; n < MAX_IPV6_PREFIX_LENGTH; ++n) {
        resBin += "0";
    }
    std::reverse(resBin.begin(), resBin.end());

    std::string resHex;
    std::string bin2HexBuf;
    for (unsigned int n = 0; n < resBin.size(); n += BIN_TO_HEX) {
        bin2HexBuf = resBin.substr(n, BIN_TO_HEX);
        resHex += BinToHex(bin2HexBuf);
    }
    for (unsigned int n = BIN_TO_HEX; n < resHex.size(); n += IPV6_INTERVAL) {
        resHex.insert(n, ":");
    }

    struct in6_addr rndAddr = IN6ADDR_ANY_INIT;
    inet_pton(AF_INET6, resHex.c_str(), &rndAddr);
    struct in6_addr ipv6Addr = IN6ADDR_ANY_INIT;
    inet_pton(AF_INET6, ipv6Prefix.c_str(), &ipv6Addr);
    ipv6Addr.s6_addr[prefixLength / CHAR_BIT] |= rndAddr.s6_addr[prefixLength / CHAR_BIT];
    for (int n = prefixLength / CHAR_BIT + 1; n < MAX_IPV6_PREFIX_LENGTH / CHAR_BIT; ++n) {
        ipv6Addr.s6_addr[n] = rndAddr.s6_addr[n];
    }
    char ipv6Buf[INET6_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET6, &ipv6Addr, ipv6Buf, INET6_ADDRSTRLEN);
    std::string res = ipv6Buf;
    return res;
}

std::string Ipv6Address::GetNetwork() const
{
    std::string ret = GetPrefix();
    ret += "/";
    ret += std::to_string(GetAddressPrefixLength());
    return ret;
}

std::string Ipv6Address::BinToHex(const std::string &strBin)
{
    int addrDec = std::stoi(strBin, nullptr, MAX_BIN);
    std::string addrHex;
    char buf[2] = {0};
    if (addrDec % MAX_HEX < MAX_DEC) {
        buf[0] = addrDec % MAX_HEX + '0';
        buf[1] = '\0';
    } else {
        buf[0] = addrDec % MAX_HEX - MAX_DEC + 'a';
        buf[1] = '\0';
    }
    addrHex = buf;
    return addrHex;
}

std::string Ipv6Address::HexToBin(const std::string &strHex)
{
    int addrDec = std::stoi(strHex, nullptr, MAX_HEX);
    std::string addrBin;
    for (int n = addrDec; n; n = n / MAX_BIN) {
        addrBin += ((n % MAX_BIN) ? "1" : "0");
    }
    std::reverse(addrBin.begin(), addrBin.end());
    return addrBin;
}

std::string Ipv6Address::MacToEui64addr(MacAddress &mac)
{
    struct sockaddr macSockaddr = mac.GetMacAddressWifiSockaddr();

    unsigned char eui64Sa[HALF_IN6ADDR] = {0};
    char eui64Addr[BUFFER_SIZE] = {0};

    eui64Sa[0] = macSockaddr.sa_data[0] | ((macSockaddr.sa_data[0] & 0x2) ^ 0x2);
    eui64Sa[1] = macSockaddr.sa_data[1];
    eui64Sa[TWO] = macSockaddr.sa_data[TWO];
    eui64Sa[THREE] = 0xFF;
    eui64Sa[FOUR] = 0xFE;
    eui64Sa[FIVE] = macSockaddr.sa_data[THREE];
    eui64Sa[SIX] = macSockaddr.sa_data[FOUR];
    eui64Sa[SEVEN] = macSockaddr.sa_data[FIVE];

    if (snprintf_s(eui64Addr,
        sizeof(eui64Addr),
        sizeof(eui64Addr) - 1,
        "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
        eui64Sa[0],
        eui64Sa[1],
        eui64Sa[TWO],
        eui64Sa[THREE],
        eui64Sa[FOUR],
        eui64Sa[FIVE],
        eui64Sa[SIX],
        eui64Sa[SEVEN]) < 0) {
        return std::string("");
    }
    return std::string(eui64Addr);
}
} // namespace WiFi
} // namespace OHOS
