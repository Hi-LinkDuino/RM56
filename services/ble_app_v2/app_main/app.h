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
#ifndef APP_H_
#define APP_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief Application entry point.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration
#include "app_ble_mode_switch.h"
#include "app_ble_param_config.h"
#include "co_bt_defines.h"

#ifdef  BLE_APP_PRESENT

#include <stdint.h>          // Standard Integer Definition
//#include <co_bt.h>           // Common BT Definitions
#include "arch.h"            // Platform Definitions
#include "gapc_msg.h"       // GAPC Definitions
#include "gapm_msg.h" 
#include "gap.h"

#if (NVDS_SUPPORT)
#include "nvds.h"
#endif // (NVDS_SUPPORT)

#if defined(CFG_APP_SEC)
#if defined(CFG_SEC_CON)
#define BLE_AUTHENTICATION_LEVEL        GAP_AUTH_REQ_SEC_CON_BOND
#else
#define BLE_AUTHENTICATION_LEVEL        GAP_AUTH_REQ_MITM_BOND
#endif
#else
#define BLE_AUTHENTICATION_LEVEL        GAP_AUTH_REQ_NO_MITM_NO_BOND
#endif
/*
 * DEFINES
 ****************************************************************************************
 */
/// Maximal length of the Device Name value
#define APP_DEVICE_NAME_MAX_LEN      (24)

// Advertising mode
#define APP_FAST_ADV_MODE   (1)
#define APP_SLOW_ADV_MODE   (2)
#define APP_STOP_ADV_MODE   (3)
#define APP_MAX_TX_OCTETS   251
#define APP_MAX_TX_TIME     2120

#define INVALID_BLE_ACTIVITY_INDEX  0xFF

#define BLE_CONN_PARAM_SLAVE_LATENCY_CNT        0
#define BLE_CONN_PARAM_SUPERVISE_TIMEOUT_MS     6000

/*
 * MACROS
 ****************************************************************************************
 */

#define APP_HANDLERS(subtask)    {&subtask##_msg_handler_list[0], ARRAY_LEN(subtask##_msg_handler_list)}

/*
 * ENUMERATIONS
 ****************************************************************************************
 */


/// Activity state machine
enum app_actv_state
{
    /// Activity does not exists
    APP_ACTV_STATE_IDLE = 0,

    /// Creating advertising activity
    APP_ADV_STATE_CREATING,
    /// Setting advertising data
    APP_ADV_STATE_SETTING_ADV_DATA,
    /// Setting scan response data
    APP_ADV_STATE_SETTING_SCAN_RSP_DATA,

    /// Starting advertising activity
    APP_ADV_STATE_STARTING,
    /// Advertising activity started
    APP_ADV_STATE_STARTED,
    /// Stopping advertising activity
    APP_ADV_STATE_STOPPING,
    /// Deleting advertising activity
    APP_ADV_STATE_DELETING,

    /// Creating scanning activity
    APP_SCAN_STATE_CREATING,        //8
    /// Starting scanning activity
    APP_SCAN_STATE_STARTING,
    /// Scanning activity started
    APP_SCAN_STATE_STARTED,
    /// Stopping scanning activity
    APP_SCAN_STATE_STOPPING,
    /// Deleting scanning activity
    APP_SCAN_STATE_DELETING,

    /// Creating connecting activity
    APP_CONNECT_STATE_CREATING,     //13
    /// Starting connecting activity
    APP_CONNECT_STATE_STARTING,
    /// Connecting activity started
    APP_CONNECT_STATE_STARTED,
    /// Stopping connecting activity
    APP_CONNECT_STATE_STOPPING,
    /// Deleting connecting activity
    APP_CONNECT_STATE_DELETING,

};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure containing information about the handlers for an application subtask
struct app_subtask_handlers
{
    /// Pointer to the message handler table
    const struct ke_msg_handler *p_msg_handler_tab;
    /// Number of messages handled
    uint16_t msg_cnt;
};

typedef enum
{
    BLE_CONN_PARAM_MODE_DEFAULT = 0,
    BLE_CONN_PARAM_MODE_AI_STREAM_ON,
    BLE_CONN_PARAM_MODE_A2DP_ON,
    BLE_CONN_PARAM_MODE_HFP_ON,
    BLE_CONN_PARAM_MODE_OTA,
    BLE_CONN_PARAM_MODE_OTA_SLOWER,
    BLE_CONN_PARAM_MODE_SNOOP_EXCHANGE,
    BLE_CONN_PARAM_MODE_BLE_AUDIO,
    BLE_CONN_PARAM_MODE_NUM,
} BLE_CONN_PARAM_MODE_E;

typedef enum
{
    BLE_CONN_PARAM_PRIORITY_NORMAL = 0,
    BLE_CONN_PARAM_PRIORITY_ABOVE_NORMAL0,
    BLE_CONN_PARAM_PRIORITY_ABOVE_NORMAL1,
    BLE_CONN_PARAM_PRIORITY_ABOVE_NORMAL2,
    BLE_CONN_PARAM_PRIORITY_HIGH,
} BLE_CONN_PARAM_PRIORITY_E;

