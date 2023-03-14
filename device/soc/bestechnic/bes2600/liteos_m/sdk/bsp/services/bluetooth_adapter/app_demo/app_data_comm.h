/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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

#ifndef __APP_DATA_COMM_H__
#define __APP_DATA_COMM_H__
#include "plat_types.h"

typedef void(*app_data_channel_state_report_cb_t)(uint8_t conidx, uint8_t enable);
typedef void(*app_data_tx_done_report_cb_t)(uint8_t conidx, uint8_t* data, uint32_t data_len);
typedef void(*app_data_rx_done_report_cb_t)(uint8_t conidx, uint8_t* data, uint32_t data_len);
typedef void(*app_data_mtu_changed_report_cb_t)(uint8_t conidx, uint16_t mtu);
typedef void(*app_data_probe_changed_report_cb_t)(uint8_t conidx, uint16_t probe_value);

typedef struct {
    app_data_channel_state_report_cb_t      channel_state;
    app_data_tx_done_report_cb_t            data_tx_done;
    app_data_rx_done_report_cb_t            data_rx_done;
    app_data_mtu_changed_report_cb_t        mtu_changed;
    app_data_probe_changed_report_cb_t      probe_changed;
} APP_DATA_COMM_CB_FUNC_T;

APP_DATA_COMM_CB_FUNC_T* app_data_comm_interfaces(void);
void app_data_comm_init(APP_DATA_COMM_CB_FUNC_T* data_cb);
void app_data_comm_send(uint8_t *data, uint32_t data_len);
void app_data_comm_txmode_set(uint8_t tx_mode);
void app_data_comm_loopback_test(uint8_t loopback);
void app_data_comm_throughput_test(uint8_t is_enable, uint16_t interval, uint16_t mtu);

#endif //end of __APP_DATA_COMM_H__