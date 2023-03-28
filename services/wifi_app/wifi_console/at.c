/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include <arpa/inet.h>
#include "cmsis_os.h"
#include "console.h"
#include "at.h"
#include "ping.h"
#include "iperf.h"
#include <netdb.h>
#include "bwifi_interface.h"
#ifdef __SNIFFER_MODE__
#include "bes_sniffer.h"
#endif
#ifdef BES_AIRKISS_SUPPORT
#include "bes_airkiss.h"
#endif

#ifdef __WIFI_NVR_SUPPORT__
#include "nvrecord_wifi.h"
#endif
#include "hexdump.h"
#include "net_memory.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "cmsis_os.h"
#ifdef osCMSIS_FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#endif
#if BES_OTA_ENABLED
#include "ota_handler.h"
#include "crc32.h"
#include "hal_norflash.h"
#endif
#include "net_misc.h"
#if (defined(__A7_DSP_ENABLE__) || defined(MCU_DSP) || defined(CP_DSP))
#include "mcu_audio.h"
#include "data_dump.h"
#endif
#ifdef __A7_DSP_ENABLE__
#include "a7_cmd.h"
#endif
#if defined(__NET_FTP_CLIENT_SUPPORT__)
#include "ftp_client.h"
#endif

#include "hal_timer.h"
#include "hal_phyif.h"

#ifdef IR_FUNCTION_TEST
#include "app_ir_test.h"
#endif

#include"hal_bootmode.h"
#include "factory_section.h"
#include "wifi_socket.h"
#include "nvrecord_env.h"

#ifdef CHIP_BEST2001
#include "hal_cmu_best2001.h"
#include "reg_cmu_best2001.h"
#endif
#ifdef CHIP_BEST2003
#include "hal_cmu_best2003.h"
#include "reg_cmu_best2003.h"
#endif

#ifdef IBRT
#include "app_ibrt_if.h"
#endif

#if RF_EFUSE_TEST
#include "factory_efuse.h"
#endif


#ifdef __CHIP_OCD__
#include "ocd_wp.h"

static const char * const watchpoint_rw_strings[] = {
    "read",
    "write",
    "access",
    "other"
};
#endif

#ifdef DEBUG_SPP_LOG
#include "at_spp.h"
#endif

#ifdef CMSIS_API_TEST
#include "cmsis_test.h"
#endif

#include "usb_audio.h"
#ifdef __NET_TELNET_SERVER_SUPPORT__
#include "telnetserver.h"
#endif

#include "wifi_drv.h"

#define MAX_LEN 128

#define HW_READ_REG(a) *(volatile uint32_t *)(a)
#define HW_WRITE_REG(a,v) *(volatile uint32_t *)(a) = v

static uint32_t StringToHex(char *str, unsigned char *out, unsigned int *outlen)
{
    char *p = str;
    char high = 0, low = 0;
    int tmplen = strlen(p), cnt = 0;
    uint32_t result = 0;
    tmplen = strlen(p);
    while(cnt < (tmplen / 2))
    {
        high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
        out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
        p ++;
        result |= out[cnt] << (cnt *8);
        cnt ++;
    }

    if(tmplen % 2 != 0)
    {
        out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
        result |= out[cnt] << (cnt *8);
    }

    if (outlen != NULL)
        *outlen = tmplen / 2 + tmplen % 2;

    return result;
}

u8 charTodata(const char ch)
{
    switch(ch)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a':
    case 'A': return 10;
    case 'b':
    case 'B': return 11;
    case 'c':
    case 'C': return 12;
    case 'd':
    case 'D': return 13;
    case 'e':
    case 'E': return 14;
    case 'f':
    case 'F': return 15;
    }
    return 0;
}

