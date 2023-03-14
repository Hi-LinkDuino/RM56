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


#ifndef APP_ANCC_H_
#define APP_ANCC_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 *
 * @brief ANCC Application entry point.
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if BLE_ANC_CLIENT
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "string.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 *
 * ANCC Application Functions
 *
 ****************************************************************************************
 */

void app_ancc_add_ancc(void);

/**
 ****************************************************************************************
 * @brief Inialize application and enable ANCC profile.
 *
 ****************************************************************************************
 */
void app_ancc_enable(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Read ANC attribute from the server with the given connection index and the 
 * attribute handle.
 *
 ****************************************************************************************
 */
void app_ancc_read(uint8_t conidx, uint16_t hdl);

/**
 ****************************************************************************************
 * @brief Write ANC attribute to the server with the given connection index and the 
 * attribute handle.
 *
 ****************************************************************************************
 */
void app_ancc_write(uint8_t conidx, uint16_t hdl, uint16_t value_length, uint8_t *value);

#endif //BLE_ANC_CLIENT

/// @} APP

#endif // APP_ANCC_H_
