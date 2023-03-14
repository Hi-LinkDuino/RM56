/*
 * Copyright (c) 2021 bestechnic (Shanghai) Technologies CO., LIMITED.
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
#include <stdlib.h>
#include "shell.h"
#include "shcmd.h"
#include "hal_trace.h"
#include "hal_cmu.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

static UINT32 doReboot(UINT32 argc, const CHAR **argv)
{
    hal_cmu_sys_reboot();
    return 0;
}

#ifdef WIFI_ENABLE
#include "wifi_device.h"
#include "bwifi_interface.h"
static UINT32 doWifiConnect(UINT32 argc, const CHAR **argv)
{
    if (argc < 1) {
        printf("usage: connect <ssid> [pwd] [ip] [netmask] [gw]\n");
        return (UINT32)-1;
    }
    WifiDeviceConfig config = {0};
    int netId = 0;
    strcpy(config.ssid, argv[0]);
    if (argc >= 2) {
        strcpy(config.preSharedKey, argv[1]);
        config.securityType = WIFI_SEC_TYPE_PSK;
    } else {
        config.securityType = WIFI_SEC_TYPE_OPEN;
    }
    if (argc >= 5) {
        config.ipType = STATIC_IP;
        config.staticIp.ipAddress = inet_addr(argv[2]);
        config.staticIp.gateway = inet_addr(argv[3]);
        config.staticIp.netmask = inet_addr(argv[4]);
        config.staticIp.dnsServers[0] = inet_addr("8.8.8.8");
    } else {
        config.ipType = DHCP;
    }

    if (AddDeviceConfig(&config, &netId) != WIFI_SUCCESS) {
        printf("AddDeviceConfig failed!\r\n");
        return (UINT32)-1;
    }

    if (IsWifiActive() == WIFI_STA_NOT_ACTIVE) {
        if (EnableWifi() != WIFI_SUCCESS) {
            printf("EnableWifi failed\r\n");
            return (UINT32)-1;
        }
    }

    if (ConnectTo(netId) != WIFI_SUCCESS) {
        printf("Connect to %s failed!\r\n", config.ssid);
        return (UINT32)-1;
    }
    return 0;
}

static UINT32 doWifiDisconnect(UINT32 argc, const CHAR **argv)
{
    RemoveDevice(0);
    if (Disconnect()) {
        printf("Disconnect failed\r\n");
        return (UINT32)-1;
    }
    return 0;
}

static UINT32 doWifiAutoConnect(UINT32 argc, const CHAR **argv)
{
    if (bwifi_auto_connect()) {
        printf("bwifi_auto_connect failed\r\n");
        return (UINT32)-1;
    }
    return 0;
}

#endif

static UINT32 doNsLookup(UINT32 argc, const CHAR **argv)
{
    if (argc < 1) {
        printf("usage: nslookup <host_name>\n");
        return (UINT32)-1;
    }
    char *name = argv[0];
    struct hostent *hptr = gethostbyname(name);
    if (!hptr) {
        printf("gethostbyname error for host: %s\n", name);
        return (UINT32)-1;
    }
    printf("\tofficial: %s\n", hptr->h_name);
    char **pptr;
    char str[INET_ADDRSTRLEN];
    for (pptr = hptr->h_aliases; *pptr != NULL; pptr++) {
        printf("\ttalias: %s\n", *pptr);
    }
    switch (hptr->h_addrtype) {
    case AF_INET:
        pptr = hptr->h_addr_list;
        for (; *pptr != NULL; pptr++) {
            printf("\taddress: %s\n", inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
        }
        break;
    default:
        printf("unknown address type\n");
        break;
    }
    return 0;
}

static CmdItem g_shellcmd[] = {
    {CMD_TYPE_EX, "reboot", 0, (CmdCallBackFunc)doReboot},
#ifdef WIFI_ENABLE
    {CMD_TYPE_EX, "connect", XARGS, (CmdCallBackFunc)doWifiConnect},
    {CMD_TYPE_EX, "disconnect", 0, (CmdCallBackFunc)doWifiDisconnect},
    {CMD_TYPE_EX, "autoconnect", 0, (CmdCallBackFunc)doWifiAutoConnect},
#endif
    {CMD_TYPE_EX, "nslookup", 1, (CmdCallBackFunc)doNsLookup},
};

void RegisterCustomShellCmd()
{
    for (int i = 0; i < sizeof(g_shellcmd) / sizeof(g_shellcmd[0]); i++) {
        UINT32 ret = osCmdReg(g_shellcmd[i].cmdType, g_shellcmd[i].cmdKey, g_shellcmd[i].paraNum, g_shellcmd[i].cmdHook);
        if (ret) {
            printf("osCmdReg %d key %s failed\n", i, g_shellcmd[i].cmdKey);
        }
    }
}
