/**
 ****************************************************************************************
 *
 * @file gatt_int.h
 *
 * @brief Header file - GATT Internals
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_INT_H_
#define GATT_INT_H_

/**
 ****************************************************************************************
 * @addtogroup GATT
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_GATT)
#include "gatt.h"
#include "co_list.h"
#include "gatt_user.h"   // GATT User API
#include "gatt_proc.h"   // GATT Procedure API
#include "gatt_db.h"     // GATT Database API

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum duration of EATT L2CAP collision mitigation (in milliseconds)
#define GATT_COLLISON_MITIGATON_TIME_MIN    (100)

/// Invalid GATT bearer local identifier
#define GATT_INVALID_BEARER_LID             (0xFF)

/// Connection state bit field
enum gatt_con_state_bf
{
    /// MUTEX on client to not modify Server write queue in different parallel procedures
    GATT_CON_WRITE_QUEUE_MUTEX_BIT       = 0x01,
    GATT_CON_WRITE_QUEUE_MUTEX_POS       = 0,
    /// Bit used to know if legacy bearer is present
    GATT_CON_LEGACY_ATT_BEARER_OPEN_BIT  = 0x02,
    GATT_CON_LEGACY_ATT_BEARER_OPEN_POS  = 1,
    /// Bit used to know if MTU exchange initiated by local device has been performed on legacy ATT bearer
    GATT_CON_INI_MTU_EXCH_BIT            = 0x04,
    GATT_CON_INI_MTU_EXCH_POS            = 2,
    /// Bit used to know if MTU an MTU exchange response has been sent to peer device on legacy ATT bearer
    GATT_CON_RSP_MTU_EXCH_BIT            = 0x08,
    GATT_CON_RSP_MTU_EXCH_POS            = 3,
    /// Retry of EATT Establishment negotiation
    GATT_CON_EATT_ESTAB_RETRY_BIT        = 0x10,
    GATT_CON_EATT_ESTAB_RETRY_POS        = 4,
};
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Structure used to manage an ATT bearer
typedef struct gatt_bearer
{
    /// Queue of PDU reception (when reception queue is busy).
    co_list_t               rx_queue;
    /// Token that identifies the Client Request procedure on-going.
    uint16_t                req_proc_token;
    /// Token that identifies the Server Indication procedure on-going.
    uint16_t                ind_proc_token;
    /// MTU size of the GATT bearer
    uint16_t                mtu;
    /// Bearer Information bit field (@see enum gatt_bearer_info_bf)
    uint16_t                info_bf;
} gatt_bearer_t;

#if (BLE_GATT_CLI)
/// Structure used to describe a registered event.
typedef struct gatt_cli_event_reg
{
    /// Register start handle
    uint16_t start_hdl;
    /// Register end handle
    uint16_t end_hdl;
    /// GATT client user that receives event information
    uint8_t  user_lid;
} gatt_cli_event_reg_t;
#endif // (BLE_GATT_CLI)


/// Connection environment
typedef struct gatt_con_env
{
    /// List of available bearers
    gatt_bearer_t*          p_bearer[BLE_GATT_BEARER_PER_CON];
    /// Procedures under execution
    co_list_t               proc_exe_queue;
    /// Procedure waiting to be granted onto a Attribute bearer - sorted according to user priority
    co_list_t               proc_wait_queue;
    /// List of buffers in prepare write queue
    /// list is sorted per new prepare write handle received.
    co_list_t               prep_write_queue;
    /// L2CAP collision mitigation timer used to establish new EATT bearers on slave side
    gapc_sdt_t              eatt_mitigation_timer;
    #if (BLE_GATT_CLI)
    /// Pointer to array that contains registered events
    gatt_cli_event_reg_t*   p_reg_events;
    /// size of register event array
    uint8_t                 reg_event_size;
    /// Numbered of register events (by GATT user clients)
    uint8_t                 reg_event_nb;
    #endif // (BLE_GATT_CLI)
    /// cumulative size of data in prepare write queue
    uint16_t                prep_write_data_len;
    /// Bit field of active bearers
    uint16_t                bearer_bf;
    /// L2CAP collision mitigation time in milliseconds (according to connection interval and slave latency)
    uint16_t                eatt_mitigation_time;
    /// Connection status bit field (@see enum gatt_con_state_bf)
    uint8_t                 state_bf;
    /// Number of bearer reserved for creation
    uint8_t                 nb_bearer_rsvd;
    /// Latest bearer granted
    uint8_t                 last_grant_bearer_lid;
} gatt_con_env_t;

/// GATT Environment structure
typedef struct gatt_env_
{
    /// Array of GATT users - Index of in array represents user LID
    gatt_user_t             users[BLE_GATT_USER_NB];
    /// List of services present in attribute database.
    gatt_db_svc_t*          p_db;
    /// Array of connection environment pointer
    gatt_con_env_t*         p_con[BLE_CONNECTION_MAX];
    #if (BLE_HL_MSG_API)
    /// pointer to the active procedure (used by message API to retrieve hidden parameters)
    gatt_proc_t*            p_cur_proc;
    #endif // (BLE_HL_MSG_API)
    /// Total memory size used by GATT layer
    uint16_t                total_mem_size;
    /// Use to know if database hash is valid
    bool                    db_hash_valid;
    /// Database hash value
    uint8_t                 db_hash[GATT_DB_HASH_LEN];
} gatt_env_t;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// GATT Environment
extern gatt_env_t gatt_env;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Remove all services associated to specify user
 *
 * @param[in] user_lid   GATT User Local Index
 ****************************************************************************************
 */
