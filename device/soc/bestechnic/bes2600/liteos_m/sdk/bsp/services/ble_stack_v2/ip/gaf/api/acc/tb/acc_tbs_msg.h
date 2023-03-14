/**
 ****************************************************************************************
 *
 * @file acc_tbs_msg.h
 *
 * @brief Audio Content Control - Definition of Kernel Messages (Telephone Bearer Server)
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef ACC_TBS_MSG_H_
#define ACC_TBS_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup ACC_TBS_MSG Audio Content Control - Definition of Kernel Messages
 * (Telephone Bearer Server)
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf.h"            // GAF Defines
#include "acc_msg.h"        // ACC Message Definitions
#include "rwip_task.h"      // RW IP Task Definition
#include "acc_tbs.h"        // Audio Content Control - Telephone Bearer Server Definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of GAF_REQ request codes
enum acc_tbs_msg_req_codes
{
    ACC_TBS_CONFIGURE = GAF_CODE(ACC, TBS, 0),
    ACC_TBS_ADD = GAF_CODE(ACC, TBS, 1),
    ACC_TBS_ENABLE = GAF_CODE(ACC, TBS, 2),
    ACC_TBS_RESTORE_BOND_DATA = GAF_CODE(ACC, TBS, 3),
    ACC_TBS_SET = GAF_CODE(ACC, TBS, 4),
    ACC_TBS_SET_STATUS = GAF_CODE(ACC, TBS, 5),
    ACC_TBS_SET_LONG = GAF_CODE(ACC, TBS, 6),
    ACC_TBS_CALL_INCOMING = GAF_CODE(ACC, TBS, 7),
    ACC_TBS_CALL_OUTGOING = GAF_CODE(ACC, TBS, 8),
    ACC_TBS_CALL_ACTION = GAF_CODE(ACC, TBS, 9),
    ACC_TBS_CALL_JOIN = GAF_CODE(ACC, TBS, 10),
};

/// List of GAF_IND indication codes
enum acc_tbs_msg_ind_codes
{
    ACC_TBS_BOND_DATA = GAF_CODE(ACC, TBS, 0),
    ACC_TBS_REPORT_INTV = GAF_CODE(ACC, TBS, 1),
};

/// List of GAF_REQ_IND request indication codes
enum acc_tbs_msg_req_ind_codes
{
    ACC_TBS_GET = GAF_CODE(ACC, TBS, 0),
    ACC_TBS_CALL_OUTGOING_RI = GAF_CODE(ACC, TBS, 1),
    ACC_TBS_CALL_ACTION_RI = GAF_CODE(ACC, TBS, 2),
    ACC_TBS_CALL_JOIN_RI = GAF_CODE(ACC, TBS, 3),
};

/*
 * API MESSAGES
 ****************************************************************************************
 */

/// Structure for ACC_TBS_CONFIGURE request message
typedef struct acc_tbs_configure_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Number of instances of the Telephone Bearer Service
    /// 0 means that only the Generic Telephone Bearer Service is supported
    uint8_t nb_tbs;
    /// Size of pool containing pre-allocated structures used to manage calls
    uint8_t call_pool_size;
    /// Maximum URI length allowed for pre-allocated call structure
    uint8_t call_pool_uri_len;
    /// Maximum URI length for a call
    uint8_t uri_len_max;
    /// Preferred MTU
    /// Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
} acc_tbs_configure_req_t;

/// Structure for ACC_TBS_ADD request message
typedef struct acc_tbs_add_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Configuration bit field
    uint8_t cfg_bf;
    /// Number of attributes reserved for the service
    uint8_t nb_att_rsvd;
    /// Content Control ID
    uint8_t ccid;
    /// Optional operation codes bit field
    uint8_t opt_opcodes_bf;
    /// Length of Bearer UCI value
    uint8_t bearer_uci_len;
    /// Bearer UCI value
    uint8_t val[__ARRAY_EMPTY];
} acc_tbs_add_req_t;

/// Structure for ACC_TBS_ENABLE request message
typedef struct acc_tbs_enable_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
} acc_tbs_enable_req_t;

/// Structure for ACC_TBS_RESTORE_BOND_DATA request message
typedef struct acc_tbs_restore_bond_data_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Signal strength reporting interval in seconds
    /// Meaningful only if Bearer Signal Strength characteristic is supported for the indication bearer
    uint8_t sign_strength_intv_s;
    /// Client configuration bit field
    uint16_t cli_cfg_bf;
    /// Event configuration bit field
    uint16_t evt_cfg_bf;
} acc_tbs_restore_bond_data_req_t;

/// Structure for ACC_TBS_SET request message
typedef struct acc_tbs_set_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Characteristic type
    uint8_t char_type;
    union
    {
        /// Value
        uint8_t val;
        /// Bearer technology
        uint8_t techno;
        /// Signal strength. Value from 101 to 254 are prohibited
        /// 255 means that signal strength values cannot be measured or has no meaning
        uint8_t signal_strength;
    } val;
} acc_tbs_set_req_t;

/// Structure for ACC_TBS_SET_STATUS request message
typedef struct acc_tbs_set_status_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Status type
    uint8_t status_type;
    /// Value
    uint8_t val;
} acc_tbs_set_status_req_t;

/// Structure for ACC_TBS_SET_LONG request message
typedef struct acc_tbs_set_long_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Length of value
    uint8_t len;
    /// Value
    uint8_t val[__ARRAY_EMPTY];
} acc_tbs_set_long_req_t;

