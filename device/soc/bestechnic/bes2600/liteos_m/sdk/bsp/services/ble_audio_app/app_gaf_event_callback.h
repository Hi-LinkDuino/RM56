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
/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_GAF_EVENT_CALLBACK_H_
#define APP_GAF_EVENT_CALLBACK_H_

//#include "app_bap_bc_scan_msg.h"
#include <stdint.h>                 // Standard Integer Definitions
#include <stdbool.h>                // Standard Boolean definitions
#include "hal_trace.h"
#include "app_bap.h"

#ifndef __ARRAY_EMPTY
#define __ARRAY_EMPTY 1
#endif

// event only can use low 12 bits
typedef enum
{
    APP_GAF_EVENT_FIRST = 0x000,

    // ASCS Events
    APP_GAF_ASCS_CIS_ESTABLISHED_IND        = APP_GAF_EVENT_FIRST,
    APP_GAF_ASCS_CIS_DISCONNETED_IND        = 0x001,
    APP_GAF_ASCS_CIS_STREAM_STARTED_IND     = 0x002,
    APP_GAF_ASCS_CIS_STREAM_STOPPED_IND     = 0x003,
    APP_GAF_ASCS_CONFIGURE_CODEC_RI         = 0x004,
    APP_GAF_ASCS_ENABLE_RI                  = 0x005,
    APP_GAF_ASCS_UPDATE_METADATA_RI         = 0x006,
    APP_GAF_ASCS_RELEASE_RI                 = 0x007,

    // PACS Event
    APP_GAF_PACS_LOCATION_SET_IND           = 0x010,

    // BIS SCAN Events
    APP_GAF_SCAN_TIMEOUT_IND                = 0x020,
    APP_GAF_SCAN_REPORT_IND                 = 0x021,
    APP_GAF_SCAN_PA_REPORT_IND              = 0x022,
    APP_GAF_SCAN_PA_ESTABLISHED_IND         = 0x023,
    APP_GAF_SCAN_PA_TERMINATED_IND          = 0x024,
    APP_GAF_SCAN_GROUP_REPORT_IND           = 0x025,
    APP_GAF_SCAN_SUBGROUP_REPORT_IND        = 0x026,
    APP_GAF_SCAN_STREAM_REPORT_IND          = 0x027,
    APP_GAF_SCAN_BIGINFO_REPORT_IND         = 0x028,
    APP_GAF_SCAN_PA_INFO_REPORT_IND         = 0x029,

    // BIS Sink Events
    APP_GAF_SINK_BIS_STATUS_IND             = 0x030,
    APP_GAF_SINK_BIS_STREAM_STARTED_IND     = 0x031,
    APP_GAF_SINK_BIS_STREAM_STOPPED_IND     = 0x032,

    // BIS Delegator Events
    APP_GAF_DELEG_SOLICITE_STARTED_IND      = 0x040,
    APP_GAF_DELEG_SOLICITE_STOPPED_IND      = 0x041,
    APP_GAF_DELEG_REMOTE_SCAN_STARTED_IND   = 0x042,
    APP_GAF_DELEG_REMOTE_SCAN_STOPPED_IND   = 0x043,
    APP_GAF_DELEG_SOURCE_ADD_RI             = 0x044,
    APP_GAF_DELEG_SOURCE_REMOVE_RI          = 0x045,
    APP_GAF_DELEG_SOURCE_UPDATE_RI          = 0x046,

    // MCC Events
    APP_GAF_MCC_SVC_DISCOVERYED_IND         = 0x050,
    APP_GAF_MCC_TRACK_CHANGED_IND           = 0x051,
    APP_GAF_MCC_MEDIA_VALUE_IND             = 0x052,
    APP_GAF_MCC_MEDIA_VALUE_LONG_IND        = 0x053,
    APP_GAF_MCC_SVC_CHANGED_IND             = 0x054,

    // TBC Events
    APP_GAF_TBC_SVC_DISCOVERYED_IND         = 0x060,
    APP_GAF_TBC_CALL_STATE_IND              = 0x061,
    APP_GAF_TBC_CALL_STATE_LONG_IND         = 0x062,
    APP_GAF_TBC_CALL_VALUE_IND              = 0x063,
    APP_GAF_TBC_CALL_VALUE_LONG_IND         = 0x064,
    APP_GAF_TBC_SVC_CHANGED_IND             = 0x065,

    // AICS Event
    APP_GAF_AICS_STATE_IND                  = 0x070,

    // MICS Event
    APP_GAF_MICS_MUTE_IND                   = 0x080,

    // VCS Events
    APP_GAF_VCS_VOLUME_IND                  = 0x090,
    APP_GAF_VCS_FLAGS_IND                   = 0x091,

    // VOCS Events
    APP_GAF_VOCS_LOCATION_SET_RI            = 0x0A0,
    APP_GAF_VOCS_OFFSET_IND                 = 0x0A1,

    // CSISM Events
    APP_GAF_CSISM_LOCK_IND                  = 0x0B0,
    APP_GAF_CSISM_AUTHORIZATION_RI          = 0x0B1,

    APP_GAF_EVENT_EARBUD_MAX                = 0x100,
    APP_GAF_EVENT_LAST                      = 0x1FF
} aob_mgr_gaf_evt_e;

