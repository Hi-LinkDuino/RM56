/**
 ****************************************************************************************
 *
 * @file gatt_msg_int.h
 *
 * @brief Header file - GATT internal message API
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_MSG_INT_H_
#define GATT_MSG_INT_H_

/**
 ****************************************************************************************
 * @addtogroup GATT
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_GATT)
#if (BLE_HL_MSG_API)
#include "gatt.h"
#include "ke_msg.h"
#include "ke_mem.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */



/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Send default message for command complete execution
 *
 * @param[in]  cmd_code Command code (@see enum gatt_cmd_code)
 * @param[in]  dummy    Dummy parameter whose meaning is upper layer dependent
 * @param[in]  src_id   ID of the sending task instance.
 * @param[in]  user_lid GATT user Local Identifier
 * @param[in]  status   Status of command execution
 ****************************************************************************************
 */
void gatt_msg_send_basic_cmp_evt(uint16_t cmd_code, uint16_t dummy, uint16_t src_id, uint8_t user_lid, uint16_t status);

/**
 ****************************************************************************************
 * @brief Send message for procedure command complete execution
 *
 * @param[in]  cmd_code Command code (@see enum gatt_cmd_code)
 * @param[in]  dummy    Dummy parameter whose meaning is upper layer dependent
 * @param[in]  conidx   Connection index
 * @param[in]  src_id   ID of the sending task instance.
 * @param[in]  user_lid GATT user Local Identifier
 * @param[in]  status   Status of command execution
 ****************************************************************************************
 */
void gatt_msg_send_proc_cmp_evt(uint16_t cmd_code, uint16_t dummy, uint8_t conidx, uint16_t src_id, uint8_t user_lid, uint16_t status);
#endif // (BLE_HL_MSG_API)
#endif // (BLE_GATT)
/// @} GATT

#endif // GATT_MSG_INT_H_