typedef enum BLE_CONNECT_STATE {
    BLE_DISCONNECTED  = 0,
    BLE_DISCONNECTING = 1,
    BLE_CONNECTED     = 2,
}BLE_CONNECT_STATE_E;

typedef struct
{
    uint8_t     ble_conn_param_mode;
    uint8_t     priority;
    uint16_t    conn_param_interval;    // in the unit of 1.25ms
} BLE_CONN_PARAM_CONFIG_T;

/// Application environment structure
typedef struct {
    /// Connection handle
    uint16_t conhdl;
    BLE_CONNECT_STATE_E connectStatus;
    uint8_t isFeatureExchanged;
    /// Bonding status
    uint8_t bonded;
    uint8_t peerAddrType;
    uint8_t isBdAddrResolvingInProgress;
    uint8_t isGotSolvedBdAddr;
    uint8_t bdAddr[BD_ADDR_LEN];
    uint8_t solvedBdAddr[BD_ADDR_LEN];
    /// ble connection param record
    APP_BLE_CONN_PARAM_T connParam;
    /// ble connection pending param
    APP_BLE_CONN_PARAM_T connPendindParam;
    /// ble connection param update times
    uint8_t conn_param_update_times;
} APP_BLE_CONN_CONTEXT_T;

/// Application environment structure
struct app_env_tag
{
    uint8_t conn_cnt;

    enum app_actv_state state[BLE_ACTIVITY_MAX];

    /// Advertising activity index
    uint8_t adv_actv_idx[BLE_ADV_ACTIVITY_USER_NUM];
    /// Scanning activity index
    uint8_t scan_actv_idx;
    /// Connecting activity index
    uint8_t connect_actv_idx;

    bool need_restart_adv;
    bool need_update_adv;
    bool need_set_rsp_data;
    BLE_ADV_PARAM_T advParam;

    bool need_restart_scan;
    uint16_t scanIntervalInMs;
    uint16_t scanWindowInMs;
    uint32_t scanFiltPolicy;

    bool need_restart_connect;
    BLE_INIT_PARAM_T ble_init_param;

    bool need_set_white_list;
    uint8_t white_list_size;
    gap_bdaddr_t white_list_addr[8];

    /// Last initialized profile
    uint8_t next_svc;

    /// Device Name length
    uint8_t dev_name_len;
    /// Device Name
    uint8_t dev_name[APP_DEVICE_NAME_MAX_LEN];

    /// Local device IRK
    uint8_t loc_irk[KEY_LEN];

    APP_BLE_CONN_CONTEXT_T context[BLE_CONNECTION_MAX];

};

// max adv data length is 31, and 3 byte is used for adv type flag(0x01)
#define ADV_DATA_MAX_LEN                            (28)

/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */

/// Application environment
extern struct app_env_tag app_env;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize the BLE demo application.
 ****************************************************************************************
 */
void appm_init(void);

/**
 ****************************************************************************************
 * @brief Add a required service in the database
 ****************************************************************************************
 */
bool appm_add_svc(void);

/**
 ****************************************************************************************
 * @brief Add the bdaddr to white list
 ****************************************************************************************
 */
void appm_set_white_list(gap_bdaddr_t *bdaddr, uint8_t size);

/**
 ****************************************************************************************
 * @brief Put the device in general discoverable and connectable mode
 ****************************************************************************************
 */
void appm_start_advertising(void *param);

/**
 ****************************************************************************************
 * @brief
 ****************************************************************************************
 */
void appm_actv_fsm_next(uint8_t actv_idx, uint8_t status);

/**
 ****************************************************************************************
 * @brief Put the device in non discoverable and non connectable mode
 ****************************************************************************************
 */
void appm_stop_advertising(uint8_t actv_idx);

/**
 ****************************************************************************************
 * @brief Send to request to update the connection parameters
 ****************************************************************************************
 */
void appm_update_param(uint8_t conidx, struct gapc_conn_param *conn_param);

/**
 ****************************************************************************************
 * @brief Send a disconnection request
 ****************************************************************************************
 */