/// Structure for BAP_UC_SRV_CIS_STATE indication message
typedef struct
{
    /// Indication code (@see enum bap_uc_srv_ind_code)
    uint16_t ind_code;
    /// ASE local index for Sink direction
    uint8_t ase_lid_sink;
    /// ASE local index for Source direction
    uint8_t ase_lid_src;
    /// CIG ID
    uint8_t cig_id;
    /// CIS ID
    uint8_t cis_id;
    /// Connection handle allocated by the controller
    /// GAP_INVALID_CONHDL means that the CIS is not established
    uint16_t conhdl;
} app_gaf_uc_srv_cis_state_ind_t;

/// Structure for cis stream started indication message
typedef struct
{
    uint8_t ase_lid;
    uint16_t cis_hdl;
    uint8_t direction;
} app_gaf_ascs_cis_stream_started_t;

/// Structure for cis stream stopped indication message
typedef struct
{
    uint8_t ase_lid;
    uint16_t cis_hdl;
    uint8_t direction;
} app_gaf_ascs_cis_stream_stopped_t;

/// Structure for BAP_UC_SRV_CONFIGURE_CODEC request indication message
typedef struct
{
    /// Request indication code (@see enum bap_uc_srv_req_ind_code)
    ///  - BAP_UC_SRV_CONFIGURE_CODEC_RI
    uint16_t req_ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// ASE instance index
    uint8_t ase_instance_idx;
    /// ASE local index
    uint8_t ase_lid;
    /// Direction (@see enum gaf_direction)
    uint8_t direction;
    /// Target Latency (@see enum bap_uc_tgt_latency)
    uint8_t tgt_latency;
    /// Target PHY (@see enum bap_uc_tgt_phy)
    uint8_t tgt_phy;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Length of Codec Configuration value
    uint16_t cfg_len;
    /// Codec Configuration value, length is cfg_len
    /// To be casted as @see bap_lc3_cfg_t for LC3
    /// To be casted as @see bap_vendor_codec_cfg_t for Vendor Codec
    uint8_t cfg[__ARRAY_EMPTY];
} app_gaf_uc_srv_configure_codec_req_ind_t;

/// Structure for BAP_UC_SRV_ENABLE request indication message
typedef struct
{
    /// Request indication code (@see enum bap_uc_srv_req_ind_code)
    ///  - BAP_UC_SRV_ENABLE
    uint16_t req_ind_code;
    /// ASE local index
    uint8_t ase_lid; 
    /// Metadata structure
    app_bap_metadata_cfg_t metadata;
} app_gaf_uc_srv_enable_req_ind_t;

/// Structure for BAP_UC_SRV_UPDATE_METADATA request indication message
typedef struct
{
    /// Request indication code (@see enum bap_uc_srv_req_ind_code)
    ///  - BAP_UC_SRV_UPDATE_METADATA_RI
    uint16_t req_ind_code;
    /// ASE local index
    uint8_t ase_lid;
    /// Metadata structure
    app_bap_metadata_cfg_t metadata;
} app_gaf_uc_srv_update_metadata_req_ind_t;

/// Structure for BAP_UC_SRV_RELEASE request indication message
typedef struct
{
    /// Request indication code (@see enum bap_uc_srv_req_ind_code)
    ///  - BAP_UC_SRV_RELEASE_RI
    uint16_t req_ind_code;
    /// ASE local index
    uint8_t ase_lid;
} app_gaf_uc_srv_release_req_ind_t;

