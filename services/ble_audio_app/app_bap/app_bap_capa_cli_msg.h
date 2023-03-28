/**
 ****************************************************************************************
 *
 * @file app_bap_capa_cli_msg.h
 *
 * @brief BLE Audio Published Audio Capabilities Client
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

#ifndef APP_BAP_CAPA_CLI_MSG_H_
#define APP_BAP_CAPA_CLI_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"
#include "co_list.h"

#define APP_BAP_DFT_CAPA_C_SINK_LOCATION_BF       (GAF_BAP_AUDIO_LOCATION_SIDE_LEFT | GAF_BAP_AUDIO_LOCATION_SIDE_RIGHT)
#define APP_BAP_DFT_CAPA_C_SRC_LOCATION_BF        (GAF_BAP_AUDIO_LOCATION_SIDE_LEFT | GAF_BAP_AUDIO_LOCATION_SIDE_RIGHT)
#define APP_BAP_DFT_CAPA_C_SINK_CONTEXT_BF        (APP_BAP_CONTEXT_TYPE_CONVERSATIONAL | APP_BAP_CONTEXT_TYPE_MEDIA)
#define APP_BAP_DFT_CAPA_C_SRC_CONTEXT_BF         (APP_BAP_CONTEXT_TYPE_CONVERSATIONAL | APP_BAP_CONTEXT_TYPE_MEDIA)
#define APP_BAP_DFT_CAPA_C_NB_PAC_SINK            1
#define APP_BAP_DFT_CAPA_C_NB_PAC_SRC             1
#define APP_BAP_DFT_CAPA_C_CODEC_ID               APP_GAF_CODEC_TYPE_LC3
#define APP_BAP_DFT_CAPA_C_SAMPLING_FREQ_BF       (GAF_BAP_SUPPORTED_SAMPLE_FREQ_48000 | GAF_BAP_SUPPORTED_SAMPLE_FREQ_24000 | GAF_BAP_SUPPORTED_SAMPLE_FREQ_16000)
#define APP_BAP_DFT_CAPA_C_FRAME_OCT_MIN          20
#define APP_BAP_DFT_CAPA_C_FRAME_OCT_MAX          400
#define APP_BAP_DFT_CAPA_C_PREFERRED_CONTEXT_BF   (APP_BAP_CONTEXT_TYPE_CONVERSATIONAL | APP_BAP_CONTEXT_TYPE_MEDIA)
#define APP_BAP_DFT_CAPA_C_FRAME_DURATION_BF      (GAF_BAP_SUPPORTED_FRAME_DURATION_7_5MS | GAF_BAP_SUPPORTED_FRAME_DURATION_10MS)
#define APP_BAP_DFT_CAPA_C_CHAN_CNT               2
#define APP_BAP_DFT_CAPA_C_MAX_FRAME_PER_SDU      2
#define APP_BAP_DFT_CAPA_C_ADD_CAPA_LEN           5
#define APP_BAP_DFT_CAPA_C_METADATA_CAPA_LEN      5

/// PAC Record information structure
typedef struct app_bap_capa_cli_supp
{
    /// Supported Audio Locations for the direction
    uint32_t location_bf;
    /// Supported Audio Contexts for the direction
    uint16_t context_bf_supp;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Pointer to Codec Capabilities structure
    union
    {
        /// Codec Capabilities
        void* p_codec_capa;
        /// LC3 Codec Capabilities
        app_bap_lc3_capa_t* p_lc3_capa;
        /// Vendor Specific Codec Capabilities
        app_bap_vendor_codec_capa_t* p_vendor_capa;
    } u;
} app_bap_capa_cli_supp_t;

/// Published Audio Capabilities Client environment structure
typedef struct app_bap_capa_cli_env
{
    /// Preferred Mtu
    uint8_t preferred_mtu;
    /// Pointer to Direction information structure
    app_bap_capa_cli_supp_t capa_info[APP_GAF_DIRECTION_MAX];
}app_bap_capa_cli_env_t;

/// PAC Record information structure
typedef struct app_bap_capa_cfg
{
    /// List header
    co_list_hdr_t hdr;
    /// Configuration identifier
    uint8_t cfg_id;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Codec Capabilities Length
    uint8_t cfg_len;
    /// Pointer to Codec Configuration structure
    union
    {
        /// Codec Capabilities
        void *p_capa_cfg;
        /// LC3 Codec Capabilities
        app_bap_lc3_cfg_t *p_lc3_cfg;
        /// Vendor Specific Codec Capabilities
        app_bap_vendor_codec_cfg_t *p_vendor_cfg;
    }u;
}app_bap_capa_cfg_t;

/// PAC information structure
typedef struct app_bap_capa_pac_cfg
{
    /// List of records (@see app_bap_capa_cfg_t for elements)
    co_list_t list_cfg;
    /// Number of cfg
    uint8_t nb_cfg;
} app_bap_capa_pac_cfg_t;

/// PACC Direction Configuration structure
typedef struct app_bap_capa_dir_cfg
{
    /// Audio Locations for the direction
    uint32_t location_bf;
    /// Audio Contexts for the direction
    uint16_t context_bf;
    /// Number of PACs for the direction
    uint8_t nb_pacs;
} app_bap_capa_dir_cfg_t;

/// Published Audio Capabilities Client Configuration structure
typedef struct app_bap_capa_cfg_env
{
    /// Connection local index
    uint8_t con_lid;
    /// Total number of PAC Groups
    uint8_t nb_pacs;
    /// Total number of PAC Groups
    uint8_t nb_cfg;
    /// Pointer to Direction information structure
    app_bap_capa_dir_cfg_t dir_cfg[APP_GAF_DIRECTION_MAX];
    /// PAC informations (nb_pacs elements)
    app_bap_capa_pac_cfg_t* p_pac_cfg;
}app_bap_capa_cfg_env_t;

uint32_t app_bap_capa_cli_cmp_evt_handler (void const *param);
uint32_t app_bap_capa_cli_ind_handler (void const *param);

void app_bap_capa_cli_init(void);
void app_bap_capa_cli_info_init(void);
void app_bap_capa_cli_start(uint8_t con_idx);
uint8_t app_bap_capa_cli_get_con_idx(uint8_t con_lid);

#endif
#endif // APP_BAP_CAPA_CLI_MSG_H_

/// @} APP_BAP
