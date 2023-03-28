/**
 ****************************************************************************************
 *
 * @file PRF_hl_api.h
 *
 * @brief Header file - PRF Internal HL API
 *
 * Copyright (C) RivieraWaves 2009-2020
 ****************************************************************************************
 */

#ifndef PRF_HL_API_H_
#define PRF_HL_API_H_

/**
 ****************************************************************************************
 * @addtogroup PRF
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_PROFILES)
#include "prf.h"
#include "ke_msg.h"

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
 * @brief Perform Profile initialization
 *
 * @param[in] init_type  Type of initialization (@see enum rwip_init_type)
 ****************************************************************************************
 */
void prf_init(uint8_t init_type);

/**
 ****************************************************************************************
 * @brief Link creation event, update profiles states.
 *
 * @param[in] conidx        connection index
 * @param[in] p_con_param    Pointer to connection parameter information
 *
 ****************************************************************************************
 */
void prf_con_create(uint8_t conidx, const gap_con_param_t* p_con_param);

/**
 ****************************************************************************************
 * @brief Link connection parameter updated
 *
 * @param[in] conidx        connection index
 * @param[in] p_con_param    Pointer to updated connection parameter information
 *
 ****************************************************************************************
 */
void prf_con_param_upd(uint8_t conidx, const gap_con_param_t* p_con_param);

/**
 ****************************************************************************************
 * @brief Link disconnection event, clean-up profiles.
 *
 * @param[in] conidx        connection index
 * @param[in] reason        detach reason
 *
 ****************************************************************************************
 */
void prf_con_cleanup(uint8_t conidx, uint8_t reason);

#if (BLE_HL_MSG_API)

/**
 ****************************************************************************************
 * @brief Initialize application message interface
 *
 * @param[in] api_id   Application identifier (@see enum TASK_API_ID)
 * @param[in] app_task Application task number
 *
 * @return Profile task number
 ****************************************************************************************
 */
ke_msg_id_t prf_msg_api_init(uint8_t api_id, ke_msg_id_t app_task);


#endif // (BLE_HL_MSG_API)

#endif // (BLE_PROFILES)

/// @} PRF

#endif // PRF_HL_API_H_