/// Structure for BAP_CAPA_SRV_LOCATION indication message
typedef struct
{
    /// Indication code (@see enum bap_capa_srv_ind_code)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Direction (@see enum gaf_direction)
    uint8_t direction;
    /// Location bit field
    uint32_t location_bf;
} app_gaf_capa_srv_location_ind_t;

/// Structure for scan report indication message
typedef struct
{
    uint8_t scan_trigger_method;
    int8_t rssi;
    app_gaf_extend_adv_report_t adv_report;
} app_gaf_bc_scan_adv_report_t;

/// Structure for BAP_BC_SCAN_PA_REPORT indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_PA_REPORT
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// Length of advertising data
    uint8_t              length;
    /// Complete periodic advertising report
    uint8_t              data[__ARRAY_EMPTY];
} app_gaf_bc_scan_pa_report_ind_t;

/// Structure for BAP_BC_SCAN_PA_ESTABLISHED indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_PA_ESTABLISHED
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// Periodic advertising address information
    app_gaf_per_adv_bdaddr_t adv_addr;
    /// Only valid for a Periodic Advertising Sync Transfer, else ignore
    uint16_t             serv_data;
} app_gaf_bc_scan_pa_established_ind_t;

/// Structure for BAP_BC_SCAN_PA_TERMINATED indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_PA_TERMINATED
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// Stop reason (@see enum gaf_err)
    uint16_t             reason;
} app_gaf_bc_scan_pa_terminated_ind_t;

/// Structure for BAP_BC_SCAN_GROUP_REPORT indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_GROUP_REPORT
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// Number of subgroups in the Broadcast Group
    uint8_t              nb_subgroups;
    /// Total number of streams in the Broadcast Group
    uint8_t              nb_streams;
    /// Codec ID value
    app_gaf_codec_id_t       codec_id;
    /// Audio output presentation delay in microseconds
    uint32_t             pres_delay_us;
    /// Length of Codec Configuration value
    uint8_t              cfg_len;
    /// Length of Metadata value
    uint8_t              metadata_len;
    /// Codec Configuration value followed by Metadata value
    /// Both values are in LTV format
    uint8_t              val[__ARRAY_EMPTY];
} app_gaf_bc_scan_group_report_ind_t;

/// Structure for BAP_BC_SCAN_SUBGROUP_REPORT indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_SUBGROUP_REPORT
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// Subgroup identifier
    uint8_t              sgrp_id;
    /// Codec ID value
    /// (Octet  = 0xFE if equals to group codec, surcharged otherwise)
    app_gaf_codec_id_t       codec_id;
    /// Stream position index bit field indicating for which streams are part of the subgroup
    uint32_t             stream_pos_bf;
    /// Length of Codec Configuration value
    uint8_t              cfg_len;
    /// Length of Metadata value
    uint8_t              metadata_len;
    /// Codec Configuration value followed by Metadata value
    /// Both values are in LTV format
    uint8_t              val[__ARRAY_EMPTY];
} app_gaf_bc_scan_subgroup_report_ind_t;

/// Structure for BAP_BC_SCAN_STREAM_REPORT indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_STREAM_REPORT
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// Stream position in group
    uint8_t              stream_pos;
    /// Codec configuration length
    uint8_t              cfg_len;
    /// Codec configuration data in LTV format
    uint8_t              cfg[__ARRAY_EMPTY];
} app_gaf_bc_scan_stream_report_ind_t;

/// BIG Info Report
typedef struct
{
    /// Value of the SDU interval in microseconds (Range 0x0000FF-0x0FFFFF)
    uint32_t  sdu_interval;
    /// Value of the ISO Interval (1.25 ms unit)
    uint16_t  iso_interval;
    /// Value of the maximum PDU size (Range 0x0000-0x00FB)
    uint16_t  max_pdu;
    /// VValue of the maximum SDU size (Range 0x0000-0x0FFF)
    uint16_t  max_sdu;
    /// Number of BIS present in the group (Range 0x01-0x1F)
    uint8_t   num_bis;
    /// Number of sub-events (Range 0x01-0x1F)
    uint8_t   nse;
    /// Burst number (Range 0x01-0x07)
    uint8_t   bn;
    /// Pre-transmit offset (Range 0x00-0x0F)
    uint8_t   pto;
    /// Initial retransmission count (Range 0x01-0x0F)
    uint8_t   irc;
    /// PHY used for transmission (0x01: 1M, 0x02: 2M, 0x03: Coded, All other values: RFU)
    uint8_t   phy;
    /// Framing mode (0x00: Unframed, 0x01: Framed, All other values: RFU)
    uint8_t   framing;
    /// True if broadcast isochronous group is encrypted, False otherwise
    bool      encrypted;
} app_gaf_big_info_t;

