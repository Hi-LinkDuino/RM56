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

#include "dhcp_binding.h"
#include <stdint.h>
#include <stdlib.h>
#include "dhcp_logger.h"
#include "address_utils.h"
#include "common_util.h"
#include "securec.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServerBinding"
#define PENDING_INTERVAL_MAX_TIME 1200
#define PENDING_INTERVAL_LEVEL0 3
#define PENDING_INTERVAL_LEVEL1 10
#define PENDING_INTERVAL_LEVEL2 30
#define PENDING_MIN_WAITING_TIMES 1
#define PENDING_INTERVAL_LEVEL1_TIMES 2
#define PENDING_INTERVAL_LEVEL2_TIMES 5


uint64_t NextPendingInterval(uint64_t pendingInterval)
{
    uint64_t next = pendingInterval;
    if (next < PENDING_INTERVAL_LEVEL0) {
        next += PENDING_MIN_WAITING_TIMES;
    } else if (next < PENDING_INTERVAL_LEVEL1) {
        next += PENDING_INTERVAL_LEVEL1_TIMES;
    } else if (next < PENDING_INTERVAL_LEVEL2) {
        next += PENDING_INTERVAL_LEVEL2_TIMES;
    } else {
        next = PENDING_INTERVAL_MAX_TIME;
    }
    return next;
}

int IsExpire(AddressBinding *binding)
{
    if (!binding) {
        LOGE("binding is null.");
        return DHCP_FALSE;
    }
    uint64_t leaseTime = binding->leaseTime;
    if (!leaseTime) {
        leaseTime = DHCP_LEASE_TIME;
    }
    uint64_t expireIn = binding->expireIn;
    if (binding->bindingStatus == BIND_PENDING) {
        expireIn = binding->pendingTime + leaseTime;
    } else if (binding->bindingStatus == BIND_ASSOCIATED) {
        expireIn = binding->bindingTime + leaseTime;
    }
    uint64_t curr = Tmspsec();
    if (curr > expireIn) {
        binding->bindingStatus = BIND_EXPIRED;
        return DHCP_TRUE;
    }
    return DHCP_FALSE;
}

#define BINDING_MAC_ADDR_POS 0
#define BINDING_IP_ADDR_POS 1
#define BINDING_LEASE_TIME_POS 2
#define BINDING_BINDING_TIME_POS 3
#define BINDING_PENDING_TIME_POS 4
#define BINDING_PENDING_INTERVAL_POS 5
#define BINDING_BINDING_MODE_POS 6
#define BINDING_BINDING_STATUS_POS 7
#define BINDING_STRING_SIZE 8

int WriteAddressBinding(const AddressBinding *binding, char *out, uint32_t size)
{
    if (!binding || !out) {
        return RET_FAILED;
    }
    const char *mac = ParseStrMac(binding->chaddr, sizeof(binding->chaddr));
    const char *ip = ParseStrIp(binding->ipAddress);
    if (mac == NULL || ip == NULL) {
        return RET_FAILED;
    }
    if (snprintf_s(out, size, size - 1, "%s %s %llu %llu %llu %llu %d %d", mac, ip, binding->leaseTime,
        binding->bindingTime, binding->pendingTime, binding->pendingInterval, binding->bindingMode,
        binding->bindingStatus) < 0) {
        return RET_FAILED;
    }
    return RET_SUCCESS;
}

static void ReleaseStrings(char **strs)
{
    if (strs == NULL) {
        return;
    }
    int i = 0;
    while (strs[i] != NULL) {
        free(strs[i]);
        strs[i] = NULL;
        ++i;
    }
    free(strs);
    strs = NULL;
    return;
}

static char **SplitString(const char *buf, const char *split)
{
    const char *pos = buf;
    const char *p = NULL;
    size_t len = strlen(split);
    int num = 0;
    while ((p = strstr(pos, split)) != NULL) {
        if (p != pos) {
            ++num;
        }
        pos = p + len;
    }
    if (*pos != '\0') {
        ++num;
    }
    if (num == 0) {
        return NULL;
    }
    char **strs = (char **)calloc(num + 1, sizeof(char *));
    if (strs == NULL) {
        return NULL;
    }
    pos = buf;
    num = 0;
    while ((p = strstr(pos, split)) != NULL) {
        if (p != pos) {
            size_t strLen = p - pos + 1;
            strs[num] = (char *)calloc(strLen, sizeof(char));
            if (strs[num] == NULL || strncpy_s(strs[num], strLen, pos, p - pos) != EOK) {
                ReleaseStrings(strs);
                return NULL;
            }
            ++num;
        }
        pos = p + len;
    }
    if (*pos != '\0') {
        size_t strLen = strlen(pos) + 1;
        strs[num] = (char *)calloc(strLen, sizeof(char));
        if (strs[num] == NULL || strncpy_s(strs[num], strLen, pos, strlen(pos)) != EOK) {
            ReleaseStrings(strs);
            return NULL;
        }
    }
    return strs;
}

int ParseAddressBinding(AddressBinding *binding, const char *buf)
{
    uint64_t curr = Tmspsec();
    char **strs = SplitString(buf, " ");
    if (strs == NULL) {
        return -1;
    }
    int num = 0;
    while (strs[num] != NULL) {
        ++num;
    }
    int ret = -1;
    do {
        if (num < BINDING_STRING_SIZE) {
            break;
        }
        ParseMacAddress(strs[BINDING_MAC_ADDR_POS], binding->chaddr);
        binding->ipAddress = ParseIpAddr(strs[BINDING_IP_ADDR_POS]);
        binding->leaseTime = atol(strs[BINDING_LEASE_TIME_POS]);
        binding->bindingTime = atol(strs[BINDING_BINDING_TIME_POS]);
        binding->pendingTime = atol(strs[BINDING_PENDING_TIME_POS]);
        if (binding->bindingTime && binding->bindingTime < binding->pendingTime) {
            break;
        }
        if (binding->pendingTime > curr) { /* if pending time over than current system time */
            binding->bindingTime = binding->bindingTime - binding->pendingTime + curr;
            binding->pendingTime = curr;
        }
        binding->pendingInterval = atol(strs[BINDING_PENDING_INTERVAL_POS]);
        binding->bindingMode = atoi(strs[BINDING_BINDING_MODE_POS]);
        binding->bindingStatus = atoi(strs[BINDING_BINDING_STATUS_POS]);
        if (binding->bindingStatus == BIND_ASSOCIATED) {
            binding->expireIn = binding->bindingTime + binding->leaseTime;
        }
        ret += 1; /* set ret = 0 */
    } while (0);
    ReleaseStrings(strs);
    return ret;
}