void str2mac(const char * szMac,uint8_t * pMac)
{
    const char * pTemp = szMac;
    for (int i = 0;i < 6;++i)
    {
        pMac[i] = charTodata(*pTemp++) * 16;
        pMac[i] += charTodata(*pTemp++);
        pTemp++;
    }
}
int do_msleep(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    unsigned int interval;
    if (argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    interval = atoi(argv[1]);
    osDelay(interval);
    RESP_OK_EQU(handler, "sleep %u ms\n", interval);
    return 0;
}

int do_add_conf( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    u8 bssid[ETH_ALEN];
    struct bwifi_station_config config;
    memset(&config, 0, sizeof(config));
    if (argc > 1)
        strcpy((char *)config.ssid, argv[1]);

    if (argc > 2)
        strcpy((char *)config.passwd, argv[2]);

    if (argc > 3)
        if (!strcmp(argv[3], "hidden"))
            config.hidden = 1;

    if (argc > 4 && strlen(argv[4])==17) {
        str2mac(argv[4], bssid);
        if (!net_is_valid_mac(bssid)) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
        memcpy(config.bssid, bssid, ETH_ALEN);
    }

    if (argc > 5)
        config.wep_keyid = atoi(argv[5]);

    ret = bwifi_add_config(&config);
    if (ret < 0) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;

}

int do_set_conf( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = -1;
    struct bwifi_config_item item;

    if (argc != 4) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    item.net_index = atoi(argv[1]);
    if (item.net_index < 0) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    if (!strcmp(argv[2], "wapi_pri_key")) {
        item.item_index = WIFI_CONFIG_ITEM_WAPI_CERT_PRIVATE_KEY;
    } else if(!strcmp(argv[2], "wapi_as_cert")) {
        item.item_index = WIFI_CONFIG_ITEM_WAPI_CERT_AS_CERT;
    } else if(!strcmp(argv[2], "wapi_user_cert")) {
        item.item_index = WIFI_CONFIG_ITEM_WAPI_CERT_ASUE_CERT;
    } else {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    item.val_len = strlen(argv[3]);
    if (item.val_len + 1 > BWIFI_CONFIG_VAL_LEN) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    strncpy((char *)item.val, argv[3], BWIFI_CONFIG_VAL_LEN - 1);
    ret = bwifi_modify_config(&item);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    RESP_OK(handler);
    return 0;
}

int do_save_conf( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    u8 bssid[ETH_ALEN];
    struct bwifi_station_config config;
    memset(&config, 0, sizeof(config));
    if (argc > 1)
        strcpy((char *)config.ssid, argv[1]);

    if (argc > 2)
        strcpy((char *)config.passwd, argv[2]);

    if (argc > 3)
        if (!strcmp(argv[3], "hidden"))
            config.hidden = 1;

    if (argc > 4 && strlen(argv[4])==17) {
        str2mac(argv[4], bssid);
        if (!net_is_valid_mac(bssid)) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
        memcpy(config.bssid, bssid, ETH_ALEN);
    }

    if (argc > 5)
        config.wep_keyid = atoi(argv[5]);

    ret = bwifi_save_config(&config);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;

}

int do_del_conf( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    u8 bssid[ETH_ALEN];
    struct bwifi_station_config config;
    memset(&config, 0, sizeof(config));

    if (argc == 2 && !strcasecmp(argv[1], "all")) {
        AT_PRINT("del all config\n");
        ret = bwifi_del_all_config();
        if (ret) {
            RESP_ERROR(handler, ERROR_FAILE);
        } else {
            RESP_OK_EQU(handler, "del config success\n");
        }
        return 0;
    }

    if (argc > 1)
        strcpy((char *)config.ssid, argv[1]);

    if (argc > 2)
        strcpy((char *)config.passwd, argv[2]);

    if (argc > 3)
        if (!strcmp(argv[3], "hidden"))
            config.hidden = 1;

    if (argc > 4 && strlen(argv[4])==17) {
        str2mac(argv[4], bssid);
        if (!net_is_valid_mac(bssid)) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
        memcpy(config.bssid, bssid, ETH_ALEN);
    }

    if (argc > 5)
        config.wep_keyid = atoi(argv[5]);
    else
        config.wep_keyid = -1;

    ret = bwifi_del_config(&config);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;
}

#define MAIN_CONFIG_COUNT 8
int do_get_current_conf( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int i;
    struct bwifi_station_config config[MAIN_CONFIG_COUNT];
    if(argc==2 && argv[1][0] == '?') {
        int num = bwifi_get_current_config(config, MAIN_CONFIG_COUNT);
        for (i=0; i<num; ++i) {
            RESP(handler, "cur_conf:%d,ssid:%s,bssid:"MACSTR",hid:%d,pwd:%s,keyid:%d\n",
                i, config[i].ssid, MAC2STR(config[i].bssid), config[i].hidden, config[i].passwd, config[i].wep_keyid);
        }
        RESP_OK_EQU(handler, "current config num:%d\n", num);
    } else {
        RESP_ERROR(handler, ERROR_ARG);
    }
    return 0;
}

int do_get_saved_conf( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int i;
    struct bwifi_station_config config[MAIN_CONFIG_COUNT];
    if(argc==2 && argv[1][0] == '?') {
        int num = bwifi_get_saved_config(config, MAIN_CONFIG_COUNT);
        for (i=0; i<num; ++i) {
            RESP(handler, "conf:%d,ssid:%s,bssid:"MACSTR",hid:%d, pwd:%s, keyid:%d\n",
                i, config[i].ssid, MAC2STR(config[i].bssid), config[i].hidden, config[i].passwd, config[i].wep_keyid);
        }
        RESP_OK_EQU(handler, "saved config num:%d\n", num);
    } else {
        RESP_ERROR(handler, ERROR_ARG);
    }
    return 0;
}

int do_scan( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int i;
    int ret = 0;
    struct bwifi_ssid *scan_ssid, *prev_scan_ssid;
    struct bwifi_scan_config scan_config;
    struct bwifi_bss_info *scan_result;

    if (argc == 1) {
        ret = bwifi_scan();
        if (ret < 0) {
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }
        RESP_OK(handler);
        return 0;
    }

    if (argc==2 && argv[1][0] == '?') {
        ret = bwifi_scan();
        if (ret < 0) {
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }
        scan_result = (struct bwifi_bss_info *)malloc(ret*sizeof(struct bwifi_bss_info));
        if (scan_result == NULL) {
            TRACE(0, "malloc scan result buf fail");
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }
        ret = bwifi_get_scan_result(scan_result, ret);
#ifndef WIFI_SCAN_DUMP
        for (i = 0; i < ret; ++i) {
            TRACE(0, "scan result%d:(ch:%d,\"%s\",%d,\""MACSTR"\")", i,
                  scan_result[i].channel, scan_result[i].ssid, scan_result[i].rssi,
                  MAC2STR(scan_result[i].bssid));
        }
#endif
        RESP_OK_EQU(handler, "scan result num:%d\n", ret);
        free(scan_result);
        return 0;
    }

    if(argc==2 && strcmp(argv[1], "_FLUSH")==0) {
        bwifi_flush_scan_result();
        RESP_OK(handler);
        return 0;
    }

    prev_scan_ssid = NULL;
    for (i = 1; i < argc; i++) {
        TRACE(0, "scan for specific SSID %s", argv[i]);
        scan_ssid = (struct bwifi_ssid *)malloc(sizeof(struct bwifi_ssid));
        if (scan_ssid == NULL) {
            TRACE(0, "malloc scan ssid struct fail");
            RESP_ERROR(handler, ERROR_FAILE);
            goto end;
        }
        strcpy((char *)scan_ssid->ssid, argv[i]);
        scan_ssid->next = NULL;
        if (prev_scan_ssid)
            prev_scan_ssid->next = scan_ssid;
        else
            scan_config.ssids = scan_ssid;
        prev_scan_ssid = scan_ssid;
    }

    scan_config.channels = NULL;
    ret = bwifi_config_scan(&scan_config);
    if (ret < 0)
        RESP_ERROR(handler, ERROR_FAILE);
    else
        RESP_OK(handler);

end:
    scan_ssid = scan_config.ssids;
    while (scan_ssid) {
        prev_scan_ssid = scan_ssid;
        scan_ssid = scan_ssid->next;
        free(prev_scan_ssid);
    }
    return 0;
}


int do_get_connected_state( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    char ssid[32+1];
    u8 bssid[ETH_ALEN];
    u8 channel;
    int8_t rssi;
#if LWIP_SUPPORT
    struct ip_info ip;
#endif
    if (argc == 2 && argv[1][0] == '?') {
        BWIFI_STATUS_T status = bwifi_get_current_status();
        RESP(handler, "current connected status: %d\n", status);
#if LWIP_SUPPORT
        if (status >= BWIFI_STATUS_GOT_IP) {
#else
        if (status >= BWIFI_STATUS_CONNECTED) {
#endif
            RESP(handler, "at connnected status\n");
            ret = bwifi_get_current_ssid(ssid);
            if (!ret)
                RESP(handler, "current ssid:%s\n", ssid);
            else
                RESP(handler, "get current ssid fail:%d\n", ret);

            ret = bwifi_get_current_bssid(bssid);
            if (!ret)
                RESP(handler, "current bssid:"MACSTR"\n", MAC2STR(bssid));
            else
                RESP(handler, "get current bssid fail:%d\n", ret);

            channel = bwifi_get_current_channel();
            if (channel>0)
                RESP(handler, "current channel:%d\n", channel);
            else
                RESP(handler, "get current channel fail:%d\n", channel);

            rssi = bwifi_get_current_rssi();
            if (rssi != -128)
                RESP(handler, "current rssi:%d\n", rssi);
            else
                RESP(handler, "get current rssi fail:%d\n", rssi);

#if LWIP_SUPPORT
            ret = bwifi_get_current_ip(&ip);
            if (!ret) {
                struct in_addr addr;
                addr.s_addr = ip.ip;
                RESP(handler, "get current ip:%s\n", inet_ntoa(addr));
                addr.s_addr = ip.netmask;
                RESP(handler, "get current mask:%s\n", inet_ntoa(addr));
                addr.s_addr = ip.gw;
                RESP(handler, "get current gw:%s\n", inet_ntoa(addr));
            } else
                RESP(handler, "get current ip fail:%d\n", ret);
#endif
        } else {
            RESP(handler, "current status(%d) is not connected\n", status);
            RESP_ERROR(handler, ERROR_FAILE);
        }
        return 0;
    } else {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

}
int do_conn( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    char *ssid = NULL;
    char *passwd = NULL;
    int8_t wep_keyid = 0;
    u8 hidden = 0;
    u8 bssid[ETH_ALEN];
    u8 mfp = WIFI_MFP_DEFAULT;
//  u8 channel;
//  int8_t rssi;
//  struct ip_info ip;

    if (argc > 1)
        ssid = argv[1];

    if (argc > 2)
        passwd = argv[2];

    if (argc > 3)
        if (!strcmp(argv[3], "hidden"))
            hidden = 1;

    if (argc > 4 && strlen(argv[4])==17) {
        str2mac(argv[4], bssid);
        if (!net_is_valid_mac(bssid)) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
    } else
        memset(bssid, 0, ETH_ALEN);

    if (argc > 5)
        wep_keyid = atoi(argv[5]);

    if(argc > 6) {
        if(!strcmp(argv[6], "disable")) {
            mfp = WIFI_MFP_DISABLE;
        } else if(!strcmp(argv[6], "enable")) {
            mfp = WIFI_MFP_ENABLE;
        } else if(!strcmp(argv[6], "require")) {
            mfp = WIFI_MFP_REQUIRED;
        } else if(!strcmp(argv[6], "default")) {
            mfp = WIFI_MFP_DEFAULT;
        } else {
            cmd_printf(handler, "parse mfp error\r\n");
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
    }
    if(argc > 6) {
        ret = bwifi_connect_to_ssid_with_mfp(ssid, passwd, wep_keyid, hidden, bssid, mfp);
    } else {
        ret = bwifi_connect_to_ssid(ssid, passwd, wep_keyid, hidden, bssid);
    }

    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;
}
extern osThreadDef_t os_thread_def__reconnect_thread;
int do_auto_conn( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
#if 0
    wpa_supplicant_enable_all_network(&g_wpa_s, 1);
    ret = osThreadCreate(osThread(_reconnect_thread), NULL);
    //ret = bwifi_auto_connect();
    if (ret == NULL) {
        RESP(handler, "create reconnect thread fail\n");
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
#else
    ret = bwifi_auto_connect();
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
#endif
    RESP_OK(handler);
    return 0;

}

int do_netid_conn( cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    u8 net_id = 0;

    if(argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    net_id = atoi(argv[1]);
    ret = bwifi_connect_to_nid(net_id);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    RESP_OK(handler);
    return 0;
}

int do_disconn(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    ret = bwifi_disconnect();
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;
}

int do_set_reconnect_policy(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int enable;
//  struct ip_info ip;

    if (argc != 2)
        RESP_ERROR(handler, ERROR_ARG);

    enable = atoi(argv[1]);
    bwifi_set_reconnect_policy(enable ? true : false);
    RESP_OK(handler);
    return 0;
}

int do_airkiss(cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
#ifdef BES_AIRKISS_SUPPORT
    int ret;
    uint8_t token;
    struct bwifi_station_config wificfg;

    bwifi_init();

    memset(&wificfg, 0, sizeof(wificfg));
    ret = bes_airkiss_start(wificfg.ssid, wificfg.passwd, &token);
    if (ret != 0) {
        RESP(handler, "airkiss failed");
        RESP_ERROR(handler, -1);
        return 0;
    }
    if (strlen((char *)wificfg.passwd)) {
        //decrypt the passwd if you use airkiss for wechat app
    }
    if (bwifi_connect(&wificfg) != 0) {
        RESP(handler, "connect to AP fail");
        RESP_ERROR(handler, -1);
        return 0;
    }

    ret = bwifi_save_config(&wificfg);
    if (ret) {
        RESP(handler, "failed to save config");
    }
    osDelay(1000);
    airkiss_notify(token);
#endif
    RESP_OK(handler);
    return 0;
}

int do_get_link_signal(cmd_tbl_t *cmdtp, int argc, char *argv[], void *handler)
{
    int8_t rssi;

    if (argc == 2 && argv[1][0] == '?') {
        BWIFI_STATUS_T status = bwifi_get_current_status();
        if (status >= BWIFI_STATUS_CONNECTED) {
            rssi = bwifi_get_current_rssi();
            RESP(handler, "link signal: %d dBm.\n", rssi);
            RESP_OK(handler);
        } else {
            RESP(handler, "current status(%d) is not connected\n", status);
            RESP_ERROR(handler, ERROR_FAILE);
        }
        return 0;
    } else {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
}

#if LWIP_SUPPORT
int do_set_static_ip(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    int enable;
    struct ip_info ip;
	ip_addr_t dnsip;

    if (argc != 2 && argc != 5 && argc != 6) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    enable = atoi(argv[1]);
    if (enable == 0) {
        ret = bwifi_set_static_ip(NULL);
		TRACE(0, "bwifi_set_static_ip ret=%d", ret);
        if (ret) {
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }
    } else {
        if (argc != 5 && argc != 6) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }

        ret = inet_aton(argv[2], &ip.ip) &&
            inet_aton(argv[3], &ip.netmask) &&
            inet_aton(argv[4], &ip.gw);
        if (ret == 0) {
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
		
        ret = bwifi_set_static_ip(&ip);
        if (ret) {
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }

		// dns 
		if (argc == 6) {
			ret = inet_aton(argv[5], &dnsip);
			if (ret == 0) {
            	RESP_ERROR(handler, ERROR_ARG);
            	return 0;
			}
        } else 
        	inet_aton("202.96.134.133", &dnsip);

		extern void dns_setserver(u8_t numdns, const ip_addr_t *dnsserver);
		dns_setserver(0, &dnsip);
    }
    RESP_OK(handler);
    return 0;
}

int do_ndns(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    ip_addr_t addr;
    BWIFI_STATUS_T status;
    BWIFI_SOFTAP_STATUS_T softap_status;

    if (argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    status = bwifi_get_current_status();
    softap_status = bwifi_get_softap_status();
    if ((status < BWIFI_STATUS_GOT_IP) && (softap_status != BWIFI_SOFTAP_STATUS_ON)) {
        RESP(handler, "current status(%d) is not connected and softap status(%d) is not started\n",
             status, softap_status);
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    struct hostent * host = gethostbyname(argv[1]);
    if (!host){
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    addr = *(ip_addr_t *)(host->h_addr_list[0]);
    struct in_addr addr_ip;
    addr_ip.s_addr = addr.addr;
    RESP_OK_EQU(handler, "%s\r\n", inet_ntoa(addr_ip));
    return 0;
}

void test_tcp_rx(void);
void test_udp_rx(void);
void test_tcp_tx(int argc, char *argv[]);
void test_udp_tx(int argc, char *argv[]);
int do_tcp_server(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    test_tcp_rx();
    RESP_OK(handler);
    return 0;
}
int do_udp_server(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    test_udp_rx();
    RESP_OK(handler);
    return 0;
}
int do_tcp_client(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 4) {
        RESP_ERROR(handler, ERROR_ARG);
        cmd_printf(handler, "AT+NTCPCLI=\"ip address\",\"port\",\"time_in_sec\"\n");
        return 0;
    }

    test_tcp_tx(argc,argv);
    RESP_OK(handler);
    return 0;
}
int do_udp_client(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 3) {
        RESP_ERROR(handler, ERROR_ARG);
        cmd_printf(handler, "AT+NUDPCLI=\"ip address\",\"port\"\n");
        return 0;
    }

    test_udp_tx(argc,argv);
    RESP_OK(handler);
    return 0;
}

static int do_ifconfig(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc == 1) {
#if (LWIP_VER_NUM >= 212) || !defined(LWIP_VER_NUM)
        struct netif *netif;
        NETIF_FOREACH(netif) {
            cmd_printf(handler, "\n%c%c%d: flags=%u< ", netif->name[0], netif->name[1], netif->num, netif->flags);
            if (netif->flags & NETIF_FLAG_UP)
                cmd_printf(handler, "UP ");

            if (netif->flags & NETIF_FLAG_BROADCAST)
                cmd_printf(handler, "BROADCAST ");

            if (netif->flags & NETIF_FLAG_LINK_UP)
                cmd_printf(handler, "RUNNING ");

            if (netif->flags & NETIF_FLAG_IGMP)
                cmd_printf(handler, "MULTICAST ");

            cmd_printf(handler, "> mtu %u\n", netif->mtu);
            struct in_addr addr;
            addr.s_addr = netif->ip_addr.addr;
            cmd_printf(handler, "\tinet %s ", inet_ntoa(addr));
            addr.s_addr = netif->netmask.addr;
            cmd_printf(handler, "netmask %s ", inet_ntoa(addr));
            addr.s_addr = netif->gw.addr;
            cmd_printf(handler, "gateway %s\n", inet_ntoa(addr));
            cmd_printf(handler, "\tether %02x:%02x:%02x:%02x:%02x:%02x\n",
                netif->hwaddr[0], netif->hwaddr[1], netif->hwaddr[2],
                netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);
        }
        RESP_OK(handler);
#else
        RESP_ERROR(handler, ERROR_ARG);
#endif
    } else if (argc == 5) {
#if (LWIP_VER_NUM >= 212) || !defined(LWIP_VER_NUM)
        u8_t ifindex = netif_name_to_index(argv[1]);
        if (ifindex == 0) {
            cmd_printf(handler, "unknown interface: %s\n", argv[1]);
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
        ip4_addr_t ip, netmask, gw;
        int ret = inet_aton(argv[2], &ip);
        ret += inet_aton(argv[3], &netmask);
        ret += inet_aton(argv[4], &gw);
        if (ret != 3) {
            cmd_printf(handler, "inet_aton error\n");
            RESP_ERROR(handler, ERROR_ARG);
        }
        netif_set_addr(netif_get_by_index(ifindex), &ip, &netmask, &gw);
        RESP_OK(handler);
#else
        RESP_ERROR(handler, ERROR_ARG);
#endif
    } else {
        RESP_ERROR(handler, ERROR_ARG);
    }
    return 0;
}
#endif
#ifdef NET_PING_SUPPORT
int do_nping(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    int count, interval, len, echo_level;
    in_addr_t ping_dst;
    if (argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    if (argc >= 3)
        count = atoi(argv[2]);
    else
        count = 4;

    if (argc >= 4)
        interval = atoi(argv[3]);
    else
        interval = 1000;

    if (argc >= 5)
        len = atoi(argv[4]);
    else
        len = 32;

    if (argc >= 6)
        echo_level = atoi(argv[5]);
    else
        echo_level = 1;

    if (count < 0 || interval < 0 || len < 0) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    if (interval < 10 )
        interval = 10;
    else if (interval > 10*1000)
        interval = 10*1000;

    if (len > 1460)
        len = 1460;

    if (echo_level > 0)
        echo_level = 1;
    else
        echo_level = 0;

    // if dest is x.x.x.x
    ret = inet_aton(argv[1], &ping_dst);
    if (!ret) {
        // if dest needs dns resolving
        struct hostent * host = gethostbyname(argv[1]);
        if (!host){
            RESP_ERROR(handler, ERROR_FAILE);
            return 0;
        }
        ping_dst = *(in_addr_t *)(host->h_addr_list[0]);
    }

    ping_init(&ping_dst, count, interval, len, echo_level);
    RESP_OK(handler);
    return 0;
}
#endif
#ifdef WIFI_IPERF_SUPPORT
int do_iperf(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (iperf_main(argc, argv)) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;
}
#endif

#ifdef NET_WIFI_SOCKET_SUPPORT

int do_wifi_socket(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s\r\n",__func__);
    struct WIFI_SOCKET_C wifisocket;
    if(argc < 2)
    {
        return -1;
    }
    for (int i = 0; i < argc; i++)
    {
        cmd_printf(handler, " argv[%d] = %s  \n",i,argv[i]);
        if (i > 0)
        {
            char *p = argv[i];
            if ( *p == 45) /* '-' */
            {
                p++;
                char opt = *p;
                char optarg[MAX_LEN] = {0};
                p++;
                int j = 0;
                for (; *p != '\0'; j++)
                {
                    optarg[j] = *p;
                    p++;
                }
                switch(opt) {
                    case 's':
                        memcpy(wifisocket.wifi_socket_ip, optarg, j);
                        break;
                    case 'p':
                        wifisocket.wifi_socket_port = atoi(optarg);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return wifi_socket_start(&wifisocket);
}

int do_srecord(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if(argc<2){
        cmd_printf(handler, "param error\r\n");
        return -1;
    }
    char state = atoi(argv[1]);
    set_wifi_audio_state((bool)state);
    return 0;
}

#endif

int hal_analogif_reg_read(unsigned short reg, unsigned short *val);
int hal_analogif_reg_write(unsigned short reg, unsigned short val);
#define MAX_USERNAME_LEN 20
#define MAX_PASSWD_LEN 30

int do_mem_read(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
	//AT+MD=0x20000000,128,192.168.0.111,21,ad,ad,dump.hex
	//		 addr,len(bit),[ftp_ip,port,username,password,filename]
    uint32_t addr, len;
    unsigned short reg_val;
    uint32_t val;
    if (argc != 3 && argc !=8) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    addr = strtoul(argv[1], NULL, 16);
	len = atoi(argv[2]);
	// to check addr and len in future;
	if (argc == 3) {// output to terminal
		for(int i = 0; i < len; i+=32){
		    if (addr < 0x4000) {
		        hal_analogif_reg_read((unsigned short)addr, &reg_val);
				RESP(handler, "%04x", reg_val);
				hal_analogif_reg_read((unsigned short)addr+16, &reg_val);
				RESP(handler, "%04x", reg_val);
		    } else if (addr < 0x8000) {
		        addr -= 0x4000;
		        hal_phyif_reg_read((unsigned short)addr, &reg_val);
		        RESP(handler, "%04x", reg_val);
				hal_phyif_reg_read((unsigned short)addr+16, &reg_val);
		        RESP(handler, "%04x", reg_val);
		    } else {
		        val = *(volatile uint32_t *)addr;
		        RESP(handler, "%08x", (unsigned int)val);
		    }
			if ((i+32)%256 == 0)
				RESP(handler, "\n");
			else
				RESP(handler, "  ");
			addr +=4;
		}
	}else if (argc == 8) {// output to ftp
		char ftp_ip[20] = {0};
	    int ftp_port = 21;
	    char ftp_data_path[MAX_LEN] = {0};
	    char ftp_username[MAX_USERNAME_LEN] = {0};
	    char ftp_password[MAX_PASSWD_LEN] = {0};
		char *CMD = "AT+MD";

		memcpy(ftp_ip, argv[3], sizeof(argv[3]));
		ftp_port = atoi(argv[4]);
        memcpy(ftp_username,argv[5], sizeof(argv[5]));
        memcpy(ftp_password,argv[6], sizeof(argv[6]));
		memcpy(ftp_data_path,argv[7], sizeof(argv[7]));
		cmd_printf(handler, "[%s] argvs=%s, %d, %s, %s, %s !\n",CMD, argv[3], ftp_port, argv[5], argv[6], argv[7]);
		
		cmd_printf(handler, "[%s] argvs=%s, %d, %s, %s, %s !\n",CMD, ftp_ip, ftp_port, ftp_username, ftp_password, ftp_data_path);
		
		void *ftp_handler = NULL;
			
		ftp_handler = ftp_client_init();
		if (ftp_handler == NULL)
		{
			cmd_printf(handler, "ftp client init failed\n");
			return 0;;
		}
		cmd_printf(handler, "[%s] ip:%s port:%d\n",CMD,argv[3],ftp_port);
		if (ftp_connect_server(ftp_handler, argv[3], ftp_port) < 0)
		{
			cmd_printf(handler, "[%s] ftp connect server failed !\n",CMD);
			ftp_client_free(ftp_handler);
			return 0;;
		}
	
		if(ftp_login(ftp_handler, argv[5], argv[6]) < 0)
		{
			cmd_printf(handler, "[%s] ftp login on server failed !\n",CMD);
			ftp_disconnect(ftp_handler);
			ftp_client_free(ftp_handler);
			return 0;;
		}
	
		if(ftp_client_notice(ftp_handler,argv[7]) !=0)
		{
			TRACE(0, "[%s] notice ftp server %s fail\n", CMD,ftp_data_path);
			ftp_disconnect(ftp_handler);
			ftp_client_free(ftp_handler);
			return 0;
		}
			
		for(int i = 0; i < len; i+=32){
		    if (addr < 0x4000) {
		        hal_analogif_reg_read((unsigned short)addr, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				hal_analogif_reg_read((unsigned short)addr+16, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				TRACE(0, "[%s] ftp_client_dumpdata\n", CMD);
		    } else if (addr < 0x8000) {
		        addr -= 0x4000;
				hal_phyif_reg_read((unsigned short)addr, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				hal_phyif_reg_read((unsigned short)addr+16, &reg_val);
				ftp_client_dumpdata(ftp_handler, (const char *)&reg_val, 2);
				TRACE(0, "[%s] ftp_client_dumpdata\n", CMD);
		    } else {
		        val = *(volatile uint32_t *)addr;
		        ftp_client_dumpdata(ftp_handler, (const char *)&val, 4);
				TRACE(0, "[%s] ftp_client_dumpdata, addr=%08x, val=%08x\n", CMD, addr, val);
		    }
			addr +=4;

		}
		ftp_disconnect(ftp_handler);
    	ftp_client_free(ftp_handler);
	}
	RESP_OK(handler) ;
    return 0;
}
int do_mem_write(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint32_t addr;
    uint32_t val, check_val;
    unsigned short reg_val;
    if (argc != 3) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    addr = strtoul(argv[1], NULL, 16);
    val = strtoul(argv[2], NULL, 16);

    if (addr < 0x4000) {
        reg_val = val;
        hal_analogif_reg_write((unsigned short)addr, reg_val);
        hal_analogif_reg_read((unsigned short)addr, &reg_val);
        RESP_OK_EQU(handler, "write 0x%08x=0x%08x done .(check 0x%04x)\n", addr, (unsigned short)val, reg_val);
    } else if (addr < 0x8000) {
        addr -= 0x4000;
        reg_val = val;
        hal_phyif_reg_write((unsigned short)addr, reg_val);
        hal_phyif_reg_read((unsigned short)addr, &reg_val);
        RESP_OK_EQU(handler, "write 0x%08x=0x%08x done .(check 0x%04x)\n", addr, (unsigned short)val, reg_val);
    } else {
        *(volatile uint32_t *)addr = val;
        check_val = *(volatile uint32_t *)addr;
        RESP_OK_EQU(handler, "write 0x%08x=0x%08x done .(check 0x%08x)\n", addr, (unsigned int)val, (unsigned int)check_val);
    }
    return 0;
}

u8 get_channel_of_beaon(u8 *pu8msa, u16 u16RxLen)
{
    u16 index = 24 + 8 + 2 + 2;
        //MAC_HDR_LEN + TIME_STAMP_LEN + BEACON_INTERVAL_LEN + CAP_INFO_LEN;

    while (index < u16RxLen) {
        if (pu8msa[index] == 3)
        //if (pu8msa[index] == IDSPARMS)
            return (pu8msa[index + 2]);
        else
            /*
             * Increment index by length information and header
             */
            index += pu8msa[index + 1] + 2;
            //index += pu8msa[index + 1] + IE_HDR_LEN;
    }

    return 0;       /* no MIB here */
}

/*
  * @brief  This function is used to set fix rate funtion.
  *
  * @param  fix_rate_idx is argv[1]
  * set rate_index value is equal static struct ieee80211_rate cw1200_rates[].hw_value
  * and static struct ieee80211_rate cw1200_mcs_rates[].hw_value;
  * 0~13 is for cw1200_rates;
  * 14~21 if for cw1200_mcs_rates;
  * default close value is 0xff;
  * example: AT+WSSETRATE=0  is select ieee80211_rate cw1200_rates[].hw_value==0
  * example: AT+WSSETRATE=13  is select ieee80211_rate cw1200_rates[].hw_value==13
  * example: AT+WSSETRATE=14  is select ieee80211_rate cw1200_mcs_rates[].hw_value==14
  * example: AT+WSSETRATE=21  is select ieee80211_rate cw1200_mcs_rates[].hw_value==21
*/
int do_set_fix_rate(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint32_t  fix_rate_idx = 0;

    if (argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        cmd_printf(handler, "AT+WSSETRATE=\"rate idx\"\n");
        return 0;
    }
    fix_rate_idx = atoi(argv[1]);
    if (fix_rate_idx >= 22)
        fix_rate_idx = 0xff;
    cmd_printf(handler, "fix rate idx %d\n", fix_rate_idx);
    bwifi_set_fix_rate(fix_rate_idx);
    RESP_OK(handler);
    return 0;
}

#ifdef ENABLE_FW_KEEP_ALIVE
int do_set_keep_alive(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint16_t idx = 0, dest_port = 0, payload_len = 0;

    /* AT+KLVCFG=127,5005,16,helloworld888888
     * AT+KLVCFG=255,5005,16,helloworld888888
     * AT+KLVCFG=0,5005,16,helloworld888888
     */
    if (argc < 5) {
        RESP_ERROR(handler, ERROR_ARG);
        cmd_printf(handler, "AT+KLVCFG=\"stream idx,dest port, payload_len, payload\"\n");
        return 0;
    }
    idx = atoi(argv[1]);
    dest_port = atoi(argv[2]);
    payload_len = atoi(argv[3]);
#if 0
    cmd_printf(handler, "at: idx %d, port %d, payload_len %d, payload %s\n",
               idx, dest_port, payload_len, argv[4]);
#endif
    bwifi_set_ip_alive(idx, dest_port, payload_len, (uint8_t *)argv[4]);
    RESP_OK(handler);
    return 0;
}

int do_enable_keep_alive(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint16_t period = 0;

    /* AT+KLVEN=10
     * AT+KLVEN=0
     */
    if (argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        cmd_printf(handler, "AT+KLVEN=\"period\"\n");
        return 0;
    }
    period = atoi(argv[1]);
#if 0
    cmd_printf(handler, "at: period %d\n", period);
#endif
    bwifi_enable_ip_alive(period);
    RESP_OK(handler);
    return 0;
}
#endif

#ifdef __SNIFFER_MODE__
static int at_sniffer_handler(const u8 *data, u16 data_len, int signal)
{
    u16 seq;
    u8 type;
    char *type_c = NULL;
    const u8 *frame = data;

    if (data == NULL) {
        cmd_printf(NULL, "ldpc:%d\n", data_len);
    } else {
        type = frame[0] & 0xFC;
        switch (type) {
            case BEACON:
                //channel = get_channel_of_beaon((u8 *)frame, data_len);
                type_c = "BEACON";
                break;
            case QOS_DATA:
            case DATA:
                if ((frame[1] & 0x3) == 0x1)
                    type_c = "TODS_DATA";
                else
                    type_c = "FROMDS_DATA";
                break;
            case PROBE_REQ:
                type_c = "PROBE_REQ";
                break;
            case PROBE_RSP:
                type_c = "PROBE_RSP";
                break;
            case AUTH:
                type_c = "AUTH";
                break;
            case DEAUTH:
                type_c = "DEAUTH";
                break;
            case ASSOC_REQ:
                type_c = "ASSOC_REQ";
                break;
            case ASSOC_RSP:
                type_c = "ASSOC_RSP";
                break;
            case RTS:
                type_c = "RTS";
                break;
            case CTS:
                type_c = "CTS";
                break;
            case ACK:
                type_c = "ACK";
                break;
            case DATA_ACK:
                type_c = "DATA_ACK";
                break;
            case BLOCKACK:
                type_c = "BLOCKACK";
                break;
            case ACTION:
                type_c = "ACTION";
                break;
            case QOS_NULL_FRAME:
                type_c = "QOS_NULL_FRAME";
                break;
            case NULL_FRAME:
                type_c = "NULL_FRAME";
                break;
            case BLOCKACK_REQ:
                type_c = "BLOCKACK_REQ";
                break;
            case PS_POLL:
                type_c = "PS_POLL";
                break;
            default:
                type_c = "UNKNOWN";
                cmd_printf(NULL, "UNKNOWN type:0x%02x, len:%d\n", type, data_len);
                DUMP8("%02x ", data - 16, 16);
                DUMP8("%02x ", data, MIN(26, data_len));
                return 0;
        }

        if (data_len < 16)
            cmd_printf(NULL, "%s A1=" MACSTR " len:%d rssi:%d\n", type_c,
                   MAC2STR(frame + 4), data_len, signal);
        else if (data_len < 22)
            cmd_printf(NULL, "%s A1=" MACSTR " A2=" MACSTR ", len:%d, rssi:%d\n", type_c,
                   MAC2STR(frame + 4), MAC2STR(frame + 10), data_len, signal);
        else {
            seq = (frame[22] | (frame[23] << 8)) >> 4;
            cmd_printf(NULL, "%s A1=" MACSTR " A2=" MACSTR " A3=" MACSTR ", seq:%d, len:%d, rssi:%d\n", type_c,
                   MAC2STR(frame + 4), MAC2STR(frame + 10), MAC2STR(frame + 16),
                   seq, data_len, signal);
        }
#if 0
        {
            int i;
            for (i = 0; i < data_len; ++i)
                cmd_printf(handler, "%02x ", frame[i]);
            cmd_printf(handler, "\n");
        }
#endif
    }
    return 0;
}

int do_sniffer_start(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    int i;
    uint8_t f_mgmt=0, f_fromds=0, f_tods=0, f_error=0, f_ack=0, f_null=0, f_rts=0;

    if (argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    bes_sniffer_start(at_sniffer_handler);

    for (i = 1; i < argc; ++i) {
        //cmd_printf(handler, "%s\n", argv[i]);
        if (strcmp(argv[i], "MGMT") == 0)
            f_mgmt = 1;
        else if (strcmp(argv[i], "FROMDS") == 0)
            f_fromds = 1;
        else if (strcmp(argv[i], "TODS") == 0)
            f_tods = 1;
        else if (strcmp(argv[i], "LDPC") == 0)
            f_error = 1;
        else if (strcmp(argv[i], "ACK") == 0)
            f_ack = 1;
        else if (strcmp(argv[i], "NULL") == 0)
            f_null = 1;
        else if (strcmp(argv[i], "RTS") == 0)
            f_rts = 1;
        else {
            bes_sniffer_stop();
            RESP_ERROR(handler, ERROR_ARG);
            return 0;
        }
    }
    ret = bes_sniffer_set_filter(f_mgmt, f_fromds, f_tods, f_error, f_ack, f_rts, f_null);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    RESP_OK(handler);
    return 0;
}

int do_sniffer_stop(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc != 1) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    bes_sniffer_stop();
    RESP_OK(handler);
    return 0;
}

int do_send_mlme(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    u8 channel;
    if (argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    channel = atoi(argv[1]);
    u8 data[] = {
        0x40, 0x00, 0x00, 0x00, // probe req,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff,// des
        0x00, 0x03, 0x7f, 0x10, 0x40, 0x98,// src // it will be refilld by our mac addr
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff,// bssid
        0x00, 0x00, //seq num
        0x00, 0x00, //ssid
        0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,// support rate
        0x32, 0x04, 0x30, 0x48, 0x60, 0x6c, // extended support rate
        //0x00, 0x00, 0x00, 0x00, //fcs
        };
    ret = bes_sniffer_send_mgmt_frame(channel, (const u8 *)data, sizeof(data));
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    RESP_OK(handler);
    return 0;
}


int do_sniffer_set_channel(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    u8 channel;

    if (argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    channel = atoi(argv[1]);
    ret = bes_sniffer_set_channel(channel);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    RESP_OK(handler);
    return 0;
}
#endif
//int bwifi_find_record(const char *type, void *record);
//int bwifi_add_record(nvrec_wifidevicerecord *record);
//int bwifi_del_record(char *type);
#ifdef CHIP_BEST2000
int do_set_calibration_data(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    nvrec_wifidevicerecord record;

    if (argc != 4) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    memset(&record, 0, sizeof(nvrec_wifidevicerecord));

    record.wifi_record.cal.freq_cal = strtoul(argv[1], NULL, 16);
    record.wifi_record.cal.iQ_cal = strtoul(argv[2], NULL, 16);
    record.wifi_record.cal.iQ_offset = strtoul(argv[3], NULL, 16);
    strcpy(record.type, BWIFI_RECORD_TYPE_CAL);
    bwifi_del_record(BWIFI_RECORD_TYPE_CAL);
    ret = bwifi_add_record(&record);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }
    TRACE(0, "freq_cal:0x%x, iq_cal:0x%x, iq_offset:0x%x\n",
        record.wifi_record.cal.freq_cal,
        record.wifi_record.cal.iQ_cal, record.wifi_record.cal.iQ_offset);

    RESP_OK(handler);
    return 0;
}

int do_get_calibration_data(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    nvrec_wifidevicerecord record;

    if (argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    ret = bwifi_find_record(BWIFI_RECORD_TYPE_CAL, &record);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    RESP(handler, "freq_cal:0x%x, iq_cal:0x%x, iq_offset:0x%x iq_cal_5G:0x%x, iq_offset_5G:0x%x\n",
        record.wifi_record.cal.freq_cal,record.wifi_record.cal.iQ_cal, record.wifi_record.cal.iQ_offset,\
        record.wifi_record.cal.iQ_cal_5G, record.wifi_record.cal.iQ_offset_5G);
    RESP_OK(handler);
    return 0;
}
#endif

#ifdef __AP_MODE__
int do_start_ap(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    char *ssid = NULL;
    int channel = 0;
    int sec_channel_offset = 0;
    int hidden = 0;
    BWIFI_SEC_TYPE_T sec = BWIFI_SECURITY_NONE;
    char *p, *passwd = NULL;

    if (argc < 2)
        goto error;

    if (argc > 1 && strcmp("", argv[1])) /* ssid is not "" */
        ssid = argv[1];

    if (argc > 2) {
        p = strchr(argv[2], '-');
        if (p) {
            *p = '\0';
            sec_channel_offset = -1; //HT40-
        } else {
            p = strchr(argv[2], '+');
            if (p) {
                *p = '\0';
                sec_channel_offset = 1; //HT40+
            }
        }
        channel = atoi(argv[2]);
    }

    if (argc > 3)
        hidden = atoi(argv[3]);

    if (argc > 4) {
        if (!strcmp(argv[4], "wpa-psk"))
            sec = BWIFI_SECURITY_WPA;
        else if (!strcmp(argv[4], "wpa2-psk"))
            sec = BWIFI_SECURITY_WPA2;
        else if (!strcmp(argv[4], "open"))
            sec = BWIFI_SECURITY_NONE;
        else
            goto error;
    }

    if (argc > 5 && strcmp("", argv[5])) /* passwd is not "" */
        passwd = argv[5];

    if (!ssid || (sec != BWIFI_SECURITY_NONE && !passwd))
        goto error;

    if (bwifi_set_softap_config(ssid, (u8)channel, sec_channel_offset, (u8)hidden, sec, passwd))
        goto error;

    if (bwifi_softap_start())
        RESP_ERROR(handler, ERROR_FAILE);
    else
        RESP_OK(handler);

    return 0;

error:
    RESP_ERROR(handler, ERROR_ARG);
    TRACE(0, "%s AT+WASTART=\"ssid\",channel,hidden,\"security(open|wpa-psk|wpa2-psk)\",\"password\"",
          __func__);
    return 0;
}

int do_stop_ap(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    bwifi_softap_stop();
    RESP_OK(handler);
    return 0;
}

int do_get_sta_info(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    struct bwifi_mac_addr mac[10] = {0};
    int num, i;

    num = sizeof(mac) / sizeof(mac[0]);

    if (argc == 2 && argv[1][0] == '?') {
        BWIFI_SOFTAP_STATUS_T softap_status = bwifi_get_softap_status();
        if (softap_status == BWIFI_SOFTAP_STATUS_ON) {
            if (bwifi_softap_get_station_list(mac, &num)) {
                RESP(handler, "get current connected sta fail\n");
                RESP_ERROR(handler, ERROR_FAILE);
            } else {
                for (i = 0; i < num; i++)
                    RESP(handler, "sta[%d]: mac_addr=" MACSTR "\n", i, MAC2STR(mac[i].mac));
                RESP_OK(handler);
            }
        } else {
            RESP(handler, "softap status(%d) is not started\n", softap_status);
            RESP_ERROR(handler, ERROR_FAILE);
        }
        return 0;
    } else {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
}
#endif

int do_set_ch(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t mode, ch;
    char *p;
    int8_t snd_offset = 0;

    if (argc < 3)
        return -1;
    mode = atoi(argv[1]);
    p = strchr(argv[2], '-');
    if (p) {
        *p = '\0';
        snd_offset = -1; //HT40-
    } else {
        p = strchr(argv[2], '+');
        if (p) {
            *p = '\0';
            snd_offset = 1; //HT40+
        }
    }
    ch = atoi(argv[2]);

    bwifi_switch_channel(mode, ch, snd_offset);
    RESP_OK(handler);
    return 0;
}

extern int nv_record_env_rebuild(void);
int do_nvrecord_rebuild(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    //nv_record_env_rebuild();
    return 0;
}

int show_free_heap_size(struct cmd_tbl_s *cmd, int argc, char *argv[], void *handler)
{
#ifdef osCMSIS_FreeRTOS
    int TotalSz = -1;
    int FreeSz = -1;
    int UsedSz = -1;
    int SucAllocNum = -1;
    int SucFreeNum = -1;
    int MinFreeSz = -1;
    int MaxFreeBlkSz = -1;
    int MinFreeBlkSz = -1;
    int BlkNum = -1;

    HeapStats_t pxHeapStats;
    vPortGetHeapStats(&pxHeapStats);
    extern size_t xPortGetTotalHeapSize( void );

    TotalSz = (int)xPortGetTotalHeapSize();
    FreeSz = (int)pxHeapStats.xAvailableHeapSpaceInBytes;
    UsedSz = TotalSz - FreeSz;
    SucAllocNum = (int)pxHeapStats.xNumberOfSuccessfulAllocations;
    SucFreeNum = (int)pxHeapStats.xNumberOfSuccessfulFrees;
    MinFreeSz = (int)pxHeapStats.xMinimumEverFreeBytesRemaining;
    MaxFreeBlkSz = (int)pxHeapStats.xSizeOfLargestFreeBlockInBytes;
    MinFreeBlkSz = (int)pxHeapStats.xSizeOfSmallestFreeBlockInBytes;
    BlkNum = (int)pxHeapStats.xNumberOfFreeBlocks;
    cmd_printf(handler, "-------------------------------------------------------------------------------------------------\r\n");
    cmd_printf(handler, "[HEAP]| TotalSz      | FreeSz       | UsedSz       | MinFreeSz    | SucAllocNum  | SucFreeNum   |\r\n");
    cmd_printf(handler, "      | 0x%08X   | 0x%08X   | 0x%08X   | 0x%08X   | 0x%08X   | 0x%08X   |\r\n",
                    TotalSz,   FreeSz,    UsedSz,    MinFreeSz, SucAllocNum,   SucFreeNum);
    cmd_printf(handler, "      -------------------------------------------------------------------------------------------\r\n");
    cmd_printf(handler, "      | BlkNum       | MaxFreeBlkSz | MinFreeBlkSz |\r\n");
    cmd_printf(handler, "      | 0x%08X   | 0x%08X   | 0x%08X   |\r\n",
                    BlkNum,        MaxFreeBlkSz,  MinFreeBlkSz);
    cmd_printf(handler, "----------------------------------------------------\r\n");
#endif /* osCMSIS_FreeRTOS */

#ifdef KERNEL_LITEOS_M
    extern uint32_t LOS_MemPoolList(void);
    LOS_MemPoolList();
#endif
    return 0;
}

int show_heap_state(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#ifdef osCMSIS_FreeRTOS
    show_free_heap_size(cmd,argc,argv, handler);
#else
    cmd_printf(handler, "application free heap size:%d\n", nts_get_free_heap_size());
#endif
    return 0;
}

int show_thread_info(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#ifdef osCMSIS_FreeRTOS
    static char task_memory[2048] = {0};
    int i = 0, len = 0;
    char buf[49] = {0};

    cmd_printf(handler, "PRIMASK :%x\n", __get_PRIMASK());
    cmd_printf(handler, "IPSP :%x\n", __get_IPSR());
    cmd_printf(handler, "BASEPRI :%x\n", __get_BASEPRI());
    memset(task_memory, 0, sizeof(task_memory));
    vTaskList(task_memory);
    len = strlen(task_memory);

    hal_trace_flush_buffer();
    cmd_printf(handler, "total active task %ld\n", uxTaskGetNumberOfTasks());
    cmd_printf(handler, "list all task info: len %d\n\n", len);
    for (i = 0; i < len; i += 48) {
        memset(buf, 0, 49);
        memcpy(buf, &task_memory[i], 48);
        cmd_printf(handler, "%s", buf);
    }
    cmd_printf(handler, "\n");
    hal_trace_flush_buffer();
    cmd_printf(handler, "dump task ok\n");
#elif defined(KERNEL_RHINO)
    extern void debug_task_overview(int (*print_func)(const char *fmt, ...));
    debug_task_overview(hal_trace_printf_without_crlf_ts);
#else
    cmd_printf(handler, "need freeRTOS\n");
#endif

    return 0;
}

extern void nts_dump_heapsummary(void);
int show_thread_heapsummary(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#if defined(KERNEL_RHINO)
    static int i = 0;
    extern void debug_task_cpu_usage_stats(void);
    debug_task_cpu_usage_stats();
    extern uint32_t debug_total_cpu_usage_get(uint32_t cpuid);
    int cpu = debug_total_cpu_usage_get(0)/100;
    TRACE(0, "now do debug_mm_overview, cpu=%d%% freq:%dMhz", cpu, hal_sys_timer_calc_cpu_freq(5, 0)/1000000);
    extern void debug_total_cpu_usage_show(void * cpuusage_record, int32_t record_len,int32_t index);
    debug_total_cpu_usage_show(NULL, 0, 0);
    dumpsys_mm_overview_func(1);
#else
    nts_dump_heapsummary();
#endif
    return 0;
}

extern void nts_set_shoottsk(const char *name);
int set_shoottsk(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc != 2) {
        cmd_printf(handler, "agrc:%d\n", argc);
        return 0;
    }
    nts_set_shoottsk(argv[1]);

    return 0;
}

#define tskRUNNING_CHAR     ( 'X' )
#define tskBLOCKED_CHAR     ( 'B' )
#define tskREADY_CHAR       ( 'R' )
#define tskDELETED_CHAR     ( 'D' )
#define tskSUSPENDED_CHAR   ( 'S' )
int show_Runtime_Stat(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#ifdef osCMSIS_FreeRTOS
    uint8_t x;
    uint32_t ulRunTimeTick,uxArraySize;
    uxArraySize = uxTaskGetNumberOfTasks();
    TaskStatus_t * pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
    if( pxTaskStatusArray != NULL )
    {
        /* Generate the (binary) data. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulRunTimeTick );
        TRACE(0, "%-15.15s %-10.10s %-10.10s %-10.10s %-10.10s %-10.10s %-10.10s %-10.10s"
            ,"TskName","TskNum","TskState","TskPrio","StackBase","TcbAddr","MinFreeStk","CPU Usage");
        char cStatus;
        for(x = 0;x < uxArraySize; x++)
        {
            switch( pxTaskStatusArray[ x ].eCurrentState )
            {
                case eRunning:      cStatus = tskRUNNING_CHAR;
                                    break;
                case eReady:        cStatus = tskREADY_CHAR;
                                    break;
                case eBlocked:      cStatus = tskBLOCKED_CHAR;
                                    break;
                case eSuspended:    cStatus = tskSUSPENDED_CHAR;
                                    break;
                case eDeleted:      cStatus = tskDELETED_CHAR;
                                    break;
                case eInvalid:      /* Fall through. */
                default:            cStatus = ( char ) 0x00;
                                    break;
            }
            TRACE(0, "%-15.15s %-10.1d %-10.1c %-10.1d 0x%x 0x%x %-10.1d %-0.2f%%",
                    pxTaskStatusArray[x].pcTaskName,
                    (int)pxTaskStatusArray[x].xTaskNumber,
                    cStatus,
                    (int)pxTaskStatusArray[x].uxCurrentPriority,
                    ( unsigned int )pxTaskStatusArray[x].pxStackBase,
                    ( unsigned int )pxTaskStatusArray[x].xHandle,
                    (int)pxTaskStatusArray[x].usStackHighWaterMark,
                    ((float)pxTaskStatusArray[x].ulRunTimeCounter*1.0/ulRunTimeTick*100.0f));
        }
        TRACE(0, "freeHeapSize:%u, minimumEver:%u", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
        TRACE(0, "dump task ok!");
    }
    vPortFree( pxTaskStatusArray );
#if 0
    static char task_memory[2048] = {0};
    int i = 0, len = 0;
    char buf[49] = {0};

    memset(task_memory, 0, sizeof(task_memory));
    vTaskGetRunTimeStats(task_memory);
    len = strlen(task_memory);

    hal_trace_flush_buffer();
    cmd_printf(handler, "run time info: len %d\n", len);
    for (i = 0; i < len; i += 48) {
        memset(buf, 0, 49);
        memcpy(buf, &task_memory[i], 48);
        cmd_printf(handler, "%s", buf);
    }
    cmd_printf(handler, "\n");
    hal_trace_flush_buffer();
    cmd_printf(handler, "run time info end\n");
#endif
#endif

#ifdef KERNEL_LITEOS_M
    extern uint32_t OsGetAllTskInfo(void);
    OsGetAllTskInfo();
#endif

    return 0;
}

int show_system_info(struct cmd_tbl_s *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "========== Heap Info  ==========\r\n");
    show_free_heap_size(cmd,argc,argv, handler);

    cmd_printf(handler, "========== Task Info  =========\r\n");
    show_Runtime_Stat(cmd,argc,argv, handler);
    return 0;
}

int set_GPIO_output(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    u8 index = 0, level = 0;
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_hw_gpio;

    if (argc != 3) {
        cmd_printf(handler, "agrc:%d\n", argc);
        return 0;
    }

    index = atoi(argv[1]);
    if (index > HAL_IOMUX_PIN_NUM) {
        cmd_printf(handler, "index:%d, HAL_IOMUX_PIN_NUM:%d\n", index, HAL_IOMUX_PIN_NUM);
        return 0;
    }

    level = atoi(argv[2]);
    if ((level != 0) && (level != 1) ){
        cmd_printf(handler, "level:%d\n", level);
        return 0;
    }

    cfg_hw_gpio.pin = index;
    cfg_hw_gpio.function = HAL_IOMUX_FUNC_AS_GPIO;
    cfg_hw_gpio.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
    cfg_hw_gpio.pull_sel = HAL_IOMUX_PIN_NOPULL;
    hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&cfg_hw_gpio, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)cfg_hw_gpio.pin, HAL_GPIO_DIR_OUT, level);

    return 0;
}

#if BES_OTA_ENABLED

/**
 * Copy one zone bin data to another.
 *
 * @param dstZone OTA_ZONE_A or OTA_ZONE_B
 * @param srcZone OTA_ZONE_B or OTA_ZONE_A
 *
 * @return
 * - HAL_NORFLASH_OK copy success
 * otherwise write fail.
 */
int ota_copy_zone_to_another(uint8_t dstZone, uint8_t srcZone)
{
    uint32_t zoneLen = 0, num = 0, dstZoneOffset = 0, srcZoneOffset = 0, leftLen = 0, flash_Offset = 0;
    FLASH_OTA_BOOT_INFO_T otaBootInfo;
    uint8_t *buf = NULL;
    uint32_t times = 0;
    BIN_INFO_T *bin_info;
    int ret = 0;
    uint32_t crc32Value = 0;
    uint32_t bin_size = 0;
    uint32_t bin_crc32 = 0;
    char bin_ver[16] = {0};

    TRACE(0, "%s %d, dstZone:%d, srcZone:%d", __func__, __LINE__, dstZone, srcZone);

    buf = (uint8_t *)malloc(FLASH_BLOCK_SIZE_IN_BYTES);
    if (!buf) {
        TRACE(0, "%s %d, malloc error", __func__, __LINE__);
        return -1;
    }

    memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
    ota_read_flash(srcZone, srcZoneOffset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
    for (num = 0; num < 96; num ++) {
        cmd_printf(handler, "%02x ", *(buf + num));
        if ((num + 1) % 16 == 0)
            cmd_printf(handler, "\n");
    }

    bin_info = (BIN_INFO_T *)buf;
    if (bin_info->magic != NORMAL_BOOT) {
        TRACE(0, "error %s %d, bin_info->magic:0x%x", __func__, __LINE__, bin_info->magic);
        goto end;
    }

    zoneLen = bin_info->size + sizeof(BIN_INFO_T);
    bin_size = bin_info->size + sizeof(BIN_INFO_T);
    bin_crc32 = bin_info->crc;
    strcpy(bin_ver, bin_info->ver);
    TRACE(0, "%s %d, zoneLen:0x%x, bin_size:0x%x, bin_crc32:0x%x, bin_ver:%s", __func__, __LINE__, zoneLen, bin_size, bin_crc32, bin_ver);

    crc32Value = crc32(crc32Value, (uint8_t *)(buf + sizeof(BIN_INFO_T)), FLASH_BLOCK_SIZE_IN_BYTES - sizeof(BIN_INFO_T));
    srcZoneOffset += FLASH_BLOCK_SIZE_IN_BYTES;
    if (zoneLen >= FLASH_BLOCK_SIZE_IN_BYTES) {
        zoneLen -= FLASH_BLOCK_SIZE_IN_BYTES;
    } else {
        ret = -1;
        goto end;
    }

    times = zoneLen / FLASH_BLOCK_SIZE_IN_BYTES;
    for(num = 0; num < times; num ++) {
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        ret = ota_read_flash(srcZone, srcZoneOffset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }
#if 0
        for (num = 0; num < FLASH_BLOCK_SIZE_IN_BYTES; num ++) {
            cmd_printf(handler, "%02x ", *(buf + num));
            if ((num + 1) % 16 == 0)
                cmd_printf(handler, "\n");
        }
#endif
        crc32Value = crc32(crc32Value, (uint8_t *)buf, FLASH_BLOCK_SIZE_IN_BYTES);
        srcZoneOffset += FLASH_BLOCK_SIZE_IN_BYTES;
        zoneLen -= FLASH_BLOCK_SIZE_IN_BYTES;
    }

    if (zoneLen) {
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);

        ret = ota_read_flash(srcZone, srcZoneOffset, buf, zoneLen);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }
        crc32Value = crc32(crc32Value, (uint8_t *)buf, zoneLen);
    }

    TRACE(0, "*************%s %d, num:%d, times:%d, zoneLen:%d*************", __func__, __LINE__, num, times, zoneLen);

    num = zoneLen - zoneLen % 16 -96;
    for (; num < zoneLen; num ++) {
        cmd_printf(handler, "%02x ", *(buf + num));
        if ((num + 1) % 16 == 0)
            cmd_printf(handler, "\n");
    }
    cmd_printf(handler, "\n");

    if (crc32Value != bin_crc32) {
        TRACE(0, "error %s %d, crc32Value:0x%x, bin_crc32:0x%x, srcZoneOffset:0x%x",
            __func__, __LINE__, crc32Value, bin_crc32, (srcZoneOffset+zoneLen));

        ret = -1;
        goto end;
    } else {
        TRACE(0, "%s %d, crc32 check ok!", __func__, __LINE__);
    }

    zoneLen = bin_size;
    srcZoneOffset = 0;
    dstZoneOffset = 0;
    for(num = 0; num < zoneLen / FLASH_BLOCK_SIZE_IN_BYTES; num ++) {
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        ret = ota_read_flash(srcZone, srcZoneOffset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }

        ret = ota_write_flash(dstZone, dstZoneOffset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }

        srcZoneOffset += FLASH_BLOCK_SIZE_IN_BYTES;
        dstZoneOffset += FLASH_BLOCK_SIZE_IN_BYTES;
    }

    leftLen = zoneLen % FLASH_BLOCK_SIZE_IN_BYTES;
    if (leftLen) {
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        ret = ota_read_flash(srcZone, srcZoneOffset, buf, leftLen);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }
        ret = ota_write_flash(dstZone, dstZoneOffset, buf, leftLen);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }
    }
    TRACE(0, "%s %d, copy finish!", __func__, __LINE__);

end:
    free(buf);

    if (!ret) {
        ota_get_boot_info(&otaBootInfo);
        otaBootInfo.bootFlag = NORMAL_BOOT;
        otaBootInfo.bootZoneNum = dstZone;
        if (dstZone == OTA_ZONE_A) {
            otaBootInfo.bootEntry = FLASH_BASE + ZONEA_IMAGE_FLASH_OFFSET;;
        } else if (dstZone == OTA_ZONE_B) {
            otaBootInfo.bootEntry = FLASH_BASE + ZONEB_IMAGE_FLASH_OFFSET;;
        }
        otaBootInfo.zone[dstZone].magicNumber = VALID_MAGIC_NUM;
        otaBootInfo.zone[dstZone].imageSize = bin_size;
        otaBootInfo.zone[dstZone].imageCrc = bin_crc32;
        strcpy(otaBootInfo.zone[dstZone].version, bin_ver);
        ota_set_boot_info(&otaBootInfo);
        ota_show_boot_info(&otaBootInfo);
        hal_cmu_sys_reboot();
        while (1) {}
    }

    return ret;
}

/**
 * Copy one zone bin data to another by ag OTA API.
 *
 * @param dstZone OTA_ZONE_A or OTA_ZONE_B
 * @param srcZone OTA_ZONE_B or OTA_ZONE_A
 *
 * @return
 * - HAL_NORFLASH_OK copy success
 * otherwise write fail.
 */
int ag_ota_copy_zone_to_another(uint8_t dst_zone, uint8_t src_zone, uint32_t length)
{
    uint32_t zoneLen = 0, num = 0, dstZoneOffset = 0, srcZoneOffset = 0, leftLen = 0;
    uint32_t times = 0;
    uint32_t ret = 0;
    uint32_t now_zone = 0;
    uint8_t *buf = NULL;

    TRACE(0, "dstZone:%d, srcZone:%d, length:0x%x", dst_zone, src_zone, length);
    now_zone = ota_get_boot_zone_number();
    if (now_zone == dst_zone) {
        TRACE(0, "%s %d, now_zone(%d) == dstZone(%d)", __func__, __LINE__, now_zone, dst_zone);
        return ret;
    }

    ota_erase_partition(dst_zone, 0);
    TRACE(0, "%s %d, ota_erase_partition finish.", __func__, __LINE__);

    buf = (uint8_t *)malloc(FLASH_BLOCK_SIZE_IN_BYTES);
    if (!buf) {
        TRACE(0, "%s %d, malloc error", __func__, __LINE__);
        return -1;
    }

    zoneLen = length;
    srcZoneOffset = 0;
    dstZoneOffset = 0;
    for(num = 0; num < zoneLen / FLASH_BLOCK_SIZE_IN_BYTES; num ++) {
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        ret = ota_read_flash(src_zone, srcZoneOffset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }

        ret = ota_write_partition(dst_zone, dstZoneOffset, buf, FLASH_BLOCK_SIZE_IN_BYTES);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }

        srcZoneOffset += FLASH_BLOCK_SIZE_IN_BYTES;
        dstZoneOffset += FLASH_BLOCK_SIZE_IN_BYTES;
    }

    leftLen = zoneLen % FLASH_BLOCK_SIZE_IN_BYTES;
    if (leftLen) {
        memset(buf, 0, FLASH_BLOCK_SIZE_IN_BYTES);
        ret = ota_read_flash(src_zone, srcZoneOffset, buf, leftLen);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }

        num = leftLen - leftLen % 16 -96;
        for (; num < leftLen; num ++) {
            cmd_printf(handler, "%02x ", *(buf + num));
            if ((num + 1) % 16 == 0)
                cmd_printf(handler, "\n");
        }
        cmd_printf(handler, "\n");

        ret = ota_write_partition(dst_zone, dstZoneOffset, buf, leftLen);
        if (ret) {
            TRACE(0, "error %s %d, ota_read_flash ret:%d", __func__, __LINE__, ret);
            goto end;
        }
    }
    TRACE(0, "%s %d, copy finish!", __func__, __LINE__);
    ota_set_boot_partition(dst_zone);

end:
    free(buf);
    buf = NULL;

    if (!ret) {
        hal_cmu_sys_reboot();
        while (1) {}
    }

    return ret;
}

int do_ota_action(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    u8 index = 0;
    int zone_num = 0, magic = 0, size = 0, crc = 0, reserved = 0, dst_zone =0, src_zone =0;
    uint32_t zone_len = 0;
    FLASH_OTA_BOOT_INFO_T otaBootInfo;

    if (argc < 2) {
        cmd_printf(handler, "error, %s %d, agrc:%d\n", __func__, __LINE__, argc);
        return -1;
    }

    index = atoi(argv[1]);
    if (index > HAL_IOMUX_PIN_NUM) {
        cmd_printf(handler, "index:%d, HAL_IOMUX_PIN_NUM:%d\n", index, HAL_IOMUX_PIN_NUM);
        return -1;
    }
    switch (index) {
        case OTA_GET_BOOT_INFO:
            ota_get_boot_info(&otaBootInfo);
            ota_show_boot_info(&otaBootInfo);
            break;

        case OTA_GET_BOOT_ZONE_NUM:
            zone_num = ota_get_boot_zone_number();
            cmd_printf(handler, "%s %d, ota_get_boot_zone_number ret:%d\n", __func__, __LINE__, zone_num);
            break;

        case OTA_SET_BOOT_ZONE_NUM:
            zone_num = atoi(argv[2]);
            if (zone_num >= OTA_ZONE_MAX) {
                cmd_printf(handler, "error, %s %d, zone_num:%d\n", __func__, __LINE__, zone_num);
                return -1;
            }
            ota_set_boot_partition(zone_num);
            break;

        case OTA_SET_ZONE_INFO:
            zone_num = atoi(argv[2]);
            if (zone_num >= OTA_ZONE_MAX) {
                cmd_printf(handler, "error, %s %d, zone_num:%d\n", __func__, __LINE__, zone_num);
                return -1;
            }
            size = strtoul(argv[3], NULL, 16);
            crc = strtoul(argv[4], NULL, 16);
            reserved = strtoul(argv[5], NULL, 16);

            cmd_printf(handler, "%s %d, zone_num:%d, size:0x%x, crc:0x%x, ver:%s\n",
                __func__, __LINE__, zone_num, size, crc, argv[5]);

            ota_get_boot_info(&otaBootInfo);
            otaBootInfo.zone[zone_num].magicNumber = VALID_MAGIC_NUM;
            otaBootInfo.zone[zone_num].imageSize = size;
            otaBootInfo.zone[zone_num].imageCrc = crc;
            otaBootInfo.zone[zone_num].reserved = reserved;
            strcpy(otaBootInfo.zone[zone_num].version, argv[6]);
            ota_set_boot_info(&otaBootInfo);
            ota_show_boot_info(&otaBootInfo);
            break;

        case OTA_SET_BOOT_INFO_INVALIDE:
            ota_get_boot_info(&otaBootInfo);
            otaBootInfo.bootFlag = ABNORMAL_BOOT;
            ota_set_boot_info(&otaBootInfo);
            ota_show_boot_info(&otaBootInfo);
            break;

        case OTA_COPY_ZONE:
            if (argc != 4) {
                cmd_printf(handler, "error, %s %d, zone_num:%d\n", __func__, __LINE__, argc);
                return -1;
            }
            dst_zone = atoi(argv[2]);
            src_zone = atoi(argv[3]);
            if ((dst_zone >= OTA_ZONE_MAX) || (src_zone >= OTA_ZONE_MAX) || (dst_zone == src_zone)){
                cmd_printf(handler, "error, %s %d, dst_zone:%d, src_zone:%d\n", __func__, __LINE__, dst_zone, src_zone);
                return -1;
            }
            cmd_printf(handler, "%s %d, dst_zone:%d, src_zone:%d\n", __func__, __LINE__, dst_zone, src_zone);
            ota_copy_zone_to_another(dst_zone, src_zone);
            cmd_printf(handler, "%s %d, ota_copy_zone_to_another end\n", __func__, __LINE__);
            break;

        case AG_OTA_COPY_ZONE:
            if (argc != 4) {
                cmd_printf(handler, "error, %s %d, zone_num:%d\n", __func__, __LINE__, argc);
                return -1;
            }

            dst_zone = atoi(argv[2]);
            src_zone = atoi(argv[3]);
            if ((dst_zone >= OTA_ZONE_MAX) || (src_zone >= OTA_ZONE_MAX) || (dst_zone == src_zone)){
                cmd_printf(handler, "error, %s %d, dst_zone:%d, src_zone:%d\n", __func__, __LINE__, dst_zone, src_zone);
                return -1;
            }
            cmd_printf(handler, "%s %d, dst_zone:%d, src_zone:%d\n", __func__, __LINE__, dst_zone, src_zone);

            ota_get_boot_info(&otaBootInfo);
            zone_len = otaBootInfo.zone[src_zone].imageSize + sizeof(BIN_INFO_T);
            ag_ota_copy_zone_to_another(dst_zone, src_zone, zone_len);
            break;

       default:
            return -1;
       }

    return 0;
}
#endif

static int gsi_atcmdecho = 0;
static int set_at_cmd_echo(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int at_cmd_echo = 0;
    if ( (argc==2 && argv[1][0] == '?') || (argc<2)){
        if(gsi_atcmdecho){
            cmd_printf(handler, "AT_CMD_ECHO:%d enable\n", gsi_atcmdecho );
        }else{
            cmd_printf(handler, "AT_CMD_ECHO:%d disable\n", gsi_atcmdecho );
        }
        return 0;
    }
    at_cmd_echo = atoi(argv[1]);
    if(at_cmd_echo<0 || at_cmd_echo >1)
    {
        gsi_atcmdecho = 0;
        cmd_printf(handler, "AT_CMD_ECHO default %d\n", gsi_atcmdecho );
    }
    else
    {
        /* code */
        gsi_atcmdecho = at_cmd_echo;
        cmd_printf(handler, "AT_CMD_ECHO:%d\n", gsi_atcmdecho );
    }
    return 0;
}
int get_at_cmd_echo()
{
    return gsi_atcmdecho;
}

#ifdef CHIP_BEST2000
int do_set_rf_power(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret;
    nvrec_wifidevicerecord record;
    int i= 0, power = 0x136;
    if (argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    memset(&record, 0, sizeof(nvrec_wifidevicerecord));
    power = strtoul(argv[1], NULL, 16);
    for(i = 0; i < 14 ; i++){
    record.wifi_record.cal.tx_power[i] = power;
    }

    strcpy(record.type, BWIFI_RECORD_TYPE_CAL);
    bwifi_del_record(BWIFI_RECORD_TYPE_CAL);
    ret = bwifi_add_record(&record);
    if (ret) {
        RESP_ERROR(handler, ERROR_FAILE);
        return 0;
    }

    RESP_OK(handler);
    return 0;

}
#endif
extern void wifi_set_xtal_cal_val(uint16_t freq_cal);
int do_freq_cal_cfg(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int cfg = 0x0058;

    if (argc != 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    cfg = strtoul(argv[1], NULL, 16);
    cmd_printf(handler, "%s cfg=0x%04X", __FUNCTION__, cfg);
    wifi_set_xtal_cal_val(cfg);

    RESP_OK(handler);
    return 0;
}


int do_set_epta_params(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int wifi_duration,bt_duration,hw_epta_enable = 0;

    if (argc < 4) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    wifi_duration = atoi(argv[1]);
    bt_duration = atoi(argv[2]);
    hw_epta_enable = atoi(argv[3]);
    cmd_printf(handler, "wifi_duration %d\n",wifi_duration);
    cmd_printf(handler, "bt_duration %d\n",bt_duration);
    cmd_printf(handler, "hw_epta_enable %d\n",hw_epta_enable);
    bwifi_set_epta_param(wifi_duration, bt_duration, hw_epta_enable);
    return 0;
}


#if defined(__NET_FTP_CLIENT_SUPPORT__) && defined(__A7_DSP_ENABLE__)
#define MAX_USERNAME_LEN 20
#define MAX_PASSWD_LEN 30

//#define ONLY_MIC_DATA
/* mic record dump */
int do_record_to_ftp(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc > 10 || argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    /**
     * AT+RECDUMP=-c2,-fS16, -r16000, -d5, -s192.168.137.1, -p21, -uusername, -wpassword, -h/test.pcm
     * [-c] mic record audio channel num
     * [-f] mic record audio format (default pcm)
     * [-r] mic record audio sample rate (48000 16000 etc.)
     * [-d] mic record audio duration (unit secord)
     * [-s] ftp server ip address
     * [-p] ftp server port
     * [-u] ftp server username
     * [-w] ftp server password
     * [-h] ftp server data path
     */
    char *CMD = "AT+RECDUMP";
    int mic_ch = 1;
    int dur_s = 1;
    char audio_format[10] = {0};
    int format = -1;
    int sample_rate = -1;
    char ftp_ip[20] = {0};
    int ftp_port = 21;
    char ftp_data_path_2mic[MAX_LEN] = {0};
	char ftp_data_path_3mic[MAX_LEN] = {0};
    char ftp_username[MAX_USERNAME_LEN] = {0};
    char ftp_password[MAX_PASSWD_LEN] = {0};

    for (int i = 0; i < argc; i++)
    {
        cmd_printf(handler, " argv[%d] = %s  \n",i,argv[i]);
        if (i > 0)
        {
            char *p = argv[i];
            if ( *p == 45) /* '-' */
            {
                p++;
                char opt = *p;
                char optarg[MAX_LEN] = {0};
                p++;
                int j = 0;
                for (; *p != '\0'; j++)
                {
                    optarg[j] = *p;
                    p++;
                }
                switch(opt) {
                    case 'c':
                        mic_ch = atoi(optarg);
                        break;
                    case 'f':
                        memcpy(audio_format,optarg,j);
                        if (!strcmp("S16", audio_format)) {
                            format = 0; /* for audio format*/
                        }
                        break;
                    case 'r':
                        sample_rate = atoi(optarg);
                        break;
                    case 'd':
                        dur_s = atoi(optarg);
                        break;
                    case 's':
                        memcpy(ftp_ip,optarg,j);
                        break;
                    case 'p':
                        ftp_port = atoi(optarg);
                        break;
                    case 'u':
                        j = (j > MAX_USERNAME_LEN) ? MAX_USERNAME_LEN : j;
                        memcpy(ftp_username,optarg,j);
                        break;
                    case 'w':
                        j = (j > MAX_PASSWD_LEN) ? MAX_PASSWD_LEN : j;
                        memset(ftp_password,0,MAX_PASSWD_LEN);
                        memcpy(ftp_password,optarg,j);
                        break;
                    case 'h':
                        memcpy(ftp_data_path_2mic,optarg,j);
						memcpy(ftp_data_path_2mic+j,"2mic",4);
						memcpy(ftp_data_path_3mic,optarg,j);
						memcpy(ftp_data_path_3mic+j,"3mic",4);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (ftp_ip == NULL)
    {
        cmd_printf(handler, "[%s] ftp ip is NULL ,like %s, .... <-s 192.168.9.184> .... \n",CMD,CMD);
        return 0;
    }

    void *ftp_handler_2mic = NULL;
	void *ftp_handler_3mic = NULL;
#ifdef ONLY_MIC_DATA
	
    ftp_handler_2mic = ftp_client_init();
    if (ftp_handler_2mic == NULL)
    {
        cmd_printf(handler, "ftp client init failed\n");
        return 0;;
    }
    cmd_printf(handler, "[%s] ip:%s port:%d\n",CMD,ftp_ip,ftp_port);
    if (ftp_connect_server(ftp_handler_2mic, ftp_ip, ftp_port) < 0)
    {
        cmd_printf(handler, "[%s] ftp connect server failed !\n",CMD);
        ftp_client_free(ftp_handler_2mic);
        return 0;;
    }
    if(ftp_login(ftp_handler_2mic, ftp_username, ftp_password) < 0)
    {
        cmd_printf(handler, "[%s] ftp login on server failed !\n",CMD);
        ftp_disconnect(ftp_handler_2mic);
        ftp_client_free(ftp_handler_2mic);
        return 0;;
    }

    if(ftp_client_notice(ftp_handler_2mic,ftp_data_path_2mic) !=0)
    {
        TRACE(0, "[%s] notice ftp server %s fail\n", CMD,ftp_data_path_2mic);
        ftp_disconnect(ftp_handler_2mic);
        ftp_client_free(ftp_handler_2mic);
        return 0;
    }
#endif
	ftp_handler_3mic = ftp_client_init();
    if (ftp_handler_3mic == NULL)
    {
        cmd_printf(handler, "ftp client init failed\n");
        return 0;;
    }
    cmd_printf(handler, "[%s] ip:%s port:%d\n",CMD,ftp_ip,ftp_port);
    if (ftp_connect_server(ftp_handler_3mic, ftp_ip, ftp_port) < 0)
    {
        cmd_printf(handler, "[%s] ftp connect server failed !\n",CMD);
        ftp_client_free(ftp_handler_2mic);
        return 0;;
    }

    if(ftp_login(ftp_handler_3mic, ftp_username, ftp_password) < 0)
    {
        cmd_printf(handler, "[%s] ftp login on server failed !\n",CMD);
        ftp_disconnect(ftp_handler_3mic);
        ftp_client_free(ftp_handler_3mic);
        return 0;;
    }
	if(ftp_client_notice(ftp_handler_3mic,ftp_data_path_3mic) !=0)
    {
        TRACE(0, "[%s] notice ftp server %s fail\n", CMD,ftp_data_path_3mic);
        ftp_disconnect(ftp_handler_3mic);
        ftp_client_free(ftp_handler_3mic);
        return 0;
    }
    /**
     * get record data
     */
    unsigned int size_KB = 32;
    void * handle = data_dump_open(aud_record_subscribe, size_KB, sample_rate, 1);
    unsigned total_record_size = 0;
    uint8_t record_buf[CHAN_NUM_CAPTURE*1024] = {0};
#ifdef ONLY_MIC_DATA
    uint8_t record_buf_mic1_mic2[2*1024] = {0};
	uint8_t record_buf_ref[1*1024] = {0};
#endif
    unsigned int record_start_time = TICKS_TO_MS(hal_sys_timer_get());
    cmd_printf(handler, "[%s] record_start_time : %d\n",CMD,record_start_time);
    while (TICKS_TO_MS(hal_sys_timer_get()) < (unsigned int)(record_start_time+dur_s*1000))
    {
        memset(record_buf,0,sizeof(record_buf));
        int read_len = data_dump_read(handle, record_buf, sizeof(record_buf));
        /* handle pcm data ,dump to ftp server */
        if (read_len > 0)
        {
            cmd_printf(handler, "[%s] read_len : %d\n",CMD,read_len);
#ifdef ONLY_MIC_DATA

            int lrsize = record_data_get_mic1_mic2((uint16_t *)record_buf,read_len/2,(uint16_t *)record_buf_mic1_mic2, (uint16_t *)record_buf_ref);
            cmd_printf(handler, "[%s] put len L:R : %d\n",CMD,lrsize*2);
            total_record_size += ftp_client_dumpdata(ftp_handler_2mic, (const char *)record_buf_mic1_mic2, lrsize*2);
			total_record_size += ftp_client_dumpdata(ftp_handler_3mic, (const char *)record_buf_ref, lrsize);
#else
            total_record_size += ftp_client_dumpdata(ftp_handler_3mic, (const char *)record_buf, read_len);
#endif
        }
#ifdef ONLY_MIC_DATA
        osDelay(3);
#else
        osDelay(1);
#endif
    }
    cmd_printf(handler, "[%s] record_end_time : %d\n",CMD,TICKS_TO_MS(hal_sys_timer_get()));
    data_dump_close(handle, aud_record_unsubscribe);
    cmd_printf(handler, "%s done, total record %ds data size %d byte\n", CMD, dur_s, total_record_size);
#ifdef ONLY_MIC_DATA
    ftp_disconnect(ftp_handler_2mic);
#endif
	ftp_disconnect(ftp_handler_3mic);
#ifdef ONLY_MIC_DATA
    ftp_client_free(ftp_handler_2mic);
#endif
    ftp_client_free(ftp_handler_3mic);
    (void)format;
    (void)mic_ch;
    return 0;
}

#if defined(__A7_DSP_ENABLE__)
int do_a7_dump_ftp(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc > 10 || argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    char *CMD = "AT+A7DUMP";
    int mic_ch = 1;
    int dur_s = 1;
    char audio_format[10] = {0};
    int format = -1;
    int sample_rate = -1;
    char ftp_ip[20] = {0};
    int ftp_port = 21;
    char ftp_data_path[MAX_LEN] = {0};
    char ftp_username[MAX_USERNAME_LEN] = {0};
    char ftp_password[MAX_PASSWD_LEN] = {0};

    for (int i = 0; i < argc; i++)
    {
        cmd_printf(handler, " argv[%d] = %s  \n",i,argv[i]);
        if (i > 0)
        {
            char *p = argv[i];
            if ( *p == 45) /* '-' */
            {
                p++;
                char opt = *p;
                char optarg[MAX_LEN] = {0};
                p++;
                int j = 0;
                for (; *p != '\0'; j++)
                {
                    optarg[j] = *p;
                    p++;
                }
                switch(opt) {
                    case 'c':
                        mic_ch = atoi(optarg);
                        break;
                    case 'f':
                        memcpy(audio_format,optarg,j);
                        if (!strcmp("S16", audio_format)) {
                            format = 0; /* for audio format*/
                        }
                        break;
                    case 'r':
                        sample_rate = atoi(optarg);
                        break;
                    case 'd':
                        dur_s = atoi(optarg);
                        break;
                    case 's':
                        memcpy(ftp_ip,optarg,j);
                        break;
                    case 'p':
                        ftp_port = atoi(optarg);
                        break;
                    case 'u':
                        j = (j > MAX_USERNAME_LEN) ? MAX_USERNAME_LEN : j;
                        memcpy(ftp_username,optarg,j);
                        break;
                    case 'w':
                        j = (j > MAX_PASSWD_LEN) ? MAX_PASSWD_LEN : j;
                        memset(ftp_password,0,MAX_PASSWD_LEN);
                        memcpy(ftp_password,optarg,j);
                        break;
                    case 'h':
                        memcpy(ftp_data_path,optarg,j);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (ftp_ip == NULL)
    {
        cmd_printf(handler, "[%s] ftp ip is NULL ,like %s, .... <-s 192.168.137.1> .... \n",CMD,CMD);
        return 0;
    }

    void *ftp_handler = NULL;
    ftp_handler = ftp_client_init();
    if (ftp_handler == NULL)
    {
        cmd_printf(handler, "ftp client init failed\n");
        return 0;
    }
    cmd_printf(handler, "[%s] ip:%s port:%d\n",CMD,ftp_ip,ftp_port);
    if (ftp_connect_server(ftp_handler, ftp_ip, ftp_port) < 0)
    {
        cmd_printf(handler, "[%s] ftp connect server failed !\n",CMD);
        ftp_client_free(ftp_handler);
        return 0;
    }

    if(ftp_login(ftp_handler, ftp_username, ftp_password) < 0)
    {
        cmd_printf(handler, "[%s] ftp login on server failed !\n",CMD);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }

    if(ftp_client_notice(ftp_handler,ftp_data_path) !=0)
    {
        TRACE(0, "[%s] notice ftp server %s fail\n", CMD,ftp_data_path);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }
    /**
     * get record data
     */
    unsigned int size_KB = 32;
    void * handle = data_dump_open(a7_to_m33_aud_subscribe, size_KB, sample_rate, 1);
    unsigned total_record_size = 0;
    char record_buf[CHAN_NUM_CAPTURE*1024] = {0};

    unsigned int record_start_time = TICKS_TO_MS(hal_sys_timer_get());
    cmd_printf(handler, "[%s] record_start_time : %d\n",CMD,record_start_time);
    while (TICKS_TO_MS(hal_sys_timer_get()) < (unsigned int)(record_start_time+dur_s*1000))
    {
        memset(record_buf,0,sizeof(record_buf));
        int read_len = data_dump_read(handle, record_buf, sizeof(record_buf));
        /* handle pcm data ,dump to ftp server */
        if (read_len > 0)
        {
            cmd_printf(handler, "[%s] read_len : %d\n",CMD,read_len);
            total_record_size += ftp_client_dumpdata(ftp_handler, record_buf, read_len);
        }
    }
    cmd_printf(handler, "[%s] record_end_time : %d\n",CMD,TICKS_TO_MS(hal_sys_timer_get()));
    data_dump_close(handle, a7_to_m33_aud_unsubscribe);
    cmd_printf(handler, "%s done, total record %ds data size %d byte\n", CMD, dur_s, total_record_size);
    ftp_disconnect(ftp_handler);
    ftp_client_free(ftp_handler);
    (void)format;
    (void)mic_ch;
    return 0;
}
#endif

/**
* AT+GETFILE=-s192.168.137.1, -p21, -uusername, -wpassword, -h/test.pcm
* [-s] ftp server ip address
* [-p] ftp server port
* [-u] ftp server username
* [-w] ftp server password
* [-h] ftp server file path
*/
int do_ftp_get_file(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc > 7 || argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }

    char *CMD = "AT+GETFILE";
    char ftp_ip[20] = {0};
    int ftp_port = 21;
    int recev_size = 0;
    char ftp_data_path[MAX_LEN] = {0};
    char ftp_username[MAX_USERNAME_LEN] = {0};
    char ftp_password[MAX_PASSWD_LEN] = {0};
    char ftp_get_buffer[512];
    for (int i = 0; i < argc; i++)
    {
        cmd_printf(handler, " argv[%d] = %s  \n",i,argv[i]);
        if (i > 0)
        {
            char *p = argv[i];
            if ( *p == 45) /* '-' */
            {
                p++;
                char opt = *p;
                char optarg[MAX_LEN] = {0};
                p++;
                int j = 0;
                for (; *p != '\0'; j++)
                {
                    optarg[j] = *p;
                    p++;
                }
                switch(opt) {
                    case 's':
                        memcpy(ftp_ip,optarg,j);
                        break;
                    case 'p':
                        ftp_port = atoi(optarg);
                        break;
                    case 'u':
                        j = (j > MAX_USERNAME_LEN) ? MAX_USERNAME_LEN : j;
                        memcpy(ftp_username,optarg,j);
                        break;
                    case 'w':
                        j = (j > MAX_PASSWD_LEN) ? MAX_PASSWD_LEN : j;
                        memset(ftp_password,0,MAX_PASSWD_LEN);
                        memcpy(ftp_password,optarg,j);
                        break;
                    case 'h':
                        memcpy(ftp_data_path,optarg,j);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (ftp_ip == NULL)
    {
        cmd_printf(handler, "[%s] ftp ip is NULL ,like %s, .... <-s 192.168.9.184> .... \n",CMD,CMD);
        return 0;
    }

    void *ftp_handler = NULL;
    ftp_handler = ftp_client_init();
    if (ftp_handler == NULL)
    {
        cmd_printf(handler, "ftp client init failed\n");
        return 0;;
    }
    cmd_printf(handler, "[%s] ip:%s port:%d\n",CMD,ftp_ip,ftp_port);
    if (ftp_connect_server(ftp_handler, ftp_ip, ftp_port) < 0)
    {
        cmd_printf(handler, "[%s] ftp connect server failed !\n",CMD);
        ftp_client_free(ftp_handler);
        return 0;;
    }

    if(ftp_login(ftp_handler, ftp_username, ftp_password) < 0)
    {
        cmd_printf(handler, "[%s] ftp login on server failed !\n",CMD);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;;
    }

    if(ftp_client_notice_get_file(ftp_handler,ftp_data_path) !=0)
    {
        TRACE(0, "[%s] notice ftp server %s fail\n", CMD,ftp_data_path);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }
    /**
     * get data
     */
    do
    {
        memset(ftp_get_buffer, 0 ,512);
        recev_size = fet_client_get_data(ftp_handler,ftp_get_buffer,512);
        cmd_printf(handler, "\r\nfile size %d:%s\r\n",recev_size,ftp_get_buffer);
    }while(recev_size > 0);

    ftp_disconnect(ftp_handler);
    ftp_client_free(ftp_handler);
    return 0;
}

#ifdef PCM_AUDIO_INSTANCE_ENABLE
#include "pcm_audio_instance.h"
#include "pcm_audio.h"
int do_pcmaudio_dump_ftp(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc > 10 || argc < 2) {
        RESP_ERROR(handler, ERROR_ARG);
        return 0;
    }
    char *CMD = "AT+PCM";
    int mic_ch = 1;
    int dur_s = 1;
    char audio_format[10] = {0};
    int format = -1;
    int sample_rate = 16000;
    char ftp_ip[20] = {0};
    int ftp_port = 21;
    char ftp_data_path[MAX_LEN] = {0};
    char ftp_username[MAX_USERNAME_LEN] = {0};
    char ftp_password[MAX_PASSWD_LEN] = {0};

    for (int i = 0; i < argc; i++)
    {
        cmd_printf(handler, " argv[%d] = %s  \n",i,argv[i]);
        if (i > 0)
        {
            char *p = argv[i];
            if ( *p == 45) /* '-' */
            {
                p++;
                char opt = *p;
                char optarg[MAX_LEN] = {0};
                p++;
                int j = 0;
                for (; *p != '\0'; j++)
                {
                    optarg[j] = *p;
                    p++;
                }
                switch(opt) {
                    case 'c':
                        mic_ch = atoi(optarg);
                        break;
                    case 'f':
                        memcpy(audio_format,optarg,j);
                        if (!strcmp("S16", audio_format)) {
                            format = 0; /* for audio format*/
                        }
                        break;
                    case 'r':
                        sample_rate = atoi(optarg);
                        break;
                    case 'd':
                        dur_s = atoi(optarg);
                        break;
                    case 's':
                        memcpy(ftp_ip,optarg,j);
                        break;
                    case 'p':
                        ftp_port = atoi(optarg);
                        break;
                    case 'u':
                        j = (j > MAX_USERNAME_LEN) ? MAX_USERNAME_LEN : j;
                        memcpy(ftp_username,optarg,j);
                        break;
                    case 'w':
                        j = (j > MAX_PASSWD_LEN) ? MAX_PASSWD_LEN : j;
                        memset(ftp_password,0,MAX_PASSWD_LEN);
                        memcpy(ftp_password,optarg,j);
                        break;
                    case 'h':
                        memcpy(ftp_data_path,optarg,j);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if (ftp_ip == NULL) {
         cmd_printf(handler, "[%s] ftp ip is NULL ,like %s, .... <-s 192.168.137.1> .... \n",CMD,CMD);
        return 0;
    }

    void *ftp_handler = NULL;

    ftp_handler = ftp_client_init();
    if (ftp_handler == NULL) {
        cmd_printf(handler, "ftp client init failed\n");
        return 0;
    }
    cmd_printf(handler, "[%s] ip:%s port:%d\n",CMD,ftp_ip,ftp_port);
    if (ftp_connect_server(ftp_handler, ftp_ip, ftp_port) < 0) {
        cmd_printf(handler, "[%s] ftp connect server failed !\n",CMD);
        ftp_client_free(ftp_handler);
        return 0;
    }

    if(ftp_login(ftp_handler, ftp_username, ftp_password) < 0)
    {
        cmd_printf(handler, "[%s] ftp login on server failed !\n",CMD);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }

    if(ftp_client_notice(ftp_handler,ftp_data_path) !=0)
    {
        TRACE(0, "[%s] notice ftp server %s fail\n", CMD,ftp_data_path);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }

    /**
     * get record data
     */
    extern void pcm_audio_resample_48_to_16_cb(void *hdl, unsigned char *in, int in_len, int samplerate, int ch_num, unsigned char **p_out, int *p_out_len);

    void * handle = pcm_open(sample_rate, CHAN_NUM_CAPTURE, pcm_audio_resample_48_to_16_cb);
    if (handle == NULL) {
        cmd_printf(handler, "%s pcm_open failed!!\n", __FUNCTION__);
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }
    int ret = pcm_start(handle);
    if (ret) {
        cmd_printf(handler, "%s pcm_start failed!!\n", __FUNCTION__);
        pcm_close(handle);
        handle = NULL;
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return 0;
    }
    unsigned total_record_size = 0;
    uint32_t read_record_len = (sample_rate / 1000) * 16 * 2 * CHAN_NUM_CAPTURE;
    char *record_buf = malloc(read_record_len);
    if (record_buf == NULL) {
        cmd_printf(handler, "%s malloc failed!!\n", __FUNCTION__);
        pcm_stop(handle);
        osDelay(5);
        pcm_close(handle);
        handle = NULL;
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        return  0;
    }

    unsigned int record_start_time = TICKS_TO_MS(hal_sys_timer_get());
    cmd_printf(handler, "[%s] record_start_time : %d read_record_len:%d sample_rate:%d dur_s:%d\n", CMD, record_start_time, read_record_len, sample_rate, dur_s);
    while (TICKS_TO_MS(hal_sys_timer_get()) < (unsigned int)(record_start_time+dur_s*1000)) {
        memset(record_buf, 0, read_record_len);
        int read_len = pcm_read(handle, (void *)record_buf, read_record_len);
        /* handle pcm data ,dump to ftp server */
        if (read_len > 0) {
            cmd_printf(handler, "[%s] read_len : %d, CHAN_NUM_CAPTURE:%d\n", CMD, read_len, CHAN_NUM_CAPTURE);
            total_record_size += ftp_client_dumpdata(ftp_handler, record_buf, read_len);
        }
    }
    cmd_printf(handler, "[%s] record_end_time : %d\n", CMD, TICKS_TO_MS(hal_sys_timer_get()));
    cmd_printf(handler, "%s done, total record %ds data size %d byte\n", CMD, dur_s, total_record_size);

    if (handle) {
        cmd_printf(handler, "%s stop capture\n", __FUNCTION__);
        pcm_stop(handle);
        osDelay(10);
        cmd_printf(handler, "%s close capture\n", __FUNCTION__);
        pcm_close(handle);
        handle = NULL;
    }
    if (record_buf) {
        free(record_buf);
        record_buf = NULL;
    }
    if (ftp_handler) {
        ftp_disconnect(ftp_handler);
        ftp_client_free(ftp_handler);
        ftp_handler = NULL;
    }
    return 0;
}
#endif

#endif//__NET_FTP_CLIENT_SUPPORT__

#ifdef IR_FUNCTION_TEST
int do_ir_functions(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s()\n",__func__);
    app_ir_parse(argc,&argv[0]);
    return 0;
}
#endif

extern int system_reset(void);;
int do_rf_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s()\n",__func__);
/*
    hal_sw_bootmode_set(HAL_SW_BOOTMODE_TEST_MODE | HAL_SW_BOOTMODE_TEST_NOSIGNALINGMODE_WIFI);
    osDelay(3);
    hal_cmu_sys_reboot();
*/
    return 0;
}


int do_test_iomux_I2S_to_gpio(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    static const struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_i2s[] = {
        {HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_GPIO,  HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_GPIO,  HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_2, HAL_IOMUX_FUNC_GPIO,  HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},
        {HAL_IOMUX_PIN_P0_3, HAL_IOMUX_FUNC_GPIO,  HAL_IOMUX_PIN_VOLTAGE_MEM, HAL_IOMUX_PIN_PULLUP_ENABLE},
    };

    hal_iomux_init(pinmux_i2s, ARRAY_SIZE(pinmux_i2s));
    TRACE(0, "40087060=0x%04x", *(volatile uint32_t *)0x40087060);
    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P0_0, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P0_1, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P0_2, HAL_GPIO_DIR_OUT, 1);
    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P0_3, HAL_GPIO_DIR_OUT, 1);
    while(1){
        hal_gpio_pin_clr(HAL_IOMUX_PIN_P0_0);
        hal_gpio_pin_clr(HAL_IOMUX_PIN_P0_1);
        hal_gpio_pin_clr(HAL_IOMUX_PIN_P0_2);
        hal_gpio_pin_clr(HAL_IOMUX_PIN_P0_3);
        hal_sys_timer_delay(MS_TO_TICKS(100));
        hal_gpio_pin_set(HAL_IOMUX_PIN_P0_0);
        hal_gpio_pin_set(HAL_IOMUX_PIN_P0_1);
        hal_gpio_pin_set(HAL_IOMUX_PIN_P0_2);
        hal_gpio_pin_set(HAL_IOMUX_PIN_P0_3);
        hal_sys_timer_delay(MS_TO_TICKS(100));
    }
    return 0;
}

#if 0
#include "vow_dev.h"
#include "vow_session.h"
#include "vow_stream.h"
#include "wi2s.h"
#include "wbt_audio.h"
#include "wadc.h"
#include "wusb.h"
#include "vowlan_main.h"
#include "hal_aud.h"
extern enum AUD_STREAM_USE_DEVICE_T i2s_device_role;

#ifdef VOWLAN_SUPPORT_MULTI_AUDIO_PROCESS
#include "multi_iir_process.h"
#include "multi_drc_process.h"
#include "multi_audio_process.h"

extern multi_ap_t *vow_map_handle;
#endif

static int g_cmd_init = 0;

#if 0
//The interface for slave to fix the interface undefine
__WEAK void wusb_init(VOW_DEVICE * vodev, uint32_t RecvInterval)
{
    //The slave is no need to do anything in this interface
    cmd_printf(handler, "%s() weak interface in slave\n",__func__);
}

__WEAK void wadc_init(VOW_DEVICE * vodev)
{
    //The slave is no need to do anything in this interface
    cmd_printf(handler, "%s(NULL) weak interface in slave\n",__func__);
}

__WEAK void wbt_audio_init(VOW_DEVICE * vodev)
{
    //The slave is no need to do anything in this interface
    cmd_printf(handler, "%s(NULL) weak interface in slave\n",__func__);
}

__WEAK void app_i2s_audio_start(VOW_DEVICE * vodev)
{
    //The slave is no need to do anything in this interface
    cmd_printf(handler, "%s(NULL) weak interface in slave \n",__func__);
}
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_USB
int do_vow_usb_audio_input(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s()\n",__func__);
    VOW_DEVICE* at_vow_dev = vow_get_dev();
    wusb_init(at_vow_dev,5000);
    return 0;
}
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_ADC
int do_vow_linein_audio_input(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint32_t smp_rate = 48000;
    uint8_t bits     = 16;

    VOW_DEVICE* at_vow_dev = vow_get_dev();

    // arg parse
    if(g_cmd_init)
    {
        TRACE(0, "The system have init already,if you want config ,please reboot");
        return 0;
    }

    smp_rate = atoi(argv[1]);
    bits     = atoi(argv[2]);
    g_cmd_init = 1;
    cmd_printf(handler, "%s() smp_rate = %d bits =%d g_cmd_init =%d\n",__func__,smp_rate,bits,g_cmd_init);

    wadc_init(at_vow_dev, smp_rate, bits);

    return 0;
}
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_BT
int do_vow_bt_audio_input(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s()\n",__func__);
    VOW_DEVICE* at_vow_dev = vow_get_dev();

    wbt_audio_init(at_vow_dev);
    return 0;
}
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_I2S
int do_vow_i2s_audio_input(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint32_t smp_rate = 48000;
    uint8_t bits     = 16;

    cmd_printf(handler, "%s()\n",__func__);
    VOW_DEVICE* at_vow_dev = vow_get_dev();

    // arg parse
    if(g_cmd_init)
    {
        TRACE(0, "The system have init already,if you want config ,please reboot");
        return 0;
    }
    smp_rate = atoi(argv[1]);
    bits     = atoi(argv[2]);
    g_cmd_init = 1;
    cmd_printf(handler, "%s() smp_rate = %d bits =%d g_cmd_init =%d\n",__func__,smp_rate,bits,g_cmd_init);

    wi2s_init(at_vow_dev , smp_rate , bits);
    return 0;
}
#endif


#if defined(VOWLAN_SUPPORT_INPUT_BY_I2S) || defined(VOWLAN_SUPPORT_OUTPUT_BY_I2S)
int do_vow_i2s_audio_role(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if((strcmp(argv[1], "master") == 0)){
        i2s_device_role = AUD_STREAM_USE_I2S0_MASTER;
    }else if((strcmp(argv[1], "slave") == 0)){
        i2s_device_role = AUD_STREAM_USE_I2S0_SLAVE;
    }
    cmd_printf(handler, "%s() i2s_device_role =%d [master =2,slave=3] \n",__func__,i2s_device_role);

    return 0;
}
#endif


#ifdef VOWLAN_SUPPORT_MULTI_AUDIO_PROCESS
int do_vow_cfg_set_pre_voluem(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() voluem =%d  \n",__func__,atoi(argv[1]));
    multi_ap_cfg_set_pre_voluem(vow_map_handle, atoi(argv[1]));
    return 0;
}

int do_vow_cfg_set_i2s_output_channel(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() i2s output channel %s \n",__func__,argv[1]);
    if((strcmp(argv[1], "R") == 0)){
        multi_ap_cfg_set_output_channel(vow_map_handle, MULTI_AP_OUTPUT_DEV_I2S, MULTI_AP_OUT_CH_R);
    }else if((strcmp(argv[1], "L") == 0)){
        multi_ap_cfg_set_output_channel(vow_map_handle, MULTI_AP_OUTPUT_DEV_I2S, MULTI_AP_OUT_CH_L);
    }

    return 0;
}

int do_vow_cfg_set_dac_output_channel(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() dac output channel %s \n",__func__,argv[1]);
    if((strcmp(argv[1], "R") == 0)){
        multi_ap_cfg_set_output_channel(vow_map_handle, MULTI_AP_OUTPUT_DEV_DAC, MULTI_AP_OUT_CH_R);
    }else if((strcmp(argv[1], "L") == 0)){
        multi_ap_cfg_set_output_channel(vow_map_handle, MULTI_AP_OUTPUT_DEV_DAC, MULTI_AP_OUT_CH_L);
    }

    return 0;
}

int do_vow_cfg_set_i2s_output_volume(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() i2s output voluem =%d  \n",__func__,atoi(argv[1]));
    multi_ap_cfg_set_output_volume(vow_map_handle, atoi(argv[1]), MULTI_AP_OUTPUT_DEV_I2S);
    return 0;
}

int do_vow_cfg_set_dac_output_volume(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() dac output voluem =%d  \n",__func__,atoi(argv[1]));
    multi_ap_cfg_set_output_volume(vow_map_handle, atoi(argv[1]), MULTI_AP_OUTPUT_DEV_DAC);
    return 0;
}

#endif//VOWLAN_SUPPORT_MULTI_AUDIO_PROCESS

int do_get_slave_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    nvrec_wifidevicerecord record;
    int i= 0;

    if (bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record) == 0) {
        for(i= 0 ; i< STA_NUM; i++){
            TRACE(0, "slave mac[%d] ="MACSTR ,i , MAC2STR(record.wifi_record.vow_msg.macData[i]));
        }
    }else{
        TRACE(0, "No mac addr save in flash");
    }
    return 0;
}

int do_set_slave_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    nvrec_wifidevicerecord record;
    unsigned char maddr[ETH_ALEN];
    int index;
    int i;
    if(argc < 2 ){
        printf ("Usage:\n%s\n", cmd->usage);
        return -1;
    }

    memset(&record,0, sizeof(nvrec_wifidevicerecord));
    if (bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record) == 0) {
        for(i= 0 ; i< STA_NUM; i++){
            TRACE(0, "Before slave mac[%d] ="MACSTR ,i , MAC2STR(record.wifi_record.vow_msg.macData[i]));
        }
    }

    index = atoi(argv[1]);
    if (index >= STA_NUM) {
        TRACE(0, "error index:%d", index);
        return 0;
    }
    if (strlen(argv[2])==17) {
        str2mac(argv[2], maddr);
        if (!net_is_valid_mac(maddr)) {
            TRACE(0, "The mac addr is invalid");
            return -1;
        }
        strcpy(record.type, BWIFI_RECORD_TYPE_VOW_MSG);
        memcpy(record.wifi_record.vow_msg.macData[index], maddr, ETH_ALEN);
        bwifi_add_record(&record);
    }

     memset(&record,0, sizeof(nvrec_wifidevicerecord));
     bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record);
     for(i= 0 ; i< STA_NUM; i++){
        TRACE(0, "After slave mac[%d] ="MACSTR ,i , MAC2STR(record.wifi_record.vow_msg.macData[i]));
     }
    return 0;
}

int do_del_slave_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    nvrec_wifidevicerecord record;
    int index;

    if (bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record) != 0) {
        TRACE(0, "Find the slave mac fail");
        return 0;
    }
    if(argc == 1){
        TRACE(0, "del all slave mac");
        memset(record.wifi_record.vow_msg.macData[0], 0 , ETH_ALEN*STA_NUM);
        strcpy(record.type, BWIFI_RECORD_TYPE_VOW_MSG);
        bwifi_add_record(&record);
        return 0;
    }else{
        index = atoi(argv[1]);
        if (index >= STA_NUM) {
            TRACE(0, "error index:%d", index);
            return 0;
        }
        TRACE(0, "del slave[%d] mac", index);
        memset(record.wifi_record.vow_msg.macData[index], 0 , ETH_ALEN);
        strcpy(record.type, BWIFI_RECORD_TYPE_VOW_MSG);
        bwifi_add_record(&record);
    }

    return 0;
}


int do_set_master_ssid(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    nvrec_wifidevicerecord record;
    int status = -1 ;
    TRACE(0, "======>set the ssid : %s" ,argv[1]);
    if( argc < 2){
        TRACE(0, "please enater param like: AT+SMASTERSSID=XXXXXX");
        return -1;
    }
    memset(&record,0, sizeof(nvrec_wifidevicerecord));
    bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record);

    strcpy((char*)record.wifi_record.vow_msg.masterSsid, argv[1]);
    strcpy(record.type, BWIFI_RECORD_TYPE_VOW_MSG);
    status = bwifi_add_record(&record);
    if(status != 0 ){
        TRACE(0, "The ssid wirte to flash fail");
    }

    //to do vertify
    memset(&record,0, sizeof(nvrec_wifidevicerecord));
    if (bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record) != 0) {
        TRACE(0, "read the master ssid fail ");
        return -1;
    }
    TRACE(0, "======>get the ssid :%s",record.wifi_record.vow_msg.masterSsid);

    return 0;
}

int do_get_master_ssid(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    nvrec_wifidevicerecord record;
    //to do vertify
    memset(&record,0, sizeof(nvrec_wifidevicerecord));
    if (bwifi_find_record(BWIFI_RECORD_TYPE_VOW_MSG, &record) != 0) {
        TRACE(0, "read the master ssid fail ");
        return -1;
    }
    TRACE(0, "======>get the ssid :%s",record.wifi_record.vow_msg.masterSsid);

    return 0;
}


int do_set_device_mac_to_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = 0;
    unsigned char maddr[ETH_ALEN];
    if (strlen(argv[1])==17) {
        str2mac(argv[1], maddr);
        if (!net_is_valid_mac(maddr)) {
            TRACE(0, "The mac addr is invalid");
            return -1;
        }
    }
    ret = factory_section_set_wifi_address(maddr);
    if(ret != 0){
        TRACE(0, "Notice set wifi mac addrress fail,please check!!!");
    }

    return 0;
}

int do_get_device_mac_from_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = 0;
    uint8_t *wifi_addr = factory_section_get_wifi_address();
    if (wifi_addr) {
        TRACE(0, "get mac from factory: " MACSTR "\n", MAC2STR(wifi_addr));
    }
    return 0;
}
#endif

extern int hal_uart0_i2c_switch(void);
extern void hal_uart0_i2c_switch_init(void);

static int do_uart_i2c_switch(struct cmd_tbl_s *cmd, int argc, char *argv[], void *handler)
{
    hal_uart0_i2c_switch_init();
    hal_uart0_i2c_switch();
    //hal_iomux_set_analog_i2c();
    return 0;
}

int do_set_jtag_mode(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if(argc == 2){
        if ((strcmp(argv[1], "CP") == 0)) {
            TRACE(0, "%s %d select jtag cp mode", __func__,__LINE__);
            hal_cmu_jtag_sel(HAL_CMU_JTAG_SEL_CP);
        }
#ifdef CHIP_HAS_A7_DSP
        else if (strcmp(argv[1], "A7") == 0) {
            TRACE(0, "%s %d select jtag a7 mode", __func__,__LINE__);
            hal_cmu_jtag_sel(HAL_CMU_JTAG_SEL_A7);
        }
#endif
        else if (strcmp(argv[1], "enable") == 0) {
            TRACE(0, "%s %d jtag enable", __func__,__LINE__);
            *(uint32_t*)0x400000F8 &= 0x7FFFFFFF;//clear bit31
            hal_iomux_set_jtag();
            hal_cmu_jtag_enable();
            hal_cmu_jtag_clock_enable();
        } else {
            TRACE(0, "%s %d: AT+JTAG=enable(main),AT+JTAG=A7/CP", __func__,__LINE__);
        }
    }
    RESP_OK(handler);
    return 0;
}

#ifdef BLE_ENABLE
#include "app_ble_include.h"
const static uint8_t adv_addr_set[6] = {0x66, 0x34, 0x33, 0x23, 0x22, 0x11};

int ble_adv_enable(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t advEnable;
    uint16_t advInterval;
    char *advname = "bes_adv_test";
    char advdata[31] = {0};


#ifdef TEST_BLE_SERVICE
static int service_added = false;
if(!service_added){
    extern void ble_demo_init(void);
    service_added = true;
    ble_demo_init();
}
#endif

    advEnable = atoi(argv[1]);
    advInterval = atoi(argv[2]);
    advdata[0] = 0x02;
    advdata[1] = 0x01;
    advdata[2] = 0x06;
    advdata[3] = 0x1b;
    advdata[4] = 0x09;
    memcpy(&advdata[5],advname,strlen(advname));

    cmd_printf(handler, "%s() %s %d  %d\n",__func__,argv[0],advEnable,advInterval);

    if(advEnable)
    {
        app_ble_custom_init();
        app_ble_custom_adv_write_data(0,
                                    true,
                                    (uint8_t *)adv_addr_set,
                                    NULL,
                                    advInterval,
                                    ADV_TYPE_UNDIRECT,
                                    12,
                                    (uint8_t *)advdata, 31,
                                    NULL, 0);

        app_ble_custom_adv_start(0);
      }
    else
    {
        app_ble_custom_adv_stop(0);
    }

    return 0;
}
#include "app_ble_uart.h"
int ble_adv_test_case(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t advEnable;
    uint8_t advTestCase;
    if(argc < 3)
    {
        TRACE(0,"%s() cmd format: AT+BLEADV=1(adv_en),1(test case) there should be at least 2 args",__func__);
        return -1;
    }
    advEnable = atoi(argv[1]);
    advTestCase = atoi(argv[2]);
    TRACE(0, "%s() advEnable=%d,advTestCase=%d\n",__func__,advEnable,advTestCase);

    if(advEnable){
      switch(advTestCase){
        case 1:
            ble_start_adv_1();
            break;
        case 2:
            ble_start_adv_2();
            break;
        case 3:
             ble_start_adv_3();
            break;
        case 4:
            ble_start_three_adv();
            break;
        default:
            ble_start_adv_1();
            break;
      }

    }else{
      switch(advTestCase){
        case 1:
            ble_stop_adv_1();
            break;
        case 2:
            ble_stop_adv_2();
            break;
        case 3:
                 ble_stop_adv_3();
            break;
        case 4:
            ble_stop_all_adv();
            break;
        default:
            ble_stop_adv_1();
            break;
      }
    }
         return 0;

}
int ble_scan_enable(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t scanEnable;
    uint16_t scanWindow, scanInterval;

    scanEnable = atoi(argv[1]);
    scanWindow = atoi(argv[2]);
    scanInterval = atoi(argv[3]);
    cmd_printf(handler, "%s() %s %d  %d  %d\n",__func__,argv[0],scanEnable,scanWindow,scanInterval);

    if(scanEnable)
    {
        app_ble_start_scan(0, scanWindow, scanInterval);
    }
    else
    {
        app_ble_stop_scan();
    }

    return 0;
}

static int at_ble_start_connect(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
      uint8_t conEnable = 0;
      ble_bdaddr_t ble_connect_addr={
        .addr={0x6f,0x64,0x35,0x54,0xdb,0xbd},
        .addr_type = 0,/*GAPM_STATIC_ADDR*/
    };
    uint8_t *p=ble_connect_addr.addr;
       if(argc < 4)
       {
            TRACE(0,"%s() cmd format: AT+BLECONN=1(enable),66 34 33 23 22 11(remote addr),0(addr type)there should be at least 3 args",__func__);
             return -1;
       }
       conEnable = atoi(argv[1]);
        if(strlen(argv[2])==17) {
            str2mac(argv[2], p);
        }
    ble_connect_addr.addr_type=atoi(argv[3]);
    TRACE(0,"%s() conEnable=%d,peer addr %2X:%2X:%2X:%2X:%2X:%2X, addr_type=%d\n",
        __func__,conEnable,p[0],p[1],p[2],p[3],p[4],p[5],ble_connect_addr.addr_type);

    if(conEnable)
    {
        app_ble_start_connect(&ble_connect_addr);
    }
    else
    {
        app_ble_cancel_connecting();
    }
    return 0;
}
#ifdef __BLE_WIFICONFIG__
#include "app_datapath_server.h"
char ble_config_buf[32+64] = {0};
void wifi_info_from_ble(uint8_t* ptrData,uint16_t length)
{

	TRACE(0,"%s() \n",__func__);
	DUMP8("%02x ",ptrData,length);
    if(memcmp(ptrData, "wificonfig", 10) == 0){
        ptrData += 10;
        length -= 10;
    }
	memcpy(ble_config_buf,ptrData,length);
	ble_config_buf[length] = '\0';
}

static int  ble_wificonfig(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t bleEnable;
    char wifi_info_ssid[32] = {0};
    char wifi_info_passwd[64] = {0};
    int i;
    int ret = 0;


    if(argc < 2)
    {
        TRACE(0,"%s() cmd format: AT+WIFICONFIG=0(regiseter)/1(get ssid & passwd info)/2(wifi connect to AP)\n",__func__);
        return -1;
    }
    TRACE(0,"%s() \n",__func__);
    bleEnable = atoi(argv[1]);
    TRACE(1,"%s() bleenable =%d \n",__func__,bleEnable);
    if(bleEnable == 0)
    {
        app_datapath_server_register_rx_done(wifi_info_from_ble);
    }
    else if(bleEnable == 1)
    {
       memset(wifi_info_ssid,0,sizeof(wifi_info_ssid));
       memset(wifi_info_passwd,0,sizeof(wifi_info_passwd));
       memcpy((char *)wifi_info_ssid, ble_config_buf,sizeof(wifi_info_ssid)-1);
       memcpy((char *)wifi_info_passwd, ble_config_buf+32,sizeof(wifi_info_passwd)-1);
       TRACE(2,"wifi_info_ssid:%s,wifi_info_passwd:%s\n", wifi_info_ssid, wifi_info_passwd);
    }
    else if(bleEnable == 2)
    {
       memset(wifi_info_ssid,0,sizeof(wifi_info_ssid));
       memset(wifi_info_passwd,0,sizeof(wifi_info_passwd));
       memcpy((char *)wifi_info_ssid, ble_config_buf,sizeof(wifi_info_ssid)-1);
       memcpy((char *)wifi_info_passwd, ble_config_buf+32,sizeof(wifi_info_passwd)-1);
        TRACE(1,"wifi connect to %s \n",wifi_info_ssid);
        ret = bwifi_connect_to_ssid(wifi_info_ssid, wifi_info_passwd, 0, 0, NULL);
        if(ret)
        {
            TRACE(0,"bwifi_connect_to_ssid connect fail\n");
        }
        else
            TRACE(0,"bwifi_connect_to_ssid connect success\n");
    }
    else
    TRACE(0,"please input AT+WIFICONFIG=0/1/2 0:regiseter 1: get info 2: connect ap\n");

    return 0;
}
#endif
#endif


#if !defined(__BT_CLOSE__)

typedef uint8_t btif_accessible_mode_t;

#define BTIF_BAM_NOT_ACCESSIBLE     0x00    /* Non-discoverable or connectable */
#define BTIF_BAM_GENERAL_ACCESSIBLE 0x03    /* General discoverable and connectable */

extern void app_bt_accessmode_set_by_AT_command(btif_accessible_mode_t mode);
int bt_scan_enable(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t advEnable;
    advEnable = atoi(argv[1]);

    cmd_printf(handler, "%s() %s %d\n",__func__,argv[0],advEnable);

    if(advEnable)
    {
        app_bt_accessmode_set_by_AT_command(BTIF_BAM_GENERAL_ACCESSIBLE);
    }
    else
    {
        app_bt_accessmode_set_by_AT_command(BTIF_BAM_NOT_ACCESSIBLE);
    }

    return 0;
}

#ifdef USE_BT_ADAPTER
extern void bes_bt_init(void);
extern bool app_is_stack_ready(void);
static int bt_init(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t BTEnable;
    BTEnable = atoi(argv[1]);

    if(BTEnable)
    {
        if(!app_is_stack_ready())
        {
            bes_bt_init();
        }
        else
        {
            TRACE(0,"%s() bt already init\n",__func__);
        }
    }

    return 0;
}
#endif

#ifdef DEBUG_SPP_LOG
static void bt_spp_init(void)
{
    AtSppInitApi();
}
#endif

/*
#include "bt_drv_reg_op.h"
int get_bt_rssi_value(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    rx_agc_t rssi_val;
    for(int i =0;i<3;i++)
    {
        bt_drv_read_rssi_in_dbm(0x80+i,&rssi_val);
        TRACE(0, "RSSI:%d  rxgain:%d",rssi_val.rssi,rssi_val.rxgain);
    }
    return 0;
}
*/

extern int factory_section_set_bt_address(uint8_t *bt_addr);
extern uint8_t* factory_section_get_bt_address(void);
extern int factory_section_set_bt_name(const char *name,int len);
extern int factory_section_set_ble_address(uint8_t* bleAddr);

static int do_set_bt_mac_to_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = 0;
    unsigned char maddr[ETH_ALEN] = {0};
    if (strlen(argv[1])==17) {
        str2mac(argv[1], maddr);
        if (!bt_is_valid_mac(maddr)) {
            TRACE(0, "The mac addr is invalid");
            return -1;
        }
    }
    ret = factory_section_set_bt_address(maddr);
    if(ret != 0){
        TRACE(0, "Notice set bt mac addrress fail,please check!!!");
    }

    return 0;
}

static int do_set_ble_mac_to_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = 0;
    unsigned char maddr[ETH_ALEN];
    if (strlen(argv[1])==17) {
        str2mac(argv[1], maddr);
        if (!bt_is_valid_mac(maddr)) {
            TRACE(0, "The mac addr is invalid");
            return -1;
        }
    }
    ret = factory_section_set_ble_address(maddr);
    if(ret != 0){
        TRACE(0, "Notice set ble mac addrress fail,please check!!!");
    }

    return 0;
}

static int do_set_bt_name_to_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = 0;

    ret = factory_section_set_bt_name(argv[1],strlen(argv[1]) +1 );
    TRACE(1,"set bt name: %s",argv[1]);
    if(ret != 0){
        TRACE(0, "Notice set bt name fail,please check!!!");
    }

    return 0;
}

static int do_get_bt_mac_from_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t *wifi_addr = factory_section_get_bt_address();
    if (wifi_addr) {
        TRACE(0, "get bt mac from factory: " MACSTR "\n", MAC2STR(wifi_addr));
        RESP_OK(handler);
    }
    return 0;
}

#endif

#if BLE_AUDIO_ENABLED && defined(BLE_V2)
extern void app_bap_bc_src_start(uint8_t big_idx);
extern void app_bap_bc_scan_start(uint8_t scan_trigger_method);
extern void app_bis_set_src_pcm_input_type(uint8_t type);
extern void app_bap_bc_src_stop(uint8_t big_idx);
extern void app_bis_sink_stop(void);
extern void app_set_src_cmd(uint8_t src_cmd);
extern void app_set_sink_cmd(uint8_t sink_cmd);
extern void app_bap_bc_scan_stop(void);

static int do_start_bis_source(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t type;
    type = atoi(argv[1]);
    app_set_src_cmd(0);
    app_bis_set_src_pcm_input_type(type);
    app_bap_bc_src_start(0);
    return 0;
}

static int do_start_bis_sink(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    app_set_sink_cmd(0);
    app_bap_bc_scan_start(0);
    return 0;
}

static int do_stop_bis_source(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    app_set_src_cmd(1);
    app_bap_bc_src_stop(0);
    return 0;
}

static int do_stop_bis_sink(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    app_set_sink_cmd(1);
    app_bap_bc_scan_stop();
    app_bis_sink_stop();
    return 0;
}
#endif

#ifdef IBRT
static int do_start_ibrt(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t role = atoi(argv[1]);
    uint8_t addr[6];
    if (strlen(argv[2])==17) {
        str2mac(argv[2], addr);
    }
    TRACE(0,"address:");
    DUMP8("%02x ",addr,6);
    app_ibrt_if_start_tws_pairing(role, addr);
    return 0;
}

static int do_stop_ibrt(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    bt_bdaddr_t invaild_addr = {{0x00,0x00,0x00,0x00,0x00,0x00}};
    struct nvrecord_env_t *nvrecord_env;
    nv_record_env_get(&nvrecord_env);
    app_ibrt_conn_disconnect_ibrt(&nvrecord_env->ibrt_mode.record.bdAddr);
    nv_record_update_ibrt_info(IBRT_UNKNOW, &invaild_addr);
    //nv_record_flash_flush();
    nv_record_rebuild();
    return 0;
}
#endif

#ifdef LINEIN_TRANS_ON
extern void app_ibrt_raw_ui_media_switch_test(void);
static int do_switch_besaud_onoff(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    app_ibrt_raw_ui_media_switch_test();
    return 0;
}
#endif

#if !defined(__BT_CLOSE__)
#include "hal_aud.h"
#include "app_bt_stream.h"
int8_t stream_bis_volume = 10;
static int do_volumeup(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    stream_bis_volume ++;
    if (stream_bis_volume > TGT_VOLUME_LEVEL_MAX)
        stream_bis_volume = TGT_VOLUME_LEVEL_MAX;
    app_bt_stream_volumeset(stream_bis_volume);
    return 0;
}

static int do_volumedown(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    stream_bis_volume --;
    if (stream_bis_volume < TGT_VOLUME_LEVEL_MUTE)
        stream_bis_volume = TGT_VOLUME_LEVEL_MUTE;
    app_bt_stream_volumeset(stream_bis_volume);
    return 0;
}
#endif

#ifdef __WIFI_APP_SUPPORT__
static int do_set_wifi_mac_to_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int ret = 0;
    unsigned char maddr[ETH_ALEN];
    if (strlen(argv[1])==17) {
        str2mac(argv[1], maddr);
        if (!net_is_valid_mac(maddr)) {
            TRACE(0, "The mac addr is invalid");
            return -1;
        }
    }
    ret = factory_section_set_wifi_address(maddr);
    if(ret != 0){
        TRACE(0, "Notice set wifi mac addrress fail,please check!!!" MACSTR "\n", MAC2STR(maddr));
    }
    RESP_OK(handler);
    return 0;
}

static int do_get_wifi_mac_from_factory(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t *wifi_addr = factory_section_get_wifi_address();
    if (wifi_addr == NULL) {
        TRACE(0, "get mac from factory fail,factory no wifi mac address?\n");
        return -1;
    }

    TRACE(0, "get wifi mac from factory: " MACSTR "\n", MAC2STR(wifi_addr));
    RESP_OK(handler);
    return 0;
}

static int do_set_ps_mode(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t ps_mode;
    ps_mode = atoi(argv[1]);
    ps_mode = ps_mode ? 1 : 0;
    bwifi_set_ps_mode(ps_mode);
    RESP_OK(handler);
    return 0;
}
#endif

#ifdef LPLAYER_SUPPORT_AT_CMD
#ifdef BT_MEDIA_SUPPORT_LPLAYER
#include "aud_player.h"
#else
#include "lplayer.h"

//#include "audio_sink.h"
static lplayer_t *g_lplayer_at_instance = NULL;
static uint8_t g_lplayer_at_terminated = 0;
static int lplayer_on_complete_notify(void)
{
    TRACE(0, "%s", __func__);
    g_lplayer_at_terminated = 1;
    return 0;
}

static int lplayer_on_error_notify(int error_code)
{
    TRACE(0, "%s error_code = %d", __func__, error_code);
    g_lplayer_at_terminated = 1;
    return 0;
}

#endif //BT_MEDIA_SUPPORT_LPLAYER

static void lplayer_printf_process_result(const char * process, const char* result)
{
    if(process && result) {
        TRACE(0, "lplayer %s %s", process, result);
    }
}

#define LPLAYER_SUCC(s) do{lplayer_printf_process_result(s, "success");}while(0);
#define LPLAYER_ERROR(s) do{lplayer_printf_process_result(s, "error");}while(0);

static int do_lplayer_start(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    char *uri = NULL;
    TRACE(0, "argc = %d", argc);
    if (argc >= 2)
    {
        uri = argv[1];
    }
#ifdef BT_MEDIA_SUPPORT_LPLAYER
    int ret = uri?aud_player_start(uri, NULL):aud_player_resume();
    TRACE(0, "%s(%s) returns %d", __func__, uri?uri:"", ret);
#else
    if (g_lplayer_at_terminated)
    {
        TRACE(0, "%s close last lplayer", __func__);
        lplayer_close(g_lplayer_at_instance);
        g_lplayer_at_instance = NULL;
    }

    if (g_lplayer_at_instance == NULL)
    {
        g_lplayer_at_instance = lplayer_open_url(uri);
        if (g_lplayer_at_instance == NULL)
        {
            LPLAYER_ERROR("start");
            return -1;
        }
        lplayer_register_complete_notify(g_lplayer_at_instance, lplayer_on_complete_notify);
        lplayer_register_error_notify(g_lplayer_at_instance, lplayer_on_error_notify);
    }

    if (0 != lplayer_play(g_lplayer_at_instance))
    {
        lplayer_close(g_lplayer_at_instance);
        g_lplayer_at_instance = NULL;
        LPLAYER_ERROR("start");
        return -1;
    }
    g_lplayer_at_terminated = 0;
    LPLAYER_SUCC("start");
#endif
    return 0;
}

static int do_lplayer_stop(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#ifdef BT_MEDIA_SUPPORT_LPLAYER
    int ret = aud_player_stop();
    TRACE(0, "%s returns %d", __func__, ret);
#else
    if (0 != lplayer_close(g_lplayer_at_instance))
    {
        LPLAYER_ERROR("stop");
        return -1;
    }
    g_lplayer_at_instance = NULL;
    LPLAYER_SUCC("stop");
#endif
    return 0;
}

static int do_lplayer_pause(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#ifdef BT_MEDIA_SUPPORT_LPLAYER
    int ret = aud_player_pause();
    TRACE(0, "%s returns %d", __func__, ret);
#else  
    if (0 != lplayer_pause(g_lplayer_at_instance))
    {
        LPLAYER_ERROR("pause");
        return -1;
    }
    LPLAYER_SUCC("pause");
#endif
    return 0;
}

static int do_lplayer_set_vol(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int vol = atoi(argv[1]);
#ifdef BT_MEDIA_SUPPORT_LPLAYER
        if (0 != aud_player_set_vol(vol))
#else
        if (0 != lplayer_set_vol(g_lplayer_at_instance, vol))
#endif
        {
            LPLAYER_ERROR("set volume");
            return -1;
        }
        RESP(handler, "volume=%d", vol);
    } else {
        //read volume
        uint8_t nv_record_get_aud_vol(void);
        RESP(handler, "volume=%d", nv_record_get_aud_vol());
    }
    return 0;
}

#ifndef BT_MEDIA_SUPPORT_LPLAYER
static int do_lplayer_enable_single_loop(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int en = atoi(argv[1]);
        TRACE(0, "en = %d", en);
        if (en != 0)
        {
            en = 1;
        }
        if (0 != lplayer_single_loop_en(g_lplayer_at_instance, en))
        {
            LPLAYER_ERROR("enable single loop");
            return -1;
        }
        LPLAYER_SUCC("enable single loop");
        return 0;
    }
    LPLAYER_ERROR("enable single loop");
    return -1;
}
#endif
static int do_lplayer_seek(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 2)
    {
        int time_s = atoi(argv[1]);
        TRACE(0, "time_s = %d", time_s);
#ifdef BT_MEDIA_SUPPORT_LPLAYER
        if (0 != aud_player_seek(time_s))
#else
        if (0 != lplayer_seek(g_lplayer_at_instance, time_s))
#endif
        {
            LPLAYER_ERROR("seek");
            return -1;
        }
        LPLAYER_SUCC("seek");
        return 0;
    }
    LPLAYER_ERROR("seek");
    return -1;
}

static int do_lplayer_show_current_play_time(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#ifdef BT_MEDIA_SUPPORT_LPLAYER
    int cur_play_time = aud_player_tell();
    if(cur_play_time < 0)
#else
    uint32_t cur_play_time = 0;
    if (0 != lplayer_get_current_pos(g_lplayer_at_instance, &cur_play_time))
#endif
    {
        LPLAYER_ERROR("get_pos");
        return -1;
    }
    TRACE(0, "%s cur_play_time ====> %d ms", __func__, cur_play_time);
    LPLAYER_SUCC("get_pos");
    return 0;
}

// static int do_lplayer_show_memory_usage(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
// {
//     lplayer_show_memory_usage();
//     return 0;
// }

static int do_lplayer_set_global_sink_type(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#if 0
    if (argc >= 2)
    {
        int type = atoi(argv[1]);
        audio_sink_type_t sink_type = 0;
        switch(type)
        {
            case 0:
            sink_type = AUDIO_SINK_ALSA;
            break;
            case 1:
            sink_type = AUDIO_SINK_LE_AUDIO;
            break;
            case 2:
            sink_type = AUDIO_SINK_BT_AUDIO;
            break;
            default:
            break;
        }
        if(sink_type != 0)
        {
            audio_sink_global_sink_type_set(sink_type);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
	#endif
	//not support from bes's message.
    return 0;
}
#endif

#ifdef __ALSA_EN__
#include "alsa.h"
#include "alsa_extra.h"

static int do_alsa_dump_config_ftp_server(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    char *ip_server = NULL;
    int port = -1;
    char *user_name = NULL;
    char *pwd = NULL;

    if (argc < 3)
    {
        TRACE(0, "%s argc %d error", __func__, argc);
        return -1;
    }

    if (argc >= 3)
    {
        ip_server = argv[1];
        port = atoi(argv[2]);
    }

    if (argc >= 5)
    {
        user_name = argv[3];
        pwd = argv[4];
    }

    TRACE(0, "%s argc = %d ip_server = %s (%d), port = %d, user_name = %s (%d), pwd = %s (%d)",
          __func__, argc,
          ip_server ? ip_server : NULL, ip_server ? strlen(ip_server) : 0,
          port,
          user_name ? user_name : NULL, user_name ? strlen(user_name) : 0,
          pwd ? pwd : NULL, pwd ? strlen(pwd) : 0);

    return alsa_dump_ftp_server_cofig(ip_server, port, user_name, pwd);

    return 0;
}

static int do_alsa_dump(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc != 3)
    {
        TRACE(0, "%s argc error", __func__);
        return -1;
    }

    int type = atoi(argv[1]);
    int enable = atoi(argv[2]);
    if (enable)
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }

    int ret;

    switch (type)
    {
    case 0:
        TRACE(0, "%s dump after_instance_write en = %d", __func__, enable);
        ret = alsa_dump_after_instance_write(enable);
        break;
    case 1:
        TRACE(0, "%s dump after_sw_gain_process en = %d", __func__, enable);
        ret = alsa_dump_after_sw_gain(enable);
        break;
    case 2:
        TRACE(0, "%s dump before_audio_process en = %d", __func__, enable);
        ret = alsa_dump_before_audio_process(enable);
        break;
    case 3:
        TRACE(0, "%s dump after_audio_process en = %d", __func__, enable);
        ret = alsa_dump_after_audio_process(enable);
        break;
    case 4:
        TRACE(0, "%s dump after_demuxer en = %d", __func__, enable);
        ret = alsa_dump_after_demuxer(enable);
        break;
    default:
        TRACE(0, "%s current not support", __func__);
        ret = -1;
        break;
    }

    return ret;
}

static int do_alsa_ultrasound(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int enable = atoi(argv[1]);
     if(enable)
     {
         TRACE(0, "start alsa ultrasound");
         alsa_ultrasound_start();
     }
     else
     {
         TRACE(0, "stop alsa ultrasound");
         alsa_ultrasound_stop();
     }
     return 0;
}

static int do_alsa_set_float_vol(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 2)
    {
        return -1;
    }

    int volume = atoi(argv[1]);
    float volume_f = volume / 100.0f;
    // TRACE(0, "set volume = %d, volume_f = %f", volume, volume_f);
    alsa_volume_float_set(volume_f);
    return 0;
}

static int do_alsa_get_float_vol(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    float volume_f = 0.0f;
    alsa_volume_float_get(&volume_f);
    TRACE(0, "get volume = %f", (double)volume_f);
    return 0;
}

extern int alsa_capture_loop_playback_test(bool onoff);
static int do_alsa_loop_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
#if 0
    if (argc < 2)
    {
        return -1;
    }

    int onoff = atoi(argv[1]);
    if(onoff) {
        return alsa_capture_loop_playback_test(true);
    } else {
        return alsa_capture_loop_playback_test(false);
    }
#endif
	//removed by bes.
	return 0;
}

#endif // __ALSA_EN__

#ifdef USE_BT_ADAPTER
extern void app_bt_af_stream_start(void);
extern void app_bt_af_stream_stop(void);

static int do_bt_af_ctrl(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int enable = atoi(argv[1]);
    if(enable)
    {
        app_bt_af_stream_start();
    }
    else
    {
        app_bt_af_stream_stop();
    }
    return 0;
}
#endif

#ifdef USB_AUDIO_ADAPTER
extern void UsbAudioDevInit(unsigned int recvInterval);
static void do_usbaudio_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    UsbAudioDevInit(5000);
}

static void do_hid_pause_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    usb_audio_hid_set_event(USB_AUDIO_HID_PLAY_PAUSE,1);
    osDelay(10);
    usb_audio_hid_set_event(USB_AUDIO_HID_PLAY_PAUSE,0);
}
#endif

