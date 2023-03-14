/**
 ****************************************************************************************
 *
 * @file app_hid.h
 *
 * @brief HID Application Module entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef APP_HID_H_
#define APP_HID_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief HID Application Module entry point
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_HID)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"         // Kernel Task Definition

#if (PS2_SUPPORT)
#include "ps2.h"             // PS2 Mouse Driver
#endif //(PS2_SUPPORT)

//#define HID_VOL_TEST
/*
 * STRUCTURES DEFINITION
 ****************************************************************************************
 */

 enum
{
    HID_KEY_IDX_RELEASE=0,
    HID_KEY_IDX_PP,
    HID_KEY_IDX_STOP,
    HID_KEY_IDX_PREV,
    HID_KEY_IDX_NEXT,
    HID_KEY_IDX_VOLUP,
    HID_KEY_IDX_VOLDOWN,
    HID_KEY_IDX_SS,
    HID_KEY_IDX_POWER
};

/// HID Application Module Environment Structure
struct app_hid_env_tag
{
    /// Connection handle
    uint8_t conidx;
    /// Mouse timeout value
    uint16_t timeout;
    /// Internal state of the module
    uint8_t state;
    /// Timer enabled
    bool timer_enabled;
    /// Number of report that can be sent
    uint8_t nb_report;
    
    uint8_t * report_map;
    
    uint8_t report_nb;
    
    uint8_t *report_id;
};

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

/// Table of message handlers
extern const struct app_subtask_handlers app_hid_handlers;

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * Health Thermometer Application Functions
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize HID Application Module
 ****************************************************************************************
 */
void app_hid_init(void);

/**
 ****************************************************************************************
 * @brief Initialize the PS2 mouse driver so that it can be used by the application
 ****************************************************************************************
 */
void app_hid_start_mouse(void);

/**
 ****************************************************************************************
 * @brief Add a HID Service instance in the DB
 ****************************************************************************************
 */
void app_hid_add_hids(void);

/**
 ****************************************************************************************
 * @brief Enable the HID Over GATT Profile device role
 *
 * @param[in]:  conhdl - Connection handle for the connection
 ****************************************************************************************
 */
void app_hid_enable_prf(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send a mouse report to the peer device
 *
 * @param[in]:  report - Mouse report sent by the PS2 driver
 ****************************************************************************************
 */
//void app_hid_send_mouse_report(struct ps2_mouse_msg report);

#endif //(BLE_APP_HID)

/// @} APP

#endif // APP_HID_H_
