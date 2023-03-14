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
#include "dhcp_socket.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "securec.h"
#include "dhcp_client.h"

#undef LOG_TAG
#define LOG_TAG "WifiDhcpSocket"


static uint16_t GetCheckSum(uint16_t *pData, int nBytes)
{
    uint32_t uTotalSum = 0;

    /* Calculates the network checksum by 2 bytes. */
    while (nBytes >= DHCP_UINT16_BYTES)  {
        uTotalSum += *pData++;
        nBytes -= DHCP_UINT16_BYTES;
    }
    /* Calculate the network checksum based on the remaining bytes. */
    if (nBytes > 0) {
        uint16_t u16Sum;
        *(uint8_t *)(&u16Sum) = *(uint8_t *)pData;
        uTotalSum += u16Sum;
    }
    /* Checksum conversion from 32-bit to 16-bit. */
    while (uTotalSum >> DHCP_UINT16_BITS) {
        uTotalSum = (uTotalSum & 0xffff) + (uTotalSum >> DHCP_UINT16_BITS);
    }

    return (uint16_t)(~uTotalSum);
}

/* Raw socket can receive data frames or data packets from the local network interface. */
int CreateRawSocket(int *rawFd)
{
    int sockFd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
    if (sockFd == -1) {
        LOGE("CreateRawSocket() failed, socket error:%{public}d.", errno);
        return SOCKET_OPT_FAILED;
    }
    *rawFd = sockFd;
    return SOCKET_OPT_SUCCESS;
}

/* Kernel socket can receive data frames or data packets from the local network interface, ip and port. */
int CreateKernelSocket(int *sockFd)
{
    if (sockFd == NULL) {
        LOGE("CreateKernelSocket() failed, sockFd is NULL!");
        return SOCKET_OPT_FAILED;
    }
    int nFd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (nFd == -1) {
        LOGE("CreateKernelSocket() failed, socket error:%{public}d.", errno);
        return SOCKET_OPT_FAILED;
    }
    *sockFd = nFd;
    return SOCKET_OPT_SUCCESS;
}

int BindRawSocket(const int rawFd, const int ifaceIndex, const uint8_t *ifaceAddr)
{
    if (rawFd < 0) {
        LOGE("BindRawSocket() failed, rawFd:%{public}d error!", rawFd);
        return SOCKET_OPT_FAILED;
    }

    struct sockaddr_ll rawAddr;
    if (memset_s(&rawAddr, sizeof(rawAddr), 0, sizeof(rawAddr)) != EOK) {
        LOGE("BindRawSocket() failed, memset_s rawAddr error!");
        close(rawFd);
        return SOCKET_OPT_FAILED;
    }
    rawAddr.sll_ifindex = ifaceIndex;
    rawAddr.sll_protocol = htons(ETH_P_IP);
    rawAddr.sll_family = AF_PACKET;
    if (ifaceAddr != NULL) {
        rawAddr.sll_halen = MAC_ADDR_LEN;
        if (memcpy_s(rawAddr.sll_addr, sizeof(rawAddr.sll_addr), ifaceAddr, MAC_ADDR_LEN) != EOK) {
            LOGE("BindRawSocket() failed, memcpy_s rawAddr.sll_addr error!");
            close(rawFd);
            return SOCKET_OPT_FAILED;
        }
    }
    int nRet = bind(rawFd, (struct sockaddr *)&rawAddr, sizeof(rawAddr));
    if (nRet == -1) {
        LOGE("BindRawSocket() index:%{public}d failed, bind error:%{public}d.", ifaceIndex, errno);
        close(rawFd);
        return SOCKET_OPT_FAILED;
    }

    return SOCKET_OPT_SUCCESS;
}