void gatt_db_svc_remove_user(uint8_t user_lid);


/**
 ****************************************************************************************
 * @brief Handle BLE link creation
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void gatt_bearer_create(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Try to acquire a bearer in order to grant execution of an attribute procedure.
 *
 * @param[in] conidx     Connection Index
 * @param[in] bearer_lid Bearer Local identifier preferred for procedure grant
 * @param[in] p_proc     Pointer to the procedure
 * @param[in] pref_mtu   Preferred MTU that should be used for attribute transaction
 *
 * @return True if procedure has been granted onto a bearer, False if it should be put
 *         in wait queue.
 ****************************************************************************************
 */
bool gatt_bearer_acquire(uint8_t conidx, uint8_t bearer_lid, gatt_proc_t* p_proc, uint16_t pref_mtu);

/**
 ****************************************************************************************
 * @brief Release bearer assigned to procedure.
 *        This function should not be executing during an attribute transaction
 *        (Not between REQ->RSP or IND->CFM)
 *
 * @param[in] conidx    Connection Index
 * @param[in] p_proc    Pointer to the procedure
 *
 * @return True if procedure has been granted onto a bearer, False if it should be put
 *         in wait queue.
 ****************************************************************************************
 */
void gatt_bearer_release(uint8_t conidx, gatt_proc_t* p_proc);

/**
 ****************************************************************************************
 * @brief Close Bearer due to a specific reason. This will abort all on-going procedures
 *        If no bearer available to contine, procedure in wait queue will be also aborted.
 *
 * @param[in] conidx        Connection index
 * @param[in] bearer_lid    Bearer Local identifier
 * @param[in] reason        Bearer closing reason
 ****************************************************************************************
 */
void gatt_bearer_close(uint8_t conidx, uint8_t bearer_lid, uint16_t reason);

/**
 ****************************************************************************************
 * @brief Ask Bearer to send a PDU.
 *
 * @param[in] conidx        Connection index
 * @param[in] bearer_lid    Bearer Local identifier
 * @param[in] token         Token used to retrieve procedure identifier
 * @param[in] p_pdu         Pointer to the PDU in unpacked format
 * @param[in] p_data        Pointer to the data buffer which contains PDU payload information
 *
 * @return Status code of execution request
 ****************************************************************************************
 */
uint16_t gatt_bearer_pdu_send(uint8_t conidx, uint8_t bearer_lid, uint16_t token, l2cap_att_pdu_t* p_pdu,
                              co_buf_t* p_data);

