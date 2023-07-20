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
#ifndef BWIFI_HAL_H
#define BWIFI_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bwifi_event.h"

#define BWIFI_MAX_COUNTRY_CODE_SIZE     2
#define BWIFI_CONFIG_VAL_LEN            100
#define WIFI_SCAN_DUMP
#define WIFI_COEX_MODE_FDD_BIT          (1<<0)
#define WIFI_COEX_MODE_FDD_HYBRID_BIT   (1<<1)
#define COEX_METRICS_PREFIX             "EPTA:"

typedef enum bwifi_security_type {
    BWIFI_SECURITY_NONE,              /*!< open access point */
    BWIFI_SECURITY_WEP40,             /*!< phrase conforms to WEP */
    BWIFI_SECURITY_WEP104,            /*!< phrase conforms to WEP EXTENDED */
    BWIFI_SECURITY_WPA,               /*!< WPA-PSK */
    BWIFI_SECURITY_WPA2,              /*!< WPA2-PSK */
    BWIFI_SECURITY_WPA_WPA2,          /*!< WPA WPA2 mixed mode */
    BWIFI_SECURITY_WPA3_SAE,          /*!< WPA3-SAE */
    BWIFI_SECURITY_WPA3_SAE_WPA2,     /*!< WPA3-SAE WPA2 mixed mode*/
    BWIFI_SECURITY_ERROR,             /*!< error mode */
} BWIFI_SEC_TYPE_T;

struct wifi_init_param {
    char     band;
    char     country[3];
    uint16_t *tx_power;
    uint16_t *tx_power_5G;
    uint8_t  *wifi_address;
    void     *fac_wifi;
    uint32_t coex_mode;  // bit0: 0 -TDD, 1 - FDD; bit1: HYBRID
    uint32_t sleep_mode; // 0 - normal mode, 1 - no sleep mode
    uint8_t  *sw_matrix_desc; // sw_matrix_desc
    int      sw_matrix_desc_len; // the sw_matrix_desc len
    uint32_t *new_run_flag; //match the same as HOOK_CFG_S.new_run_flag in wifi_drv.c
    uint32_t mac_sleep_lock_bit; //match LMAC_SLEPP in wiif_drv.h
#ifdef BES_SOFT_AGC_ANTI_NOISE
    uint32_t *agc_mode;	// 0 hw_agc, 1 soft agc , 2 invalid agc
#endif
};

struct bwifi_mac_addr {
    uint8_t mac[6];
};

struct bwifi_bss_info {
    uint8_t bssid[6];
    char ssid[32 + 1];
    uint8_t channel;
    int8_t rssi;
    BWIFI_SEC_TYPE_T security_type;
    uint8_t *ie; //user program couldn't free(ie);
    uint32_t ie_length;
};

struct bwifi_ap_info {
    char ssid[32 + 1];
    uint8_t channel;
    uint8_t hidden;
    BWIFI_SEC_TYPE_T security;
    int sec_channel_offset;
    int wpa_mode;
    char passwd[64];
};

struct bwifi_ssid {
    struct bwifi_ssid *next;
    uint8_t ssid[32 + 1]; //null-terminated string
};

struct bwifi_scan_config {
    struct bwifi_ssid *ssids;   /**< List of specified SSIDs */
    int *channels;              /**< Array of channels (zero-terminated) to scan or NULL for all channels */
};

struct bwifi_station_config {
    uint8_t ssid[33];
    uint8_t passwd[65];//passwd for encrypted ssid, set "all zero" for unencryped ssid
    int8_t wep_keyid;//for wep,[0-3] default val:0
    uint8_t hidden;//1:hidden
    uint8_t bssid[ETH_ALEN];//bssid or "all zero"
    uint8_t mfp;
};

typedef struct bwifi_trans_stat_t {
    uint32_t tx_succ_cnt;
    uint32_t tx_fail_cnt;
    uint32_t tx_retry_cnt;
    uint32_t rx_cnt;
    uint32_t rx_leak;
    uint32_t txbps; //tx throughput bit/s
    uint32_t rxbps; //rx throughput bit/s
} bwifi_trans_stat;

