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
#ifndef __APP_TWS_IBRT_CORE_TYPE_H__
#define __APP_TWS_IBRT_CORE_TYPE_H__

#if defined(IBRT_CORE_V2_ENABLE)

#include "stdint.h"
#include "cmsis_os.h"

#include "app_ibrt_conn_evt.h"

#define  IBRT_UI_INVALID_RSSI                      (100)

#define  IBRT_UI_MIN_RSSI                          (-100)


typedef enum
{
    IBRT_UI_NO_ERROR,
    IBRT_UI_RSP_TIMEOUT,
    IBRT_UI_NOT_ACCEPT,
    IBRT_UI_CONNECT_FAILED,
    IBRT_UI_PAGE_TIMEOUT             = 0x04,
    IBRT_UI_STATUS_ERROR,
    IBRT_CONN_CONNECTION_TIMEOUT     = 0x08,
    IBRT_UI_ACL_ALREADY_EXIST        = 0x0B,
    IBRT_UI_CMD_DISALLOWED           = 0X0C,
    IBRT_UI_LIMITED_RESOURCE         = 0x0D,
    IBRT_UI_HOST_ACCEPT_TIMEOUT      = 0x10,
    IBRT_CONN_TERM_USER_REQ          = 0x13,
    IBRT_UI_CONN_TERM_LOW_RESOURCES  = 0x14,
    IBRT_CONN_TERM_BY_LOCAL_HOST     = 0x16,
    IBRT_CONN_ERROR_LMP_RSP_TIMEOUT  = 0x22,
    IBRT_UI_MOBILE_CONN_DISCONNECTED = 0X2A,
    IBRT_UI_TWS_CONN_DISCONNECTED    = 0X2B,
    IBRT_UI_SNOOP_DISCONNECTED       = 0X2C,
    IBRT_CONN_NO_PROFILE_CONNECTED   = 0x50,
    IBRT_UI_CONNECTION_INCOMING      = 0x99,
    IBRT_UI_EVT_STATUS_ERROR         = 0x9A,
    IBRT_UI_TWS_CMD_SEND_FAILED      = 0X9B,
    IBRT_UI_MOBILE_PAIR_CANCLED      = 0X9C,
    BT_LINK_REAL_DISCONNECTED        = 0xB8,
    IBRT_BT_CANCEL_PAGE              = 0xB9,
    IBRT_UI_SWITCH_IN_POCESS         = 0xBA,
    IBRT_LMP_TX_BUFFER_OVERFLOW      = 0xBD,
    IBRT_CONN_LINK_KEY_MISSING       = 0xF1,
} ibrt_conn_error_e;

typedef struct
{
    uint16_t battery_volt;
    uint16_t  mobile_conhandle;
    uint16_t  tws_conhandle;
    rssi_t   raw_rssi;
} app_ui_rssi_battery_info_t;

#define  IBRT_STOP_IBRT                         1

//HCI opcode
#define    IBRT_HCI_CREATE_CON_CMD_OPCODE       0x0405
#define    IBRT_HCI_EXIT_SNIFF_MODE_CMD_OPCODE  0x0804
#define    IBRT_HCI_SWITCH_ROLE_CMD_OPCODE      0x080B
#define    IBRT_HCI_STOP_IBRT_OPCODE            0xFCA8
#define    IBRT_HCI_START_IBRT_OPCODE           0xFCA3
#define    IBRT_HCI_RESET_OPCODE                0x0C03
#define    IBRT_HCI_DSIC_CON_CMD_OPCODE         0x0406
#define    IBRT_HCI_SET_ENV_CMD_OPCODE          0xFC8E


typedef enum
{
    OUTGOING_CONNECTION_REQ = 0,
    INCOMMING_CONNECTION_REQ,

}connection_direction_t;

typedef struct
{
    uint8_t   num_hci_cmd_packets;
    uint16_t  cmd_opcode;
    uint8_t   param[1];
} __attribute__ ((packed)) ibrt_cmd_comp_t;

typedef struct
{
    uint8_t  status;
    uint8_t  num_hci_cmd_packets;
    uint16_t cmd_opcode;
    bt_bdaddr_t bdaddr;
} __attribute__ ((packed)) ibrt_cmd_status_t;

