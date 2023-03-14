/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#include "hal_trace.h"
#include <stdlib.h>
#include <string.h>
#include "wifi_device.h"
#include "wifi_hotspot_config.h"
#include "cmsis_os.h"
#include "bwifi_interface.h"
#define IFNAMSIZ IF_NAMESIZE
#include "lwip/tcpip.h"
#include "cmsis_os2.h"

#define WIFI_MAX_CONFIG_BITMAP_SIZE    (((WIFI_MAX_CONFIG_SIZE) >> 3) + (WIFI_MAX_CONFIG_SIZE % 8? 1 : 0))
#ifdef  HMOS_DEBUG
#define hmos_printf(...)                printf(__VA_ARGS__)
#else
#define hmos_printf(...)
#endif

static WifiEvent        g_staEventHandler  = {0};
static bool             _station_static_ip = false;
static osTimerId_t        _dhcp_timer_id;
static struct ip_info   _current_ip;
extern struct netif if_wifi;
static u8   _mac_addr[ETH_ALEN];

WifiErrorCode RegisterWifiEvent(WifiEvent *event);

typedef enum {
    HMOS_ON_WIFI_CONNECTION_CHANGED     = 0,
    HMOS_ON_WIFI_SCAN_STATE_CHANGED     = 1,
    HMOS_ON_HOTSPOT_STATE_CHANGED       = 2,
    HMOS_ON_HOTSPOT_STA_JOIN_CHANGED    = 3,
    HMOS_ON_HOTSPOT_STA_LEAVE_CHANGED   = 4,
    HMOS_ON_WIFI_CB_ID_MAX
} HalHmosWifiCallId;

typedef enum {
    SCAN_INIT       = 0,
    SCAN_REQUEST    = 1,
    SCAN_TRIGGER    = 2,
    SCAN_DONE       = 3,
} HalHmosScanState;

typedef union {
    int connect_info;
    struct bwifi_scan_config scan_info;
    int hotspot_state_info;
    int hotspot_sta_join;
    int hotspot_sta_leve;
} HalHmosWifiCmdInfo;

typedef union {
    struct {
        int state;
        WifiLinkedInfo info;
    } wifi_connection_changed;
    struct {
        int state;
        int size;
    } wifi_scan_sate_changed;
    struct {
        int state;
    } hotspot_state_changed;
    struct {
        StationInfo info;
    } hotspot_sta_join;
    struct {
        StationInfo info;
    } hotspot_sta_leave;
} HalHmosWifiEventInfo;


typedef struct hal_hmos_bwifi_event {
    HalHmosWifiCallId event_id;         /**< event ID */
    HalHmosWifiEventInfo event_info;     /**< event information */
} HalHmosEventInfo;

typedef struct hal_hmos_bwifi_cmd {
    HalHmosWifiCallId cmd_id;         /**< cmd ID */
    HalHmosWifiCmdInfo cmd_info;     /**< cmd information */
} HalHmosCmdInfo;

typedef struct {
    int                         scan_size;
    int                         networkId; /* connected AP save it */
    uint8_t wifi_config_map[WIFI_MAX_CONFIG_BITMAP_SIZE];
    WifiDeviceConfig wifi_config[WIFI_MAX_CONFIG_SIZE];
} HalHmosWifiConfig;

typedef struct {
    volatile HalHmosScanState   scan_state;
    uint8_t                       wifi_init;
    uint8_t                       hidden;
    uint8_t                       hmos_event_thread_init;
    osMutexId                   hal_hmos_mutex_id;
    HalHmosWifiConfig           hmos_config_info;
    WifiLinkedInfo              info;
} HalHmosWifiInfo;

static WifiEvent            *g_HalHmosEvent[WIFI_MAX_EVENT_SIZE]    = {0};
static HalHmosWifiInfo      g_HalHmosWifiInfo                       = {0};
static osThreadId           _HalHmosWifiEventThreadId               = NULL;
static osMailQId            _HalHmosWifiEventCallId                 = NULL;

osSemaphoreDef(_hal_hmos_wait_sem);
osMailQDef(HalHmosWifiCallbackQueue, 6, HalHmosCmdInfo);
osMutexDef(HalHmosWifiMutex);

static void HalHmosStaEventConnect(WIFI_USER_EVT_ID evt_id, void *arg);
static void HalHmosStaEventDisconn(WIFI_USER_EVT_ID evt_id, void *arg);
static void HalHmosApEventEnabled(WIFI_USER_EVT_ID evt_id, void *arg);
static void HalHmosApEventDisabled(WIFI_USER_EVT_ID evt_id, void *arg);
static void HalHmosApEventStaJoin(WIFI_USER_EVT_ID evt_id, void *arg);
static void HalHmosApEventStaLeave(WIFI_USER_EVT_ID evt_id, void *arg);
extern void ethernetif_input(u16_t devnum, void *p_buf, int size);
void HalHmosWifiLock(void)
{
    if (g_HalHmosWifiInfo.hal_hmos_mutex_id == NULL) {
        g_HalHmosWifiInfo.hal_hmos_mutex_id = osMutexCreate((osMutex(HalHmosWifiMutex)));
    }

    if (g_HalHmosWifiInfo.hal_hmos_mutex_id != NULL)
        osMutexWait(g_HalHmosWifiInfo.hal_hmos_mutex_id, osWaitForever);
}
void HalHmosWifiUnLock(void)
{
    if (g_HalHmosWifiInfo.hal_hmos_mutex_id != NULL)
        osMutexRelease(g_HalHmosWifiInfo.hal_hmos_mutex_id);
}

