/**
 ****************************************************************************************
 *
 * @file gatt_user.h
 *
 * @brief Header file - GATT User API for GATT client and server
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_USER_H_
#define GATT_USER_H_

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
#include "gatt.h"

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

/// Union of Client or Server callback
typedef union gatt_user_cb
{
    /// Server callback
    gatt_srv_cb_t           srv;
    /// Client callback
    gatt_cli_cb_t           cli;
} gatt_user_cb_t;

/// GATT User information
typedef struct gatt_user
{
    /// Set of Callback functions
    const gatt_user_cb_t*   p_cb;
    #if (BLE_HL_MSG_API)
    /// Destination task number if user registered through message API
    uint16_t                dest_task_nbr;
    #endif // (BLE_HL_MSG_API)
    /// Preferred MTU for attribute transaction
    uint16_t                pref_mtu;
    /// Priority level to access ATT bearer
    uint8_t                 prio;
    /// User role (@see enum gatt_user_role)
    uint8_t                 role;
} gatt_user_t;


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
 * @brief Retrieve User information
 *
 * Minimum preferred length is equals to minimum L2CAP COC length
 *
 * @param[in] user_lid      User local identifier
 *
 * @return Pointer to user information, NULL if not found.
 ****************************************************************************************
 */
gatt_user_t* gatt_user_get(uint8_t user_lid);


/**
 ****************************************************************************************
 * @brief Retrieve preferred MTU to use according to registered users
 *
 * @return Maximum preferred MTU
 ****************************************************************************************
 */
uint16_t gatt_user_pref_mtu_get(void);
/**
 ****************************************************************************************
 * @brief configure preferred MTU to use according to registered users
 *
 * @return void
 ****************************************************************************************
 */
void gatt_user_pref_mtu_set(uint8_t  user_lid,uint16_t pref_mtu);


/**
 ****************************************************************************************
 * @brief  Retrieve number of client users
 *
 * @return Number of client users
 ****************************************************************************************
 */
uint8_t gatt_user_cli_nb_get(void);

#endif // (BLE_GATT)
/// @} GATT

#endif // GATT_USER_H_
