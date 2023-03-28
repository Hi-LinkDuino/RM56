/**
 ****************************************************************************************
 *
 * @file iap_msg.h
 *
 * @brief Isochronous Access Profile - Definition of Kernel Messages
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef IAP_MSG_H_
#define IAP_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup IAP Isochronous Access Profile - Definition of Kernel Messages
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "iap.h"            // IAP Definitions
#include "gaf.h"            // GAF Defines

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of GAF_IND indication codes
enum iap_msg_ind_codes
{
    /// Unicast group management indications
    IAP_US_ENABLED = GAF_CODE(IAP, UG, 0),
    IAP_US_DISABLED = GAF_CODE(IAP, UG, 1),

    /// Broadcast group management indications
    IAP_BG_ENABLED = GAF_CODE(IAP, BG, 0),
    IAP_BG_DISABLED = GAF_CODE(IAP, BG, 1),
    IAP_BG_SYNC_ADDED = GAF_CODE(IAP, BG, 2),

    /// Data path management indications
    IAP_DP_UPDATE = GAF_CODE(IAP, DP, 0),

    /// Test mode management indications
    IAP_TM_CNT = GAF_CODE(IAP, TM, 0),
};

/*
 * API MESSAGES
 ****************************************************************************************
 */

/// Structure for GAF_CMD message
typedef struct iap_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
} iap_cmd_t;

/// Structure for GAF_CMP_EVT message
typedef struct iap_cmp_evt
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Group local index
    uint8_t group_lid;
    /// Stream local index
    uint8_t stream_lid;
} iap_cmp_evt_t;

/// Structure for GAF_REQ message
typedef struct iap_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
} iap_req_t;

/// Structure for GAF_RSP message
typedef struct iap_rsp
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
} iap_rsp_t;

/// Structure for GAF_IND message
typedef struct iap_ind
{
    /// Indication code
    uint16_t ind_code;
} iap_ind_t;

/// Structure for IAP_UG_ADD request message
typedef struct iap_ug_add_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Unicast group information
    iap_ug_param_t group_info;
    /// CIG ID
    uint8_t cig_id;
} iap_ug_add_req_t;

/// Structure for IAP_UG_ADD response message
typedef struct iap_ug_add_rsp
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// CIG ID
    uint8_t cig_id;
    /// Allocated Group local index
    uint8_t group_lid;
} iap_ug_add_rsp_t;

/// Structure for IAP_US_CONFIG request message
typedef struct iap_us_config_req
{
    /// Command code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Group local index
    uint8_t group_lid;
    /// CIS IS
    uint8_t cis_id;
    /// Unicast stream information
    iap_us_param_t stream_info;
} iap_us_config_req_t;

/// Structure for IAP_US_CONFIG response message
typedef struct iap_us_config_rsp
{
    /// Command code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Group local index
    uint8_t group_lid;
    /// CIS IS
    uint8_t cis_id;
    /// Allocated Stream local index
    uint8_t stream_lid;
} iap_us_config_rsp_t;

/// Structure for IAP_US_PREPARE request message
typedef struct iap_us_prepare_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Connection local index
    uint8_t con_lid;
    /// CIG ID
    uint8_t cig_id;
    /// CIS ID
    uint8_t cis_id;
} iap_us_prepare_req_t;

/// Structure for IAP_US_PREPARE response message
typedef struct iap_us_prepare_rsp
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Connection local index
    uint8_t con_lid;
    /// CIG ID
    uint8_t cig_id;
    /// CIS ID
    uint8_t cis_id;
    /// Group local index
    uint8_t group_lid;
    /// Stream local index
    uint8_t stream_lid;
} iap_us_prepare_rsp_t;

/// Structure for IAP_US_BIND request message
typedef struct iap_us_bind_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Connection local index
    uint8_t con_lid;
} iap_us_bind_req_t;

/// Structure for IAP_UG_UPDATE command message
typedef struct iap_ug_update_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Group local index
    uint8_t group_lid;
} iap_ug_update_cmd_t;

/// Structure for IAP_US_ENABLE command message
typedef struct iap_us_enable_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Group local index
    uint8_t group_lid;
} iap_us_enable_cmd_t;

/// Structure for IAP_US_DISABLE command message
typedef struct iap_us_disable_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Stream local index
    uint8_t stream_lid;
} iap_us_disable_cmd_t;

/// Structure for IAP_US_RELEASE request message
typedef struct iap_us_release_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Stream local index
    uint8_t stream_lid;
} iap_us_release_req_t;

/// Structure for IAP_UG_REMOVE command message
typedef struct iap_ug_remove_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Group local index
    uint8_t group_lid;
} iap_ug_remove_cmd_t;

/// Structure for IAP_BG_ADD request message
typedef struct iap_bg_add_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Group information
    iap_bg_param_t group_info;
    /// BIG Handle
    uint8_t big_handle;
    /// Number of streams in the group
    uint8_t nb_streams;
} iap_bg_add_req_t;

