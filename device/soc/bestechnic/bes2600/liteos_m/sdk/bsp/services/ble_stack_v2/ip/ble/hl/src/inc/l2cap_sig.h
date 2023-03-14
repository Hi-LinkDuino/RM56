/**
 ****************************************************************************************
 *
 * @file l2cap_sig.h
 *
 * @brief Header file - L2CAP Signaling Internals
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_SIG_H_
#define L2CAP_SIG_H_

/**
 ****************************************************************************************
 * @addtogroup L2CAP
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"
#if (BLE_L2CAP)
#include "l2cap.h"
#include "co_list.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */
/// Number of L2CAP channel in negotiation
#define L2CAP_CHAN_NEGO_NB  (5)
/// Optional parameter length - Reject of CID
#define L2CAP_OPT_LEN    (2)

/// signaling codes
/*@TRACE*/
enum l2cap_sig_code
{
    /// Reserved code
    L2CAP_SIG_RESERVED_OPCODE              = 0x00,
    /// Reject request
    L2CAP_SIG_REJECT_OPCODE                = 0x01,
    /// Connection request
    L2CAP_SIG_CONNECTION_REQ_OPCODE        = 0x02,
    /// Connection response
    L2CAP_SIG_CONNECTION_RSP_OPCODE        = 0x03,
    /// Configuration request
    L2CAP_SIG_CONFIGURATION_REQ_OPCODE     = 0x04,
    /// Configuration response
    L2CAP_SIG_CONFIGURATION_RSP_OPCODE     = 0x05,
    /// Disconnection request
    L2CAP_SIG_DISCONNECT_REQ_OPCODE        = 0x06,
    /// Disconnection response
    L2CAP_SIG_DISCONNECT_RSP_OPCODE        = 0x07,
    /// Echo request
    L2CAP_SIG_ECHO_REQ_OPCODE              = 0x08,
    /// Echo response
    L2CAP_SIG_ECHO_RSP_OPCODE              = 0x09,
    /// Information request
    L2CAP_SIG_INFORMATION_REQ_OPCODE       = 0x0A,
    /// Information response
    L2CAP_SIG_INFORMATION_RSP_OPCODE       = 0x0B,
    /// Create channel request
    L2CAP_SIG_CREATE_CHANNEL_REQ_OPCODE    = 0x0C,
    /// Create channel response
    L2CAP_SIG_CREATE_CHANNEL_RSP_OPCODE    = 0x0D,
    /// Move channel request
    L2CAP_SIG_MOVE_CHANNEL_REQ_OPCODE      = 0x0E,
    /// Move channel response
    L2CAP_SIG_MOVE_CHANNEL_RSP_OPCODE      = 0x0F,
    /// Move channel confirmation
    L2CAP_SIG_MOVE_CHANNEL_CFM_OPCODE      = 0x10,
    /// Move channel confirmation response
    L2CAP_SIG_MOVE_CHANNEL_CFM_RSP_OPCODE  = 0x11,
    /// Connection Parameter Update Request
    L2CAP_SIG_CONN_PARAM_UPD_REQ_OPCODE    = 0x12,
    /// Connection Parameter Update Response
    L2CAP_SIG_CONN_PARAM_UPD_RSP_OPCODE    = 0x13,
    /// LE Credit Based Connection request
    L2CAP_SIG_LECB_CONNECT_REQ_OPCODE      = 0x14,
    /// LE Credit Based Connection response
    L2CAP_SIG_LECB_CONNECT_RSP_OPCODE      = 0x15,
    /// LE Flow Control Credit
    L2CAP_SIG_FLOW_CONTROL_CREDIT_OPCODE   = 0x16,
    /// L2CAP Credit Based Connection request
    L2CAP_SIG_CB_CONNECT_REQ_OPCODE        = 0x17,
    /// L2CAP Credit Based Connection response
    L2CAP_SIG_CB_CONNECT_RSP_OPCODE        = 0x18,
    /// L2CAP Credit Based Reconfigure request
    L2CAP_SIG_CB_RECONFIGURE_REQ_OPCODE    = 0x19,
    /// L2CAP Credit Based Reconfigure response
    L2CAP_SIG_CB_RECONFIGURE_RSP_OPCODE    = 0x1A,