static void net_intf_status_change_cb(struct netif *netif)
{
    if (netif_is_up(netif) && !ip_addr_isany(&netif->ip_addr)) {
        uint32_t ip = ((ip4_addr_t *)&netif->ip_addr)->addr;
        uint32_t gw = ((ip4_addr_t *)&netif->gw)->addr;
        uint32_t mask = ((ip4_addr_t *)&netif->netmask)->addr;
        printf("net_intf_status_change_cb **ip = %s\n", inet_ntoa(ip));
        printf("net_intf_status_change_cb **netmask = %s\n", inet_ntoa(mask));
        printf("net_intf_status_change_cb **gw = %s\n", inet_ntoa(gw));

        struct ip_info set_ip;
        set_ip.ip = ip;
        set_ip.netmask = mask;
        set_ip.gw = gw;
        bwifi_set_ip_addr(WIFI_IF_STATION, &set_ip);
        //bwifi_change_current_status(BWIFI_STATUS_GOT_IP);
    }
}

static void WifiConnectionChangedHandler(int state, WifiLinkedInfo *info)
{
    struct netif *p_netif = &if_wifi;
    if (p_netif == NULL)
        return;
    if (state == WIFI_STATE_NOT_AVALIABLE) {
        /* clear IP address in case that dhcp_stop not do it */
        netifapi_netif_set_addr(p_netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
        dns_setserver(0, IP4_ADDR_ANY);
        netif_set_link_down(p_netif);
        netif_set_down(p_netif);
        if (_station_static_ip == false) {
            osTimerStop(_dhcp_timer_id);
            dhcp_stop(p_netif);
        }
    } else if ( state == WIFI_STATE_AVALIABLE) {
        netif_set_link_up(p_netif);
        netif_set_up(p_netif);
        if (_station_static_ip == true) {
            netifapi_netif_set_addr(p_netif, &_current_ip.ip, &_current_ip.netmask, &_current_ip.gw);
            printf("**ip = %s\n", inet_ntoa(_current_ip.ip));
            printf("**netmask = %s\n", inet_ntoa(_current_ip.netmask));
            printf("**gw = %s\n", inet_ntoa(_current_ip.gw));
        } else {
            dhcp_start(p_netif);
            osTimerStop(_dhcp_timer_id);
            osTimerStart(_dhcp_timer_id, 30 * 1000);
        }
    }
}

static void HalHmosWifiEventCall(HalHmosEventInfo *event, WifiEvent *hmos_fun_cb)
{

    if (event->event_id == HMOS_ON_WIFI_CONNECTION_CHANGED &&
        hmos_fun_cb->OnWifiConnectionChanged != NULL) {
        hmos_fun_cb->OnWifiConnectionChanged(event->event_info.wifi_connection_changed.state,
                                             &(event->event_info.wifi_connection_changed.info));
    } else if (event->event_id == HMOS_ON_WIFI_SCAN_STATE_CHANGED &&
               hmos_fun_cb->OnWifiScanStateChanged != NULL) {
        if (event->event_info.wifi_scan_sate_changed.size < 0)
            event->event_info.wifi_scan_sate_changed.size = 0;
        hmos_fun_cb->OnWifiScanStateChanged(event->event_info.wifi_scan_sate_changed.state,
                                            event->event_info.wifi_scan_sate_changed.size);
    } else if (event->event_id == HMOS_ON_HOTSPOT_STATE_CHANGED &&
               hmos_fun_cb->OnHotspotStateChanged != NULL) {
        hmos_fun_cb->OnHotspotStateChanged(event->event_info.hotspot_state_changed.state);
    } else if (event->event_id == HMOS_ON_HOTSPOT_STA_JOIN_CHANGED &&
               hmos_fun_cb->OnHotspotStaJoin != NULL) {
        hmos_fun_cb->OnHotspotStaJoin(&(event->event_info.hotspot_sta_join.info));
    } else if (event->event_id == HMOS_ON_HOTSPOT_STA_LEAVE_CHANGED &&
               hmos_fun_cb->OnHotspotStaLeave != NULL) {
        hmos_fun_cb->OnHotspotStaLeave(&(event->event_info.hotspot_sta_leave.info));
    } else {
        hmos_printf("HalHmosWifiEventThread invalid id or no cb function\n\r");
    }
}

static void HalHmosWifiEventThread(void const *argument)
{
    int i                                   = 0;
    HalHmosWifiConfig  *hmos_config_info    = &(g_HalHmosWifiInfo.hmos_config_info);
    HalHmosEventInfo event;
    printf("%s start\n", __func__);
    while (1) {
        osEvent evt = osMailGet(_HalHmosWifiEventCallId, osWaitForever);
        printf("%s, id:0x%x\n", __func__, evt.status);

        if (evt.status == osEventMail) {
            HalHmosCmdInfo *cmd = (HalHmosCmdInfo *)evt.value.p;
            memset(&event, 0, sizeof(HalHmosEventInfo));
            event.event_id = cmd->cmd_id;
            if (cmd->cmd_id == HMOS_ON_WIFI_SCAN_STATE_CHANGED) {
                event.event_info.wifi_scan_sate_changed.state = WIFI_STATE_AVALIABLE;
                g_HalHmosWifiInfo.scan_state = SCAN_TRIGGER;
                if (cmd->cmd_info.scan_info.ssids == NULL && cmd->cmd_info.scan_info.channels == NULL) {
                    hmos_config_info->scan_size = bwifi_scan();
                } else {

                    struct bwifi_ssid *ssid_info = cmd->cmd_info.scan_info.ssids;
                    struct bwifi_ssid *ssid_free_info = cmd->cmd_info.scan_info.ssids;
                    hmos_config_info->scan_size = bwifi_config_scan(&(cmd->cmd_info.scan_info));
                    /*  malloc it in the AdvanceScan */
                    while (ssid_info != NULL) {
                        ssid_info = ssid_info->next;
                        free(ssid_free_info);
                        ssid_free_info = ssid_info;
                    }

                    if (cmd->cmd_info.scan_info.channels != NULL)
                        free(cmd->cmd_info.scan_info.channels);
                }
                g_HalHmosWifiInfo.scan_state = SCAN_DONE;
                event.event_info.wifi_scan_sate_changed.size = hmos_config_info->scan_size;
            }
            for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
                if (g_HalHmosEvent[i] != NULL)
                    HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
            }
            osMailFree(_HalHmosWifiEventCallId, cmd);
        }

    }
    osThreadExit();
}
osThreadDef(HalHmosWifiEventThread, osPriorityHigh, 1, 4 * 1024, "hal_hmos_wifi_event");

WifiErrorCode HalHmosWifiEventInit(void)
{
    if (g_HalHmosWifiInfo.hmos_event_thread_init == true)
        return WIFI_SUCCESS;

    _HalHmosWifiEventCallId = osMailCreate(osMailQ(HalHmosWifiCallbackQueue), NULL);
    if (_HalHmosWifiEventCallId == NULL) {
        hmos_printf("create wifi event mail queue fail\n");
        return ERROR_WIFI_UNKNOWN;
    }

    _HalHmosWifiEventThreadId = osThreadCreate(osThread(HalHmosWifiEventThread), NULL);
    if (_HalHmosWifiEventThreadId == NULL) {
        hmos_printf("create wifi event thread fail\n");
        return ERROR_WIFI_UNKNOWN;
    }
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_CONNECTED, HalHmosStaEventConnect);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_DISCONNECTED, HalHmosStaEventDisconn);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_AP_ENABLED, HalHmosApEventEnabled);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_AP_DISABLED, HalHmosApEventDisabled);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_AP_STA_CONNECTED, HalHmosApEventStaJoin);
    bwifi_reg_user_evt_handler(WIFI_USER_EVT_AP_STA_DISCONNECTED, HalHmosApEventStaLeave);

    g_HalHmosWifiInfo.hmos_event_thread_init = true;
    return WIFI_SUCCESS;
}

static int HalHmosGetNewNetId(HalHmosWifiConfig *config)
{
    int netId = 0;
    while (netId < WIFI_MAX_CONFIG_SIZE) {
        if ((config->wifi_config_map[netId >> 3] &  (1 << (netId % 8))) == 0)
            break;
        netId++;
    }
    return netId;
}

static void HalHmosAddNetIdConfig(HalHmosWifiConfig *config, int netId, const WifiDeviceConfig *save_config)
{
    memcpy(&(config->wifi_config[netId]), save_config, sizeof(WifiDeviceConfig));
    config->wifi_config[netId].netId = netId;
    config->wifi_config_map[netId >> 3] |= (1 << (netId % 8));
    hmos_printf("%s ssid==%s netid=%d\n\r", __func__, config->wifi_config[netId].ssid, netId);
}

static void HalHmosRemoveNetId(HalHmosWifiConfig *config, int netId)
{
    config->wifi_config_map[netId >> 3] &= ( ~(1 << (netId % 8)));
}

static int HalHmosSendEvent(HalHmosWifiCallId event_id, HalHmosWifiCmdInfo *info)
{
    HalHmosCmdInfo *data;
    hmos_printf("%s, id:%d\n", __func__, event_id);

    if (event_id >= HMOS_ON_WIFI_CB_ID_MAX)
        return BWIFI_R_INVALID_ARG;
    data = (HalHmosCmdInfo *)osMailAlloc(_HalHmosWifiEventCallId, 0);

    if (data == NULL)
        return BWIFI_R_COMMON_FAIL;

    if (event_id == HMOS_ON_WIFI_SCAN_STATE_CHANGED)
        g_HalHmosWifiInfo.scan_state = SCAN_REQUEST;
    memset(data, 0, sizeof(HalHmosCmdInfo));
    data->cmd_id = event_id;
    if (info != NULL) {
        memcpy(&(data->cmd_info), info, sizeof(HalHmosWifiCmdInfo));
    }

    if (osMailPut(_HalHmosWifiEventCallId, (void *)data))
        return BWIFI_R_COMMON_FAIL;

    return BWIFI_R_OK;
}

static void HalHmosStaEventConnect(WIFI_USER_EVT_ID evt_id, void *arg)
{
    WifiLinkedInfo  *info = &(g_HalHmosWifiInfo.info);
    hmos_printf("F:%s L:%d event:%d, arg=%d\n", __func__, __LINE__, evt_id, (*(uint8_t *)arg));

    if (evt_id == WIFI_USER_EVT_CONNECTED) {
        int i = 0;
        HalHmosEventInfo event;
        event.event_id = HMOS_ON_WIFI_CONNECTION_CHANGED;

        event.event_info.wifi_connection_changed.state = WIFI_STATE_AVALIABLE;
        info->rssi = bwifi_get_current_rssi();
        info->disconnectedReason = 0;
        info->connState = WIFI_CONNECTED;
        memcpy(info->bssid, arg, WIFI_MAC_LEN);
        memcpy(&(event.event_info.wifi_connection_changed.info), info, sizeof(WifiLinkedInfo));

        hmos_printf("connected status=%d bssid=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:\n\r",
                    event.event_info.wifi_connection_changed.info.connState, info->bssid[0],
                    info->bssid[1], info->bssid[2], info->bssid[3], info->bssid[4], info->bssid[5]);

        for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
            if (g_HalHmosEvent[i] != NULL)
                HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
        }

    }
}

static void HalHmosStaEventDisconn(WIFI_USER_EVT_ID evt_id, void *arg)
{
    WifiLinkedInfo  *info = &(g_HalHmosWifiInfo.info);

    hmos_printf("F:%s L:%d event:%d,arg=%d\n", __func__, __LINE__, evt_id, (*(u16 *)arg));

    if (evt_id == WIFI_USER_EVT_DISCONNECTED) {
        int i;
        HalHmosEventInfo event;

        event.event_id = HMOS_ON_WIFI_CONNECTION_CHANGED;
        event.event_info.wifi_connection_changed.state = WIFI_STATE_AVALIABLE;
        info->connState = WIFI_DISCONNECTED;
        info->disconnectedReason = (unsigned short)(*(u16 *)arg);
        info->disconnectedReason &= 0x7fff;
        memcpy(&(event.event_info.wifi_connection_changed.info), info, sizeof(WifiLinkedInfo));
        hmos_printf("disconn status=%d reason=%d\n\r",
                    event.event_info.wifi_connection_changed.info.connState, info->disconnectedReason);

        for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
            if (g_HalHmosEvent[i] != NULL)
                HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
        }

    }
}

static void HalHmosApEventEnabled(WIFI_USER_EVT_ID evt_id, void *arg)
{
    WifiLinkedInfo  *info = &(g_HalHmosWifiInfo.info);
    hmos_printf("F:%s L:%d event:%d \n", __func__, __LINE__, evt_id);

    if (evt_id == WIFI_USER_EVT_AP_ENABLED) {
        int i;
        HalHmosEventInfo event;

        event.event_id = HMOS_ON_HOTSPOT_STATE_CHANGED;
        event.event_info.hotspot_state_changed.state = WIFI_STATE_AVALIABLE;
        memcpy(&(event.event_info.wifi_connection_changed.info), info, sizeof(WifiLinkedInfo));

        for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
            if (g_HalHmosEvent[i] != NULL)
                HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
        }

    }
}

static void HalHmosApEventDisabled(WIFI_USER_EVT_ID evt_id, void *arg)
{
    WifiLinkedInfo  *info = &(g_HalHmosWifiInfo.info);
    hmos_printf("F:%s L:%d event:%d \n", __func__, __LINE__, evt_id);

    if (evt_id == WIFI_USER_EVT_AP_DISABLED) {
        int i;
        HalHmosEventInfo event;

        event.event_id = HMOS_ON_HOTSPOT_STATE_CHANGED;
        event.event_info.hotspot_state_changed.state = WIFI_STATE_NOT_AVALIABLE;
        memcpy(&(event.event_info.wifi_connection_changed.info), info, sizeof(WifiLinkedInfo));

        for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
            if (g_HalHmosEvent[i] != NULL)
                HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
        }

    }
}

static void HalHmosApEventStaJoin(WIFI_USER_EVT_ID evt_id, void *arg)
{
    BWIFI_AP_STA_INFO_T  *info = (BWIFI_AP_STA_INFO_T *)arg;
    if (info != NULL) {
        hmos_printf("F:%s L:%d event:%d, arg=%p\n", __func__, __LINE__, evt_id, info);

        if (evt_id == WIFI_USER_EVT_AP_STA_CONNECTED) {
            int i;
            HalHmosEventInfo event;

            event.event_id = HMOS_ON_HOTSPOT_STA_JOIN_CHANGED;
            event.event_info.hotspot_sta_join.info.disconnectedReason = info->reason;
            event.event_info.hotspot_sta_join.info.disconnectedReason &= 0x7fff;
            memcpy(event.event_info.hotspot_sta_join.info.macAddress, info->mac, WIFI_MAC_LEN);

            for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
                if (g_HalHmosEvent[i] != NULL)
                    HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
            }
        }
    }
}