#ifdef __CHIP_OCD__
int at_watch_func(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    volatile uint32_t *addr = NULL;
    char *type = NULL;
    enum watchpoint_rw rw = DWT_MODE_OTHER;
    char *watch_func = NULL;

    watch_func = argv[1];
    TRACE(0, "argv[1]=%s", watch_func);
    if (argc >= 3) {
        addr = (volatile uint32_t *)strtoul(argv[2], NULL, 16);
    }
    if (argc == 4) {
        type = argv[3];
        if (strcmp(type, "read") == 0) {         /* read watchpoint */
            rw = DWT_READ;
        } else if (strcmp(type, "write") == 0) {  /* write watchpoint */
            rw = DWT_WRITE;
        } else if (strcmp(type, "access") == 0) {  /* access watchpoint */
            rw = DWT_ACCESS;
        } else {
            TRACE(0, "watch point invalid defalut write");
            rw = DWT_WRITE;
        }
    } else {
        TRACE(0, "watch point defalut write");
        rw = DWT_WRITE;
    }
    if (argc >= 3) {
        TRACE(0,"%s watch addr 0x%x  type %s", watch_func, (uint32_t)addr, watchpoint_rw_strings[rw]);
    }
    if (addr && strcmp(watch_func, "add") == 0) {
        ocd_add_watch_point((uint32_t)addr, rw);
    } else if (addr && strcmp(watch_func, "remove") == 0) {
        ocd_remove_watch_point((uint32_t)addr, rw);
    } else if (strcmp(watch_func, "show") == 0) {
        ocd_watchpoint_show();
    } else {
        TRACE(0, "ERROR watch point opeart, for example:");
        TRACE(0, "AT+WATCH=add, 0x00000000, write");
        TRACE(0, "AT+WATCH=remove, 0x00000000");
        TRACE(0, "AT+WATCH=show");
    }
    return 0;
}