/// Structure for ACC_TBS_CALL_INCOMING request message
typedef struct acc_tbs_call_incoming_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Length of Incoming URI value
    uint8_t uri_len;
    /// Length of Incoming Call Target URI value
    /// Meaningful only if Incoming Call Target Bearer URI characteristic is supported
    uint8_t tgt_uri_len;
    /// Length of Friendly Name value
    /// Meaningful only if Call Friendly Name characteristic is supported
    uint8_t friendly_name_len;
    /// Incoming URI value followed by Incoming Call Target URI value followed by
    /// Friendly Name value
    uint8_t val[__ARRAY_EMPTY];
} acc_tbs_call_incoming_req_t;

/// Structure for ACC_TBS_CALL_OUTGOING request message
typedef struct acc_tbs_call_outgoing_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Length of Outgoing URI value
    uint8_t uri_len;
    /// Length of Friendly Name value
    /// Meaningful only if Call Friendly Name characteristic is supported
    uint8_t friendly_name_len;
    /// Incoming URI value followed by Outgoing URI value followed by Friendly Name value
    uint8_t val[__ARRAY_EMPTY];
} acc_tbs_call_outgoing_req_t;

/// Structure for ACC_TBS_CALL_ACTION request message
typedef struct acc_tbs_call_action_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t call_id;
    /// Action
    uint8_t action;
    /// Termination reason
    uint8_t reason;
} acc_tbs_call_action_req_t;

/// Structure for ACC_TBS_CALL_JOIN request message
typedef struct acc_tbs_call_join_req
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Number of joined calls
    uint8_t nb_calls;
    /// Call index of calls to join
    uint8_t call_ids[__ARRAY_EMPTY];
} acc_tbs_call_join_req_t;

/// Structure for response message
typedef struct acc_tbs_rsp
{
    /// Request code (@see enum acc_req_codes)
    uint16_t req_code;
    /// Status
    uint16_t status;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Union
    union
    {
        /// Value
        uint8_t val;
        /// Characteristic type for ACC_TBS_SET request
        uint8_t char_type;
        /// Status type for ACC_TBS_SET_STATUS request
        uint8_t status_type;
        /// Connection local index for ACC_TBS_RESTORE_BOND_DATA request
        uint8_t con_lid;
        /// Allocated Call index
        uint8_t call_id;
    } u;
    /// Action for ACC_TBS_CALL_ACTION request
    uint8_t action;
} acc_tbs_rsp_t;

/// Structure for ACC_TBS_BOND_DATA indication message
typedef struct acc_tbs_cfg_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Client configuration bit field
    uint16_t cli_cfg_bf;
} acc_tbs_cfg_ind_t;

/// Structure for ACC_TBS_REPORT_INTV indication message
typedef struct acc_tbs_report_intv_ind
{
    /// Indication code (@see enum acc_ind_codes)
    uint16_t ind_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Signal Strength Reporting Interval in seconds
    uint8_t sign_strength_intv_s;
} acc_tbs_report_intv_ind_t;

/// Structure for ACC_TBS_GET request indication message
typedef struct acc_tbs_get_req_ind
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t call_id;
    /// Connection local index
    uint8_t con_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Token value to return in the confirmation
    uint16_t token;
    /// Offset
    uint16_t offset;
    /// Maximum length
    uint16_t length;
} acc_tbs_get_req_ind_t;

/// Structure for ACC_TBS_CALL_OUTGOING request indication message
typedef struct acc_tbs_call_out_req_ind
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Call index
    uint8_t call_id;
    /// Length of Outgoing URI value
    uint8_t uri_len;
    /// Outgoing URI value
    uint8_t uri[__ARRAY_EMPTY];
} acc_tbs_call_out_req_ind_t;

/// Structure for ACC_TBS_CALL_ACTION request indication message
typedef struct acc_tbs_call_action_req_ind
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Connection local index
    uint8_t con_lid;
    /// Operation code
    uint8_t opcode;
    /// Call index
    uint8_t call_id;
} acc_tbs_call_action_req_ind_t;

/// Structure for ACC_TBS_CALL_JOIN request indication message
typedef struct acc_tbs_call_join_req_ind
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Connection local index
    uint8_t con_lid;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Number of calls
    uint8_t nb_calls;
    /// List of call indexes
    uint8_t call_ids[__ARRAY_EMPTY];
} acc_tbs_call_join_req_ind_t;

/// Structure for ACC_TBS_GET confirmation message
typedef struct acc_tbs_get_cfm
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Status
    uint16_t status;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Call index
    uint8_t call_id;
    /// Connection local index
    uint8_t con_lid;
    /// Characteristic type
    uint8_t char_type;
    /// Token value to return in the confirmation
    uint16_t token;
    /// Offset
    uint16_t offset;
    /// Length
    uint16_t length;
    /// Request value
    uint8_t val[__ARRAY_EMPTY];
} acc_tbs_get_cfm_t;

/// Structure for ACC_TBS_CALL_ACTION/JOIN confirmation message
typedef struct acc_tbs_call_cfm
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Status
    uint16_t status;
    /// Bearer local index
    uint8_t bearer_lid;
} acc_tbs_call_cfm_t;


/// Structure for ACC_TBS_CALL_OUTGOING confirmation message
typedef struct acc_tbs_call_out_cfm
{
    /// Request Indication code (@see enum acc_req_ind_codes)
    uint16_t req_ind_code;
    /// Status
    uint16_t status;
    /// Bearer local index
    uint8_t bearer_lid;
    /// Length of Call Friendly Name value
    uint8_t friendly_name_len;
    /// Call Friendly Name value
    uint8_t friendly_name[__ARRAY_EMPTY];
} acc_tbs_call_out_cfm_t;

/// @}

#endif // ACC_TBS_MSG_H_