    /// max number of signaling codes
    L2CAP_SIG_OPCODE_MAX,
};


/// signaling message length
enum l2cap_sig_msg_len
{
    /// Reserved code
    L2CAP_SIG_RESERVED_LEN                 = 0,
    /// Reject request
    L2CAP_SIG_REJECT_LEN                   = 2,
    /// Connection request
    L2CAP_SIG_CONNECTION_REQ_LEN           = 0,
    /// Connection response
    L2CAP_SIG_CONNECTION_RSP_LEN           = 0,
    /// Configuration request
    L2CAP_SIG_CONFIGURATION_REQ_LEN        = 0,
    /// Configuration response
    L2CAP_SIG_CONFIGURATION_RSP_LEN        = 0,
    /// Disconnection request
    L2CAP_SIG_DISCONNECT_REQ_LEN           = 4,
    /// Disconnection response
    L2CAP_SIG_DISCONNECT_RSP_LEN           = 4,
    /// Echo request
    L2CAP_SIG_ECHO_REQ_LEN                 = 0,
    /// Echo response
    L2CAP_SIG_ECHO_RSP_LEN                 = 0,
    /// Information request
    L2CAP_SIG_INFORMATION_REQ_LEN          = 0,
    /// Information response
    L2CAP_SIG_INFORMATION_RSP_LEN          = 0,
    /// Create channel request
    L2CAP_SIG_CREATE_CHANNEL_REQ_LEN       = 0,
    /// Create channel response
    L2CAP_SIG_CREATE_CHANNEL_RSP_LEN       = 0,
    /// Move channel request
    L2CAP_SIG_MOVE_CHANNEL_REQ_LEN         = 0,
    /// Move channel response
    L2CAP_SIG_MOVE_CHANNEL_RSP_LEN         = 0,
    /// Move channel confirmation
    L2CAP_SIG_MOVE_CHANNEL_CFM_LEN         = 0,
    /// Move channel confirmation response
    L2CAP_SIG_MOVE_CHANNEL_CFM_RSP_LEN     = 0,
    /// Connection Parameter Update Request
    L2CAP_SIG_CONN_PARAM_UPD_REQ_LEN       = 8,
    /// Connection Parameter Update Response
    L2CAP_SIG_CONN_PARAM_UPD_RSP_LEN       = 2,
    /// LE Credit Based Connection request
    L2CAP_SIG_LECB_CONNECT_REQ_LEN         = 10,
    /// LE Credit Based Connection response
    L2CAP_SIG_LECB_CONNECT_RSP_LEN         = 10,
    /// LE Flow Control Credit
    L2CAP_SIG_FLOW_CONTROL_CREDIT_LEN      = 4,
    /// L2CAP Credit Based Connection request
    L2CAP_SIG_CB_CONNECT_REQ_LEN           = 8,
    /// L2CAP Credit Based Connection response
    L2CAP_SIG_CB_CONNECT_RSP_LEN           = 8,
    /// L2CAP Credit Based Reconfigure request
    L2CAP_SIG_CB_RECONFIGURE_REQ_LEN       = 4,
    /// L2CAP Credit Based Reconfigure response
    L2CAP_SIG_CB_RECONFIGURE_RSP_LEN       = 2,
};

/// Procedure type supported
enum l2cap_sig_proc_type
{
    /// Request procedure
    L2CAP_SIG_PROC_REQ,
    /// Response procedure
    L2CAP_SIG_PROC_RSP,
};