typedef struct bwifi_station_linkinfo_t {
    bwifi_trans_stat stat;
    int8_t rssi;
    int8_t channel;
    /* BIT 31: MCS_FLAG = 1, BTI30:ShortGI ,BIT29-26: BW(0 20M,1 40M), BIT25-0,MCS index
     * BIT 31: MCS_FALG = 0, BIT0-B25 is b/g tx rate
     * */
    int tx_rate;
    /* [hw rate]:
     * MCS rate value:    [14-21]={MCS0,MCS1,MCS2,MCS3,MCS4,MCS5,MCS6,MCS7}
     * 11g/11a rate value: [6-13]={6M, 9M,12M 18M,24M,36M,48M,54M}
     * 11b rate value: [0-3]={1M,2M,5.5M,11M}
     */
    /* BIT 31: MCS_FLAG = 1, BIT[4-0] is [hw rate].
     * BIT 31: MCS_FALG = 0, BIT[4-0] is [hw rate].
     * */
    int rx_rate;

} bwifi_station_linkinfo;

enum wifi_config_item {
    WIFI_CONFIG_ITEM_WAPI_CERT_PRIVATE_KEY = 0, /**< WAPI own private key */
    WIFI_CONFIG_ITEM_WAPI_CERT_AS_CERT,         /**< WAPI AS certification */
    WIFI_CONFIG_ITEM_WAPI_CERT_ASUE_CERT,       /**< WAPI ASUE certification */
    WIFI_CONFIG_ITEM_MAX
};

struct bwifi_config_item {
    int net_index;                      /**< network index */
    int item_index;                     /**< config item index */
    int val_len;                        /**< config item value length */
    uint8_t val[BWIFI_CONFIG_VAL_LEN];  /**< config item value */
};

enum bwifi_epta_state {
    EPTA_STATE_WIFI_DISCONNECTED,
    EPTA_STATE_WIFI_SCANNING,
    EPTA_STATE_WIFI_CONNECTING,
    EPTA_STATE_WIFI_CONNECTED,
    EPTA_STATE_WIFI_GOT_IP,
    EPTA_STATE_WIFI_TTS_START,
    EPTA_STATE_WIFI_TTS_END,
    EPTA_STATE_BT_INACTIVE,
    EPTA_STATE_BT_ACTIVE,
    EPTA_STATE_BT_A2DP,
    EPTA_STATE_NUM
};

enum bwifi_sniffer_cmd {
    SNIFFER_CMD_START,
    SNIFFER_CMD_STOP,
    SNIFFER_CMD_SETFILTER,
    SNIFFER_CMD_SETCH
};

enum bwifi_low_power_state {
	WIFI_LOW_POWER_DISABLE,
	WIFI_LOW_POWER_ENABLE,
};

#ifdef ENABLE_FW_KEEP_ALIVE
/*
 * ip keep alive feature's parameters.
 * @proto 0 for udp and 1 for tcp;
 * @klv_vendor different number stands for different vendor's keepalive configuration;
 * @idx add(idx=15) a tcp/udp keep alive stream, or the idx number(idx= 0-7) when reconfig one;
 *      set payload_len to 0 and payload to NULL when deleting one stream.
 * @src_port local port;
 * @dst_port tcp server's listen port;
 * @src_ip local ip address;
 * @dst_ip tcp server's ip address;
 * @payload payload of the keep alive packet.
 * @payload_len length of the payload.
 */
struct ip_alive_paras
{
    uint8_t klv_vendor;
    uint8_t idx;
    uint8_t period;
    uint8_t proto;
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t payload_len;
    uint8_t *payload;
    uint8_t *key;
    uint8_t *iv;
};
#endif

#ifdef __SET_MULTICAST_FILTER__
struct multicast_filter_paras
{
    u8 mode;
    u8 addr[ETH_ALEN];
};
#endif

struct bwifi_hal_ops {
	/* wifi power status 0 - power on, 1 - power off */
	int power_status;

	/**
	 * add_config - add sta config
	 * @config: bwifi_station_config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*add_config)(struct bwifi_station_config *config);

	/**
	 * modify_config - modify sta config
	 * @config: bwifi_station_config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*modify_config)(struct bwifi_config_item *item);

	/**
	 * count_ssid_config - get the number of sta config items
	 * @item: bwifi_config_item
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*count_ssid_config)(void);

	/**
	 * get_current_config - get current sta config to connect
	 * @config: bwifi_station_config array
     * @count: max item number of config array
	 *
	 * Returns: the number of current config
	 */
    int (*get_current_config)(struct bwifi_station_config *config, int count);

