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
#ifndef __APP_BT_H__
#define __APP_BT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "me_api.h"
#include "a2dp_api.h"
#include "hfp_api.h"
#include "audio_policy.h"

enum APP_BT_REQ_T {
    APP_BT_REQ_ACCESS_MODE_SET,
    APP_BT_REQ_AUTO_TEST,
    APP_BT_DO_FUNCTION,
    APP_BT_REQ_NUM
};

#ifdef IBRT
typedef enum {
    RETRIGGER_BY_ROLE_SWITCH                  = 0,
    RETRIGGER_BY_DECODE_ERROR                 = 1,
    RETRIGGER_BY_DECODE_STATUS_ERROR          = 2,
    RETRIGGER_BY_ROLE_MISMATCH                = 3,
    RETRIGGER_BY_TRIGGER_FAIL                 = 4,
    RETRIGGER_BY_L_R_SYNC_DETECT              = 5,
    RETRIGGER_BY_SYNCHRONIZE_CNT_LIMIT        = 6,
    RETRIGGER_BY_LOW_BUFFER                   = 7,
    RETRIGGER_BY_SEQ_MISMATCH                 = 8,
    RETRIGGER_BY_AUTO_SYNC_NO_SUPPORT         = 9,
    RETRIGGER_BY_PLAYER_NOT_ACTIVE            = 10,
    RETRIGGER_BY_STATUS_ERROR                 = 11,
    RETRIGGER_BY_STREAM_RESTART               = 12,
    RETRIGGER_BY_SYNC_MISMATCH                = 13,
    RETRIGGER_BY_SYNC_FAIL                    = 14,
    RETRIGGER_BY_SYS_BUSY                     = 15,
    RETRIGGER_BY_SYNC_TARGET_CNT_ERROR        = 16,
    RETRIGGER_BY_AI_STREAM                    = 17,
    RETRIGGER_BY_IN_SNIFF                     = 18,
    RETRIGGER_BY_UNKNOW                       = 19,
    RETRIGGER_MAX                             = 20,
} AppIbrtAudioRetriggerType;
#endif

typedef uint8_t voice_report_role_t;
#define VOICE_REPORT_MASTER   0x00
#define VOICE_REPORT_SLAVE    0x01
#define VOICE_REPORT_LOCAL    0x02

voice_report_role_t app_ibrt_voice_report_get_role(void);

enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T {
    APP_BT_GOLBAL_HANDLE_HOOK_USER_0 = 0,
    APP_BT_GOLBAL_HANDLE_HOOK_USER_1,
    APP_BT_GOLBAL_HANDLE_HOOK_USER_QTY
};

typedef struct app_bt_search_result {
    bt_bdaddr_t *addr;
    const char *name;
    int name_len;
} app_bt_search_result_t;

#ifdef RESUME_MUSIC_AFTER_CRASH_REBOOT
typedef struct
{
    bt_bdaddr_t addr;
} app_bt_curr_palyback_device_t;
#endif

void app_bt_reconnect_a2dp_profile(bt_bdaddr_t *mobile_addr);
void app_bt_reconnect_hfp_profile(bt_bdaddr_t * mobile_addr);
void app_bt_reconnect_avrcp_profile(bt_bdaddr_t *mobile_addr);

void app_bt_disconnect_a2dp_profile(a2dp_stream_t * a2dp);
void app_bt_disconnect_avrcp_profile(btif_avrcp_channel_t * avrcp);
void app_bt_disconnect_hfp_profile(btif_hf_channel_t * hfp);

const char* app_bt_a2dp_get_all_device_streaming_state(void);
const char* app_bt_a2dp_get_all_device_state(void);
const char* app_bt_hf_get_all_device_state(void);
const char *app_bt_get_device_current_roles(void);

void app_bt_disconnect_link_by_id(uint8_t device_id);
void app_bt_disconnect_link_byaddr(bt_bdaddr_t * remote);

void app_bt_switch_to_non_prompt_disc_a2dp_play_mode(void);
void app_bt_switch_to_multi_a2dp_quick_switch_play_mode(void);

struct app_bt_profile_manager;

void app_bt_reset_profile_manager(struct app_bt_profile_manager *mgr);

void app_bt_a2dp_state_checker(void);
void app_bt_hfp_state_checker(void);

void app_bt_link_state_checker(void);
bool app_bt_checker_print_link_state(const char* tag, btif_remote_device_t *btm_conn);

void app_bt_audio_state_checker(void);
const char* app_bt_get_active_media_state(void);

void app_bt_a2dp_send_set_abs_volume(uint8_t device_id, uint8_t volume);

void app_bt_a2dp_send_key_request(uint8_t device_id, uint8_t a2dp_key);