/// list of procedures supported by signaling protocol
enum l2cap_sig_proc_id
{
    /// Connection parameter update
    L2CAP_SIG_PROC_CON_PARAM_UPDATE,
    /// Create new L2CAP COCs
    L2CAP_SIG_PROC_COC_CREATE,
    /// Handle Connect request from peer device
    L2CAP_SIG_PROC_COC_CONNECT,
    /// Terminate existing L2CAP COC
    L2CAP_SIG_PROC_COC_TERMINATE,
    /// Add new reception credit for a L2CAP COC
    L2CAP_SIG_PROC_COC_RX_CREDIT_ADD,
    /// Reconfigure MTU/MPS for some L2CAP COCs
    L2CAP_SIG_PROC_COC_RECONFIGURE,
    #if(RW_DEBUG)
    /// Reconfigure MTU/MPS for some L2CAP COCs
    L2CAP_SIG_PROC_COC_RECONFIGURE_DBG,
    #endif // (RW_DEBUG)
};

/// State of procedure (minimum set)
enum l2cap_sig_proc_state
{
    /// Procedure started for the first time
    L2CAP_SIG_PROC_START,
    /// An error is detected during procedure execution
    L2CAP_SIG_PROC_ERROR,
    /// A protocol response received
    L2CAP_SIG_PROC_RSP_RECEIVED,
    /// Response received from upper layer
    L2CAP_SIG_PROC_UPPER_LAYER_RSP_RECEIVED,
    /// Channel TX Flow Off propely performed
    L2CAP_SIG_PROC_TX_FLOW_OFF,
    /// Signaling PDU pushed onto lower layer buffers
    L2CAP_SIG_PROC_PDU_PUSHED_TO_LL,
};

///  command reject reasons in signaling protocol
enum l2cap_sig_reject_reason
{
    L2CAP_SIG_REJECT_CMD_NOT_UNDERSTOOD     = 0x0000,
    L2CAP_SIG_REJECT_MTU_SIG_EXCEEDED       = 0x0001,
    L2CAP_SIG_REJECT_INVALID_CID            = 0x0002,
};

/// result for connection parameter update response
enum l2cap_sig_con_up_rsp_code
{
    L2CAP_SIG_CON_PARAM_ACCEPT              = 0x0000,
    L2CAP_SIG_CON_PARAM_REJECT              = 0x0001,
};
/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Command Reject
/*@TRACE*/
typedef struct l2cap_sig_reject
{
    /// Signaling code - 0x01
    uint8_t  code;
    /// The Reason field describes why the Request packet was rejected
    /// - 0x0000 Command not understood
    /// - 0x0001 Signaling MTU exceeded
    /// - 0x0002 Invalid CID in request
    uint16_t reason;
} l2cap_sig_reject_t;

/// Disconnection Request
/*@TRACE*/
typedef struct l2cap_sig_disconnect_req
{
    /// Signaling code - 0x06
    uint8_t  code;
    /// Destination CID
    uint16_t dcid;
    /// Source CID
    uint16_t scid;
} l2cap_sig_disconnect_req_t;

/// Disconnection Response
/*@TRACE*/
typedef struct l2cap_sig_disconnect_rsp
{
    /// Signaling code - 0x07
    uint8_t  code;
    /// Destination CID
    uint16_t dcid;
    /// Source CID
    uint16_t scid;
} l2cap_sig_disconnect_rsp_t;

/// Connection Parameter Update Request
/*@TRACE*/
typedef struct l2cap_sig_conn_param_upd_req
{
    /// Signaling code - 0x12
    uint8_t  code;
    /// minimum value for the connection event interval
    uint16_t intv_min;
    /// maximum value for the connection event interval
    uint16_t intv_max;
    /// slave latency parameter
    uint16_t latency;
    /// connection timeout parameter
    uint16_t timeout;
} l2cap_sig_conn_param_upd_req_t;

/// Connection Parameter Update Response
/*@TRACE*/
typedef struct l2cap_sig_conn_param_upd_rsp
{
    /// Signaling code - 0x13
    uint8_t  code;
    /// Result field indicates the response to the Connection Parameter Update Request
    /// - 0x0000 Connection Parameters accepted
    /// - 0x0001 Connection Parameters rejected
    uint16_t response;
} l2cap_sig_conn_param_upd_rsp_t;

