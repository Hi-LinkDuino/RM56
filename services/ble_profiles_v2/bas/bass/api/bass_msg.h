/**
 ****************************************************************************************
 *
 * @file bass_msg.h
 *
 * @brief Header file - Battery Service Server Role - Message API.
 *
 * Copyright (C) RivieraWaves 2009-2019
 *
 ****************************************************************************************
 */


#ifndef _BASS_MSG_H_
#define _BASS_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup BASS
 * @ingroup Profile
 * @brief  Battery Service Server Role - Message API.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "prf_types.h"
#include "rwip_task.h" // Task definitions

/*
 * DEFINES
 ****************************************************************************************
 */

#define BAS_BATTERY_LVL_MAX               (100)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// Messages for Battery Server
/*@TRACE*/
enum bass_msg_id
{
    /// Start the Battery Server - at connection used to restore bond data
    BASS_ENABLE_REQ             = MSG_ID(BASS, 0x00),
    /// Confirmation of the Battery Server start
    BASS_ENABLE_RSP             = MSG_ID(BASS, 0x01),
    /// Battery Level Value Update Request
    BASS_BATT_LEVEL_UPD_REQ     = MSG_ID(BASS, 0x02),
    /// Inform APP if Battery Level value has been notified or not
    BASS_BATT_LEVEL_UPD_RSP     = MSG_ID(BASS, 0x03),
    /// Inform APP that Battery Level Notification Configuration has been changed - use to update bond data
    BASS_BATT_LEVEL_NTF_CFG_IND = MSG_ID(BASS, 0x04),
};

/// Features Flag Masks
enum bass_features
{
    /// Battery Level Characteristic doesn't support notifications
    BAS_BATT_LVL_NTF_NOT_SUP,
    /// Battery Level Characteristic support notifications
    BAS_BATT_LVL_NTF_SUP,
};

/*
 * APIs Structures
 ****************************************************************************************
 */

/// Parameters for the database creation
struct bass_db_cfg
{
    /// Number of BAS to add
    uint8_t             bas_nb;
    /// Features of each BAS instance
    uint8_t             features[BASS_NB_BAS_INSTANCES_MAX];
    /// Battery Level Characteristic Presentation Format - Should not change during connection
    prf_char_pres_fmt_t batt_level_pres_format[BASS_NB_BAS_INSTANCES_MAX];
};

/// Parameters of the @ref BASS_ENABLE_REQ message
struct bass_enable_req
{
    /// connection index
    uint8_t conidx;
    /// Notification Configuration
    uint8_t ntf_cfg;
    /// Old Battery Level used to decide if notification should be triggered
    uint8_t old_batt_lvl[BASS_NB_BAS_INSTANCES_MAX];
};

/// Parameters of the @ref BASS_ENABLE_RSP message
struct bass_enable_rsp
{
    /// connection index
    uint8_t conidx;
    ///status
    uint16_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_UPD_REQ message
struct bass_batt_level_upd_req
{
    /// BAS instance
    uint8_t bas_instance;
    /// Battery Level
    uint8_t batt_level;
};

///Parameters of the @ref BASS_BATT_LEVEL_UPD_RSP message
struct bass_batt_level_upd_rsp
{
    ///status
    uint16_t status;
};

///Parameters of the @ref BASS_BATT_LEVEL_NTF_CFG_IND message
struct bass_batt_level_ntf_cfg_ind
{
    /// connection index
    uint8_t conidx;
    ///Notification Configuration
    uint8_t ntf_cfg;
};

/// @} BASS

#endif /* _BASS_MSG_H_ */
