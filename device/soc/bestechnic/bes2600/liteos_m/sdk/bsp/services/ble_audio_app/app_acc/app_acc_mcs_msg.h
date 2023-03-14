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

#ifndef APP_ACC_MCS_MSG_H_
#define APP_ACC_MCS_MSG_H_
#if BLE_AUDIO_ENABLED

#define APP_ACC_DFT_MCS_NUM (2)
#define APP_ACC_DFT_MCS_PLAYER_NAME "MCS_PALYER"
#define APP_ACC_DFT_TITLE_NAME "TEST_TITLE_NAME"
#define APP_ACC_DFT_ICON_URL "www.bluetooth.com"
#define MCS_MEDIA_PALYER_ICON_OBJ_LEN (32)
#define MCS_CURRENT_TRACK_SEG_OBJ_LEN (32)
#define MCS_CURRENT_TRACK_OBJ_LEN (32)
#define MCS_NEXT_TRACK_OBJ_LEN (32)
#define MCS_CURRENT_GROUP_OBJ_LEN (32)
#define MCS_PARENT_GROUP_OBJ_LEN (32)
#define MCS_SEARCH_RESULTS_OBJ_LEN (32)

enum acc_mcs_obj_idx
{
    MCS_MEDIA_PALYER_ICON_OBJ = 0,
    MCS_CURRENT_TRACK_SEG_OBJ,
    MCS_CURRENT_TRACK_OBJ,
    MCS_NEXT_TRACK_OBJ,
    MCS_CURRENT_GROUP_OBJ,
    MCS_PARENT_GROUP_OBJ,
    MCS_SEARCH_RESULTS_OBJ,
    MCS_OBJ_MAX,
};

uint32_t app_acc_mcs_rsp_handler(void const *param);
uint32_t app_acc_mcs_ind_handler(void const *param);
uint32_t app_acc_mcs_req_ind_handler(void const *param);

void app_acc_mcs_init(void);

#endif
#endif // APP_ACC_MCS_MSG_H_

/// @} APP_ACC_MCS_MSG_H_