	/**
	 * save_config - save sta config to nvram
	 * @config: bwifi_station_config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*save_config)(struct bwifi_station_config *config);

	/**
	 * get_saved_config - get saved sta config
	 * @config: bwifi_station_config array
     * @count: max number of bwifi_station_config in config array
	 *
	 * Returns: the number of saved config
	 */
    int (*get_saved_config)(struct bwifi_station_config *config, int count);

	/**
	 * get_saved_ssid - get saved config of specific ssid
     * @ssid: specific ssid
     * @config: bwifi_station_config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*get_saved_ssid)(const char *ssid, struct bwifi_station_config *config);

	/**
	 * del_all_config - delete all sta config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*del_all_config)(void);

	/**
	 * del_config - delete the specific config
     * @config: specific config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*del_config)(struct bwifi_station_config *config);

	/**
	 * scan - start to scan
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*start_scan)(void);

	/**
	 * config_scan - config specific scan
     * @scan_config: scan config
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*config_scan)(struct bwifi_scan_config *scan_config);

	/**
	 * get_scan_result - get scan result
     * @result: scan result buffer
     * @count: max number of bwifi_bss_info in scan result buffer
	 *
	 * Returns: the number of scanned bss
	 */
    int (*get_scan_result)(struct bwifi_bss_info *result, int count);

	/**
	 * flush_scan_res - flush all scan results
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*flush_scan_res)(void);

	/**
	 * connect - start to connect
	 *
	 * Returns: 0 - no suitable network found, 1 - connect success, other - connect fail
	 */
    int (*start_connect)(void);

	/**
	 * network_enable - enable all network config to connect with
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*network_enable)(uint32_t enable);

	/**
	 * network_enable - enable specific network config to connect with
     * @nid: network id
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*network_id_enable)(int nid, uint32_t enable);

	/**
	 * wps_bss_find - find wps enabled bss
     * @bss: bwifi_bss_info
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*wps_bss_find)(struct bwifi_bss_info * bss);

	/**
	 * wps_connect - start wps connection
     * @bssid: specific bssid to connect
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*wps_connect)(uint8_t *bssid);

	/**
	 * disconnect - disconnect current connection
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*disconnect)(void);

	/**
	 * get_current_ssid - get the ssid of current connection
     * @ssid: ssid buffer, 32 ascii chars with '0' end
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*get_current_ssid)(char *ssid);

	/**
	 * get_current_bssid - get the bssid of current connection
     * @bssid: bssid buffer
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*get_current_bssid)(uint8_t *bssid);

	/**
	 * get_own_mac - get sta mac address
     * @addr: mac address buffer
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*get_own_mac)(uint8_t *addr);

	/**
	 * get_current_rate - get current tx rate
	 *
	 * Returns: rate index
     * bit 31: MCS rate flag
     * bit 30:ShortGI flag
     * bit 29~26: BW (0:20M, 1:40M, other:reserved)
     * bit 25-0: MCS index
	 */
    int (*get_current_rate)(void);

	/**
	 * get_station_linkinfo - get link info of current sta connection
     * @info: bwifi_station_linkinfo buffer
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*get_station_linkinfo)(bwifi_station_linkinfo *info);

	/**
	 * get_current_rssi - get current rssi
	 *
	 * Returns: rssi (dbm)
	 */
    int8_t (*get_current_rssi)(void);

	/**
	 * set_ifa_ip - set ip address of specific interface
     * @dev_no: device interface index
     * @ip: hex format ip address
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*set_ifa_ip)(uint8_t dev_no, uint32_t *ip);

	/**
	 * work_channel_notify - notify current work channel
     * @ch: channel number
	 *
	 * Returns: 0 - success, other - failure
     * Need to notify work channel to wifi hal to use the most suitable configuration
     * e.g. dpd parameter, coexistance mode, etc.
	 */
    int (*work_channel_notify)(uint8_t ch);

