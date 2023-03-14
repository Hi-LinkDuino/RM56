/**
 ****************************************************************************************
 *
 * @file gatt_proc.h
 *
 * @brief Header file - GATT Procedure API for GATT client and server
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_PROC_H_
#define GATT_PROC_H_

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
#include "../inc/l2cap_att.h"   // Attribute L2CAP Protocol
#include "../inc/gap_hl_api.h"  // Simple Timer and Defer

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/// Macro used to generate procedure identifier
/// @param[in] id            Procedure identifier
/// @param[in] type          Procedure type
/// @param[in] pref_mtu      True if user preferred MTU is important for procedure, else false
#define GATT_PROC_ID(id, type, pref_mtu)  ((((id) << 3) & 0x78) | ((type) & 0x7) | (((pref_mtu) << 7) & 0x80))

/// Retrieve procedure type
#define GATT_PROC_TYPE_GET(proc_id)  (proc_id & 0x07)

/// Does procedure grant requires to check preferred user MTU
#define GATT_PROC_PREF_MTU_CHK(proc_id) ((proc_id & 0x80) >> 7)

/// All procedure (used for procedure grant)
#define GATT_PROC_ALL               (0x0F)

/*
 * DEFINES
 ****************************************************************************************
 */

/// Type of Attribute transaction used during procedure execution
enum gatt_proc_type
{
    /// Procedure that uses Attribute requests
    GATT_PROC_REQ               = 0x00,
    /// Procedure that uses Attribute commands
    GATT_PROC_CMD               = 0x01,
    /// Procedure that uses Attribute indication
    GATT_PROC_IND               = 0x02,
    /// Procedure that uses Attribute notification
    GATT_PROC_NTF               = 0x03,
    /// Procedure that handles a peer Attribute transaction (client side)
    GATT_PROC_CLI_HANDLER       = 0x04,
    /// Procedure that handles a peer Attribute transaction (server side)
    GATT_PROC_SRV_HANDLER       = 0x05,
};

/// Type of Attribute Procedure Identifier
enum gatt_proc_id
{
    /// Perform a MTU exchange
    GATT_PROC_MTU_EXCH          = GATT_PROC_ID(0,  GATT_PROC_REQ,         false),

    /// Perform an atomic attribute read
    GATT_PROC_READ              = GATT_PROC_ID(1,  GATT_PROC_REQ,         true),
    /// Perform a multiple attribute read
    GATT_PROC_READ_MULTIPLE     = GATT_PROC_ID(2,  GATT_PROC_REQ,         true),
    /// Perform a read according to a given UUID
    GATT_PROC_READ_BY_UUID      = GATT_PROC_ID(3,  GATT_PROC_REQ,         true),

    /// Perform a write using Attribute request transaction
    GATT_PROC_WRITE             = GATT_PROC_ID(4,  GATT_PROC_REQ,         false),
    /// Perform a write (long) using prepare write attribute request transaction
    GATT_PROC_WRITE_LONG        = GATT_PROC_ID(5,  GATT_PROC_REQ,         false),
    /// Perform a write using attribute command
    GATT_PROC_WRITE_NO_RESP     = GATT_PROC_ID(6,  GATT_PROC_CMD,         false),
    /// Perform a write Signed using attribute command
    GATT_PROC_WRITE_SIGNED      = GATT_PROC_ID(7,  GATT_PROC_CMD,         false),
    /// Perform a write Execute
    GATT_PROC_WRITE_EXE         = GATT_PROC_ID(8,  GATT_PROC_REQ,         false),

    /// Discover services
    GATT_PROC_DISCOVER_SVC      = GATT_PROC_ID(9,  GATT_PROC_REQ,         true),
    /// Discover included services
    GATT_PROC_DISCOVER_INC_SVC  = GATT_PROC_ID(10,  GATT_PROC_REQ,        true),
    /// Discover characteristics
    GATT_PROC_DISCOVER_CHAR     = GATT_PROC_ID(11, GATT_PROC_REQ,         true),
    /// Discover secondary services using UUID value
    GATT_PROC_DISCOVER_DESC     = GATT_PROC_ID(12, GATT_PROC_REQ,         true),
    /// Perform a full services discovery
    GATT_PROC_DISCOVER_FULL     = GATT_PROC_ID(13, GATT_PROC_REQ,         true),

    /// Send Notification event
    GATT_PROC_NOTIFY            = GATT_PROC_ID(0,  GATT_PROC_NTF,         false),
    /// Send Indication event
    GATT_PROC_INDICATE          = GATT_PROC_ID(1,  GATT_PROC_IND,         false),

