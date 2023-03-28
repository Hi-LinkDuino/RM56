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

#ifndef APP_ATC_CSISM_MSG_H_
#define APP_ATC_CSISM_MSG_H_
#if BLE_AUDIO_ENABLED

#define APP_ATC_DFT_SETS_SIZE CSIS_SIZE_MIN
#define APP_ATC_DFT_SETS_NUM (1)
#define APP_ATC_DFT_CSIS_TIMEOUT (60)

typedef struct csism_info
{
    uint8_t set_lid;
}csism_info_t;

typedef struct app_atc_csism_info
{
    csism_info_t csism_info[APP_ATC_DFT_SETS_NUM];
}app_atc_csism_info_t;

uint32_t app_atc_csism_cmp_evt_handler(void const *param);
uint32_t app_atc_csism_rsp_handler(void const *param);
uint32_t app_atc_csism_ind_handler(void const *param);
uint32_t app_atc_csism_req_ind_handler(void const *param);

void app_atc_csism_init(void);

#endif
#endif // APP_ATC_CSISM_MSG_H_

/// @} APP_ATC_CSISM_MSG_H_
