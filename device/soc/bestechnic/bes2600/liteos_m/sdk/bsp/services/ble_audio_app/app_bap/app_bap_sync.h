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

#ifndef APP_BAP_SYNC_H_
#define APP_BAP_SYNC_H_
#if BLE_AUDIO_ENABLED


#ifdef __cplusplus
extern "C" {
#endif

void app_bap_sync_init(void);
uint32_t app_bap_sync_get_clk(void);
uint16_t app_bap_sync_get_finecnt(void);
uint32_t app_bap_sync_get_curr_time(void);   //uinit -- us
void app_bap_sync_trigger_init(void);
bool app_ble_sync_enable(uint32_t op_code);
void app_bap_sync_slave_set_tg_ticks(uint8_t *p_buff, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_SYNC_H_

/// @} APP_BAP
