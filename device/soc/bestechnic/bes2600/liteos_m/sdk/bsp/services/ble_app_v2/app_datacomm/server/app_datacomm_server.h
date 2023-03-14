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
#ifndef APP_DATACOMM_SERVER_H_
#define APP_DATACOMM_SERVER_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Datacomm Server Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_DATACOMM_SERVER)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"

#define BLE_INVALID_CONNECTION_INDEX    0xFF

#define HIGH_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS        20
#define HIGH_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS        30
#define HIGH_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS  2000
#define HIGH_SPEED_BLE_CONNECTION_SLAVELATENCY              0



#define LOW_SPEED_BLE_CONNECTION_INTERVAL_MIN_IN_MS         400
#define LOW_SPEED_BLE_CONNECTION_INTERVAL_MAX_IN_MS         500
#define LOW_SPEED_BLE_CONNECTION_SUPERVISOR_TIMEOUT_IN_MS   2000
#define LOW_SPEED_BLE_CONNECTION_SLAVELATENCY               0

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


extern struct app_env_tag app_env;

/// health thermometer application environment structure
struct app_datacomm_server_env_tag
{
    uint8_t connectionIndex;
    uint8_t isNotificationEnabled;
    uint8_t isProbeNotificationEnable;
    uint8_t probeEnable;
};

typedef void(*app_datacomm_server_tx_done_t)(void);
typedef void(*app_datacomm_server_data_received_callback_func_t)(uint8_t *p_buff, uint16_t bufLength);
typedef void(*app_datacomm_server_disconnected_done_t)(uint8_t conidx);
typedef void(*app_datacomm_server_connected_done_t)(uint8_t conidx);
typedef void(*app_datacomm_server_mtuexchanged_done_t)(uint8_t conidx, uint16_t mtu);

typedef void(*app_datacomm_server_activity_stopped_t)(void);

typedef void(*app_datacomm_server_probe_state_changed_cb_t)(uint8_t conidx, uint8_t probe_enable);

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Health Thermomter Application environment
extern struct app_datacomm_server_env_tag app_datacomm_server_env;

/// Table of message handlers
extern const struct app_subtask_handlers app_datacomm_server_table_handler;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Initialize Datacomm Server Application
 ****************************************************************************************
 */
void app_datacomm_server_init(void);

/**
 ****************************************************************************************
 * @brief Add a Datacomm Server instance in the DB
 ****************************************************************************************
 */
void app_datacomm_server_register_tx_done(app_datacomm_server_tx_done_t callback);

void app_datacomm_server_register_rx_done(app_datacomm_server_data_received_callback_func_t callback);

void app_datacomm_server_register_disconnected_done(app_datacomm_server_disconnected_done_t callback);

void app_datacomm_server_register_connected_done(app_datacomm_server_connected_done_t callback);

void app_datacomm_server_register_mtu_exchanged_done(app_datacomm_server_mtuexchanged_done_t callback);

void app_datacomm_add_datacomm(void);

void app_datacomm_server_connected_evt_handler(uint8_t conidx);

void app_datacomm_server_disconnected_evt_handler(uint8_t conidx);

void app_datacomm_server_send_data_via_notification(uint8_t* ptrData, uint32_t length);

void app_datacomm_server_send_data_via_indication(uint8_t* ptrData, uint32_t length);

void app_datacomm_server_send_data_via_write_command(uint8_t* ptrData, uint32_t length);

void app_datacomm_server_send_data_via_write_request(uint8_t* ptrData, uint32_t length);

void app_datacomm_server_control_notification(uint8_t conidx,bool isEnable);

void app_datacomm_server_mtu_exchanged_handler(uint8_t conidx, uint16_t mtu);

void app_datacomm_server_register_probe_state_change(app_datacomm_server_probe_state_changed_cb_t callback);

void app_datacomm_server_probe_value_set(uint8_t enable);

uint8_t app_datacomm_server_probe_value_get(void);

#ifdef __cplusplus
}
#endif

#endif //(BLE_APP_DATACOMM_SERVER)

/// @} APP

#endif // APP_DATACOMM_SERVER_H_