static void HalHmosApEventStaLeave(WIFI_USER_EVT_ID evt_id, void *arg)
{
    BWIFI_AP_STA_INFO_T  *info = (BWIFI_AP_STA_INFO_T *)arg;
    if (info != NULL) {
        hmos_printf("F:%s L:%d event:%d, arg=%p\n", __func__, __LINE__, evt_id, info);

        if (evt_id == WIFI_USER_EVT_AP_STA_DISCONNECTED) {
            int i;
            HalHmosEventInfo event;

            event.event_id = HMOS_ON_HOTSPOT_STA_LEAVE_CHANGED;
            event.event_info.hotspot_sta_join.info.disconnectedReason = info->reason;
            memcpy(event.event_info.hotspot_sta_join.info.macAddress, info->mac, WIFI_MAC_LEN);

            for (i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
                if (g_HalHmosEvent[i] != NULL)
                    HalHmosWifiEventCall(&event, g_HalHmosEvent[i]);
            }
        }
    }
}

static WifiSecurityType HalHmosSecTypeConvert(BWIFI_SEC_TYPE_T security_type)
{
    switch (security_type) {
    case BWIFI_SECURITY_NONE:
        return WIFI_SEC_TYPE_OPEN;
    case BWIFI_SECURITY_WEP40:
    case BWIFI_SECURITY_WEP104:
        return WIFI_SEC_TYPE_WEP;
    case BWIFI_SECURITY_WPA:
    case BWIFI_SECURITY_WPA2:
    case BWIFI_SECURITY_WPA_WPA2:
        return WIFI_SEC_TYPE_PSK;
    case BWIFI_SECURITY_WPA3_SAE:
    case BWIFI_SECURITY_WPA3_SAE_WPA2:
        return WIFI_SEC_TYPE_SAE;
    default:
        return WIFI_SEC_TYPE_INVALID;
    }
}

static int HalHmosConvertChanToFreq(u8 chan)
{
    int freq = 0;

    if (chan >= 1 && chan <= 13)
        freq = 2407 + chan * 5;
    else if (chan == 14)
        freq = 2484;
    else if (chan >= 182 && chan <= 196)
        freq = 4000 + chan * 5;
    else if (chan >= 36 && chan <= 169)
        freq = 5000 + chan * 5;

    return freq;
}
static int HalHmosConvertFreqToChan(int freq)
{
    int chan = 0;

    if (freq >= 2412 && freq <= 2472)
        chan = (freq - 2407) / 5;
    else if (freq == 2484)
        chan = 14;
    else if (freq >= 4900 && freq < 5000)
        chan = (freq - 4000) / 5;
    else if (freq >= 5000 && freq < 5900)
        chan = (freq - 5000) / 5;
    else if (freq >= 56160 + 2160 * 1 && freq <= 56160 + 2160 * 4)
        chan = (freq - 56160) / 2160;

    return chan;
}

static int HalHmosChannToBand(u8 chan)
{
    if (chan <= 14)
        return HOTSPOT_BAND_TYPE_2G;

    return HOTSPOT_BAND_TYPE_5G;
}

static WifiErrorCode HalHmosGetNidDeviceConfigs(WifiDeviceConfig *result, int nid)
{
    int i                                   = 0;
    WifiErrorCode ret                       = ERROR_WIFI_BUSY;
    HalHmosWifiConfig  *hmos_config_info    = &(g_HalHmosWifiInfo.hmos_config_info);

    HalHmosWifiLock();
    for (i = 0; i < WIFI_MAX_CONFIG_SIZE; i++) {
        if (hmos_config_info->wifi_config_map[i >> 3] & (1 << (i % 8))) {
            if (hmos_config_info->wifi_config[i].netId == nid) {
                memcpy(result, &(hmos_config_info->wifi_config[i]), sizeof(WifiDeviceConfig));
                ret =  WIFI_SUCCESS;
            }

        }

    }
    HalHmosWifiUnLock();

    return ret;
}

WifiErrorCode EnableWifi(void)
{
    WifiErrorCode ret;
    uint8_t mode;

    if (WIFI_SUCCESS != HalHmosWifiEventInit())
        return ERROR_WIFI_UNKNOWN;

    if (g_HalHmosWifiInfo.wifi_init == true)
        return ERROR_WIFI_BUSY;

    HalHmosWifiLock();
    bwifi_reg_eth_input_handler(ethernetif_input);
#if LOSCFG_SOC_SERIES_BES2600
    mode = (DUAL_BAND << 4) | 0x0;      /*BAND_2G4 or DUAL_BAND*/
    ret = bwifi_init(mode);
#elif LOSCFG_SOC_SERIES_BES2700
    ret = bwifi_init();
#endif
    if (ret == WIFI_SUCCESS) {
        extern struct netif if_wifi;
        struct netif *p_netif = &if_wifi;

        tcpip_init(NULL, NULL);

        extern int bwifi_get_own_mac(u8 *addr);
        bwifi_get_own_mac(_mac_addr);
        lwip_netif_mac_addr_init(p_netif, _mac_addr, ETH_ALEN);

        extern err_t ethernetif_init(struct netif *netif);
        if (netif_add(p_netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4, NULL, ethernetif_init, tcpip_input) == 0) {
            printf("netif add wifi interface failed\n");
            return -1;
        }

        g_HalHmosWifiInfo.wifi_init = true;

        netif_set_default(p_netif);

        netif_set_status_callback(p_netif, net_intf_status_change_cb);

        g_staEventHandler.OnWifiConnectionChanged = WifiConnectionChangedHandler;
        int error = RegisterWifiEvent(&g_staEventHandler);
        if (error != WIFI_SUCCESS) {
            printf("[sample] RegisterWifiEvent fail, error = %d\n", error);
            return -1;
        }
    }

    HalHmosWifiUnLock();

    return ret;
}