int BindKernelSocket(const int sockFd, const char *ifaceName, const uint32_t sockIp, const int sockPort, bool bCast)
{
    if (sockFd < 0) {
        LOGE("BindKernelSocket() failed, sockFd:%{public}d error!", sockFd);
        return SOCKET_OPT_FAILED;
    }

    /* Bind the specified interface. */
    if (ifaceName != NULL) {
        struct ifreq ifaceReq;
        if (strncpy_s(ifaceReq.ifr_name, sizeof(ifaceReq.ifr_name), ifaceName, strlen(ifaceName)) != EOK) {
            close(sockFd);
            return SOCKET_OPT_FAILED;
        }
        if (setsockopt(sockFd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifaceReq, sizeof(ifaceReq)) == -1) {
            LOGE("BindKernelSocket() %{public}s SO_BINDTODEVICE error:%{public}d.", ifaceName, errno);
            close(sockFd);
            return SOCKET_OPT_FAILED;
        }
    }

    /* Set the broadcast feature of the data sent by the socket. */
    if (bCast) {
        if (setsockopt(sockFd, SOL_SOCKET, SO_BROADCAST, (const char *)&bCast, sizeof(bool)) == -1) {
            LOGE("BindKernelSocket() sockFd:%{public}d SO_BROADCAST error:%{public}d.", sockFd, errno);
            close(sockFd);
            return SOCKET_OPT_FAILED;
        }
    }

    /* Allow multiple sockets to use the same port number. */
    int bReuseaddr = 1;
    if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReuseaddr, sizeof(bReuseaddr)) == -1) {
        LOGE("BindKernelSocket() sockFd:%{public}d SO_REUSEADDR error:%{public}d.", sockFd, errno);
        close(sockFd);
        return SOCKET_OPT_FAILED;
    }

    struct sockaddr_in kernelAddr;
    if (memset_s(&kernelAddr, sizeof(kernelAddr), 0, sizeof(kernelAddr)) != EOK) {
        close(sockFd);
        return SOCKET_OPT_FAILED;
    }
    kernelAddr.sin_addr.s_addr = sockIp;
    kernelAddr.sin_port = htons(sockPort);
    kernelAddr.sin_family = AF_INET;
    int nRet = bind(sockFd, (struct sockaddr *)&kernelAddr, sizeof(kernelAddr));
    if (nRet == -1) {
        LOGE("BindKernelSocket() sockFd:%{public}d failed, bind error:%{public}d.", sockFd, errno);
        close(sockFd);
        return SOCKET_OPT_FAILED;
    }

    return SOCKET_OPT_SUCCESS;
}

int SendToDhcpPacket(
    const struct DhcpPacket *sendPacket, uint32_t srcIp, uint32_t destIp, int destIndex, const uint8_t *destHwaddr)
{
    int nFd = -1;
    if (CreateRawSocket(&nFd) != SOCKET_OPT_SUCCESS) {
        return SOCKET_OPT_FAILED;
    }

    struct sockaddr_ll rawAddr;
    if ((memset_s(&rawAddr, sizeof(rawAddr), 0, sizeof(rawAddr)) != EOK) ||
        (memcpy_s(rawAddr.sll_addr, sizeof(rawAddr.sll_addr), destHwaddr, MAC_ADDR_LEN) != EOK)) {
        close(nFd);
        return SOCKET_OPT_FAILED;
    }
    rawAddr.sll_ifindex = destIndex;
    rawAddr.sll_protocol = htons(ETH_P_IP);
    rawAddr.sll_family = AF_PACKET;
    rawAddr.sll_halen = MAC_ADDR_LEN;
    if (bind(nFd, (struct sockaddr *)&rawAddr, sizeof(rawAddr)) == -1) {
        close(nFd);
        return SOCKET_OPT_FAILED;
    }

    /* Filling the structure information. */
    struct UdpDhcpPacket udpPackets;
    if (memset_s(&udpPackets, sizeof(udpPackets), 0, sizeof(udpPackets)) != EOK) {
        close(nFd);
        return SOCKET_OPT_FAILED;
    }
    udpPackets.udp.source = htons(BOOTP_CLIENT);
    udpPackets.udp.dest = htons(BOOTP_SERVER);
    udpPackets.udp.len = htons(sizeof(udpPackets.udp) + sizeof(struct DhcpPacket));
    udpPackets.ip.tot_len = udpPackets.udp.len;
    udpPackets.ip.protocol = IPPROTO_UDP;
    udpPackets.ip.saddr = srcIp;
    udpPackets.ip.daddr = destIp;
    if (memcpy_s(&(udpPackets.data), sizeof(struct DhcpPacket), sendPacket, sizeof(struct DhcpPacket)) != EOK) {
        close(nFd);
        return SOCKET_OPT_FAILED;
    }
    udpPackets.udp.check = GetCheckSum((uint16_t *)&udpPackets, sizeof(struct UdpDhcpPacket));
    udpPackets.ip.ihl = sizeof(udpPackets.ip) >> DHCP_UINT16_BYTES;
    udpPackets.ip.version = IPVERSION;
    udpPackets.ip.tot_len = htons(sizeof(struct UdpDhcpPacket));
    udpPackets.ip.ttl = IPDEFTTL;
    udpPackets.ip.check = GetCheckSum((uint16_t *)&(udpPackets.ip), sizeof(udpPackets.ip));

    ssize_t nBytes = sendto(nFd, &udpPackets, sizeof(udpPackets), 0, (struct sockaddr *)&rawAddr, sizeof(rawAddr));
    if (nBytes <= 0) {
        LOGE("SendToDhcpPacket() fd:%{public}d failed, sendto error:%{public}d.", nFd, errno);
    } else {
        LOGI("SendToDhcpPacket() fd:%{public}d, index:%{public}d, bytes:%{public}d.", nFd, destIndex, (int)nBytes);
    }
    close(nFd);
    return (nBytes <= 0) ? SOCKET_OPT_FAILED : SOCKET_OPT_SUCCESS;
}

