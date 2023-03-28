/**
 ****************************************************************************************
 *
 * @file acc_tbc.h
 *
 * @brief Audio Content Control - Telephone Bearer Client - Definitions
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef ACC_TBC_H_
#define ACC_TBC_H_

/**
 ****************************************************************************************
 * @addtogroup ACC_TBC Audio Content Control - Telephone Bearer Client
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf.h"               // GAF Defines
#include "acc_tb.h"            // Audio Content Control - Telephone Bearer Definition

#if (GAF_ACC_TBC)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of Telephone Bearer Service (Generic Telephone Bearer Service excluded)
/// that can be handled (limited by Bearer local index set in dummy value provided to GATT)
#define ACC_TBC_NB_TBS_MAX          (126)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// List of GAF_CMD command codes
enum acc_tbc_cmd_codes
{
    ACC_TBC_DISCOVER = GAF_CODE(ACC, TBC, 0),
    ACC_TBC_GET = GAF_CODE(ACC, TBC, 1),
    ACC_TBC_SET_CFG = GAF_CODE(ACC, TBC, 2),
    ACC_TBC_GET_CFG = GAF_CODE(ACC, TBC, 3),
    ACC_TBC_SET_REPORT_INTV = GAF_CODE(ACC, TBC, 4),
    ACC_TBC_CALL_OUTGOING = GAF_CODE(ACC, TBC, 5),
    ACC_TBC_CALL_ACTION = GAF_CODE(ACC, TBC, 6),
    ACC_TBC_CALL_JOIN = GAF_CODE(ACC, TBC, 7),
};

/// List of GAF_REQ request codes
enum acc_tbc_msg_req_codes
{
    ACC_TBC_CONFIGURE = GAF_CODE(ACC, TBC, 0),
    ACC_TBC_RESTORE_BOND_DATA = GAF_CODE(ACC, TBC, 1),
};

/// List of GAF_IND indication codes
enum acc_tbc_msg_ind_codes
{
    ACC_TBC_BOND_DATA = GAF_CODE(ACC, TBC, 0),
    ACC_TBC_CFG = GAF_CODE(ACC, TBC, 1),
    ACC_TBC_CALL_STATE = GAF_CODE(ACC, TBC, 2),
    ACC_TBC_CALL_STATE_LONG = GAF_CODE(ACC, TBC, 3),
    ACC_TBC_VALUE_LONG = GAF_CODE(ACC, TBC, 4),
    ACC_TBC_VALUE = GAF_CODE(ACC, TBC, 5),
    ACC_TBC_SVC_CHANGED = GAF_CODE(ACC, TBC, 6),
};
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Content description structure for Telephone Bearer Service
typedef struct acc_tbc_tbs_info
{
    /// Service description
    prf_svc_t svc_info;
    /// Characteristics description
    prf_char_t char_info[ACC_TB_CHAR_TYPE_MAX];
    /// Descriptors description
    prf_desc_t desc_info[ACC_TB_DESC_TYPE_MAX];
} acc_tbc_tbs_info_t;

/*
 * CALLBACK FUNCTIONS DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback function called when handling of a command has been completed
 *
 * @param[in] cmd_code      Command code
 * @param[in] status        Status
 * @param[in] con_lid       Connection local index
 * @param[in] bearer_lid    Bearer local index
 *                          Meaningless for ACC_TBC_DISCOVER command
 * @param[in] param         Characteristic type for ACC_TBC_GET command
 *                          Characteristic type for ACC_TBC_SET_CFG/GET_CFG commands
 *                          Operation code for ACC_TBC_CALL_OUTGOING/ACTION/JOIN commands
 *                          Meaningless for ACC_TBC_SET_REPORT_INTV command
 * @param[in] call_id       Call index for ACC_TBC_CALL_OUTGOING/ACTION/JOIN commands.
 *                          Meaningless for ACC_TBC_CALL_OUTGOING command if sending of notifications
 *                          not enabled for Call Control Point characteristic
 * @param[in] result        Control point result for ACC_TBC_CALL_OUTGOING/ACTION/JOIN commands.
 *                          Meaningless if sending of notifications not enabled for Call Control
 *                          Point characteristic
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_cmp_evt)(uint16_t cmd_code, uint16_t status,
                                   uint8_t con_lid, uint8_t bearer_lid, uint8_t param,
                                   uint8_t call_id, uint8_t result);

/**
 ****************************************************************************************
 * @brief Callback function called when Client Characteristic Configuration of a notification-capable
 * characteristic has been received
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] char_type     Characteristic type
 * @param[in] enabled           Indicate if sending of notifications is enabled or not
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_cfg)(uint8_t con_lid, uint8_t bearer_lid, uint8_t char_type,
                               uint8_t enabled);

/**
 ****************************************************************************************
 * @brief Callback function called when Call state has been received through Call State
 * characteristic
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] call_id           Call Index
 * @param[in] flags             Call Flags
 * @param[in] state             Call State
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_call_state)(uint8_t con_lid, uint8_t bearer_lid, uint8_t call_id,
                                      uint8_t flags, uint8_t state);

/**
 ****************************************************************************************
 * @brief Callback function called when Call state has been received through Bearer List
 * Current Calls characteristic
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] call_id           Call Index
 * @param[in] flags             Call Flags
 * @param[in] state             Call State
 * @param[in] uri_len           Length of Incoming or Outgoing Call URI value
 * @param[in] p_uri             Pointer to Incoming or Outgoing Call URI value
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_call_state_long)(uint8_t con_lid, uint8_t bearer_lid, uint8_t call_id,
                                           uint8_t flags, uint8_t state, uint8_t uri_len,
                                           uint8_t* p_uri);

/**
 ****************************************************************************************
 * @brief Callback function called when value of one of the following characteristic is
 * received:
 *     - Bearer Provider Name characteristic
 *     - Bearer UCI characteristic
 *     - Bearer URI Schemes Supported List characteristic
 *     - Incoming Call Target Bearer URI characteristic
 *     - Incoming Call characteristic
 *     - Call Friendly Name characteristic
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] call_id           Call ID
 * @param[in] char_type         Characteristic type
 * @param[in] val_len           Length of value
 * @param[in] p_val             Pointer to value
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_value_long)(uint8_t con_lid, uint8_t bearer_lid, uint8_t call_id,
                                      uint8_t char_type, uint16_t val_len, uint8_t* p_val);

/**
 ****************************************************************************************
 * @brief Callback function called when value of one of the following characteristic is
 * received:
 *     - Bearer Technology characteristic
 *     - Bearer Signal Strength characteristic
 *     - Bearer Signal Strength Reporting Interval characteristic
 *     - Content Control ID characteristic
 *     - Status Flags characteristic
 *     - Call Control Point Optional Opcodes characteristic
 *     - Termination Reason characteristic
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] call_id           Call index
 * @param[in] char_type         Characteristic type
 * @param[in] val               Value
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_value)(uint8_t con_lid, uint8_t bearer_lid, uint8_t call_id,
                                 uint8_t char_type, uint16_t val);

/**
 ****************************************************************************************
 * @brief Callback function called when an instance of (Generic) Telephone Bearer Control
 * Service has been discovered
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] uuid              UUID
 * @param[in] p_tbs_info        Pointer to Content description structure
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_bond_data)(uint8_t con_lid, uint8_t bearer_lid, uint16_t uuid,
                                 acc_tbc_tbs_info_t* p_tbs_info);

/**
 ****************************************************************************************
 * @brief Callback function called when a service changed indicated is received from a Server device
 *
 * @param[in] con_lid           Connection local index
 ****************************************************************************************
 */
