/**
 ****************************************************************************************
 *
 * @file acc_tbc_msg.h
 *
 * @brief Audio Content Control - Definition of Kernel Messages (Telephone Bearer Client)
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef ACC_TBC_MSG_H_
#define ACC_TBC_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup ACC_TBC_MSG Audio Content Control - Definition of Kernel Messages
 * (Telephone Bearer Client)
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "acc_msg.h"        // ACC Message Definitions
#include "rwip_task.h"      // RW IP Task Definition
#include "acc_tbc.h"        // Audio Content Control - Telephone Bearer Client Definitions

/*
 * API MESSAGES
 ****************************************************************************************
 */

/// Structure for ACC_TBC_DISCOVER command message
typedef struct acc_tbc_discover_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Maximum number of Telephone Bearer Servie instance that can be found
    uint8_t nb_tbs_max;
} acc_tbc_discover_cmd_t;

/// Structure for ACC_TBC_GET command message
typedef struct acc_tbc_get_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Characteristic type
    uint8_t char_type;
} acc_tbc_get_cmd_t;

/// Structure for ACC_TBC_SET_CFG command message
typedef struct acc_tbc_set_cfg_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Indicate if sending of notifications must be enabled (!=0) or disabled
    uint8_t enable;
} acc_tbc_set_cfg_cmd_t;

/// Structure for ACC_TBC_GET_CFG command message
typedef struct acc_tbc_get_cfg_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Characteristic type
    uint8_t char_type;
} acc_tbc_get_cfg_cmd_t;

/// Structure for ACC_TBC_SET_REPORT_INTV command message
typedef struct acc_tbc_set_report_intv_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Indicate if feedback from Server device is required (!= 0) or not
    uint8_t reliable;
    /// Signal Strength Reporting Interval in seconds
    uint8_t sign_strength_intv_s;
} acc_tbc_set_report_intv_cmd_t;

/// Structure for ACC_TBC_CALL_OUTGOING command message
typedef struct acc_tbc_call_outgoing_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Indicate if feedback from Server device is required (!= 0) or not
    uint8_t reliable;
    /// Length of Outgoing URI value
    uint8_t uri_len;
    /// Outgoing URI value
    uint8_t uri[__ARRAY_EMPTY];
} acc_tbc_call_outgoing_cmd_t;

/// Structure for ACC_TBC_CALL_ACTION command message
typedef struct acc_tbc_call_action_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Indicate if feedback from Server device is required (!= 0) or not
    uint8_t reliable;
    /// Call index
    uint8_t call_id;
    /// Operation code
    uint8_t opcode;
} acc_tbc_call_action_cmd_t;

/// Structure for ACC_TBC_CALL_JOIN command message
typedef struct acc_tbc_call_join_cmd
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Indicate if feedback from Server device is required (!= 0) or not
    uint8_t reliable;
    /// Number of joined calls
    uint8_t nb_calls;
    /// Call index of calls to join
    uint8_t call_ids[__ARRAY_EMPTY];
} acc_tbc_call_join_cmd_t;

/// Structure for command complete event message
typedef struct acc_tbc_cmp_evt
{
    /// Command code (@see enum acc_cmd_codes)
    uint16_t cmd_code;
    /// Status
    uint16_t status;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    union
    {
        /// Value
        uint8_t val;
        /// Characteristic type
        uint8_t char_type;
        /// Operation code
        uint8_t opcode;
    } u;
    /// Call index
    uint8_t call_id;
    /// Result
    uint8_t result;
} acc_tbc_cmp_evt_t;

/// Structure for ACC_TBC_CONFIGURE request message
typedef struct acc_tbc_configure_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Preferred MTU
    uint16_t pref_mtu;
} acc_tbc_configure_req_t;

/// Structure for ACC_TBC_RESTORE_BOND_DATA request message
typedef struct acc_tbc_restore_bond_data_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Connection local index
    uint8_t con_lid;
    /// Number of instances of the (Generic) Telephone Bearer Service discovered
    uint8_t nb_bearers;
    /// Content description of (Generic) Telephone Bearer Service instances
    /// Generic Telephone Bearer Service is first service
    acc_tbc_tbs_info_t tbs_info[__ARRAY_EMPTY];
} acc_tbc_restore_bond_data_req_t;

/// Structure for response message
typedef struct acc_tbc_rsp
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Connection local index
    uint8_t con_lid;
} acc_tbc_rsp_t;

/// Structure for ACC_TBC_BOND_DATA indication message
typedef struct acc_tbc_bond_data_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// UUID
    uint16_t uuid;
    /// Content description of (Generic) Telephone Bearer Service instance
    acc_tbc_tbs_info_t tbs_info;
} acc_tbc_bond_data_ind_t;

/// Structure for ACC_TBC_CFG indication message
typedef struct acc_tbc_cfg_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Indicate if sending of notifications is enabled (!= 0) or not
    uint8_t enabled;
} acc_tbc_cfg_ind_t;

/// Structure for ACC_TBC_CALL_STATE indication message
typedef struct acc_tbc_call_state_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t id;
    /// Call flags
    uint8_t flags;
    /// Call state
    uint8_t state;
} acc_tbc_call_state_ind_t;

/// Structure for ACC_TBC_CALL_STATE_LONG indication message
typedef struct acc_tbc_call_state_long_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t id;
    /// Call flags
    uint8_t flags;
    /// Call state
    uint8_t state;
    /// Length of Incoming or Outgoing Call URI value
    uint8_t uri_len;
    /// Incoming or Outgoing Call URI value
    uint8_t uri[__ARRAY_EMPTY];
} acc_tbc_call_state_long_ind_t;

/// Structure for ACC_TBC_VALUE_LONG indication message
typedef struct acc_tbc_value_long_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t call_id;
    /// Characteristic type
    uint8_t char_type;
    /// Length of value
    uint16_t val_len;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} acc_tbc_value_long_ind_t;

/// Structure for ACC_TBC_VALUE indication message
typedef struct acc_tbc_value_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    /// Meaningful only for Termination Reason characteristic
    uint8_t call_id;
    /// Characteristic type
    uint8_t char_type;
    /// Value
    union
    {
        /// Value
        uint16_t val;
        /// Bearer Technology
        uint8_t techno;
        /// Signal Strength
        uint8_t sign_strength;
        /// Signal Strength Reporting Interval in seconds
        uint8_t sign_strength_intv_s;
        /// Content Control ID
        uint8_t ccid;
        /// Status Flags bit field
        uint16_t status_flags_bf;
        /// Call Control Point Optional Opcodes bit field
        uint16_t opt_opcodes_bf;
        /// Termination Reason
        uint8_t term_reason;
    } val;
} acc_tbc_value_ind_t;

/// Structure for ACC_TBC_SVC_CHANGED indication message
typedef struct acc_tbc_svc_changed_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Connection local index
    uint8_t con_lid;
} acc_tbc_svc_changed_ind_t;

/// @}

#endif // ACC_TBC_MSG_H_