/// Structure for BAP_BC_SCAN_BIG_INFO_REPORT indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_scan_ind_codes)
    /// - BAP_BC_SCAN_BIG_INFO_REPORT
    uint16_t             ind_code;
    /// Periodic Advertising local index
    uint8_t              pa_lid;
    /// BIG Info Report
    app_gaf_big_info_t       report;
} app_gaf_bc_scan_big_info_report_ind_t;

/// Broadcast group configuration structure (provided by controller after stream establisment)
typedef struct
{
    /// Transmission delay time in microseconds of all BISs in the BIG (in us range 0x0000EA-0x7FFFFF)
    uint32_t sync_delay_us;
    /// The maximum delay time, in microseconds, for transmission of SDUs of all BISes
    /// (in us range 0x0000EA-0x7FFFFF)
    uint32_t tlatency_us;
    /// PHY used, bit 0: 1Mbps, bit 1: 2Mbps, bit 2: LE-Coded
    uint8_t phy;
    /// The number of subevents in each BIS event in the BIG, range 0x01-0x1E
    uint8_t nse;
    /// The number of new payloads in each BIS event, range 0x01-0x07
    uint8_t bn;
    /// Offset used for pre-transmissions, range 0x00-0x0F
    uint8_t pto;
    /// The number of times a payload is transmitted in a BIS event, range 0x01-0x0F
    uint8_t irc;
    /// Maximum size of the payload in octets, range 0x00-0xFB
    uint8_t max_pdu;
} app_gaf_iap_bg_config_t;

/// Structure for BAP_BC_SINK_STATUS indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_sink_ind_codes)
    /// - BAP_BC_SINK_STATUS
    uint16_t            ind_code;
    /// Group local index
    uint8_t             grp_lid;
    /// Broadcast Sink state (@see enum bap_bc_sink_state)
    uint8_t             state;
    /// Stream position bit field in broadcast group to receive
    uint32_t            stream_pos_bf;
    /// When sync established, provides information about broadcast group else meaningless
    app_gaf_iap_bg_config_t     bg_cfg;
} app_gaf_bc_sink_status_ind_t;

/// Structure for BAP_BC_DELEG_SOLICITE_STOPPED indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_deleg_ind_code)
    ///  - BAP_BC_DELEG_SOLICITE_STOPPED
    uint16_t ind_code;
    /// Reason why sending of solicitation request has been stopped (@see enum bap_bc_deleg_reason)
    uint16_t reason;
} app_gaf_bc_deleg_solicite_stopped_ind_t;

/// Structure for BAP_BC_DELEG_BOND_REMOTE_SCAN indication message
typedef struct
{
    /// Indication code (@see enum bap_bc_deleg_ind_code)
    ///  - BAP_BC_DELEG_BOND_REMOTE_SCAN
    uint16_t ind_code;
    /// Connection local index
    uint8_t  con_lid;
    /// Broadcast Assistant device scan state (@see enum bap_bc_deleg_scan_state)
    uint8_t  state;
} app_gaf_bc_deleg_bond_remote_scan_ind_t;

/// Data value in LTV format
typedef struct
{
    /// Length of data value
    uint8_t len;
    /// Data value
    uint8_t data[__ARRAY_EMPTY];
} app_gaf_gaf_ltv_t;

/// Structure for BAP_BC_DELEG_SOURCE_ADD_RI request indication message
typedef struct
{
    /// Request Indication code (@see enum bap_bc_deleg_req_ind_code)
    ///  - BAP_BC_DELEG_SOURCE_ADD_RI
    uint16_t             req_ind_code;
    /// Allocated Source local index
    uint8_t              src_lid;
    /// Connection local index for device that has added the source
    uint8_t              con_lid;
    /// Periodic advertising address
    app_gaf_per_adv_bdaddr_t addr;
    /// Metadata in LTV format
    app_gaf_gaf_ltv_t    metadata;
} app_gaf_bc_deleg_source_add_req_ind_t;