volatile uint32_t  g_dwt_test = 0;
int debug_dwt_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc >= 1) {
        if (strcmp(argv[1], "add") == 0) {
            TRACE(0,"add watch point test");
            ocd_add_watch_point((uint32_t)&g_dwt_test, DWT_WRITE);
        } else if(strcmp(argv[1], "rm") == 0) {
            TRACE(0,"add watch_off");
            ocd_remove_watch_point((uint32_t)&g_dwt_test, DWT_WRITE);
        } else if(strcmp(argv[1], "trig") == 0) {
            TRACE(0,"add debug_watch trig ");
            /* trigger watchpoint, system will crash */
            g_dwt_test = 1;
        } else if (strcmp(argv[1], "show") == 0) {
            TRACE(0,"add debug_watch show");
            ocd_watchpoint_show();
        } else {
            TRACE(0,"dwt cmd not support");
            TRACE(0,"AT+WATCHPOINT test:");
            TRACE(0,"AT+WATCHPOINT=add : watch one global int");
            TRACE(0,"AT+WATCHPOINT=rm : remove one watchpoint");
            TRACE(0,"AT+WATCHPOINT=trig: trigger watchpoint,system will crash");
            TRACE(0,"AT+WATCHPOINT=show: show watchpoint");
        }
    } else {
            TRACE(0,"dwt cmd not support");
            TRACE(0,"AT+WATCHPOINT test:");
            TRACE(0,"AT+WATCHPOINT=add : watch one global int");
            TRACE(0,"AT+WATCHPOINT=rm : remove one watchpoint");
            TRACE(0,"AT+WATCHPOINT=trig: trigger watchpoint,system will crash");
            TRACE(0,"AT+WATCHPOINT=show: show watchpoint");
    }
    return 0;
}
#endif