/// LE Credit based connection request
/*@TRACE*/
typedef struct l2cap_sig_lecb_connect_req
{
    /// Signaling code - 0x14
    uint8_t  code;
    /// Simplified Protocol/Service Multiplexer
    uint16_t spsm;
    /// Source CID
    uint16_t scid;
    /// Maximum Transmission Unit
    uint16_t mtu;
    /// Maximum PDU Size
    uint16_t mps;
    /// Initial credits
    uint16_t initial_credits;
} l2cap_sig_lecb_connect_req_t;

/// LE Credit based connection response
/*@TRACE*/
typedef struct l2cap_sig_lecb_connect_rsp
{
    /// Signaling code - 0x15
    uint8_t  code;
    /// Destination CID
    uint16_t dcid;
    /// Maximum Transmission Unit
    uint16_t mtu;
    /// Maximum PDU Size
    uint16_t mps;
    /// Initial credits
    uint16_t initial_credits;
    /// Result
    uint16_t result;
} l2cap_sig_lecb_connect_rsp_t;

/// LE Flow Control Credit
/*@TRACE*/
typedef struct l2cap_sig_flow_control_credit
{
    /// Signaling code - 0x16
    uint8_t  code;
    /// CID
    uint16_t cid;
    /// Credits
    uint16_t credits;
} l2cap_sig_flow_control_credit_t;


/// L2CAP Credit based connection request
/*@TRACE*/
typedef struct l2cap_sig_cb_connect_req
{
    /// Signaling code - 0x17
    uint8_t  code;
    /// Simplified Protocol/Service Multiplexer
    uint16_t spsm;
    /// Maximum Transmission Unit
    uint16_t mtu;
    /// Maximum PDU Size
    uint16_t mps;
    /// Initial credits
    uint16_t initial_credits;
    // Source CID [1 to 5]
} l2cap_sig_cb_connect_req_t;

/// L2CAP Credit based connection response
/*@TRACE*/
typedef struct l2cap_sig_cb_connect_rsp
{
    /// Signaling code - 0x18
    uint8_t  code;
    /// Maximum Transmission Unit
    uint16_t mtu;
    /// Maximum PDU Size
    uint16_t mps;
    /// Initial credits
    uint16_t initial_credits;
    /// Result
    uint16_t result;
    // Destination CID [1 to 5]
} l2cap_sig_cb_connect_rsp_t;


/// L2CAP Credit based connection reconfigure request
/*@TRACE*/
typedef struct l2cap_sig_cb_reconfigure_req
{
    /// Signaling code - 0x19
    uint8_t  code;
    /// Maximum Transmission Unit
    uint16_t mtu;
    /// Maximum PDU Size
    uint16_t mps;
    // Destination CID [1 to 5]
} l2cap_sig_cb_reconfigure_req_t;

/// L2CAP Credit based connection reconfigure response
/*@TRACE*/
typedef struct l2cap_sig_cb_reconfigure_rsp
{
    /// Signaling code - 0x1A
    uint8_t  code;
    /// Information about the success of the request.
    uint16_t result;
} l2cap_sig_cb_reconfigure_rsp_t;


/*@TRACE
 @trc_ref l2cap_cid, l2cap_sig_code
 */
union l2cap_sig_pdu
{
    /// L2Cap packet code.
    uint8_t                         code;

