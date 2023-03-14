/**
 ****************************************************************************************
 *
 * @file app_dis.h
 *
 * @brief Device Information Application Module Entry point
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

#ifndef APP_DIS_H_
#define APP_DIS_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief Device Information Application Module Entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW Configuration

#if (BLE_APP_DIS)

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */


#if 0 // (BLE_APP_HID)
#define APP_DIS_FEATURES             (DIS_MANUFACTURER_NAME_CHAR_SUP_BIT |\
                                      DIS_MODEL_NB_STR_CHAR_SUP_BIT      |\
                                      DIS_SYSTEM_ID_CHAR_SUP_BIT         |\
                                      DIS_PNP_ID_CHAR_SUP_BIT)

#elif (BLE_APP_AM0)
#define APP_DIS_FEATURES             (DIS_MANUFACTURER_NAME_CHAR_SUP |\
                                      DIS_MODEL_NB_STR_CHAR_SUP      |\
                                      DIS_FIRM_REV_STR_CHAR_SUP         |\
                                      DIS_SW_REV_STR_CHAR_SUP)
#else
#define APP_DIS_FEATURES             (DIS_ALL_FEAT_SUP)
#endif //(BLE_APP_HID)


/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

/// Table of message handlers
extern const struct app_subtask_handlers app_dis_handlers;

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize Device Information Service Application
 ****************************************************************************************
 */
void app_dis_init(void);

/**
 ****************************************************************************************
 * @brief Add a Device Information Service instance in the DB
 ****************************************************************************************
 */
void app_dis_add_dis(void);

/**
 ****************************************************************************************
 * @brief Enable the Device Information Service
 ****************************************************************************************
 */


#endif //BLE_APP_DIS

/// @} APP

#endif //APP_DIS_H_
