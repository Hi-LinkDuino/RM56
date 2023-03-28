/**
 ****************************************************************************************
 *
 * @file gaf_msg.h
 *
 * @brief Generic Audio Framework - Definition of Kernel Messages
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef GAF_MSG_H_
#define GAF_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup GAF Generic Audio Framework - Definition of Kernel Messages
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf.h"            // GAF Definitions
#include "rwip_task.h"      // RW IP Task Definition

#if (GAF_API_MSG)

#include "ke_task.h"        // Kernel Tasks Definitions

/*
 * MESSAGE API
 ****************************************************************************************
 */

/// List of GAF message IDs
enum acc_msg_ids
{
    /// GAF Command
    GAF_CMD         = MSG_ID(GAF, 0),
    /// GAF Command complete event
    GAF_CMP_EVT     = MSG_ID(GAF, 1),
    /// GAF Request
    GAF_REQ         = MSG_ID(GAF, 2),
    /// GAF Response
    GAF_RSP         = MSG_ID(GAF, 3),
    /// GAF Indication
    GAF_IND         = MSG_ID(GAF, 4),
    /// GAF Request Indication
    GAF_REQ_IND     = MSG_ID(GAF, 5),
    /// GAF Confirmation
    GAF_CFM         = MSG_ID(GAF, 6),
};


/// List of GAF_IND indication codes
enum gaf_msg_ind_codes
{
    GAF_UNKNOWN_MSG = GAF_CODE(GAF, COMMON, 0),
};




/// Basic structure for XXX_CMD message
typedef struct gaf_cmd
{
    /// Command code
    uint16_t cmd_code;
} gaf_cmd_t;

/// Basic structure for XXX_CMP_EVT message
typedef struct gaf_cmp_evt
{
    /// Command code
    uint16_t cmd_code;
    /// Status
    uint16_t status;
} gaf_cmp_evt_t;

/// Basic structure for XXX_REQ message
typedef struct gaf_req
{
    /// Request code
    uint16_t req_code;
} gaf_req_t;

/// Basic structure for XXX_RSP message
typedef struct gaf_rsp
{
    /// Request code
    uint16_t req_code;
    /// Status
    uint16_t status;
} gaf_rsp_t;

/// Basic structure for XXX_IND message
typedef struct gaf_ind
{
    /// Indication code
    uint16_t ind_code;
} gaf_ind_t;

/// Basic structure for XXX_REQ_IND message
typedef struct gaf_req_ind
{
    /// Request Indication code
    uint16_t req_ind_code;
} gaf_req_ind_t;

/// Basic structure for XXX_CFM message
typedef struct gaf_cfm
{
    /// Request Indication code
    uint16_t req_ind_code;
    /// Status
    uint16_t status;
} gaf_cfm_t;

/// Structure for GAF_UNKNOWN_MSG indication message
typedef struct gaf_unknown_msg_ind
{
    /// Indication code (@see enum gaf_msg_ind_codes)
    uint16_t ind_code;
    /// Message ID
    ke_msg_id_t msg_id;
} gaf_unknown_msg_ind_t;


#endif //(GAF_API_MSG)

/// @}

#endif // GAF_MSG_H_