    /* Connection Signaling Packets */
    /// Command Reject
    //@trc_union code == L2CAP_SIG_REJECT_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_reject_t              reject;
    /// Connection Parameter Update Request
    //@trc_union code == L2CAP_SIG_CONN_PARAM_UPD_REQ_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_conn_param_upd_req_t  update_req;
    /// Connection Parameter Update Response
    //@trc_union code == L2CAP_SIG_CONN_PARAM_UPD_RSP_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_conn_param_upd_rsp_t  update_rsp;
    /// LE Credit based connection request
    //@trc_union code == L2CAP_SIG_LECB_CONNECT_REQ_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_lecb_connect_req_t    lecb_con_req;
    /// LE Credit based connection response
    //@trc_union code == L2CAP_SIG_LECB_CONNECT_RSP_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_lecb_connect_rsp_t    lecb_con_rsp;
    /// LE Flow Control Credit
    //@trc_union code == L2CAP_SIG_FLOW_CONTROL_CREDIT_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_flow_control_credit_t flow_ctl_credit;
    /// LE disconnection request
    //@trc_union code == L2CAP_SIG_DISCONNECT_REQ_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_disconnect_req_t      disc_req;
    /// LE disconnection response
    //@trc_union code == L2CAP_SIG_DISCONNECT_RSP_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_disconnect_rsp_t      disc_rsp;

    /// L2CAP Credit based connection request
    //@trc_union code == L2CAP_SIG_CB_CONNECT_REQ_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_cb_connect_req_t      cb_con_req;
    /// L2CAP Credit based connection response
    //@trc_union code == L2CAP_SIG_CB_CONNECT_RSP_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_cb_connect_rsp_t      cb_con_rsp;
    /// L2CAP Credit based connection reconfigure request
    //@trc_union code == L2CAP_SIG_CB_RECONFIGURE_REQ_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_cb_reconfigure_req_t  cb_reconfig_req;
    /// L2CAP Credit based connection reconfigure response
    //@trc_union code == L2CAP_SIG_CB_RECONFIGURE_RSP_OPCODE and parent.chan_id == L2CAP_CID_LE_SIGNALING
    l2cap_sig_cb_reconfigure_rsp_t  cb_reconfig_rsp;
};
typedef union l2cap_sig_pdu l2cap_sig_pdu_t;


/// definition of structure used in callback
struct l2cap_sig_proc;

/**
 ****************************************************************************************
 * @brief function called when procedure state is updated
 *
 * @param[in] conidx     Connection index
 * @param[in] p_proc     Pointer to procedure to continue
 * @param[in] proc_state Procedure transition state (@see enum l2cap_sig_proc_state)
 * @param[in] status     Execution status
 ****************************************************************************************
 */
typedef void (*l2cap_sig_proc_cb_t) (uint8_t conidx, struct l2cap_sig_proc* p_proc, uint8_t proc_state, uint16_t status);


/// Structure used to retrieve information about signaling procedure
typedef struct l2cap_sig_proc
{
    /// Procedures are put into a dedicated queue
    co_list_hdr_t        hdr;
    /// Callback to continue function
    l2cap_sig_proc_cb_t  cb_continue;
    /// Token of used for response handling - 0 for a request
    uint16_t             token;
    #if (BLE_HL_MSG_API)
    /// Destination task number if procedure created through message API
    uint16_t             dest_task_nbr;
    #endif // (BLE_HL_MSG_API)
    /// Procedure under execution (@see enum l2cap_sig_proc_id)
    uint8_t              proc_id;
    /// Packet identifier used for L2CAP transaction
    uint8_t              pkt_id;
} l2cap_sig_proc_t;



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
 * @brief Start the transaction timer
 *
 * @param[in] conidx Connection index
 ****************************************************************************************
 */
