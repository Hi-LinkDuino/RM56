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
#ifndef __APP_IBRT_UI_TEST_H__
#define __APP_IBRT_UI_TEST_H__
#include <stdint.h>

#if defined(IBRT) && defined(IBRT_CORE_V2_ENABLE)

typedef void (*app_uart_test_function_handle)(void);
typedef void (*app_uart_test_function_handle_with_param)(unsigned char *, unsigned int);

typedef struct
{
    const char* string;
    app_uart_test_function_handle function;
} app_uart_handle_t;

typedef struct
{
    const char* string;
    app_uart_test_function_handle_with_param function;
} app_uart_handle_t_p;


#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_BT_ADAPTER)
int app_ibrt_raw_ui_adapter_config_load(void *config);
#endif

int app_ibrt_raw_ui_test_config_load(void *config);
void app_tws_ibrt_raw_ui_test_key_init();
void app_ibrt_ui_v2_add_test_cmd_table(void);
int app_ibrt_raw_ui_test_cmd_handler_with_param(unsigned char *buf, unsigned char *param, unsigned int length);
void app_ibrt_ui_test_voice_assistant_key(APP_KEY_STATUS *status, void *param);
int parse_bt_addr(unsigned char *addr_buf, bt_bdaddr_t *bt_addr, unsigned int lenth);
void app_ibrt_ui_automate_test_cmd_handler(uint8_t group_code, uint8_t operation_code, uint8_t *param, uint8_t param_len);
void app_ibrt_auto_test_inform_cmd_received(uint8_t group_code, uint8_t operation_code);
void app_ibrt_raw_ui_media_switch_test(void);

//void app_ibrt_search_ui_gpio_key_handle(APP_KEY_STATUS *status, void *param);

#ifdef __cplusplus
}
#endif

#endif

#endif /*__APP_IBRT_RAW_UI_TEST_H__ */
