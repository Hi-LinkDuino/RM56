/**
 ****************************************************************************************
 *
 * @file acc_tbs.h
 *
 * @brief Audio Content Control - Telephone Bearer Server - Definitions
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef ACC_TBS_H_
#define ACC_TBS_H_

/**
 ****************************************************************************************
 * @addtogroup ACC_TBS Audio Content Control - Telephone Bearer Server
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "acc_tb.h"            // Audio Content Control - Telephone Bearer Definition

#if (GAF_ACC_TBS)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Bearer configuration bit field
enum acc_tbs_cfg_bf
{
    /// Indicate if Bearer Signal Strength can be measured and reported to a Client device
    ACC_TBS_CFG_SIGNAL_STRENGTH_SUPP_POS = 0,
    ACC_TBS_CFG_SIGNAL_STRENGTH_SUPP_BIT = CO_BIT(ACC_TBS_CFG_SIGNAL_STRENGTH_SUPP_POS),

    /// Indicate if Incoming Call Target Bearer URI characteristic is supported or not
    ACC_TBS_CFG_INCOMING_TARGET_URI_SUPP_POS = 1,
    ACC_TBS_CFG_INCOMING_TARGET_URI_SUPP_BIT = CO_BIT(ACC_TBS_CFG_INCOMING_TARGET_URI_SUPP_POS),

    /// Indicate if Call Friendly Name characteristic is supported or not
    ACC_TBS_CFG_FRIENDLY_NAME_SUPP_POS = 2,
    ACC_TBS_CFG_FRIENDLY_NAME_SUPP_BIT = CO_BIT(ACC_TBS_CFG_FRIENDLY_NAME_SUPP_POS),

    /// Indicate if sending of notifications is supported or not for Bearer URI Schemes
    /// Supported List characteristic
    ACC_TBS_CFG_URI_SCHEMES_NTF_SUPP_POS = 3,
    ACC_TBS_CFG_URI_SCHEMES_NTF_SUPP_BIT = CO_BIT(ACC_TBS_CFG_URI_SCHEMES_NTF_SUPP_POS),
};

/// Call action values
enum acc_tbs_call_action
{
    /// Accept call
    ACC_TBS_ACTION_ACCEPT = 0,
    /// Terminate call
    ACC_TBS_ACTION_TERMINATE,
    /// Hold call (local)
    ACC_TBS_ACTION_HOLD_LOCAL,
    /// Retrieve call (local)
    ACC_TBS_ACTION_RETRIEVE_LOCAL,
    /// Hold call (remote)
    ACC_TBS_ACTION_HOLD_REMOTE,
    /// Retrieve call (remote)
    ACC_TBS_ACTION_RETRIEVE_REMOTE,
    /// Remote alert started for a call
    ACC_TBS_ACTION_ALERT_START,
    /// Remote answer for a call
    ACC_TBS_ACTION_ANSWER,

    ACC_TBS_ACTION_MAX
};

/// Status type values
enum acc_tbs_status_type
{
    /// Inband ringtone
    ACC_TBS_STATUS_TYPE_INBAND_RINGTONE = 0,
    /// Silent mode
    ACC_TBS_STATUS_TYPE_SILENT_MODE,

    ACC_TBS_STATUS_TYPE_MAX,
};

/*
 * CALLBACK FUNCTIONS DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback function called when Client Characteristic Configuration of a notification-capable
 * characteristic has been updated by a peer client
 *
 * @param[in] bearer_lid    Bearer local index
 * @param[in] con_lid       Connection local index
 * @param[in] cli_cfg_bf    Client configuration bit field
 ****************************************************************************************
 */
typedef void (*acc_tbs_cb_bond_data)(uint8_t bearer_lid, uint8_t con_lid, uint16_t cli_cfg_bf);

/**
 ****************************************************************************************
 * @brief Callback function called when Input State characteristic value has been updated
 *
 * @param[in] bearer_lid            Bearer local index
 * @param[in] con_lid               Connection local index
 * @param[in] sign_strength_intv_s  Signal Strength Reporting Interval in seconds
 ****************************************************************************************
 */
typedef void (*acc_tbs_cb_report_intv)(uint8_t bearer_lid, uint8_t con_lid, uint8_t sign_strength_intv_s);

/**
 ****************************************************************************************
 * @brief Callback function called to request from upper layer complete or piece of value for
 * one of the following characteristics:
 *      - Bearer Provider Name characteristic
 *      - Incoming Call Target Bearer URI characteristic
 *      - Call Friendly Name characteristic
 *
 * @param[in] bearer_lid        Bearer local index
 * @param[in] call_id           Call index
 * @param[in] con_lid           Connection local index
 * @param[in] char_type         Characteristic type
 * @param[in] token             Token
 * @param[in] offset            Offset
 * @param[in] length            Length
 ****************************************************************************************
 */