int SendDhcpPacket(struct DhcpPacket *sendPacket, uint32_t srcIp, uint32_t destIp)
{
    int nFd = -1;
    if ((CreateKernelSocket(&nFd) != SOCKET_OPT_SUCCESS) ||
        (BindKernelSocket(nFd, NULL, srcIp, BOOTP_CLIENT, false) != SOCKET_OPT_SUCCESS)) {
        LOGE("SendDhcpPacket() fd:%{public}d,srcIp:%{private}u failed!", nFd, srcIp);
        return SOCKET_OPT_FAILED;
    }

    struct sockaddr_in kernelAddr;
    if (memset_s(&kernelAddr, sizeof(kernelAddr), 0, sizeof(kernelAddr)) != EOK) {
        close(nFd);
        return SOCKET_OPT_FAILED;
    }
    kernelAddr.sin_addr.s_addr = destIp;
    kernelAddr.sin_port = htons(BOOTP_SERVER);
    kernelAddr.sin_family = AF_INET;
    int nRet = connect(nFd, (struct sockaddr *)&kernelAddr, sizeof(kernelAddr));
    if (nRet == -1) {
        LOGE("SendDhcpPacket() nFd:%{public}d failed, connect error:%{public}d.", nFd, errno);
        close(nFd);
        return SOCKET_OPT_FAILED;
    }

    ssize_t nBytes = write(nFd, sendPacket, sizeof(struct DhcpPacket));
    if (nBytes <= 0) {
        LOGE("SendDhcpPacket() fd:%{public}d failed, write error:%{public}d.", nFd, errno);
    } else {
        LOGI("SendDhcpPacket() fd:%{public}d, srcIp:%{private}u, bytes:%{public}d.", nFd, srcIp, (int)nBytes);
    }
    close(nFd);
    return (nBytes <= 0) ? SOCKET_OPT_FAILED : SOCKET_OPT_SUCCESS;
}

