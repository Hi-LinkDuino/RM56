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
#ifndef __APP_TWS_IBRT__
#define __APP_TWS_IBRT__
#include "a2dp_api.h"
#include "cmsis_os.h"
#include "rfcomm_api.h"
#include "spp_api.h"
#include "hfp_api.h"
#include "bt_drv_reg_op.h"
#include "bluetooth.h"
#include "me_api.h"
#include "btapp.h"
//#include "bt_drv.h"

#define ACCESS_MODE_ARRAY_MAX           10
#define APP_IBRT_RECONNECT_TIMEOUT_MS   5

#define LETOHOST16(ptr)  (uint16_t)( ((uint16_t) *((uint8_t*)(ptr)+1) << 8) | \
                                ((uint16_t) *((uint8_t*)(ptr))) )

#define LETOHOST32(ptr)  (uint32_t)( ((uint32_t) *((uint8_t*)(ptr)+3) << 24) | \
                                ((uint32_t) *((uint8_t*)(ptr)+2) << 16) | \
                                ((uint32_t) *((uint8_t*)(ptr)+1) << 8)  | \
                                ((uint32_t) *((uint8_t*)(ptr))) )


#define UINT16LOW(num)   ((uint8_t)(num & 0xFF))
#define UINT16HIGH(num)  ((uint8_t)((num >> 8) & 0xFF))

#define APP_IBRT_SUPV_TO (0x640)

#define NV_RAM_RECORD_NUM   (10)

#define INVALID_ERROR        0xFF
#define INVALID_HANDLE       0xFFFF
#define BD_ADDR_LEN          6
#define DEVICE_INVALID_ID    0xFF


#define IBRT_DISABLE         0
#define IBRT_ENABLE          1
#define IBRT_SWITCH          2

#define IBRT_NONE_ROLE       0
#define IBRT_SNIFFER_ROLE    1
#define IBRT_FORWARD_ROLE    2

#define IBRT_TWS_SNIFF_MAX_INTERVAL (254)
#define IBRT_TWS_SNIFF_MIN_INTERVAL (254)
#define IBRT_TWS_SNIFF_TIMEOUT      (0)

#define IBRT_TWS_LINK_LARGE_DURATION           (12)

#define IBRT_TWS_LINK_DEFAULT_DURATION       (0x8)

#define IBRT_ENCRYPT_DISABLE 0x00
#define IBRT_ENCRYPT_ENABLE  0x01
#define IBRT_ENCRYPT_REFRESH 0x02

#define  IBRT_TWS_BT_TPOLL_STOP_IBRT           (40)

typedef uint8_t ibrt_link_mode_e;
#define IBRT_ACTIVE_MODE     0x00
#define IBRT_HOLD_MODE       0x01
#define IBRT_SNIFF_MODE      0x02
#define IBRT_PARK_MODE       0x03
#define IBRT_SCATTER_MODE    0x04

typedef enum
{
    NO_LINK_TYPE,
    SNOOP_LINK,
    MOBILE_LINK,
    TWS_LINK,
} ibrt_link_type_e;

typedef uint8_t ibrt_role_e;
#define   IBRT_MASTER       0
#define   IBRT_SLAVE        1
#define   IBRT_UNKNOW       0xff

typedef uint8_t sniff_req_direction_e;
#define   SNIFF_LOCAL_REQ        0
#define   SNIFF_REMOTE_REQ       1

/*
 * tx data protect in tws switch
 */
#ifndef __TWS_SWITCH_TX_DATA_PROTECT__
#define __TWS_SWITCH_TX_DATA_PROTECT__
#endif

typedef uint8_t tss_state_e;
#define  TSS_IDLE_STATE          (0x00)
#define  TSS_CRITICAL_WAIT_STATE (0x01)

#define  TSS_TX_WAIT_STATE       (0x02)
#define  TSS_TIMER_WAIT_STATE    (0x04)
#define  TSS_CMD_STATE           (0x08)
#define  TSS_SLAVE_WAIT_STATE    (0x10)

/*
 * wait some high priority criticals to be finished
 * such as sending accept response of avdtp start/suspend cmd
 * then launch tws switch
 */
#define  TSS_DELAY_CRITICAL_TIMEOUT_MS  50

/*
 * wait profile layer tx data to hci txbuf list
 * wait controller tx data to air
 */
#define  TSS_DELAY_HCI_CMD_TIMEOUT_MS  20