/*
 * IBRT RAW_UI EVENT
 */
typedef enum {
    IBRT_STATUS_SUCCESS                  = 0,
    IBRT_STATUS_PENDING                  = 1,
    IBRT_STATUS_ERROR_INVALID_PARAMETERS = 2,
    IBRT_STATUS_ERROR_NO_CONNECTION      = 3,
    IBRT_STATUS_ERROR_CONNECTION_EXISTS  = 4,
    IBRT_STATUS_IN_PROGRESS              = 5,
    IBRT_STATUS_ERROR_DUPLICATE_REQUEST  = 6,
    IBRT_STATUS_ERROR_INVALID_STATE      = 7,
    IBRT_STATUS_ERROR_TIMEOUT            = 8,
    IBRT_STATUS_ERROR_ROLE_SWITCH_FAILED = 9,
    IBRT_STATUS_ERROR_UNEXPECTED_VALUE  = 10,
    IBRT_STATUS_ERROR_OP_NOT_ALLOWED    = 11,

    // Start vendor section
    IBRT_STATUS_VENDOR_START = 0x80,
    // BTCLIENT_STATUS_ERROR_SAMPLE = BTCLIENT_STATUS_VENDOR_START + 0,
    // End vendor
} ibrt_status_t;

typedef enum {
    ACL_CONNECTED_USR_TERMINATE        = 0,
    ACL_CONNECTING_CANCELED ,
    ACL_CONNECTING_FAILURE,
} acl_disconnect_reason;

typedef struct
{
    bool tws_switch_according_to_rssi_value;
    uint8_t rssi_threshold;
    uint8_t role_switch_timer_threshold;

    uint32_t connect_no_03_timeout;
    uint32_t disconnect_no_05_timeout;
} ibrt_conn_config_t;

typedef enum
{
    FREEMAN_MODE,
    IBRT_MODE,
}ibrt_mode_e;

typedef struct
{
    bt_bdaddr_t   mobile_addr;
    bt_bdaddr_t   nv_mobile_addr;
    ibrt_role_e   ibrt_role;
    uint8_t       mobile_connected:1;
    uint8_t       ibrt_connected:1;
    uint8_t       connecting_mobile:1;
    uint64_t      constate;
} __attribute__((packed)) app_tws_info_t;


typedef uint8_t tws_role_e;
#define   TWS_MASTER       0
#define   TWS_SLAVE        1
#define   TWS_ROLE_UNKNOW  0xff

// UI master is the one that communicates with mobile,
// it doesn't matter whether TWS is connected or not
typedef uint8_t TWS_UI_ROLE_E;
#define TWS_UI_MASTER       IBRT_MASTER
#define TWS_UI_SLAVE        IBRT_SLAVE
#define TWS_UI_UNKNOWN      IBRT_UNKNOW

typedef struct
{
    bt_bdaddr_t   mobile_addr;
    uint8_t       mobile_connected;
    uint8_t       sco_status;
} __attribute__((packed)) app_tws_start_ibrt_info_t;

typedef struct
{
    bt_bdaddr_t     local_addr;
    tws_role_e      current_ibrt_role;

    bt_bdaddr_t     peer_addr;
    tws_role_e      peer_ibrt_role;

    tws_role_e      nv_role;
} app_tws_buds_info_t;

typedef struct
{
    int32_t a2dp_volume;
    int32_t hfp_volume;
} ibrt_volume_info_t;

typedef enum {
    CONN_ACCEPT     = 0,
    CONN_REJECT     = 1,
    CONN_IGNORE     = 2,
} ibrt_conn_req_rsp_t;

typedef void (*ibrt_post_func)(void);

typedef ibrt_conn_req_rsp_t (*ibrt_ui_reject_connect_req_func)(void);

