/**
 ****************************************************************************************
 *
 * @file app_bap_bc_src_msg.h
 *
 * @brief BLE Audio Broadcast Source
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

#ifndef APP_BAP_BC_SRC_MSG_H_
#define APP_BAP_BC_SRC_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"

//Broadcast source default value
#define APP_BAP_DFT_BC_SRC_MAX_BIG_NUM            1
#define APP_BAP_DFT_BC_SRC_MAX_SUBGRP_NUM         2
#define APP_BAP_DFT_BC_SRC_MAX_STREAM_NUM         IAP_NB_STREAMS
#define APP_BAP_DFT_BC_SRC_NB_STREAMS             1
#define APP_BAP_DFT_BC_SRC_NB_SUBGRPS             1
#define APP_BAP_DFT_BC_SRC_SDU_INTV_US            10000
#define APP_BAP_DFT_BC_SRC_MAX_SDU_SIZE           240
#define APP_BAP_DFT_BC_SRC_MAX_TRANS_LATENCY_MS   10
#define APP_BAP_DFT_BC_SRC_PACKING_TYPE           GAF_BAP_PACK_TYPE_SEQ
#define APP_BAP_DFT_BC_SRC_FRAMING_TYPE           GAF_BAP_FRAME_TYPE_FRAMED
#define APP_BAP_DFT_BC_SRC_PHY_BF                 GAF_BAP_PHY_2MBPS
#define APP_BAP_DFT_BC_SRC_RTN                    1
#define APP_BAP_DFT_BC_SRC_ADV_INTERVAL           256 //100ms  *0.625ms
#define APP_BAP_DFT_BC_SRC_PERIODIC_INTERVAL      80  //100ms  *1.25ms

#define APP_BAP_DFT_BC_SRC_PRES_DELAY_US          10000
#define APP_BAP_DFT_BC_SRC_METADATA_LEN           5
#define APP_BAP_DFT_BC_SRC_CODEC_ID               APP_GAF_CODEC_TYPE_LC3
#define APP_BAP_DFT_BC_SRC_ADD_CODEC_LEN          5
#define APP_BAP_DFT_BC_SRC_LOCATION_BF            (GAF_BAP_AUDIO_LOCATION_SIDE_LEFT | GAF_BAP_AUDIO_LOCATION_SIDE_RIGHT)
#define APP_BAP_DFT_BC_SRC_FRAME_OCTET            100
#define APP_BAP_DFT_BC_SRC_SAMPLING_FREQ          GAF_BAP_SAMPLE_FREQ_48000
#define APP_BAP_DFT_BC_SRC_FRAME_DURATION         GAF_BAP_FRAME_DURATION_10MS
#define APP_BAP_DFT_BC_SRC_NB_LC3_STREAM          0
#define APP_BAP_DFT_BC_SRC_IS_ENCRYPTED           0
#define APP_BAP_DFT_BC_SRC_DP_ID                  0
#define APP_BAP_DFT_BC_SRC_CTL_DELAY_US           0x0102

#define APP_BAP_DFT_BC_SRC_ADV_DATA               "bis_adv_test"
#define APP_BAP_DFT_BC_SRC_PA_ADV_DATA            "bis_pa_adv_test"
#define APP_BAP_DFT_BC_SRC_MAX_ADV_DATA_LEN       252


/// Broadcast source group state
enum app_bap_bc_src_state
{
    /// Broadcast group in idle state
    APP_BAP_BC_SRC_STATE_IDLE,
    /// Broadcast group in configured state
    /// - Periodic ADV started
    APP_BAP_BC_SRC_STATE_CONFIGURED,
    /// Broadcast group in stream state
    /// - broadcaster started
    APP_BAP_BC_SRC_STATE_STREAMING,
};

/// Stream information
typedef struct app_bap_bc_src_stream
{
    /// Stream local identifier
    uint8_t                  stream_lid;
    /// Subgroup local identifier
    uint8_t                  sgrp_lid;
    /// IAP configuration
    app_bap_dp_cfg_t          iap_cfg;
    /// Pointer to level 3 Codec configuration value (in LTV format)
    uint8_t cfg_len;
    /// Pointer to Codec Parameters structure
    union
    {
        /// Pointer to Codec Parameters structure
        void* p_codec_cfg;
        /// Pointer to LC3 Parameters structure
        app_bap_lc3_cfg_t* p_lc3_cfg;
        /// Pointer to Vendor Codec Parameters structure
        app_bap_vendor_codec_cfg_t* p_vendor_cfg;
    } u;
    /// BIS Handle
    uint16_t        bis_hdl;
} app_bap_bc_src_stream_t;

/// subgroup information
typedef struct app_bap_bc_src_subgrp
{
    /// Subgroup local identifier
    uint8_t                  sgrp_lid;
    /// Pointer to level 2 Metadata value
    app_gaf_ltv_t*         p_metadata;
    /// Stream local identifier bit field of stream lid present in subgroup
    uint32_t                 stream_lid_bf;
//    /// Pointer to level 2 Codec configuration value (in LTV format)
//    const app_gaf_ltv_t*           p_codec_cfg;
//    /// Codec Identifier - octet 0 = 0xFE if does not redefined group codec
//    app_gaf_codec_id_t           codec_id;
} app_bap_bc_src_subgrp_t;

/// union that contains stream or subgroup information
typedef union app_bap_bc_src_subgrp_stream
{
    /// Subgroup data information
    app_bap_bc_src_subgrp_t subgrp;
    /// Stream data information
    app_bap_bc_src_stream_t stream;
} app_bap_bc_src_subgrp_stream_t;


/// Broadcast Source BIG Information Structure
typedef struct app_bap_bc_src_grp
{
    /// Group local index
    uint8_t        grp_lid;
    /// BIG State
    uint8_t big_state;
    /// Number of Streams in the Broadcast Group. Cannot be 0
    uint8_t                nb_streams;
    /// Number of Subgroups in the Broadcast Group. Cannot be 0
    uint8_t                nb_subgroups;
    /// Broadcast Group parameters
    app_bap_bc_grp_param_t     grp_param;
    /// Advertising parameters
    app_bap_bc_adv_param_t     adv_param;
    /// Periodic Advertising parameters
    app_bap_bc_per_adv_param_t per_adv_param;
    // Audio output presentation delay in microseconds
    uint32_t pres_delay_us;
    /// Pointer to level 1 Metadata value
    app_gaf_ltv_t*           p_metadata;
    /// Codec Identifier
    app_gaf_codec_id_t             codec_id;
    /// Pointer to level 1 Codec configuration value
    uint8_t cfg_len;
    /// Pointer to Codec Parameters structure
    union
    {
        /// Pointer to Codec Parameters structure
        void* p_codec_cfg;
        /// Pointer to LC3 Parameters structure
        app_bap_lc3_cfg_t* p_lc3_cfg;
        /// Pointer to Vendor Codec Parameters structure
        app_bap_vendor_codec_cfg_t* p_vendor_cfg;
    } u;
    // Indicate if streams are encrypted (!= 0) or not
    uint8_t encrypted;
    // Broadcast code. Meaningful only if encrypted parameter indicates that streams are encrypted
    app_gaf_bcast_code_t bcast_code;
    /// Length of additional advertising data
    uint8_t        adv_data_len;
    /// Length of additional periodic advertising data
    uint8_t        per_adv_data_len;
    /// Additional advertising data followed by additional periodic advertising data
    uint8_t        *adv_data;
    /// Subgroup/Stream/ content
    /// [0, nb_subgroups] contains subgroups configuration
    /// [nb_streams,nb_subgroups+nb_streams] contains streams configuration
    app_bap_bc_src_subgrp_stream_t *cfg;
}app_bap_bc_src_grp_t;

/// Content of Broadcast source environment
typedef struct app_bap_bc_src_env
{
    /// BIG Information
    app_bap_bc_src_grp_t p_grp[APP_BAP_DFT_BC_SRC_MAX_BIG_NUM];
}app_bap_bc_src_env_t;


#ifdef __cplusplus
extern "C" {
#endif

uint32_t app_bap_bc_src_cmp_evt_handler(void const *param);
uint32_t app_bap_bc_src_rsp_handler(void const *param);

void app_bap_bc_src_init(void);
void app_bap_bc_src_info_init(void);
#ifdef __cplusplus
}
#endif


#endif
#endif // APP_BAP_BC_SRC_MSG_H_

/// @} APP_BAP
