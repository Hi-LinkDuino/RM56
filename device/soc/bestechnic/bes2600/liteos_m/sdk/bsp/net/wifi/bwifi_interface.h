/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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

#ifndef BWIFI_INTERFACE_H
#define BWIFI_INTERFACE_H

#include "plat_types.h"
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "wifi_def.h"
#include "bwifi_hal.h"
#include "bwifi_event.h"
#if LWIP_ETHERNETIF
#include "lwip/netif.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BWIFI_MAX_COUNTRY_CODE_SIZE     2
#define BWIFI_CONFIG_VAL_LEN            100
#define WIFI_SCAN_DUMP

typedef enum {
    WIFI_IF_STATION,
    WIFI_IF_SOFTAP,
} BWIFI_INTF_TYPE_T;

typedef enum {
    BWIFI_STATUS_IDLE            = 0,
    BWIFI_STATUS_DISCONNECTING   = 1,
    BWIFI_STATUS_SCANNING        = 2,
    BWIFI_STATUS_CONNECTING      = 3,
    BWIFI_STATUS_WPS_CONNECTING  = 4,
    BWIFI_STATUS_CONNECTED       = 5,
    BWIFI_STATUS_DHCPING         = 6,
    BWIFI_STATUS_GOT_IP          = 7,
    BWIFI_STATUS_ONLINE_SCANNING = 8,
    BWIFI_STATUS_END_DEF,  /* must be the last one */
} BWIFI_STATUS_T;

enum {
    BWIFI_R_OK                  = 0,
    BWIFI_R_COMMON_FAIL         = -1,
    BWIFI_R_INVALID_ARG         = -2,  //invalid argument
    BWIFI_R_INVALID_PASSWORD    = -3,  //invalid password
    BWIFI_R_MEMORY_ERROR        = -4,  //no memory to allocate resource
    BWIFI_R_INIT_FAIL           = -5,  //init wifi fail
    BWIFI_R_NOT_INITED          = -6,  //wifi is not initialized
    BWIFI_R_STATUS_ERROR        = -7,  //request in error STATUS
    BWIFI_R_SCAN_REQ_FAIL       = -8,  //scan fail to start
    BWIFI_R_SCAN_NO_AP_FOUND    = -9,  //scan result is NULL (didn't find any SSID)
    BWIFI_R_NO_SUITABLE_NETWORK = -10, //no suitable network to connect
    BWIFI_R_CONN_REQ_FAIL       = -11, //connect fail to start
    BWIFI_R_CONN_FAIL           = -12, //connect procedure result in fail
    BWIFI_R_CONN_NO_SSID_CONFIG = -13, //no saved SSID config to connect
    BWIFI_R_DISC_FAIL           = -14, //disconnect procedure result in fail
    BWIFI_R_WPS_NOT_FOUND       = -15, //couldn't find WPS AP
    BWIFI_R_WPS_REQ_FAIL        = -16, //WPS fail to start
};

enum {
    WIFI_MFP_DISABLE = 0,   /**< disable mfp capability */
    WIFI_MFP_ENABLE,        /**< have mfp capability, but not require peer to support mfp */
    WIFI_MFP_REQUIRED,      /**< have mfp capability, and require peer to support mfp */
    WIFI_MFP_DEFAULT,       /**< mfp is not specified, setting it based on our capability and network type */
};

typedef enum bwifi_softap_status {
    BWIFI_SOFTAP_STATUS_OFF   = 0,
    BWIFI_SOFTAP_STATUS_ON,
} BWIFI_SOFTAP_STATUS_T;