void l2cap_sig_trans_timer_start(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Stop the transaction timer
 *
 * @param[in] conidx Connection index
 ****************************************************************************************
 */
void l2cap_sig_trans_timer_stop(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Generate a new packet identifier for new SIG transaction
 *
 * @param[in] conidx Connection index
 *
 * @return Packet identifier
 ****************************************************************************************
 */
uint8_t l2cap_sig_pkt_id_get(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Send a SIG SDU
 *
 * @param[in] conidx  Connection index
 * @param[in] token   Token identifier used to retrieve procedure on-going to inform that PDU has been pushed to LL
 *                    0: unused
 * @param[in] pkt_id  Packet identifier
 * @param[in] p_pdu   PDU in unpacked format
 * @param[in] opt_len Number of optional parameters
 * @param[in] p_opt   Pointer to optional parameter array
 *
 * @return Status of the request (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_sig_pdu_send(uint8_t conidx, uint16_t token, uint8_t pkt_id, l2cap_sig_pdu_t* p_pdu,
                            uint8_t opt_len, const uint16_t* p_opt);


/**
 ****************************************************************************************
 * @brief Create a new L2CAP SIG procedure
 *
 * @param[in]  conidx        Connection index
 * @param[in]  proc_type     procedure Type (@see enum l2cap_sig_proc_type)
 * @param[in]  proc_id       Signaling procedure identifier (@see enum l2cap_sig_proc_id)
 * @param[in]  continue_cb   Continue procedure execution
 * @param[in]  proc_size     Size of procedure parameters
 * @param[out] pp_proc         Pointer to new procedure
 *
 * @return Status of procedure creation
 ****************************************************************************************
 */
uint16_t l2cap_sig_proc_create(uint8_t conidx, uint8_t proc_type, uint8_t proc_od, l2cap_sig_proc_cb_t continue_cb,
                             uint16_t proc_size, l2cap_sig_proc_t** pp_proc);

/**
 ****************************************************************************************
 * @brief Push a new procedure in processing queue
 *        For a Request procedure, if there is no procedure in queue, its execution starts
 *        For a response procedure, it can be immediately stated.
 *
 * @param[in] conidx Connection index
 * @param[in] p_proc   New procedure to execute
 ****************************************************************************************
 */
void l2cap_sig_proc_push(uint8_t conidx, l2cap_sig_proc_t* p_proc);

/**
 ****************************************************************************************
 * @brief Stop procedure on processing queue, clean-up it.
 *        if there is another procedure in queue, its execution starts
 *
 * @param[in] conidx Connection index
 * @param[in] p_proc   procedure to pop from request or response queue
 ****************************************************************************************
 */
void l2cap_sig_proc_pop(uint8_t conidx, l2cap_sig_proc_t* p_proc);

/**
 ****************************************************************************************
 * @brief Retrieve first procedure present in SIG procedure queue if token is zero, else
 *        procedure from response queue if token != 0
 *
 * @param[in] conidx Connection index
 * @param[in] token  Token of the procedure, 0 for procedure request
 *
 * @return procedure requested
 ****************************************************************************************
 */
l2cap_sig_proc_t* l2cap_sig_proc_pick(uint8_t conidx, uint16_t token);

/**
 ****************************************************************************************
 * @brief Requests Signaling to continue procedure execution
 *
 * @param[in] conidx   Connection index
 * @param[in] token  Token of the procedure, 0 for procedure request
 * @param[in] proc_state procedure transition state (@see enum l2cap_sig_proc_state)
 * @param[in] status   procedure status update
 ****************************************************************************************
 */
void l2cap_sig_proc_continue(uint8_t conidx, uint16_t token, uint8_t proc_state, uint16_t status);

/**
 ****************************************************************************************
 * @brief Send a L2CAP SIG Command Reject PDU
 *
 * @param conidx Connection index
 * @param pkt_id Command Packet identifier
 * @param reason Reject reason (@see enum l2cap_sig_reject_reason)
 * @param opt_1  First optional parameter (valid for L2CAP_SIG_REJECT_MTU_SIG_EXCEEDED
 *               and L2CAP_SIG_REJECT_INVALID_CID)
 * @param opt_2  Second optional parameter (valid for L2CAP_SIG_REJECT_INVALID_CID)
 *
 * @return Execution status code (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_sig_reject_send(uint8_t conidx, uint8_t pkt_id, uint16_t reason, uint16_t opt_1, uint16_t opt_2);

/**
 ****************************************************************************************
 * @brief Handle BLE link creation
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void l2cap_sig_create(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Handle BLE link disconnection
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void l2cap_sig_cleanup(uint8_t conidx);

/// @} L2CAP

#endif // (BLE_L2CAP)
#endif // L2CAP_SIG_H_