// hci tx buf type mapping
#define IBRT_HBT_TX_ACL     0x05  //#define HBT_TX_ACL   0x05
#define IBRT_HBT_TX_BLE     0x08  //#define HBT_TX_BLE   0x08

typedef  uint8_t   ibrt_controller_error_type;

#define  IBRT_CONTROLLER_TWS_NO_03      (1)
#define  IBRT_CONTROLLER_MOBILE_NO_03   (2)
#define  IBRT_CONTROLLER_TWS_NO_05      (3)
#define  IBRT_CONTROLLER_MOBILE_NO_05   (4)
#define  IBRT_CONTROLLER_DEAD           (5)
#define  IBRT_CONTROLLER_RX_SEQ_ERROR   (6)
#define  IBRT_MOBILE_ENCRYP_ERROR       (7)
#define  IBRT_TWS_BESAUD_CONNECT_ERROR  (8)

typedef enum
{
    TWS_INIT_TRIGGER,
    MOBILE_INIT_TRIGGER,
    START_IBRT_TRIGGER,
    EXCHANGE_PROFILE_TRIGGER,
    SCO_CONNECTED_TRIGGER,
    SCO_DISCONNECT_TRIGGER,
    MOBILE_MSS_TRIGGER,
} ibrt_trigger_link_policy_e;


typedef struct
{
    uint8_t  codec_type;
    uint8_t  sample_bit;
    uint8_t  sample_rate;
    uint8_t  vendor_para;
} __attribute__((packed)) ibrt_codec_t;

typedef struct
{
    uint8_t  backup_index;
    uint8_t  pickup_index;
    uint8_t  array_counter;
    btif_accessible_mode_t access_mode_array[ACCESS_MODE_ARRAY_MAX];
} access_mode_backup_t;

typedef struct
{
    uint8_t     vers;
    uint16_t    compid;
    uint16_t    subvers;
} __attribute__((packed)) rem_ver_t;

typedef enum
{
    AUDIO_CHANNEL_SELECT_STEREO,
    AUDIO_CHANNEL_SELECT_LRMERGE,
    AUDIO_CHANNEL_SELECT_LCHNL,
    AUDIO_CHANNEL_SELECT_RCHNL,
} AUDIO_CHANNEL_SELECT_E;

typedef enum {
    IBRT_ROLE_SWITCH_USER_AI    = (1 << 0),
    IBRT_ROLE_SWITCH_USER_OTA   = (1 << 1),
}IBRT_ROLE_SWITCH_USER_E;

/**
 * @brief The user of the timing control user
 *
 */
typedef enum
{
    TWS_TIMING_CONTROL_USER_SNIFF = 0,

    TWS_TIMING_CONTROL_USER_DEFAULT,

    TWS_TIMING_CONTROL_USER_A2DP,

    TWS_TIMING_CONTROL_USER_OTA,

    TWS_TIMING_CONTROL_USER_AI_VOICE,

    TWS_TIMING_CONTROL_USER_START_IBRT,

    TWS_TIMING_CONTROL_USER_IBRT_SWITCH,

    TWS_TIMING_CONTROL_USER_NUM,
} TWS_TIMING_CONTROL_USER_E;
//sorted by priority

typedef struct
{
    bool freeman_enable;
    bool suppport_io_capability;
    bool no_profile_stop_ibrt;
    uint8_t support_max_remote_link;
    bool lowlayer_monitor_enable;
    bool allow_sniff_in_sco;
    uint16_t reconnect_ibrt_max_times;
    bool is_changed_to_ui_master_on_tws_disconnected;
    uint8_t llmonitor_report_format;
    uint32_t llmonitor_report_count;
    bool allow_mobile_con_req_with_tws_connecting;
}ibrt_core_param_t;

typedef struct
{
    uint8_t default_tws_duration;
    uint8_t large_tws_duration;
    uint8_t audio_sync_mismatch_resume_version;
    bool    tws_switch_tx_data_protect;

    uint32_t tws_cmd_send_timeout;
    uint32_t tws_cmd_send_counter_threshold;

    uint16_t mobile_page_timeout;
    uint16_t tws_connection_timeout;
} ibrt_core_config_t;

typedef struct
{
    bt_bdaddr_t  addr;
    uint8_t      link_type;
    uint16_t     link_policy;
}link_policy_info_t;

typedef struct
{
    bt_bdaddr_t mobile_addr;
}exit_sniff_info_t;

typedef struct
{
    uint8_t     status;
    bt_bdaddr_t mobile_addr;
}start_ibrt_rsp_info_t;