typedef void (*acc_tbs_cb_get_req)(uint8_t bearer_lid, uint8_t call_id, uint8_t con_lid,
                                   uint8_t char_type, uint16_t token, uint16_t offset,
                                   uint16_t length);

/**
 ****************************************************************************************
 * @brief Callback function called to inform upper layer that a client device has requested creation
 * of an outgoing call
 *
 * @param[in] bearer_lid        Bearer local index
 * @param[in] con_lid           Connection local index
 * @param[in] opcode            Operation code
 * @param[in] call_id           Call index
 * @param[in] len               Number of calls to join or Outgoing URI length
 * @param[in] p_val             Pointer to list of calls to join or to Outgoing URI value
 ****************************************************************************************
 */
typedef void (*acc_tbs_cb_call_req)(uint8_t bearer_lid, uint8_t con_lid, uint8_t opcode,
                                    uint8_t call_id, uint8_t len, uint8_t* p_val);

/*
 * CALLBACK SET DEFINITION
 ****************************************************************************************
 */

/// Set of callback functions for Telephone Bearer Server
typedef struct acc_tbs_cb
{
    /// Callback function called when Client Characteristic Configuration of a notification-capable
    /// characteristic has been updated by a peer client
    acc_tbs_cb_bond_data cb_bond_data;
    /// Callback function called when Input State characteristic value has been updated
    acc_tbs_cb_report_intv cb_report_intv;
    /// Callback function called to request from upper layer complete or piece of value for
    /// either Bearer Provider Name characteristic or Incoming Call Target Bearer URI characteristic
    /// or Call Friendly Name characteristic
    acc_tbs_cb_get_req cb_get_req;
    /// Callback function called to inform upper layer that a client device has requested an action
    /// for a call
    acc_tbs_cb_call_req cb_call_req;
} acc_tbs_cb_t;