/**
 ****************************************************************************************
 * @brief Retrieve bearer MTU information
 *
 * @param[in] conidx       Connection index
 * @param[in] bearer_lid   Bearer Local Identifier
 *
 * @return    MTU value for specific bearer
 ****************************************************************************************
 */
uint16_t gatt_bearer_mtu_get(uint8_t conidx, uint8_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Update MTU value
 *
 * @param[in] conidx       Connection index
 * @param[in] bearer_lid   Bearer Local Identifier
 * @param[in] mtu          New MTU value
 *
 * @return function execution status (@see enum hl_err)
 ****************************************************************************************
 */
void gatt_bearer_mtu_set(uint8_t conidx, uint8_t bearer_lid, uint16_t mtu);


/**
 ****************************************************************************************
 * @brief Mark that reception for the procedure can be resume
 *
 * @param[in] conidx       Connection index
 * @param[in] bearer_lid   Bearer Local Identifier
 ****************************************************************************************
 */
void gatt_bearer_rx_continue(uint8_t conidx, uint8_t bearer_lid);

/**
 ****************************************************************************************
 * @brief Check if MTU exchange is supported or not.
 *
 * @param[in]  conidx         Connection index
 * @param[uot] p_bearer_lid   Pointer to the legacy ATT bearer local identifier.
 *
 * @return True if supported, false else
 ****************************************************************************************
 */
bool gatt_bearer_mtu_exch_supported(uint8_t conidx, uint8_t* p_bearer_lid);

/**
 ****************************************************************************************
 * @brief When a bearer state changed, this function is used to check if a specific
 *        procedure execution can be granted
 *
 * @param[in] conidx     Connection index
 * @param[in] bearer_lid  Bearer Local Identifier available
 * @param[in] proc_type  Procedure type to grant (@see enum gatt_proc_type)
 *
 * @return Procedure requested
 ****************************************************************************************
 */
void gatt_proc_check_grant(uint8_t conidx, uint8_t bearer_lid, uint8_t proc_type);

/**
 ****************************************************************************************
 * @brief  Ask GATT Module to create an attribute procedure handler
 *
 * @param[in]  conidx       Connection index
 * @param[in]  op_code      Attribute operation code received
 * @param[in]  bearer_lid   Bearer local identifier
 * @param[out] p_pdu_hdl_cb Pointer to PDU Handler call-back
 * @param[out] pp_proc      Pointer to the procedure created, NULL if procedure not created
 *
 * @return Procedure creation status code (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t gatt_proc_handler_create(uint8_t conidx, uint8_t op_code, uint8_t bearer_lid,
                                  gatt_proc_pdu_handler_cb* p_pdu_hdl_cb, gatt_proc_handler_t** pp_proc);

/**
 ****************************************************************************************
 * @brief Check if user is active onto a GATT procedure
 *
 * @param[in] user_lid GATT user local index
 *
 * @return True if GATT user associated to one procedure, False else.
 ****************************************************************************************
 */
bool gatt_proc_is_user_active(uint8_t user_lid);

/**
 ****************************************************************************************
 * @brie Clean-up all procedure associated to a connection
 *
 * @param[in]  conidx       Connection index
 * @param[in]  reason       Clean-up reason
 ****************************************************************************************
 */
void gatt_proc_cleanup(uint8_t conidx, uint16_t reason);

/**
 ****************************************************************************************
 * @brie Clean-up the Prepare write queue
 *
 * @param[in]  conidx       Connection index
 ****************************************************************************************
 */
void gatt_srv_write_queue_cleanup(uint8_t conidx);

#if (BLE_GATT_CLI)

/**
 ****************************************************************************************
 * @brief Remove all client handle range registered to a specific user for event reception
 *
 * @param[in] user_lid GATT user local index
 ****************************************************************************************
 */
void gatt_cli_event_remove_user(uint8_t user_lid);
#endif // (BLE_GATT_CLI)

#endif // (BLE_GATT)

/// @} GATT

#endif // GATT_INT_H_
