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

#include "address_utils.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <securec.h>
#include "dhcp_logger.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServerAddressUtil"

#define IPV4_ADDRESS_BITS 32
#define BIT_MAX_VALUE 2
#define IP_ADDRESS_LENGTH 4
#define MAD_ADDR_BUF_SIZE 50
#define MAC_STRING_SIZE 17
#define IP_ADDRESS_STRING_LENGTH 64

enum MacAddressIndex {
    MAI_ZERO = 0,
    MAI_ONE = 1,
    MAI_TWO = 2,
    MAI_THREE = 3,
    MAI_FOUR = 4,
    MAI_FIVE = 5
};

uint32_t NetworkAddress(uint32_t ip, uint32_t netmask)
{
    return (ip & netmask);
}

uint32_t FirstIpAddress(uint32_t ip, uint32_t netmask)
{
    uint32_t network = NetworkAddress(ip, netmask);
    uint32_t firstIp = htonl(network) + 1;
    return htonl(firstIp);
}

uint32_t NextIpAddress(uint32_t currIp, uint32_t netmask, uint32_t offset)
{
    uint32_t network = NetworkAddress(currIp, netmask);
    uint32_t broadcast = BroadCastAddress(currIp, netmask);
    uint32_t lastIp = LastIpAddress(currIp, netmask);
    uint32_t hostTotal = HostTotal(netmask);
    uint32_t next = offset;
    if (currIp == lastIp || currIp == broadcast) {
        return FirstIpAddress(currIp, netmask);
    }
    if (next > hostTotal && hostTotal > 0) {
        next = next % hostTotal;
    }
    uint32_t nextIp = htonl(currIp) + next + 1;
    if (next && nextIp > htonl(lastIp)) {
        nextIp = htonl(network) + (nextIp - htonl(lastIp));
    }
    return htonl(nextIp);
}

uint32_t FirstNetIpAddress(uint32_t network)
{
    uint32_t firstIp = htonl(network) + 1;
    return htonl(firstIp);
}

uint32_t LastIpAddress(uint32_t ip, uint32_t netmask)
{
    uint32_t network = NetworkAddress(ip, netmask);
    uint32_t lastIp = htonl(network) + HostTotal(netmask);
    if (lastIp) {
        lastIp -= 1;
    }
    return htonl(lastIp);
}

int IpInNetwork(uint32_t ip, uint32_t network, uint32_t netmask)
{
    uint32_t firstNet = NetworkAddress(ip, netmask);
    uint32_t secondNet = NetworkAddress(network, netmask);

    uint32_t beginIp = FirstIpAddress(network, netmask);
    uint32_t broadCast = BroadCastAddress(network, netmask);

    if (firstNet == secondNet) {
        if (ip >= beginIp && ip <= broadCast) {
            return DHCP_TRUE;
        }
    }
    return DHCP_FALSE;
}

int IpInRange(uint32_t ip, uint32_t beginIp, uint32_t endIp, uint32_t netmask)
{
    uint32_t network = NetworkAddress(ip, netmask);
    uint32_t firstNet = NetworkAddress(beginIp, netmask);
    uint32_t secondNet = NetworkAddress(endIp, netmask);
    if (network != firstNet || firstNet != secondNet) {
        return 0;
    }
    if (ip >= beginIp && ip <= endIp) {
        return DHCP_TRUE;
    }
    return DHCP_FALSE;
}

uint32_t BroadCastAddress(uint32_t ip, uint32_t netmask)
{
    uint32_t network = NetworkAddress(ip, netmask);
    uint32_t broadcast = htonl(network) + HostTotal(netmask);
    return htonl(broadcast);
}

int NetworkBits(uint32_t netmask)
{
    int bits = 0;
    uint32_t net = htonl(netmask);
    for (size_t i = 0; i < IPV4_ADDRESS_BITS; i++) {
        if (net == 0) {
            break;
        }
        bits++;
        net <<= 1;
    }
    return bits;
}

int HostBits(uint32_t netmask)
{
    int bits = 0;
    uint32_t net = htonl(netmask);
    for (int i = IPV4_ADDRESS_BITS; i > 0; --i) {
        bits++;
        net >>= 1;
        if ((net & 1) != 0) {
            break;
        }
    }
    return bits;
}

int HostTotal(uint32_t netmask)
{
    int hostBits = HostBits(netmask);
    int total = 1;
    for (size_t i = 0; i < (size_t)hostBits; i++) {
        total *= BIT_MAX_VALUE;
    }
    total--;
    return total;
}

uint32_t ParseIpAddr(const char *strIp)
{
    struct in_addr inAddr;
    uint32_t ip = 0;
    int ret = inet_aton(strIp, &inAddr);
    if (ret != 0) {
        if (memcpy_s(&ip, sizeof(uint32_t), &inAddr, sizeof(struct in_addr)) != EOK) {
            return 0;
        }
        return ip;
    }
    return 0;
}

