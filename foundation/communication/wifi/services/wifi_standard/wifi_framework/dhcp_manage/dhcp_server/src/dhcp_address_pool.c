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

#include "dhcp_address_pool.h"
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "securec.h"
#include "address_utils.h"
#include "common_util.h"
#include "dhcp_logger.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServerAddressPool"

#define DHCP_POOL_INIT_SIZE 10
#define DHCP_RELEASE_REMOVE_MODE 0

static int g_releaseRemoveMode = DHCP_RELEASE_REMOVE_MODE;
static HashTable g_bindingRecoders;
static int g_distributeMode = 0;

AddressBinding *GetBindingByMac(HashTable *bindTable, uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (!bindTable) {
        LOGE("binding table pointer is null.");
        return NULL;
    }
    if (!Initialized(bindTable)) {
        LOGE("binding recoders table dosn't initialized");
        return NULL;
    }
    if (ContainsKey(&g_bindingRecoders, (uintptr_t)macAddr)) {
        return (AddressBinding *)At(bindTable, (uintptr_t)macAddr);
    }
    return NULL;
}

AddressBinding *QueryBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH], PDhcpOptionList cliOptins)
{
    return GetBindingByMac(&g_bindingRecoders, macAddr);
}

AddressBinding *GetBindingByIp(HashTable *bindTable, uint32_t ipAddress)
{
    if (!bindTable) {
        LOGE("binding table pointer is null.");
        return NULL;
    }
    if (!Initialized(bindTable)) {
        LOGE("binding recoders table dosn't initialized");
        return NULL;
    }
    if (ContainsKey(bindTable, (uintptr_t)&ipAddress)) {
        return (AddressBinding *)At(bindTable, (uintptr_t)&ipAddress);
    }
    return NULL;
}

AddressBinding *AddNewBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH], PDhcpOptionList cliOptins)
{
    AddressBinding newBind = {0};
    newBind.bindingMode = BIND_MODE_DYNAMIC;
    newBind.bindingStatus = BIND_PENDING;
    if (memcpy_s(newBind.chaddr, DHCP_HWADDR_LENGTH, macAddr, DHCP_HWADDR_LENGTH) != EOK) {
        return NULL;
    }
    newBind.bindingTime = Tmspsec();
    newBind.pendingTime = Tmspsec();
    newBind.expireIn = newBind.bindingTime + DHCP_LEASE_TIME;
    newBind.leaseTime = DHCP_LEASE_TIME;
    Insert(&g_bindingRecoders, (uintptr_t)macAddr, (uintptr_t)&newBind);
    return GetBindingByMac(&g_bindingRecoders, macAddr);
}

int CheckIpAvailability(DhcpAddressPool *pool, uint8_t macAddr[DHCP_HWADDR_LENGTH], uint32_t distIp)
{
    if (!pool) {
        LOGE("pool pointer is null.");
        return DHCP_FALSE;
    }
    if (IsReserved(macAddr)) {
        LOGW("client address(%s) is reserved address.", ParseLogMac(macAddr));
        return DHCP_FALSE;
    }
    AddressBinding *lease = GetLease(pool, distIp);
    if (lease) {
        int same = AddrEquels(lease->chaddr, macAddr, MAC_ADDR_LENGTH);
        if (distIp == pool->serverId || distIp == pool->gateway) {
            return DHCP_FALSE;
        }
        if (lease->bindingMode == BIND_MODE_STATIC && !same) {
            return DHCP_FALSE;
        }
        if (IsReservedIp(pool, distIp) && !same) {
            return DHCP_FALSE;
        }
        if (same) {
            lease->pendingTime = Tmspsec();
            lease->bindingTime = lease->pendingTime;
            return DHCP_TRUE;
        }
        if (IsExpire(lease) && !same) {
            LOGD("the binding recoder has expired.");
            lease->pendingTime = Tmspsec();
            lease->bindingTime = lease->pendingTime;
            RemoveBinding(lease->chaddr);
            if (memcpy_s(lease->chaddr, DHCP_HWADDR_LENGTH, macAddr, MAC_ADDR_LENGTH) != EOK) {
                LOGD("failed to rewrite client address.");
            }
            return DHCP_TRUE;
        }
        return DHCP_FALSE;
    }
    return DHCP_TRUE;
}