    /// Handle Notification reception
    GATT_PROC_HANDLE_NTF        = GATT_PROC_ID(0,  GATT_PROC_CLI_HANDLER, false),
    /// Handle Multiple Notification reception
    GATT_PROC_HANDLE_NTF_MULT   = GATT_PROC_ID(1,  GATT_PROC_CLI_HANDLER, false),
    /// Handle Indication reception
    GATT_PROC_HANDLE_IND        = GATT_PROC_ID(2,  GATT_PROC_CLI_HANDLER, false),

    /// Handle MTU exchange request
    GATT_PROC_HANDLE_MTU_EXCH   = GATT_PROC_ID(0,  GATT_PROC_SRV_HANDLER, false),

    /// Handle write request or write command(s)
    GATT_PROC_HANDLE_WRITE      = GATT_PROC_ID(1,  GATT_PROC_SRV_HANDLER, false),
    /// Handle prepare write request
    GATT_PROC_HANDLE_PREP_WRITE = GATT_PROC_ID(2,  GATT_PROC_SRV_HANDLER, false),
    /// Handle execute write request
    GATT_PROC_HANDLE_EXE_WRITE  = GATT_PROC_ID(3,  GATT_PROC_SRV_HANDLER, false),

    /// Handle read request
    GATT_PROC_HANDLE_READ       = GATT_PROC_ID(4,  GATT_PROC_SRV_HANDLER, false),

    /// Handle discovery request
    GATT_PROC_HANDLE_DISCOVER   = GATT_PROC_ID(5,  GATT_PROC_SRV_HANDLER, false),
};

/// Procedure information bit field
enum gatt_proc_info_bf
{
    /// current procedure state (@see enum gatt_proc_state)
    GATT_PROC_STATE_LSB       = 0,
    GATT_PROC_STATE_MASK      = 0x0F,

    /// Procedure in continue function
    GATT_PROC_IN_CONTINUE_POS = 4,
    GATT_PROC_IN_CONTINUE_BIT = 0x10,

    /// False if procedure is waiting for user confirmation
    GATT_PROC_USER_CFM_POS    = 5,
    GATT_PROC_USER_CFM_BIT    = 0x20,

    /// True if procedure must be free after continue execution
    GATT_PROC_FREE_POS        = 6,
    GATT_PROC_FREE_BIT        = 0x40,
};

/// Procedure transition state
enum gatt_proc_state
{
    /// Procedure waiting to be granted
    GATT_PROC_WAIT_GRANT,
    /// Procedure wait to be started
    GATT_PROC_WAIT_START,
    /// Start procedure execution
    GATT_PROC_START,
    /// Attribute PDU transmitted to peer device
    GATT_PROC_PDU_PUSHED_TO_LL,
    /// Some attribute data has been received (and already handled by procedure if succeed)
    GATT_PROC_PDU_RX,
    /// A confirmation has been sent from GATT USER layers - wait it to be defer
    GATT_PROC_WAIT_USER_CFM,
    /// A confirmation has been sent from GATT USER layers
    GATT_PROC_USER_CFM,
    /// AES result properly received
    GATT_PROC_AES_RES,
    /// An error appears during procedure execution
    GATT_PROC_ERROR,
};


/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// definition of structure used in callback
struct gatt_proc;

/**
 ****************************************************************************************
 * @brief function called when operation state is updated
 *
 * @param[in] conidx     Connection index
 * @param[in] p_proc     Pointer to procedure to continue
 * @param[in] proc_state Operation transition state (@see enum gatt_proc_state)
 * @param[in] status     Execution status
 ****************************************************************************************
 */
typedef void(*gatt_proc_cb)(uint8_t conidx, struct gatt_proc* p_proc, uint8_t proc_state, uint16_t status);

/**
 ****************************************************************************************
 * @brief function called when a PDU is received and can be handled by a procedure
 *
 * @param[in] conidx     Connection index
 * @param[in] p_proc     Pointer to procedure under execution
 * @param[in] p_pdu      Pointer to unpacked PDU
 * @param[in] p_buf      Data of received but not yet extracted
 * @param[in] mtu        MTU size of the bearer
 *
 * @return PDU handling return status that will be provided to the procedure handler.
 ****************************************************************************************
 */
typedef uint16_t (*gatt_proc_pdu_handler_cb)(uint8_t conidx, struct gatt_proc* p_proc, l2cap_att_pdu_t* p_pdu,
                                             co_buf_t* p_buf, uint16_t mtu);

