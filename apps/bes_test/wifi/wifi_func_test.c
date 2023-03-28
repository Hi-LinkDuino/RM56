#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "app_utils.h"
#include "cmsis_os.h"
#include <assert.h>
#include "hal_trace.h"
#include "apps.h"
#include "net_memory.h"
#include "net_debug.h"
#include "bwifi_interface.h"
#include "beswifi.h"
#include "hal_timer.h"
#if LWIP_SUPPORT
#include "lwip/inet.h"
#endif

#define BES_WIFI_FUNC_TEST 1

#if BES_WIFI_FUNC_TEST
struct ap_info{
u8 ssid[33]  ;
u8 passwd[65];
u8 hidden    ;//1:hidden
};

struct ap_info ap_list[] =
{
    { "BESFD5.8G"     ,  "besfd888" , 0 },
    { "BESFD2.4G" ,  "besfd888" , 0 },
    { "\"junfenghuang\"的iPhone"     ,  "88888888"       , 0 },
};
#define MAX_AP_LIST  (sizeof(ap_list)/sizeof(ap_list[0]))

int  wifi_func_connect_network_test(void)
{
    int ret = 0;
    struct bwifi_station_config config;
    u8 macaddr[6] = {0};
    static int connect_num = 1;
    ret = bwifi_connect_to_ssid(ap_list[0].ssid, ap_list[0].passwd, 0, ap_list[0].hidden, NULL);
    if (ret)
    {
       TRACE(0, "%s connect to AP fail\n",__func__);
       ret = -1;
    }
    else
    {
        TRACE(0, "%s connect to AP success\n", __func__);
    }
    
    return ret;
}
int  wifi_func_connect_all_network_test(void)
{
    int ret   = 0;
    int index = 0;
    u8 macaddr[6] = {0};

    for(index = 0;index < MAX_AP_LIST;index++)
    {
        TRACE(0, "connect ssid[%d]:%s hidden:%d\n",index,ap_list[index].ssid,ap_list[index].hidden);
        if(ap_list[index].passwd[0] == 0)
        {
            TRACE(0, "key is null\n");
            ret = bwifi_connect_to_ssid(ap_list[index].ssid,NULL, 0, ap_list[index].hidden, NULL);
        }
        else
            ret = bwifi_connect_to_ssid(ap_list[index].ssid, ap_list[index].passwd, 0, ap_list[index].hidden, NULL);
        if (ret)
        {
            TRACE(0, "%s connect to %s fail\n",__func__,ap_list[index].ssid);
            ret = -1;
        }

        TRACE(0, "%s ap_list[%d]:%s\n",__func__,index,ap_list[index].ssid);

        osDelay(500);
        ret = bwifi_disconnect();
        if (ret)
        {
            TRACE(0, "%s disconnect to %s fail\n",__func__,ap_list[index].ssid);
        }
        TRACE(0, "%s disconnect to %s finished\n",__func__,ap_list[index].ssid);
    }

    return ret;
}

int  wifi_func_static_ip_all_test(void)
{
#if LWIP_SUPPORT
    int ret   = 0;
    int index = 0;
    struct ip_info ip;
    char ip_addr[]="192.168.0.118";
    char    mask[]="255.255.255.0";
    char      gw[]="192.168.0.1";

    for(index = 0;index < MAX_AP_LIST;index++)
    {
        if(index == MAX_AP_LIST-1)
        {
            strcpy(ip_addr,"192.168.0.118");
            strcpy(gw,"192.168.0.1");
        }
        ret = inet_aton(ip_addr, &ip.ip) && inet_aton(mask, &ip.netmask) && inet_aton(gw, &ip.gw);
        if (ret == 0)
        {
            TRACE(0, "%s parameter set error\n",__func__);
            return 0;
        }

        ret = bwifi_set_static_ip(&ip);
        if (ret)
        {
            TRACE(0, "%s set static ip fail\n",__func__);
            return -1;
        }
        if(ap_list[index].passwd[0] =='\0')
            ret = bwifi_connect_to_ssid(ap_list[index].ssid, NULL, 0, ap_list[index].hidden, NULL);
        else
            ret = bwifi_connect_to_ssid(ap_list[index].ssid, ap_list[index].passwd, 0, ap_list[index].hidden, NULL);
        if (ret)
        {
            TRACE(0, "%s connect to AP fail\n",__func__);
            ret = -1;
        }

        osDelay(500);
        ret = bwifi_disconnect();
        if (ret)
        {
            TRACE(0, "%s disconnect to %s fail\n",__func__,ap_list[index].ssid);
            return ret;
        }
        TRACE(0, "%s disconnect to %s finished\n",__func__,ap_list[index].ssid);
    }
#endif//LWIP_SUPPORT

    return 0;
}

int  wifi_func_dhcp_ip(void)
{
#if LWIP_SUPPORT
    int ret=0;

    ret = bwifi_set_static_ip(NULL);
    if (ret) {
        TRACE(0, "%s set static ip fail\n",__func__);
        return -1;
    }
    else
    {
        TRACE(0, "%s set static ip success\n", __func__);
    }

#endif//LWIP_SUPPORT

    return 0;
}