int CheckRangeAvailability(
    DhcpAddressPool *pool, uint8_t macAddr[DHCP_HWADDR_LENGTH], uint32_t distIp, int *outOfRange)
{
    if (!pool || !pool->addressRange.beginAddress || !pool->addressRange.endAddress) {
        return RET_ERROR;
    }
    if (!pool->netmask || IsEmptyHWAddr(macAddr)) {
        return RET_ERROR;
    }
    uint32_t beginIp = pool->addressRange.beginAddress;
    uint32_t endIp = pool->addressRange.endAddress;
    if (IpInRange(distIp, beginIp, endIp, pool->netmask)) {
        LOGD("distribution IP address");
        AddressBinding lease = {0};
        lease.pendingTime = Tmspsec();
        lease.leaseTime = pool->leaseTime;
        lease.bindingMode = BIND_PENDING;
        lease.ipAddress = distIp;
        lease.bindingTime = lease.pendingTime;
        lease.leaseTime = pool->leaseTime;
        if (memcpy_s(lease.chaddr, sizeof(lease.chaddr), macAddr, MAC_ADDR_LENGTH) != EOK) {
            LOGE("failed to set lease chaddr fields");
            return RET_ERROR;
        }
        if (AddLease(pool, &lease) != RET_SUCCESS) {
            LOGE("failed to add lease.");
            return RET_ERROR;
        }
        return RET_SUCCESS;
    }
    if (*outOfRange) {
        LOGD("address is out of range");
        return RET_FAILED;
    } else {
        distIp = beginIp;
        *outOfRange = 1;
    }
    return RET_FAILED;
}

uint32_t NextIpOffset(uint32_t netmask)
{
    uint32_t offset = 0;
    if (g_distributeMode && netmask) {
        uint32_t total = HostTotal(netmask);
        if (total) {
            offset = Tmspusec() % total;
        }
        LOGD("next ip offset is: %u", offset);
    }
    return offset;
}

uint32_t AddressDistribute(DhcpAddressPool *pool, uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (!pool || !pool->addressRange.beginAddress || !pool->addressRange.endAddress) {
        return 0;
    }
    if (!pool->netmask || IsEmptyHWAddr(macAddr)) {
        return 0;
    }
    if (pool->distribution == 0) {
        pool->distribution = pool->addressRange.beginAddress;
    }
    int total = HostTotal(pool->netmask);
    uint32_t distIp = pool->distribution;
    if (!distIp || distIp < pool->addressRange.beginAddress) {
        distIp = pool->addressRange.beginAddress;
    }
    int distSucess = 0;
    int outOfRange = 0;
    for (int i = 0; i < total; i++) {
        int offset = 0;
        if (i == 0) {
            offset = NextIpOffset(pool->netmask);
        }
        distIp = NextIpAddress(distIp, pool->netmask, offset);
        if (!CheckIpAvailability(pool, macAddr, distIp)) {
            continue;
        }
        int ret = CheckRangeAvailability(pool, macAddr, distIp, &outOfRange);
        if (ret == RET_ERROR) {
            break;
        }
        if (ret == RET_SUCCESS) {
            distSucess = 1;
            break;
        }
    }
    if (!distSucess || !distIp) {
        return 0;
    }
    pool->distribution = distIp;
    return pool->distribution;
}