/// Structure used to retrieve information about User initiated procedure
typedef struct gatt_proc
{
    // -- User initiated procedure and handler procedure must have same header data.
    /// Operation are put into a dedicated queue
    co_list_hdr_t               hdr;
    /// Callback to continue function
    gatt_proc_cb                cb_continue;
    /// Timer used for transaction
    /// Defer used when GATT user confirmation received
    gapc_sdt_t                  defer_timer;

    /// Attribute bearer local identifier assigned for local procedure execution
    uint8_t                     bearer_lid;
    /// Identifier of procedure (@see enum gatt_proc_id)
    uint8_t                     proc_id;
    /// Procedure information bit field (@see enum gatt_proc_info_bf)
    uint8_t                     info_bf;
    /// User that requests procedure execution
    uint8_t                     user_lid;
    /// Token of used to authenticate a procedure
    /// This variable contains PDU transmit length before procedure grant to select a proper bearer
    uint16_t                    token;

    // -- User specific data starts here.
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in procedure complete.
    uint16_t                    dummy;
    /// MTU size used for transmission - that should be used as reference for PDU reception
    uint16_t                    tx_mtu;
    /// Transmission ATT Operation Code
    uint8_t                     tx_opcode;
    /// Handler of PDU parsing
    gatt_proc_pdu_handler_cb    cb_pdu_handler;
} gatt_proc_t;


/// Structure used to retrieve information about hander procedure
typedef struct gatt_proc_handler
{
    // -- Hander procedure and user initiated procedure must have same header data.
    /// Operation are put into a dedicated queue
    co_list_hdr_t               hdr;
    /// Callback to continue function
    gatt_proc_cb                cb_continue;
    /// Defer used when GATT user confirmation received
    gapc_sdt_t                  defer;

    /// Attribute bearer local identifier assigned for local procedure execution
    uint8_t                     bearer_lid;
    /// Identifier of procedure (@see enum gatt_proc_id)
    uint8_t                     proc_id;
    /// Procedure information bit field (@see enum gatt_proc_info_bf)
    uint8_t                     info_bf;
    /// User that requests procedure execution
    uint8_t                     user_lid;
    /// Token of used to authenticate a procedure
    uint16_t                    token;

    // -- Handler specific data starts here.
} gatt_proc_handler_t;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Create a new GATT User initiated procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  user_lid     GATT User Local identifier
 * @param[in]  role         GATT procedure role required (@see enum gatt_user_role)
 * @param[in]  dummy        Dummy parameter whose meaning is upper layer dependent and
 *                          which is returned in procedure complete.
 * @param[in]  proc_id      Procedure Identifier (@see enum gatt_proc_id)
 * @param[in]  tx_length    Data Size expected to be transmitted
 * @param[in]  proc_size    Size of procedure parameters
 * @param[in]  continue_cb  Continue operation execution
 * @param[out] pp_proc      Pointer to new procedure
 *
 * @return Status of operation creation
 ****************************************************************************************
 */
uint16_t gatt_proc_create(uint8_t conidx, uint8_t user_lid, uint16_t dummy, uint8_t proc_id, uint16_t tx_length,
                          uint16_t proc_size, gatt_proc_cb continue_cb,  gatt_proc_t** pp_proc);

/**
 ****************************************************************************************
 * @brief Allocate a new GATT Handler procedure
 *
 * @param[in]  conidx       Connection index
 * @param[in]  proc_id      Procedure Identifier (@see enum gatt_proc_id)
 * @param[in]  proc_size    Size of procedure parameters
 * @param[in]  continue_cb  Continue operation execution
 * @param[out] pp_proc      Pointer to new procedure
 *
 * @return Status of operation creation
 ****************************************************************************************
 */
uint16_t gatt_proc_handler_alloc(uint8_t conidx, uint8_t proc_id, uint16_t proc_size, gatt_proc_cb continue_cb,
                                 gatt_proc_handler_t** pp_proc);
/**
 ****************************************************************************************
 * @brief Push new procedure in waiting queue
 *        If procedure can be granted onto a bearer, its execution immediately starts
 *
 *        Waiting queue sorts procedure waiting to be granted according to user priority
 *
 * @param[in] conidx      Connection index
 * @param[in] p_proc      New operation to execute
 ****************************************************************************************
 */
void gatt_proc_push(uint8_t conidx, gatt_proc_t* p_proc);

