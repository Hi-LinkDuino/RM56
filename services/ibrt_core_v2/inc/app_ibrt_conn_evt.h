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
#ifndef __APP_IBRT_CONN_EVENT_H__
#define __APP_IBRT_CONN_EVENT_H__

#include "nvrecord_extension.h"
#include "btapp.h"
#include "app_tws_ibrt.h"

/*This file is not only contain ibrt connection message and also contain
* IBRT link mgr message, IBRT connection messge is from IBRT connection,
* such as mobile connection,TWS connection status changed and so on.
* if function need to switch thread ......, reuse recive thread.
*/

#define IBRT_CONN_STATE_MASK        0xF0
#define IBRT_CONN_STATE_PAIRING     0x00
#define IBRT_CONN_STATE_CONNECTION  0x10

typedef  uint16_t   ibrt_conn_box_state;

typedef enum {
    IBRT_CONN_STATUS_SUCCESS                  = 0,
    IBRT_CONN_STATUS_PENDING                  = 1,
    IBRT_CONN_STATUS_ERROR_INVALID_PARAMETERS = 2,
    IBRT_CONN_STATUS_ERROR_NO_CONNECTION      = 3,
    IBRT_CONN_STATUS_ERROR_CONNECTION_EXISTS  = 4,
    IBRT_CONN_STATUS_IN_PROGRESS              = 5,
    IBRT_CONN_STATUS_ERROR_DUPLICATE_REQUEST  = 6,
    IBRT_CONN_STATUS_ERROR_INVALID_STATE      = 7,
    IBRT_CONN_STATUS_ERROR_TIMEOUT            = 8,
    IBRT_CONN_STATUS_ERROR_ROLE_SWITCH_FAILED = 9,
    IBRT_CONN_STATUS_ERROR_UNEXPECTED_VALUE  = 10,
    IBRT_CONN_STATUS_ERROR_OP_NOT_ALLOWED    = 11,

    IBRT_CONN_MGR_STATUS_VENDOR_START = 0x80,

} ibrt_conn_status;

typedef enum {
    IBRT_CONN_EVENT_FIRST                 = 0,
    IBRT_CONN_EVENT_API_STATUS            = IBRT_CONN_EVENT_FIRST,
    IBRT_CONN_EVENT_TW_PAIRING_STATE      = 1,
    IBRT_CONN_EVENT_TW_CONNECTION_STATE   = 2,
    IBRT_CONN_EVENT_TW_ROLE_CHANGE_STATE  = 3,
    IBRT_CONN_EVENT_HOST_PAIRING_STATE    = 4,
    IBRT_CONN_EVENT_MOBILE_CONNECTION_STATE = 5,
    IBRT_CONN_EVENT_IBRT_CONNECTION_STATE = 6,
    IBRT_CONN_EVENT_LE_STATE              = 7,
    IBRT_CONN_EVENT_BT_STATE              = 8,
    IBRT_CONN_EVENT_RSSI_REPORT           = 9,
    IBRT_CONN_EVENT_A2DP_STATE            = 10,
    IBRT_CONN_EVENT_AVRCP_STATE           = 11,
    IBRT_CONN_EVENT_HFP_STATE             = 12,
    IBRT_CONN_EVENT_HSP_STATE             = 13,
    IBRT_CONN_EVENT_GLOBAL_STATE          = 14,
    IBRT_CONN_EVENT_SCO_CONNECTION_STATE  = 15,
    IBRT_CONN_EVENT_ACCESS_MODE           = 16,

    // Extend for ibrt manager
    IBRT_CONN_EVENT_BOX_STATE             = 20,
    IBRT_CONN_EVENT_OPERATION_STATE       = 21,
    IBRT_CONN_EVENT_EXCHANGE_LINK_INFO    = 22,
    IBRT_CONN_EVENT_SYNC_LINK_INFO        = 23,
    IBRT_CONN_MOBILE_SM_RUN_COMPLETE      = 24,
    IBRT_CONN_SYNC_PAIRING_MODE           = 25,
    IBRT_CONN_NOTIFY_LINK_STATUS          = 26,

    IBRT_CONN_EVENT_LAST                  = 0x1000
} ibrt_conn_event;