typedef void (*acc_tbc_cb_svc_changed)(uint8_t con_lid);

/*
 * CALLBACK SET DEFINITION
 ****************************************************************************************
 */

/// Set of callback functions for Telephone Bearer Client
typedef struct acc_tbc_cb
{
    /// Callback function called when handling of a command has been completed
    acc_tbc_cb_cmp_evt cb_cmp_evt;
    /// Callback function called when Client Characteristic Configuration of a notification-capable
    /// characteristic has been received
    acc_tbc_cb_cfg cb_cfg;
    /// Callback function called when Call state has been received through Call State
    /// characteristic
    acc_tbc_cb_call_state cb_call_state;
    /// Callback function called when Call state has been received through Bearer List
    /// Current Calls characteristic
    acc_tbc_cb_call_state_long cb_call_state_long;
    /// Callback function called when value of one of the following characteristic is
    /// received:
    ///     - Bearer Provider Name characteristic
    ///     - Bearer UCI characteristic
    ///     - Bearer URI Schemes Supported List characteristic
    ///     - Incoming Call Target Bearer URI characteristic
    ///     - Incoming Call characteristic
    ///     - Call Friendly Name characteristic
    acc_tbc_cb_value_long cb_value_long;
    /// Callback function called when value of one of the following characteristic is
    /// received:
    ///     - Bearer Technology characteristic
    ///     - Bearer Signal Strength characteristic
    ///     - Bearer Signal Strength Reporting Interval characteristic
    ///     - Content Control ID characteristic
    ///     - Status Flags characteristic
    ///     - Call Control Point Optional Opcodes characteristic
    ///     - Termination Reason characteristic
    acc_tbc_cb_value cb_value;
    /// Callback function called when an instance of (Generic) Telephone Bearer Control
    /// Service has been discovered
    acc_tbc_cb_bond_data cb_bond_data;
    /// Callback function called when a service changed indicated is received from a Server device
    acc_tbc_cb_svc_changed cb_svc_changed;
} acc_tbc_cb_t;