int InitAddressPool(DhcpAddressPool *pool, const char *ifname, PDhcpOptionList options)
{
    if (!pool) {
        LOGD("address pool pointer is null.");
        return RET_ERROR;
    }
    if (memset_s(pool, sizeof(DhcpAddressPool), 0, sizeof(DhcpAddressPool)) != EOK) {
        LOGD("failed to init dhcp pool.");
        return RET_ERROR;
    }
    if (memset_s(pool->ifname, IFACE_NAME_SIZE, '\0', IFACE_NAME_SIZE) != EOK) {
        LOGD("failed to reset interface name.");
        return RET_ERROR;
    }
    if (strncpy_s(pool->ifname, IFACE_NAME_SIZE, ifname, strlen(ifname)) != EOK) {
        LOGD("failed to set interface name.");
        return RET_ERROR;
    }
    if (InitOptionList(&pool->fixedOptions) != RET_SUCCESS) {
        LOGD("failed to init options field for dhcp pool.");
        return RET_FAILED;
    }
    if (CreateHashTable(&pool->leaseTable, sizeof(uint32_t), sizeof(AddressBinding), DHCP_POOL_INIT_SIZE) !=
        HASH_SUCCESS) {
        LOGD("failed to create lease table.");
        FreeOptionList(&pool->fixedOptions);
        return RET_FAILED;
    }
    if (!Initialized(&g_bindingRecoders)) {
        if (CreateHashTable(&g_bindingRecoders, MAC_ADDR_LENGTH, sizeof(AddressBinding), DHCP_POOL_INIT_SIZE) !=
            HASH_SUCCESS) {
            LOGD("failed to create binding recoding table.");
            FreeAddressPool(pool);
            return RET_FAILED;
        }
    }

    pool->distribue = AddressDistribute;
    pool->binding = QueryBinding;
    pool->newBinding = AddNewBinding;
    return RET_SUCCESS;
}

void FreeAddressPool(DhcpAddressPool *pool)
{
    if (!pool) {
        return;
    }

    if (pool->fixedOptions.size > 0) {
        ClearOptions(&pool->fixedOptions);
    }

    if (pool && Initialized(&pool->leaseTable)) {
        DestroyHashTable(&pool->leaseTable);
    }

    if (pool && HasInitialized(&pool->fixedOptions)) {
        FreeOptionList(&pool->fixedOptions);
    }
}

AddressBinding *FindBindingByIp(uint32_t ipAddress)
{
    if (!Initialized(&g_bindingRecoders)) {
        if (CreateHashTable(&g_bindingRecoders, MAC_ADDR_LENGTH, sizeof(AddressBinding), DHCP_POOL_INIT_SIZE) !=
            HASH_SUCCESS) {
            LOGD("failed to create binding recoding table.");
            return NULL;
        }
    }
    if (!g_bindingRecoders.nodes) {
        return NULL;
    }
    for (size_t current = 0; current < g_bindingRecoders.capacity; ++current) {
        HashNode *node = g_bindingRecoders.nodes[current];
        while (node) {
            HashNode *next = node->next;
            AddressBinding *binding = (AddressBinding *)node->value;
            if (binding && ipAddress == binding->ipAddress) {
                return binding;
            }
            node = next;
        }
    }
    return NULL;
}

int IsReserved(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (!Initialized(&g_bindingRecoders)) {
        LOGD("binding recoders table dosn't initialized");
        return DHCP_FALSE;
    }
    if (ContainsKey(&g_bindingRecoders, (uintptr_t)macAddr)) {
        AddressBinding *binding = GetBindingByMac(&g_bindingRecoders, macAddr);
        if (binding && binding->bindingMode == BIND_MODE_RESERVED) {
            return DHCP_TRUE;
        }
    }
    return DHCP_FALSE;
}

int IsReservedIp(DhcpAddressPool *pool, uint32_t ipAddress)
{
    if (!pool) {
        return DHCP_FALSE;
    }
    if (!ipAddress) {
        return DHCP_FALSE;
    }
    if (ContainsKey(&pool->leaseTable, (uintptr_t)&ipAddress)) {
        AddressBinding *lease = GetBindingByIp(&pool->leaseTable, ipAddress);
        if (lease && lease->bindingMode == BIND_MODE_RESERVED) {
            return DHCP_TRUE;
        }
    }
    return DHCP_FALSE;
}