void app_bt_local_volume_up(void (*cb)(uint8_t device_id));
void app_bt_local_volume_down(void (*cb)(uint8_t device_id));
void app_bt_local_volume_set(uint16_t volume, void (*cb)(uint8_t device_id));

void app_bt_set_a2dp_default_abs_volume(uint8_t volume);
void app_bt_update_a2dp_default_abs_volume(int device_id, uint8_t volume);
void app_bt_set_a2dp_current_abs_volume(int device_id, uint8_t volume);
uint8_t app_bt_get_a2dp_current_abs_volume(int device_id);
void app_bt_a2dp_current_abs_volume_just_set(int device_id, uint8_t volume);
void app_bt_a2dp_abs_volume_mix_version_handled(int device_id);

#ifdef IBRT
const char *app_bt_get_profile_exchanged_state(void);
void app_bt_ibrt_mobile_link_state_checker(void);
bool app_bt_ibrt_has_mobile_link_connected(void);
bool app_bt_ibrt_has_snoop_link_connected(void);
#endif

typedef void (*APP_BT_REQ_CONNECT_PROFILE_FN_T)(void *, void *);
typedef bt_status_t  (*APP_BT_REQ_HF_OP_FN_T)(void *);
typedef void (*APP_BT_GOLBAL_HANDLE_HOOK_HANDLER)(const btif_event_t*Event);
typedef void (*APP_BT_LE_READ_CHANNEL_MAP_CALLBACK) (void *);

#define app_bt_accessmode_set_req(accmode) do{app_bt_send_request(APP_BT_REQ_ACCESS_MODE_SET, accmode, 0, 0,0);}while(0)

typedef enum
{
    // always in active mode during a2dp streaming
    ACTIVE_MODE_KEEPER_A2DP_STREAMING = 0,
    // always in active mode during ai voice up-streaming
    ACTIVE_MODE_KEEPER_AI_VOICE_STREAM,
    // always in active mode during OTA
    ACTIVE_MODE_KEEPER_OTA,
    // stay in active mode for 15 seconds after role switch
    ACTIVE_MODE_KEEPER_ROLE_SWITCH,
    // stay in active mode during synchronous voice prompt playing
    ACTIVE_MODE_KEEPER_SYNC_VOICE_PROMPT,
    // always in active mode during phone call
    ACTIVE_MODE_KEEPEER_SCO_STREAMING,


} BT_LINK_ACTIVE_MODE_KEEPER_USER_E;

#ifdef __SOURCE_TRACE_RX__
typedef struct
{
    uint32_t index;
    bt_bdaddr_t addr;
    char name[256];
}device_info_t;
#endif

#define MAX_ACTIVE_MODE_MANAGED_LINKS       3
#define UPDATE_ACTIVE_MODE_FOR_ALL_LINKS    MAX_ACTIVE_MODE_MANAGED_LINKS

#ifdef USE_BT_ADAPTER
enum BT_ACL_CONN_TYPE{
    BT_ACL_CONNECT      = 0,
    BT_ACL_DISCONNECT   = 1
};

enum BT_PAIR_TYPE{
    APP_BT_PAIR_NONE      = 0,
    APP_BT_PAIRED         = 1
};

// type: ACL_CONNECT/ACL_DISCONNECT event;
// addr: peer device address;
// para: event reason. For ACL_CONNECT event, it is acl link status reason; For ACL_DISCONNECT event, it is disconnect 
// status reason.
typedef void (*bt_acl_conn_state_change_cb_t)(uint8_t type,bt_bdaddr_t* addr, uint32_t para);
typedef void (*bt_pair_state_change_cb_t)(bt_bdaddr_t* addr, uint32_t para);
extern bt_pair_state_change_cb_t g_bt_pair_state_callback;

void app_bt_acl_conn_callback_register(bt_acl_conn_state_change_cb_t cb);
void app_bt_acl_conn_callback_deregister(void);
void app_bt_pair_callback_register(bt_pair_state_change_cb_t cb);
void app_bt_pair_callback_deregister(void);

#endif

void app_bt_host_fault_dump_trace(void);

void app_bt_active_mode_manager_init(void);

void app_bt_active_mode_reset(uint32_t linkIndex);

void app_bt_get_remote_device_name(const bt_bdaddr_t * bdaddr);

void app_bt_inquiry_remote_device_name(const bt_bdaddr_t * bdaddr);

void app_bt_read_rssi_avg(bt_bdaddr_t * bdaddr);

void app_bt_active_mode_set(BT_LINK_ACTIVE_MODE_KEEPER_USER_E user, uint32_t linkIndex);

void app_bt_active_mode_clear(BT_LINK_ACTIVE_MODE_KEEPER_USER_E user, uint32_t linkIndex);

void app_bt_global_handle(const btif_event_t *Event);
bool is_app_bt_pairing_running(void);