/// Structure for IAP_BG_ADD response message
typedef struct iap_bg_add_rsp
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// BIG Handle
    uint8_t big_handle;
    /// Number of streams in the group
    uint8_t nb_streams;
    /// Allocated group local index
    uint8_t group_lid;
    /// List of allocated stream local indexes
    uint8_t stream_lids[__ARRAY_EMPTY];
} iap_bg_add_rsp_t;

/// Structure for IAP_BG_ENABLE command message
typedef struct iap_bg_enable_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Group local index
    uint8_t group_lid;
    /// Advertising activity local index
    uint8_t adv_actv_lid;
    /// Indicate if streams are encrypted (!=0) or not (=0)
    uint8_t encrypted;
    /// Code used to generate the encryption key for encrypting payloads of all streams
    uint8_t broadcast_code[16];
} iap_bg_enable_cmd_t;

/// Structure for IAP_BG_SYNC command message
typedef struct iap_bg_sync_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Synchronization timeout in multiple of 10ms
    uint16_t sync_timeout_10ms;
    /// BIG handle
    uint8_t big_handle;
    /// Synchronization activity local index
    uint8_t sync_actv_lid;
    /// Maximum number of subevents the controller should use to receive data payloads in each interval
    uint8_t mse;
    /// Indicate if streams are encrypted (!=0) or not (=0)
    uint8_t encrypted;
    /// Code used to generate the encryption key for encrypting payloads of all streams
    uint8_t broadcast_code[16];
    /// Stream position bit field to synchronize with.
    uint32_t stream_pos_bf;
} iap_bg_sync_cmd_t;

/// Structure for IAP_BG_SYNC_ADDED indication message
typedef struct iap_bg_sync_added_ind
{
    /// Indication code (@see enum iap_ind_codes)
    uint16_t ind_code;
    /// BIG handle
    uint8_t big_handle;
    /// Number of streams to synchronize with
    uint8_t nb_streams;
    /// Group local index
    uint8_t group_lid;
    /// List of stream local indexes
    uint8_t stream_lids[__ARRAY_EMPTY];
} iap_bg_sync_added_ind_t;

/// Structure for IAP_BG_DISABLE command message
typedef struct iap_bg_disable_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Group local index
    uint8_t group_lid;
} iap_bg_disable_cmd_t;

/// Structure for IAP_BG_REMOVE command message
typedef struct iap_bg_remove_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Group local index
    uint8_t group_lid;
} iap_bg_remove_cmd_t;

/// Structure for IAP_DP_VS_SET_TRIGGER command message
typedef struct iap_dp_vs_set_trigger_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Direction (0 = TX, 1 = RX)
    uint8_t direction;
    /// Enable
    uint8_t enable;
    /// Trigger offset in microseconds
    /// For TX direction, trigger offset is applied using SDU at source reference
    /// For RX direction, trigger offset is applied using SDU at destination reference
    /// (SDU at source + transport latency)
    int32_t trigger_offset_us;
} iap_dp_vs_set_trigger_cmd_t;

/// Structure for IAP_UG_TEST_ADD request message
typedef struct iap_ug_test_add_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Unicast group test information structure
    iap_ug_test_param_t group_info;
    /// CIG ID
    uint8_t cig_id;
} iap_ug_test_add_req_t;

/// Structure for IAP_US_TEST_CONFIG request message
typedef struct iap_us_test_config_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Group local index
    uint8_t group_lid;
    /// CIS IS
    uint8_t cis_id;
    /// Unicast stream information
    iap_us_test_param_t stream_info;
} iap_us_test_config_req_t;

/// Structure for IAP_BG_TEST_ADD request message
typedef struct iap_bg_test_add_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Broadcast group test information structure
    iap_bg_test_param_t group_info;
    /// BIG Handle
    uint8_t big_handle;
    /// Number of streams in the group
    uint8_t nb_streams;
    /// Indicate if streams are encrypted (!=0) or not (=0)
    uint8_t encrypted;
    /// Code used to generate encryption key used to encrypt payloads
    uint8_t broadcast_code[16];
} iap_bg_test_add_req_t;

/// Structure for IAP_DP_CONFIG request message
typedef struct iap_dp_config_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Direction
    uint8_t direction;
    /// Controller Delay in microseconds
    uint32_t ctl_delay_us;
    /// Codec ID
    uint8_t codec_id[GAF_CODEC_ID_LEN];
    /// Datapath ID
    uint8_t dp_id;
    /// Codec Configuration length
    uint8_t codec_cfg_len;
    /// Additional Codec Configuration length
    uint8_t add_codec_cfg_len;
    /// Codec Configuration followed by additional Codec Configuration
    uint8_t codec_cfg[__ARRAY_EMPTY];
} iap_dp_config_req_t;

