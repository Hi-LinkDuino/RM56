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

#ifndef APP_CAP_H_
#define APP_CAP_H_
#if BLE_AUDIO_ENABLED

uint32_t app_cap_cmp_evt_handler(void const *param);
uint32_t app_cap_rsp_handler(void const *param);
uint32_t app_cap_ind_handler(void const *param);
uint32_t app_cap_req_ind_handler(void const *param);

/* ble audio gaf cap(Common Audio Control) init */
void app_cap_client_init(void);
void app_cap_server_init(void);

void app_cap_start(uint8_t con_lid);

#endif
#endif // APP_CAP_H_

/// @} APP_CAP