int AddBinding(AddressBinding *binding)
{
    if (!Initialized(&g_bindingRecoders)) {
        if (CreateHashTable(&g_bindingRecoders, MAC_ADDR_LENGTH, sizeof(AddressBinding), DHCP_POOL_INIT_SIZE) !=
            HASH_SUCCESS) {
            LOGD("failed to create binding recoding table.");
            return RET_FAILED;
        }
    }
    if (!binding) {
        LOGE("binding pointer is null.");
        return RET_ERROR;
    }
    if (IsEmptyHWAddr(binding->chaddr)) {
        LOGE("binding address is empty.");
        return RET_ERROR;
    }
    if (!binding->ipAddress) {
        LOGE("binding ip is empty.");
        return RET_ERROR;
    }
    if (!ContainsKey(&g_bindingRecoders, (uintptr_t)binding->chaddr)) {
        if (Insert(&g_bindingRecoders, (uintptr_t)binding->chaddr, (uintptr_t)binding) == HASH_INSERTED) {
            return RET_SUCCESS;
        }
    } else {
        LOGW("binding recoder exist.");
        return RET_FAILED;
    }
    return RET_FAILED;
}
int AddReservedBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (!Initialized(&g_bindingRecoders)) {
        if (CreateHashTable(&g_bindingRecoders, MAC_ADDR_LENGTH, sizeof(AddressBinding), DHCP_POOL_INIT_SIZE) !=
            HASH_SUCCESS) {
            LOGD("failed to create binding recoding table.");
            return RET_FAILED;
        }
    }
    AddressBinding *binding = GetBindingByMac(&g_bindingRecoders, macAddr);
    if (binding) {
        binding->bindingMode = BIND_MODE_RESERVED;
    } else {
        AddressBinding bind = {0};
        bind.bindingMode = BIND_MODE_RESERVED;
        bind.bindingTime = Tmspsec();
        bind.pendingTime = bind.bindingTime;
        if (Insert(&g_bindingRecoders, (uintptr_t)macAddr, (uintptr_t)&bind) == HASH_INSERTED) {
            return RET_SUCCESS;
        }
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

int RemoveBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (ContainsKey(&g_bindingRecoders, (uintptr_t)macAddr)) {
        if (Remove(&g_bindingRecoders, (uintptr_t)macAddr) == HASH_SUCCESS) {
            return RET_SUCCESS;
        }
        return RET_ERROR;
    }
    return RET_FAILED;
}

int RemoveReservedBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (ContainsKey(&g_bindingRecoders, (uintptr_t)macAddr)) {
        AddressBinding *binding = GetBindingByMac(&g_bindingRecoders, macAddr);
        if (!binding) {
            LOGE("failed to get binding recoder.");
            return RET_FAILED;
        }
        if (binding->bindingMode == BIND_MODE_RESERVED) {
            if (Remove(&g_bindingRecoders, (uintptr_t)macAddr) == HASH_SUCCESS) {
                return RET_SUCCESS;
            }
            LOGE("failed to remove reserved binding recoder.");
        } else {
            LOGW("binding mode is not 'BIND_MODE_RESERVED'.");
        }
        return RET_FAILED;
    }
    return RET_FAILED;
}

int ReleaseBinding(uint8_t macAddr[DHCP_HWADDR_LENGTH])
{
    if (ContainsKey(&g_bindingRecoders, (uintptr_t)macAddr)) {
        if (g_releaseRemoveMode) {
            return Remove(&g_bindingRecoders, (uintptr_t)macAddr);
        }

        AddressBinding *binding = GetBindingByMac(&g_bindingRecoders, macAddr);
        if (!binding) {
            LOGE("failed to query binding.");
            return RET_ERROR;
        }
        binding->bindingStatus = BIND_RELEASED;
        return RET_SUCCESS;
    }
    return RET_FAILED;
}

int AddLease(DhcpAddressPool *pool, AddressBinding *lease)
{
    if (!pool) {
        return RET_ERROR;
    }

    if (!lease || !lease->ipAddress || IsEmptyHWAddr(lease->chaddr)) {
        return RET_ERROR;
    }
    if (!ContainsKey(&pool->leaseTable, (uintptr_t)&lease->ipAddress)) {
        if (Insert(&pool->leaseTable, (uintptr_t)&lease->ipAddress, (uintptr_t)lease) == HASH_INSERTED) {
            return RET_SUCCESS;
        }
    } else {
        if (Insert(&pool->leaseTable, (uintptr_t)&lease->ipAddress, (uintptr_t)lease) == HASH_UPDATED) {
            LOGD("update lease info.");
            return RET_SUCCESS;
        }
    }
    return RET_FAILED;
}

AddressBinding *GetLease(DhcpAddressPool *pool, uint32_t ipAddress)
{
    if (!ipAddress) {
        return NULL;
    }
    if (!pool) {
        return NULL;
    }
    int ipAddr = ipAddress;
    if (ContainsKey(&pool->leaseTable, (uintptr_t)&ipAddr)) {
        AddressBinding *lease = GetBindingByIp(&pool->leaseTable, ipAddress);
        if (!lease) {
            LOGE("failed to update lease recoder.");
            return NULL;
        }
        return lease;
    }
    return NULL;
}

