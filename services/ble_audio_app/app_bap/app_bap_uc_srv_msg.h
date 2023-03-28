/**
 ****************************************************************************************
 *
 * @file app_bap_uc_srv_msg.h
 *
 * @brief BLE Audio Audio Stream Control Service Server
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

#ifndef APP_BAP_UC_SRV_MSG_H_
#define APP_BAP_UC_SRV_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"
#include "co_list.h"

#define APP_BAP_DFT_ASCS_NB_ASE_CHAR              3
#define APP_BAP_DFT_ASCS_NB_ASE_CFG               (APP_BAP_DFT_ASCS_NB_ASE_CHAR)*(BLE_AUDIO_CONNECTION_CNT)
#define APP_BAP_DFT_ASCS_DP_ID                    0
#define APP_BAP_DFT_ASCS_CTL_DELAY_US             0x0102
#define APP_BAP_DFT_ASCS_MIN_SDU_INTERVAL_US      10000
#define APP_BAP_DFT_ASCS_MAX_SDU_INTERVAL_US      10000
#define APP_BAP_DFT_ASCS_MIN_PRES_DELAY_US        10000
#define APP_BAP_DFT_ASCS_MAX_PRES_DELAY_US        10000
#define APP_BAP_DFT_ASCS_MAX_TRANS_LATENCY_MS     20
#define APP_BAP_DFT_ASCS_MAX_SDU_SIZE             120
#define APP_BAP_DFT_ASCS_RTN                      1
#define APP_BAP_DFT_ASCS_PHY_BF                   GAF_BAP_PHY_2MBPS
#define APP_BAP_DFT_ASCS_FRAMING_TYPE             GAF_BAP_FRAME_TYPE_FRAMED

/// ASCS ASE Structure
typedef struct app_bap_ascs_ase
{
    /// Data Path Configuration
    app_bap_dp_cfg_t dp_cfg;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Codec Configuration length
    uint8_t cfg_len;
    /// Pointer to Codec Configuration structure
    union
    {
        /// Pointer to Codec Parameters structure
        void* p_codec_cfg;
        /// Pointer to LC3 Parameters structure
        app_bap_lc3_cfg_t* p_lc3_cfg;
        /// Pointer to Vendor Codec Parameters structure
        app_bap_vendor_codec_cfg_t* p_vendor_cfg;
    } u;
    /// QoS Requirements
    app_bap_qos_req_t qos_req;
    /// QoS Configuration structure
    app_bap_qos_cfg_t qos_cfg;
    /// Pointer to Metadata structure
    app_bap_metadata_cfg_t* p_metadata;
    /// ASE State
    uint8_t ase_state;
    /// ASE Instance local index
    uint8_t ase_instance_idx;
    /// ASE local index
    uint8_t ase_lid;
    /// Connection local index
    uint8_t con_lid;
    /// ASE Direction
    enum app_gaf_direction direction;
    /// CIS Connection Handle
    uint16_t cis_hdl;
}app_bap_ascs_ase_t;

/// Audio Stream Control Service Server environment structure
typedef struct app_bap_ascs_env
{
    ///prefferred Mtu
    uint8_t preferred_mtu;
    /// Number of instances of the ASE characteristic
    /// Shall be in the range [0, 15]
    uint8_t nb_ase_chars;
    /// Number of ASE configurations that can be maintained
    /// Shall be at least equal to nb_ase_chars
    /// Should be a multiple of nb_ase_chars
    /// Shall not be larger than nb_ase_chars * BLE_AUDIO_CONNECTION_CNT
    uint8_t nb_ases_cfg;
    /// ASCS ASE Information
    app_bap_ascs_ase_t *ase_info;
}app_bap_ascs_env_t;

uint32_t app_bap_uc_srv_cmp_evt_handler (void const *param);
uint32_t app_bap_uc_srv_rsp_handler (void const *param);
uint32_t app_bap_uc_srv_ind_handler (void const *param);
uint32_t app_bap_uc_srv_req_ind_handler (void const *param);

void app_bap_uc_srv_init(void);
void app_bap_uc_srv_info_init(void);

#endif
#endif // APP_BAPS_MSG_H_

/// @} APP_BAP