int  wifi_func_disconn_network_test(void)
{
    int ret = 0;

    ret = bwifi_disconnect();
    if (ret)
    {
        TRACE(0, "\n%s disconnect to AP fail\n",__func__);
        return ret;
    }
    else
        TRACE(0, "\n%s disconnect to AP success\n", __func__);

    return ret;
}

int  wifi_func_scanning_test(void)
{
    int i;
    int ret = 0;
    struct bwifi_scan_config scan_config;
    struct bwifi_bss_info *scan_result = NULL;

    //bwifi_switch_channel(0);
    ret = bwifi_scan();
    scan_result = (struct bwifi_bss_info *)nts_malloc(ret*sizeof(struct bwifi_bss_info));
    TRACE(0, "scan_result_num:%d\n", ret);
    if (scan_result == NULL)
    {
        TRACE(0, "malloc scan result buf fail");
        return 0;
    }

    ret = bwifi_get_scan_result(scan_result, ret);
    for (i = 0; i < ret; ++i)
    {
        TRACE(0, "scan result%d:(ch:%d,\"%s\",%d,\""MACSTR"\")", i,
              scan_result[i].channel, scan_result[i].ssid, scan_result[i].rssi,
              MAC2STR(scan_result[i].bssid));
    }
    nts_free(scan_result);

    TRACE(0, "\n%s wifi scan finish\n",__func__);

    return 0;
}

int  wifi_func_online_scanning_test(void)
{
    int i;
    int ret = 0;
    struct bwifi_bss_info *scan_result = NULL;

    ret = bwifi_scan();
    scan_result = (struct bwifi_bss_info *)nts_malloc(ret*sizeof(struct bwifi_bss_info));

    if (scan_result == NULL)
    {
        TRACE(0, "malloc scan result buf fail");
        return 0;
    }

    ret = bwifi_get_scan_result(scan_result, ret);
    for (i = 0; i < ret; ++i)
    {
        TRACE(0, "scan result%d:(ch:%d,\"%s\",%d,\""MACSTR"\")", i,
              scan_result[i].channel, scan_result[i].ssid, scan_result[i].rssi,
              MAC2STR(scan_result[i].bssid));
    }
    TRACE(0, "scan_result_num:%d\n", ret);
    nts_free(scan_result);
    TRACE(0, "\n %s finish", __func__ );

    return 0;
}

int  wifi_func_scan_special_ap_test(void)
{
    struct bwifi_ssid scan_ssid;
    struct bwifi_scan_config scan_config;
    int ret = 0;

    strcpy((char *)scan_ssid.ssid, "BESFD5.8G");
    scan_ssid.next = NULL;
    scan_config.ssids = &scan_ssid;
    scan_config.channels = NULL;

    ret = bwifi_config_scan(&scan_config);
    if (ret < 0)
    {
        TRACE(0, "%s scan for specific AP %s fail\n", __func__, scan_ssid.ssid);
        return ret;
    }
    TRACE(0, "%s num:%d\n",__func__,ret);

    return 0;
}

int  wifi_func_scan_channel_test(void)
{
    int scan_chs[2] = {0, 0};
    struct bwifi_scan_config scan_config;
    int ret = 0;
    scan_chs[0] = 5;
    scan_config.ssids = NULL;
    scan_config.channels = scan_chs;

    ret = bwifi_config_scan(&scan_config);
    if (ret < 0)
    {
        TRACE(0, "%s scan on specific channel %d fail\n", __func__, scan_chs[0]);
        return ret;
    }
    else
    {
        TRACE(0, "%s scan on specific channel %d success\n", __func__, scan_chs[0]);
    }

    return 0;
}

int  wifi_func_enable_ps_mode_test(void)
{
    int ret = 0;

    if (bwifi_set_ps_mode(1))
    {
        TRACE(0, "\n%s fail\n", __func__);
        return -1;
    }
    else
    {
        TRACE(0, "\n%s  success\n", __func__);
    }

    return ret;
}

int  wifi_func_disable_ps_mode_test(void)
{
    int ret = 0;

    if (bwifi_set_ps_mode(0))
    {
        TRACE(0, "\n%s fail\n", __func__);
        return -1;
    }
    else
    {
        TRACE(0, "\n%s  success\n", __func__);
    }

    return ret;
}
#endif//BES_WIFI_FUNC_TEST


int bes_wifi_func_test( void )
{
#if BES_WIFI_FUNC_TEST
    wifi_func_disconn_network_test();
    wifi_func_scanning_test();
    wifi_func_scan_channel_test();
    wifi_func_enable_ps_mode_test();
    wifi_func_disable_ps_mode_test();
    wifi_func_static_ip_all_test();
    wifi_func_dhcp_ip();
    wifi_func_online_scanning_test();
    wifi_func_scan_special_ap_test();
    wifi_func_connect_all_network_test();
    wifi_func_connect_network_test();
#endif//BES_WIFI_FUNC_TEST

    return 0;
}
