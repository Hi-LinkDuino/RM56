/**
 ****************************************************************************************
 *
 * @file app_bap_bc_deleg_msg.h
 *
 * @brief BLE Audio Broadcast Delegator
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

#ifndef APP_BAP_DELEG_MSG_H_
#define APP_BAP_DELEG_MSG_H_

#if BLE_AUDIO_ENABLED

#ifdef __cplusplus
extern "C"{
#endif
#include "app_bap.h"

#define APP_BAP_DFT_BC_DELEG_NB_SRCS                (2)
#define APP_BAP_DFT_BC_DELEG_ADV_INTV_MIN_SLOT      256 //100ms  *0.625ms
#define APP_BAP_DFT_BC_DELEG_ADV_INTV_MAX_SLOT      256 //100ms  *0.625ms
#define APP_BAP_DFT_BC_DELEG_CHNL_MAP               ADV_ALL_CHNLS_EN
#define APP_BAP_DFT_BC_DELEG_PHY_PRIM               GAF_BAP_PHY_1MBPS
#define APP_BAP_DFT_BC_DELEG_PHY_SECOND             GAF_BAP_PHY_1MBPS
#define APP_BAP_DFT_BC_DELEG_ADV_SID                1
#define APP_BAP_DFT_BC_DELEG_SKIP                   1
#define APP_BAP_DFT_BC_DELEG_REPORT_FILTER_BF       0x1F
#define APP_BAP_DFT_BC_DELEG_SYNC_TO                1000 //Uint 10ms
#define APP_BAP_DFT_BC_DELEG_TIMEOUT_S              0 //Always scan periodic adv
#define APP_BAP_BC_DELEG_MAX_ADV_DATA_LEN           200
#define APP_BAP_DFT_BC_DELEG_SOLICITE_ADV_DATA      "bc_delegator_solicite_adv_data_test"

/// Structure for delegator periodic advertising sync
typedef struct app_bap_bc_deleg_pa_sync
{
    /// Bit field of filtered report types - The reception of BIGInfo is forced to enable
    /// (@see enum bap_bc_scan_report_filter_bf)
    uint8_t  report_filter_bf;
    /// Number of periodic advertising that can be skipped after a successful receive.
    /// Maximum authorized value is 499
    uint16_t skip;
    /// Synchronization timeout for the periodic advertising
    /// (in unit of 10ms between 100ms and 163.84s)
    uint16_t sync_to;
    /// Sync establishment timeout in seconds
    uint16_t timeout_s;
} app_bap_bc_deleg_pa_sync_t;

/// Structure for big sync
typedef struct app_bap_bc_deleg_sink_enable
{
    /// Source local index
    uint8_t          src_lid;
    /// True if Broadcast code is meaningful, False otherwise
    /// if False it means that broadcast code has already been received by delegator or Broadcast group isn't encrypted
    bool             bcast_code_present;
    /// Broadcast code
    app_gaf_bcast_code_t bcast_code;
    /// Stream position bit field indicating streams to synchronize with.
    uint32_t         stream_pos_bf;
    /// Timeout duration (10ms unit) before considering synchronization lost - (Range 100 ms to 163.84 s).
    uint16_t         timeout_10ms;
    /// Maximum number of subevents the controller should use to received data payloads in each interval
    uint8_t          mse;
} app_bap_bc_deleg_sink_enable_t;

/// Content of Broadcast Delegator environment
typedef struct app_bap_bc_deleg_env
{
    /// Number of supported Broadcast Sources Up to 15
    uint8_t  nb_srcs;
    /// Preferred Mtu
    uint8_t preferred_mtu;
    /// Advertising parameters
    app_bap_bc_adv_param_t adv_param;
    /// PA sync parameters
    app_bap_bc_deleg_pa_sync_t sync_param;
    /// Advertising data length
    uint8_t adv_data_len;
    /// Advertising data
    uint8_t *solicite_adv_data;
}app_bap_bc_deleg_env_t;


uint32_t app_bap_bc_deleg_cmp_evt_handler(void const *param);
uint32_t app_bap_bc_deleg_rsp_handler(void const *param);
uint32_t app_bap_bc_deleg_ind_handler(void const *param);
uint32_t app_bap_bc_deleg_req_ind_handler(void const *param);

void app_bap_bc_deleg_init(void);
void app_bap_bc_deleg_info_init(void);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_DELEG_MSG_H_

/// @} APP_BAP_DELEG_MSG_H_