	/**
	 * traffic_stat_en - enable wifi traffic statistic
     * @en: enable/disable statistic
     * @interval_sec: the interval of statistic (seconds)
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*traffic_stat_en)(uint8_t en, uint8_t interval_sec);

	/**
	 * traffic_stat_get - get the latest traffic statistic
     * @stat: statistic data buffer
     * @clear: indicate if need to clear statistic data
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*traffic_stat_get)(bwifi_trans_stat *stat, uint8_t clear);

	/**
	 * init - wifi subsystem initialize
	 * @flag: bit7~4 indicate WIFI_BAND: 0 = 2.4G; 1 = 5G; 2 = dual_band,
	 *        bit3~0 indicate WIFI_RUN_MODE: 0 = normal; 1 = nonsignaling; 2 = wifi reset
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*init)(uint8_t flag);

	/**
	 * power_on - set power status of wifi subsystem
	 * @onoff: 1 - power on, 0 - power off
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*power_on)(uint8_t onoff);

	/**
	 * ps_mode_set - set wifi power saving mode
	 * @enable: 0 - disable lmac ps mode, 1 - enable lmac ps mode
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*ps_mode_set)(int enable);

	/**
	 * low_power_set - set wifi low power mode
	 * @enable: 0 - disable umac ps mode, 1 - enable umac ps mode
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*low_power_set)(int enable);

#ifdef __AP_MODE__
	/**
	 * softap_start - start ap interface
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_start)(void);

	/**
	 * softap mode disconnect a station
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_disconnect_sta)(uint8_t *ucMac);

	/**
	 * softap_stop - stop ap interface
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_stop)(void);

	/**
	 * softap_config_set - set softap config parameters
	 * @info: config parameters
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_config_set)(struct bwifi_ap_info *info);

	/**
	 * softap_peer_num_set - set the number of peer STAs that softap allow to connect
     * @num: max sta number
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_peer_num_set)(int num);

	/**
	 * softap_country_code_set - set the country code of working region
     * @country_code: courntry code of regulation domain
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_country_code_set)(char *country_code);

	/**
	 * softap_vendor_elements_set - set vendor ie of softap
     * @ie: information element buffer
     * @len: the length of information element
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*softap_vendor_elements_set)(const uint8_t *ie, size_t ie_len);

	/**
	 * softap_peer_query - query connected peer address list
     * @mac_list: peer address buffer
     * @count: max peer count in mac_list
	 *
	 * Returns: the number of connected peer sta(s)
	 */
    int (*softap_peer_query)(struct bwifi_mac_addr *mac_list, int count);
#endif

	/**
	 * channel_switch - switch channel in no signal mode
     * @mode: interface mode, 0 - softap interface, 1 - sta interface
     * @channel: channel nunber
     * @snd_offset: secondary channel offset to center freq, 0 - no snd ch, 1 - upper 10M, -1 - lower 10M
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*channel_switch)(uint8_t mode, uint8_t channel, int8_t snd_offset);

	/**
	 * set_country_code - set country code of regulation domain
     * @country: country code represented by two ascii characters ended with '\0'
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*set_country_code)(char *country);

	/**
	 * get_country_code - get country code of regulation domain
     * @country: country code represented by two ascii characters ended with '\0'
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*get_country_code)(char *country);

	/**
	 * send_mgmt_frame - send mgmt frame on specific channel
     * @data: frame buffer
     * @len: frame length
     * @noack: indicate if ack needed
     * @chan: channel number to send
	 *
	 * Returns: 0 - success, other - failure
	 */
    int (*send_mgmt_frame)(uint8_t dev_no, const uint8_t *data, size_t len, int noack, uint8_t chan);

	/**
	 * data_rate_set - set the rate to send data frame
     * @rate_idx: rate index
     *   0 ~ 3:   DSSS/CCK rates: 1, 2, 5.5, 11;
     *   4 ~ 5:   reserved;
     *   6 ~ 13:  legacy OFDM rates: 6, 9, 12, 18, 24, 36, 48, 54;
     *   14 ~ 21: 1*1 MCS rates: MCS0 ~ MCS7;
     *   0xff:    auto-adjusting mode (default)
	 * Returns: 0 - success, other - failure
	 */
    int (*data_rate_set)(uint32_t rate_idx);

#ifdef ENABLE_FW_KEEP_ALIVE
    /*
     * Set ip keep alive feature's parameters.
     * @paras ip alive parameters;
     * Returns: the idx of the tcp stream when adding one or the delete result when deleting one;
     */
    int (*set_ip_alive)(struct ip_alive_paras *paras);
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
    int (*set_multicast_filter)(struct multicast_filter_paras *paras);
#endif

#ifdef __SET_BROADCAST_FILTER__
    /*
    * Enable/disable broadcast filter.
    *
    * Returns: BWIFI_R_OK on success, others on failure;
    */
    int (*set_broadcast_filter)(u8 enable);
#endif

#ifdef __SET_ARP_OFFLOAD__
    /*
    * Enable/disable arp offload.
    *
    * Returns: BWIFI_R_OK on success, others on failure;
    */
    int (*set_arp_offload)(u32 ipv4_addr);
#endif

