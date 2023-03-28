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
#ifndef __APP_FACTORY_BT_H__
#define __APP_FACTORY__BTH__

#include "app_key.h"

enum TEST_MODE_TIMER_TYEP_T{
   BT_NONSIGNALING_TX_TYPE = 0,
   BT_NONSIGNALING_RX_TYPE,
   LE_TEST_TX_TYPE,
   LE_TEST_RX_TYPE,
   TEST_ERROR_TYPE,
};

void app_factorymode_bt_create_connect(void);

void app_factorymode_bt_init_connect(void);

int app_factorymode_bt_xtalcalib_proc(void);

void app_factorymode_bt_xtalrangetest(APP_KEY_STATUS *status, void *param);

void app_factorymode_bt_signalingtest(APP_KEY_STATUS *status, void *param);

void app_factorymode_bt_nosignalingtest(APP_KEY_STATUS *status, void *param);

void app_factorymode_bt_xtalcalib(APP_KEY_STATUS *status, void *param);

void app_factory_enter_le_tx_test(const char* cmd, uint32_t cmd_len);

void app_factory_enter_le_rx_test(const char* cmd, uint32_t cmd_len);

void app_factory_enter_le_continueous_tx_test(const char* cmd, uint32_t cmd_len);

void app_factory_enter_le_continueous_rx_test(const char* cmd, uint32_t cmd_len);

void app_factory_remote_fetch_le_teset_result(void);

void app_factory_terminate_le_test(void);

void app_factory_enter_bt_38chanl_dh5_prbs9_tx_test(void);

void app_factory_enter_bt_38chanl_dh1_prbs9_rx_test(void);

void app_factory_remote_fetch_bt_nonsig_test_result(void);
void app_factory_enter_le_tx_test_v2(int timeout, int tx_channel, int data_len, int pkt_payload, int phy);
void app_factory_enter_le_rx_test_v2(int timeout, int remote_tx_nb, int rx_channel, int phy, int mod_idx);

void app_factorymode_btc_only_mode(APP_KEY_STATUS *status, void *param);

#endif