/// Structure for BAP_BC_DELEG_SOURCE_REMOVE_RI request indication message
typedef struct
{
    /// Request Indication code (@see enum bap_bc_deleg_req_ind_code)
    ///  - BAP_BC_DELEG_SOURCE_REMOVE_RI
    uint16_t             req_ind_code;
    /// Source local index
    uint8_t              src_lid;
    /// Connection local index
    uint8_t              con_lid;
} app_gaf_bc_deleg_source_remove_req_ind_t;

/// Structure for BAP_BC_DELEG_SOURCE_UPDATE_RI request indication message
typedef struct
{
    /// Request Indication code (@see enum bap_bc_deleg_req_ind_code)
    ///  - BAP_BC_DELEG_SOURCE_UPDATE_RI
    uint16_t             req_ind_code;
    /// Source local index
    uint8_t              src_lid;
    /// Connection local index
    uint8_t              con_lid;
    /// Metadata in LTV format
    app_gaf_gaf_ltv_t            metadata;
} app_gaf_bc_deleg_source_update_req_ind_t;

/// Structure for command complete event message
typedef struct
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Connection local index
    uint8_t con_lid;
    /// Media local index
    uint8_t media_lid;
    union
    {
        /// Additional parameter
        uint8_t param;
        /// Characteristic type
        uint8_t char_type;
        /// Operation code
        uint8_t opcode;
    } u;
    /// Result
    uint8_t result;
} app_gaf_acc_mcc_cmp_evt_t;

/// Structure for ACC_MCC_TRACK_CHANGED indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Media local index
    uint8_t media_lid;
} app_gaf_acc_mcc_track_changed_ind_t;

/// Structure for ACC_MCC_VALUE indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Media local index
    uint8_t media_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Value
    union
    {
        /// Value
        uint32_t val;
        /// Playback speed
        int8_t playback_speed;
        /// Seeking speed
        int8_t seeking_speed;
        /// Media state
        uint8_t state;
        /// Supported media control operation codes bit field
        uint32_t opcodes_supp_bf;
        /// Playing Order
        uint8_t playing_order;
        /// Supported Playing Order bit field
        uint32_t playing_order_supp_bf;
        /// Track duration of the current track in 0.01 second resolution
        int32_t track_dur;
        /// Track position of the current track in 0.01 second resolution
        int32_t track_pos;
        /// Content Control ID
        uint8_t ccid;
    } val;
} app_gaf_acc_mcc_value_ind_t;

/// Structure for ACC_MCC_VALUE_LONG indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Media local index
    uint8_t media_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Length of value
    uint16_t val_len;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} app_gaf_acc_mcc_value_long_ind_t;

/// Structure for ACC_MCC_SVC_CHANGED indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
} app_gaf_acc_mcc_svc_changed_ind_t;

/// Structure for command complete event message
typedef struct
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    union
    {
        /// Value
        uint8_t val;
        /// Characteristic type
        uint8_t char_type;
        /// Operation code
        uint8_t opcode;
    } u;
    /// Call index
    uint8_t call_id;
    /// Result
    uint8_t result;
} app_gaf_acc_tbc_cmp_evt_t;

/// Structure for ACC_TBC_CALL_STATE indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t id;
    /// Call flags
    uint8_t flags;
    /// Call state
    uint8_t state;
} app_gaf_acc_tbc_call_state_ind_t;

/// Structure for ACC_TBC_CALL_STATE_LONG indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t id;
    /// Call flags
    uint8_t flags;
    /// Call state, @see enum acc_tb_call_state
    uint8_t state;
    /// Length of Incoming or Outgoing Call URI value
    uint8_t uri_len;
    /// Incoming or Outgoing Call URI value
    uint8_t uri[__ARRAY_EMPTY];
} app_gaf_acc_tbc_call_state_long_ind_t;

