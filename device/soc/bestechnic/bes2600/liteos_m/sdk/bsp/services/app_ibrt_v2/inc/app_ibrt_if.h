/**
 ****************************************************************************************
 *
 * @file app_ibrt_if.h
 *
 * @brief APIs For Customer
 *
 * Copyright 2015-2019 BES.
 *
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
 ****************************************************************************************
 */

#ifndef __APP_IBRT_IF__
#define __APP_IBRT_IF__
#include "cmsis_os.h"
#include "app_tws_ibrt.h"
#include "app_key.h"
#include "app_ibrt_middleware.h"
#include "app_tws_ibrt_cmd_handler.h"
#ifdef IBRT_CORE_V2_ENABLE
#include "app_tws_ibrt_conn_api.h"
#endif

#ifdef IBRT_UI_V2
#include "ibrt_mgr.h"
#include "ibrt_mgr_evt.h"
#endif

#define APP_IBRT_UI_MOBILE_PAIR_CANCELED(addr)                 app_tws_ibrt_mobile_pair_canceled(addr)
#define APP_IBRT_UI_GET_MOBILE_CONNHANDLE(addr)                app_tws_ibrt_get_mobile_handle(addr)
#define APP_IBAT_UI_GET_CURRENT_ROLE(addr)                     app_tws_get_ibrt_role(addr)
#define APP_IBRT_UI_GET_IBRT_HANDLE(addr)                      app_tws_ibrt_get_ibrt_handle(addr)
#define APP_IBRT_MOBILE_LINK_CONNECTED(addr)                   app_tws_ibrt_mobile_link_connected(addr)
#define APP_IBRT_SLAVE_IBRT_LINK_CONNECTED(addr)               app_tws_ibrt_slave_ibrt_link_connected(addr)
#define APP_IBRT_IS_PROFILE_EXCHANGED(addr)                    app_ibrt_conn_is_profile_exchanged(addr)
#define APP_IBRT_UI_GET_MOBILE_CONNSTATE(addr)                 app_ibrt_conn_get_mobile_constate(addr)
#define APP_IBRT_UI_GET_IBRT_CONNSTATE(addr)                   app_ibrt_conn_get_ibrt_constate(addr)
#define APP_IBRT_UI_GET_MOBILE_MODE(addr)                      app_ibrt_conn_get_mobile_mode(addr)
#define APP_IBRT_IS_A2DP_PROFILE_EXCHNAGED(addr)               app_ibrt_a2dp_profile_is_exchanged(addr)

#ifdef IBRT_UI_V2
typedef ibrt_link_status_changed_cb_t  APP_IBRT_IF_LINK_STATUS_CHANGED_CALLBACK;
typedef ibrt_pairing_mode_changed_cb_t APP_IBRT_IF_PAIRING_MODE_CHANGED_CALLBACK;
typedef ibrt_case_event_complete_cb_t  APP_IBRT_IF_CASE_EVENT_COMPLETE_CALLBACK;
typedef ibrt_vender_event_handler_ind  APP_IBRT_IF_VENDER_EVENT_HANDLER_IND;
#endif
typedef tws_switch_disallow_cb         APP_IBRT_IF_ROLE_SWITCH_IND;

typedef enum {
    APP_IBRT_IF_STATUS_SUCCESS                  = 0,
    APP_IBRT_IF_STATUS_PENDING                  = 1,
    APP_IBRT_IF_STATUS_ERROR_INVALID_PARAMETERS = 2,
    APP_IBRT_IF_STATUS_ERROR_NO_CONNECTION      = 3,
    APP_IBRT_IF_STATUS_ERROR_CONNECTION_EXISTS  = 4,
    APP_IBRT_IF_STATUS_IN_PROGRESS              = 5,
    APP_IBRT_IF_STATUS_ERROR_DUPLICATE_REQUEST  = 6,
    APP_IBRT_IF_STATUS_ERROR_INVALID_STATE      = 7,
    APP_IBRT_IF_STATUS_ERROR_TIMEOUT            = 8,
    APP_IBRT_IF_STATUS_ERROR_ROLE_SWITCH_FAILED = 9,
    APP_IBRT_IF_STATUS_ERROR_UNEXPECTED_VALUE  = 10,
    APP_IBRT_IF_STATUS_ERROR_OP_NOT_ALLOWED    = 11,
} AppIbrtStatus;


typedef enum {
    APP_IBRT_IF_A2DP_IDLE             = 0,
    APP_IBRT_IF_A2DP_CODEC_CONFIGURED = 1,
    APP_IBRT_IF_A2DP_OPEN             = 2,
    APP_IBRT_IF_A2DP_STREAMING        = 3,
    APP_IBRT_IF_A2DP_CLOSED           = 4,
}AppIbrtA2dpState;