typedef enum {
    WIFI_USER_EVT_CONN_INTER_STATE,
    WIFI_USER_EVT_CONNECTED,
    WIFI_USER_EVT_GOT_IP,
    WIFI_USER_EVT_DISCONNECTED,
    WIFI_USER_EVT_ROAM_COMPLETE,
    WIFI_USER_EVT_AP_ENABLED,
    WIFI_USER_EVT_AP_DISABLED,
    WIFI_USER_EVT_AP_STA_CONNECTED,
    WIFI_USER_EVT_AP_STA_DISCONNECTED,
    WIFI_USER_EVT_FATAL_ERROR,
    /* mutually exclusive events */
    /* If user layer wants to take over the initiative of wifi reset,
     * register this event for the timing to trigger reset.
     */
    WIFI_USER_EVT_MAC_RESET_REQUEST,
    /* If user layer is only interested in the process of wifi reset,
     * register these events to get notify when reset start/end.
     */
    WIFI_USER_EVT_MAC_RESET_START,
    WIFI_USER_EVT_MAC_RESET_DONE,
    WIFI_USER_EVT_COEX_MODE_CHANGE,
    WIFI_USER_EVT_LOW_POWER_CHANGE,
    WIFI_USER_EVT_MAX
} WIFI_USER_EVT_ID;

typedef enum {
    FATAL_ERROR = 1,
    UMAC_CRASH  = 2,
} BWIFI_MAC_RESET_REASON;

struct ip_info {
    uint32_t ip;               /**< IP address */
    uint32_t netmask;          /**< netmask */
    uint32_t gw;               /**< gateway */
};

struct bwifi_quick_connect_config {
    struct bwifi_station_config config;
    uint32_t channel;
    uint32_t ip[3];//struct ip_info ip;
};

typedef void (*user_evt_handler_t)(WIFI_USER_EVT_ID evt_id, void *arg);

void bwifi_reg_user_evt_handler(WIFI_USER_EVT_ID evt_id, user_evt_handler_t cb);
void bwifi_unreg_user_evt_handler(WIFI_USER_EVT_ID evt_id);
void bwifi_reg_eth_input_handler(eth_input_handler cb);
void bwifi_unreg_eth_input_handler();
#ifdef __SNIFFER_MODE__
void bwifi_reg_sniffer_recv_handler(sniffer_recv_handler cb);
void bwifi_unreg_sniffer_recv_handler();
int bwifi_sniffer_cmd(int cmd, void *param);
#endif
#if WIFI_NET_TOOL_SUPPORT
/*
 * Add network test tool tcpdump element to wpa_supplicant
 */
typedef void (*bwifi_tcpdump_callback)(unsigned char *buf, int len);
void bwifi_reg_tcpdump_handler(bwifi_tcpdump_callback cb);
void bwifi_unreg_tcpdump_handler(void);
/*
 * Add network test tool wifi status query pta element to wpa_supplicant
 * return the pta of current wifi status.
 */
float bwifi_get_pta_set(void);
#endif
/*
 * wifi record interface
 */
int bwifi_find_record(const char *type, void *record_info);
int bwifi_add_record(void *record_info);
int bwifi_del_record(char *type);
/*
 * Add network config to wpa_supplicant
 * return the allocated network id, or negative on failure.
 */
int bwifi_add_config(struct bwifi_station_config *config);
/*
 * Modify network config item
 * return the result of current operation
 */
int bwifi_modify_config(struct bwifi_config_item *item);
/*
 * Get the number of saved network configs from wpa_supplicant.
 */
int bwifi_count_configured_ssid(void);
/*
 * Get network configs saved in wpa_supplicant
 * return the number of networks
 */
int bwifi_get_current_config(struct bwifi_station_config *config, int count);
/*
 * Save network config to flash (max 8)
 */
int bwifi_save_config(struct bwifi_station_config *config);
/*
 * Get network configs saved in flash
 * return the number of networks
 */
int bwifi_get_saved_config(struct bwifi_station_config *config, int count);
/*
 * Find network config saved in flash who matches with the specified ssid
 */
int bwifi_find_saved_config_by_ssid(const char *ssid, struct bwifi_station_config *config);
/*
 * Delete all network configs saved in wpa and flash
 */
int bwifi_del_all_config(void);
/*
 * Delete network config saved in wpa and flash
 * It will check ssid, passwd, hidden, web_keyid, bssid (if not zero),
 * if config is NULL or config->ssid is zero length, delete all.
 */