static int do_reboot_func(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    TRACE(0,"Now reboot system...");
    osDelay(10);
    hal_cmu_sys_reboot();
    return 0;
}

#ifdef KERNEL_LITEOS_M
static void do_liteos_pagpio_init(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    TRACE(0,"gpio init start");
    /* fsusb_gpio GpioInit*/
    struct HAL_IOMUX_PIN_FUNCTION_MAP fsusb_gpio[] = {
         {HAL_IOMUX_PIN_P3_0, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
         {HAL_IOMUX_PIN_P1_2, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}
    };
    hal_iomux_init(&fsusb_gpio[0], sizeof(fsusb_gpio)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)fsusb_gpio[0].pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(10);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)fsusb_gpio[1].pin, HAL_GPIO_DIR_OUT, 0);
    osDelay(10);

    /* pa_gpio GpioInit*/
    struct HAL_IOMUX_PIN_FUNCTION_MAP pa_gpio[] = {
        {HAL_IOMUX_PIN_P3_5, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P3_4, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
        {HAL_IOMUX_PIN_P0_5, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL}
    };
    hal_iomux_init(&pa_gpio[0], sizeof(pa_gpio)/sizeof(struct HAL_IOMUX_PIN_FUNCTION_MAP));
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pa_gpio[0].pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(100);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pa_gpio[1].pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(100);

    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)pa_gpio[2].pin, HAL_GPIO_DIR_OUT, 1);
    osDelay(100);
    TRACE(0,"gpio init done");
}
#endif