void PairingTransferToConnectable(void);

void app_bt_global_handle_init(void);

void app_bt_opening_reconnect(void);

void app_bt_accessmode_set(  btif_accessible_mode_t mode);

int app_bt_send_request(uint32_t message_id, uint32_t param0, uint32_t param1, uint32_t param2,uint32_t ptr);

void app_bt_init(void);

int app_bt_state_checker(void);

void *app_bt_profile_active_store_ptr_get(uint8_t *bdAddr);

void app_bt_profile_connect_manager_opening_reconnect(void);

BOOL app_bt_profile_connect_openreconnecting(void *ptr);

int app_bt_global_handle_hook_set(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user, APP_BT_GOLBAL_HANDLE_HOOK_HANDLER handler);

APP_BT_GOLBAL_HANDLE_HOOK_HANDLER app_bt_global_handle_hook_get(enum APP_BT_GOLBAL_HANDLE_HOOK_USER_T user);

bool app_is_hfp_service_connected(uint8_t device_id);

#if defined(IBRT)
void app_bt_ibrt_reconnect_mobile_profile_flag_set(void);
void app_bt_ibrt_reconnect_mobile_profile_flag_clear(void);
bool app_bt_ibrt_reconnect_mobile_profile_flag_get(void);

void app_bt_ibrt_connect_mobile_a2dp_profile(const bt_bdaddr_t *addr);
void app_bt_ibrt_connect_mobile_hfp_profile(const bt_bdaddr_t *addr);
#endif
#if defined(GET_PEER_RSSI_ENABLE)
void app_bt_ibrt_rssi_status_checker(void);
#endif

void app_bt_stay_active(uint8_t deviceId);

void app_bt_allow_sniff(uint8_t deviceId);

btif_remote_device_t* app_bt_get_remoteDev(uint8_t deviceId);

void app_bt_conn_stop_sniff(btif_remote_device_t* currentRemDev);

void app_bt_stop_sniff(uint8_t deviceId);

void app_bt_sniff_config(btif_remote_device_t *remDev);

bool app_bt_has_mobile_device_in_sniff_mode(void);

bool app_bt_has_mobile_device_in_active_mode(void);

void app_bt_allow_sniff_rem_dev(btif_remote_device_t* pRemDev);

void app_bt_stay_active_rem_dev(btif_remote_device_t* pRemDev);

void app_check_pending_stop_sniff_op(void);

uint8_t app_bt_count_connected_device(void);

void app_bt_pause_media_player_again(uint8_t deviceId);

bool app_bt_is_music_player_working(uint8_t deviceId);

bool app_bt_pause_music_player(uint8_t deviceId);

void app_bt_resume_music_player(uint8_t deviceId);

bool app_bt_is_device_profile_connected(uint8_t deviceId);

bool app_bt_is_acl_connected(uint8_t device_id);

bool app_bt_is_sco_connected(uint8_t device_id);

bool app_bt_is_hfp_connected(uint8_t device_id);

uint8_t app_bt_get_device_id_byaddr(bt_bdaddr_t *remote);

bool app_bt_is_a2dp_connected(uint8_t device_id);

bool app_bt_is_acl_connected_byaddr(const bt_bdaddr_t *remote);

bool app_bt_is_sco_connected_byaddr(bt_bdaddr_t *remote);

bool app_bt_is_hfp_connected_byaddr(bt_bdaddr_t *remote);

bool app_bt_is_a2dp_connected_byaddr(bt_bdaddr_t *remote);

bool app_bt_is_a2dp_streaming(uint8_t deviceId);

void app_bt_suspend_a2dp_streaming(uint8_t deviceId);

bool app_bt_get_device_bdaddr(uint8_t deviceId, uint8_t* btAddr);

void fast_pair_enter_pairing_mode_handler(void);

bool app_bt_is_in_reconnecting(void);

bool app_bt_is_profile_connected_before(uint8_t device_id);

bool btapp_hfp_is_dev_sco_connected(uint8_t devId);

bool app_bt_is_in_connecting_profiles_state(void);

void app_bt_clear_connecting_profiles_state(uint8_t devId);

void app_bt_set_connecting_profiles_state(uint8_t devId);

#if defined(__INTERCONNECTION__)
btif_accessible_mode_t app_bt_get_current_access_mode(void);
#endif

bool app_bt_is_hfp_audio_on(void);

btif_remote_device_t* app_bt_get_connected_mobile_device_ptr(void);
bt_bdaddr_t* app_bt_get_remote_device_address(uint8_t device_id);
void app_bt_set_spp_device_ptr(btif_remote_device_t* device);
btif_remote_device_t* app_bt_get_spp_device_ptr(void);

bool app_is_disconnecting_all_bt_connections(void);

void app_set_disconnecting_all_bt_connections(bool isEnable);