int bwifi_del_config(struct bwifi_station_config *config);
/*
 * Scan for wildcard ssid and saved hidden ssids
 * you can get bss list from bwifi_get_scan_result
 * return bss number or error code
 */
int bwifi_scan(void);
/*
 * Scan for specified ssid (if not NULL) on the specified channels (if no 0)
 * you can get bss list from bwifi_get_scan_result
 * return bss number or error code
 */
int bwifi_config_scan(struct bwifi_scan_config *scan_config);
/*
 * Get scan result
 */
int bwifi_get_scan_result(struct bwifi_bss_info *result, int count);
/*
 * Clear saved scan list which is not in use
 */
void bwifi_flush_scan_result(void);
/*
 * Auto connect to an AP saved in wpa_supplicant
 */
int bwifi_auto_connect(void);
/*
 * Connect to an AP assigned by user config
 *
 * This function will add the network config to wpa_supplicant and trigger new connection.
 * If user wants to connect to another AP after connected:
 *   1. bwifi_disconnect()
 *   2. bwifi_connect(config)
 */
int bwifi_connect(struct bwifi_station_config *config);
/*
 * Connect to an AP via WPS PBC
 */
int bwifi_connect_wps_pbc(void);
/*
 * Connect to specified ssid, passwd can be NULL for unencrypted AP.
 */
int bwifi_connect_to_ssid(const char *ssid, const char *passwd, int8_t wep_keyid, uint8_t hidden, uint8_t *bssid);
/*
 * Connect to specified ssid and mfp, passwd can't be NULL
 */
int bwifi_connect_to_ssid_with_mfp(const char *ssid, const char *passwd, int8_t wep_keyid, uint8_t hidden, uint8_t *bssid, uint8_t mfp);
/*
 * Connect to specified bssid, passwd can be NULL for unencrypted AP.
 */
int bwifi_connect_to_bssid(uint8_t *bssid, const char *passwd);
/*
 * Connect to specified network by network id allocated in wpa_supplicant.
 */
int bwifi_connect_to_nid(int nid);
/*
 * Disconnect to current connected AP or stop connecting to AP
 */
int bwifi_disconnect(void);

/*
 * Quick connect functions
 */
int bwifi_get_quick_connect_config(struct bwifi_quick_connect_config *quick_config);
int bwifi_set_quick_connect_config(struct bwifi_quick_connect_config *quick_config);
int bwifi_del_quick_connect_config(void);
int bwifi_quick_connect(struct bwifi_quick_connect_config *quick_config);

/*
 * Set whether auto reconnect after disconnection
 * default val true
 */
void bwifi_set_reconnect_policy(bool enable);
/*
 * Get current reconnect policy
 */
bool bwifi_get_reconnect_policy(void);
/*
 * Get current connected AP info
 */
int  bwifi_get_current_ssid(char *ssid);
int  bwifi_get_current_bssid(uint8_t *bssid);
int  bwifi_get_own_mac(uint8_t *addr);
uint8_t   bwifi_get_current_channel(void);
int8_t bwifi_get_current_rssi(void);
int  bwifi_get_current_rate(void);

/**
 * Get station linkinfo statistics which can be used to evaluate network traffic.
 * @info: pointer to a bwifi_station_linkinfo struct to linkinfo statistics
 * Returns: void
 */
void bwifi_get_station_linkinfo(bwifi_station_linkinfo *info);

#if LWIP_SUPPORT
/*
 * Enable or disable using the static IP for subsequent connection.
 *
 * The DHCP clent is enabled by default and collides with the static IP.
 * If this API is callbed with a valid pointer to the ip_info struct,
 * DHCP client will be disabled and the static IP in ip_info will be used;
 * if this API is called with NULL, then DHCP client will be enabled.
 * It depends on the latest configuration.
 */
int bwifi_set_static_ip(struct ip_info *ip);
/*
 * Get current ip addr of wifi station (dhcpc or static ip)
 */