/**
 ****************************************************************************************
 * @brief Stop procedure on top processing queue, clean-up it.
 *
 * @param[in]  conidx      Connection index
 * @param[in]  p_proc      Procedure to extract from waiting or execution queue
 * @param[in]  free        True to clean-up procedure, False else
 ****************************************************************************************
 */
void gatt_proc_pop(uint8_t conidx, gatt_proc_t* p_proc, bool free);

/**
 ****************************************************************************************
 * @brief Retrieve procedure from execution queue with a specific token value
 *
 * @param[in] conidx Connection index
 * @param[in] token  Token of the operation, 0 for operation request
 *
 * @return Procedure requested
 ****************************************************************************************
 */
gatt_proc_t* gatt_proc_pick(uint8_t conidx, uint16_t token);

/**
 ****************************************************************************************
 * @brief Find a procedure with a specific Dummy value.
 *
 * @param[in] conidx Connection index
 * @param[in] dummy  Dummy value of the operation
 *
 * @return Procedure found, NULL if nothing found
 ****************************************************************************************
 */
gatt_proc_t* gatt_proc_find(uint8_t conidx, uint16_t dummy);

/**
 ****************************************************************************************
 * @brief Ask procedure to send a PDU. No action on procedure should be performed until
 *        GATT_PROC_PDU_PUSHED_TO_LL procedure state is reached (@see enum gatt_proc_state)
 *
 * @param[in] conidx         Connection index
 * @param[in] p_proc         Pointer to the procedure under execution
 * @param[in] p_pdu          Pointer to the PDU in unpacked format
 * @param[in] p_data         Pointer to the data buffer which contains PDU payload information
 * @param[in] rsp_op_code    Expected response Attribute operation code
 * @param[in] cb_rsp_handler Callback executed when attribute response is received.
 *
 * @return Status code of execution request
 ****************************************************************************************
 */
uint16_t gatt_proc_pdu_send(uint8_t conidx, gatt_proc_t* p_proc, l2cap_att_pdu_t* p_pdu, co_buf_t* p_data,
                            gatt_proc_pdu_handler_cb cb_rsp_handler);

/**
 ****************************************************************************************
 * @brief Ask Procedure manager to update Procedure state. The procedure state update
 *        is performed in a background task
 *
 * @param[in] conidx   Connection index
 * @param[in] p_proc   Pointer to the procedure state to update
 * @param[in] state    New value of the procedure state (@see enum gatt_proc_state)
 * @param[in] status   Status code associated to the procedure transition state
 ****************************************************************************************
 */
void gatt_proc_continue(uint8_t conidx, gatt_proc_t* p_proc, uint8_t state, uint16_t status);

/**
 ****************************************************************************************
 * @brief Ask Procedure manager to continue reception on the bearer
 *
 * @param[in] conidx   Connection index
 * @param[in] p_proc   Pointer to the procedure
 ****************************************************************************************
 */
void gatt_proc_bearer_rx_continue(uint8_t conidx, gatt_proc_t* p_proc);

/**
 ****************************************************************************************
 * @brief Retrieve MTU available on the bearer allocated to the procedure
 *
 * @param[in] conidx   Connection index
 * @param[in] p_proc   Pointer to the procedure
 *
 * @return MTU size that can be used by procedure for attribute transactions
 ****************************************************************************************
 */
uint16_t gatt_proc_mtu_get(uint8_t conidx, gatt_proc_t* p_proc);

#if(BLE_GATT_CLI)
/**
 ****************************************************************************************
 * @brief Enable or Disable Write queue usage mutex
 *        If mutex is released, it automatically searches procedure that want to modify
 *        server write queue.
 *
 * @return   Pointer to the procedure
 ****************************************************************************************
 */
void gatt_proc_write_queue_mutex_set(uint8_t conidx, bool enable);
#endif // (BLE_GATT_CLI)

#if (BLE_HL_MSG_API)
/**
 ****************************************************************************************
 * @brief Store temporary the current active procedure
 *
 * @param[in] p_proc   Pointer to the procedure
 ****************************************************************************************
 */
void gatt_proc_cur_set(gatt_proc_t* p_proc);

/**
 ****************************************************************************************
 * @brief Retrieve pointer to current active procedure
 *
 * @return   Pointer to the procedure
 ****************************************************************************************
 */
gatt_proc_t* gatt_proc_cur_get(void);
#endif // (BLE_HL_MSG_API)

#endif // (BLE_GATT)
/// @} GATT

#endif // GATT_PROC_H_
