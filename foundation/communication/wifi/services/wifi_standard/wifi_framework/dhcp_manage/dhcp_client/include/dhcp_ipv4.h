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
#ifndef OHOS_IP4_H
#define OHOS_IP4_H

#include <stdint.h>
#include <sys/types.h>

#include "dhcp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

void SetSocketMode(uint32_t mode);
int SetIpv4State(int state);
int InitSignalHandle(void);
int ExecDhcpRelease(void);
int ExecDhcpRenew(void);

int PublishDhcpResultEvent(const char *ifname, const int code, struct DhcpResult *result);
int GetPacketHeaderInfo(struct DhcpPacket *packet, uint8_t type);
int GetPacketCommonInfo(struct DhcpPacket *packet);
int DhcpDiscover(uint32_t transid, uint32_t requestip);
int DhcpRequest(uint32_t transid, uint32_t reqip, uint32_t servip);
int DhcpRenew(uint32_t transid, uint32_t clientip, uint32_t serverip);
int DhcpRelease(uint32_t clientip, uint32_t serverip);
int StartIpv4(void);
void SendReboot(struct DhcpPacket *p, time_t timestamp);

int GetPacketReadSockFd(void);
int GetSigReadSockFd(void);
uint32_t GetDhcpTransID(void);

#ifdef __cplusplus
}
#endif
#endif