typedef enum {
    IBRT_CONN_PAIRING_IDLE        = 0,
    IBRT_CONN_PAIRING_IN_PROGRESS = 1,
    IBRT_CONN_PAIRING_COMPLETE    = 2,
    IBRT_CONN_PAIRING_TIMEOUT     = 3,
} ibrt_conn_pairing_state;

typedef struct {
    ibrt_conn_event   type;
    uint16_t          length;
} ibrt_conn_evt_header;

typedef enum {
    IBRT_CONN_EV_CASE_EVT_NONE = 0x100,
    IBRT_CONN_EV_CASE_OPEN,
    IBRT_CONN_EV_CASE_CLOSE,
    IBRT_CONN_EV_DOCK,
    IBRT_CONN_EV_UNDOCK,
    IBRT_CONN_EV_WEAR_UP,
    IBRT_CONN_EVT_WEAR_DOWN,
    IBRT_CONN_EV_PEER_EV_NONE  = 0x200,
    IBRT_CONN_EV_PEER_CASE_OPEN,
    IBRT_CONN_EV_PEER_CASE_CLOSE,
    IBRT_CONN_EV_PEER_DOCK,
    IBRT_CONN_EV_PEER_UNDOCK,
    IBRT_CONN_EVT_PEER_WEAR_UP,
    IBRT_CONN_EVT_PEER_WEAR_DOWN,
} ibrt_conn_box_evt;

typedef enum {
    IBRT_CONN_ACL_DISCONNECTED        = 0,
    IBRT_CONN_ACL_CONNECTING          = 1,
    IBRT_CONN_ACL_CONNECTING_CANCELED = 2,
    IBRT_CONN_ACL_CONNECTING_FAILURE  = 3,
    IBRT_CONN_ACL_RAW_CONNECTED       = 4,
    IBRT_CONN_ACL_CONNECTED           = 5,
    IBRT_CONN_ACL_PROFILES_CONNECTED  = 6,
    IBRT_CONN_ACL_AUTH_COMPLETE       = 7,
    IBRT_CONN_ACL_DISCONNECTING       = 8,
    IBRT_CONN_ACL_UNKNOWN             = 9,
} ibrt_conn_acl_state;

typedef enum {
    IBRT_CONN_SCO_DISCONNECTED        = 0,
    IBRT_CONN_SCO_CONNECTED           = 1,
    IBRT_CONN_SCO_UNKNOWN             = 2,
} ibrt_conn_sco_state;


typedef enum {
    IBRT_CONN_IBRT_DISCONNECTED       = 0,
    IBRT_CONN_IBRT_CONNECTED          = 1,
    IBRT_CONN_IBRT_START_FAIL         = 2,
    IBRT_CONN_IBRT_ACL_CONNECTED      = 3, 
} ibrt_conn_ibrt_state;

typedef enum {
    IBRT_CONN_A2DP_IDLE             = 0,
    IBRT_CONN_A2DP_CLOSED           = 1,
    IBRT_CONN_A2DP_CODEC_CONFIGURED = 2,
    IBRT_CONN_A2DP_OPEN             = 3,
    IBRT_CONN_A2DP_STREAMING        = 4,
    IBRT_CONN_A2DP_SUSPENED         = 5,
} ibrt_conn_a2dp_state;

typedef enum {
    IBRT_CONN_CODEC_AAC  = 0,
    IBRT_CONN_CODEC_SBC  = 1,
    IBRT_CONN_CODEC_MSBC = 2,
    IBRT_CONN_CODEC_CVSD = 3,
} ibrt_conn_codec_type;

typedef enum {
    IBRT_CONN_AUDIO_SAMPLING_RATE_8000  = 0,
    IBRT_CONN_AUDIO_SAMPLING_RATE_16000 = 1,
    IBRT_CONN_AUDIO_SAMPLING_RATE_44100 = 2,
    IBRT_CONN_AUDIO_SAMPLING_RATE_48000 = 3,
} ibrt_conn_audio_sampling_rate;

typedef enum {
    IBRT_CONN_L2CAP_DISCONNECTED = 0,
    IBRT_CONN_L2CAP_CONNECTING   = 1,
    IBRT_CONN_L2CAP_CONNECTED    = 2,
    IBRT_CONN_L2CAP_ERROR        = 3,
} ibrt_conn_l2cap_state;