#if defined(__A7_DSP_ENABLE__)
static int do_send2a7_cmd(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() argc: %d \n",__func__,argc);
    if (argc >= 2) {
        A7_CMD_T a7_cmd;
        a7_cmd.p2 = 0;
        a7_cmd.p1 = 0;
        if (strcmp(argv[1], "PANIC") == 0) {
            a7_cmd.type = A7_CMD_TYPE_PANIC;
            if(argc == 3) {
                a7_cmd.p1 = atoi(argv[2]);
            }
        } else if (strcmp(argv[1], "PS") == 0) {
            a7_cmd.type = A7_CMD_TYPE_DEBUG_MM;
            if(argc == 3) {
                a7_cmd.p1 = atoi(argv[2]);
            }
        } else if (strcmp(argv[1], "disable") == 0) {
            mcu_dsp_disable();
            cmd_printf(handler, "%s() disable a7 dsp\n", __func__);
            RESP_OK(handler);
            return 0;
        } else if (strcmp(argv[1], "enable") == 0) {
            mcu_dsp_enable();
            cmd_printf(handler, "%s() enable a7 dsp\n", __func__);
            RESP_OK(handler);
            return 0;
        } else {
            a7_cmd.type = A7_CMD_TYPE_CUST_CTRL_PLANE;
            a7_cmd.p1 = atoi(argv[1]);
            if(argc == 3) {
                a7_cmd.p2 = atoi(argv[2]);
            }
        }
        cmd_printf(handler, "%s() cmd type:0x%x, P1:0x%x, p2:0x%x\n",__func__, a7_cmd.type, a7_cmd.p1, a7_cmd.p2);
        mcu_cmd_send(&a7_cmd);
        RESP_OK(handler);
    }
    return 0;
}

extern void a7_dsp_reboot(void);
static int do_ca7_reboot(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    TRACE(0,"Now reboot a7 dsp...");
    a7_dsp_reboot();
    return 0;
}
#endif

#ifdef SPEECH_MSG_TEST_CASE
#include "speech_msg_test.h"
static int do_speech_msg_config(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() argc: %d \n", __func__, argc);
#ifdef SPEECH_MSG_TEST_TX
    if (argc > 2) {
        if (strcmp(argv[1], "tx") == 0) {
            uint32_t interval = atoi(argv[2]);
            speech_msg_test_init();
            speech_msg_set_tx_interval(interval);
            if ((argc > 3) && (strcmp(argv[3], "-r") == 0)) {
                speech_msg_a7_tx_start(interval);
                speech_msg_tx_stop();
            } else if ((argc > 3) && (strcmp(argv[3], "-b") == 0)) {
                speech_msg_tx_start(interval);
                speech_msg_a7_tx_start(interval);
            } else {
                speech_msg_tx_start(interval);
                speech_msg_a7_tx_stop();
            }
        }
        cmd_printf(handler, "%s() paramter[1]:%s, paramter[2]:%s\n",__func__, argv[1], argv[2]);
        RESP_OK(handler);
    }
#endif
    return 0;
}
#endif

#ifdef CP_BOOT
extern void cp_reboot(void);
static int do_cp_reboot(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    TRACE(0,"Now reboot cp...");
    cp_reboot();
    return 0;
}
#endif

static int panic_trigger(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    __asm__ __volatile__("udf":::"memory");
    return 0;
}