typedef struct
{
    bt_bdaddr_t mobile_addr;
}start_ibrt_info_t;

typedef struct
{
    bt_bdaddr_t mobile_addr;
}set_env_info_t;

typedef struct
{
    uint8_t status;
    uint8_t opcode;
    uint8_t error_code;
    bt_bdaddr_t mobile_addr;
}stop_ibrt_rsp_t;

typedef struct
{
    uint8_t opcode;
    uint8_t error_code;
    bt_bdaddr_t mobile_addr;
    uint8_t ibrt_role;
}stop_ibrt_info_t;

typedef struct
{
    bt_bdaddr_t addr;
    uint16_t  mobile_conn_handle;
    uint16_t  ibrt_conn_handle;
    uint32_t constate;
} mobile_info_exchange_t;

typedef struct
{
    bt_bdaddr_t mobile_addr;
    uint8_t master_volume_change_trans_id;
    uint8_t master_avctp_ctl_trans_id;
    bt_bdaddr_t a2dp_last_paused_device;
}ibrt_tws_switch_data_sync_t;

#define RSSI_WINDOW_SIZE 30
typedef struct
{
    int8_t buf[RSSI_WINDOW_SIZE];
    uint8_t index;
} ibrt_rssi_window_t;

typedef struct
{
    bool     timing_enable;
    uint8_t  acl_slot_num;
    uint16_t acl_interval;
    uint16_t acl_interval_in_sco;
} TWS_TIMING_CONTROL_INFO_T;

typedef struct
{
    bt_bdaddr_t mobile_addr;
    ibrt_role_e current_role;
    uint16_t mobile_conhandle;
    uint16_t ibrt_conhandle;

    uint64_t mobile_constate;
    uint64_t ibrt_constate;

    uint64_t latest_disconnected_profile;

    osTimerDefEx_t delay_tws_switch_timer_def;
    osTimerId delay_tws_switch_cmd_timer_id;

    osTimerDefEx_t delay_profile_send_timer_def;
    osTimerId delay_profile_send_timer_id;

    osTimerDefEx_t wait_profile_ready_timer_def;
    osTimerId wait_profile_ready_timer_id;

    osTimerDefEx_t wait_profile_exchange_complete_timer_def;
    osTimerId wait_profile_exchange_complete_timer_id;

    bool wait_profile;
    uint16_t profile_exchange_state;
    uint32_t profile_exchange_delay;
    uint64_t rx_profile_update;
    uint64_t tx_profile_update;
    bool snoop_connected;
    bool profile_exchanged;
    bool a2dp_profile_exchanged;
    bool avrcp_profile_exchanged;
    tss_state_e tss_state;

    bool mobile_pair_canceled;
    bool mobile_encryp_done;

    uint8_t ibrt_disc_reason;

    //SCO sniffing connected before HFP profle restored
    uint8_t ibrt_sco_activate;
    hfp_sco_codec_t ibrt_sco_codec;

    uint8_t tws_switch_local_done: 1;
    uint8_t tws_switch_peer_done: 1;
    uint8_t master_tws_switch_pending: 1;
    uint8_t avrcp_register_notify_event;
    uint8_t avrcp_wait_register_notify_rsp;

    btif_remote_device_t *p_mobile_remote_dev;
    ibrt_codec_t a2dp_codec;
    void *p_mobile_a2dp_profile;
    void *p_mobile_hfp_profile;
    void *p_mobile_avrcp_profile;
    void *p_mobile_map_profile;
    void *p_mobile_reseved0_profile;
    void *p_mobile_reseved1_profile;
    ibrt_link_mode_e mobile_mode;
    uint16_t sniff_interval;
    uint8_t mobile_linkKey[16];
    bool phone_connect_happened;

    rssi_t raw_rssi;
    rssi_t peer_raw_rssi;
    ibrt_rssi_window_t rssi_window;

    bool sync_a2dp_status;
    uint64_t basic_profiles;
} ibrt_mobile_info_t;