/// Structure for ACC_TBC_VALUE indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    /// Meaningful only for Termination Reason characteristic
    uint8_t call_id;
    /// Characteristic type
    uint8_t char_type;
    /// Value
    union
    {
        /// Value
        uint16_t val;
        /// Bearer Technology
        uint8_t techno;
        /// Signal Strength
        uint8_t sign_strength;
        /// Signal Strength Reporting Interval in seconds
        uint8_t sign_strength_intv_s;
        /// Content Control ID
        uint8_t ccid;
        /// Status Flags bit field
        uint16_t status_flags_bf;
        /// Call Control Point Optional Opcodes bit field
        uint16_t opt_opcodes_bf;
        /// Termination Reason
        uint8_t term_reason;
    } val;
} app_gaf_acc_tbc_value_ind_t;

/// Structure for ACC_TBC_VALUE_LONG indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t call_id;
    /// Characteristic type
    uint8_t char_type;
    /// Length of value
    uint16_t val_len;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} app_gaf_acc_tbc_value_long_ind_t;

/// Structure for ACC_TBC_SVC_CHANGED indication message
typedef struct
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
} app_gaf_acc_tbc_svc_changed_ind_t;

/// Structure for ARC_AICS_STATE indication message
typedef struct
{
    /// Indication code (@see enum arc_aics_ind_code)
    ///  - ARC_AICS_STATE
    uint16_t ind_code;
    /// Input local index
    uint8_t input_lid;
    /// Gain
    int8_t gain;
    /// Gain Mode
    uint8_t gain_mode;
    /// Mute
    uint8_t mute;
} app_gaf_arc_aics_state_ind_t;

/// Structure for ARC_MICS_MUTE indication message
typedef struct
{
    /// Indication code (@see enum arc_mics_ind_code)
    ///  - ARC_MICS_MUTE
    uint16_t ind_code;
    /// Mute value
    uint8_t mute;
} app_gaf_arc_mics_mute_ind_t;

/// Structure for ARC_VCS_VOLUME indication message
typedef struct
{
    /// Indication code (@see enum arc_vcs_ind_code)
    ///  - ARC_VCS_VOLUME
    uint16_t ind_code;
    /// Volume
    uint8_t volume;
    /// Mute
    uint8_t mute;
} app_gaf_arc_vcs_volume_ind_t;

/// Structure for ARC_VCS_FLAGS indication message
typedef struct
{
    /// Indication code (@see enum arc_vcs_ind_code)
    ///  - ARC_VCS_FLAGS
    uint16_t ind_code;
    /// Volume Flags
    uint8_t flags;
} app_gaf_arc_vcs_flags_ind_t;

/// Structure for ARC_VOCS_SET_LOCATION request indication message
typedef struct
{
    /// Request Indication code (@see enum arc_vocs_req_ind_code)
    ///  - ARC_VOCS_SET_LOCATION
    uint16_t req_ind_code;
    /// Output local index
    uint8_t output_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Audio location
    uint8_t location;
} app_gaf_arc_vocs_set_location_req_ind_t;

/// Structure for ARC_VOCS_OFFSET indication message
typedef struct
{
    /// Indication code (@see enum arc_vocs_ind_code)
    ///  - ARC_VOCS_OFFSET
    uint16_t ind_code;
    /// Output local index
    uint8_t output_lid;
    /// Offset
    int16_t offset;
} app_gaf_arc_vocs_offset_ind_t;

/// Structure for CSISM_LOCK indication message
typedef struct
{
    /// Indication code
    uint16_t ind_code;
    /// Coordinated Set local index
    uint8_t set_lid;
    /// New lock state
    uint8_t lock;
    /// Connection local index of connection for which Coordinated Set has been locked
    /// or was locked
    uint8_t con_lid;
    /// Reason why Coordinated Set is not locked anymore
    uint8_t reason;
} app_gaf_atc_csism_lock_ind_t;

/// Structure for CSISM_AUTHORIZATION request indication message
typedef struct
{
    /// Request indication code
    uint16_t req_ind_code;
    /// Coordinated Set local index
    uint8_t set_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Action for which authorization is required
    uint8_t action;
} app_gaf_atc_csism_authorization_req_ind_t;