WifiErrorCode DisableWifi(void)
{
    WifiErrorCode ret = ERROR_WIFI_BUSY;
    int disconn_return = 0;

    HalHmosWifiLock();
    disconn_return = bwifi_disconnect();
    if (disconn_return == BWIFI_R_OK || disconn_return == BWIFI_R_STATUS_ERROR) {
        if(g_HalHmosWifiInfo.wifi_init == false){
            ret = ERROR_WIFI_NOT_STARTED;
        } else {
            g_HalHmosWifiInfo.wifi_init = false;
            ret = WIFI_SUCCESS;
        }
    }

    HalHmosWifiUnLock();

    return ret;
}

int IsWifiActive(void)
{
    return (g_HalHmosWifiInfo.wifi_init == true ? WIFI_STA_ACTIVE : WIFI_STA_NOT_ACTIVE);
}

WifiErrorCode Scan(void)
{
    WifiErrorCode ret = ERROR_WIFI_BUSY;


    if (IsWifiActive() != WIFI_STA_ACTIVE)
        return ERROR_WIFI_IFACE_INVALID;

    if (g_HalHmosWifiInfo.scan_state == SCAN_REQUEST ||
        g_HalHmosWifiInfo.scan_state == SCAN_TRIGGER)
        return ERROR_WIFI_BUSY;

    HalHmosWifiLock();
    ret = ((HalHmosSendEvent(HMOS_ON_WIFI_SCAN_STATE_CHANGED, NULL) == 0) ? WIFI_SUCCESS : ERROR_WIFI_BUSY);
    HalHmosWifiUnLock();

    return ret;
}

WifiErrorCode GetScanInfoList(WifiScanInfo *result, unsigned int *size)
{
    int i;
    int scan_result_len                         = *size;
    struct bwifi_bss_info *b_result             = NULL;
    HalHmosWifiConfig  *hmos_config_info        = &(g_HalHmosWifiInfo.hmos_config_info);
    WifiErrorCode ret                           = ERROR_WIFI_BUSY;

    if (IsWifiActive() != WIFI_STA_ACTIVE)
        return ERROR_WIFI_IFACE_INVALID;

    if (scan_result_len >= WIFI_SCAN_HOTSPOT_LIMIT) {

        if (hmos_config_info->scan_size > 0 &&
            scan_result_len > hmos_config_info->scan_size)
            scan_result_len = hmos_config_info->scan_size;

        b_result = malloc(scan_result_len * sizeof(struct bwifi_bss_info));
        if (b_result != NULL) {
            scan_result_len = bwifi_get_scan_result(b_result, scan_result_len);
            for (i = 0; i < scan_result_len; i++) {
                memcpy((result + i)->ssid, (b_result + i)->ssid, WIFI_MAX_SSID_LEN);
                memcpy((result + i)->bssid, (b_result + i)->bssid, WIFI_MAC_LEN);
                (result + i)->band = HalHmosChannToBand((b_result + i)->channel);
                (result + i)->frequency = HalHmosConvertChanToFreq((b_result + i)->channel);
                (result + i)->securityType = HalHmosSecTypeConvert((b_result + i)->security_type);
                (result + i)->rssi = (b_result + i)->rssi;
                hmos_printf("%s ssid=%s channel=%d sec=%d\n\r", __func__,
                            (result + i)->ssid, (result + i)->frequency, (result + i)->securityType);
            }

            free(b_result);
            *size = scan_result_len;
            ret = WIFI_SUCCESS;
        }
    }
    return ret;
}

WifiErrorCode AddDeviceConfig(const WifiDeviceConfig *config, int *result)
{
    int netId;
    WifiErrorCode ret                           = ERROR_WIFI_BUSY;

    HalHmosWifiLock();
    netId = HalHmosGetNewNetId(&(g_HalHmosWifiInfo.hmos_config_info));
    if (netId < WIFI_MAX_CONFIG_SIZE) {
        HalHmosAddNetIdConfig(&(g_HalHmosWifiInfo.hmos_config_info), netId, config);
        *result = netId;
        ret = WIFI_SUCCESS;
    }
    HalHmosWifiUnLock();
    return ret;
}

WifiErrorCode GetDeviceConfigs(WifiDeviceConfig *result, unsigned int *size)
{
    int i = 0 ;
    int cnt = 0;
    HalHmosWifiConfig  *hmos_config_info = &(g_HalHmosWifiInfo.hmos_config_info);

    if (g_HalHmosWifiInfo.hmos_config_info.wifi_config_map[0 >> 3] == 0)
        return ERROR_WIFI_NOT_AVAILABLE;

    HalHmosWifiLock();
    for (i = 0; i < WIFI_MAX_CONFIG_SIZE; i++) {
        hmos_printf("%s i=%d cnt=%d\n\r", __func__, i, cnt);
        if (hmos_config_info->wifi_config_map[i >> 3] & (1 << (i % 8))) {
            memcpy(result + cnt, &(hmos_config_info->wifi_config[i]), sizeof(WifiDeviceConfig));
            cnt++;
        }
    }
    *size = cnt;
    HalHmosWifiUnLock();
    return WIFI_SUCCESS;
}

