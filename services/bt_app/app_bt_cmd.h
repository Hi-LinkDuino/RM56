/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __APP_BT_CMD_H__
#define __APP_BT_CMD_H__
#include "app_trace_rx.h"
#include "bluetooth.h"

bt_bdaddr_t *app_bt_get_pts_address(void);

typedef void (*app_bt_cmd_function_handle)(void);

typedef void (*app_bt_cmd_function_handle_with_parm)(const char* bt_cmd, uint32_t cmd_len);

typedef void (*app_bt_cmd_test_ind_handler_func)(uint8_t test_type);

typedef struct
{
    const char* string;
    app_bt_cmd_function_handle function;
} app_bt_cmd_handle_t;

typedef struct
{
    const char* string;
    app_bt_cmd_function_handle_with_parm function;
} app_bt_cmd_handle_with_parm_t;

typedef struct
{
    const app_bt_cmd_handle_t *start_cmd;
    uint16_t cmd_count;
    bool inuse;
} app_bt_cmd_handle_table_t;

typedef struct
{
    const app_bt_cmd_handle_with_parm_t *start_cmd;
    uint16_t cmd_count;
    bool inuse;
} app_bt_cmd_handle_with_parm_table_t;

void app_bt_register_test_ind_callback(app_bt_cmd_test_ind_handler_func cb);

void app_bt_add_string_test_table(void);
void app_bt_cmd_line_handler(char *cmd, unsigned int cmd_length);
#if defined(APP_TRACE_RX_ENABLE) || defined(IBRT)

/*************************
*   automate_cmd format
*   item        len(byte)
*   group_code  1
*   opera_code  1
*   test_times  1
*   param_len   1
*   *param      param_len(0-255)
*************************/
#define AUTOMATE_TEST_CMD_CRC_RECORD_LEN    (2)
#define AUTOMATE_TEST_CMD_PARAM_MAX_LEN     (255)

typedef struct {
    uint8_t group_code;
    uint8_t opera_code;
    uint8_t test_times;
    uint8_t param_len;
    uint8_t param[0];
} AUTO_TEST_CMD_T;

typedef struct {
    uint8_t message_id;
    uint8_t extra_data[10];
    uint32_t message_ptr;
    uint32_t message_Param0;
    uint32_t message_Param1;
    uint32_t message_Param2;
} APP_BT_CMD_MSG_BODY;

typedef struct {
    APP_BT_CMD_MSG_BODY msg_body;
} APP_BT_CMD_MSG_BLOCK;

int app_bt_cmd_mailbox_put(APP_BT_CMD_MSG_BLOCK* msg_src);
int app_bt_cmd_mailbox_free(APP_BT_CMD_MSG_BLOCK* msg_p);
int app_bt_cmd_mailbox_get(APP_BT_CMD_MSG_BLOCK** msg_p);
void app_bt_cmd_thread_init(void);
void app_bt_cmd_init(void);

void app_bt_cmd_add_test_table(const app_bt_cmd_handle_t *start_cmd, uint16_t cmd_count);
void app_bt_cmd_add_test_table_with_param(const app_bt_cmd_handle_with_parm_t *start_cmd, uint16_t cmd_count);

void app_bt_test_linein_start(void);
void app_bt_test_linein_stop(void);

#ifdef IBRT
void app_ibrt_peripheral_run0(uint32_t ptr);
void app_ibrt_peripheral_run1(uint32_t ptr);
void app_ibrt_peripheral_run2(uint32_t ptr);
void app_ibrt_peripheral_auto_test_stop(void);
#endif

#endif // APP_TRACE_RX_ENABLE || IBRT

#endif // __APP_BT_CMD_H__