int CheckReadBytes(const int count, const int totLen)
{
    if (count < 0) {
        LOGE("CheckReadBytes() couldn't read on raw listening socket, count:%{public}d, error:%{public}d!",
            count, errno);
        /* The specified network interface service may be down. */
        sleep(NUMBER_ONE);
        return SOCKET_OPT_ERROR;
    }

    int nCommonSize = sizeof(struct iphdr) + sizeof(struct udphdr);
    if (count < nCommonSize) {
        LOGE("CheckReadBytes() read size:%{public}d less than common size:%{public}d!", count, nCommonSize);
        return SOCKET_OPT_FAILED;
    }

    if (count < totLen) {
        LOGE("CheckReadBytes() count:%{public}d less than totLen:%{public}d, packet is Truncated!", count, totLen);
        return SOCKET_OPT_FAILED;
    }

    LOGI("CheckReadBytes() count:%{public}d, tot:%{public}d, common:%{public}d.", count, totLen, nCommonSize);
    return SOCKET_OPT_SUCCESS;
}

int CheckUdpPacket(struct UdpDhcpPacket *pPacket, const int totLen)
{
    if (pPacket == NULL) {
        LOGE("CheckUdpPacket() failed, pPacket == NULL!");
        return SOCKET_OPT_FAILED;
    }

    if (totLen > (int)sizeof(struct UdpDhcpPacket)) {
        LOGE("CheckUdpPacket() totLen:%{public}d more than %{public}d!", totLen, (int)sizeof(struct UdpDhcpPacket));
        return SOCKET_OPT_FAILED;
    }

    if ((pPacket->ip.protocol != IPPROTO_UDP) || (pPacket->ip.version != IPVERSION)) {
        LOGE("CheckUdpPacket() failed, pPacket->ip.protocol:%{public}d or version:%{public}u error!",
            pPacket->ip.protocol, pPacket->ip.version);
        return SOCKET_OPT_FAILED;
    }

    uint32_t uIhl = (uint32_t)(sizeof(pPacket->ip) >> DHCP_UINT16_BYTES);
    if (pPacket->ip.ihl != uIhl) {
        LOGE("CheckUdpPacket() failed, pPacket->ip.ihl:%{public}u error, uIhl:%{public}u!", pPacket->ip.ihl, uIhl);
        return SOCKET_OPT_FAILED;
    }

    if (pPacket->udp.dest != htons(BOOTP_CLIENT)) {
        LOGE("CheckUdpPacket() failed, pPacket->udp.dest:%{public}d error, htons:%{public}d!",
            pPacket->udp.dest, htons(BOOTP_CLIENT));
        return SOCKET_OPT_FAILED;
    }

    uint16_t uLen = (uint16_t)(totLen - (int)sizeof(pPacket->ip));
    if (ntohs(pPacket->udp.len) != uLen) {
        LOGE("CheckUdpPacket() failed, pPacket->udp.len:%{public}d error, uLen:%{public}d!", pPacket->udp.len, uLen);
        return SOCKET_OPT_FAILED;
    }
    LOGI("CheckUdpPacket() success, totLen:%{public}d.", totLen);
    return SOCKET_OPT_SUCCESS;
}

int CheckPacketIpSum(struct UdpDhcpPacket *pPacket, const int bytes)
{
    if (pPacket == NULL) {
        return SOCKET_OPT_FAILED;
    }

    if (CheckUdpPacket(pPacket, bytes) != SOCKET_OPT_SUCCESS) {
        usleep(SLEEP_TIME_500_MS);
        return SOCKET_OPT_FAILED;
    }

    /* Check packet ip sum. */
    uint16_t uCheck = pPacket->ip.check;
    pPacket->ip.check = 0;
    uint16_t uCheckSum = GetCheckSum((uint16_t *)&(pPacket->ip), sizeof(pPacket->ip));
    if (uCheck != uCheckSum) {
        LOGE("CheckPacketIpSum() failed, ip.check:%{public}d, uCheckSum:%{public}d!", uCheck, uCheckSum);
        return SOCKET_OPT_ERROR;
    }
    LOGI("CheckPacketIpSum() success, bytes:%{public}d.", bytes);
    return SOCKET_OPT_SUCCESS;
}