void app_bt_start_search(void);

void app_bt_start_search_with_callback(void (*cb)(bt_bdaddr_t *remote), void (*result_cb)(app_bt_search_result_t *result));

void app_bt_del_search_except_device(const bt_bdaddr_t *addr);

void app_bt_add_search_except_device(const bt_bdaddr_t *addr);

void app_bt_clear_search_except_device_list(void);

typedef void (*inquiryResponseCallback_t)(uint8_t *btAddr, uint8_t *deviceName, uint8_t *eir);
/**
 ****************************************************************************************
 * @brief register inquiry_response_callback to get bt addr, device name and eir.
 *
 * @param[in] inquiryResponseCallback_t callback
 ****************************************************************************************
 */
void app_bt_register_inquiry_response_callback(inquiryResponseCallback_t callback);

/**
 ****************************************************************************************
 * @brief bt start inquiry.
 *
 * @param[in] inquiryLastingTime        Inquiry lasting time, uint is 1.28s.
                                        Range: 0x01 to 0x30
                                        Time(seconds) = inquiryLastingTime * 1.28s
                                        Range: 1.28 to 61.44 s
 ****************************************************************************************
 */
void app_bt_start_inquiry(uint8_t inquiryLastingTime);

/**
 ****************************************************************************************
 * @brief bt stop inquiry.
 *
 ****************************************************************************************
 */
void app_bt_stop_inquiry(void);

/**
 ****************************************************************************************
 * @brief bt set channel classification map which related Set AFH Host Channel Classification command
 * AFH_Host_Channel_Classification: Size: 10 octets (79 bits meaningful)
 * This parameter contains 80 1-bit fields
 * The nth such field (in the range 0 to 78) contains the value for channel n:
 *    0: channel n is bad
 *    1: channel n is unknown
 *
 * The most significant bit (bit 79) is reserved for future use
 * At least (Nmin == 20) channels shall be marked as unknown.
 * default all bits value is 1
 *
 ****************************************************************************************
 */
void app_bt_set_chnl_classification(uint8_t *chnl_map);

void app_bt_read_le_chnl_map(uint16_t conn_handle);
void app_bt_set_le_host_chnl_classification(uint8_t *chnl_map);
void app_bt_registe_read_le_chnl_map_callback(APP_BT_LE_READ_CHANNEL_MAP_CALLBACK cb);

#if defined(IBRT)
uint32_t app_bt_save_bd_addr_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_bd_addr_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_a2dp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_a2dp_app_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_avrcp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_avrcp_app_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_hfp_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_hfp_app_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_save_spp_app_ctx(uint64_t app_id,btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_spp_app_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len, uint64_t app_id);
#ifdef BT_MAP_SUPPORT
uint32_t app_bt_save_map_app_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
uint32_t app_bt_restore_map_app_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
#endif
bool app_bt_is_any_connection(void);
#endif

void app_stop_fast_connectable_ble_adv_timer(void);
int8_t app_bt_get_rssi(void);
void app_bt_print_buff_status(void);

#ifdef  TILE_DATAPATH
int8_t app_tile_get_ble_rssi(void);
#endif

#ifdef CTKD_ENABLE
bool app_bt_ctkd_is_connecting_mobile_pending(void);
void app_bt_ctkd_connecting_mobile_handler(void);
#endif

typedef bool (*app_factory_test_ind_handler_func)(uint8_t test_type);
void app_factory_register_test_ind_callback(app_factory_test_ind_handler_func cb);
#ifdef _SUPPORT_REMOTE_COD_
void app_bt_get_remote_cod(uint8_t *cod0, uint8_t *cod1);
bool app_bt_get_remote_cod_by_addr(const bt_bdaddr_t *bd_ddr, uint8_t *cod);
#endif
void app_bt_report_audio_retrigger(uint8_t retrgigerType);
void app_bt_reset_tws_acl_data_packet_check(void);
void app_bt_update_link_monitor_info(uint8_t *ptr);
void app_bt_acl_data_packet_check(uint8_t *data);
#ifdef RESUME_MUSIC_AFTER_CRASH_REBOOT
void app_bt_resume_music_after_crash_reboot(uint8_t device_id);
void app_bt_resume_music_after_crash_reboot_init(void);
void app_bt_reset_curr_playback_device(uint8_t device_id);
#endif
void app_bt_reset_rssi_collector(void);
int32_t app_bt_tx_rssi_analyzer(int8_t rssi);
void app_bt_sleep_init(void);
uint8_t app_bt_ibrt_mobile_link_num_get(void);
#ifdef LINK_KEY_ENCRYPT_BY_CUSTOMER
uint8_t *app_get_current_remote_device_name(void);
#endif

#ifdef __cplusplus
}
#endif
#endif /* BESBT_H */
