/***************************************************************************
*
*Copyright 2015-2019 BES.
*All rights reserved. All unpublished rights reserved.
*
*No part of this work may be used or reproduced in any form or by any
*means, or stored in a database or retrieval system, without prior written
*permission of BES.
*
*Use of this work is governed by a license granted by BES.
*This work contains confidential and proprietary information of
*BES. which is protected by copyright, trade secret,
*trademark and other intellectual property rights.
*
****************************************************************************/

#ifndef __APP_BLE_MODE_SWITCH_H__
#define __APP_BLE_MODE_SWITCH_H__

#ifdef __cplusplus
extern "C" {
#endif

/*****************************header include********************************/
#include "bluetooth.h"
#include "app_ble_param_config.h"

/******************************macro defination*****************************/
#define BLE_ADV_DATA_STRUCT_HEADER_LEN (2)
#define BLE_ADV_TYPE_SHORTENED_NAME 0x08

#ifndef BLE_CONNECTION_MAX
#define BLE_CONNECTION_MAX (1)
#endif
#ifndef BLE_AUDIO_CONNECTION_CNT
#define BLE_AUDIO_CONNECTION_CNT (2)
#endif
#ifndef ADV_DATA_LEN
#define ADV_DATA_LEN                    0x1F
#endif
#ifndef BLE_ADV_FLAG_PART_LEN
#define BLE_ADV_FLAG_PART_LEN           0x03
#endif
#ifndef BLE_ADV_DATA_WITHOUT_FLAG_LEN
#define BLE_ADV_DATA_WITHOUT_FLAG_LEN   (ADV_DATA_LEN)
#endif
#ifndef BLE_ADV_DATA_WITH_FLAG_LEN
#define BLE_ADV_DATA_WITH_FLAG_LEN      (ADV_DATA_LEN - BLE_ADV_FLAG_PART_LEN)
#endif

#define WHITE_LIST_MAX_NUM 8

// the default interval is 160ms, note that for Bisto user case, to
// let GVA iOS version pop-out notification smoothly, the maximum interval should be this value
#define BLE_ADV_INVALID_INTERVAL (0)
#define BLE_ADVERTISING_INTERVAL (160)
#define BLE_FAST_ADVERTISING_INTERVAL (48)

#define BLE_ADV_SVC_FLAG  0x16
#define BLE_ADV_MANU_FLAG 0xFF

// Maximal length of the Device Name value
#define APP_DEVICE_NAME_MAX_LEN      (24)

#define ADV_PARTICLE_HEADER_LEN (1)

#define __ARRAY_EMPTY_F 1

#define BLE_ADV_INVALID_TXPWR (100)

/******************************type defination******************************/

enum BLE_ADV_SWITCH_USER_E {
    BLE_SWITCH_USER_RS          = 0, // used for role switch
    BLE_SWITCH_USER_BOX         = 1, // used for box open/close
    BLE_SWITCH_USER_AI          = 2, // used for ai
    BLE_SWITCH_USER_BT_CONNECT  = 3, // used for bt connect
    BLE_SWITCH_USER_SCO         = 4, // used for sco
    BLE_SWITCH_USER_IBRT        = 5, // used for ibrt
    BLE_SWITCH_USER_FPGA        = 6, // used for fpga
    BLE_SWITCH_USER_BLE_AUDIO   = 7, // used for ble audio

    BLE_SWITCH_USER_NUM,
};

typedef enum _BLE_WHITE_LIST_USER_E {
    BLE_WHITE_LIST_USER_TWS     = 0,    // used for aob_tws
    BLE_WHITE_LIST_USER_MOBILE,         // used for mobile manager

    BLE_WHITE_LIST_USER_NUM,
} BLE_WHITE_LIST_USER_E;

//Scan filter policy
enum BLE_SCAN_FILTER_POLICY {
    ///Allow advertising packets from anyone
    BLE_SCAN_ALLOW_ADV_ALL            = 0x00,
    ///Allow advertising packets from White List devices only
    BLE_SCAN_ALLOW_ADV_WLST,
    ///Allow advertising packets from anyone and Direct adv using RPA in InitA
    BLE_SCAN_ALLOW_ADV_ALL_AND_INIT_RPA,
    ///Allow advertising packets from White List devices only and Direct adv using RPA in InitA
    BLE_SCAN_ALLOW_ADV_WLST_AND_INIT_RPA,
};

/// Initiating Types
typedef enum app_ble_init_type
{
    /// Direct connection establishment, establish a connection with an indicated device
    APP_BLE_INIT_TYPE_DIRECT_CONN_EST = 0,
    /// Automatic connection establishment, establish a connection with all devices whose address is
    /// present in the white list
    APP_BLE_INIT_TYPE_AUTO_CONN_EST,
    /// Name discovery, Establish a connection with an indicated device in order to read content of its
    /// Device Name characteristic. Connection is closed once this operation is stopped.
    APP_BLE_INIT_TYPE_NAME_DISC,
} BLE_INIT_TYPE_E;

/* Advertise Types                                  */
/* ************************************************ */
typedef enum app_ble_adv_type
{
    /// Connectable and scannable undirected advertising
    ADV_TYPE_UNDIRECT = 0,
    /// Directed connectable with Low Duty Cycle
    ADV_TYPE_DIRECT_LDC,
    /// Directed connectable with High Duty Cycle
    ADV_TYPE_DIRECT_HDC,
    /// Non-connectable but Scannable undirected advertising
    ADV_TYPE_NON_CONN_SCAN,
    /// Non-connectable and nonscannable undirected
    ADV_TYPE_NON_CONN_NON_SCAN,
} BLE_ADV_TYPE_E;


#ifdef BLE_USE_RPA
#define BLE_DEFAULT_SCAN_POLICY BLE_SCAN_ALLOW_ADV_ALL_AND_INIT_RPA
#else
#define BLE_DEFAULT_SCAN_POLICY BLE_SCAN_ALLOW_ADV_ALL
#endif

/// Activity action
typedef enum 
{
    /// Activity do nothing
    BLE_ACTV_ACTION_IDLE = 0,

    /// Activity starting advertise
    BLE_ACTV_ACTION_STARTING_ADV,
    /// Activity stopping advertise
    BLE_ACTV_ACTION_STOPPING_ADV,

    /// Activity starting scan
    BLE_ACTV_ACTION_STARTING_SCAN,
    /// Activity stopping scan
    BLE_ACTV_ACTION_STOPPING_SCAN,

    /// Activity connecting
    BLE_ACTV_ACTION_CONNECTING,
    /// Activity stop connecting
    BLE_ACTV_ACTION_STOP_CONNECTING,
    /// Activity disconnecting
    BLE_ACTV_ACTION_DISCONNECTING,

    /// Activity connecting
    BLE_ACTV_ACTION_SET_WHITE_LIST,
} BLE_ACTV_ACTION_E;

typedef struct ble_bdaddr
{
    /// BD Address of device
    uint8_t addr[BTIF_BD_ADDR_SIZE];
    /// Address type of the device 0=public/1=private random
    uint8_t addr_type;
} ble_bdaddr_t;

typedef struct {
    BLE_ADV_ACTIVITY_USER_E adv_actv_user;
    bool withFlags;
    /// Advertising filtering policy (@see enum adv_filter_policy)
    uint8_t filter_pol;
    BLE_ADV_TYPE_E advType;
    uint16_t advInterval;
    uint16_t advUserInterval[BLE_ADV_USER_NUM];
    // Maximum power level
    BLE_ADV_TX_POWER_LEVEL_E advTxPwr;
    uint8_t advDataLen;
    uint8_t advData[ADV_DATA_LEN];
    uint8_t scanRspDataLen;
    uint8_t scanRspData[ADV_DATA_LEN];
    uint8_t localAddr[BTIF_BD_ADDR_SIZE];
    ble_bdaddr_t peerAddr;
}  __attribute__((__packed__)) BLE_ADV_PARAM_T;

typedef void (*BLE_DATA_FILL_FUNC_T)(void *advParam);
typedef void (*BLE_USER_DATA_FILL_FUNC_T)(BLE_ADV_USER_E, void *advParam);

typedef struct {
    uint8_t scanType;
    uint16_t scanWindow;
    uint16_t scanInterval;
} BLE_SCAN_PARAM_T;

typedef struct
{
    /// Initiating type (@see enum gapm_init_type)
    uint8_t gapm_init_type;
    /// Timeout for automatic connection establishment (in unit of 10ms). Cancel the procedure if not all
    /// indicated devices have been connected when the timeout occurs. 0 means there is no timeout
    uint16_t conn_to;
    /// Address of peer device in case white list is not used for connection
    ble_bdaddr_t peer_addr;
} __attribute__((__packed__)) BLE_INIT_PARAM_T;

typedef struct {
    bool pendingConnect;
    /// Initiating param
    BLE_INIT_PARAM_T init_param;
} __attribute__((__packed__)) BLE_INIT_INFO_T;

typedef struct
{
    ///Connection interval value
    uint16_t            con_interval;
    ///Connection latency value
    uint16_t            con_latency;
    ///Supervision timeout
    uint16_t            sup_to;
} APP_BLE_CONN_PARAM_T;

typedef struct {
    bool enable;
    /// Number of entries to be added in the list. 0 means that list content has to be cleared
    uint8_t size;
    /// BD Address of device
    ble_bdaddr_t bdaddr[WHITE_LIST_MAX_NUM];
} BLE_WHITE_LIST_PARAM_T;

typedef struct {
    bool ble_is_busy;
    BLE_ACTV_ACTION_E ble_actv_action;
    uint8_t ble_op_actv_idx;

    uint32_t advSwitch;         //one bit represent one user
    uint32_t adv_user_register; //one bit represent one user
    uint32_t adv_user_enable[BLE_ADV_ACTIVITY_USER_NUM];   //one bit represent one user

    BLE_DATA_FILL_FUNC_T bleDataFillFunc[BLE_ADV_USER_NUM];

    // param used for BLE adv
    bool adv_has_pending_op;
    bool withPendingFlag;
    uint16_t advPendingInterval;
    BLE_ADV_TYPE_E advPendingType;
    ble_bdaddr_t advPendingPeerAddr;
    int8_t advPendingTxpwr[BLE_ADV_ACTIVITY_USER_NUM];
    uint8_t advPendingLocalAddr[BLE_ADV_ACTIVITY_USER_NUM][BTIF_BD_ADDR_SIZE];
    int8_t advCurrentTxpwr[BLE_ADV_ACTIVITY_USER_NUM];
    BLE_ADV_PARAM_T advCurrentInfo[BLE_ADV_ACTIVITY_USER_NUM];
    BLE_ADV_PARAM_T advParamInfo;

    // prarm used for BLE scan
    bool scan_has_pending_op;
    bool scan_has_pending_stop_op;
    BLE_SCAN_PARAM_T scanCurrentInfo;
    BLE_SCAN_PARAM_T scanPendingInfo;

    // prarm used for BLE initiating
    bool pending_stop_connecting;
    BLE_INIT_INFO_T bleToConnectInfo[BLE_CONNECTION_MAX];
    bool pendingDisconnect[BLE_CONNECTION_MAX];

    // prarm used for set white list
    bool pending_set_white_list;
    BLE_WHITE_LIST_PARAM_T ble_white_list[BLE_WHITE_LIST_USER_NUM];

    // pointer of @see app_env
    struct app_env_tag *bleEnv;
} __attribute__((__packed__)) BLE_MODE_ENV_T;

typedef struct
{
    uint8_t length;
    uint8_t type;
    uint8_t value[__ARRAY_EMPTY_F];
} BLE_ADV_DATA_T;

/*---------------------------------------------------------------------------
 *            app_ble_mode_init
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    init the bleModeEnv
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void app_ble_mode_init(void);

/*---------------------------------------------------------------------------
 *            app_ble_register_data_fill_handler
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    register a BLE advertisement and scan response data fill handler for a
 *    specific user(see @BLE_ADV_USER_E), so that the adv/scan response data
 *    could present in BLE adv/scan response data
 *
 * Parameters:
 *    user - see the defination in BLE_ADV_USER_E
 *    func - adv/scan response data fill handler for specific user
 *
 * Return:
 *    void
 */
void app_ble_register_data_fill_handle(BLE_ADV_USER_E user, BLE_DATA_FILL_FUNC_T func, bool enable);

/*---------------------------------------------------------------------------
 *            app_ble_data_fill_enable
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    enable/disable specific user to fill the adv/scan response data
 *
 * Parameters:
 *    user : user to enable/disable fill data
 *    enable : true - enable user
 *             false - disable user
 *
 * Return:
 *    void
 */
void app_ble_data_fill_enable(BLE_ADV_USER_E user, bool enable);

/*---------------------------------------------------------------------------
 *            app_ble_start_connectable_adv
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    start connetable BLE advertise
 *
 * Parameters:
 *    advertisement interval in ms
 *
 * Return:
 *    None
 */
void app_ble_start_connectable_adv(uint16_t advInterval);

/*---------------------------------------------------------------------------
 *            app_ble_refresh_adv_state
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    refresh adv state
 *
 * Parameters:
 *    advertisement interval in ms
 *
 * Return:
 *    None
 */
void app_ble_refresh_adv_state(uint16_t advInterval);

/*---------------------------------------------------------------------------
 *            app_ble_set_adv_type
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    for ble to set adv type
 *
 * Parameters:
 *    advType: advertising type
 *    peer_addr: peer address
 *
 * Return:
 *    None
 */
void app_ble_set_adv_type(BLE_ADV_TYPE_E advType, ble_bdaddr_t *peer_addr);

/*---------------------------------------------------------------------------
 *            app_ble_set_adv_txpwr_by_actv_user
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    set adv tx power value by activity user
 *
 * Parameters:
 *    actv_user: activity user
 *    txpwr_dbm: the value of tx power, -21 -> 16
 *
 * Return:
 *    None
 */
void app_ble_set_adv_txpwr_by_actv_user(BLE_ADV_ACTIVITY_USER_E actv_user, int8_t txpwr_dbm);

/*---------------------------------------------------------------------------
 *            app_ble_set_adv_txpwr_by_adv_user
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    set adv tx power value by adv user
 *
 * Parameters:
 *    user: adv user
 *    txpwr_dbm: the value of tx power, -21 -> 16
 *
 * Return:
 *    None
 */
void app_ble_set_adv_txpwr_by_adv_user(BLE_ADV_USER_E user, int8_t txpwr_dbm);

/*---------------------------------------------------------------------------
 *            app_ble_set_all_adv_txpwr
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    set adv tx power value of all activity
 *
 * Parameters:
 *    txpwr_dbm: the value of tx power, -21 -> 16
 *
 * Return:
 *    None
 */
void app_ble_set_all_adv_txpwr(int8_t txpwr_dbm);

/*---------------------------------------------------------------------------
 *            app_ble_set_adv_local_addr_by_adv_user
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    for ble to set adv local address by advertising user
 *
 * Parameters:
 *    user: advertising user
 *    addr: the address that needed to set
 *
 * Return:
 *    None
 */
void app_ble_set_adv_local_addr_by_adv_user(BLE_ADV_USER_E user, uint8_t *addr);

/*---------------------------------------------------------------------------
 *            app_ble_set_adv_local_addr_by_actv_user
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    for ble to set adv local address by adv activity user
 *
 * Parameters:
 *    actv_user: adv activity user
 *    addr: the address that needed to set
 *
 * Return:
 *    None
 */
void app_ble_set_adv_local_addr_by_actv_user(BLE_ADV_ACTIVITY_USER_E actv_user, uint8_t *addr);

/*---------------------------------------------------------------------------
 *            app_ble_force_switch_adv
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    enable/disable all BLE adv request for specific UI user
 *
 * Parameters:
 *    user : UI user
 *    onOff : true - enable BLE adv for specific UI user
 *            false -  disable BLE adv for specific UI user
 *
 * Return:
 *    void
 */
void app_ble_force_switch_adv(enum BLE_ADV_SWITCH_USER_E user, bool onOff);

/*---------------------------------------------------------------------------
 *            app_ble_start_scan
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    start BLE scan
 *
 * Parameters:
 *    scanFilterPolicy : Scan filter policy
 *    scanWindow : BLE scan window size(in ms)
 *    scanWnInterval : BLE scan window interval(in ms)
 *
 * Return:
 *    void
 */
void app_ble_start_scan(enum BLE_SCAN_FILTER_POLICY scanFilterPolicy, uint16_t scanWindow, uint16_t scanWnterval);

/*---------------------------------------------------------------------------
 *            app_ble_stop_scan
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    stop BLE scan
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void app_ble_stop_scan(void);

/*---------------------------------------------------------------------------
 *            app_ble_start_connect
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    start connect BLE with the given address
 *
 * Parameters:
 *    bdAddrToConnect : BLE address to connnect
 *
 * Return:
 *    void
 */
void app_ble_start_connect(ble_bdaddr_t *addr);

/*---------------------------------------------------------------------------
 *            app_ble_start_connect_with_init_type
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    start connect BLE with the given address
 *
 * Parameters:
 *    bdAddrToConnect : BLE address to connnect
 *
 * Return:
 *    void
 */
void app_ble_start_connect_with_init_type(ble_bdaddr_t *addr, BLE_INIT_TYPE_E init_type, uint16_t conn_to);

/*---------------------------------------------------------------------------
 *            app_ble_cancel_connecting
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    cancel connecting as connecting cost too long time.
 *
 * Parameters:
 *    NULL
 *
 * Return:
 *    void
 */
void app_ble_cancel_connecting(void);

/*---------------------------------------------------------------------------
 *            app_ble_is_connection_on
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    is a specific BLE connection exist
 *
 * Parameters:
 *    index: Index of the BLE connection to check
 *
 * Return:
 *    true - BLE connection exists
 *    false - BLE connection doesn't exist
 */
bool app_ble_is_connection_on(uint8_t index);

/*---------------------------------------------------------------------------
 *            ble_execute_pending_op
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    execute pended BLE op
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void ble_execute_pending_op(BLE_ACTV_ACTION_E finish_action, uint8_t actv_idx);

/*---------------------------------------------------------------------------
 *            app_ble_is_any_connection_exist
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    is there any BLE connection exist
 *
 * Parameters:
 *    void
 *
 * Return:
 *    true - at least one BLE connection exist
 *    false - no BLE connection exists
 */
bool app_ble_is_any_connection_exist();

/*---------------------------------------------------------------------------
 *            app_ble_start_disconnect
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    disconnect the BLE connection with given connection index
 *
 * Parameters:
 *    conIdx: connection index to disconnect
 *
 * Return:
 *    void
 */
void app_ble_start_disconnect(uint8_t conIdx);

/*---------------------------------------------------------------------------
 *            app_ble_disconnect_all
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    disconnect all BLE connections
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void app_ble_disconnect_all(void);

/*---------------------------------------------------------------------------
 *            app_ble_is_in_advertising_state
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    is BLE in advertising progress
 *
 * Parameters:
 *    void
 *
 * Return:
 *    true - BLE adv is in progress
 *    false - BLE adv is not in progress
 */
bool app_ble_is_in_advertising_state(void);

/*---------------------------------------------------------------------------
 *            app_ble_get_user_register
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    to get adv user register
 *
 * Parameters:
 *    void
 *
 * Return:
 *    uint32_t -- adv user register
 */
uint32_t app_ble_get_user_register(void);

/*---------------------------------------------------------------------------
 *            app_ble_get_runtime_adv_param
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    to get current ble advertising parameters
 *
 * Parameters:
 *    pAdvType: Output pointer of adv type
 *    pAdvIntervalMs: Output pointer of adv internal in ms
 *
 * Return:
 *    void
 */
void app_ble_get_runtime_adv_param(uint8_t* pAdvType, uint16_t* pAdvIntervalMs);

/*---------------------------------------------------------------------------
 *            app_ble_refresh_irk
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    to refresh ble identity key
 *
 * Parameters:
 *    void
 *
 * Return:
 *    void
 */
void app_ble_refresh_irk(void);

/*---------------------------------------------------------------------------
 *            app_ble_get_conhdl
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get ble connection handle by ble connection index
 *
 * Parameters:
 *    conidx: ble connection index
 *
 * Return:
 *    uint16_t: ble connection handle
 */
uint16_t app_ble_get_conhdl(uint8_t conidx);

/*---------------------------------------------------------------------------
 *            app_ble_get_conhdl
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get ble device name
 *
 * Parameters:
 *    void
 *
 * Return:
 *    uint8_t *: ble device name
 */
uint8_t *app_ble_get_dev_name(void);

/*---------------------------------------------------------------------------
 *            app_ble_get_mode_env
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    get ble mode env
 *
 * Parameters:
 *    void
 *
 * Return:
 *    BLE_MODE_ENV_T *: ble mode env
 */
BLE_MODE_ENV_T *app_ble_get_mode_env(void);

/*---------------------------------------------------------------------------
 *            app_ble_update_param_clear
 *---------------------------------------------------------------------------
 *
 *Synopsis:
 *    clear ble update param record
 *
 * Parameters:
 *    conidx: connection index
 *
 * Return:
 *    void
 */
void app_ble_update_param_clear(uint8_t conidx);

uint32_t ble_get_manufacture_data_ptr(uint8_t *advData,uint32_t dataLength,uint8_t *manufactureData);
bool app_ble_is_white_list_enable(void);
void app_ble_set_white_list(BLE_WHITE_LIST_USER_E user, ble_bdaddr_t *bdaddr, uint8_t size);
void app_ble_clear_white_list(BLE_WHITE_LIST_USER_E user);
void app_ble_clear_all_white_list(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __APP_BLE_MODE_SWITCH_H__ */