typedef enum {
    APP_IBRT_IF_AVRCP_DISCONNECTED   = 0,
    APP_IBRT_IF_AVRCP_CONNECTED      = 1,
    APP_IBRT_IF_AVRCP_PLAYING        = 2,
    APP_IBRT_IF_AVRCP_PAUSED         = 3,
    APP_IBRT_IF_AVRCP_VOLUME_UPDATED = 4,
}AppIbrtAvrcpState;

typedef enum {
    APP_IBRT_IF_HFP_SLC_DISCONNECTED = 0,
    APP_IBRT_IF_HFP_CLOSED           = 1,
    APP_IBRT_IF_HFP_SCO_CLOSED       = 2,
    APP_IBRT_IF_HFP_PENDING          = 3,
    APP_IBRT_IF_HFP_SLC_OPEN         = 4,
    APP_IBRT_IF_HFP_NEGOTIATE        = 5,
    APP_IBRT_IF_HFP_CODEC_CONFIGURED = 6,
    APP_IBRT_IF_HFP_SCO_OPEN         = 7,
    APP_IBRT_IF_HFP_INCOMING_CALL    = 8,
    APP_IBRT_IF_HFP_OUTGOING_CALL    = 9,
    APP_IBRT_IF_HFP_RING_INDICATION  = 10,
} AppIbrtHfpState;

typedef enum {
    APP_IBRT_IF_NO_CALL             = 0,
    APP_IBRT_IF_CALL_ACTIVE         = 1,
    APP_IBRT_IF_HOLD                = 2,
    APP_IBRT_IF_SETUP_INCOMMING     = 3,
    APP_IBRT_IF_SETUP_OUTGOING      = 4,
    APP_IBRT_IF_SETUP_ALERT         = 5,
} AppIbrtCallStatus;


