/**
 ****************************************************************************************
 *
 * @file app_bap_uc_cli_msg.h
 *
 * @brief BLE Audio Audio Stream Control Service Client
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

#ifndef APP_BAP_UC_CLI_MSG_H_
#define APP_BAP_UC_CLI_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_bap.h"
#include "co_list.h"

#define APP_BAP_DFT_ASCC_NB_ASE_CFG               3*BLE_AUDIO_CONNECTION_CNT
#define APP_BAP_DFT_ASCC_DP_ID                    0
#define APP_BAP_DFT_ASCC_PHY_BF                   GAF_BAP_PHY_2MBPS
#define APP_BAP_DFT_ASCC_CTL_DELAY_US             0x0102
#define APP_BAP_DFT_ASCC_PACKING_TYPE             GAF_BAP_PACK_TYPE_SEQ
#define APP_BAP_DFT_ASCC_FRAMING_TYPE             GAF_BAP_FRAME_TYPE_FRAMED
#define APP_BAP_DFT_ASCC_SCA                      4

/// QoS Configuration structure (short)
typedef struct app_bap_uc_cli_qos_cfg
{
    /// PHY
    uint8_t phy;
    /// Maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
    /// Maximum SDU size
    /// From 0 to 4095 bytes (0xFFF)
    uint16_t max_sdu_size;
    /// Presentation delay in microseconds
    uint32_t pres_delay_us;
} app_bap_uc_cli_qos_cfg_t;

/// CIG params structure
typedef struct app_bap_uc_cli_grp_param
{
    /// SDU interval from Master to Slave in microseconds
    /// From 0xFF (255us) to 0xFFFF (1.048575s)
    uint32_t sdu_intv_m2s_us;
    /// SDU interval from Slave to Master in microseconds
    /// From 0xFF (255us) to 0xFFFF (1.048575s)
    uint32_t sdu_intv_s2m_us;
    /// Maximum time (in milliseconds) for an SDU to be transported from master controller to slave
    /// controller. From 0x5 (5ms) to 0xFA0 (4s)
    uint16_t tlatency_m2s_ms;
    /// Maximum time (in milliseconds) for an SDU to be transported from slave controller to master
    /// controller. From 0x5 (5ms) to 0xFA0 (4s)
    uint16_t tlatency_s2m_ms;
    /// Sequential or interleaved scheduling
    uint8_t packing;
    /// Unframed or framed mode
    uint8_t framing;
    /// Worst slow clock accuracy of slaves
    uint8_t sca;
} app_bap_uc_cli_grp_param_t;

/// ASCC ASE Information structure
typedef struct app_bap_ascc_ase
{
    /// Data Path Configuration
    app_bap_dp_cfg_t dp_cfg;
    /// Codec ID
    app_gaf_codec_id_t codec_id;
    /// Codec Configuration Length
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
    /// QoS configuration
    app_bap_uc_cli_qos_cfg_t qos_cfg;
    /// ASE State
    uint8_t ase_state;
    /// Connection local index
    uint8_t con_lid;
    /// ASE local index
    uint8_t ase_lid;
    /// ASE instance index
    uint8_t ase_instance_idx;
    /// ASE Direction
    enum app_gaf_direction direction;
    /// CIS index
    uint8_t cis_id;
    /// CIS Connection Handle
    uint16_t cis_hdl;
} app_bap_ascc_ase_t;

/// ASCC CIG Information structure
typedef struct app_bap_ascc_grp
{
    /// CIG local index
    uint8_t cig_grp_lid;
    /// Group parameters
    app_bap_uc_cli_grp_param_t grp_params;
} app_bap_ascc_grp_t;

/// Audio Stream Control Service Client environment structure
typedef struct app_bap_ascc_env
{
    /// Preferred Mtu
    uint8_t preferred_mtu;
    /// Number of ASE configurations that can be maintained
    uint8_t nb_ases_cfg;
    /// PHY type
    uint8_t phy;
    /// Group Information
    app_bap_ascc_grp_t grp_info;
    /// ASE Information
    app_bap_ascc_ase_t *ase_info;
}app_bap_ascc_env_t;


#ifdef __cplusplus
extern "C" {
#endif

uint8_t app_bap_uc_cli_iso_send_data_to_all_channel(uint8_t **payload, uint16_t payload_len, uint32_t ref_time);
uint32_t app_bap_uc_cli_cmp_evt_handler (void const *param);
uint32_t app_bap_uc_cli_rsp_handler (void const *param);
uint32_t app_bap_uc_cli_ind_handler (void const *param);

void app_bap_uc_cli_init(void);
void app_bap_uc_cli_info_init(void);
void app_bap_uc_cli_start(uint8_t con_idx);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_UC_CLI_MSG_H_

/// @} APP_BAP