int CheckPacketUdpSum(struct UdpDhcpPacket *pPacket, const int bytes)
{
    if (pPacket == NULL) {
        LOGE("CheckPacketUdpSum() failed, pPacket == NULL!");
        return SOCKET_OPT_FAILED;
    }

    /* Check packet udp sum. */
    uint16_t uCheck = pPacket->udp.check;
    pPacket->udp.check = 0;
    u_int32_t source = pPacket->ip.saddr;
    u_int32_t dest = pPacket->ip.daddr;
    if (memset_s(&pPacket->ip, sizeof(pPacket->ip), 0, sizeof(pPacket->ip)) != EOK) {
        LOGE("CheckPacketUdpSum() failed, memset_s ERROR!");
        return SOCKET_OPT_FAILED;
    }
    pPacket->ip.protocol = IPPROTO_UDP;
    pPacket->ip.saddr = source;
    pPacket->ip.daddr = dest;
    pPacket->ip.tot_len = pPacket->udp.len;
    uint16_t uCheckSum = GetCheckSum((uint16_t *)pPacket, bytes);
    if (uCheck && (uCheck != uCheckSum)) {
        LOGE("CheckPacketUdpSum() failed, udp.check:%{public}d, uCheckSum:%{public}d!", uCheck, uCheckSum);
        return SOCKET_OPT_FAILED;
    }
    LOGI("CheckPacketUdpSum() success, bytes:%{public}d.", bytes);
    return SOCKET_OPT_SUCCESS;
}

int GetDhcpRawPacket(struct DhcpPacket *getPacket, int rawFd)
{
    if (getPacket == NULL) {
        return SOCKET_OPT_FAILED;
    }

    /* Get and check udp dhcp packet bytes. */
    struct UdpDhcpPacket udpPackets;
    if (memset_s(&udpPackets, sizeof(struct UdpDhcpPacket), 0, sizeof(struct UdpDhcpPacket)) != EOK) {
        return SOCKET_OPT_FAILED;
    }
    int nBytes = read(rawFd, &udpPackets, sizeof(struct UdpDhcpPacket));
    int nRet = CheckReadBytes(nBytes, (int)ntohs(udpPackets.ip.tot_len));
    if (nRet != SOCKET_OPT_SUCCESS) {
        usleep(SLEEP_TIME_200_MS);
        return nRet;
    }

    /* Check udp dhcp packet sum. */
    nBytes = (int)ntohs(udpPackets.ip.tot_len);
    if (((nRet = CheckPacketIpSum(&udpPackets, nBytes)) != SOCKET_OPT_SUCCESS) ||
        ((nRet = CheckPacketUdpSum(&udpPackets, nBytes)) != SOCKET_OPT_SUCCESS)) {
        return nRet;
    }

    int nDhcpPacket = nBytes - (int)(sizeof(udpPackets.ip) + sizeof(udpPackets.udp));
    if (memcpy_s(getPacket, sizeof(struct DhcpPacket), &(udpPackets.data), nDhcpPacket) != EOK) {
        LOGE("GetDhcpRawPacket() memcpy_s packet.data failed!");
        return SOCKET_OPT_FAILED;
    }
    if (ntohl(getPacket->cookie) != MAGIC_COOKIE) {
        LOGE("GetDhcpRawPacket() cook:%{public}x error, COOK:%{public}x!", ntohl(getPacket->cookie), MAGIC_COOKIE);
        return SOCKET_OPT_FAILED;
    }
    return nDhcpPacket;
}

int GetDhcpKernelPacket(struct DhcpPacket *getPacket, int sockFd)
{
    if (getPacket == NULL) {
        return SOCKET_OPT_FAILED;
    }

    int nBytes = -1;
    if ((nBytes = read(sockFd, getPacket, sizeof(struct DhcpPacket))) == -1) {
        LOGE("GetDhcpKernelPacket() couldn't read on kernel listening socket, error:%{public}d!", errno);
        return SOCKET_OPT_ERROR;
    }

    if (ntohl(getPacket->cookie) != MAGIC_COOKIE) {
        LOGE("GetDhcpKernelPacket() cook:%{public}x error, COOK:%{public}x!", ntohl(getPacket->cookie), MAGIC_COOKIE);
        return SOCKET_OPT_FAILED;
    }
    return nBytes;
}