int bwifi_get_current_ip(struct ip_info *ip);

#else

#if LWIP_ETHERNETIF
/*
 * Get netif struct of wifi station or softap
 *
 * This function should only be called when SDK inside LWIP is turned off
 * but ethernet interface is added and initialized by us.
 * return the netif struct to user's LWIP stack for further management.
 */
struct netif *bwifi_get_netif(BWIFI_INTF_TYPE_T type);
#endif

/*
 * Set netif ip addr to wifi mac layer for ARP filter feature
 *
 * This function should only be called when SDK inside LWIP is turned off
 * and DHCP procedure is also taken over by user's LWIP stack,
 * we need user to tell us the assigned local ip addr.
 */
int bwifi_set_ip_addr(BWIFI_INTF_TYPE_T type, struct ip_info *ip);
#endif

/**
 * Change current wifi status
 *
 * @new_status: The new wifi status
 * Returns: void
 */
void bwifi_change_current_status(BWIFI_STATUS_T new_status);

void airkiss_notify(uint8_t token);

/**
 * Enable or disable the statistics of the frames sent out and received
 * in the last interval_sec time.
 * @en: 0 = disable, 1 = enable
 * @interval_sec: time of the statistics in seconds
 * Returns: void
 *
 * @note    If enabled, the statistics information will be output via uart log.
 */
void bwifi_trans_stat_en(uint8_t en, uint8_t interval_sec);
/**
 * Get trans statistics which can be used to evaluate network traffic.
 * @stat: pointer to a bwifi_trans_stat struct to retrieve trans statistics
 * @clear: 1 = clear, 0 = keep
 * Returns: void
 *
 * @note    Elements in bwifi_trans_stat will be cleared every time this function
 *          is called if clear is set to 1.
 */
void bwifi_trans_stat_get(bwifi_trans_stat *stat, int8_t clear);

/*
 * Initialize wifi hardware and interface
 * @mode: bit7~4 indicate WIFI_BAND: 0 = 2.4G; 1 = 5G; 2 = dual_band
 *        bit3~0 indicate WIFI_RUN_MODE: 0 = normal; 1 = nonsignaling; 2 = wifi reset
 */
int bwifi_init(uint8_t mode);

#ifdef WIFI_ON_CP
/*
 * reset wifi stack on cp
 */
int bwifi_reinit(void);
#endif

/*
 * Set country code
 * If not set, we use the default country code CN.
 */
int bwifi_set_country_code(char *country);
/*
 * Get current country code
 */
int bwifi_get_country_code(char *country);

void bwifi_set_connecting_status(void);
/*
 * Set powersave mode for legacy Wi-Fi (LMAC).
 * @ps: 0 = disable, 1 = enable
 * Returns: 0 on success or negtive on failure
 */
int bwifi_set_ps_mode(int ps);

/*
 * Set low power for legacy Wi-Fi (UMAC).
 * @ps: 0 = disable, 1 = enable
 * Returns: 0 on success or negtive on failure
 */
int bwifi_set_low_power(int lowpower);

/*
 * swtich channel dynamically
 * @mode: 0 softap mode, 1 sta mode
 * @channel: channel number
 * @snd_offset: secondary channel offset to center freq
 *     0 - no snd ch, 1 - upper 10M, -1 - lower 10M
 * Returns: 0 on success or negtive on failure
 */
int bwifi_switch_channel(uint8_t mode, uint8_t channel, int8_t snd_offset);

#ifdef __AP_MODE__
/*
 * Start softap with previous configuration
 */
int bwifi_softap_start(void);

/*
 * softap mode disconnect a station
 */
int bwifi_softap_disconnect_sta(uint8_t *ucMac);

/*
 * Stop softap
 */
void bwifi_softap_stop(void);

