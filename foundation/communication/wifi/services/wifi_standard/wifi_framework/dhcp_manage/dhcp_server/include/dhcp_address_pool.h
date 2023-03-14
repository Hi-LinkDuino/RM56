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

#ifndef OHOS_DHCP_ADDRESS_POOL_H
#define OHOS_DHCP_ADDRESS_POOL_H

#include <stdint.h>
#include "dhcp_define.h"
#include "dhcp_option.h"
#include "hash_table.h"
#include "dhcp_binding.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct DhcpAddressPool DhcpAddressPool;
typedef AddressBinding *(*QueryBind)(uint8_t macAddr[DHCP_HWADDR_LENGTH], PDhcpOptionList cliOptins);
typedef AddressBinding *(*AddBind)(uint8_t macAddr[DHCP_HWADDR_LENGTH], PDhcpOptionList cliOptins);
typedef uint32_t (*Distribute)(DhcpAddressPool *pool, uint8_t macAddr[DHCP_HWADDR_LENGTH]);

typedef struct {
    uint32_t beginAddress;
    uint32_t endAddress;
} IpAddressRange;

struct DhcpAddressPool {
    char ifname[IFACE_NAME_SIZE];
    uint32_t netmask;
    IpAddressRange addressRange;
    uint32_t serverId;
    uint32_t gateway;
    uint32_t leaseTime;
    uint32_t renewalTime;
    uint32_t rebindingTime;
    uint32_t distribution;
    Distribute distribue;
    QueryBind binding;
    AddBind newBinding;
    HashTable leaseTable;
    DhcpOptionList fixedOptions;
    int initialized;
};

int InitAddressPool(DhcpAddressPool *pool, const char *ifname, PDhcpOptionList options);
void FreeAddressPool(DhcpAddressPool *pool);
AddressBinding *FindBindingByIp(uint32_t bingdingIp);
int IsReserved(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
int IsReservedIp(DhcpAddressPool *pool, uint32_t ipAddress);
int AddBinding(AddressBinding *binding);
int AddReservedBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
int RemoveReservedBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
int RemoveBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
int ReleaseBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH]);
int AddLease(DhcpAddressPool *pool, AddressBinding *lease);
AddressBinding *GetLease(DhcpAddressPool *pool, uint32_t ipAddress);
int UpdateLease(DhcpAddressPool *pool, AddressBinding *lease);
int RemoveLease(DhcpAddressPool *pool, AddressBinding *lease);
int LoadBindingRecoders(DhcpAddressPool *pool);
int SaveBindingRecoders(const DhcpAddressPool *pool, int force);
AddressBinding *GetBindingByIp(HashTable *bindTable, uint32_t ipAddress);
AddressBinding *QueryBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH], PDhcpOptionList cliOptins);
void SetDistributeMode(int mode);
int GetDistributeMode(void);

#ifdef __cplusplus
}
#endif
#endif