WifiErrorCode RemoveDevice(int networkId)
{
    WifiErrorCode ret = ERROR_WIFI_INVALID_ARGS;

    HalHmosWifiLock();
    if (networkId < WIFI_MAX_CONFIG_SIZE) {
        HalHmosRemoveNetId(&(g_HalHmosWifiInfo.hmos_config_info), networkId);
        ret =  WIFI_SUCCESS;
    }

    HalHmosWifiUnLock();
    return ret;
}

WifiErrorCode ConnectTo(int networkId)
{
    WifiLinkedInfo *info                    = &(g_HalHmosWifiInfo.info);
    HalHmosWifiConfig *hmos_config_info     = &(g_HalHmosWifiInfo.hmos_config_info);
    WifiErrorCode   ret                     = ERROR_WIFI_UNKNOWN;
    WifiDeviceConfig result;

    if (IsWifiActive() != WIFI_STA_ACTIVE)
        return ERROR_WIFI_IFACE_INVALID;

    HalHmosWifiLock();
    if (WIFI_SUCCESS == HalHmosGetNidDeviceConfigs(&result, networkId)) {
        hmos_printf("%s %d ssid=%s,psk=%s,wepid=%d ,hidder=0\n\r",
                    __func__, __LINE__, result.ssid, result.preSharedKey,
                    result.securityType == WIFI_SEC_TYPE_WEP ? 1 : 0);
        hmos_printf("bssid=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n\r",
                    result.bssid[0], result.bssid[1], result.bssid[2],
                    result.bssid[3], result.bssid[4], result.bssid[5]);
        hmos_config_info->networkId = networkId;
        memset(info, 0, sizeof(WifiLinkedInfo));
        memcpy(info->bssid, result.bssid, WIFI_MAC_LEN);
        strcpy(info->ssid, result.ssid);
        info->rssi = bwifi_get_current_rssi();

        if (BWIFI_R_OK == bwifi_connect_to_ssid(info->ssid, result.preSharedKey,
                                                result.securityType == WIFI_SEC_TYPE_WEP ? 1 : 0,
                                                g_HalHmosWifiInfo.hidden, info->bssid))
            ret = WIFI_SUCCESS;

    }
    HalHmosWifiUnLock();
    return ret;
}

WifiErrorCode Disconnect(void)
{
    WifiErrorCode ret;
    if (IsWifiActive() != WIFI_STA_ACTIVE)
        return ERROR_WIFI_IFACE_INVALID;
    HalHmosWifiLock();
    ret = ( bwifi_disconnect() == BWIFI_R_OK ? WIFI_SUCCESS : ERROR_WIFI_UNKNOWN);
    if (ret == WIFI_SUCCESS)
        g_HalHmosWifiInfo.scan_state = SCAN_INIT;
    HalHmosWifiUnLock();
    return ret;
}

WifiErrorCode GetLinkedInfo(WifiLinkedInfo *result)
{
    WifiLinkedInfo *info    = &(g_HalHmosWifiInfo.info);
    if (result == NULL)
        return ERROR_WIFI_INVALID_ARGS;
    info->rssi = bwifi_get_current_rssi();
    memcpy(result, info, sizeof(WifiLinkedInfo));
    hmos_printf("%s ssid=%s rssi=%d stat=%d\n\r", __func__,
                info->ssid, info->rssi, info->connState);
    return WIFI_SUCCESS;
}

WifiErrorCode RegisterWifiEvent(WifiEvent *event)
{
    WifiErrorCode ret = ERROR_WIFI_UNKNOWN;

    if (event == NULL) {
        hmos_printf("%s e1\n\r", __func__);
        return ERROR_WIFI_INVALID_ARGS;
    }

    if (WIFI_SUCCESS != HalHmosWifiEventInit())
        return ERROR_WIFI_UNKNOWN;

    for (int i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
        if (g_HalHmosEvent[i] == event) {
            hmos_printf("%s e2\n\r", __func__);
            return ERROR_WIFI_INVALID_ARGS;
        }
        if (g_HalHmosEvent[i] == NULL) {
            HalHmosWifiLock();
            g_HalHmosEvent[i] = event;
            ret = WIFI_SUCCESS;
            i = WIFI_MAX_EVENT_SIZE;
            HalHmosWifiUnLock();
        }
    }

    return ret;
}

WifiErrorCode UnRegisterWifiEvent(const WifiEvent *event)
{
    if (event == NULL)
        return ERROR_WIFI_INVALID_ARGS;

    for (int i = 0; i < WIFI_MAX_EVENT_SIZE; i++) {
        if (g_HalHmosEvent[i] == event) {
            HalHmosWifiLock();
            g_HalHmosEvent[i] = NULL;
            HalHmosWifiUnLock();
            return WIFI_SUCCESS;
        }
    }

    return ERROR_WIFI_UNKNOWN;
}

