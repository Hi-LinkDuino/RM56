/**
 ****************************************************************************************
 *
 * @file basc_msg.h
 *
 * @brief Header file - Battery Service Client - Message API
 *
 * Copyright (C) RivieraWaves 2009-2019
 *
 ****************************************************************************************
 */


#ifndef _BASC_MSG_H_
#define _BASC_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup BASC Battery Service Client
 * @ingroup Profile
 * @brief Battery Service Client - Message API
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" // Task definitions
#include "prf_types.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*@TRACE*/
enum basc_msg_id
{
    /// Start the Battery Service Client Role - at connection
    BASC_ENABLE_REQ                 = MSG_ID(BASC, 0x00),
    ///Confirm that cfg connection has finished with discovery results, or that normal cnx started
    BASC_ENABLE_RSP                 = MSG_ID(BASC, 0x01),

    /// Read Characteristic Value Request
    BASC_READ_INFO_REQ              = MSG_ID(BASC, 0x02),
    /// Read Characteristic Value Request
    BASC_READ_INFO_RSP              = MSG_ID(BASC, 0x03),

    /// Write Battery Level Notification Configuration Value request
    BASC_BATT_LEVEL_NTF_CFG_REQ     = MSG_ID(BASC, 0x04),
    /// Write Battery Level Notification Configuration Value response
    BASC_BATT_LEVEL_NTF_CFG_RSP     = MSG_ID(BASC, 0x05),

    /// Indicate to APP that the Battery Level value has been received
    BASC_BATT_LEVEL_IND             = MSG_ID(BASC, 0x06),
};

/// Peer battery info that can be read
enum basc_info
{
    /// Battery Level value
    BASC_BATT_LVL_VAL,
    /// Battery Level Client Characteristic Configuration
    BASC_NTF_CFG,
    /// Battery Level Characteristic Presentation Format
    BASC_BATT_LVL_PRES_FORMAT,

    BASC_INFO_MAX,
};

/// Battery Service Characteristics
enum bass_char_type
{
    /// Battery Level
    BAS_CHAR_BATT_LEVEL,

    BAS_CHAR_MAX,
};

/// Battery Service Descriptors
enum bass_desc_type
{
    /// Battery Level Characteristic Presentation Format
    BAS_DESC_BATT_LEVEL_PRES_FORMAT,
    /// Battery Level Client Characteristic Configuration
    BAS_DESC_BATT_LEVEL_CFG,

    BAS_DESC_MAX,
};

/*
 * APIs Structure
 ****************************************************************************************
 */

///Structure containing the characteristics handles, value handles and descriptors
typedef struct bas_content
{
    /// service info
    prf_svc_t svc;

    /// Characteristic Info:
    /// - Battery Level
    prf_char_t chars[BAS_CHAR_MAX];

    /// Descriptor handles:
    /// - Battery Level Client Characteristic Configuration
    /// - Battery Level Characteristic Presentation Format
    prf_desc_t descs[BAS_DESC_MAX];
} bas_content_t;

/// Parameters of the @ref BASC_ENABLE_REQ message
struct basc_enable_req
{
    /// Connection Index
    uint8_t       conidx;
    ///Connection type
    uint8_t       con_type;
    /// Number of BAS instances that have previously been found
    uint8_t       bas_nb;
    /// Existing handle values bas
    bas_content_t bas[BASC_NB_BAS_INSTANCES_MAX];
};

/// Parameters of the @ref BASC_ENABLE_RSP message
struct basc_enable_rsp
{
    /// Connection Index
    uint8_t       conidx;
    /// Status
    uint16_t      status;
    /// Number of BAS that have been found
    uint8_t       bas_nb;
    ///Existing handle values bas
    bas_content_t bas[BASC_NB_BAS_INSTANCES_MAX];
};

///Parameters of the @ref BASC_READ_INFO_REQ message
struct basc_read_info_req
{
    /// Connection Index
    uint8_t conidx;
    ///Characteristic info see enum #basc_info
    uint8_t info;
    ///Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
};

///Parameters of the @ref  BASC_READ_INFO_RSP message
struct basc_read_info_rsp
{
    /// Connection Index
    uint8_t                 conidx;
    /// status of the request
    uint16_t                status;
    ///Characteristic info see enum #basc_info
    uint8_t                 info;
    ///Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t                 bas_nb;

    /// Information data
    union basc_data
    {
        /// Battery Level - if info = BASC_BATT_LVL_VAL
        uint8_t             batt_level;
        ///Notification Configuration Value - if info = BASC_NTF_CFG
        uint16_t            ntf_cfg;
        ///Characteristic Presentation Format - if info = BASC_BATT_LVL_PRES_FORMAT
        prf_char_pres_fmt_t char_pres_format;
    } data;
};

///Parameters of the @ref BASC_BATT_LEVEL_NTF_CFG_REQ message
struct basc_batt_level_ntf_cfg_req
{
    /// Connection Index
    uint8_t  conidx;
    ///Notification Configuration
    uint16_t ntf_cfg;
    ///Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t  bas_nb;
};

///Parameters of the @ref BASC_BATT_LEVEL_NTF_CFG_RSP message
struct basc_batt_level_ntf_cfg_rsp
{
    /// Connection Index
    uint8_t  conidx;
    ///Status
    uint16_t status;
    ///Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t  bas_nb;
};

///Parameters of the @ref BASC_BATT_LEVEL_IND message
struct basc_batt_level_ind
{
    /// Connection Index
    uint8_t conidx;
    ///Battery Level
    uint8_t batt_level;
    ///Battery Service Instance - From 0 to BASC_NB_BAS_INSTANCES_MAX-1
    uint8_t bas_nb;
};


/// @} BASC

#endif /* _BASC_MSG_H_ */