#ifdef __cplusplus
extern "C" {
#endif

typedef ibrt_ctrl_t app_ibrt_if_ctrl_t;

enum APP_IBRT_IF_SNIFF_CHECKER_USER_T
{
    APP_IBRT_IF_SNIFF_CHECKER_USER_HFP,
    APP_IBRT_IF_SNIFF_CHECKER_USER_A2DP,
    APP_IBRT_IF_SNIFF_CHECKER_USER_SPP,

    APP_IBRT_IF_SNIFF_CHECKER_USER_QTY
};

#define TWS_LINK_START_IBRT_DURATION                        (8)
#define TWS_LINK_START_IBRT_INTERVAL                        (0xffff)
#define TWS_LINK_START_IBRT_INTERVAL_IN_SCO                 (0xffff)

#define app_ibrt_if_request_modify_tws_bandwidth  app_tws_ibrt_request_modify_tws_bandwidth

#define app_ibrt_if_exit_sniff_with_mobile(mobileAddr) app_ibrt_middleware_exit_sniff_with_mobile(mobileAddr)

#ifdef IBRT_UI_V2
void app_ibrt_if_register_custom_ui_callback(APP_IBRT_IF_LINK_STATUS_CHANGED_CALLBACK* cbs);
void app_ibrt_if_register_pairing_mode_callback(APP_IBRT_IF_PAIRING_MODE_CHANGED_CALLBACK* cbs);
void app_ibrt_if_register_case_event_complete_callback(APP_IBRT_IF_CASE_EVENT_COMPLETE_CALLBACK* cbs);
void app_ibrt_if_register_vender_handler_ind(APP_IBRT_IF_VENDER_EVENT_HANDLER_IND handler);
#endif

void app_ibrt_if_set_disc_dev_if_3rd_dev_request(const bt_bdaddr_t* remote_addr);
uint8_t app_ibrt_if_get_connected_remote_dev_count();

typedef enum {
    IBRT_BAM_NOT_ACCESSIBLE_MODE = 0x00,
    IBRT_BAM_DISCOVERABLE_ONLY = 0x01,
    IBRT_BAM_CONNECTABLE_ONLY = 0x02,
    IBRT_BAM_GENERAL_ACCESSIBLE = 0x03,
} ibrt_if_access_mode_enum;

typedef struct
{
    uint8_t eir[240];
} ibrt_if_extended_inquiry_response_t;

typedef struct
{
    uint16_t spec_id;
    uint16_t vend_id;
    uint16_t prod_id;
    uint16_t prod_ver;
    uint8_t  prim_rec;
    uint16_t vend_id_source;
} ibrt_if_pnp_info;

typedef struct {
    bt_bdaddr_t btAddr;
    uint8_t linkKey[16];
} ibrt_if_link_key_info;

typedef struct {
    int pairedDevNum;
    ibrt_if_link_key_info linkKey[MAX_BT_PAIRED_DEVICE_COUNT];
} ibrt_if_paired_bt_link_key_info;

// NULL will be returned if the remote device's dip info hasn't been gathered
ibrt_if_pnp_info* app_ibrt_if_get_pnp_info(bt_bdaddr_t *remote);
void app_ibrt_if_set_access_mode(ibrt_if_access_mode_enum mode);
void app_ibrt_if_set_extended_inquiry_response(ibrt_if_extended_inquiry_response_t *data);
void app_ibrt_if_enable_bluetooth(void);
void app_ibrt_if_disable_bluetooth(void);

void app_ibrt_if_stack_is_ready(void);
void app_ibrt_if_link_disconnected(void);
void app_ibrt_if_case_is_closed_complete(void);

#ifdef IBRT_UI_V2
void app_ibrt_if_config(ibrt_mgr_config_t *ui_config);
void app_ibrt_if_event_entry(ibrt_mgr_evt_t event);
#endif

void app_ibrt_if_enter_pairing_after_tws_connected(void);
void app_ibrt_if_enter_pairing_after_power_on(void);
void app_ibrt_if_enter_freeman_pairing(void);
void app_ibrt_if_nvrecord_config_load(void *config);
void app_ibrt_if_nvrecord_update_ibrt_mode_tws(bool status);
int app_ibrt_if_nvrecord_get_latest_mobiles_addr(bt_bdaddr_t *mobile_addr1, bt_bdaddr_t* mobile_addr2);

app_ibrt_if_ctrl_t *app_ibrt_if_get_bt_ctrl_ctx(void);

int app_ibrt_if_voice_report_trig_retrigger(void);
int app_ibrt_if_force_audio_retrigger(uint8_t retriggerType);
int app_ibrt_if_keyboard_notify_v2(bt_bdaddr_t *remote, APP_KEY_STATUS *status, void *param);
void app_ibrt_if_a2dp_lowlatency_scan(uint16_t interval, uint16_t window, uint8_t interlanced);
void app_ibrt_if_a2dp_restore_scan(void);
void app_ibrt_if_sco_lowlatency_scan(uint16_t interval, uint16_t window, uint8_t interlanced);
void app_ibrt_if_sco_restore_scan(void);
#ifdef IBRT_SEARCH_UI
void app_start_tws_serching_direactly();
void app_bt_manager_ibrt_role_process(const btif_event_t *Event);
#ifdef SEARCH_UI_COMPATIBLE_UI_V2
void app_ibrt_search_ui_init(bool boxOperation,ibrt_mgr_evt_t box_event);
#else
void app_ibrt_search_ui_init(bool boxOperation,ibrt_event_type evt_type);
#endif
void app_ibrt_enter_limited_mode(void);
void app_ibrt_reconfig_btAddr_from_nv();
#endif
void app_ibrt_if_ctx_checker(void);
int app_ibrt_if_sniff_checker_start(enum APP_IBRT_IF_SNIFF_CHECKER_USER_T user);
int app_ibrt_if_sniff_checker_stop(enum APP_IBRT_IF_SNIFF_CHECKER_USER_T user);
int app_ibrt_if_sniff_checker_init(uint32_t delay_ms);
int app_ibrt_if_sniff_checker_reset(void);

int app_ibrt_if_config_keeper_mobile_update(bt_bdaddr_t *addr);
int app_ibrt_if_config_keeper_tws_update(bt_bdaddr_t *addr);
void app_ibrt_if_pairing_mode_refresh(void);
bool app_ibrt_if_tws_switch_prepare_needed(uint32_t *wait_ms);

void app_ibrt_if_tws_swtich_prepare(uint32_t timeoutMs);
void app_ibrt_if_tws_switch_prepare_done_in_bt_thread(IBRT_ROLE_SWITCH_USER_E user, uint32_t role);
void app_ibrt_if_post_role_switch_handler(ibrt_mobile_info_t *p_mobile_info);

bool app_ibrt_if_start_ibrt_onporcess(const bt_bdaddr_t *addr);

void app_ibrt_if_get_tws_conn_state_test(void);
void app_ibrt_if_register_ibrt_cbs();
void app_ibrt_if_init_open_box_state_for_evb(void);
bool app_ibrt_if_is_maximum_mobile_dev_connected(void);
bool app_ibrt_if_is_any_mobile_connected(void);
uint8_t app_ibrt_if_get_support_max_remote_link();
void app_ibrt_if_dump_ibrt_mgr_info();
bt_status_t app_tws_if_ibrt_write_link_policy(const bt_bdaddr_t *p_addr, btif_link_policy_t policy);
void app_ibrt_if_sync_volume_info_v2(uint8_t device_id);
void app_ibrt_if_profile_connect(uint8_t device_id, int profile_id, uint32_t extra_data);
void app_ibrt_if_profile_disconnect(uint8_t device_id, int profile_id);


typedef enum {
    APP_IBRT_HFP_PROFILE_ID = 1,
    APP_IBRT_A2DP_PROFILE_ID = 2,
    APP_IBRT_AVRCP_PROFILE_ID = 3,
    APP_IBRT_HID_PROFILE_ID = 4,
} app_ibrt_profile_id_enum;



/**************************************APIs For Customer**********************************************/

/**
 ****************************************************************************************
 * @brief Connect hfp profile.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_connect_hfp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Connect a2dp profile.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_connect_a2dp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Connect avrcp profile.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_connect_avrcp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect hfp profile.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_disconnect_hfp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect a2dp profile.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_disconnect_a2dp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect avrcp profile.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_disconnect_avrcp_profile(uint8_t device_id);

/**
 ****************************************************************************************
 * @brief Connect hfp profile, only for ibrt master
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_master_connect_hfp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Connect a2dp profile, only for ibrt master
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_master_connect_a2dp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Connect avrcp profile, only for ibrt master
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_master_connect_avrcp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect hfp profile, only for ibrt master
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_master_disconnect_hfp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect a2dp profile, only for ibrt master
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_master_disconnect_a2dp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect avrcp profile, only for ibrt master
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_master_disconnect_avrcp_profile(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send media play command.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_a2dp_send_play(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send media pause command.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */

void app_ibrt_if_a2dp_send_pause(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send media forward command.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */

void app_ibrt_if_a2dp_send_forward(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send media backward command.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */

void app_ibrt_if_a2dp_send_backward(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send media volume up command.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_a2dp_send_volume_up(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send media volume down command.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_a2dp_send_volume_down(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Send the command to set media absolute volume.
 *
 * @param[in] device_id            device index
 * @param[in] volume               absolute volume value
 ****************************************************************************************
 */
void app_ibrt_if_a2dp_send_set_abs_volume(uint8_t device_id, uint8_t volume);


/**
 ****************************************************************************************
 * @brief Create an audio sco link.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_hf_create_audio_link(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Disconnect an audio sco link.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_hf_disc_audio_link(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief redial a call.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_hf_call_redial(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief answer a call.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_hf_call_answer(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief hangup a call.
 *
 * @param[in] device_id            device index
 ****************************************************************************************
 */
void app_ibrt_if_hf_call_hangup(uint8_t device_id);


/**
 ****************************************************************************************
 * @brief Set local volume up.
 ****************************************************************************************
 */
void app_ibrt_if_set_local_volume_up(void);


/**
 ****************************************************************************************
 * @brief Set local volume down.
 ****************************************************************************************
 */
void app_ibrt_if_set_local_volume_down(void);


/**
 ****************************************************************************************
 * @brief switch audio sco link.
 ****************************************************************************************
 */
void app_ibrt_if_switch_streaming_sco(void);


/**
 ****************************************************************************************
 * @brief switch streaming a2dp link.
 ****************************************************************************************
 */
void app_ibrt_if_switch_streaming_a2dp(void);

/**
 ****************************************************************************************
 * @brief tota printf data to remote device
 ****************************************************************************************
 */

#ifdef TOTA_v2
void app_ibrt_if_tota_printf(const char * format, ...);
void app_ibrt_if_tota_printf_by_device(uint8_t device_id, const char * format, ...);
#endif

/**
 ****************************************************************************************
 * @brief Get A2DP stream state of specific mobile device address.
 *
 * @param[in] addr                Specify the mobile device address to get A2DP stream state
 * @param[out] a2dp_state         Pointer at which pointer to indicate A2DP stream state
 *
 * @return An error status
 ****************************************************************************************
 */
AppIbrtStatus app_ibrt_if_get_a2dp_state(bt_bdaddr_t *addr, AppIbrtA2dpState *a2dp_state);


/**
 ****************************************************************************************
 * @brief Get AVRCP playback state of specific mobile device address.
 *
 * @param[in] addr                Specify the mobile device address to get AVRCP palyback state
 * @param[out] avrcp_state        Pointer at which pointer to indicate AVRCP palyback state
 *
 * @return An error status
 ****************************************************************************************
 */
AppIbrtStatus app_ibrt_if_get_avrcp_state(bt_bdaddr_t *addr,AppIbrtAvrcpState *avrcp_state);


/**
 ****************************************************************************************
 * @brief Get HFP state iof specific mobile device address.
 *
 * @param[in] addr               Specify the mobile device address to get HFP state
 * @param[out] hfp_state         Pointer at which pointer to indicate HFP state
 *
 * @return An error status
 ****************************************************************************************
 */
AppIbrtStatus app_ibrt_if_get_hfp_state(bt_bdaddr_t *addr, AppIbrtHfpState *hfp_state);


/**
 ****************************************************************************************
 * @brief Get HFP call status of specific mobile device address.
 *
 * @param[in] addr               Specify the mobile device address to get HFP Call status
 * @param[out] call_status       Pointer at which pointer to indicate HFP Call status
 *
 * @return An error status
 ****************************************************************************************
 */
AppIbrtStatus app_ibrt_if_get_hfp_call_status(bt_bdaddr_t *addr, AppIbrtCallStatus *call_status);


/**
 ****************************************************************************************
 * @brief Get current connected mobile device count and address list.
 *
 * @param[in] addr_list          Used to return current connected mobile device address list
 *
 * @return The number of current connected mobile
 ****************************************************************************************
 */
uint8_t app_ibrt_if_get_mobile_connected_dev_list(bt_bdaddr_t *addr_list);


/**
 ****************************************************************************************
 * @brief Check if the tws role switch is ongoing.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Role switch is ongoing
 * <tr><td>False  <td>Role switch is not ongoing
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_is_tws_role_switch_on(void);


/**
 ****************************************************************************************
 * @brief Perform the role switch operation.
 *
 * @return
 *      none
 ****************************************************************************************
 */
void app_ibrt_if_tws_role_switch_request(void);


/**
 ****************************************************************************************
 * @brief Get address list of the history paired mobiles.
 *
 * @param[in] mobile_addr_list       Address list of the history paired mobiles
 * @param[in] count                  Address count pointer
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Executed successfully
 * <tr><td>False  <td>Executed failed
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_nvrecord_get_mobile_addr(bt_bdaddr_t mobile_addr_list[],uint8_t *count);


/**
 ****************************************************************************************
 * @brief Clear all history paired mobile records in the flash.
 ****************************************************************************************
 */
void app_ibrt_if_nvrecord_delete_all_mobile_record(void);


/**
 ****************************************************************************************
 * @brief Get the history paired mobile records.
 *
 * @param[in] nv_record             Record list of the history paired mobiles
 * @param[in] count                 Record count pointer
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Executed successfully
 * <tr><td>False  <td>Executed failed
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_nvrecord_get_mobile_paired_dev_list(nvrec_btdevicerecord *nv_record,uint8_t *count);


/**
 ****************************************************************************************
 * @brief Check if the tws link is connected.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Executed successfully
 * <tr><td>False  <td>Executed failed
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_is_tws_link_connected(void);


/**
 ****************************************************************************************
 * @brief Get current connected mobile count.
 *
 * @return The number of current connected mobile
 ****************************************************************************************
 */
uint8_t app_ibrt_if_get_connected_mobile_count(void);


/**
 ****************************************************************************************
 * @brief Check if freeman mode is enable.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Freeman mode is enable
 * <tr><td>False  <td>Freeman mode is disable
 * </table>
 ****************************************************************************************
 */
uint8_t app_ibrt_if_is_in_freeman_mode(void);


/**
 ****************************************************************************************
 * @brief Disconnect tws link.
 *
 * @return Execute result status
 ****************************************************************************************
 */
ibrt_status_t app_ibrt_if_tws_disconnect_request(void);


/**
 ****************************************************************************************
 * @brief Disconnect mobile link.
 *
 * @param[in] addr              Address of the mobile to be disconnected
 * @param[in] post_func         Callback function when disconnecting is completed
 *
 * @return Execute result status
 ****************************************************************************************
 */
ibrt_status_t app_ibrt_if_mobile_disconnect_request(const bt_bdaddr_t *addr,ibrt_post_func post_func);


/**
 ****************************************************************************************
 * @brief Check if in tws pairing state.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>In tws pairing state
 * <tr><td>False  <td>Not in tws pairing state
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_is_tws_in_pairing_state(void);


/**
 ****************************************************************************************
 * @brief Check if earbud side is left.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Earbud side is left
 * <tr><td>False  <td>Earbud side is not left
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_is_left_side(void);


/**
 ****************************************************************************************
 * @brief Check if earbud side is right.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Earbud side is right
 * <tr><td>False  <td>Earbud side is not right
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_is_right_side(void);


/**
 ****************************************************************************************
 * @brief Check if earbud side is unknown.
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Earbud side is unknown
 * <tr><td>False  <td>Earbud side is not unknown
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_unknown_side(void);


/**
 ****************************************************************************************
 * @brief Set earbud side.
 *
 * @param[in] side               Only can be set to left or right side
 ****************************************************************************************
 */
#define app_ibrt_if_set_side    app_ibrt_middleware_set_side


/**
 ****************************************************************************************
 * @brief Get role of earbud.
 *
 * @return Role of earbud
 ****************************************************************************************
 */
TWS_UI_ROLE_E app_ibrt_if_get_ui_role(void);


/**
 ****************************************************************************************
 * @brief Check the IBRT UI role
 *
 * @return bool
 ****************************************************************************************
 */
bool app_ibrt_if_is_ui_slave(void);

/**
 ****************************************************************************************
 * @brief Convert role value to string.
 *
 * @param[in] uiRole                Role value of earbud
 *
 * @return Role string
 ****************************************************************************************
 */
const char* app_ibrt_if_uirole2str(TWS_UI_ROLE_E uiRole);


/**
 ****************************************************************************************
 * @brief Register user and its handling functions for specific tws information sync.
 *
 * @param[in] id                  User id
 * @param[in] user                Handling functions belonging to the user
 ****************************************************************************************
 */
void app_ibrt_if_register_sync_user(TWS_SYNC_USER_E id, TWS_SYNC_USER_T *user);


/**
 ****************************************************************************************
 * @brief Unregister user.
 *
 * @param[in] id                  User id
 ****************************************************************************************
 */
void app_ibrt_if_deregister_sync_user(TWS_SYNC_USER_E id);


/**
 ****************************************************************************************
 * @brief Initialize and make preparation for TWS synchronization.
 ****************************************************************************************
 */
void app_ibrt_if_prepare_sync_info(void);


/**
 ****************************************************************************************
 * @brief Flush pending tws sync information to peer device.
 ****************************************************************************************
 */
void app_ibrt_if_flush_sync_info(void);


/**
 ****************************************************************************************
 * @brief Fill tws information into pending sync list.
 *
 * @param[in] id                User id for tws information filling
 *
 * @return Status of the tws information filling
 ****************************************************************************************
 */
TWS_SYNC_FILL_RET_E app_ibrt_if_sync_info(TWS_SYNC_USER_E id);


/**
 ****************************************************************************************
 * @brief Write local bt address into factory section and activate it.
 *
 * @param[in] btAddr                Pointer of the bt local address
 ****************************************************************************************
 */
void app_ibrt_if_write_bt_local_address(uint8_t* btAddr);


/**
 ****************************************************************************************
 * @brief Get local bt address.
 *
 * @return Pointer of the bt local address to be returned
 ****************************************************************************************
 */
uint8_t *app_ibrt_if_get_bt_local_address(void);


/**
 ****************************************************************************************
 * @brief Get peer bt address.
 *
 * @return Pointer of the bt peer address to be returned
 ****************************************************************************************
 */
uint8_t *app_ibrt_if_get_bt_peer_address(void);


/**
 ****************************************************************************************
 * @brief Write local tws role and peer device bt address, the TWS pairing will be
 * immediately started.
 *
 * @param[in] role                  The tws role to update
 * @param[in] peerAddr              The peer device bt address to update
 ****************************************************************************************
 */
void app_ibrt_if_start_tws_pairing(ibrt_role_e role, uint8_t* peerAddr);


/**
 ****************************************************************************************
 * @brief Write local tws role and peer device bt address, the TWS pairing won’t be started.
 *
 * @param[in] role                  The tws role to update
 * @param[in] peerAddr              The peer device bt address to update
 ****************************************************************************************
 */
void app_ibrt_if_update_tws_pairing_info(ibrt_role_e role, uint8_t* peerAddr);

#ifdef IBRT_UI_V2
/**
 ****************************************************************************************
 * @brief Check current event have already been pushed into the queue
 *
 * @param[in] remote_addr        Remote device address
 * @param[in] event              The event which will be quiry
 *
 * @return
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Push event into the queue success
 * <tr><td>False  <td>Push event  into the queue failed
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_event_has_been_queued(const bt_bdaddr_t* remote_addr,ibrt_mgr_evt_t event);


/**
 ****************************************************************************************
 * @brief Get remote device current running event,such as case open/close,reconnect and so on.
 *
 * @param[in] remote_addr        Remote device address
 *
 * @return current running event
 ****************************************************************************************
 */
ibrt_mgr_evt_t app_ibrt_if_get_remote_dev_active_event(const bt_bdaddr_t* remote_addr);


/**
 ****************************************************************************************
 * @brief Convert event(hex) to string
 *
 * @param[in] type        event type
 *
 * @return string
 ****************************************************************************************
 */
const char* app_ibrt_if_ui_event_to_string(ibrt_mgr_evt_t type);

#endif

/**
 ****************************************************************************************
 * @brief Exit pairing mode
 ****************************************************************************************
 */
void app_ibrt_if_exit_pairing_mode();


/**
 ****************************************************************************************
 * @brief Freeman mode test
 ****************************************************************************************
 */
void app_ibrt_if_test_enter_freeman(void);


/**
 ****************************************************************************************
 * @brief Custom handler of the post device reboot
 *
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>True  <td>Execute successful
 * <tr><td>False  <td>Execute failed
 * </table>
 ****************************************************************************************
 */
bool app_ibrt_if_post_custom_reboot_handler(void);


/**
 ****************************************************************************************
 * @brief Get loal bt name
 *
 * @param[out] name_buf        Remote device name
 * @param[in]  max_size            maximum name length
 *
 * @return Execute result status
 ****************************************************************************************
 */
ibrt_status_t app_ibrt_if_get_local_name(uint8_t* name_buf, uint8_t max_size);


#ifdef PRODUCTION_LINE_PROJECT_ENABLED
/**
 ****************************************************************************************
 * @brief Open box test API for production line
 ****************************************************************************************
 */
void app_ibrt_if_test_open_box(void);


/**
 ****************************************************************************************
 * @brief Close box test API for production line
 ****************************************************************************************
 */
void app_ibrt_if_test_close_box(void);
#endif

#ifdef __GMA_VOICE__
void app_ibrt_gma_exchange_ble_key();
#endif

/**
 ****************************************************************************************
 * @brief Get the link key of mobile link
 ****************************************************************************************
 */
void app_ibrt_if_get_mobile_bt_link_key(uint8_t *linkKey1, uint8_t *linkKey2);

/**
 ****************************************************************************************
 * @brief Get the link key of TWS link
 ****************************************************************************************
 */
void app_ibrt_if_get_tws_bt_link_key(uint8_t *linkKey);

/**
 ****************************************************************************************
 * @brief Get the mobile link's service state
 ****************************************************************************************
 */
bool app_ibrt_if_is_audio_active(uint8_t device_id);

/**
 ****************************************************************************************
 * @brief Get active device's addr
 ****************************************************************************************
 */
bool app_ibrt_if_get_active_device(bt_bdaddr_t* device);

/**
 ****************************************************************************************
 * @brief Get all paired BT link key info
 ****************************************************************************************
 */
bool app_ibrt_if_get_all_paired_bt_link_key(ibrt_if_paired_bt_link_key_info *linkKey);

/**
 ****************************************************************************************
 * @brief Set mobile link tpoll value
 ****************************************************************************************
 */
void app_ibrt_if_update_mobile_link_qos(uint8_t device_id, uint8_t tpoll_slot);

/**
 ****************************************************************************************
 * @brief Check whether the address is a TWS address
 *
 * @param[in] pBdAddr        6 bytes of bt address
 *
 * @return true if it's tws address, otherwise false
 ****************************************************************************************
 */
bool app_ibrt_if_is_tws_addr(const uint8_t* pBdAddr);

/**
 ****************************************************************************************
 * @brief Get the latest newly paired mobile bt address.
 *
 * @return uint8_t* pointer of the bt address
 ****************************************************************************************
 */
uint8_t* app_ibrt_if_get_latest_paired_mobile_bt_addr(void);

/**
 ****************************************************************************************
 * @brief Clear the latest newly paired mobile bt address
 *
 * @return
 ****************************************************************************************
 */
void app_ibrt_if_clear_newly_paired_mobile(void);

/**
 ****************************************************************************************
 * @brief Initialize the newly paired mobile callback function
 *
 * @return
 ****************************************************************************************
 */
void app_ibrt_if_init_newly_paired_mobile_callback(void);

/**
 ****************************************************************************************
 * @brief Get the tws mtu size
 *
 * @return uint32_t
 ****************************************************************************************
 */
uint32_t app_ibrt_if_get_tws_mtu_size(void);

#define RX_DM1  12
#define RX_2DH1 18
#define RX_2DH3 20
#define RX_2DH5 22
#define HEC_ERR 24

typedef struct
{
    uint8_t hec_err;
    uint8_t crc_err;
    uint8_t fec_err;
    uint8_t grad_err;
    uint8_t ecc_cnt;
    uint8_t rx_dm1;
    uint8_t rx_2dh1;
    uint8_t rx_2dh3;
    uint8_t rx_2dh5;
    uint8_t rx_seq_err_cnt;
    uint8_t rev_fa_cnt;
    uint32_t last_ticks;
    uint32_t curr_ticks;
} __attribute__ ((__packed__)) ll_monitor_info_t;

typedef struct
{
    int8_t lastReceivedRssi;
    int8_t currentReceivedRssi;
    uint16_t AclPacketInterval;
    uint32_t AclPacketBtclock;
} __attribute__ ((__packed__)) acl_packet_interval_t;

#define ACL_PACKET_INTERVAL_THRESHOLD_MS            60
#define TOP_ACL_PACKET_INTERVAL_CNT 1

typedef struct
{
    uint8_t retriggerType;
    //uint8_t mobileChlMap[BT_DEVICE_NUM][10];
    int8_t mobile_rssi[BT_DEVICE_NUM];
    int8_t tws_rssi;
    uint32_t clock;
    acl_packet_interval_t acl_packet_interval[TOP_ACL_PACKET_INTERVAL_CNT];
    ll_monitor_info_t ll_monitor_info;

} __attribute__ ((__packed__)) connectivity_log_t;

#define RECORD_RX_NUM 5

typedef struct
{
    uint32_t clkn;
    int8_t rssi;
} __attribute__ ((__packed__)) rx_clkn_rssi_t;

typedef struct
{
    uint8_t disconnectReson;
    uint8_t disconnectObject;
    uint8_t lcState;
    uint8_t addr[6];
    uint8_t activeConnection;
    uint8_t mobileCurrMode[BT_DEVICE_NUM];
    uint8_t twsCurrMode;
    uint32_t mobileSniffInterval[BT_DEVICE_NUM];
    uint32_t twsSniffInterval;
    rx_clkn_rssi_t rxClknRssi[RECORD_RX_NUM];
} __attribute__ ((__packed__)) disconnect_reason_t;

typedef void (*reportConnectivityLogCallback_t)(connectivity_log_t* connectivity_log);
typedef void (*reportDisconnectReasonCallback_t)(disconnect_reason_t *disconnectReason);
typedef void (*connectivity_log_report_intersys_api)(uint8_t* data);
typedef void (*BSIR_event_callback_t)(uint8_t is_in_band_ring);
typedef void (*sco_disconnect_event_callback_t)(uint8_t *addr, uint8_t disonnectReason);
extern connectivity_log_report_intersys_api ibrt_if_report_intersys_callback;

void app_ibrt_if_report_connectivity_log_init(void);
void app_ibrt_if_register_report_connectivity_log_callback(reportConnectivityLogCallback_t callback);
void app_ibrt_if_register_report_disonnect_reason_callback(reportDisconnectReasonCallback_t callback);
void app_ibrt_if_save_bt_clkoffset(uint32_t clkoffset, uint8_t device_id);
void app_ibrt_if_disconnect_event(btif_remote_device_t *rem_dev, bt_bdaddr_t *addr, uint8_t disconnectReason, uint8_t activeConnection);
void app_ibrt_if_save_curr_mode_to_disconnect_info(uint8_t currMode, uint32_t interval, bt_bdaddr_t *addr);
void app_ibrt_if_update_rssi_info(const char* tag, rx_agc_t link_agc_info, uint8_t device_id);
void app_ibrt_if_update_chlMap_info(const char* tag, uint8_t *chlMap, uint8_t device_id);
void app_ibrt_if_report_audio_retrigger(uint8_t retriggerType);
void app_ibrt_if_report_connectivity_log(void);
void app_ibrt_if_update_link_monitor_info(uint8_t *ptr);
void app_ibrt_if_reset_acl_data_packet_check(void);
void app_ibrt_if_reset_tws_acl_data_packet_check(void);
void app_ibrt_if_check_acl_data_packet_during_a2dp_streaming(void);
void app_ibrt_if_check_tws_acl_data_packet(void);
bool app_ibrt_if_is_mobile_connhandle(uint16_t connhandle);
bool app_ibrt_if_is_tws_connhandle(uint16_t connhandle);
void app_ibrt_if_acl_data_packet_check_handler(uint8_t *data);
void app_ibrt_if_reg_disallow_role_switch_callback(APP_IBRT_IF_ROLE_SWITCH_IND cb);
void app_ibrt_if_register_BSIR_command_event_callback(BSIR_event_callback_t callback);
void app_ibrt_if_BSIR_command_event(uint8_t is_in_band_ring);
void app_ibrt_if_register_sco_disconnect_event_callback(sco_disconnect_event_callback_t callback);
void app_ibrt_if_sco_disconnect(uint8_t *addr, uint8_t disconnect_rseason);
void app_ibrt_if_a2dp_set_delay(a2dp_stream_t *Stream, uint16_t delayMs);
void app_ibrt_if_disconnect_mobile_links(void);
void app_ibrt_if_disconnect_specific_mobile(uint8_t device_id);
void app_ibrt_if_disonnect_rfcomm(struct spp_device *dev,uint8_t reason);
void app_ibrt_if_enable_hfp_voice_assistant(bool isEnable);
void app_ibrt_if_enter_non_signalingtest_mode(void);
void app_ibrt_if_bt_stop_inqury(void);
void app_ibrt_if_bt_start_inqury(void);
void app_ibrt_if_bt_set_local_dev_name(const uint8_t *dev_name, unsigned char len);
void app_ibrt_if_disconnect_all_bt_connections(void);
void app_ibrt_if_spp_write(struct spp_device *dev, char *buff, uint16_t length);
void app_ibrt_if_spp_open(struct spp_device *dev,btif_remote_device_t *rem_dev,spp_callback_t callback);
void app_ibrt_if_write_page_timeout(uint16_t timeout);
void app_ibrt_if_conn_tws_connect_request(bool isInPairingMode);
bool app_ibrt_if_is_peer_mobile_link_exist_but_local_not_on_tws_connected(void);
// to avoid corner cases, call this API to clear the flag after playing local connected prompt on the slave side
// when app_ibrt_if_is_peer_mobile_link_exist_but_local_not_on_tws_connected returns true
void app_ibrt_if_clear_flag_peer_mobile_link_exist_but_local_not_on_tws_connected(void);
void app_ibrt_if_register_is_reject_new_mobile_connection_query_callback(reject_new_connection_callback_t callback);
void app_ibrt_if_write_page_scan_setting(uint16_t window_slots, uint16_t interval_slots);
void app_ibrt_if_register_is_reject_tws_connection_callback(reject_new_connection_callback_t callback);

#ifdef IBRT_UI_V2
bool app_ibrt_if_is_earbud_in_pairing_mode(void);
#endif

void app_ibrt_if_reconnect_once(void);

#ifdef __cplusplus
}
#endif

#endif