typedef struct
{
    ibrt_role_e init_done;
    ibrt_role_e nv_role;
    uint8_t role_switch_debonce_time;
    uint16_t local_battery_volt;
    uint16_t peer_battery_volt;
    bt_bdaddr_t local_addr;
    bt_bdaddr_t peer_addr;
    uint8_t mobile_linkKey[16];
    uint8_t  ibrt_disc_reason;
    uint16_t tws_conhandle;
    uint16_t peer_tws_conhandle;
    uint16_t peer_mobile_conhandle;
    uint32_t tws_constate;
    uint32_t ibrt_ai_role_switch_handle;    //one bit represent a AI
    uint32_t ibrt_role_switch_handle_user;  //one bit represent a user
    osMailQId tws_mailbox;
    ibrt_codec_t a2dp_codec;
    uint32_t     audio_chnl_sel;
    void *p_mobile_map_profile;
    btif_remote_device_t *p_tws_remote_dev;
    btif_accessible_mode_t access_mode;
    bool access_mode_sending;
    bool is_ibrt_search_ui;
    uint8_t tws_switch_local_done: 1;
    uint8_t tws_switch_peer_done: 1;
    uint8_t master_tws_switch_pending: 1;
    uint8_t avrcp_register_notify_event;
    uint8_t avrcp_wait_register_notify_rsp;
    ibrt_link_mode_e tws_mode;
    access_mode_backup_t access_mode_backup;

    bool ibrt_in_poweroff;
    uint32_t tws_cmd_send_time;
    bool w4_stop_ibrt_for_rx_seq_error;
    bool dbg_state_timer_ongoing;
    bool stop_ibrt_timer_ongoing;
    bool ibrt_stopped_due_to_checker;
    ibrt_rssi_window_t tws_rssi_window;
    rssi_t raw_rssi;
    rssi_t peer_raw_rssi;

    ibrt_core_config_t config;
    ibrt_core_param_t custom_config;

    mobile_info_exchange_t peer_mobile_dev_info[BT_DEVICE_NUM];

    // tws timing control info
    TWS_TIMING_CONTROL_INFO_T *tws_timing_table;
    uint8_t  acl_slot_num;
    uint16_t acl_interval;
    uint16_t acl_interval_in_sco;

    uint8_t  default_acl_slot_num;
    uint16_t default_acl_interval;
    uint16_t default_acl_interval_in_sco;
    bool    isPeerMobileLinkExistButLocalNotOnTwsConnected;
} ibrt_ctrl_t;

typedef struct
{
    ibrt_role_e nv_role;
    bt_bdaddr_t local_addr;
    bt_bdaddr_t peer_addr;
    bt_bdaddr_t mobile_addr;
    uint32_t    audio_chnl_sel;
} ibrt_config_t;

typedef struct
{
    uint8_t length;
    uint8_t tws_name[251];
}tws_name_t;

typedef void* MobileInfoArg;
typedef bool (*tws_is_connected_t)(void);
typedef uint32_t (*spp_is_connected_t)(rfcomm_uuid_t uuid);
typedef bool (*reject_new_connection_callback_t)(void);