WifiErrorCode GetDeviceMacAddress(unsigned char *result)
{
    if (result == NULL)
        return ERROR_WIFI_INVALID_ARGS;

    if (BWIFI_R_OK != bwifi_get_own_mac((uint8_t *)result))
        return ERROR_WIFI_UNKNOWN;

    return WIFI_SUCCESS;
}

WifiErrorCode AdvanceScan(WifiScanParams *params)
{
    int chan[2]                         = {0};
    WifiErrorCode ret                   = ERROR_WIFI_UNKNOWN;
    struct bwifi_ssid *scan_ssid        = NULL;
    HalHmosWifiCmdInfo scan_config      = {0};
    WifiScanParams params_tmp = {0};
    char tmp[] = "wifi_service_xts";

    if (params == NULL || memcmp(params, &params_tmp, sizeof(WifiScanParams)) == 0)
        return ERROR_WIFI_UNKNOWN;

    params->scanType = (params->scanType == 255 ? WIFI_BAND_SCAN : params->scanType);
    params_tmp.scanType = params->scanType;
    if (memcmp(params, &params_tmp, sizeof(WifiScanParams)) == 0 && params_tmp.scanType != WIFI_BAND_SCAN)
        return ERROR_WIFI_UNKNOWN;

    memset(&params_tmp, 0, sizeof(WifiScanParams));
    strcpy(&params_tmp.ssid, tmp);
    params->scanType = (params->scanType == 255 ? WIFI_BAND_SCAN : params->scanType);
    params_tmp.scanType = params->scanType;
    if (memcmp(params, &params_tmp, sizeof(WifiScanParams)) == 0 && params_tmp.scanType != WIFI_BAND_SCAN)
        return ERROR_WIFI_UNKNOWN;

    if (IsWifiActive() != WIFI_STA_ACTIVE)
        return ERROR_WIFI_IFACE_INVALID;

    if (g_HalHmosWifiInfo.scan_state == SCAN_REQUEST ||
        g_HalHmosWifiInfo.scan_state == SCAN_TRIGGER)
        return ERROR_WIFI_BUSY;

    HalHmosWifiLock();
    if (params != NULL) {
        if (params->scanType == WIFI_FREQ_SCAN) {
            chan[0] = HalHmosConvertFreqToChan(params->freqs);
            /* free it by the thread HalHmosWifiEventThread */
            scan_config.scan_info.channels = (int *)malloc(sizeof(chan));
            if (scan_config.scan_info.channels != NULL) {
                hmos_printf("%s chann =%p\n\r", __func__, scan_config.scan_info.channels);
                memcpy(scan_config.scan_info.channels, chan, sizeof(chan));
            }
        } else if (params->scanType == WIFI_SSID_SCAN) {
            /* free it by the thread HalHmosWifiEventThread */
            scan_ssid = (struct bwifi_ssid *)malloc(sizeof(struct bwifi_ssid));
            if (scan_ssid != NULL) {

                memcpy(scan_ssid->ssid, params->ssid, params->ssidLen);
                scan_config.scan_info.ssids = scan_ssid;
                scan_ssid->next = NULL;
                scan_ssid->ssid[(int)(0xff & params->ssidLen)] = '\0';
                hmos_printf("%s ssid =%p ssid=%s\n\r", __func__, scan_ssid, scan_ssid->ssid);
                ret = WIFI_SUCCESS;
            }
        } else {
            printf("WIFI_BSSID_SCAN and WIFI_BAND_SCAN not support\n");
            ret = ERROR_WIFI_INVALID_ARGS;
        }
    }
    ret = ((HalHmosSendEvent(HMOS_ON_WIFI_SCAN_STATE_CHANGED, &scan_config) == 0) ? WIFI_SUCCESS : ERROR_WIFI_BUSY);
    HalHmosWifiUnLock();

    if (ret == ERROR_WIFI_BUSY) {
        if (scan_config.scan_info.channels != NULL)
            free(scan_config.scan_info.channels);
        if (scan_ssid != NULL)
            free(scan_ssid);
    }

    return ret;
}

WifiErrorCode SetCountryCode(const char *countryCode)
{
    WifiErrorCode ret = ERROR_WIFI_UNKNOWN;

    if (countryCode == NULL || strlen(countryCode) > BWIFI_MAX_COUNTRY_CODE_SIZE)
        return ERROR_WIFI_INVALID_ARGS;

    HalHmosWifiLock();
    if (bwifi_set_country_code((char *)countryCode) == BWIFI_R_OK)
        ret = WIFI_SUCCESS;
    HalHmosWifiUnLock();

    hmos_printf("%s=%s,ret=%d\n\r", __func__, countryCode, ret);
    return ret;
}

WifiErrorCode GetCountryCode(char *countryCode, unsigned int *len)
{
    WifiErrorCode ret = ERROR_WIFI_UNKNOWN;

    if (countryCode == NULL)
        return ERROR_WIFI_INVALID_ARGS;

    HalHmosWifiLock();
    if (bwifi_get_country_code(countryCode) == BWIFI_R_OK)
        ret = WIFI_SUCCESS;
    HalHmosWifiUnLock();
    hmos_printf("%s=%s\n\r", __func__, countryCode);
    return ret;
}