	/**
	 * epta_param_config - set epta parameter for wifi/bt TDD coexistance 
     * @wifi_dur: wifi duration (ms) 
     * @bt_dur: bt duration (ms)
     * @mode: epta work mode
     *   0: periodic arbitrate by software, using wifi_dur and bt_dur to alloc active time for wifi and bt,
     *       wifi_dur + bt_dur should equal the 100ms period time
     *   1: arbitrate by hw itself
	 * Returns: 0 - success, other - failure
	 */
    int (*epta_param_config)(int wifi_dur, int bt_dur, int mode);

	/**
	 * set_epta_param - notify wifi state to epta
     * @state: wifi connection state, 0 - disconnected, 1 - connected
	 * Returns: 0 - success, other - failure
	 */
    int (*epta_state_notify)(enum bwifi_epta_state state);

	/**
	 * reset - wifi subsystem reset
	 * Returns: 0 - success, other - failure
	 */
    int (*reset)(void);

	/**
	 * data_recv_cb_set - set callback to handle ethernet frame
	 * @hdl: call back func pointer
	 * Returns: 0 - success, other - failure
	 */
    int (*data_recv_cb_set)(eth_input_handler hdl);

#ifdef __SNIFFER_MODE__
	/**
	 * sniffer_recv_cb_set - set callback to handle sniffer frame
	 * @hdl: call back func pointer
	 * Returns: 0 - success, other - failure
	 */
    int (*sniffer_recv_cb_set)(sniffer_recv_handler hdl);

	/**
	 * sniffer cmd - send sniffer cmd.
	 * @cmd: cmd id.
	 * @param: match with cmd id.
	 * Returns: 0 - success, other - failure
	 */
    int (*sniffer_cmd)(int cmd, void *param);
#endif

#ifdef CSI_REPORT
	/**
	 * csi_recv_cb_set - set callback to handle csi info
	 * @hdl: call back func pointer
	 * Returns: 0 - success, other - failure
	 */
    int (*csi_recv_cb_set)(csi_recv_handler hdl);
#endif

	/**
	 * eth_cb_set - set callback to handle ethernet frame
	 * @hdl: call back func pointer
	 * Returns: 0 - success, other - failure
	 */
    uint8_t **(*get_send_buf)(void);

	/**
	 * eth_cb_set - set callback to handle ethernet frame
	 * @hdl: call back func pointer
	 * Returns: 0 - success, other - failure
	 */
    int (*data_send)(uint8_t devnum, uint8_t **tx_buf, uint16_t tx_len);

	/**
	 * event_cb_set - set callback to handle event
	 * @hdl: call back func pointer
	 * Returns: 0 - success, other - failure
	 */
    int (*event_cb_set)(evt_recv_handler hdl);

	/**
	 * str_cmd - send string command to wifi subsystem
	 * @type: command type, 0 - wifi rf test, other - reserved
	 * @cmd_buf: command string ended with '\0'
	 * @rsp_buf: response string ended with '\0'
	 * @rsp_size: size of buffer to hold the rsp_buf's content
	 * Returns: 0 - success, other - failure
	 */
    int (*str_cmd)(uint8_t type, uint8_t *cmd_buf, uint8_t *rsp_buf, uint32_t rsp_size);

    /**
     * dat_cmd - send data command to wifi subsystem
     * @tx_buf: pointer of the data buffer to be sent.
     * @tx_len: length of the data to be sent.
     * Returns: 0 - success, other - failure
     */
    int (*dat_cmd)(uint8_t *tx_buf, uint16_t tx_len);

};

extern struct bwifi_hal_ops whops;
#define BWIFI_HAL_API_CALL(func,...)	((whops.func && (!whops.power_status || ((void *)whops.func == (void *)whops.power_on))) ? (int)whops.func(__VA_ARGS__) : -1)

extern struct wifi_init_param wifi_param;

#ifdef __cplusplus
}
#endif

#endif