/// Structure for IAP_DP_SETUP request message
typedef struct iap_dp_setup_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Direction
    uint8_t direction;
} iap_dp_setup_req_t;

/// Structure for IAP_DP_REMOVE request message
typedef struct iap_dp_remove_req
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Direction bit field
    uint8_t direction_bf;
} iap_dp_remove_req_t;

/// Structure for IAP_TM_START command message
typedef struct iap_tm_start_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Transmit (!=0) or receive (=0) test payload
    uint8_t transmit;
    /// Payload type
    uint8_t payload_type;
} iap_tm_start_cmd_t;

/// Structure for IAP_TM_CNT_GET command message
typedef struct iap_tm_cnt_get_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Stream local index
    uint8_t stream_lid;
} iap_tm_cnt_get_cmd_t;

/// Structure for IAP_TM_STOP command message
typedef struct iap_tm_stop_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Stream local index
    uint8_t stream_lid;
} iap_tm_stop_cmd_t;

/// Structure for IAP_GET_QUALITY command  message
typedef struct iap_get_quality_cmd
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Stream local index
    uint8_t stream_lid;
} iap_get_quality_cmd_t;

/// Structure for IAP_QUALITY command complete event message
typedef struct iap_get_quality_cmp_evt
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Stream local index
    uint8_t stream_lid;
    /// Number of unacked transmitted packets
    uint32_t tx_unacked_packets;
    /// Number of flushed transmitted packets
    uint32_t tx_flushed_packets;
    /// Number of packets transmitted during last subevent
    uint32_t tx_last_subevent_packets;
    /// Number of retransmitted packets
    uint32_t retransmitted_packets;
    /// Number of packets received with a CRC error
    uint32_t crc_error_packets;
    /// Number of unreceived packets
    uint32_t rx_unreceived_packets;
    /// Number of duplicate packets received
    uint32_t duplicate_packets;
} iap_get_quality_cmp_evt_t;

/// Structure for IAP_INTF_REGISTER command complete event message
typedef struct iap_register_cmp_evt
{
    /// Command code (@see enum iap_cmd_codes)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Allocated interface local index
    uint8_t intf_lid;
} iap_register_cmp_evt_t;

/// Structure for IAP_US_DISABLED indication message
typedef struct iap_us_disabled_ind
{
    /// Indication code (@see enum iap_msg_ind_codes)
    uint16_t ind_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Reason
    uint8_t reason;
} iap_us_disabled_ind_t;

/// Structure for IAP_BG_DISABLED indication message
typedef struct iap_bg_disabled_ind
{
    /// Indication code (@see enum iap_msg_ind_codes)
    uint16_t ind_code;
    /// Group local index
    uint8_t group_lid;
    /// Reason
    uint8_t reason;
} iap_bg_disabled_ind_t;

/// Structure for IAP_US_ENABLED indication message
typedef struct iap_us_enabled_ind
{
    /// Indication code (@see enum iap_msg_ind_codes)
    uint16_t ind_code;
    /// Group local index
    uint8_t group_lid;
    /// Stream local index
    uint8_t stream_lid;
    /// Group configuration
    iap_ug_config_t ug_config;
    /// Stream configuration
    iap_us_config_t us_config;
} iap_us_enabled_ind_t;

/// Structure for IAP_BG_ENABLED indication message
typedef struct iap_bg_enabled_ind
{
    /// Indication code (@see enum iap_msg_ind_codes)
    uint16_t ind_code;
    /// Group local index
    uint8_t group_lid;
    /// Group configuration
    iap_bg_config_t config;
} iap_bg_enabled_ind_t;

/// Structure for IAP_DP_UPDATE indication message
typedef struct iap_dp_update_ind
{
    /// Indication code (@see enum iap_msg_ind_codes)
    uint16_t ind_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Data path update type
    uint8_t dp_update;
    /// Direction for setup update, direction bit field for remove update
    uint8_t direction;
    /// Status
    uint16_t status;
} iap_dp_update_ind_t;

/// Structure for IAP_TM_CNT indication message
typedef struct iap_tm_cnt_ind
{
    /// Indication code (@see enum iap_msg_ind_codes)
    uint16_t ind_code;
    /// Stream local index
    uint8_t stream_lid;
    /// Number of received packets
    uint32_t nb_rx;
    /// Number of missed packets
    uint32_t nb_missed;
    /// Number of failed packets
    uint32_t nb_failed;
} iap_tm_cnt_ind_t;

/// Structure for IAP_DBG_FEATURES response message
typedef struct iap_dbg_features_rsp
{
    /// Request code (@see enum iap_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Number of streams supported
    uint8_t nb_streams;
    /// Unicast supported
    uint8_t unicast;
    /// Broadcast supported
    uint8_t broadcast;
} iap_dbg_features_rsp_t;

/// @}

#endif // IAP_MSG_H_
