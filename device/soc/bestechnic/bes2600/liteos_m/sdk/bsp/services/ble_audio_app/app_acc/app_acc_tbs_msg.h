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

#ifndef APP_ACC_TBS_MSG_H_
#define APP_ACC_TBS_MSG_H_

#if BLE_AUDIO_ENABLED
//There is a GTBS by default, so it just means the number of TBS
#define APP_ACC_DFT_TBS_NUM (1)
//The number of calls that maintained at the same time
#define APP_ACC_CALL_POOL_SIZE (2)
//Such as: the length of phone number, tel:+13888888888
#define APP_ACC_MAX_URI_LEN (32)

#define APP_GAF_ACC_GTBS_CCID (0)
#define APP_GAF_ACC_TBS_CCID (1)
#define APP_GAF_ACC_GMCS_CCID (2)
#define APP_GAF_ACC_MCS_CCID (3)
#define APP_GAF_ACC_UCI_VAL1 "eyebm"
#define APP_GAF_ACC_UCI_VAL2 "qq"

#define APP_ACC_DFT_TECHNO_VAL ACC_TB_TECHNO_4G
#define APP_ACC_DFT_SIGN_STRENGTH_VAL (100)
#define APP_ACC_DFT_PROV_NAME "CHINA MOBILE"
#define APP_ACC_DFT_URI_SCHEMES_LIST "tel,sip,skype"
#define APP_ACC_DFT_CALL_INCOMING_URI "tel:+13888888888"
#define APP_ACC_DFT_CALL_INCOMING_TGT_URI "tel:+13666666666"
#define APP_ACC_DFT_CALL_INCOMING_FRIENDLY_NAME "INCOMING CALL"
#define APP_ACC_DFT_CALL_OUTGOING_URI "tel:+13222222222"
#define APP_ACC_DFT_CALL_OUTGOING_FRIENDLY_NAME "OUTGOING CALL"

uint32_t app_acc_tbs_rsp_handler(void const *param);
uint32_t app_acc_tbs_ind_handler(void const *param);
uint32_t app_acc_tbs_req_ind_handler(void const *param);
void app_acc_tbs_init(void);
#endif

#endif // APP_ACC_TBS_MSG_H_

/// @} APP_ACC_TBS_MSG_H_