#define BTDIGITAL_REG(a)                        (*(volatile uint32_t *)(a))
static int do_controller_tport(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t hexval[8];
    uint32_t val = 0, reg = 0;
    val = StringToHex(argv[1], hexval, NULL);
    TRACE(1,"0xd0220050:0x%08x",val);
    BTDIGITAL_REG(0x40086014) = 0x00000F0F;
    BTDIGITAL_REG(0x40086018) = 0x00000005;
    reg = BTDIGITAL_REG(0x40086008);
    BTDIGITAL_REG(0x40086008) = (reg&0xFFFF0000)|0xeeee;
    BTDIGITAL_REG(0x40086004) = 0x0FFFeeee;
    BTDIGITAL_REG(0xd0340000) = 0x23620200;
    BTDIGITAL_REG(0xd0220050) = val;

    return 0;
}
#ifdef BLE_GATT_CLIENT_TEST
extern int gatt_client_test(size_t argc, char *argv[]);
static int do_gatt_client_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int i=0;
    while(i<argc)
    {
        TRACE(0, "argv[%d]=%s\n",i,argv[i]);
        i++;
    }

    //AT+BLEGATTC=conn,66:55:44:33:22:11
    //argv[0]:AT+BLEGATTC
    //argv[1]:conn
    //argv[2]:66:55:44:33:22:11
    //argc=3

    gatt_client_test(argc-1,&argv[1]);
    return 0;
}
#endif
static int a7_trace_hook(const char *tag, const char *fmt, uint32_t len)
{
    if ((fmt == NULL) || (len == 0)) {
        cmd_printf(NULL, "%s() paramter error!! len:%d \n", __func__, len);
        return 0;
    }
    hal_trace_output((const unsigned char *)fmt, len);
    return 1;
}
static int cp_trace_hook(const char *tag, const char *fmt, uint32_t len)
{
    if ((fmt == NULL) || (len == 0)) {
        cmd_printf(NULL, "%s() paramter error!! len:%d \n", __func__, len);
        return 0;
    }
    REL_LOG_RAW_OUTPUT((const unsigned char *)fmt, len);
    return 1;
}
static int do_trace_hook(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    cmd_printf(handler, "%s() argc: %d \n",__func__,argc);
    if (argc >= 2) {
        if (strcmp(argv[1], "a7") == 0) {
#ifdef __A7_DSP_ENABLE__
            hal_trace_register_hook(a7_trace_hook);
#endif
        } else if (strcmp(argv[1], "cp") == 0) {
#ifdef CP_BOOT
            hal_trace_register_cp_hook(cp_trace_hook);
#endif
        }
        RESP_OK(handler);
    }
    return 0;
}

#ifdef __NET_TELNET_SERVER_SUPPORT__
static int do_telnet_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    telnetserver_start();
}

static int do_telnet_print(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    TelnetServer_t *telnet_handler = (TelnetServer_t*)handler;
    if (telnet_handler) {
        TelnetRedirectPritHanlder(telnet_handler, telnet_handler->index);
    }
    return 0;
}
#endif

#ifdef DLNA_SUPPORT
extern void dlna_run(int is_master);
extern void dlna_stop();
extern void set_porting_socket_interface_default();
static void dlna_test() {

}
static int do_dlna(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 2) {
        TRACE(0, "param err");
        return 0;
    }
    set_porting_socket_interface_default();
    int type = atoi(argv[1]);
    switch(type) {
        case 0: dlna_stop(); break;
        case 1: dlna_run(0); break; // slave
        case 2: dlna_run(1); break; // master
        case 3: dlna_test(); break;
        default:
            TRACE(0, "cmd unsupport  %d", type);
            break;
    }
    return 0;
}
#endif

#ifdef AIRPLAY_SUPPORT
extern void test_shairport();
extern void airplay_run(int is_master);
static int do_airplay(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 2) {
        TRACE(0, "param err");
        return 0;
    }
    int type = atoi(argv[1]);
    switch(type) {
        case 0: break;
        case 1: test_shairport(); break; 
        case 2: airplay_run(1); break; 
        default:
            TRACE(0, "cmd unsupport  %d", type);
            break;
    }
    return 0;
}
#endif

#ifdef CMSIS_API_TEST
void cmsis_api_os2_test_start(void)
{
    CmsisTaskFuncTestSuite();

    CmsisTaskPriFuncTestSuite();

    CmsisTimerFuncTestSuite();

    CmsisSemFuncTestSuite();

    CmsisMutexFuncTestSuite();

    CmsisMsgFuncTestSuite();

    CmsisEventFuncTestSuite();
}

static int cmsis_api_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 2) {
        TRACE(0, "param err");
        return 0;
    }

    int type = atoi(argv[1]);
    switch(type) {
        case 1: 
            cmsis_api_os2_test_start();
            break;

        default:
            TRACE(0, "cmd unsupport  %d", type);
            break;
    }

    return 0;
}
#endif

#ifdef AT_KEY_TEST
static void key_io_irq_handler(enum HAL_GPIO_PIN_T pin)
{
    printf("%s key(%d) pressed!\n", __func__, pin);
}

static int do_atkey(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    int edge = HAL_GPIO_IRQ_POLARITY_HIGH_RISING;
    struct HAL_IOMUX_PIN_FUNCTION_MAP cfg_io;

    if (argc < 2) {
        TRACE(0, "param err");
        return -1;
    }
    int keyio = atoi(argv[1]);
    if (argc > 2) {
        edge = atoi(argv[2]);
    }
    if (keyio > HAL_GPIO_PIN_LED_NUM) {
        TRACE(0, "%s key keyio:%d error", __func__, keyio);
        return -1;
    }

    TRACE(0, "%s keyio:%d edge:%d", __func__, keyio, edge);
    cfg_io.pin = keyio;
    cfg_io.pull_sel = HAL_IOMUX_PIN_PULLUP_ENABLE;
    cfg_io.volt = HAL_IOMUX_PIN_VOLTAGE_VIO;
    cfg_io.function = HAL_IOMUX_FUNC_AS_GPIO;
    hal_iomux_init(&cfg_io, sizeof(cfg_io));
    hal_gpio_pin_set_dir(keyio, HAL_GPIO_DIR_IN, 1);    //设置gpio输入

    struct HAL_GPIO_IRQ_CFG_T irq_cfg;
    irq_cfg.irq_debounce = 1;
    irq_cfg.irq_enable = 1;
    irq_cfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
    irq_cfg.irq_handler = key_io_irq_handler;
    irq_cfg.irq_polarity = edge;
    hal_gpio_setup_irq(keyio, &irq_cfg);
    return 0;
}
#endif

static int do_test_rf_cmd(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint8_t* rsp_buf = (uint8_t*)malloc(1024);
    memset(rsp_buf, 0, 1024);
    cmd_printf(handler, "%s() argc: %d \n",__func__,argc);
    if (argc < 1) {
        TRACE(0, "###argc < 1");
        return -1;
    }

    if (!rsp_buf) {
        TRACE(0, "### !rsp_buf");
        return -1;
    }

    bwifi_str_cmd(0, (uint8_t *)argv[1], rsp_buf, 1024);

#ifdef __RF_TRACE_BY_SPRINTF__
    if (rsp_buf)
        cmd_printf(handler,"###AT GET RF TRACE: %s", rsp_buf);
#endif
    //RESP_OK(handler);
    free(rsp_buf);
    return 0;
}

#ifndef CHIP_BEST2002
static void at_wifi_rx_mem_dump_test(uint32_t len, uint32_t threshold, uint32_t trigger)
{
    uint32_t val;
    uint8_t flag = 1;
    uint32_t ori_cfg;
    uint32_t size;

    ori_cfg = HW_READ_REG(0x4000008c);
    val = ori_cfg | (0x3 <<22); //config sram4,5 can be accessed, not A7
    HW_WRITE_REG(0x4000008c, val);

    val = HW_READ_REG(0x9000804c);
    val &= ~0x1fff98; // clear bit4~3, bit12~7, bit20~13
    if (trigger) {
        val |= (0x2 << 3);
        val |= (32 << 7);
        val |= ((threshold & 0xFF) << 13);
    }
    HW_WRITE_REG(0x9000804c, val);

    do {
        uint32_t* rxdes = (uint32_t*)0x20180000;
        memset(rxdes, 0x00,(len & 0xfffff)*4);

        // 1. config sram4,5 config for mem_dump test
        val = HW_READ_REG(0x4000008c);
        val |= (0x3 <<26);
        HW_WRITE_REG(0x4000008c, val);

        // 5. config sram4,5 config for mem_dump test
        //val = 1; // bit0, from memry to Tx path
        val = (0x2 <<1); //bit3:1 mem config: 0: no rx notx; 1: all for tx; 2: all rx; 3: mem0 rx, mem1 tx
        HW_WRITE_REG(0x90008040, val);

        //mem rx
        // 3. config rx prescale
        /*
            cfg_prescale_1us       bit7~0
            cfg_wait_sync_us		bit13~8
            cfg_wait_crc_us		bit19~14
        */
        if (flag !=0) {
            val = 0x8209f;
        } else {
            val = 0;
        }
        HW_WRITE_REG(0x90008050, val);

        // 4. config rx start adr bit16:0
        val = 0; // start
        HW_WRITE_REG(0x90008054, val);
        // 5. config rx end adr bit16:0
        val = val + len; // start 0x00
        HW_WRITE_REG(0x90008058, val);
        //rx en

        val = HW_READ_REG(0x9000804c);
        val |= 1; // cfg_handle_rx_enable
        HW_WRITE_REG(0x9000804c, val);
        osDelay((10));

        uint16 rx_timeout = 0;
        val = HW_READ_REG(0x90008060);
        while ((val & (0xf <<4)) !=0 ) {
            osDelay((2));
            val = HW_READ_REG(0x90008060);
            rx_timeout++;
            if (rx_timeout > 1000) {
                TRACE(1, "%s Line%d error:rx timeout", __FUNCTION__, __LINE__);
                break;
            }
        }

        TRACE(1,"cfg_mem_dump, dump done!" );
        //stop rx
        val = HW_READ_REG(0x9000804c);
        val &=~(0x01);
        HW_WRITE_REG(0x9000804c, val);

        // 1. config sram4,5 config for sram
        val = HW_READ_REG(0x4000008c);
        val &= ~(0x3 <<26);
        HW_WRITE_REG(0x4000008c, val);

        TRACE(1,"cfg_mem_dump, des addr 0x%08x", (uint32_t)rxdes);
        // read actual dump size
        size = (HW_READ_REG(0x90008068) & 0x1FFFF);
        TRACE(1,"buf len %d, dump size %d (words)", len, size);
        for (int m=0; m<(size/256); m++) {
            for (int i=0; i<16; i++) {
                DUMP32("%08x ",rxdes,16);
                rxdes +=16;
            }
            osDelay((200));
        }
        size %= 256;
        for (int i=0; i<(size/16); i++) {
            DUMP32("%08x ",rxdes,16);
            rxdes +=16;
        }
        size %= 16;
        DUMP32("%08x ",rxdes,size);
        TRACE(1,"dump done !" );
    } while(0);

    // recover sram config
    HW_WRITE_REG(0x4000008c, ori_cfg);
}

#if defined(__SNIFFER_MODE__)
static int do_wifi_rf_rx_dump(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    uint32_t period = 0x1000;
    uint32_t threshold = 32;
    uint32_t trigger = 0;
    if (argc < 2) {
        TRACE(0, "param err");
        return 0;
    }

    int channel = atoi(argv[1]);
    if (argc > 2)
        period = atoi(argv[2]);
    if (argc > 3)
        threshold = atoi(argv[3]);
    if (argc > 4)
        trigger = atoi(argv[4]);

    bes_sniffer_start(NULL);
    bes_sniffer_set_filter(0, 0, 0, 0, 0, 0, 0);
    bes_sniffer_set_channel(channel);

    at_wifi_rx_mem_dump_test(period, threshold, trigger);
    bes_sniffer_stop();
    return 0;
}
#endif
#endif

#if RF_EFUSE_TEST
static int do_rf_efuse_calib_power(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{

    if (argc < 3) {
        TRACE(0, "param err");
        return 0;
    }
    int ch = atoi(argv[1]);
    int powerlevel = atoi(argv[2]);
    TRACE(0," powerlevel = 0x%4x", powerlevel);
    factory_set_power_into_efuse(powerlevel, ch);
    return 0;
}

static int do_rf_efuse_set_unit_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{

    if (argc < 3) {
        TRACE(0, "param err");
        return 0;
    }
    uint16_t param = atoi(argv[1]);
    uint16_t freqcalib = atoi(argv[2]);
    factory_set_efuse_unit_test(param, freqcalib);
    return 0;
}

static int do_rf_efuse_get_unit_test(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{

    if (argc < 2) {
        TRACE(0, "param err");
        return 0;
    }
    uint8_t clear_flags = atoi(argv[1]);
    factory_get_efuse_unit_test(clear_flags);
    return 0;
}
#endif

int do_rf_set_tpc_power_table(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 3) {
        TRACE(0, "param err");
        return 0;
    }

    uint16_t index = atoi(argv[1]);
    if (index == 2) {
        factory_section_select_power_table_for_tpc(index);
    } else {
        factory_section_select_power_table_for_tpc(1);
    }

    return 0;
}

int do_rf_dump_power_reg_list(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    volatile uint32_t* power_reg_base = (volatile uint32_t*)0x9013801c;
    int i = 0;
    int end = 0;

    if (argc < 3) {
        end = 11;//9013801c ~ 90138044
    } else {
        end = atoi(argv[1]);
    }

    for (i=0 ; i < end; i++) {
        TRACE(0,"%p=0x%4x",power_reg_base, *power_reg_base);
        power_reg_base += 1;
    }
    return 0;
}

int do_wifi_power_on(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
    if (argc < 2) {
        TRACE(0, "param err");
        return 0;
    }
    int onoff = atoi(argv[1]);
    bwifi_power_on(onoff);
    return 0;
}

static int at_bt_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  int ret = 0;
  unsigned char maddr[6] = {0};
  if (argc < 2 || strcmp(argv[1], "?") == 0)
  {
    const uint8_t *mac = factory_section_get_bt_address();
    if (mac)
    {
      TRACE(0, "get bt mac from factory: " MACSTR "\n", MAC2STR(mac));
      RESP_OK(handler);
    }
    else
    {
      TRACE(0, "get bt mac from factory failed: ");
      RESP_ERROR(handle, -1);
    }
    return 0;
  }

  if (strlen(argv[1]) == 17)
  {
    str2mac(argv[1], maddr);
    if (!bt_is_valid_mac(maddr))
    {
      TRACE(0, "The mac addr is invalid");
      return -1;
    }
    ret = factory_section_set_bt_address(maddr);
    if (ret != 0)
    {
      TRACE(0, "Notice set bt mac addrress fail(%d),please check!!!", ret);
    }
    else
    {
      RESP_OK(handler);
    }
  }
  return 0;
}

static int at_bt_name(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  int ret = 0;
  unsigned char maddr[6] = {0};
  if (argc < 2 || strcmp(argv[1], "?") == 0)
  {
    const uint8_t *name = factory_section_get_bt_name();
    if (name)
    {
      TRACE(0, "get bt name from factory: %s", name);
      RESP_OK(handler);
    }
    else
    {
      TRACE(0, "get bt name from factory failed");
      RESP_ERROR(handle, -1);
    }
    return 0;
  }
  char *name = argv[1];
  if (strlen(name) == 0)
  {
      TRACE(0, "bt name is invalid");
      return -1;
  }
  ret = factory_section_set_bt_name(name, strlen(name)+1);
  if (ret != 0)
  {
    TRACE(0, "Notice set bt name fail(%d),please check!!!", ret);
  }
  else
  if(app_is_stack_ready()){
    ret=bt_set_local_dev_name((const unsigned char *)name, strlen(name)+1);
    ret = bt_stack_config((const unsigned char *)name, strlen(name)+1);
    //btif_update_bt_name((const unsigned char *)name, strlen(name)+1);
    TRACE(0, "%s: bt_stack_config(%s) returns %d", __func__, name, ret);
    RESP_OK(handler);
  }

  return 0;
}


static int at_ble_mac(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  int ret = 0;
  unsigned char maddr[6] = {0};
  if (argc < 2 || strcmp(argv[1], "?") == 0)
  {
    const uint8_t *mac = factory_section_get_ble_address();
    if (mac)
    {
      TRACE(0, "get ble mac from factory: " MACSTR "\n", MAC2STR(mac));
      RESP_OK(handler);
    }
    else
    {
      TRACE(0, "get ble mac from factory failed: ");
      RESP_ERROR(handle, -1);
    }
    return 0;
  }

  if (strlen(argv[1]) == 17)
  {
    str2mac(argv[1], maddr);
    if (!bt_is_valid_mac(maddr))
    {
      TRACE(0, "The mac addr is invalid");
      return -1;
    }
    ret = factory_section_set_ble_address(maddr);
    if (ret != 0)
    {
      TRACE(0, "Notice set ble mac addrress fail(%d),please check!!!", ret);
    }
    else
    {
      RESP_OK(handler);
    }
  }
  return 0;
}

#include "apps.h"
static int at_prompt_play(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if(argc > 2){
    RESP_ERROR(handle, -1);
    return 0;
  }
  APP_STATUS_INDICATION_T status = APP_STATUS_INDICATION_POWERON;
  if(argc > 1)status = atoi(argv[1]);
  int ret = app_voice_report(status, 0);
  TRACE(0, "app_voice_report: returns %d", ret);

  return 0;
}
#if defined(LFS_FLASH_TOTAL_SIZE) && defined(LFS_FLASH_BASE_ADDR)
#if defined(LITTLEFS_PARTITION_SUPPORT)
#include <sys/dirent.h>
// #include "lfs.h"
int at_fs_list(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if(argc > 2){
    RESP_ERROR(handle, -1);
    return 0;
  }
  char *path = "/";
  if(argc > 1 && argv[1] && strlen(argv[1]) > 0){
    path = argv[1];
  }
  DIR * dir = opendir(path);
  if(dir == NULL){
    TRACE(0, "%s: open %s failed", __func__, path);
    return -1;
  }
  while(true){
    struct dirent *ent = readdir(dir);
    //TRACE(0, "readdir returns %d", ret);
    if(ent == NULL)break;
    if (ent->d_type == DT_REG)
        TRACE(0, "    [F] %s        %d", ent->d_name, ent->d_size);
    else if (ent->d_type == DT_DIR)
        TRACE(0, "    [D] %s", ent->d_name);
    else
        TRACE(0, "    [E] %s        %d", ent->d_name, ent->d_size);
    free(ent);
  }
  closedir(dir);
#if 0
  //test for fseek/fgets: /test.txt
  char fname[64];
  strcpy(fname, path);
  if(fname[strlen(fname)-1] != '/')strcat(fname, "/test.txt");
  else strcat(fname, "test.txt");
  FILE *fp = fopen(fname, "r");
  TRACE(0, "fopen(%s): %p", fname, fp);
  if(fp){
    char line[32]={0,};
    char *p = fgets(line, sizeof(line), fp);
    TRACE(0, "fgets returns %p: %s", p, line);
    int ret = fseek(fp, 5, SEEK_SET);
    TRACE(0, "fseek returns %d", ret);
    p = fgets(line, sizeof(line), fp);
    TRACE(0, "fgets returns %p: %s", p, line);
    fclose(fp);
  }
#endif
  return 0;
}
#else
#error
    //services/wifi_app/littlefs/lfs_file_ops.h
#include "lfs_file_ops.h"
static int at_fs_list(cmd_tbl_t *cmd, int argc, char *argv[], void *handler)
{
  if(argc > 2){
    RESP_ERROR(handle, -1);
    return 0;
  }
  char *path = "/";
  if(argc > 1 && argv[1] && strlen(argv[1]) > 0){
    path = argv[1];
  }
  lfs_dir_t * dir = lfs_file_ops_opendir(path);
  if(dir == NULL){
    TRACE(0, "%s: open %s failed", __func__, path);
    return -1;
  }
  while(true){
    struct lfs_info info;
    int ret = lfs_file_ops_readdir(dir, &info);
    //TRACE(0, "readdir returns %d", ret);
    if(ret <= 0)break;
    if (info.type == LFS_TYPE_REG)
        TRACE(0, "    [F] %s        %d", info.name, info.size);
    else if (info.type == LFS_TYPE_DIR)
        TRACE(0, "    [D] %s        %d", info.name, info.size);
    else
        TRACE(0, "    [E] %s        %d", info.name, info.size);
  }
  lfs_file_ops_closedir(dir);
  return 0;
}
#endif //defined(LITTLEFS_PARTITION_SUPPORT)

#endif //defined(LFS_FLASH_TOTAL_SIZE) && defined(LFS_FLASH_BASE_ADDR)

#ifdef MCU_TFLM_ENABLE
extern int tf_main(int argc, char** argv);

static int do_tflm(cmd_tbl_t *cmd, int argc, char *argv[], void *handler) {
	uint32_t start_ticks = hal_sys_timer_get();

	tf_main(0,0);
	uint32_t endtick = hal_sys_timer_get();
	uint32_t costtime = TICKS_TO_MS(endtick - start_ticks);
	
	printf("%s %d start:%u, end:%u costtime:%ums freq is %d\n", __FUNCTION__, __LINE__, start_ticks, endtick, costtime, hal_cmu_sys_get_freq());


return 0;
}

#endif

