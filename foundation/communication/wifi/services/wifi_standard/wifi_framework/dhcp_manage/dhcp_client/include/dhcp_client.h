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
#ifndef OHOS_DHCPC_H
#define OHOS_DHCPC_H

#include <pthread.h>

#include "dhcp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WORKDIR                 "/data/dhcp/"
#define DHCPC_NAME              "dhcp_client_service"
#define DHCPC_CONF              "dhcp_client_service.conf"
#define DHCPC_PID               "dhcp_client_service.pid"
#define DHCPC_VERSION           "1.0"
#define DHCPC_LEASE             "dhcp_client_service-%s.lease"

enum DHCP_IP_TYPE {
    DHCP_IP_TYPE_NONE =  0,
    DHCP_IP_TYPE_ALL  =  1,
    DHCP_IP_TYPE_V4   =  2,
    DHCP_IP_TYPE_V6   =  3
};

struct DhcpClientCfg {
    char workDir[DIR_MAX_LEN];              /* Current process working directory. */
    char confFile[DIR_MAX_LEN];             /* Current process Configuration Directory. */
    char pidFile[DIR_MAX_LEN];              /* Current process pid file. */
    char resultFile[DIR_MAX_LEN];           /* Save the obtained IPv4 result. */
    char ifaceName[INFNAME_SIZE];           /* Name of the network interface used by the current process. */
    int  ifaceIndex;                        /* Index of the network interface used by the current process. */
    unsigned int ifaceIpv4;                 /* IPv4 of the network interface used by the current process. */
    unsigned int getMode;                   /* Current process obtaining IPv4 address mode. */
    unsigned char ifaceMac[MAC_ADDR_LEN];   /* Mac addr of the network interface used by the current process. */
    unsigned char *pOptClientId;            /* DHCP packet options field clientid. */
    bool timeoutExit;                       /* DHCP packet sending times out and exits automatically. */
    char leaseFile[DIR_MAX_LEN];
    char result6File[DIR_MAX_LEN];
    pthread_t thrId;
};

int StartProcess(void);
int StopProcess(const char *pidFile);
int GetProStatus(const char *pidFile);

struct DhcpClientCfg *GetDhcpClientCfg(void);

#ifdef __cplusplus
}
#endif
#endif
