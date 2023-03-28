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


#ifndef _AMS_COMMON_H_
#define _AMS_COMMON_H_

/**
 ****************************************************************************************
 * @addtogroup AMS Profile
 * @ingroup AMS
 * @brief AMS Profile
 *****************************************************************************************
 */

#include "rwip_config.h"

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#if (BLE_AMS_CLIENT || BLE_AMS_SERVER)

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

/// Error Codes
// The MR has not properly set up the AMS
#define AMS_INVALID_STATE             (0xA0)
// The command was improperly formatted
#define AMS_INVALID_CMD               (0xA1)
// The corresponding attribute is empty
#define AMS_ABSENT_ATTRIBUTE          (0xA2)

/*
 * STRUCTURES
 ****************************************************************************************
 */

typedef struct defer_cfm_info
{
    uint8_t conidx;
    uint8_t user_lid;
    uint16_t token;
    uint8_t status;
} defer_cfm_info_t;

/// Remote Command Characteristic Value Structure
struct ams_remote_cmd
{
    /// Remote command
    uint8_t remote_command_id;
};

/// Entity Update Characteristic Value Structure
struct ams_entity_update
{
    /// Remote command
    uint8_t entity_id;
    uint8_t attribute_id;
    uint8_t entity_update_flags;
    uint8_t value;
};

typedef struct ams_cli_rd_cmd
{
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t token;
    /// GATT user local identifier
    uint8_t user_lid;
    /// Connection index
    uint8_t conidx;
    /// Attribute handle
    uint16_t hdl;
} ams_cli_rd_cmd_t;

typedef struct ams_srv_cfm
{

} ams_srv_cfm_t;

#endif //(BLE_AMS_CLIENT || BLE_AMS_SERVER)

/// @} ams_common

#endif //(_AMS_COMMON_H_)
