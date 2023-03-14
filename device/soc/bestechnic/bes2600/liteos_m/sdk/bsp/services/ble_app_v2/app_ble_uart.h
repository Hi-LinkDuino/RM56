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
#ifndef __APP_BLE_UART_H__
#define __APP_BLE_UART_H__

typedef void (*ble_uart_test_function_handle)(void);
typedef struct
{
    const char* string;
    ble_uart_test_function_handle function;
} ble_uart_handle_t;

#ifdef __cplusplus
extern "C"{
#endif

void ble_start_three_adv(void);
void ble_stop_all_adv(void);
void ble_start_adv_1(void);
void ble_stop_adv_1(void);
void ble_start_adv_2(void);
void ble_start_adv_3(void);
void ble_stop_adv_2(void);
void ble_stop_adv_3(void);
void ble_update_adv_data_1_to_2(void);
void ble_update_adv_data_1_back_to_1(void);
void ble_open_scan(void);
void ble_close_scan(void);
void ble_start_connect(void);
void ble_stop_connect(void);
void ble_disconnect_all(void);

void ble_uart_cmd_init(void);

#ifdef __cplusplus
}
#endif

#endif

