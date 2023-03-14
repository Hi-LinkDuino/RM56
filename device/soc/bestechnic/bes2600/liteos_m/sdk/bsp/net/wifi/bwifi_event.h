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

#ifndef BWIFI_EVENT_H
#define BWIFI_EVENT_H
#ifdef  __cplusplus
extern "C"
{
#endif

typedef struct {
    int ap_num;                     /**< number of scanning APs */
    //struct dl_list *ap_list;      /**< list of APs found */
} BWIFI_EVENT_STAMODE_SCANDONE_T;

typedef enum {
    INTER_STATE_AUTHENTICATING,     /* Authentication start */
    INTER_STATE_AUTH_REJECT,        /* Authentication rejected by AP */
    INTER_STATE_AUTH_TIMEOUT,       /* Authentication timeout */
    INTER_STATE_ASSOCIATING,        /* Association start */
    INTER_STATE_ASSOC_REJECT,       /* Association request rejected by AP */
    INTER_STATE_ASSOC_TIMEOUT,      /* Association timeout */
    INTER_STATE_ASSOCIATED,         /* Associated to target AP */
    INTER_STATE_4WAY_HANDSHAKE,     /* 4way-handshake start */
    INTER_STATE_HANDSHAKE_FAIL,     /* 4way-handshake fail */
    INTER_STATE_GROUP_HANDSHAKE,    /* Group handshake start */
    /* CONNECTED */
} BWIFI_CONNECT_INTER_STATE;

typedef struct {
    uint8_t ssid[32];                    /**< SSID of the connecting AP */
    uint8_t ssid_len;                    /**< SSID length of the connecting AP */
    uint8_t bssid[6];                    /**< BSSID of the connecting AP */
    BWIFI_CONNECT_INTER_STATE state;/**< intermediate state during connection */
} BWIFI_EVENT_STAMODE_CONNECTING_T;

typedef struct {
    uint8_t ssid[32];                    /**< SSID of connected AP */
    uint8_t ssid_len;                    /**< SSID length of connected AP */
    uint8_t bssid[6];                    /**< BSSID of connected AP */
    uint8_t channel;                     /**< channel of connected AP */
} BWIFI_EVENT_STAMODE_CONNECTED_T;

typedef struct {
    uint8_t ssid[32];                    /**< SSID of disconnected AP */
    uint8_t ssid_len;                    /**< SSID length of disconnected AP */
    uint8_t bssid[6];                    /**< BSSID of disconnected AP */
    uint16_t reason;                     /**< reason of disconnection */
} BWIFI_EVENT_STAMODE_DISCONNECTED_T;

typedef struct {
    uint8_t old_mode;                    /**< the old auth mode of AP */
    uint8_t new_mode;                    /**< the new auth mode of AP */
} BWIFI_EVENT_STAMODE_AUTHMODE_CHANGE_T;

typedef struct {
    uint32_t ip;                   /**< IP address that station got from connected AP */
    uint32_t mask;                 /**< netmask that station got from connected AP */
    uint32_t gw;                   /**< gateway that station got from connected AP */
} BWIFI_EVENT_STAMODE_GOT_IP_T;

typedef struct {
    uint8_t ssid[32];                    /**< SSID of connected AP */
    uint8_t ssid_len;                    /**< SSID length of connected AP */
    uint8_t bssid[6];                    /**< BSSID of connected AP */
    uint8_t channel;                     /**< channel of connected AP */
    bool renew_dhcp;                     /**< whether renew dhcp after roaming */
} BWIFI_EVENT_STAMODE_ROAM_COMPLETE_T;

typedef struct {
    uint32_t pac_rxc_rx_buf_in_ptr;
    uint32_t pac_rxc_rx_buf_out_ptr;
    uint32_t scheduler_events;
    uint32_t lmac_pc0;
    uint32_t lmac_pc1;
    uint32_t lmac_lr;
    uint32_t lmac_sp;
    uint32_t pac_ntd_status_peek;
    uint32_t pac_txc_status;
    uint32_t QUEUE_0_CONTROL;
    uint32_t QUEUE_1_CONTROL;
    uint32_t QUEUE_2_CONTROL;
    uint32_t QUEUE_3_CONTROL;
    uint32_t wlan_sw_override_1;
    uint32_t tsq_in_prog;
    uint32_t tsq_in_cmpl;
} BWIFI_LMAC_STATUS_DUMP_T;

typedef struct {
    uint8_t rst_flag;                         /**< reset flag to indicate status:
                                              0 - no reset; 1 - reset start; 2 - reset end.*/
    uint16_t error_cause;                     /**< error cause (bitmask) */
    BWIFI_LMAC_STATUS_DUMP_T dump_info;  /**< LMAC status dump on fatal error */
} BWIFI_FATAL_ERROR_RESET_T;

typedef struct {
    uint16_t error_cause;                     /**< error cause (bitmask) */
    BWIFI_LMAC_STATUS_DUMP_T dump_info;  /**< LMAC status dump on fatal error */
} BWIFI_EVENT_LMAC_FATAL_ERROR_T;

typedef struct {
    uint8_t mac[6];                           /**< MAC address of the station connected or disconnected */
    uint16_t reason;                          /**< reason of disconnection */
} BWIFI_AP_STA_INFO_T;

typedef struct {
    uint8_t mac[6];                           /**< MAC address of the station connected to soft-AP */
//    uint8_t aid;                            /**< the aid that soft-AP gives to the station connected to */
} BWIFI_EVENT_SAPMODE_STA_CONNECTED_T;

typedef struct {
    uint8_t mac[6];                           /**< MAC address of the station disconnects to soft-AP */
    uint16_t reason;                          /**< reason of disconnection */
} BWIFI_EVENT_SAPMODE_STA_DISCONNECTED_T;

//typedef struct {
//    int rssi;		/**< Received probe request signal strength */
//    uint8_t mac[6];		/**< MAC address of the station which send probe request */
//} BWIFI_EVENT_SAPMODE_PROBEREQRECVED_T;

typedef struct {
    uint8_t mode;                           /**< wifi/bt coex mode, 0: tdd, 1: fdd */
} BWIFI_EVENT_COEX_MODE_T;

typedef struct {
    uint8_t status;                           /**< 1 low_pwer mode, 0 active mode */
} BWIFI_EVENT_LOW_POWER_T;

typedef union {
    BWIFI_EVENT_STAMODE_SCANDONE_T            scan_done;          /**< station scan (APs) done */
    BWIFI_EVENT_STAMODE_CONNECTING_T          connecting;         /**< station is connecting to AP */
    BWIFI_EVENT_STAMODE_CONNECTED_T           connected;          /**< station connected to AP */
    BWIFI_EVENT_STAMODE_DISCONNECTED_T        disconnected;       /**< station disconnected to AP */
    BWIFI_EVENT_STAMODE_AUTHMODE_CHANGE_T     auth_change;        /**< the auth mode of AP connected by station changed */
    BWIFI_EVENT_STAMODE_GOT_IP_T              got_ip;             /**< station got IP */
    BWIFI_EVENT_STAMODE_ROAM_COMPLETE_T       roam_complete;      /**< station roamed to a new AP */
    BWIFI_EVENT_SAPMODE_STA_CONNECTED_T       sta_connected;      /**< a station connected to soft-AP */
    BWIFI_EVENT_SAPMODE_STA_DISCONNECTED_T    sta_disconnected;   /**< a station disconnected to soft-AP */
    //BWIFI_EVENT_SAPMODE_PROBEREQRECVED_T      ap_probereqrecved;  /**< soft-AP received probe request packet */
    BWIFI_EVENT_COEX_MODE_T                   coex_mode;          /**< wifi/bt coex mode */
    BWIFI_EVENT_LMAC_FATAL_ERROR_T            fatal_err;          /**< LMAC fatal error */
    BWIFI_EVENT_LOW_POWER_T                   lower_power;        /**< low_power status */
} BWIFI_EVENT_INFO_U;

typedef enum {
    EVENT_STAMODE_SCAN_DONE                   = 0,  /**< station finish scanning AP */
    EVENT_STAMODE_CONNECTING                  = 1,  /**< station is connecting to AP */
    EVENT_STAMODE_CONNECTED                   = 2,  /**< station connected to AP */
    EVENT_STAMODE_DISCONNECTED                = 3,  /**< station disconnected to AP */
    EVENT_STAMODE_AUTHMODE_CHANGE             = 4,  /**< the auth mode of AP connected by station changed */
    EVENT_STAMODE_GOT_IP                      = 5,  /**< station got IP from connected AP */
    EVENT_STAMODE_DHCP_TIMEOUT                = 6,  /**< station dhcp client got IP timeout */
    EVENT_STAMODE_WPS_CONNECTED               = 7,  /**< WPS connected to AP */
    EVENT_STAMODE_ROAM_COMPLETE               = 8,  /**< station roamed to another AP in the same ESS */
    EVENT_SAPMODE_STA_CONNECTED               = 9,  /**< a station connected to soft-AP */
    EVENT_SAPMODE_STA_DISCONNECTED            = 10, /**< a station disconnected to soft-AP */
    //EVENT_SAPMODE_PROBEREQRECVED            = 11, /**< soft-AP received probe request packet */
    EVENT_COEX_MODE_CHANGE                          = 12, /**< switch wifi/bt coex mode between fdd and tdd */

    EVENT_LMAC_FATAL_ERROR                    = 20, /**< lower mac got a fatal error */
    EVENT_UMAC_CRASH                          = 21, /**< upper mac (cp) got crashed */
    EVENT_LOW_POWER_CHANGE                    = 22, /**<low power change status */
    EVENT_MAX
} BWIFI_SYSTEM_EVENT;

typedef struct bwifi_event {
    BWIFI_SYSTEM_EVENT event_id;      /**< even ID */
    BWIFI_EVENT_INFO_U event_info;    /**< event information */
} BWIFI_SYSTEM_EVENT_T;

/**
  * The Wi-Fi tcpip input handler.
  *
  * The example: if tcpip is lwip, set ethernetif_input func.
  *
  * @param uint16_t devnum , 0 is station, 1 is softap.
  * @param void *buf, rx pkt data buf.
  * @param int size, the length of param buf.
  * @return null
  */
typedef void (*eth_input_handler)(uint8_t devnum, void *buf, int size);

#ifdef __SNIFFER_MODE__
/**
  * The Wi-Fi sniffer recv handler.
  *
  * The example: if wifi is sniffer mode, set bes_sniffer_rx_handler func.
  *
  * @param uint8 *data, rx pkt data buf.
  * @param uint32 length, the length of param buf.
  * @param int signal, the pkt rssi.
  * @return null
  */
typedef void (*sniffer_recv_handler)(const u8 *data, u32 length, int signal);
#endif

#ifdef CSI_REPORT
/**
  * The Wi-Fi csi recv handler.
  *
  * The example: if wifi is test debug mode, set csi_recv_handler func.
  *
  * @param const void *buf, rx csi data buf.
  * @param uint32_t size, the length of param buf.
  * @return null
  */
typedef void (*csi_recv_handler)(const void *buf, uint32_t size);
#endif

/**
  * The Wi-Fi event handler.
  *
  * No complex operations are allowed in callback.
  * If users want to execute any complex operations, please post message to another task instead.
  *
  * @param BWIFI_SYSTEM_EVENT_T *event : WiFi event
  *
  * @return null
  */
typedef int (*evt_recv_handler)(BWIFI_SYSTEM_EVENT_T *event);


#ifdef  __cplusplus
}
#endif
#endif /*BWIFI_STATEMACHINE_H*/
