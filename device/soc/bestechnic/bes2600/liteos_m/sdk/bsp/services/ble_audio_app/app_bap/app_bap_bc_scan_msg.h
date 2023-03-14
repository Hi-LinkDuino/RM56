/**
 ****************************************************************************************
 *
 * @file app_bap_bc_scan_msg.h
 *
 * @brief BLE Audio Broadcast Scan
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

/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_BAP_BC_SACN_MSG_H_
#define APP_BAP_BC_SACN_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"

#define APP_BAP_DFT_BC_SCAN_CACHE_SIZE                 2
#define APP_BAP_DFT_BC_SCAN_NB_SYNC                    1
#define APP_BAP_DFT_BC_SCAN_SCAN_TIMEOUT_S             0
#define APP_BAP_DFT_BC_SCAN_INTV_1M_SLOT               0x30
#define APP_BAP_DFT_BC_SCAN_WD_1M_SLOT                 0x20
#define APP_BAP_DFT_BC_SCAN_INTV_CODED_SLOT            0
#define APP_BAP_DFT_BC_SCAN_WD_CODED_SLOT              0
#define APP_BAP_DFT_BC_SCAN_SKIP                       1
#define APP_BAP_DFT_BC_SCAN_REPORT_FILTER_BF           0x1F
#define APP_BAP_DFT_BC_SCAN_SYNC_TO                    1000 //Uint 10ms
#define APP_BAP_DFT_BC_SCAN_TIMEOUT_S                  0 //Always scan periodic adv
#define APP_BAP_PER_ADV_DATA_MAX_LEN                   247

///  scan group state
enum app_bap_bc_scan_state
{
    ///  group in idle state
    APP_BAP_BC_SCAN_STATE_IDLE = 0,
    ///  group in scanning state
    APP_BAP_BC_SCAN_STATE_SCANNING,
    ///  group in periodic synchronizing state
    APP_BAP_BC_SCAN_STATE_SYNCHRONIZING,
    ///  group in periodic synchronized state
    APP_BAP_BC_SCAN_STATE_SYNCHRONIZED,
    ///  group in stream state
    APP_BAP_BC_SCAN_STATE_STREAMING,
};

/// scan method
enum app_bap_bc_scan_method
{
    ///  sink scan broadcast src directly
    APP_BAP_BC_SINK_SCAN = 0,
    ///  assist scan broadcast src on behalf of deleg
    APP_BAP_BC_ASSIST_SCAN,
};

/// scan trigger method
enum app_bap_bc_scan_trigger_method
{
    ///  sink scan broadcast src directly
    APP_BAP_BC_SINK_TRIGGER = 0,
    ///  assist scan broadcast src on behalf of deleg
    APP_BAP_BC_ASSIST_TRIGGER,
    ///  deleg scan pa after solicition
    APP_BAP_BC_DELEG_TRIGGER,
};

/// Bit Field of periodic advertising information that is supposed to be report by scan module
/// Filtering level to report information about basic audio announcement present in periodic advertising report
enum app_bap_bc_scan_report_filter_bf
{
    /// Report Periodic advertising data (in raw format)
    APP_BAP_BC_SCAN_REPORT_PER_ADV_DATA_BIT     = 0x01,
    /// Report BIG Info present in ACAD data of a periodic advertising report
    APP_BAP_BC_SCAN_REPORT_BIG_INFO_BIT         = 0x02,
    /// Report Broadcast Group information of basic audio announcement present in periodic advertising report
    APP_BAP_BC_SCAN_REPORT_ANNOUNCE_LVL_1_BIT   = 0x04,
    /// Report Broadcast SubGroups information of basic audio announcement present in periodic advertising report
    APP_BAP_BC_SCAN_REPORT_ANNOUNCE_LVL_2_BIT   = 0x08,
    /// Report Broadcast Stream information of basic audio announcement present in periodic advertising report
    APP_BAP_BC_SCAN_REPORT_ANNOUNCE_LVL_3_BIT   = 0x10,
};

/// Periodic Advertising Sync Params
typedef struct app_bap_bc_scan_sync_param
{
    /// Periodic advertising address information
    app_gaf_per_adv_bdaddr_t adv_addr;
    /// Number of periodic advertising that can be skipped after a successful receive.
    /// Maximum authorized value is 499
    uint16_t             skip;
    /// Bit field of filtered report types (@see enum bap_bc_scan_report_filter_bf)
    uint8_t              report_filter_bf;
    /// Synchronization timeout for the periodic advertising (in unit of 10ms between 100ms and 163.84s)
    uint16_t             sync_to;
    /// Scan Timeout in seconds
    uint16_t             timeout_s;
}app_bap_bc_scan_sync_param_t;

/// Scan Parameters structure
typedef struct app_bap_bc_scan_param
{
    /// Scan configuration bit field (see TODO)
    uint8_t  cfg_bf;
    /// Scan interval for LE 1M PHY in multiple of 0.625ms - Must be higher than 2.5ms
    uint16_t intv_1m_slot;
    /// Scan interval for LE Codec PHY in multiple of 0.625ms - Must be higher than 2.5ms
    uint16_t intv_coded_slot;
    /// Scan window for LE 1M PHY in multiple of 0.625ms - Must be higher than 2.5ms
    uint16_t wd_1m_slot;
    /// Scan window for LE Codec PHY in multiple of 0.625ms - Must be higher than 2.5ms
    uint16_t wd_coded_slot;
}app_bap_bc_scan_param_t;

/// Stream information
typedef struct app_bap_bc_scan_stream
{
    /// Stream position in group
    uint8_t              stream_pos;
    /// Codec configuration length
    uint8_t              cfg_len;
    /// Codec configuration data in LTV format
    uint8_t              *cfg;
} app_bap_bc_scan_stream_t;

/// subgroup information
typedef struct app_bap_bc_scan_subgrp
{
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
    uint8_t              *val;
} app_bap_bc_scan_subgrp_t;

/// union that contains stream or subgroup information
typedef union app_bap_bc_scan_subgrp_stream
{
    /// Subgroup data information
    app_bap_bc_scan_subgrp_t subgrp;
    /// Stream data information
    app_bap_bc_scan_stream_t stream;
} app_bap_bc_scan_subgrp_stream_t;

typedef struct app_bap_bc_scan_base_info
{
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
    uint8_t              *val;
    /// Subgroup/Stream/ content
    /// [0, nb_subgroups] contains subgroups configuration
    /// [nb_streams,nb_subgroups+nb_streams] contains streams configuration
    app_bap_bc_scan_subgrp_stream_t *sgrp_stream;
}app_bap_bc_scan_base_info_t;

/// BIG Info Report
typedef struct app_bap_bc_scan_big_info
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
} app_bap_bc_scan_big_info_t;

typedef struct app_bap_bc_scan_pa_report_info
{
    /// Periodic Advertising local index
    uint8_t pa_lid;
    /// Length of periodic advertising data
    uint8_t length;
    /// Complete periodic advertising report
    uint8_t pa_adv_data[APP_BAP_PER_ADV_DATA_MAX_LEN];
    /// Broadcast Audio Source Endpoint (BASE) Info
    app_bap_bc_scan_base_info_t       base_info;
    /// BIG Info
    app_bap_bc_scan_big_info_t       big_info;
}app_bap_bc_scan_pa_report_info_t;

/// Content of Broadcast Scan environment
typedef struct app_bap_bc_scan_env
{
    /// Number of Broadcast Source information that may be stored in the cache
    uint8_t cache_size;
    /// Number of Periodic Synchronizations that may be established in parallel
    uint8_t nb_sync;
    // Scanning trigger method
    uint8_t scan_trigger_method;
    // Scanning State
    uint8_t scan_state;
    /// Timeout duration in seconds
    uint16_t scan_timeout_s;
    /// Scan parameters
    app_bap_bc_scan_param_t scan_param;
    /// PA Sync Params
    app_bap_bc_scan_sync_param_t sync_param;
    // PA Report Information
    app_bap_bc_scan_pa_report_info_t pa_info[APP_BAP_DFT_BC_SCAN_NB_SYNC];
}app_bap_bc_scan_env_t;


#ifdef __cplusplus
extern "C" {
#endif

uint32_t app_bap_bc_scan_cmp_evt_handler(void const *param);
uint32_t app_bap_bc_scan_ind_handler(void const *param);
uint32_t app_bap_bc_scan_req_ind_handler(void const *param);
uint32_t app_bap_bc_scan_rsp_handler(void const *param);

void app_bap_bc_scan_init(void);
void app_bap_bc_scan_info_init(void);


#ifdef __cplusplus
}
#endif


#endif
#endif // APP_BAP_BC_SACN_MSG_H_

/// @} APP_BAP