/*
 * API FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Configure use of Telephone Bearer Client module as Client
 *
 * @param[in] p_cb          Pointer to set of callback functions for communication with upper
 * layers
 * @param[in] pref_mtu      Preferred MTU. Values from 0 to 63 are equivalent to 64
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_configure(const acc_tbc_cb_t* p_cb, uint16_t pref_mtu);

/**
 ****************************************************************************************
 * @brief Enable use of Telephone Bearer Service block as Client for a Server device with which
 * a bonding has been established during a previous connection.
 *
 * @param[in] con_lid       Connection local index
 * @param[in] nb_bearers    Number of instances of the (Generic) Telephone Bearer Service
 * @param[in] p_tbs_info    Pointer to Content description structures
 *                          Generic Telephone Bearer Service first
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_restore_bond_data(uint8_t con_lid, uint8_t nb_bearers, acc_tbc_tbs_info_t* p_tbs_info);

/**
 ****************************************************************************************
 * @brief Enable use of Telephone Bearer Service block as Client for a connected device with which
 * no bonding has been established during a previous connection.
 * Sending of notifications for Termination Reason characteristic of each discovered (Generic)
 * Telephone Bearer Service instance is enabled during the discovery procedure.
 *
 * @param[in] con_lid       Connection local index
 * @param[in] nb_tbs_max    Maximum number of Telephone Bearer Service instances that can be found
 *                          in peer device database additionally to the Generic Telephone Bearer
 *                          Service
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_discover(uint8_t con_lid, uint8_t nb_tbs_max);

/**
 ****************************************************************************************
 * @brief Get value for one of the following characteristics for a Bearer exposed by a Server device:
 *    - Bearer Provider Name characteristic
 *    - Bearer UCI characteristic
 *    - Bearer Technology characteristic
 *    - Bearer URI Schemes Supported List characteristic
 *    - Bearer Signal Strength characteristic if characteristic is supported for the indicated Bearer
 *    - Bearer Signal Strength Reporting Interval characteristic if characteristic is supported for the
 *    indicated Bearer
 *    - Bearer List Current Calles characteristic
 *    - Content Control ID characteristic
 *    - Status Flags characteristic
 *    - Incoming Call Target Bearer URI characteristic if characteristic is supported for the indicated Bearer
 *    - Call State characteristic
 *    - Call Control Point Optional Opcodes characteristic
 *    - Incoming Call characteristic
 *    - Call Friendly Name if characteristic is supported for the indicated Bearer
 *
 * @param[in] con_lid       Connection local index
 * @param[in] bearer_lid    Bearer local index
 * @param[in] char_type     Characteristic type
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_get(uint8_t con_lid, uint8_t bearer_lid, uint8_t char_type);

/**
 ****************************************************************************************
 * @brief Enable or disable sending of notifications for one of the following characteristics for
 * a Bearer exposed by a Server device:
 *    - Bearer Provider Name characteristic
 *    - Bearer Technology characteristic
 *    - Bearer Signal Strength characteristic if characteristic is supported for the indicated Bearer
 *    - Bearer List Current Calls characteristic
 *    - Status Flags characteristic
 *    - Incoming Call Target Bearer URI characteristic if characteristic is supported for the indicated
 *    Bearer
 *    - Call State characteristic
 *    - Termination Reason characteristic
 *    - Incoming Call characteristic
 *    - Call Friendly Name if characteristic is supported for the indicated Bearer
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] char_type     Characteristic type
 * @param[in] enable            Indicate if sending of notifications must be enabled (!= 0) or
 * disabled for the indicated characteristic
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_set_cfg(uint8_t con_lid, uint8_t bearer_lid, uint8_t char_type,
                         uint8_t enable);

/**
 ****************************************************************************************
 * @brief Get current client configuration for one of the following characteristics for a Bearer
 * exposed by a Server device:
 *    - Bearer Provider Name characteristic
 *    - Bearer Technology characteristic
 *    - Bearer Signal Strength characteristic if characteristic is supported for the indicated Bearer
 *    - Bearer List Current Calls characteristic
 *    - Status Flags characteristic
 *    - Incoming Call Target Bearer URI characteristic if characteristic is supported for the indicated
 *    Bearer
 *    - Call State characteristic
 *    - Termination Reason characteristic
 *    - Incoming Call characteristic
 *    - Call Friendly Name if characteristic is supported for the indicated Bearer
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] char_type     Characteristic type
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_get_cfg(uint8_t con_lid, uint8_t bearer_lid, uint8_t char_type);

/**
 ****************************************************************************************
 * @brief Configure Signal Strength Reporting Interval (by setting Bearer Signal Strength Reporting
 * Interval characteristic value) for a Bearer exposed by a Service device if reporting of signal
 * strength is supported by the Bearer.
 * Upper layer can require use of either a Write Request or a Write Command by using the ‘reliable’
 * parameter.
 *
 * @param[in] con_lid               Connection local index
 * @param[in] bearer_lid            Bearer local index
 * @param[in] reliable              Indicate if feedback from Server device is required (!= 0)
 * or not
 * @param[in] sign_strength_intv_s  Signal Strength Reporting Interval in seconds
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_set_report_intv(uint8_t con_lid, uint8_t bearer_lid, uint8_t reliable,
                                 uint8_t sign_strength_intv_s);

/**
 ****************************************************************************************
 * @brief Require Server device to originate an outgoing call on one of its exposed Bearer
 * (by setting Call Control Point characteristic value on the indicated Bearer).
 * Upper layer can require use of either a Write Request or a Write Command by using the ‘reliable’
 * parameter.
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] reliable          Indicate if feedback from Server device is required (!= 0)
 * or not
 * @param[in] uri_len           Length of Outgoing URI value
 * @param[in] p_uri             Pointer to Outgoing URI value
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_call_outgoing(uint8_t con_lid, uint8_t bearer_lid, uint8_t reliable,
                               uint8_t uri_len, uint8_t* p_uri);

/**
 ****************************************************************************************
 * @brief Accept, terminate, hold or retrieve a call currently existing on a Bearer exposed
 * by a Server device (by setting Call Control Point characteristic value on the indicated Bearer).
 * Upper layer can require use of either a Write Request or a Write Command by using the ‘reliable’
 * parameter.
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] reliable          Indicate if feedback from Server device is required (!= 0)
 * or not
 * @param[in] call_id           Call index
 * @param[in] opcode            Operation code
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_call_action(uint8_t con_lid, uint8_t bearer_lid, uint8_t reliable,
                             uint8_t call_id, uint8_t opcode);

/**
 ****************************************************************************************
 * @brief Join two or more calls currently existing on a Bearer exposed by a Server device
 * (by setting Call Control Point characteristic value on the indicated Bearer).
 * Note that only the first provided Call index is returned in the command complete event.
 * Upper layer can require use of either a Write Request or a Write Command by using the
 * ‘reliable’ parameter.
 *
 * @param[in] con_lid           Connection local index
 * @param[in] bearer_lid        Bearer local index
 * @param[in] reliable          Indicate if feedback from Server device is required (!= 0)
 * or not
 * @param[in] nb_calls          Number of calls to join
 * @param[in] p_call_ids        Pointer to lisst of Call indexes of calls to join
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbc_call_join(uint8_t con_lid, uint8_t bearer_lid, uint8_t reliable,
                             uint8_t nb_calls, uint8_t* p_call_ids);

#endif //(GAF_ACC_TBC)

/// @}

#endif // ACC_TBC_H_
