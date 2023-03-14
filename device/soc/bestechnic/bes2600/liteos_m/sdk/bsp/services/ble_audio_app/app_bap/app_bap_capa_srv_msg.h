/**
 ****************************************************************************************
 *
 * @file app_bap_capa_srv_msg.h
 *
 * @brief BLE Audio Published Audio Capabilities Server
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

#ifndef APP_BAP_CAPA_SRV_MSG_H_
#define APP_BAP_CAPA_SRV_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"
#include "co_list.h"

#define APP_BAP_DFT_CAPA_S_CODEC_ID               APP_GAF_CODEC_TYPE_LC3
#define APP_BAP_DFT_CAPA_S_SAMPLING_FREQ_BF       (GAF_BAP_SUPPORTED_SAMPLE_FREQ_48000 | GAF_BAP_SUPPORTED_SAMPLE_FREQ_24000 | GAF_BAP_SUPPORTED_SAMPLE_FREQ_16000)
#define APP_BAP_DFT_CAPA_S_FRAME_OCT_MIN          120
#define APP_BAP_DFT_CAPA_S_FRAME_OCT_MAX          120
#define APP_BAP_DFT_CAPA_S_PREFERRED_CONTEXT_BF   (APP_BAP_CONTEXT_TYPE_CONVERSATIONAL | APP_BAP_CONTEXT_TYPE_MEDIA)
#define APP_BAP_DFT_CAPA_S_FRAME_DURATION_BF      (GAF_BAP_SUPPORTED_FRAME_DURATION_7_5MS | GAF_BAP_SUPPORTED_FRAME_DURATION_10MS)
#define APP_BAP_DFT_CAPA_S_CHAN_CNT               1
#define APP_BAP_DFT_CAPA_S_MAX_FRAME_PER_SDU      1
#define APP_BAP_DFT_CAPA_S_ADD_CAPA_LEN           5
#define APP_BAP_DFT_CAPA_S_METADATA_CAPA_LEN      5

/// Available Context information structure
typedef struct app_bap_capa_ava_context
{
    /// Connection Local Index
    uint8_t con_lid;
    /// Available Context Type Bitfield
    uint16_t ava_bf;
} app_bap_capa_ava_context_t;

/// Direction information structure
typedef struct app_bap_capa_srv_dir
{
    /// Supported Audio Locations for the direction
    uint32_t location_bf;
    /// Available Audio Contexts for the direction (one for each connection)
    app_bap_capa_ava_context_t context_bf_ava[BLE_AUDIO_CONNECTION_CNT];
    /// Supported Audio Contexts for the direction
    uint16_t context_bf_supp;
    /// Number of PACs for the direction
    uint8_t nb_pacs;
} app_bap_capa_srv_dir_t;

/// PAC Record information structure
typedef struct app_bap_capa_srv_record
{
    /// List header
    co_list_hdr_t hdr;
    /// Record identifier
    uint8_t record_id;
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
} app_bap_capa_srv_record_t;

/// PAC information structure
typedef struct app_bap_capa_srv_pac
{
    /// List of records (@see app_bap_capa_srv_record_t for elements)
    co_list_t list_record;
    /// Number of records
    uint8_t nb_records;
} app_bap_capa_srv_pac_t;

/// Published Audio Capabilities Server environment structure
typedef struct app_bap_capa_srv_env
{
    /// Preferred Mtu
    uint8_t preferred_mtu;
    /// Total number of PAC
    uint8_t nb_pacs;
    /// Total number of PAC Records
    uint8_t nb_records;
    /// Pointer to Direction information structure
    app_bap_capa_srv_dir_t dir_info[APP_GAF_DIRECTION_MAX];
    /// PAC informations (nb_pacs elements)
    app_bap_capa_srv_pac_t* p_pac_info;
}app_bap_capa_srv_env_t;

uint32_t app_bap_capa_srv_rsp_handler (void const *param);
uint32_t app_bap_capa_srv_ind_handler (void const *param);

void app_bap_capa_srv_init(void);
void app_bap_capa_srv_info_init(app_bap_capa_srv_dir_t *sink_capa_info, app_bap_capa_srv_dir_t *src_capa_info);

#endif
#endif // APP_BAP_CAPA_SRV_MSG_H_

/// @} APP_BAP