/*
 * API FUNCTIONS DECLARATION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Configure use of Telephone Bearer Server module as Server
 *
 * @param[in] nb_tbs                 Number of instances of the Telephone Bearer Service
 *                                   0 means that only the Generic Telephone Bearer Service is
 *                                   supported
 * @param[in] call_pool_size         Size of pool containing pre-allocated structures used to manage
 *                                   Calls
 * @param[in] call_pool_uri_len      Maximum URI length allowed for pre-allocated Call structures
 * @param[in] uri_len_max            Maximum URI length for a Call. 0 means this is no limitation
 * @param[in] p_cb                   Pointer to set of callback functions for communication with upper
 *                                   layer
 * @param[in] pref_mtu               Preferred MTU
 *                                   Values from 0 to 63 are equivalent to 64
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_configure(uint8_t nb_tbs, uint8_t call_pool_size, uint8_t call_pool_uri_len,
                           uint8_t uri_len_max, const acc_tbs_cb_t* p_cb, uint16_t pref_mtu);

/**
 ****************************************************************************************
 * @brief Add and configure an instance of the Telephone Bearer Service in the database
 *
 * @param[in] cfg_bf                Configuration bit field
 * @param[in] nb_att_rsvd           Number of attributes reserved for the service
 * @param[in] ccid                  Content Control ID
 * @param[in] opt_opcodes_bf        Optional operation codes bit field
 * @param[in] bearer_uci_len        Length of Bearer UCI value
 * @param[in] p_bearer_uci          Pointer to Bearer UCI value
 * @param[in] p_bearer_lid          Pointer at which allocated Bearer local index is returned
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_add(uint8_t cfg_bf, uint8_t nb_att_rsvd, uint8_t ccid, uint8_t opt_opcodes_bf,
                     uint8_t bearer_uci_len, uint8_t* p_bearer_uci, uint8_t* p_bearer_lid);

/**
 ****************************************************************************************
 * @brief Enable use of Telephone Bearer Server module as Server
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_enable(void);

/**
 ****************************************************************************************
 * @brief Set bonding information for either the Generic Telephone Bearer Service or an instance
 * of the Telephone Bearer Service Service after connection with a Client device with which a
 * bonded relationship had been established during a previous connection.
 *
 * @param[in] bearer_lid               Bearer local index
 * @param[in] con_lid                  Connection local index
 * @param[in] sign_strength_intv_s     Signal strength reporting interval in seconds
 * @param[in] cli_cfg_bf               Client configuration bit field
 * @param[in] evt_cfg_bf               Event configuration bit field
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_restore_bond_data(uint8_t bearer_lid, uint8_t con_lid, uint8_t sign_strength_intv_s,
                          uint16_t cli_cfg_bf, uint16_t evt_cfg_bf);

/**
 ****************************************************************************************
 * @brief Set value of one of the following characteristics for a specific bearer.
 *
 * @param[in] bearer_lid               Bearer local index
 * @param[in] char_type                Characteristic type
 * @param[in] val                      Bearer technology
 *                                  or Signal Strength (from 0 to 100 or 255)
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_set(uint8_t bearer_lid, uint8_t char_type, uint8_t val);

/**
 ****************************************************************************************
 * @brief Enable or disable Silent Mode or Inband Ringtone for a Bearer.
 *
 * @param[in] bearer_lid               Bearer local index
 * @param[in] status_type              Status type
 * @param[in] val                      Value
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_set_status(uint8_t bearer_lid, uint8_t status_type, uint8_t val);

/**
 ****************************************************************************************
 * @brief Indicate an update of the Bearer Provider Name or the Bearer URI Scheme Supported
 * List characteristic
 *
 * @param[in] bearer_lid         Bearer local index
 * @param[in] char_type          Characteristic type
 * @param[in] len                Length of value
 * @param[in] name               Pointer to value
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_set_long(uint8_t bearer_lid, uint8_t char_type, uint8_t len, uint8_t* p_val);

/**
 ****************************************************************************************
 * @brief Add an incoming call.
 *
 * @param[in] bearer_lid        Bearer local index
 * @param[in] uri_len           Length of Incoming URI value
 * @param[in] tgt_uri_len       Length of Incoming Call Target URI value
 * Meaningful only if Incoming Call Target Bearer URI characteristic is supported
 * @param[in] friendly_name_len Length of Friendly Name value
 * Meaningful only if Call Friendly Name characteristic is supported
 * @param[in] p_uri             Pointer to Incoming URI value
 * @param[in] p_tgt_uri         Pointer to Incoming Call Target URI value
 * @param[in] p_friendly_name   Pointer to Friendly Name value
 * @param[out] p_call_id        Pointer at which allocated Call index is returned
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_call_incoming(uint8_t bearer_lid, uint8_t uri_len, uint8_t tgt_uri_len,
                               uint8_t friendly_name_len, uint8_t* p_uri, uint8_t* p_tgt_uri,
                               uint8_t* p_friendly_name, uint8_t* p_call_id);

/**
 ****************************************************************************************
 * @brief Add an outgoing call.
 *
 * @param[in] bearer_lid        Bearer local index
 * @param[in] uri_len           Length of Outgoing URI value
 * @param[in] friendly_name_len Length of Friendly Name value
 * Meaningful only if Call Friendly Name characteristic is supported
 * @param[in] p_uri             Pointer to Outgoing URI value
 * @param[in] p_friendly_name   Pointer to Friendly Name value
 * @param[out] p_call_id        Pointer at which allocated Call index is returned
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_call_outgoing(uint8_t bearer_lid, uint8_t uri_len,
                               uint8_t friendly_name_len, uint8_t* p_uri,
                               uint8_t* p_friendly_name, uint8_t* p_call_id);

/**
 ****************************************************************************************
 * @brief Update state of a call existing a Bearer.
 *
 * @param[in] bearer_lid        Bearer local index
 * @param[in] call_id           Call index
 * @param[in] action            Action
 * @param[in] reason            Termination reason
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_call_action(uint8_t bearer_lid, uint8_t call_id, uint8_t action, uint8_t reason);

/**
 ****************************************************************************************
 * @brief Join two or more calls currently existing on a Bearer.
 * Note that only the first provided Call index is returned in the response.
 *
 * @param[in] bearer_lid    Bearer local index
 * @param[in] nb_calls      Number of calls to join
 * @param[in] p_call_lids   Pointer to list of Call indexes of call to join
 *
 * @return An error status
 ****************************************************************************************
 */
uint16_t acc_tbs_call_join(uint8_t bearer_lid, uint8_t nb_calls, uint8_t* p_call_ids);

/**
 ****************************************************************************************
 * @brief Confirmation for ACC_TBS_GET request indication
 *
 * @param[in] status        Status
 * @param[in] bearer_lid    Bearer local index
 * @param[in] call_id       Call index
 * @param[in] con_lid       Connection local index
 * @param[in] char_type     Characteristic type
 * @param[in] token         Token
 * @param[in] offset        Offset
 * @param[in] length        Length
 * @param[in] p_val         Pointer to requested value
 *
 * @return An error status
 ****************************************************************************************
 */
void acc_tbs_cfm_get(uint16_t status, uint8_t bearer_lid, uint8_t call_id, uint8_t con_lid,
                     uint8_t char_type, uint16_t token, uint16_t offset, uint16_t length, uint8_t* p_val);

/**
 ****************************************************************************************
 * @brief Confirmation for ACC_TBS_CALL_OUTGOING or ACC_TBS_CALL_ACTION or
 * ACC_TBS_CALL_JOIN request indication.
 *
 * @param[in] status                Status
 * @param[in] bearer_lid            Bearer local index
 * @param[in] friendly_name_len     Length of friendly name value for Originate action
 * @param[in] p_friendly_name       Pointer to friendly name value
 *
 * @return An error status
 ****************************************************************************************
 */
void acc_tbs_cfm_call(uint16_t status, uint8_t bearer_lid,
                      uint8_t friendly_name_len, uint8_t* p_friendly_name);

#endif //(GAF_ACC_TBS)

/// @}

#endif // ACC_TBS_H_