typedef enum {
    IBRT_BLUETOOTH_ENABLED      = 1,
    IBRT_BLUETOOTH_DISABLED     = 2,
} ibrt_global_state_enum;

typedef struct {
    ibrt_conn_codec_type          codec;
    ibrt_conn_audio_sampling_rate sampling_rate;
    uint32_t                      bitrate;
    bool                          variable_bitrate;
} ibrt_conn_a2dp_audio_settings;

typedef enum {
    IBRT_CONN_AVRCP_DISCONNECTED   = 0,
    IBRT_CONN_AVRCP_CONNECTED      = 1,
    IBRT_CONN_AVRCP_VOLUME_UPDATED = 2,
    IBRT_CONN_AVRCP_REMOTE_CT_0104 = 3,
    IBRT_CONN_AVRCP_REMOTE_SUPPORT_PLAYBACK_STATUS_CHANGED_EVENT = 4,
    IBRT_CONN_AVRCP_PLAYBACK_STATUS_CHANGED = 5,
} ibrt_conn_avrcp_state;

typedef enum {
    IBRT_CONN_HFP_SLC_DISCONNECTED = 0,
    IBRT_CONN_HFP_SLC_OPEN         = 1,
    IBRT_CONN_HFP_SCO_OPEN         = 2,
    IBRT_CONN_HFP_SCO_CLOSED       = 3,
    IBRT_CONN_HFP_RING_IND         = 4,
    IBRT_CONN_HFP_CALL_IND         = 5,
    IBRT_CONN_HFP_CALLSETUP_IND    = 6,
    IBRT_CONN_HFP_CALLHELD_IND     = 7,
    IBRT_CONN_HFP_CIEV_SERVICE_IND = 8,
    IBRT_CONN_HFP_CIEV_SIGNAL_IND  = 9,
    IBRT_CONN_HFP_CIEV_ROAM_IND    = 10,
    IBRT_CONN_HFP_CIEV_BATTCHG_IND = 11,
    IBRT_CONN_HFP_SPK_VOLUME_IND   = 12,
    IBRT_CONN_HFP_MIC_VOLUME_IND   = 13,
    IBRT_CONN_HFP_IN_BAND_RING_IND = 14,
    IBRT_CONN_HFP_VR_STATE_IND     = 15,
    IBRT_CONN_HFP_AT_CMD_COMPLETE  = 16,
} ibrt_conn_hfp_state;

typedef enum
{
    // Request/response information types
    REQ_NONE,
    REQ_LOCAL_RUN,        /* Request running of initiator*/
    REQ_WAIT_DECISION,    /* Request running is decision by responder*/
    REQ_LOCAL_RUN_PREFER, /* if peer mobile connected,peer run,otherwise local run */

    // Response exchange infor types from deciesion
    RSP_LOCAL_RUN,         /* Running of initiator*/
    RSP_LOCAL_RUN_DEFER,   /* Running of initiator,but defered until respondor idle*/
    RSP_PEER_RUN,          /* Running of responder */
    RSP_RESTART,

    // Sync NV address information between earbuds when NV mismatch
    RSP_FORCE_SYNC_NV,     /*Sync nv of initiator*/

    // Notify information types
    NOTIFY_LINK_IDLE,              /* Notify peer side current link is idle */
    NOTIFY_LINK_GROUP_IDLE,        /* Notify peer side link group is idle */
    NOTIFY_PEER_EVENT_COMPLETE,    /* Notify peer side peer event run complete */
    NOTIFY_GROUP_INFO,             /* Notify peer side add current link into group */

}ibrt_req_info_type_t;

typedef enum
{
    SYNC_PAIRING_NONE,
    SYNC_FOR_DICONNECTED,
    SYNC_FOR_CONNECTED_FAIL,
}sync_pairing_reason_t;

typedef struct
{
    ibrt_req_info_type_t   req_type;
    ibrt_req_info_type_t   rsp_type;
    uint8_t                req_link_id;
}ibrt_conn_req_hdr_t;

typedef struct {
    ibrt_conn_acl_state acl_state;
    /* Valid for ACL_DISCONNECTED, ACL_CONNECTING_FAILURE, ACL_CONNECTING_CANCELED */
    uint8_t          bluetooth_reason_code;
} ibrt_conn_connection_state;