#ifdef __cplusplus
extern "C" {
#endif

void app_tws_ibrt_init(void);
void app_tws_ibrt_start(ibrt_config_t *config, bool is_search_ui);
void app_tws_ibrt_core_reconfig(ibrt_core_param_t *params,ibrt_core_config_t* configration);
void app_tws_ibrt_sync_sco_codec_info_when_needed(void);
ibrt_core_param_t* app_tws_ibrt_get_custom_config();
uint8_t app_tws_ibrt_support_max_remote_link();
void app_tws_ibrt_global_callback(const btif_event_t *event);
ibrt_link_type_e app_tws_ibrt_get_remote_link_type(const bt_bdaddr_t *p_addr);
void app_tws_ibrt_set_a2dp_codec_v2(ibrt_mobile_info_t *p_mobile_info, const a2dp_callback_parms_t *info);
void app_tws_ibrt_profile_callback(uint8_t device_id, uint64_t profile,void *param1,void *param2,void* param3);
void app_tws_ibrt_spp_callback(uint8_t device_id, void* spp_devi, void* spp_para);
ibrt_ctrl_t* app_tws_ibrt_get_bt_ctrl_ctx(void);
bool app_tws_ibrt_mobile_link_connected(const bt_bdaddr_t *mobile_addr);
void app_tws_is_connected_register(tws_is_connected_t func);
void app_spp_is_connected_register(spp_is_connected_t func);
bool app_tws_ibrt_tws_link_connected(void);
bool app_tws_is_connected(void);
bt_status_t app_tws_ibrt_create_tws_connection(uint16_t page_timeout);
bt_status_t app_tws_ibrt_create_mobile_connection(uint16_t mobile_page_timeout,bt_bdaddr_t *mobile_addr);
void app_tws_ibrt_proctect_bt_tx_type(uint16_t length);
bt_status_t app_tws_ibrt_disconnect_connection(btif_remote_device_t *remdev);
void app_tws_ibrt_clear_mobile_reconnecting(void);
void app_tws_ibrt_clear_tws_reconnecting(void);
void app_tws_ibrt_set_tws_reconnecting(void);
bool app_tws_ibrt_is_reconnecting_tws(void);
void app_bt_ibrt_reconnect_mobile_profile(bt_bdaddr_t *mobile_addr);
void app_tws_ibrt_reset_bdaddr_to_nv_original(void);
void app_tws_ibrt_set_bdaddr_to_nv_master(void);
bt_status_t app_tws_ibrt_init_access_mode(btif_accessible_mode_t mode);
bt_status_t app_tws_ibrt_set_access_mode(btif_accessible_mode_t mode);
bt_status_t app_tws_ibrt_do_mss_with_mobile(bt_bdaddr_t *p_mobile_addr);
bt_status_t app_tws_ibrt_write_link_policy(const bt_bdaddr_t *p_addr, btif_link_policy_t policy);
bt_status_t app_tws_ibrt_do_mss_with_tws(void);
bt_status_t app_tws_ibrt_exit_sniff_with_mobile(const bt_bdaddr_t *p_mobile_addr);
bt_status_t app_tws_ibrt_exit_sniff_with_tws(void);
btif_connection_role_t app_tws_ibrt_get_local_tws_role(void);
btif_connection_role_t app_tws_ibrt_get_local_mobile_role(bt_bdaddr_t *p_mobile_addr);
void app_tws_ibrt_delay_slave_create_connection(void const *para);
void app_tws_ibrt_store_mobile_link_key(const bt_bdaddr_t *addr,uint8_t *p_link_key);
ibrt_link_type_e app_tws_ibrt_get_link_type_by_addr(bt_bdaddr_t *p_addr);
bool app_tws_ibrt_slave_ibrt_link_connected(const bt_bdaddr_t *addr);
void app_tws_ibrt_disconnect_callback(const btif_event_t * event);
void app_tws_ibrt_sniff_timeout_handler(evm_timer_t *timer, unsigned int *skipInternalHandler);
void app_tws_ibrt_sniff_manager_callback(const btif_event_t *event);
void app_tws_ibrt_use_the_same_bd_addr(void);
btif_accessible_mode_t app_tws_ibrt_access_mode_pickup(void);
void app_tws_ibrt_access_mode_backup(btif_accessible_mode_t mode);
void app_tws_ibrt_sdp_disconnect_callback(const void *bd_addr);
uint8_t app_tws_ibrt_role_get_callback(const void *bd_addr);
const char* app_tws_ibrt_role2str(uint8_t role);
bool app_tws_ibrt_key_already_exist(void *bdaddr);
int app_ibrt_release_cmd_semphore(void);
int app_ibrt_clear_cmd_semphore(void);
void app_tws_ibrt_disconnect_all_connection_direct(void);
void app_tws_ibrt_disconnect_mobile(bt_bdaddr_t *mobile_addr);
void app_tws_ibrt_disconnect_all_connection(void);
bool app_tws_ibrt_get_phone_connect_happened(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_clear_phone_connect_happened(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_set_phone_connect_happened(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_hci_tx_buf_tss_process(void);
void app_tws_ibrt_delay_hci_tss_cmd_timer_cb(void const *para);
void app_tws_ibrt_switch_role(bt_bdaddr_t *p_bd_addr);
void app_tws_ibrt_set_tss_state(ibrt_mobile_info_t *p_mobile_info,tss_state_e state);
tss_state_e app_tws_ibrt_get_tss_state(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_reset_tss_state(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_set_controller_rx_flow_stop(ibrt_mobile_info_t *p_mobile_info );
void app_tws_ibrt_clear_controller_rx_flow_stop(ibrt_mobile_info_t *p_mobile_info);
uint8_t app_tws_ibrt_get_audio_voice_active_state(uint8_t device_id);
bool app_tws_ibrt_if_any_a2dp_streaming(void);
uint8_t app_tws_ibrt_sync_airmode_check_ind_handler(uint8_t voice_setting, bt_bdaddr_t *bdaddr);
void app_tws_ibrt_register_esco_auto_accept(void);
uint16_t app_tws_get_tws_conhdl(void);
ibrt_ctrl_t *app_tws_ibrt_get_ctrl_ctx(void);
bt_status_t app_tws_update_local_bt_addr(uint8_t* pBtAddr);
ibrt_role_e app_tws_get_ibrt_role(const bt_bdaddr_t *p_bd_addr);
bool app_tws_ibrt_is_profile_exchanged(const bt_bdaddr_t *addr);

//Extend for RAW UI
void app_tws_ibrt_start_role_switch(const bt_bdaddr_t *mobile_addr);
void app_tws_ibrt_reconfig_role(uint32_t role,uint8_t *pMaster, uint8_t *pSlaveAddr, bool isRightMasterSidePolicy);
void  app_tws_ibrt_set_env(ibrt_mobile_info_t* mobile_info);
bool app_tws_ibrt_set_env_needed(const bt_bdaddr_t *mobile_addr);
bool app_tws_ibrt_start_needed(const bt_bdaddr_t* mobile_addr);
bt_status_t app_tws_ibrt_tws_connect_request(uint32_t page_to);
bt_status_t app_tws_ibrt_connect_besaud();

uint32_t app_tws_ibrt_get_ibrt_connstate(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_set_ibrt_handle(ibrt_mobile_info_t *p_mobile_info, uint16_t ibrt_handle);
void app_tws_ibrt_set_mobile_mode(ibrt_mobile_info_t *p_mobile_info, ibrt_link_mode_e mobile_mode);
void app_tws_ibrt_set_ibrt_role(ibrt_mobile_info_t *p_mobile_info, ibrt_role_e ibrt_role);
void app_tws_ibrt_clear_mobile_pair_canceled(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_set_mobile_pair_canceled(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_clear_mobile_encryped(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_set_mobile_encryped(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_set_mobile_addr(ibrt_mobile_info_t *p_mobile_info, bt_bdaddr_t *p_bd_addr);
ibrt_role_e app_tws_get_ibrt_role(const bt_bdaddr_t *p_bd_addr);
uint16_t app_tws_ibrt_get_mobile_handle(const bt_bdaddr_t *p_bd_addr);
uint16_t app_tws_ibrt_get_ibrt_handle(const bt_bdaddr_t *p_bd_addr);
bt_status_t app_tws_ibrt_set_role(const bt_bdaddr_t *p_bd_addr, ibrt_role_e new_role);
void app_tws_ibrt_mobile_info_timer_init(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_mobile_info_init(const bt_bdaddr_t *addr,ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_free_mobile_info(ibrt_mobile_info_t* p_mobile_info);
bool app_tws_ibrt_mobile_pair_canceled(const bt_bdaddr_t *p_bd_addr);
bool app_tws_ibrt_rx_data_filter_callback(const void *bd_addr,uint8_t rx_filter_type,void*para);
void app_tws_set_ibrt_slave_sniff_timer_info(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_restart_ibrt_master_sniff_timer(bt_bdaddr_t *addr);
void app_tws_ibrt_bandwidth_table_register(TWS_TIMING_CONTROL_INFO_T *tws_timing_table);
void app_tws_ibrt_init_default_tws_bandwidth_config(uint8_t defaultAclSlotNum,
    uint16_t defaultAclInterval, uint16_t defaultAclIntervalInSco);
void app_tws_ibrt_request_modify_tws_bandwidth(TWS_TIMING_CONTROL_USER_E user, bool enable);
void app_tws_ibrt_bandwidth_table_clean();
void app_tws_ibrt_bandwidth_set_reconfig(void);
bool app_tws_ibrt_is_connected_with_wrong_peer(void);
void app_tws_ibrt_exit_sniff_for_all_mobile_device(void);
bool app_tws_ibrt_compare_btaddr(void);
uint8_t app_tws_ibrt_get_mobile_num_connected(void);
bool app_ibrt_any_basic_profiles_established(ibrt_mobile_info_t *p_mobile_info);
void app_tws_ibrt_register_is_reject_new_mobile_connection_callback(reject_new_connection_callback_t callback);
bool app_tws_ibrt_is_reject_new_mobile_connection(void);
void app_tws_ibrt_register_is_reject_tws_connection_callback(reject_new_connection_callback_t callback);
bool app_tws_ibrt_is_reject_tws_connection(void);
bool app_tws_ibrt_is_profile_initiator(ibrt_mobile_info_t *p_mobile_info,uint64_t profile);
void  app_tws_ibrt_sync_tws_name(uint8_t * tws_name, uint8_t length);
bool app_tws_is_orignial_address(uint8_t * address);

#ifdef __cplusplus
}
#endif

#endif/*__APP_TWS_IBRT__ */
