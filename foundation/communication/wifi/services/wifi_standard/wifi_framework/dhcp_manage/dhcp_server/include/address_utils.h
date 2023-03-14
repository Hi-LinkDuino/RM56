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

#ifndef OHOS_ADDRESS_UTILS_H
#define OHOS_ADDRESS_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <dhcp_define.h>

#define MAC_ADDR_LENGTH 6

#ifdef __cplusplus
extern "C" {
#endif

uint32_t NetworkAddress(uint32_t ip, uint32_t netmask);
uint32_t FirstIpAddress(uint32_t ip, uint32_t netmask);
uint32_t NextIpAddress(uint32_t currIp, uint32_t netmask, uint32_t offset);
uint32_t FirstNetIpAddress(uint32_t networkAddr);
uint32_t LastIpAddress(uint32_t ip, uint32_t netmask);
int IpInNetwork(uint32_t ip, uint32_t network, uint32_t netmask);
int IpInRange(uint32_t ip, uint32_t beginIp, uint32_t endIp, uint32_t netmask);
uint32_t BroadCastAddress(uint32_t ip, uint32_t netmask);
uint32_t ParseIpAddr(const char *strIp);
uint32_t ParseIpHtonl(const char *strIp);
int NetworkBits(uint32_t netmask);
int HostBits(uint32_t netmask);
int HostTotal(uint32_t netmask);
uint32_t ParseIp(const uint8_t *ipAddr);
const char *ParseStrIp(uint32_t ipAddr);
int IsEmptyHWAddr(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
char *ParseStrMac(const uint8_t *macAddr, size_t addrSize);
int ParseMacAddress(const char *strMac, uint8_t macAddr[DHCP_HWADDR_LENGTH]);

uint32_t HostToNetwork(uint32_t host);
uint32_t NetworkToHost(uint32_t network);

char *ParseLogMac(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
int AddrEquels(uint8_t firstAddr[DHCP_HWADDR_LENGTH], uint8_t secondAddr[DHCP_HWADDR_LENGTH], int addrLength);
#ifdef __cplusplus
}
#endif
#endif