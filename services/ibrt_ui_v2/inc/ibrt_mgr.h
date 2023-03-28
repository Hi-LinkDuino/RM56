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
#ifndef __IBRT_MGR_H__
#define __IBRT_MGR_H__

#include "app_tws_ibrt.h"
#include "ibrt_mgr_evt.h"

typedef struct {
    void (*ibrt_global_state_changed)(ibrt_global_state_change_event *state);
    void (*ibrt_a2dp_state_changed)(const bt_bdaddr_t* addr, ibrt_conn_a2dp_state_change *state);
    void (*ibrt_hfp_state_changed)(const bt_bdaddr_t* addr, ibrt_conn_hfp_state_change *state);
    void (*ibrt_avrcp_state_changed)(const bt_bdaddr_t* addr, ibrt_conn_avrcp_state_change *state);
    void (*ibrt_tws_pairing_changed)(ibrt_conn_pairing_state state,uint8_t reason_code);
    void (*ibrt_tws_acl_state_changed)(ibrt_conn_tws_conn_state_event *state,uint8_t reason_code);
    void (*ibrt_mobile_acl_state_changed)(const bt_bdaddr_t *addr, ibrt_mobile_conn_state_event *state, uint8_t reason_code);
    void (*ibrt_sco_state_changed)(const bt_bdaddr_t *addr, ibrt_sco_conn_state_event *state,uint8_t reason_code);
    void (*ibrt_tws_role_switch_status_ind)(const bt_bdaddr_t *addr,ibrt_conn_role_change_state state,ibrt_role_e role);
    void (*ibrt_ibrt_state_changed)(const bt_bdaddr_t *addr, ibrt_connection_state_event* state, ibrt_role_e role,uint8_t reason_code);
    void (*ibrt_access_mode_changed)(btif_accessible_mode_t newAccessMode);
}ibrt_link_status_changed_cb_t;

typedef struct{
    void (*ibrt_pairing_mode_entry_func)();
    void (*ibrt_pairing_mode_exit_func)();
}ibrt_pairing_mode_changed_cb_t;

typedef struct{
    void (*ibrt_case_open_run_complete_callback)();
    void (*ibrt_case_close_run_complete_callback)();
}ibrt_case_event_complete_cb_t;

typedef void (*ibrt_vender_event_handler_ind)(uint8_t, uint8_t *, uint8_t);

/**@struct ibrt_mgr_config_t
* @brief ibrt mgr config struct
*/
typedef struct
{
    uint32_t rx_seq_error_timeout;
    uint32_t rx_seq_error_threshold;
    uint32_t rx_seq_recover_wait_timeout;
    uint32_t rssi_monitor_timeout;
    /// pairing mode timeout config
    uint32_t disable_bt_scan_timeout;
    uint32_t tws_conn_failed_wait_time;
    uint32_t connect_no_03_timeout;
    uint32_t disconnect_no_05_timeout;
    uint32_t tws_cmd_send_timeout;
    uint32_t tws_cmd_send_counter_threshold;

    /// freeman mode config, default should be false
    bool freeman_enable;
    /// tws earphone set the same addr, UI will be flexible, default should be true
    bool tws_use_same_addr;
    /// following cases the reconnect will be fail for freeman, please set to true if you want to reconnect successful:
    /// 1. freeman has link key but mobile deleted the link key
    /// 2. freeman changed its bt address after reboot and use the new address to reconnect mobile
    bool freeman_accept_mobile_new_pairing;
    /// for some proj no box key, default should be false
    bool enter_pairing_on_empty_mobile_addr;
    ///
    bool paring_mode_support;

    /// support single earphone entry pairing mode
    bool single_earbud_entry_pairing_mode;

    /// disconnect all remote device if this configure is true
    bool paring_with_disc_remote_dev;
    /// disconnect sco or not when accepting phone connection in pairing mode
    bool disc_sco_during_pairing;

    ///no need to reconnect mobile when easbuds in busy mode
    bool disallow_reconnect_in_streaming_state;

    /// for some proj no box key, default should be false
    bool enter_pairing_on_reconnect_mobile_failed;
    bool enter_pairing_on_reconnect_mobile_failed_once;
    /// for some proj no box key, default should be false
    bool enter_pairing_on_mobile_disconnect;
    bool disc_tws_before_reconnect_mobile;

    /// do tws switch when RSII value change, default should be true
    bool tws_switch_according_to_rssi_value;
    /// controller basband monitor
    bool lowlayer_monitor_enable;
    bool support_steal_connection;
    bool wear_updown_detect_supported;
    bool check_plugin_excute_closedbox_event;
    //if add ai feature
    bool ibrt_with_ai;
    bool suppport_io_capability;
    bool no_profile_stop_ibrt;

    bool tws_switch_tx_data_protect;

    bool without_reconnect_when_fetch_out_wear_up;
    /// do tws switch when rssi value change over threshold
    uint8_t rssi_threshold;
    /// do tws switch when RSII value change, timer threshold
    uint8_t role_switch_timer_threshold;
    uint8_t audio_sync_mismatch_resume_version;

    uint8_t  profile_concurrency_supported;
    uint8_t  connect_new_mobile_enable;

    /// close box debounce time config
    uint16_t close_box_event_wait_response_timeout;
    /// reconnect event internal config wait timer when tws disconnect
    uint16_t reconnect_wait_ready_timeout;
    uint16_t reconnect_mobile_wait_ready_timeout;
    uint16_t reconnect_tws_wait_ready_timeout;

    /// wait time before launch reconnect event
    uint16_t reconnect_mobile_wait_response_timeout;
    uint16_t reconnect_ibrt_wait_response_timeout;
    uint16_t nv_master_reconnect_tws_wait_response_timeout;
    uint16_t nv_slave_reconnect_tws_wait_response_timeout;

    /// open box reconnect mobile times config
    uint16_t open_reconnect_mobile_max_times;
    /// open box reconnect tws times config
    uint16_t open_reconnect_tws_max_times;
    /// connection timeout reconnect mobile times config
    uint16_t reconnect_mobile_max_times;
    /// connection timeout reconnect tws times config
    uint16_t reconnect_tws_max_times;

    /// connection timeout reconnect ibrt times config
    uint16_t reconnect_ibrt_max_times;
    uint16_t mobile_page_timeout;

    /// tws connection supervision timeout
    uint16_t tws_connection_timeout;
    uint16_t wait_time_before_disc_tws;

    uint16_t radical_scan_interval_nv_slave;
    uint16_t radical_scan_interval_nv_master;

    uint16_t scan_interval_in_sco_tws_disconnected;
    uint16_t scan_window_in_sco_tws_disconnected;

    uint16_t scan_interval_in_sco_tws_connected;
    uint16_t scan_window_in_sco_tws_connected;

    uint16_t scan_interval_in_a2dp_tws_disconnected;
    uint16_t scan_window_in_a2dp_tws_disconnected;

    uint16_t scan_interval_in_a2dp_tws_connected;
    uint16_t scan_window_in_a2dp_tws_connected;

    /// support max remote link count
    uint8_t support_max_remote_link;
    bool support_steal_connection_in_sco;
    bool allow_sniff_in_sco;
    bt_bdaddr_t new_mobile_address;
    bool always_interlaced_scan;
    uint8_t llmonitor_report_format;
    uint32_t llmonitor_report_count;

    bool is_changed_to_ui_master_on_tws_disconnected;
    bool allow_mobile_con_req_with_tws_connecting;
    bool ibrt_role_switch_control_by_customer;
} ibrt_mgr_config_t;

