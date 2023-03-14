/***************************************************************************
 *
 * Copyright 2015-2021 BES.
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
#ifndef __APP_BLE_EVT_MEDIATOR_H__
#define __APP_BLE_EVT_MEDIATOR_H__
/*****************************header include********************************/
#include "app_ble_evt_type.h"

/******************************macro defination*****************************/

/******************************type defination******************************/
typedef enum {
    APP_BLE_EVT_USER_AOB        = 0, // used for aob stm
    APP_BLE_EVT_USER_BIS        = 1, // used for bis instance

    APP_BLE_EVT_USER_NUM,
} APP_BLE_EVT_USER_E;

typedef struct {
    uint16_t evt_id;
    void *p;
} app_ble_evt_t;

typedef void (*app_ble_evt_cb_t)(app_ble_evt_t *event);

/****************************function declearation**************************/
#ifdef __cplusplus
extern "C" {
#endif

void *app_ble_evt_heap_malloc(uint32_t size);
void *app_ble_evt_heap_cmalloc(uint32_t size);
void *app_ble_evt_heap_realloc(void *rmem, uint32_t newsize);
void app_ble_evt_heap_free(void *rmem);
int app_ble_evt_get_maibox_cnt(void);
void app_ble_evt_thread_init();
int app_ble_evt_put_mailbox(app_ble_evt_t *evt);

/*---------------------------------------------------------------------------
 *            app_ble_evt_cb_register
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    to register custom callback function
 *
 * Parameters:
 *    cb -- custom callback function
 *
 * Return:
 *    void
 */
void app_ble_evt_cb_register(APP_BLE_EVT_USER_E user, app_ble_evt_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif

