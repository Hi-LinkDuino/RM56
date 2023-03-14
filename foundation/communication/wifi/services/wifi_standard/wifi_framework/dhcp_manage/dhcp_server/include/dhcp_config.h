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

#ifndef OHOS_DHCP_CONFIG_H
#define OHOS_DHCP_CONFIG_H


#include <stdint.h>
#include <stdint.h>
#include <time.h>
#include "dhcp_define.h"
#include "dhcp_option.h"
#include "dhcp_address_pool.h"
#include "dhcp_binding.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct DhcpConfig DhcpConfig;
struct DhcpConfig {
    char ifname[IFACE_NAME_SIZE];           /* Network interface name. */
    uint32_t serverId;                      /* Server identifier. */
    uint32_t netmask;                       /* Default subnet mask. */
    uint32_t gateway;                       /* Default gateway. */
    uint32_t leaseTime;                     /* Lease time value, default DHCP_LEASE_TIME. */
    uint32_t renewalTime;                   /* Renewal time value. */
    uint32_t rebindingTime;                 /* Rebinding time value. */
    uint32_t distribution;                  /* Ip distribue mode [0|1]: 0-random, 1-sequence(default). */
    uint32_t broadcast;                     /* Broadcast flags enable [0|1]: 0-no, 1-yes(default). */
    IpAddressRange pool;                    /* Address Range. */
    DhcpOptionList options;                 /* Append dhcp options. */
};
typedef struct DhcpConfig *PDhcpConfig;

typedef struct DhcpConfigNode DhcpConfigNode;
struct DhcpConfigNode {
    DhcpConfig config;
    DhcpConfigNode *next;
};
typedef struct DhcpConfigNode *PDhcpConfigNode;

typedef struct DhcpConfigList DhcpConfigList;
struct DhcpConfigList {
    int size;
    DhcpConfigNode *first;
};
typedef struct DhcpConfigList *PDhcpConfigList;

int LoadConfig(const char *configFile, const char *ifname, DhcpConfig *config);

#ifdef __cplusplus
}
#endif
#endif