void ibrt_mgr_init();
ibrt_mgr_config_t* ibrt_mgr_get_config();
int ibrt_mgr_reconfig_env(ibrt_mgr_config_t *config);

void ibrt_mgr_event_entry(ibrt_mgr_evt_t evt);
void ibrt_mgr_notify_ibrt_event(ibrt_conn_evt_header * ev);
void ibrt_mgr_dump_info();
void ibrt_mgr_handle_vender_event(uint8_t evt_type, uint8_t * buffer, uint32_t length);

ibrt_box_state ibrt_mgr_get_local_box_state(uint16_t link_id);
ibrt_box_state ibrt_mgr_get_peer_box_state(uint16_t link_id);
ibrt_box_state ibrt_mgr_get_latest_box_state();
void ibrt_mgr_init_open_box_state_for_evb(void);
ibrt_box_state app_ibrt_mgr_get_box_state_by_box_evt(ibrt_mgr_evt_t box_evt);
ibrt_box_state app_ibrt_mgr_get_next_box_state(ibrt_box_state box_state,ibrt_mgr_evt_t box_evt);
void ibrt_mgr_set_box_state_only(ibrt_box_state box_state);
void ibrt_mgr_update_scan_type_policy(ibrt_update_scan_evt_type trigger_evt_type);
void ibrt_mgr_send_tws_reconnect_event(ibrt_mgr_evt_t reconect_event);
void ibrt_mgr_send_mobile_reconnect_event(uint8_t link_id);
void ibrt_mgr_choice_mobile_connect(const bt_bdaddr_t* mobile_addr);

uint8_t ibrt_mgr_get_connected_remote_dev_count();
bool ibrt_mgr_is_any_mobile_connected(void);
bool ibrt_mgr_is_maximum_mobile_dev_connected(void);

void ibrt_mgr_register_custom_ui_callback(ibrt_link_status_changed_cb_t* custom_ui_cb);
void ibrt_mgr_register_pairing_mode_changed_callback(ibrt_pairing_mode_changed_cb_t *cbs);
void ibrt_mgr_register_case_evt_complete_callback(ibrt_case_event_complete_cb_t *cbs);
void ibrt_mgr_register_vender_event_update_ind(ibrt_vender_event_handler_ind handler);
void ibrt_mgr_set_disc_dev_if_3rd_dev_request(const bt_bdaddr_t* remote_addr);
bool ibrt_mgr_event_has_been_queued(const bt_bdaddr_t* remote_addr,ibrt_mgr_evt_t event);
ibrt_mgr_evt_t ibrt_mgr_get_active_event(const bt_bdaddr_t* remote_addr);
void ibrt_mgr_start_exit_pairing_mode();
bool ibrt_mgr_high_priority_event_interrupt_reconnect();
bool ibrt_mgr_disallow_reconnect_mobile_by_peer_status(void);
bool ibrt_mgr_is_earbud_in_pairing_mode(void);
void ibrt_mgr_make_vaild_dev_replace_tmp_dev(const bt_bdaddr_t* addr,uint8_t link_id);


#endif
