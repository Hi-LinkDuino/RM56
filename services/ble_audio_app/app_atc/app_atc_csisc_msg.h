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
 * @addtogroup APP_ACC
 
 * @{
 ****************************************************************************************
 */

#ifndef APP_ATC_CSISC_MSG_H_
#define APP_ATC_CSISC_MSG_H_
#if BLE_AUDIO_ENABLED
#include "app_atc_csism_msg.h"
#include "csis.h"

typedef struct app_atc_csisc_info
{
    csis_sirk_t sirk[APP_ATC_DFT_SETS_SIZE * APP_ATC_DFT_SETS_NUM];
}app_atc_csisc_info_t;

uint32_t app_atc_csisc_cmp_evt_handler(void const *param);
uint32_t app_atc_csisc_rsp_handler(void const *param);
uint32_t app_atc_csisc_ind_handler(void const *param);

void app_atc_csisc_init(void);
void app_atc_csisc_start(uint8_t con_lid);

#endif
#endif // APP_ATC_CSISC_MSG_H_

/// @} APP_ATC_CSISC_MSG_H_