typedef struct {
    void (*ibrt_conn_a2dp_callback)(uint8_t device_id, void *param1,void *param2,void* param3);
    void (*ibrt_conn_hfp_callback)(uint8_t device_id, void *param1,void *param2,void* param3);
    void (*ibrt_conn_avrcp_callback)(uint8_t device_id, void *param1,void *param2,void* param3);
    void (*ibrt_conn_gsound_state_change)(bool state);
    void (*ibrt_conn_tws_on_pairing_changed)(ibrt_conn_pairing_state state,uint8_t reason_code);
    void (*ibrt_conn_tws_on_acl_state_changed)(ibrt_conn_acl_state state,uint8_t reason_code);
    void (*ibrt_conn_on_host_paring_changed)(const bt_bdaddr_t *addr,ibrt_conn_pairing_state state,uint8_t reason_code);
    void (*ibrt_conn_on_mobile_acl_state_changed)(const bt_bdaddr_t *addr,ibrt_conn_acl_state state,uint8_t reason_code);
    void (*ibrt_conn_on_tws_role_changed)(const bt_bdaddr_t *addr,ibrt_conn_role_change_state state,ibrt_role_e role);
    void (*ibrt_conn_access_mode_change)(btif_accessible_mode_t access_mode);
    void (*ibrt_conn_cancel_connect_state_changed)(int8_t state);
    void (*ibrt_conn_sco_state_changed)(const bt_bdaddr_t *addr, ibrt_conn_sco_state state,uint8_t reason_code);
    void (*ibrt_conn_on_ibrt_state_changed)(const bt_bdaddr_t *addr,ibrt_conn_ibrt_state state,ibrt_role_e role,uint8_t reason_code);
    void (*ibrt_conn_on_phone_connect_indication)(const bt_bdaddr_t *addr);
    void (*ibrt_conn_on_snoop_state_changed)(const bt_bdaddr_t *addr,uint8_t snoop_connected);
    void (*ibrt_conn_reset_tws_acl_data_packet_check)(void);
}app_ibrt_conn_event_cb;

//Register for BES UI
typedef struct{
    void (*send_mgr_info_hanlder_cb)(uint16_t rsp_seq,uint8_t *p_buff, uint16_t length);
    void (*send_mgr_info_rsp_handler_cb)(uint8_t *p_buff, uint16_t length);
    void (*send_mgr_info_rsp_timeout_hanlder_cb)(uint8_t *p_buff, uint16_t length);
    void (*send_mgr_info_fail_cb)(uint8_t *p_buff);

    void (*notify_running_info_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*notify_running_info_rsp_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*notify_running_info_rsp_timeout_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*notify_running_info_done)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);

    void (*delete_nv_mobile_addr_handler)(uint16_t rsp_seq,uint8_t *p_buff, uint16_t length);
    void (*delete_nv_mobile_addr_rsp_handler)(uint8_t *p_buff, uint16_t length);
    void (*delete_nv_mobile_addr_rsp_timeout_hanlder)(uint8_t *p_buff, uint16_t length);

    void (*disc_mobile_link_handler)(uint16_t rsp_seq,uint8_t *p_buff, uint16_t length);
    void (*disc_mobile_link_rsp_handler)(uint8_t *p_buff, uint16_t length);
    void (*disc_mobile_link_rsp_timeout_hanlder)(uint8_t *p_buff, uint16_t length);

    void (*perform_received_choice_mobile_connect)(const bt_bdaddr_t *addr);
}app_ibrt_cmd_ui_handler_cb;

typedef struct {
    void (*keyboard_request_handler)(uint16_t rsp_seq, uint8_t *p_buff, uint16_t length);
    void (*ui_perform_user_action)(uint8_t *p_buff, uint16_t length);
    void (*conn_profile_handler)(bool is_ibrt_slave_receive_request, uint8_t *p_buff, uint16_t length);
    void (*disc_profile_handler)(uint8_t *p_buff, uint16_t length);
    void (*disc_rfcomm_handler)(uint8_t *p_buff);
    bool (*ibrt_if_sniff_prevent_need)(bt_bdaddr_t *p_mobile_addr);
    bool (*tws_switch_prepare_needed)(uint32_t *wait_ms);
    void (*tws_swtich_prepare)(uint32_t timeoutMs);
}app_ibrt_if_cbs_t;

#endif

#endif /*__APP_TWS_IBRT_RAW_UI_H__*/