int UpdateLease(DhcpAddressPool *pool, AddressBinding *lease)
{
    if (!pool) {
        return RET_ERROR;
    }

    if (!lease || !lease->ipAddress || IsEmptyHWAddr(lease->chaddr)) {
        return RET_ERROR;
    }

    if (ContainsKey(&pool->leaseTable, (uintptr_t)&lease->ipAddress)) {
        if (Insert(&pool->leaseTable, (uintptr_t)&lease->ipAddress, (uintptr_t)lease) == HASH_UPDATED) {
            return RET_SUCCESS;
        }
    }
    return RET_FAILED;
}

int RemoveLease(DhcpAddressPool *pool, AddressBinding *lease)
{
    if (!pool) {
        return RET_ERROR;
    }

    if (!lease || !lease->ipAddress || IsEmptyHWAddr(lease->chaddr)) {
        return RET_ERROR;
    }

    if (ContainsKey(&pool->leaseTable, (uintptr_t)&lease->ipAddress)) {
        if (Remove(&pool->leaseTable, (uintptr_t)&lease->ipAddress) == HASH_SUCCESS) {
            return RET_SUCCESS;
        }
    }
    return RET_FAILED;
}

int LoadBindingRecoders(DhcpAddressPool *pool)
{
    if (pool == NULL) {
        return RET_FAILED;
    }
    char filePath[DHCP_LEASE_FILE_LENGTH] = {0};
    if (snprintf_s(filePath, sizeof(filePath), sizeof(filePath) - 1, "%s.%s", DHCPD_LEASE_FILE, pool->ifname) < 0) {
        LOGE("Failed to get dhcp lease file path!");
        return RET_FAILED;
    }
    FILE *fp = fopen(filePath, "r");
    if (fp == NULL) {
        return RET_FAILED;
    }
    char line[DHCP_FILE_LINE_LENGTH] = {0};
    while (fgets(line, DHCP_FILE_LINE_LENGTH, fp) != NULL) {
        TrimString(line);
        if (line[0] == '\0') { /* skip empty line */
            continue;
        }
        AddressBinding bind = {0};
        if (ParseAddressBinding(&bind, line) != 0) {
            continue;
        }
        Insert(&(pool->leaseTable), (uintptr_t)&bind.ipAddress, (uintptr_t)&bind);
    }
    fclose(fp);
    return RET_SUCCESS;
}

int SaveBindingRecoders(const DhcpAddressPool *pool, int force)
{
    if (pool == NULL) {
        return RET_FAILED;
    }
    static uint64_t lastTime = 0;
    uint64_t currTime = Tmspsec();
    if (force == 0 && currTime < lastTime + DHCP_REFRESH_LEASE_FILE_INTERVAL) {
        return RET_WAIT_SAVE;
    }
    char filePath[DHCP_LEASE_FILE_LENGTH] = {0};
    if (snprintf_s(filePath, sizeof(filePath), sizeof(filePath) - 1, "%s.%s", DHCPD_LEASE_FILE, pool->ifname) < 0) {
        LOGE("Failed to set dhcp lease file path!");
        return RET_FAILED;
    }
    char line[DHCP_FILE_LINE_LENGTH] = {0};
    FILE *fp = fopen(filePath, "w");
    if (fp == NULL) {
        return RET_FAILED;
    }
    for (size_t index = 0; index < pool->leaseTable.capacity; ++index) {
        HashNode *node = pool->leaseTable.nodes[index];
        while (node != NULL) {
            AddressBinding *binding = (AddressBinding *)node->value;
            if (WriteAddressBinding(binding, line, sizeof(line)) != RET_SUCCESS) {
                LOGW("Failed to convert binding info to string");
            } else {
                fprintf(fp, "%s\n", line);
            }
            node = node->next;
        }
    }
    fclose(fp);
    lastTime = currTime;
    return RET_SUCCESS;
}

void SetDistributeMode(int mode)
{
    g_distributeMode = mode;
}
int GetDistributeMode(void)
{
    return g_distributeMode;
}