typedef enum {
    IBRT_CORE_V2_HOST_CONNECT_CANCEL = 0,
} ibrt_conn_api;

typedef struct {
    ibrt_conn_ibrt_state ibrt_state;
    uint8_t              ibrt_reason_code;
} ibrt_conn_ibrt_connection_state;

typedef struct {
    ibrt_conn_sco_state  sco_state;
    uint8_t              sco_reason_code;
} ibrt_conn_sco_connection_state;

typedef enum {
    IBRT_CONN_ROLE_IDLE             = 0,
    IBRT_CONN_ROLE_SWAP_INITIATED   = 1,
    IBRT_CONN_ROLE_SWAP_COMPLETE    = 2,
    IBRT_CONN_ROLE_CHANGE_REQUESTED = 3,
    IBRT_CONN_ROLE_CHANGE_COMPLETE  = 4,
    IBRT_CONN_ROLE_CHANGED          = 5,
    IBRT_CONN_ROLE_SWAP_DISALLOW    = 6,
} ibrt_conn_role_change_state;

typedef struct {
    ibrt_conn_req_hdr_t req_hdr;
    bt_bdaddr_t         addr;
    uint8_t             link_id;
    ibrt_conn_box_evt   box_event;
    ibrt_conn_box_state box_state;
    uint16_t            group_mask;
    bool                mobile_link_status;
    bool                ibrt_link_status;
    bool                is_busy;
    bool                exchangeinfo_ongoing;
    bool                peer_busy_status;
    bool                is_sco_active;
} ibrt_share_link_info; //aligment. 18 bytes

typedef struct {
    nvrec_btdevicerecord  dev_record;
    bool                  mobile_link_status;
} ibrt_nv_info_rec; //26 bytes

typedef struct {
    ibrt_nv_info_rec  info_record[BT_DEVICE_NUM];
    uint16_t          mobile_connected_count;
    uint16_t          sync_count;
} ibrt_sync_nv_info;

typedef struct {
    ibrt_conn_evt_header header;
    ibrt_conn_api        command;
    ibrt_conn_status     status;
} ibrt_conn_api_status_event;

typedef struct {
    ibrt_conn_evt_header       header;
    ibrt_conn_connection_state connection_state;
    ibrt_conn_pairing_state    pairing_state;
} ibrt_conn_tws_pairing_state_evt;

typedef struct {
    ibrt_conn_evt_header     header;
    ibrt_conn_connection_state state;
    uint8_t              current_role;
} ibrt_conn_tws_conn_state_event;

typedef struct {
    ibrt_conn_evt_header         header;
    ibrt_conn_role_change_state  role_state;
    ibrt_conn_status             role_change_status;
    ibrt_role_e                  role;
    bt_bdaddr_t                  addr;
} ibrt_conn_tw_role_change_state_event;

typedef struct {
    ibrt_conn_evt_header       header;
    ibrt_conn_connection_state connection_state;
    ibrt_conn_pairing_state    pairing_state;
    bt_bdaddr_t                addr;
} ibrt_mobile_pairing_state_event;

typedef struct {
    ibrt_conn_evt_header       header;
    ibrt_conn_connection_state state;
    uint8_t                    current_role;
    uint8_t                    device_id;
    bt_bdaddr_t                addr;
} ibrt_mobile_conn_state_event;

typedef struct {
    ibrt_conn_evt_header             header;
    ibrt_conn_ibrt_connection_state  state;
    uint8_t                          current_role;
    uint8_t                          device_id;
    bt_bdaddr_t                      addr;
} ibrt_connection_state_event;

typedef struct {
    ibrt_conn_evt_header             header;
    ibrt_conn_sco_connection_state   state;
    uint8_t                          current_role;
    uint8_t                          device_id;
    bt_bdaddr_t                      addr;
} ibrt_sco_conn_state_event;

typedef struct {
    ibrt_conn_evt_header          header;
    ibrt_global_state_enum        state;
} ibrt_global_state_change_event;

typedef struct {
    ibrt_conn_evt_header          header;
    btif_accessible_mode_t        access_mode;
} ibrt_access_mode_change_event;

