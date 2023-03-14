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

#ifndef APP_ACC_OTS_MSG_H_
#define APP_ACC_OTS_MSG_H_
#if BLE_AUDIO_ENABLED

#define APP_ACC_DFT_OTS_NUM (2)
#define APP_ACC_DFT_OTS_NAME "MC_OTS"
#define APP_ACC_DFT_OTS_NAME_LEN (32)
#define APP_ACC_DFT_LOCAL_MAX_SDU (128)
#define APP_ACC_COC_TEST_SDU_LEN (32)

typedef struct ots_info
{
    uint8_t filter_val[OTP_FILTER_NUM];
    uint8_t current_obj_name[APP_ACC_DFT_OTS_NAME_LEN];
}ots_info_t;

struct app_acc_ots_info
{
    uint8_t nb_object;
    ots_info_t ots_info[APP_ACC_DFT_OTS_NUM];
};

uint32_t app_acc_ots_cmp_evt_handler(void const *param);
uint32_t app_acc_ots_rsp_handler(void const *param);
uint32_t app_acc_ots_ind_handler(void const *param);
uint32_t app_acc_ots_req_ind_handler(void const *param);

void app_acc_ots_init(void);
void app_acc_ots_start(uint8_t con_lid);

#endif
#endif // APP_ACC_OTS_MSG_H_

/// @} APP_ACC_OTS_MSG_H_