/**
 * Set softap basic configuration
 *
 * @ssid: AP's SSID
 * @channel: Primary channel for AP to start on
 * @sec_channel_offset: Secondary channel offset for HT40
 *     0 = HT40 disabled;
 *     1 = HT40 enabled, secondary channel above primary;
 *    -1 = HT40 enabled, secondary channel below primary
 * @hidden: Whether to ignore broadcast SSID
 *     0 = broadcast ssid;
 *     1 = hide ssid with zero length;
 *     2 = hide ssid with zero contents
 * @security: Enum of security type
 * @password: User passphrase
 *
 * Returns: 0 on success, nagtive if invalid arguments
 */
int bwifi_set_softap_config(char *ssid,
                            uint8_t channel, int sec_channel_offset,
                            uint8_t hidden,
                            BWIFI_SEC_TYPE_T security,
                            char *passwd);
/*
 * Set softap's maximum client number
 * @sta_num: Maximum number of STAs
 * Returns: void
 */
void bwifi_set_softap_max_sta_num(int sta_num);
/*
 * Set softap's country code
 * @country_code: Two octets string to represent a country code
 * Returns: void
 */
void bwifi_set_softap_country_code(char *country_code);
/**
 * Add vendor elements in softap's Beacon/ProbeRsp
 * @ie: Pointer to raw data of vendor elements
 * @ie_len: Length of vendor elements
 * Returns: 0 on success, nagtive on failure
 */
int bwifi_add_softap_vendor_elements(const uint8_t *ie, size_t ie_len);
/*
 * Get softap's client list
 * @mac_list: Pointer to buffer to store the mac address list
 * @count: STA count to retrieve, returns the actual connected STA count
 * Returns: 0 on success, nagtive on failure
 */
int bwifi_softap_get_station_list(struct bwifi_mac_addr *mac_list, int *count);

#endif

/*
 * Get current Wi-Fi status
 */
BWIFI_STATUS_T bwifi_get_current_status(void);
/*
 * Get current softap status
 */
BWIFI_SOFTAP_STATUS_T bwifi_get_softap_status(void);

/*
 * This function is used for userspace to send an encapsulated mgmt frame.
 * @data:  Pointer to the frame data
 * @len:   Length of the frame
 * @noack: Whether need to wait for ack
 * @chan:  Which channel to send
 */
int bwifi_send_mgmt_frame_on_chan(const uint8_t *data, size_t len, int noack, uint8_t chan);

/*
 * Enable or disable Wi-Fi recovery mechanism on fatal error.
 * which is turned off by default for debug purpose.
 */
void bwifi_recovery_enable(bool en);

/**
 * Set fix data rate for Wi-Fi transmission
 * @fix_rate_idx
 *   - 0 ~ 3:   DSSS/CCK rates: 1, 2, 5.5, 11;
 *   - 4 ~ 5:   reserved;
 *   - 6 ~ 13:  legacy OFDM rates: 6, 9, 12, 18, 24, 36, 48, 54;
 *   - 14 ~ 21: 1*1 MCS rates: MCS0 ~ MCS7;
 *   - 0xff:    default value to disable the fix rate function.
 */
void bwifi_set_fix_rate(uint32_t fix_rate_idx);
/*
 * Get current set fix rate index
 */
uint32_t  bwifi_get_fix_rate(void);

#ifdef ENABLE_FW_KEEP_ALIVE

/*
 * Set ip keep alive feature's parameters.
 * @paras ip alive parameters;
 * Returns: the idx of the tcp stream when adding one or the delete result when deleting one;
 */
int bwifi_set_ip_alive(struct ip_alive_paras *paras);

/*
 * Enable/disable ip keep alive function.
 * @period interval to sending tcp/udp heartbeat in seconds, set to 0 will stop sending out any;
 *
 * Returns: BWIFI_R_OK on success, others on failure;
 */
int bwifi_enable_ip_alive(uint16_t period);
#endif

#ifdef __SET_MULTICAST_FILTER__
/*
 * set a multicast address filter.
 * @paras mode. 0:disable filter. 1:enable filter. 2:clean addr table
 *              3:filter all multicast. 4:filter add. 5:filter del
 * @paras addr. filter address.
 *
 * Returns: BWIFI_R_OK on success, others on failure;
 */
