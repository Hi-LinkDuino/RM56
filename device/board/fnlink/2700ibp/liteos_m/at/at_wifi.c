/*
 * Copyright (c) 2021 FnLink (hunan) Technologies CO., LIMITED.
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

#include "cmsis_os2.h"
#include "console.h"
#include "hal_trace.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lwip/api_shell.h"
#include "lwip/tcpip.h"
#include "ohos_init.h"
#include "wifi_hotspot.h"
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "bwifi_interface.h"
#include "wifi_device.h"
#include "wifi_error_code.h"

static int at_param_null_check(int *_argc, char **argv)
{
    int argc = *_argc;
    printf("argc = %d\r\n", argc);
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) {
            return -1;
        }
        printf("argv[%d] = %s\r\n", i, argv[i]);
    }
    argc = argc - 1;
    for (int i = 0; i < argc; i++) {
        argv[i] = argv[i+1];
    }
    *_argc = argc;
    return 0;
}

unsigned int lwip_ifconfig_check(int argc, const char **argv)
{
    if ((argc == 0)) {
        return 0;
    } else if (argc == 2) { /* 2 Parameters */
        if ((strcmp("up", argv[1]) == 0) || (strcmp("down", argv[1]) == 0)) {
            return 0;
        } else {
            return -1;
        }
    } else if (argc == 6) { /* 6 Parameter */
        if ((strcmp("netmask", argv[2]) == 0) && (strcmp("gw", argv[4]) == 0) && /* 2/4:Members of parameters */
            (strcmp("inet", argv[1]) != 0) && (strcmp("inet6", argv[1]) != 0)) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

static int at_lwip_ifconfig(cmd_tbl_t *cmd, int argc, char *argv[])
{
    if (at_param_null_check(&argc, argv) == -1) {
        return -1;
    }

    int ret = lwip_ifconfig_check(argc, argv);
    if (ret != 0) {
        return -1;
    }
    struct netif *netif = NULL;
    if (argc != 0) {
        if ((netif = netif_find(argv[0])) == NULL) {
            netif = netifapi_netif_find_by_name(argv[0]);
        }
    }

    if (argc == 2) { /* 2:Number of parameters */
        if (netif == NULL) {
            return -1;
        }
        if (strcmp(argv[1], "down") == 0) {
            (void)netifapi_netif_set_link_down(netif);
            (void)netifapi_netif_set_down(netif);
            // (void)netif_l2_netdev_status_callback(netif);
            (void)netifapi_netif_set_addr(netif, NULL, NULL, NULL);
#if LWIP_IPV6            
            for (hi_u8 index = 0; index < LWIP_IPV6_NUM_ADDRESSES; index++) {
                (void)netifapi_netif_rmv_ip6_address(netif, &netif->ip6_addr[index]);
            }
#endif
        } else if (strcmp(argv[1], "up") == 0) {
            (void)netifapi_netif_set_up(netif);
            // (void)netif_l2_netdev_status_callback(netif);
#if LWIP_IPV6            
                (hi_void)netifapi_netif_add_ip6_linklocal_address(netif, HI_TRUE);
#endif
                (void)netifapi_netif_set_link_up(netif);

                (void)netifapi_netif_set_up(netif);
        }
    } else if (argc == 6){
        if (netif == NULL || !netif_is_up(netif)) {
            return -1;
        }
        ip4_addr_t ipaddr, netmask, gw;
        ip4addr_aton(argv[1],&ipaddr);
        ip4addr_aton(argv[3],&netmask);
        ip4addr_aton(argv[5],&gw);
        (void)netifapi_netif_set_addr(netif, &ipaddr, &netmask, &gw);
    } else {
        ret = lwip_ifconfig(argc, (const char **)argv);
        if (ret == 0) {
            return 0;
        } else if (ret == 3) { /* 3:up down execution succeeded. */
            printf("OK\r\n");
            return 0;
        }
        return -1;
    }
    printf("OK\r\n");
    return 0;
}
static int g_apEnableSuccess = 0;
static WifiEvent g_wifiEventHandler = {0};
static WifiErrorCode error;

static void OnHotspotStaJoinHandler(StationInfo *info)
{
    if (info == NULL) {
        printf("HotspotStaJoin:info is null.\r\n");
        return;
    }
    StationInfo stainfo[WIFI_MAX_STA_NUM] = {0};
    StationInfo *sta_list_node = NULL;
    unsigned int size = WIFI_MAX_STA_NUM;

    error = GetStationList(stainfo, &size);
    if (error != WIFI_SUCCESS) {
        printf("HotspotStaJoin:get list fail, error is %d.\r\n", error);
        return;
    }
    sta_list_node = stainfo;
    for (uint32_t i = 0; i < size; i++, sta_list_node++) {
        unsigned char *mac = sta_list_node->macAddress;
        printf("HotspotSta[%u]: macAddress=%02X:%02X:%02X:%02X:%02X:%02X\r\n", i,
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    g_apEnableSuccess++;
}

static void OnHotspotStaLeaveHandler(StationInfo *info)
{
    if (info == NULL) {
        printf("HotspotStaLeave:info is null.\r\n");
        return;
    }
    unsigned char *mac = info->macAddress;
    printf("HotspotStaLeave: macAddress=%02X:%02X:%02X:%02X:%02X:%02X, reason=%d.\r\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
           info->disconnectedReason);
    g_apEnableSuccess--;
}

static void OnHotspotStateChangedHandler(int state)
{
    printf("HotspotStateChanged:state is %d.\r\n", state);
    if (state == WIFI_HOTSPOT_ACTIVE) {
        printf("wifi hotspot active.\r\n");
    } else {
        printf("wifi hotspot noactive.\r\n");
    }
}

static void net_intf_status_change_cb(struct netif *netif)
{
    if (netif_is_up(netif) && !ip_addr_isany(&netif->ip_addr)) {
        struct ip4_addr_t * ip = (ip4_addr_t *)&netif->ip_addr;
        struct ip4_addr_t * gw = (ip4_addr_t *)&netif->gw;
        struct ip4_addr_t * mask = (ip4_addr_t *)&netif->netmask;
        printf("net_intf_status_change_cb **ip = %s\n", ip4addr_ntoa(ip));
        printf("net_intf_status_change_cb **netmask = %s\n", ip4addr_ntoa(mask));
        printf("net_intf_status_change_cb **gw = %s\n", ip4addr_ntoa(gw));
    }
}

static int WifiAPStart(char* ap_ssid, char* ap_psk, int sec_type, int band_type, int channel)
{
    osDelay(2000);

    //注册wifi事件的回调函数
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;
    error = RegisterWifiEvent(&g_wifiEventHandler);
    if (error != WIFI_SUCCESS) {
        printf("RegisterWifiEvent failed, error = %d.\r\n", error);
        return -1;
    }
    printf("RegisterWifiEvent succeed!\r\n");

    HotspotConfig config = {0};
    strcpy(config.ssid, ap_ssid);
    strcpy(config.preSharedKey, ap_psk);
    config.securityType = sec_type;
    config.band = band_type;
    config.channelNum = channel;

    error = SetHotspotConfig(&config);
    printf("SetHotspotConfig succeed!\r\n");

    if (error != WIFI_SUCCESS) {
        printf("SetHotspotConfig failed, error = %d.\r\n", error);
        return -1;
    }

    error = EnableHotspot();
    if (error != WIFI_SUCCESS) {
        printf("EnableHotspot failed, error = %d.\r\n", error);
        return -1;
    }
    printf("EnableHotspot succeed!\r\n");

    if (IsHotspotActive() == WIFI_HOTSPOT_NOT_ACTIVE) {
        printf("Wifi Hotspot is not actived.\r\n");
        return -1;
    }
    printf("Wifi Hotspot is actived!\r\n");
    return 0;
}

static int at_start_softap(cmd_tbl_t *cmd, int argc, char *argv[])
{
    if (at_param_null_check(&argc, argv) == -1 || argc != 5) {
        return -1;
    }

    char* ap_ssid = argv[0];
    char* ap_psk = argv[1];
    int sec_type = atoi(argv[2]);
    int band_type = atoi(argv[3]);
    int channel = atoi(argv[4]);

    if(WifiAPStart(ap_ssid, ap_psk, sec_type, band_type, channel) != 0){
        return -1;
    }
    printf("OK\r\n");
    return 0;    
}

static int at_stop_softap(cmd_tbl_t *cmd, int argc, char *argv[])
{
    int ret = 0;

    if(DisableHotspot() != WIFI_SUCCESS) {
        return -1;
    }
    printf("OK\r\n");
    return 0;    
}
static int at_start_wifista(cmd_tbl_t *cmd, int argc, char *argv[])
{
    int ret = 0;
    ret = EnableWifi();

    printf("OK\r\n");
    return ret;    
}

static int at_stop_wifista(cmd_tbl_t *cmd, int argc, char *argv[])
{
    if (IsWifiActive() == WIFI_STA_NOT_ACTIVE) {
        printf("Wifi station is not actived.\n");
        return -1;
    }
    if(DisableWifi() != WIFI_SUCCESS) {
        return -1;
    }
    
    printf("OK\r\n");
    return 0;    
}

static int at_setup_dhcp(cmd_tbl_t *cmd, int argc, char *argv[])
{
    int ret = 0;

    if (at_param_null_check(&argc, argv) == -1) {
        return -1;
    }
    if (argc != 2) {
        return -1;
    }

    struct netif *netif_p = NULL;
    if (argc != 0) {
        if ((netif_p = netif_find(argv[0])) == NULL) {
            netif_p = netifapi_netif_find_by_name(argv[0]);
        }
    }
    if (netif_p == NULL) {
        return -1;
    }

    if (strcmp(argv[1], "1") == 0) {
        printf("netifapi_dhcp_start\r\n");
        ret = netifapi_dhcp_start(netif_p);
    } else if (strcmp(argv[1], "0") == 0) {
        printf("netifapi_dhcp_stop\r\n");
        ret = netifapi_dhcp_stop(netif_p);
    } else {
        printf("error param\r\n");
        return -1;
    }
    if (ret == LOS_OK) {
        printf("OK\r\n");
    }

    return ret;
}

static int at_setup_dhcps(cmd_tbl_t *cmd, int argc, char *argv[])
{
    int ret = 0;

    if (at_param_null_check(&argc, argv) == -1) {
        return -1;
    }
    if (argc != 2) {
        return -1;
    }    

    struct netif *netif_p = NULL;
    if (argc != 0) {
        if ((netif_p = netif_find(argv[0])) == NULL) {
            netif_p = netifapi_netif_find_by_name(argv[0]);
        }
    }
    if (netif_p == NULL) {
        return -1;
    }

    if (ip_addr_isany_val(netif_p->ip_addr)) {
        printf("Please set ip address for dhcp server\r\n");
        return -1;
    }
    if (strcmp(argv[1], "1") == 0) {
        printf("netifapi_dhcps_start\r\n");
        ret = netifapi_dhcps_start(netif_p, NULL, 0);
    } else if (strcmp(argv[1], "0") == 0) {
        printf("netifapi_dhcps_stop\r\n");
        ret = netifapi_dhcps_stop(netif_p);
    } else {
        printf("error param\r\n");
        return -1;
    }
    if (ret == LOS_OK) {
        printf("OK\r\n");
    }

    return ret;
}
static void RegisterCustomATCmd()
{
    cmd_tbl_t cmd_list[] = {
        {"AT+IFCFG", 8, at_lwip_ifconfig, "AT+IFCFG - ifconfig\n"},
        {"AT+STARTAP", 7, at_start_softap, "AT+STARTAP - start wifi softap\n"},
        {"AT+STOPAP", 1, at_stop_softap, "AT+STOPAP - stop wifi softap\n"},
        {"AT+STARTSTA", 1, at_start_wifista, "AT+STARTSTA - start wifi sta\n"},
        {"AT+STOPSTA", 1, at_stop_wifista, "AT+STOPSTA - stop wifi sta\n"},
        {"AT+DHCP", 3, at_setup_dhcp, "AT+DHCP - dhcp\n"},
        {"AT+DHCPS", 3, at_setup_dhcps, "AT+DHCPS - dhcps\n"},
    };
    for (int i = 0; i < sizeof(cmd_list) / sizeof(cmd_tbl_t); i++) {
        console_cmd_add(&cmd_list[i]);
    }
}
SYS_SERVICE_INIT(RegisterCustomATCmd);