void appm_disconnect(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Retrieve device name
 *
 * @param[out] device name
 *
 * @return name length
 ****************************************************************************************
 */
uint8_t appm_get_dev_name(uint8_t* name);

uint8_t appm_is_connected();

/**
 ****************************************************************************************
 * @brief Return if the device is currently bonded
 ****************************************************************************************
 */
bool app_sec_get_bond_status(void);

/*---------------------------------------------------------------------------
 *            app_ble_connected_evt_handler
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    callback function of BLE connected event
 *
 * Parameters:
 *    conidx - connection index
 *    pPeerBdAddress - connected BLE device address
 *
 * Return:
 *    void
 */
void app_ble_connected_evt_handler(uint8_t conidx, gap_bdaddr_t *pPeerBdAddress);


/**
 ****************************************************************************************
 * @brief delete advertising
 *
 * @param[in] none
 ****************************************************************************************
 */

void appm_delete_activity(uint8_t actv_idx);

void appm_update_actv_state(uint8_t actv_idx, enum app_actv_state newState);

/*---------------------------------------------------------------------------
 *            app_ble_disconnected_evt_handler
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    ble disconnect event received callback
 *
 * Parameters:
 *    conidx - connection index
 *
 * Return:
 *    void
 */
void app_ble_disconnected_evt_handler(uint8_t conidx, uint8_t errCode);

void app_connecting_stopped(gap_bdaddr_t *peer_addr);

bool appm_check_adv_activity_index(uint8_t actv_idx);

void l2cap_update_param(uint8_t  conidx,
                        uint32_t min_interval_in_ms,
                        uint32_t max_interval_in_ms,
                        uint32_t supervision_timeout_in_ms,
                        uint8_t  slaveLatency);

void appm_start_connecting(BLE_INIT_PARAM_T *init_param);

void appm_stop_connecting(void);

void appm_start_scanning(uint16_t intervalInMs, uint16_t windowInMs, uint32_t filtPolicy);

void appm_stop_scanning(void);

void appm_create_advertising(void);

void appm_create_connecting(void);

void app_advertising_stopped(uint8_t actv_idx);

void app_advertising_starting_failed(uint8_t actv_idx, uint8_t err_code);

void app_scanning_stopped(void);

void app_scanning_starting_failed(uint8_t actv_idx, uint8_t err_code);

void app_connecting_failed(uint8_t actv_idx, uint8_t err_code);

void app_ble_update_param_failed(uint8_t conidx, uint8_t errCode);

void app_ble_update_param_successful(uint8_t conidx, APP_BLE_CONN_PARAM_T* pConnParam);

void appm_exchange_mtu(uint8_t conidx);

void app_ble_system_ready(void);

void appm_set_private_bd_addr(uint8_t* bdAddr);

void appm_add_dev_into_whitelist(struct gap_bdaddr* ptBdAddr);

void app_scanning_started(void);

void app_advertising_started(uint8_t actv_idx);

void app_connecting_started(void);

bool appm_resolve_random_ble_addr_from_nv(uint8_t conidx, uint8_t* randomAddr);

void appm_resolve_random_ble_addr_with_sepcific_irk(uint8_t conidx, uint8_t* randomAddr, uint8_t* pIrk);

void appm_random_ble_addr_solved(bool isSolvedSuccessfully, uint8_t* irkUsedForSolving);

uint8_t app_ble_get_actv_state(uint8_t actv_idx);

uint8_t app_ble_connection_count(void);

bool app_is_arrive_at_max_ble_connections(void);

bool app_is_resolving_ble_bd_addr(void);

void app_enter_fastpairing_mode(void);

bool app_is_in_fastpairing_mode(void);

void app_set_in_fastpairing_mode_flag(bool isEnabled);

uint16_t appm_get_conhdl_from_conidx(uint8_t conidx);

void appm_check_and_resolve_ble_address(uint8_t conidx);

uint8_t* appm_get_current_ble_addr(void);

void app_trigger_ble_service_discovery(uint8_t conidx, uint16_t shl, uint16_t ehl);

uint8_t* appm_get_local_identity_ble_addr(void);

void app_exchange_remote_feature(uint8_t conidx);

void app_ble_update_conn_param_mode_of_specific_connection(uint8_t conidx, BLE_CONN_PARAM_MODE_E mode, bool isEnable);

void app_ble_reset_conn_param_mode_of_specifc_connection(uint8_t conidx);

void app_ble_update_conn_param_mode(BLE_CONN_PARAM_MODE_E mode, bool isEnable);

void app_ble_reset_conn_param_mode(uint8_t conidx);

void appm_refresh_ble_irk(void);

bool app_ble_get_conn_param(uint8_t conidx,  APP_BLE_CONN_PARAM_T* pConnParam);

void appm_update_adv_data(void *param);

bool gattc_check_if_notification_processing_is_busy(uint8_t conidx);

#ifdef GFPS_ENABLED
void fp_update_ble_connect_param_start(uint8_t ble_conidx);
void fp_update_ble_connect_param_stop(uint8_t ble_conidx);
#endif

bool app_ble_is_connection_on_by_addr(uint8_t *addr);

void app_ble_set_white_list_complete(void);

uint8_t* app_ble_get_peer_addr(uint8_t conidx);

bool app_ble_is_connection_on_by_index(uint8_t conidx);

void app_init_ble_name(const char *name);

void app_ble_on_bond_success(uint8_t conidx);

void app_ble_on_encrypt_success(uint8_t conidx, uint8_t pairing_lvl);

#ifdef __cplusplus
}
#endif
/// @} APP

#endif //(BLE_APP_PRESENT)

#endif // APP_H_