typedef union {
    //BAP ASCS Callback Param
    app_gaf_uc_srv_cis_state_ind_t  ascs_cis_established;
    app_gaf_uc_srv_cis_state_ind_t  ascs_cis_disconnected;
    app_gaf_ascs_cis_stream_started_t   ascs_cis_stream_started;
    app_gaf_ascs_cis_stream_stopped_t   ascs_cis_stream_stopped;
    app_gaf_uc_srv_configure_codec_req_ind_t    ascs_configure_codec_ri;
    app_gaf_uc_srv_enable_req_ind_t ascs_enable_ri;
    app_gaf_uc_srv_update_metadata_req_ind_t    ascs_update_metadata_ri;
    app_gaf_uc_srv_release_req_ind_t    ascs_release_ri;

    //BAP PACS Callback Param
    app_gaf_capa_srv_location_ind_t pacs_location_set;

    //BAP BIS Scan Callback Functions
    app_gaf_bc_scan_adv_report_t scan_adv_report;
    app_gaf_bc_scan_pa_report_ind_t scan_pa_report;
    app_gaf_bc_scan_pa_established_ind_t scan_pa_established;
    app_gaf_bc_scan_pa_terminated_ind_t scan_pa_terminated;
    app_gaf_bc_scan_group_report_ind_t scan_group_report;
    app_gaf_bc_scan_subgroup_report_ind_t scan_subgroup_report;
    app_gaf_bc_scan_stream_report_ind_t scan_stream_report;
    app_gaf_bc_scan_big_info_report_ind_t scan_biginfo_report;
    app_bap_bc_scan_pa_report_info_t scan_pa_info_report;

    //BAP BIS Sink Callback Param
    app_gaf_bc_sink_status_ind_t sink_bis_status;
    uint16_t sink_bis_stream_started_hdl;
    uint16_t sink_bis_stream_stopped_hdl;

    //BAP BIS Delegator Callback Param
    app_gaf_bc_deleg_solicite_stopped_ind_t deleg_solicite_stopped;
    app_gaf_bc_deleg_bond_remote_scan_ind_t deleg_remote_scan_started;
    app_gaf_bc_deleg_bond_remote_scan_ind_t deleg_remote_scan_stopped;
    app_gaf_bc_deleg_source_add_req_ind_t deleg_source_add_ri;
    app_gaf_bc_deleg_source_remove_req_ind_t deleg_source_remove_ri;
    app_gaf_bc_deleg_source_update_req_ind_t deleg_source_update_ri;

    //ACC MCC Callback Param
    app_gaf_acc_mcc_cmp_evt_t mcc_svc_discoveryed;
    app_gaf_acc_mcc_track_changed_ind_t mcc_track_changed;
    app_gaf_acc_mcc_value_ind_t mcc_media_value;
    app_gaf_acc_mcc_value_long_ind_t mcc_media_value_long;
    app_gaf_acc_mcc_svc_changed_ind_t mcc_svc_changed;

    //ACC TBC Callback Param
    app_gaf_acc_tbc_cmp_evt_t tbc_svc_discoveryed;
    app_gaf_acc_tbc_call_state_ind_t tbc_call_state;
    app_gaf_acc_tbc_call_state_long_ind_t tbc_call_state_long;
    app_gaf_acc_tbc_value_ind_t tbc_call_value;
    app_gaf_acc_tbc_value_long_ind_t tbc_call_value_long;
    app_gaf_acc_tbc_svc_changed_ind_t tbc_svc_changed;

    //ARC AICS Callback Param
    app_gaf_arc_aics_state_ind_t aics_state;

    //ARC MICS Callback Param
    app_gaf_arc_mics_mute_ind_t mics_mute;

    //ARC VCS Callback Param
    app_gaf_arc_vcs_volume_ind_t vcs_volume;
    app_gaf_arc_vcs_flags_ind_t vcs_flags;

    //ARC VOCS Callback Param
    app_gaf_arc_vocs_set_location_req_ind_t vocs_location_set_ri;
    app_gaf_arc_vocs_offset_ind_t vocs_offset;

    //ATC CSISM Callback Param
    app_gaf_atc_csism_lock_ind_t csism_lock;
    app_gaf_atc_csism_authorization_req_ind_t csism_authorization_ri;
} ble_audio_event_t;

#ifdef __cplusplus
extern "C" {
#endif

void app_gaf_event_cb(aob_mgr_gaf_evt_e evt_type, void *evt, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // APP_GAF_EVENT_CALLBACK_H_

/// @} APP_GAF_EVENT_CALLBACK_H_