int bwifi_set_multicast_filter(u8 mode, u8* addr);
#endif

#ifdef __SET_BROADCAST_FILTER__
/*
 * Enable/disable broadcast filter.
 *
 * Returns: BWIFI_R_OK on success, others on failure;
 */
int bwifi_set_broadcast_filter(u8 enable);
#endif

#ifdef __SET_ARP_OFFLOAD__
/*
 * Enable/disable arp offload.
 *
 * Returns: BWIFI_R_OK on success, others on failure;
 */
int bwifi_set_arp_offload(u8 enable);
#endif

/*
 * Do wifi reset
 * Returns: BWIFI_R_OK on success, others on failure
 */
int bwifi_reset(void);


/*
 * wifi power on/off
 * onoff: 1 - wifi power on, 0 - wifi power off
 * Returns: BWIFI_R_OK on success, others on failure
 */
int bwifi_power_on(uint8_t onoff);

/*
 * set epta parameters of wifi/bt coex
 * wifi_dur: wifi duration of active window
 * bt_dur: bt duration of active window
 * mode: epta mode
        0 - periodic mode in which the wifi/bt active time is specified by wifi_dur/bt_dur, only support 100ms period
        1 - arbitrate mode by hw itself
        2 - wifi active only mode
*/
void bwifi_set_epta_param(int wifi_dur, int bt_dur, int mode);

/*
0 - bt not active, allow to alloc max duration for wlan
1 - bt active, allow to alloc medium duration for wlan
2 - bt a2dp active
*/
void bwifi_bt_state_notify(int state);

/*
 * get free buf to send
 * Returns: address of buf pointer
 */
static inline uint8_t ** bwifi_get_data_buf(void)
{
    return (uint8_t **)BWIFI_HAL_API_CALL(get_send_buf);
}

/*
 * send data buf
 * @devnum: dev interface number
 * @tx_buf: address of send buffer pointer
 * @tx_len: send data length
 * Returns: BWIFI_R_OK on success, others on failure
 */
static inline int bwifi_send_data_buf(uint8_t devnum, uint8_t **tx_buf, uint16_t tx_len)
{
    return BWIFI_HAL_API_CALL(data_send, devnum, tx_buf, tx_len);
}

/*
 * bwifi_str_cmd - send string command
 * @type: command type, 0 - rf test, other - reserved
 * @cmd_buf: command string ended with '\0'
 * @rsp_buf: response string ended with '\0'
 * Returns: 0 - success, other - failure
 */
int bwifi_str_cmd(uint8_t type, uint8_t *cmd_buf, uint8_t *rsp_buf, uint32_t rsp_size);

/*
 * bwifi_autoip_enable - set autoip enable
 * @flag: autoip enable flag, 0 - disable(false), other - enable(true)
 * Return void
 */
void bwifi_autoip_enable(uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif /* BWIFI_INTERFACE_H */

/*
 * bwifi_check_capability - Check whether the macros in struct bwifi_hal_ops are aligned
 * Instruction: First, at the begining of bwifi_init(), the state of macros in bwifi_hal_ops
 * are saved with varible g_wifi_hal_cap; Then we can check the state of macros in bwifi_hal_ops
 * after bwifi_init() through call this function bwifi_check_capability().
 * Returns: 0 - success, other - failure
 */
extern uint32_t g_wifi_hal_cap;
static inline int bwifi_check_capability(uint32_t *localCap)
{
    uint32_t local_cap = 0;

#ifdef __AP_MODE__
    local_cap |= (1 << 0);
#endif

#ifdef ENABLE_FW_KEEP_ALIVE
    local_cap |= (1 << 1);
#endif

#ifdef __SNIFFER_MODE__
    local_cap |= (1 << 2);
#endif

#ifdef CSI_REPORT
    local_cap |= (1 << 3);
#endif

    if (localCap)
        *localCap = local_cap;

    if (local_cap == g_wifi_hal_cap)
        return 0;
    else
        return -1;
}

