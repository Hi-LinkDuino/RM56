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

#ifndef APP_VCC_H_
#define APP_VCC_H_
#if BLE_AUDIO_ENABLED
#include "prf_types.h"

#define APP_ARC_VCS_DFT_STEP_SIZE 10
#define APP_ARC_VOCS_DESC_MAX_LEN 50
#define APP_ARC_VOCS_DFT_NB_OUTPUTS 1
#define APP_ARC_AICS_DFT_NB_INPUTS 2
#define APP_ARC_AICS_DESC_MAX_LEN 50
#define APP_ARC_AICS_DFT_GAIN_UNITS 10
#define APP_ARC_AICS_DFT_GAIN_MAX 127
#define APP_ARC_AICS_DFT_GAIN_MIN (-128)
#define APP_ARC_DESC_STR "mdb"


typedef struct arc_info
{
    uint8_t vocs_output_num;
    prf_svc_t vocs_svc_hdl[APP_ARC_VOCS_DFT_NB_OUTPUTS];
    uint8_t aics_input_num;
    prf_svc_t aics_svc_hdl[APP_ARC_AICS_DFT_NB_INPUTS];
}arc_info_t;

typedef struct app_arc_info
{
    arc_info_t arc_info[BLE_AUDIO_CONNECTION_CNT];
}app_arc_info_t;

uint32_t app_arc_get_acl_idx_by_acl_hdl(uint16_t acl_hdl);
uint32_t app_arc_get_acl_idx_by_con_lid(uint8_t con_lid);
uint32_t app_arc_cmp_evt_handler(void const *param);
uint32_t app_arc_rsp_handler(void const *param);
uint32_t app_arc_ind_handler(void const *param);
uint32_t app_arc_req_ind_handler(void const *param);

/* ble audio gaf arc(Audio Rendering Control) init */
void app_arc_client_init(void);
void app_arc_server_init(void);

void app_arc_start(uint8_t con_lid);

#endif
#endif // APP_VCC_H_

/// @} APP_VCC