uint32_t ParseIpHtonl(const char *strIp)
{
    uint32_t ip = ParseIpAddr(strIp);
    return htonl(ip);
}

uint32_t ParseIp(const uint8_t *ipAddr)
{
    uint32_t ip = 0;
    if (memcpy_s(&ip, IP_ADDRESS_LENGTH, ipAddr, IP_ADDRESS_LENGTH) != EOK) {
        return 0;
    }
    return ip;
}

const char *ParseStrIp(uint32_t ipAddr)
{
    static char strIpAddr[IP_ADDRESS_STRING_LENGTH] = {0};
    struct in_addr inAddr;
    if (memcpy_s(&inAddr, sizeof(inAddr), &ipAddr, sizeof(ipAddr)) != EOK ||
        memset_s(strIpAddr, sizeof(strIpAddr), 0, sizeof(strIpAddr)) != EOK) {
        return "0.0.0.0";
    }
    if (inet_ntop(AF_INET, &inAddr, strIpAddr, sizeof(strIpAddr)) == NULL) {
        return "0.0.0.0";
    }
    return strIpAddr;
}

char *ParseStrMac(const uint8_t *macAddr, size_t addrSize)
{
    static char strMacAddr[MAD_ADDR_BUF_SIZE] = {0};
    if (!macAddr || addrSize < MAC_ADDR_LENGTH) {
        return 0;
    }
    if (memset_s(strMacAddr, MAD_ADDR_BUF_SIZE, '\0', sizeof(strMacAddr)) != EOK ||
        sprintf_s(strMacAddr, MAD_ADDR_BUF_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x", macAddr[MAI_ZERO],
            macAddr[MAI_ONE], macAddr[MAI_TWO], macAddr[MAI_THREE], macAddr[MAI_FOUR], macAddr[MAI_FIVE]) < 0) {
        return 0;
    }
    return strMacAddr;
}

static int8_t IsValidHexCharAndConvert(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + ('9' - '0' + 1);
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + ('9' - '0' + 1);
    }
    return -1;
}

int ParseMacAddress(const char *strMac, uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (strMac == NULL || strlen(strMac) != MAC_STRING_SIZE) {
        return DHCP_FALSE;
    }
    size_t len = strlen(strMac);
    const int shiftNum = 4;
    const int macSpaceNum = 3;
    unsigned char tmp = 0;
    for (size_t i = 0, j = 0; i < len; ++i) {
        if (j == 0 || j == 1) {
            int8_t v = IsValidHexCharAndConvert(strMac[i]);
            if (v < 0) {
                return 0;
            }
            tmp <<= shiftNum;
            tmp |= v;
            ++j;
        } else {
            if (strMac[i] != ':') {
                return 0;
            }
            macAddr[i / macSpaceNum] = tmp;
            j = 0;
            tmp = 0;
        }
    }
    macAddr[MAC_STRING_SIZE / macSpaceNum] = tmp;
    return DHCP_TRUE;
}

uint32_t HostToNetwork(uint32_t host)
{
    return htonl(host);
}

uint32_t NetworkToHost(uint32_t network)
{
    return ntohl(network);
}

char *ParseLogMac(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    static char strLogMacAddr[MAD_ADDR_BUF_SIZE] = {0};
    if (!macAddr) {
        return 0;
    }
    if (memset_s(strLogMacAddr, MAD_ADDR_BUF_SIZE, '\0', MAD_ADDR_BUF_SIZE) != EOK ||
        sprintf_s(strLogMacAddr, MAD_ADDR_BUF_SIZE, "??:%02x:??:??:%02x:%02x", macAddr[NUM_ONE], macAddr[MAI_FOUR],
            macAddr[MAI_FIVE]) < 0) {
        return 0;
    }
    return strLogMacAddr;
}

int IsEmptyHWAddr(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    for (int i = 0; i < MAC_ADDR_LENGTH; i++) {
        if (macAddr[i] != 0) {
            return DHCP_FALSE;
        }
    }
    return DHCP_TRUE;
}

int AddrEquels(uint8_t firstAddr[DHCP_HWADDR_LENGTH], uint8_t secondAddr[DHCP_HWADDR_LENGTH], int addrLength)
{
    int len = addrLength;
    if (len > DHCP_HWADDR_LENGTH) {
        len = DHCP_HWADDR_LENGTH;
    }
    for (int i = 0; i < len; i++) {
        if ((firstAddr[i] != secondAddr[i])) {
            return DHCP_FALSE;
        }
    }
    return DHCP_TRUE;
}