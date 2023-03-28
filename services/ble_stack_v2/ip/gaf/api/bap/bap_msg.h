/**
 ****************************************************************************************
 *
 * @file bap_msg.h
 *
 * @brief Basic Audio Profile - Definition of Kernel Messages
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef BAP_MSG_H_
#define BAP_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup BAP Basic Audio Profile - Definition of Kernel Messages
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "bap.h"            // BAP Definitions

#if (GAF_BAP)

#include "gaf_msg.h"        // Generic Audio Framework API Message Definitions

/*
 * API MESSAGES
 ****************************************************************************************
 */

/// Structure for BAP_CONFIGURE request message
typedef struct bap_configure_req
{
    /// Request code (@see enum bap_msg_req_code)
    ///  - BAP_CONFIGURE
    uint16_t req_code;
    /// Supported role bit field (@see enum bap_role_bf)
    uint32_t role_bf;
    /// Capabilities Server configuration
    bap_capa_srv_cfg_t capa_srv_cfg;
    /// Capabilities Client configuration
    bap_capa_cli_cfg_t capa_cli_cfg;
    /// Unicast Server configuration
    bap_uc_srv_cfg_t uc_srv_cfg;
    /// Unicast Client configuration
    bap_uc_cli_cfg_t uc_cli_cfg;
    /// Broadcast Scan configuration
    bap_bc_scan_cfg_t bc_scan_cfg;
    /// Broadcast Delegator configuration
    bap_bc_deleg_cfg_t bc_deleg_cfg;
    /// Broadcast Assistant configuration
    bap_bc_assist_cfg_t bc_assist_cfg;
} bap_configure_req_t;

/// Structure for BAP_GET_ROLES request message
typedef struct bap_get_roles_req
{
    /// Request code (@see enum bap_msg_req_code)
    ///  - BAP_GET_ROLES
    uint16_t req_code;
} bap_get_roles_req_t;

/// Structure for BAP_GET_ROLES response message
typedef struct bap_get_roles_rsp
{
    /// Request code (@see enum bap_msg_req_code)
    ///  - BAP_GET_ROLES
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Role bit field (@see enum bap_role_bf)
    uint32_t role_bf;
} bap_get_roles_rsp_t;

#endif //(GAF_BAP)

/// @}

#endif // BAP_MSG_H_
