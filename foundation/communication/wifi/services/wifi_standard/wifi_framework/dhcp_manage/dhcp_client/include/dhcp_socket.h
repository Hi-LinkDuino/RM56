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
#ifndef OHOS_SOCKET_H
#define OHOS_SOCKET_H


#include "dhcp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

int CreateRawSocket(int *rawFd);
int CreateKernelSocket(int *sockFd);
int BindRawSocket(const int rawFd, const int ifaceIndex, const uint8_t *ifaceAddr);
int BindKernelSocket(const int sockFd, const char *ifaceName, const uint32_t sockIp, const int sockPort, bool bCast);
int SendToDhcpPacket(
    const struct DhcpPacket *sendPacket, uint32_t srcIp, uint32_t destIp, int destIndex, const uint8_t *destHwaddr);
int SendDhcpPacket(struct DhcpPacket *sendPacket, uint32_t srcIp, uint32_t destIp);
int CheckReadBytes(const int count, const int totLen);
int CheckUdpPacket(struct UdpDhcpPacket *pPacket, const int totLen);
int CheckPacketIpSum(struct UdpDhcpPacket *pPacket, const int bytes);
int CheckPacketUdpSum(struct UdpDhcpPacket *pPacket, const int bytes);
int GetDhcpRawPacket(struct DhcpPacket *getPacket, int rawFd);
int GetDhcpKernelPacket(struct DhcpPacket *getPacket, int sockFd);

#ifdef __cplusplus
}
#endif
#endif