typedef struct {
    ibrt_conn_evt_header          header;
    ibrt_conn_l2cap_state         l2cap_state;
    ibrt_conn_a2dp_state          a2dp_state;
    ibrt_conn_a2dp_audio_settings audio_settings; /* Valid for CODEC_CONFIGURED */
    bool                          delay_report_support;
    uint8_t                       device_id;
    bt_bdaddr_t                   addr;
} ibrt_conn_a2dp_state_change;

typedef struct {
    ibrt_conn_evt_header          header;
    ibrt_conn_l2cap_state         l2cap_state;
    ibrt_conn_avrcp_state         avrcp_state;
    uint8_t                       volume; /* Valid for VOLUME_UPDATE */
    uint8_t                       playback_status;
    bool                          support;
    uint8_t                       device_id;
    bt_bdaddr_t                   addr;
} ibrt_conn_avrcp_state_change;

typedef struct {
    ibrt_conn_evt_header  header;
    ibrt_conn_l2cap_state l2cap_state;
    ibrt_conn_hfp_state   hfp_state;
    ibrt_conn_codec_type  codec;
    uint8_t               device_id;
    bt_bdaddr_t           addr;
    uint8_t               ciev_status;
    uint8_t               volume_ind;
    uint8_t               error_code;
    uint8_t               voice_rec_state;
    uint8_t               in_band_ring_enable;
} ibrt_conn_hfp_state_change;

//Extend for ibrt manager
typedef struct {
    ibrt_conn_evt_header   header;
    ibrt_conn_box_evt      box_evt;
} ibrt_conn_box_state_evt;

typedef struct {
    ibrt_conn_evt_header     header;
    uint32_t                 status;     /* 0:SUCCESS,1:FAIL */

    union
    {
        ibrt_share_link_info share_link_info[BT_DEVICE_NUM];
        ibrt_sync_nv_info    sync_link_info;
    }link_info;
} ibrt_conn_mgr_info_pkt;

typedef uint16_t ibrt_conn_ibrt_evt;

typedef struct {
    ibrt_conn_evt_header    header;
    ibrt_conn_ibrt_evt      ibrt_evt;
    uint16_t                link_id;
    bt_bdaddr_t             addr;
} ibrt_conn_ibrt_pkt;

typedef struct {
    ibrt_conn_evt_header     header;
    uint8_t                  action;       /* 0:None,1:Entry,2:Exit */
    uint8_t                  action_rsp;   /* 0:Agree,1:Reject */
    sync_pairing_reason_t    reason;
} ibrt_conn_sync_paring_mode_pkt;

typedef struct {
    ibrt_conn_evt_header    header;
    uint16_t                link_id;
    uint16_t                complete_evt;
} ibrt_conn_mobile_link_pkt;

typedef struct {
    ibrt_conn_evt_header    header;
    uint8_t                 type;
    uint16_t                peer_latest_evt;
    uint32_t                p_mobile_link;
    uint32_t                link_group_mask;
} ibrt_conn_link_status_pkt;

//Add more packet
typedef union {
  ibrt_conn_tws_pairing_state_evt       tws_pairing_state;
  ibrt_conn_tws_conn_state_event        tws_conn_state;
  ibrt_conn_tw_role_change_state_event  role_change_state;
  ibrt_mobile_pairing_state_event       mobile_pairing_state;
  ibrt_mobile_conn_state_event          mobile_conn_state;
  ibrt_connection_state_event           ibrt_conn_state;
  ibrt_conn_a2dp_state_change           a2dp_state;
  ibrt_conn_avrcp_state_change          avrcp_state;
  ibrt_conn_hfp_state_change            hfp_state;
  ibrt_global_state_change_event        global_state;
  ibrt_sco_conn_state_event             sco_conn_state;
  ibrt_access_mode_change_event         access_mode_state;

  //Extend for ibrt manager
  ibrt_conn_box_state_evt               box_state;
  ibrt_conn_mgr_info_pkt                ibrt_mgr_info;
  ibrt_conn_ibrt_pkt                    ibrt_event;
  ibrt_conn_mobile_link_pkt             mobile_complete_evt;
  ibrt_conn_link_status_pkt             link_status;
} ibrt_conn_event_packet;

#endif