void add_cmd()
{
    int i, j;
    cmd_tbl_t cmd_list[] = {
        /*Basic CMD*/
        {
            "AT+HELP",          1,   do_help,
            "AT+HELP                - show AT CMD list\n"
        },
        {
             "AT+RF",          4,   do_test_rf_cmd,
             "AT+RF                - do rf test\n"
        },
        {
             "AT+GPWR",          4,   do_rf_dump_power_reg_list,
             "AT+GPWR                - do_rf_dump_power_reg_list\n"
        },
        {
             "AT+TPC",          3,   do_rf_set_tpc_power_table,
             "AT+TPC                - select power table 2-new table,other-default\n"
        },
#if RF_EFUSE_TEST
        {
             "AT+EFS_PWR",       3,   do_rf_efuse_calib_power,
             "AT+EFS_PWR                - do_rf_efuse_calib_power\n"
        },
        {
             "AT+EFS_SET_TEST",       3,   do_rf_efuse_set_unit_test,
             "AT+EFS_SET_TEST                - do_rf_efuse_set_unit_test\n"
        },
        {
             "AT+EFS_GET_TEST",       3,   do_rf_efuse_get_unit_test,
             "AT+EFS_GET_TEST                - do_rf_efuse_get_unit_test\n"
        },
#endif
#if !defined(CHIP_BEST2002) && defined(__SNIFFER_MODE__)
        {
             "AT+WIFI_DUMP",          5,   do_wifi_rf_rx_dump,
             "AT+WIFI_DUMP                - do wifi dump test\n"
        },
#endif
        {
            "AT+MSLEEP",        2,  do_msleep,
            "AT+MSLEEP          - sleep for ms\n"
        },
        {
            "AT+RFTEST",        2,  do_rf_test,
            "AT+RFTEST          - do rf test\n"
        },
#if 1
        {
            "AT+MD",           8,  do_mem_read,
            "AT+MD                 - read mem or reg\n"
        },
        {
            "AT+MW",           3,  do_mem_write,
            "AT+MW                 - write mem or reg\n"
        },
        {
            "AT+I2C",           3,  do_uart_i2c_switch,
            "AT+I2C                 - do_uart_i2c_switch\n"
        },
#endif
        /*WIFI CMD*/
        {
            "AT+WSACONF",       6,   do_add_conf,
            "AT+WSACONF         - add AP config\n"
        },
        {
            "AT+WSMCONF",       4,   do_set_conf,
            "AT+WSMCONF         - modify specific AP config item\n"
        },
        {
            "AT+WSSCONF",       6,   do_save_conf,
            "AT+WSSCONF         - save AP config\n"
        },
        {
            "AT+WSDCONF",       6,   do_del_conf,
            "AT+WSDCONF         - del AP config\n"
        },
        {
            "AT+WSGCCONF",      2,   do_get_current_conf,
            "AT+WSGCCONF            - get current AP config\n"
        },
        {
            "AT+WSGSCONF",      2,   do_get_saved_conf,
            "AT+WSGSCONF            - get saved AP config\n"
        },
        {
            "AT+WSSCAN",        4,   do_scan,
            "AT+WSSCAN          - scan AP\n"
        },
        {
            "AT+WSCONN",        7,   do_conn,
            "AT+WSCONN          - start wifi connect\n"
        },
        {
            "AT+WSACONN",       1,   do_auto_conn,
            "AT+WSACONN         - start wifi auto connect\n"
        },
        {
            "AT+WSNICONN",       2,   do_netid_conn,
            "AT+WSNICONN         - start wifi connect to specific network id\n"
        },
        {
            "AT+WSGSTA",        2,   do_get_connected_state,
            "AT+WSGSTA          - get connect state\n"
        },
        {
            "AT+WSDISCONN",     1,   do_disconn,
            "AT+WSDISCONN       - disconnect AP\n"
        },
        {
            "AT+WSSETRECON",    2,   do_set_reconnect_policy,
            "AT+WSSETRECON      - set reconnect policy\n"
        },
        {
            "AT+WSAIRKISS",     1,   do_airkiss,
            "AT+WSAIRKISS       - start airkiss\n"
        },
        {
            "AT+WSGLINKSIG",    2,   do_get_link_signal,
            "AT+WSGLINKSIG      - get current link signal (RSSI)\n"
        },
        {
            "AT+WSSETRATE",     2,   do_set_fix_rate,
            "AT+WSSETRATE       - set fix rate\n"
        },
#ifdef ENABLE_FW_KEEP_ALIVE
        {
            "AT+KLVCFG", 5,   do_set_keep_alive,
            "AT+KLVCFG       - set ip keep alive's parameters;\n"
        },
        {
            "AT+KLVEN",  2,   do_enable_keep_alive,
            "AT+KLVEN       - enable ip keep alive function;\n"
        },
#endif
#ifdef __AP_MODE__
        /*SOFTAP CMD*/
        {
            "AT+WASTART",       6,   do_start_ap,
            "AT+WASTART         - do start softap\n"
        },
        {
            "AT+WASTOP",        1,   do_stop_ap,
            "AT+WASTOP          - do stop softap\n"
        },
        {
            "AT+WAGSTA",        2,   do_get_sta_info,
            "AT+WAGSTA          - get current connected sta info\n"
        },
#endif
        {
            "AT+WPCH",          3,   do_set_ch,
            "AT+WPCH            - set operating channel\n"
        },
        {
            "AT+WPON",          2,   do_wifi_power_on,
            "AT+WPON            - wifi power on/off\n"
        },
#if LWIP_SUPPORT
        /*NET CMD*/
        {
            "AT+WSSETIP",       6,   do_set_static_ip,
            "AT+WSSETIP         - set static ip addr\n"
        },
        {
            "AT+NDNS",          2,   do_ndns,
            "AT+NDNS            - do dns\n"
        },
        {
            "AT+NTCPSER",       1,   do_tcp_server,
            "AT+NTCPSER         - start tcp server\n"
        },
        {
            "AT+NUDPSER",       1,   do_udp_server,
            "AT+NUDPSER         - start udp server\n"
        },
        {
            "AT+NTCPCLI",       4,   do_tcp_client,
            "AT+NTCPCLI         - start tcp client\n"
        },
        {
            "AT+NUDPCLI",       3,   do_udp_client,
            "AT+NUDPCLI         - start udp client\n"
        },
        {
            "AT+ifconfig",      5,   do_ifconfig,
            "AT+ifconfig        - show or config netif\n"
        },
#ifdef NET_WIFI_SOCKET_SUPPORT
        {
            "AT+NWIFISOCKET",     8,  do_wifi_socket,
            "AT+NWIFISOCKET       - open wifi socket\n"
        },
        {
            "AT+NSRECORD",     3,  do_srecord,
            "AT+NSRECORD       - open socket record\n"
        },
#endif
#endif
#ifdef WIFI_IPERF_SUPPORT
        {
            "AT+NIPERF",        12,  do_iperf,
            "AT+NIPERF          - start iperf test\n"
        },
#endif
#ifdef NET_PING_SUPPORT
        {
            "AT+NPING",         6,   do_nping,
            "AT+NPING           - do ping\n"
        },
#endif
#ifdef __SNIFFER_MODE__
        /*SNIFFR CMD*/
        {
            "AT+SNSTART",         8,   do_sniffer_start,
            "AT+SNSTART           - start sniffer\n"
        },
        {
            "AT+SNSETCH",         2,   do_sniffer_set_channel,
            "AT+SNSETCH           - set sniffer channel\n"
        },
        {
            "AT+SNSTOP",          1,   do_sniffer_stop,
            "AT+SNSTOP           - stop sniffer\n"
        },
        {
            "AT+SNSEND",          2,   do_send_mlme,
            "AT+SNSEND           - send management frame\n"
        },
#endif
        /*factory CMD*/
#ifdef CHIP_BEST2000
        {
            "AT+SETCALI",         5,   do_set_calibration_data,
            "AT+SETCALI           - save calibration data(id, freq_cal, iq_cal, iq_offset)\n"
        },
        {
            "AT+GETCALI",         2,   do_get_calibration_data,
            "AT+GETCALI           - get calibration data\n"
        },
#endif
        {
            "AT+RBNVREC",        1,   do_nvrecord_rebuild,
            "AT+RBNVREC         - rebuild nvrecord\n"
        },
        {
            "AT+FREEHEAPSIZE",    1,   show_free_heap_size,
            "AT+FREEHEAPSIZE         - show RTOS/application free heap size\n"
        },
        {
            "AT+THREAD",  1,   show_thread_info,
            "AT+THREAD         - show thread information\n"
        },
        {
            "AT+HEAP",  1,   show_thread_heapsummary,
            "AT+HEAP       - show thread heap malloc-free summary\n"
        },
        {
            "AT+SHOOTTSK",  2,   set_shoottsk,
            "AT+SHOOTTSK=tsk_name       - trace dedicate thread heap malloc-free\n"
        },
        {
            "AT+RUNTIME",  1,   show_Runtime_Stat,
            "AT+RUNTIME         - show run time statistics\n"
        },
        {
            "AT+SETGPIO",         3,   set_GPIO_output,
            "AT+SETGPIO                 - set gpio output low/high\n"
        },
        {
            "AT+DEBUGSYS",        1,   show_system_info,
            "AT+DEBUGSYS             - show system info [heap,task]\n"
        },
#if BES_OTA_ENABLED
        {
            "AT+OTA",             7,    do_ota_action,
            "AT+OTA              - set/get OTA information\n"
        },
#endif
        {
            "AT+ECHO",        2,     set_at_cmd_echo,
            "AT+ECHO                -set at cmd echo disable/enable\n"
        },
#ifdef CHIP_BEST2000
        {
            "AT+POWER",        2,   do_set_rf_power,
            "AT+POWER         - rebuild nvrecord\n"
        },
#endif
        {
            "AT+FCC",        2,   do_freq_cal_cfg,
            "AT+FCC=\"FED2\"    - config freq cal cfg\n"
        },
        {
            "AT+EPTA",       4,   do_set_epta_params,
            "AT+EPTA        - config epta params(WiFi/bt)\n"
        },
#if defined(__NET_FTP_CLIENT_SUPPORT__) && defined(__A7_DSP_ENABLE__)
        {
            "AT+RECDUMP",    10,   do_record_to_ftp,
            "AT+RECDUMP       - mic record data to ftp server\n"
        },
#if defined(__A7_DSP_ENABLE__)
        {
            "AT+A7DUMP",     10,   do_a7_dump_ftp,
            "AT+A7DUMP        - a7 data to ftp server\n"
        },
#endif
        {
            "AT+GETFILE",    7,    do_ftp_get_file,
            "AT+GETFILE      -get file from server\n"
        },
#ifdef PCM_AUDIO_INSTANCE_ENABLE
        {
            "AT+PCM",     10,   do_pcmaudio_dump_ftp,
            "AT+PCM        - pcm data to ftp server\n"
        },
#endif
#endif
        {
            "AT+i2s",   2,   do_test_iomux_I2S_to_gpio,
            "AT+i2s        - i2s\n"
        },

#if 0
#ifdef VOWLAN_SUPPORT_INPUT_BY_USB
        {
            "AT+USBAUDIO",   2,   do_vow_usb_audio_input,
            "AT+USBAUDIO        - type-c audio input\n"
        },
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_ADC
        {
            "AT+LINEIN",   3,   do_vow_linein_audio_input,
            "AT+LINEIN        - line in audio input\n"
        },
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_BT
        {
            "AT+BTAUDIO",   3,   do_vow_bt_audio_input,
            "AT+BTAUDIO        - bt audio input\n"
        },
#endif

#ifdef VOWLAN_SUPPORT_INPUT_BY_I2S
        {
            "AT+I2SAUDIO",   3,   do_vow_i2s_audio_input,
            "AT+I2SAUDIO        - i2s audio input\n"
        },
#endif

#if defined(VOWLAN_SUPPORT_INPUT_BY_I2S) || defined(VOWLAN_SUPPORT_OUTPUT_BY_I2S)
        {
            "AT+I2S_ROLE",   2,   do_vow_i2s_audio_role,
            "AT+I2S_ROLE        - AT+I2S_ROLE=master/slave\n"
        },
#endif

#ifdef VOWLAN_SUPPORT_MULTI_AUDIO_PROCESS
        {
            "AT+PREV_VLM",   2,   do_vow_cfg_set_pre_voluem,
            "AT+PREV_VLM        - AT+PREV_VLM=50, 0-100\n"
        },

        {
            "AT+I2S_CH",   2,   do_vow_cfg_set_i2s_output_channel,
            "AT+I2S_CH        - AT+I2S_CH=R [R-RIGHT, L=LEFT]\n"
        },

        {
            "AT+DAC_CH",   2,   do_vow_cfg_set_dac_output_channel,
            "AT+DAC_CH        - AT+DAC_CH=R [R-RIGHT, L=LEFT]\n"
        },

        {
            "AT+DAC_OUT_VLM",   2,   do_vow_cfg_set_dac_output_volume,
            "AT+DAC_OUT_VLM        - AT+DAC_OUT_VLM=50, 0-100\n"
        },

        {
            "AT+I2S_OUT_VLM",   2,   do_vow_cfg_set_i2s_output_volume,
            "AT+I2S_OUT_VLM        - AT+I2S_OUT_VLM=50, 0-100\n"
        },

#endif //VOWLAN_SUPPORT_MULTI_AUDIO_PROCESS
        {
            "AT+GSLAVEMAC",          2,   do_get_slave_mac,
            "AT+GSLAVEMAC           - get slave mac addr data\n"
        },
        {
            "AT+SSLAVEMAC",          3,   do_set_slave_mac,
            "AT+SSLAVEMAC           - set slave mac addr data\n"
        },
        {
            "AT+DSLAVEMAC",       2,   do_del_slave_mac,
            "AT+DSLAVEMAC          - del slave mac addr data\n"
        },
        {
            "AT+SMASTERSSID",       2,   do_set_master_ssid,
            "AT+SMASTERSSID          - set ssid param into flash for slave\n"
        },
        {
            "AT+GMASTERSSID",      2,   do_get_master_ssid,
            "AT+GMASTERSSID       - get ssid from flash\n"
        },
        {
            "AT+SETMAC",    2,   do_set_device_mac_to_factory,
            "AT+SETMAC      - set mac address to factory\n"
        },
        {
            "AT+GETMAC",    2,   do_get_device_mac_from_factory,
            "AT+GETMAC      - Get mac address from factory\n"
        },
#endif //VOWLAN_SUPPORT

#ifdef IR_FUNCTION_TEST
        {
            "AT+IR",   12,   do_ir_functions,
            "AT+IR        ir functions\n"
        },
#endif
        {
            "AT+JTAG",    2,   do_set_jtag_mode,
            "AT+JTAG        - change to jtag mode, AT+JTAG=enable(main),AT+JTAG=A7/CP\n"
        },
#ifdef BLE_ENABLE
        {
            "AT+BLE",   3,   ble_adv_enable,
            "AT+BLE        ble adv open/close test\n"
        },
        {
            "AT+BLEADV",   3,   ble_adv_test_case,
            "AT+BLEADV        ble adv open/close test\n"
        },
        {
            "AT+BLESCAN",   4,   ble_scan_enable,
            "AT+BLESCAN        ble scan open/close test\n"
        },
        {
            "AT+BLECONN",   4,   at_ble_start_connect,
            "AT+BLECONN        ble start connect/cancel test\n"
        },
#ifdef BLE_GATT_CLIENT_TEST
        {
            "AT+BLEGATTC",   6,   do_gatt_client_test,
            "AT+BLEGATTC        ble gatt client test\n"
        },
#endif
#if  __BLE_WIFICONFIG__
        {
            "AT+WIFICONFIG",   2,   ble_wificonfig,
            "AT+WIFICONFIG       ble wificonfig 0/1/2 test\n"
        },
#endif
#endif

#if !defined(__BT_CLOSE__)
        {
            "AT+BTSCAN",   2,   bt_scan_enable,
            "AT+BTSCAN        bt scan open/close \n"
        },
        /*{
            "AT+GETBTRSSI",   1,   get_bt_rssi_value,
            "AT+GETBTRSSI        get bt rssi \n"
        },*/
        {
            "AT+SETBTMAC",    2,   do_set_bt_mac_to_factory,
            "AT+SETBTMAC      - set bt mac address to factory\n"
        },
        {
            "AT+SETBLEMAC",    2,   do_set_ble_mac_to_factory,
            "AT+SETBLEMAC      - set ble mac address to factory\n"
        },
        {
            "AT+GETBTMAC",    2,   do_get_bt_mac_from_factory,
            "AT+GETBTMAC      - Get bt mac address from factory\n"
        },
        {
            "AT+SETBTNAME",    2,   do_set_bt_name_to_factory,
            "AT+SETBTNAME      - set bt name to factory\n"
        },
#ifdef USE_BT_ADAPTER
        {
            "AT+BTINIT",   2,   bt_init,
            "AT+BTINIT        bt init power on \n"
        },
        {
            "AT+BT_AF_CTRL",  3, do_bt_af_ctrl,
            "AT+BT_AF_CTRL     - ctrl bt audioflinger. 0 stop 1 start\n",
        },
#endif
#ifdef DEBUG_SPP_LOG
        {
            "AT+BT_SPP_INIT", 1, bt_spp_init,
            "BT_SPP_INIT  - bt spp server open\n"
        },
#endif
#endif
#ifdef __WIFI_APP_SUPPORT__
        {
            "AT+SETWMAC",    2,   do_set_wifi_mac_to_factory,
            "AT+SETWMAC      - set wifi mac address to factory\n"
        },
        {
            "AT+GETWMAC",    2,   do_get_wifi_mac_from_factory,
            "AT+GETWMAC      - Get wifi mac address from factory\n"
        },
        {
            "AT+WSPS",    2,   do_set_ps_mode,
            "AT+WSPS      - Set wifi ps mode\n"
        },
#endif
#ifdef LPLAYER_SUPPORT_AT_CMD
        {
            "AT+LPLAYER_START", 2, do_lplayer_start,
            "AT+LPLAYER_START        - lplayer create and start. the arg could be a url or local file path.\n"
        },
        {
            "AT+LPLAYER_STOP", 2, do_lplayer_stop,
            "AT+LPLAYER_STOP         - lplayer stop and destory.\n"
        },
        {
            "AT+LPLAYER_PAUSE", 2, do_lplayer_pause,
            "AT+LPLAYER_PAUSE        - lplayer pause, resume could use AT+LPLAYER_START.\n"
        },
        {
            "AT+LPLAYER_VOL", 2, do_lplayer_set_vol,
            "AT+LPLAYER_VOL      - lplayer set vol. [0,16]\n"
        },
        {
            "AT+LPLAYER_SEEK", 2, do_lplayer_seek,
            "AT+LPLAYER_SEEK         - lplayer seek to specific position. unit: s\n"
        },
#ifndef BT_MEDIA_SUPPORT_LPLAYER
        {
            "AT+LPLAYER_SINGLE_LOOP", 2, do_lplayer_enable_single_loop,
            "AT+LPLAYER_SINGLE_LOOP  - lplayer enable or disable single loop.\n"
        },
#endif
        {
            "AT+LPLAYER_PLAY_TIME", 2, do_lplayer_show_current_play_time,
            "AT+LPLAYER_PLAY_TIME    - show current program play time.\n"
        },
        // {
        //     "AT+LPLAYER_SHOW_MEM", 2, do_lplayer_show_memory_usage,
        //     "AT+LPLAYER_SHOW_MEM     - show current os memory usage.\n"
        // },
        {
            "AT+LPLAYER_SINK_TYPE", 2, do_lplayer_set_global_sink_type,
            "AT+LPLAYER_SINK_TYPE    - set lplayer sink type: 0 alsa, 1 LE-audio, 2 BT-audio.\n"
        },
#endif

#ifdef __ALSA_EN__
        {
            "AT+ALSA_FTP_CFG", 5, do_alsa_dump_config_ftp_server,
            "AT+ALSA_FTP_CFG    - alsa dump ftp server config.\n"
        },
        {
            "AT+ALSA_DUMP",    3, do_alsa_dump,
            "AT+ALSA_DUMP       - alsa dump type(0~4), enable(0,1).\n"
        },
        {
            "AT+ALSA_ULTRASOUND", 3, do_alsa_ultrasound,
            "AT+ALSA_ULTRASOUND - start(1) or stop(0) ultrasound.\n"
        },
        {
            "AT+ALSA_SET_FVOL", 3, do_alsa_set_float_vol,
            "AT+ALSA_SET_FVOL   - set vol. [0~100]\n"
        },
        {
            "AT+ALSA_GET_FVOL", 3, do_alsa_get_float_vol,
            "AT+ALSA_GET_FVOL   - get float vol.\n"
        },
        {
            "AT+ALSA_LOOP_TEST", 3, do_alsa_loop_test,
            "AT+ALSA_LOOP_TEST - alsa capture loop playback test.\n"
        },
#endif // __ALSA_EN__

#ifdef USB_AUDIO_ADAPTER
        {
            "AT+USB_AUDIO", 1, do_usbaudio_test,
            "AT+USB_AUDIO usbaudio test start\n"
        },
        {
            "AT+HIDPAUSE", 2, do_hid_pause_test,
            "AT+HIDPAUSE,     do hid test\n"
        },
#endif

#ifdef __CHIP_OCD__
        {
            "AT+WATCHTEST", 2, debug_dwt_test,
            "AT+WATCHTEST   watch point test\n"
        },
        {
            "AT+WATCH", 4, at_watch_func,
            "AT+WATCH      add watch point\n"
        },
#endif
        {
            "AT+REBOOT", 2, do_reboot_func,
            "AT+REBOOT      - reboot system\n"
        },
#ifdef KERNEL_LITEOS_M
        {
            "AT+PAENABLE", 2, do_liteos_pagpio_init,
            "AT+PAENABLE,     pa enable\n"
        },
#endif
#if defined(__A7_DSP_ENABLE__)
        {
            "AT+A7CMD", 4, do_send2a7_cmd,
            "AT+A7CMD,     - show a7 info \n"
        },
        {
            "AT+A7RB", 2, do_ca7_reboot,
            "AT+A7RB      - reboot A7\n"
        },
#endif
#ifdef CP_BOOT
        {
            "AT+CPRB", 2, do_cp_reboot,
            "AT+CPRB      - reboot CP\n"
        },
#endif
#if BLE_AUDIO_ENABLED && defined(BLE_V2)
        {
            "AT+BISSRC", 2, do_start_bis_source,
            "AT+BISSRC,     - start bis source \n"
        },
        {
            "AT+BISSINK", 2, do_start_bis_sink,
            "AT+BISSINK,     - start bis sink \n"
        },
        {
            "AT+BISSRCSTOP", 2, do_stop_bis_source,
            "AT+BISSRCSTOP,     - stop bis source \n"
        },
        {
            "AT+BISSINKSTOP", 2, do_stop_bis_sink,
            "AT+BISSINKSTOP,     - stop bis sink \n"
        },
#endif
#if defined(IBRT)
        {
            "AT+IBRT", 3, do_start_ibrt,
            "AT+IBRT,     - start ibrt \n"
        },
        {
            "AT+IBRTSTOP", 3, do_stop_ibrt,
            "AT+IBRTSTOP,     - stop ibrt \n"
        },
#endif
#ifdef LINEIN_TRANS_ON

        {
            "AT+BESAUD_ONOFF", 2, do_switch_besaud_onoff,
            "AT+BESAUD_ONOFF,     - switch besaud onoff \n"
        },
#endif
        {
            "AT+PANIC", 3, panic_trigger,
            "AT+PANIC,     - start panic \n"
        },
#if !defined(__BT_CLOSE__)
        {
            "AT+VOLUP", 3, do_volumeup,
            "AT+VOLUP,     - volume up \n"
        },
        {
            "AT+VOLDOWN", 3, do_volumedown,
            "AT+VOLDOWN,     - volume down \n"
        },
#endif
#ifdef SPEECH_MSG_TEST_CASE
        {
            "AT+SPEECHMSG",  5, do_speech_msg_config,
            "AT+SPEECHMSG,   - AT+SPEECHMSG=tx,[intervel], -r / -b[option]\n"
        },
#endif
        {
            "AT+TPORT", 2, do_controller_tport,
            "AT+TPORT,     - tport \n"
        },
        {
            "AT+TRACEHOOK", 2, do_trace_hook,
            "AT+TRACEHOOK      - TEST TRACE HOOK\n"
        },
#ifdef __NET_TELNET_SERVER_SUPPORT__
        {
            "AT+TELNET", 1, do_telnet_test,
            "AT+TELNET       -TEST TELNET\n"
        },
        {
            "AT+TELNET_PRINT", 1 , do_telnet_print,
            "AT+TELNET_PRINT,  - TEST TELNET PRINT\n"
        },
#endif

#ifdef DLNA_SUPPORT
        {
            "AT+DLNA", 2, do_dlna,
            "AT+DLNA      - AT+DLNA=0 stop / 1 start\n"
        },

#endif
#ifdef AIRPLAY_SUPPORT
        {
            "AT+AIRPLAY", 2, do_airplay,
            "AT+AIRPLAY   - AT+AIRPLAY=0 stop / 1 start\n"
        },

#endif

#ifdef CMSIS_API_TEST
        {
            "AT+CMSIS", 2, cmsis_api_test,
            "AT+CMSIS      - AT+CMSIS=1 start\n"
        },
#endif

#ifdef AT_KEY_TEST
        {
            "AT+KEY", 3, do_atkey,
            "AT+KEY      - AT+KEY=<GPIO>,<fn>\n"
        },
#endif
        {
            "AT+BTMAC", 2, at_bt_mac,
            "AT+BTMAC=?     - read bt mac\nAT+BTMAC=BE:99:34:45:56:68    - set bt mac\n"
        },
        {
            "AT+BTNAME", 2, at_bt_name,
            "AT+BTNAME=?    - read bt name\nAT+BTNAME=newBtName    - set bt name\n"
        },
        {
            "AT+BLEMAC", 2, at_ble_mac,
            "AT+BLEMAC=?     - read ble mac\nAT+BLEMAC=BE:99:34:45:56:68    - set ble mac\n"
        },
#ifdef MEDIA_PLAYER_SUPPORT
        {
            "AT+PROMPT", 2, at_prompt_play,
            "AT+PROMPT[=<aud_id>]       - Play prompt audio (0 default). \n"
        },
#endif
#if defined(LFS_FLASH_TOTAL_SIZE) && defined(LFS_FLASH_BASE_ADDR)
        {
            "AT+FSLIST", 2, at_fs_list,
            "AT+FSLIST[=<path>]       - show file list (\"/\" default). \n"
        },
#endif
#ifdef MCU_TFLM_ENABLE
		{
            "AT+TFLM", 3, do_tflm,
            "AT+TFLM      - tensorflow lite micro test\n"
        }

#endif
    };

    j = sizeof(cmd_list) / sizeof(cmd_tbl_t);
    for (i = 0; i < j; i ++) {
        if (0 != console_cmd_add(&cmd_list[i])) {
            AT_PRINT("Add cmd failed\r\n");
        }
    }
}

void start_at(void)
{
    add_cmd();
	add_factory_cmd();
}

