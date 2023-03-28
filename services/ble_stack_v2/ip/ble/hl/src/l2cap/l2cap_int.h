/**
 ****************************************************************************************
 *
 * @file l2cap_int.h
 *
 * @brief Header file - L2CAP Internals
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_INT_H_
#define L2CAP_INT_H_

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
#include "../inc/gap_hl_api.h"
#include "l2cap.h"
#include "co_list.h"
#include "co_djob.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/// Size of L2CAP Length field
#define L2CAP_LENGTH_LEN       (2)
/// Size of L2CAP CID field
#define L2CAP_CID_LEN          (2)

/// Size of L2CAP header
#define L2CAP_HEADER_LEN       (L2CAP_LENGTH_LEN + L2CAP_CID_LEN)
/// Size of SDU Length Field in first segment
#define L2CAP_SDU_LEN          (2)
/// only provide 5 buffers per transmission loops
#define L2CAP_ACL_BUF_PER_LOOP (5)


/// L2CAP Connection state Bit Field
enum  l2cap_state_bf
{
    /// Reception flow enabled
    L2CAP_RX_FLOW_ENABLE_BIT            = 0x01,
    L2CAP_RX_FLOW_ENABLE_POS            = 0,

    /// Segment transmission ongoing onto a channel
    L2CAP_TX_SEGMENT_ONGOING_BIT        = 0x02,
    L2CAP_TX_SEGMENT_ONGOING_POS        = 1,

    /// Peer device supports Enhanced L2CAP COC Creation
    L2CAP_COC_ENHANCED_SUPPORTED_BIT    = 0x04,
    L2CAP_COC_ENHANCED_SUPPORTED_POS    = 2,
};

/// LE Credit Based fields.
enum l2cc_chan_cid_type
{
    /// Local channel ID (used for reception)
    L2CAP_CHAN_CID_LOCAL,
    /// Peer channel ID (used for transmission)
    L2CAP_CHAN_CID_PEER,
};

/// L2CAP channel configuration extension Bit Field (@see enum  l2cap_chan_cfg_bf)
enum  l2cap_chan_cfg_bf_cont
{
    /// Fixed L2CAP channel; else dynamic  (@note cannot be reconfigured)
    L2CAP_CHAN_CONFIG_INFO_MASK     = 0x07,
    L2CAP_CHAN_CONFIG_INFO_LSB      = 0,

    #if (BLE_HL_MSG_API)
    /// Debug Mpde enabled onto a channel (valid only for Message API and Dynamic channel)
    L2CAP_CHAN_DBG_MODE_EN_BIT      = 0x40,
    L2CAP_CHAN_DBG_MODE_EN_POS      = 6,
    #endif  // (BLE_HL_MSG_API)

    /// Fixed L2CAP channel; else dynamic  (@note cannot be reconfigured)
    L2CAP_CHAN_FIX_BIT              = 0x80,
    L2CAP_CHAN_FIX_POS              = 7,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Channel environment (for a dynamically allocated channel)
typedef struct l2cap_chan
{
    /// Callback of function handled by an upper layer API for registered channel
    const l2cap_chan_coc_cb_t*  p_cb;
    /// Buffer transmission queue
    co_list_t                   tx_queue;
    /// Reception buffer
    co_buf_t*                   p_rx_sdu;
    #if (BLE_HL_MSG_API)
    /// Destination task number if channel registered through message API
    uint16_t                    dest_task_nbr;
    #endif // (BLE_HL_MSG_API)
    /// Configuration bit field (@see enum l2cap_chan_cfg_bf)
    uint16_t                    config_bf;
    /// Reception L2CAP Channel Identifier
    uint16_t                    rx_cid;
    /// Transmission L2CAP Channel Identifier
    uint16_t                    tx_cid;
    ///Reception Maximum Transmit Unit Size
    uint16_t                    rx_mtu;
    /// Transmission Maximum Transmit Unit Size
    uint16_t                    tx_mtu;
    /// Reception Maximum Packet Size.
    uint16_t                    rx_mps;
    /// Transmission Maximum Packet Size.
    uint16_t                    tx_mps;
    /// Reception credit number.
    uint16_t                    rx_credit;
    /// Maximum number of reception credits
    uint16_t                    rx_credit_max;
    /// Transmission reception credit number.
    uint16_t                    tx_credit;
} l2cap_chan_t;


/// Connection Oriented channel specific environment
typedef struct l2cap_sig_env
{
    /// Request procedure queue (initiated by local device)
    co_list_t         req_proc_queue;
    /// Response procedure Queue (initiated by peer device).
    co_list_t         rsp_proc_queue;
    /// Transaction timer
    gapc_sdt_t        trans_timer;
    /// Channel Local Identifier for Signaling L2CAP
    uint8_t           chan_lid;
    /// Packet identifier counter
    uint8_t           pkt_id_counter;
    /// Signaling protocol state (@see enum l2cap_sig_state_bf)
    uint8_t           state_bf;
} l2cap_sig_env_t;

/// Connection environment
typedef struct l2cap_con_env
{
    /// Connection Oriented channel specific environment
    l2cap_sig_env_t   sig;
    #if (BLE_HL_MSG_API)
    /// List of SDU received but waiting for Upper layer usage confirmation
    /// this is handled by buffer release in native API
    co_list_t         msg_api_sdu_queue;
    #endif // (BLE_HL_MSG_API)
    /// List of received ACL packet not yet processed (when connection RX flow is off or paused due to lake of buffers)
    co_list_t         rx_queue;
    /// Bit field of channel ready to transmit SDU
    uint32_t          tx_channel_bf;
    /// Channel index under transmission
    uint8_t           tx_chan_lid;
    /// Channel index under reception
    uint8_t           rx_chan_lid;
    /// Temporary Expected RX size length
    uint8_t           rx_temp_exp_len;
    /// Temporary RX data cursor
    uint8_t           rx_temp_cursor;
    /// Temporary buffer used to receive L2Cap Header + SDU Len
    uint8_t           rx_temp_buf[L2CAP_HEADER_LEN + L2CAP_SDU_LEN];
    /// Connection bit field used to know status of the connection. (@see enum l2cap_state_bf)
    uint8_t           state_bf;
    /// Number of ACL buffer waiting to be transmitted.
    uint8_t           nb_tx_pending;
    ///  Number of L2Cap channel (max 32)
    uint8_t           nb_channel;
    /// channel Array of channel environment pointer
    l2cap_chan_t*     p_chan[BLE_L2CAP_CHAN_PER_CON];
} l2cap_con_env_t;

/// SPSM Information
typedef struct l2cap_spsm
{
    /// List Header
    co_list_hdr_t               hdr;
    /// Callback that handle events onto a registered SPSM
    const l2cap_coc_spsm_cb_t* p_cb;
    #if (BLE_HL_MSG_API)
    /// Destination task number if SPSM register through message API
    uint16_t                    dest_task_nbr;
    #endif // (BLE_HL_MSG_API)
    /// Simplified Protocol/Service Multiplexer value
    uint16_t                    spsm;
    /// Security level bit field (@see enum l2cap_sec_lvl_bf)
    uint8_t                     sec_lvl_bf;
} l2cap_spsm_t;

/// L2CAP Environment structure
typedef struct l2cap_env_
{
    /// L2CAP PDU Transmission background job
    co_djob_t         tx_bg_job;
    /// L2CAP PDU Reception background job
    co_djob_t         rx_bg_job;
    /// List of Registered SPSM
    co_list_t         reg_spsm;
    /// Array of connection environment pointer
    l2cap_con_env_t*  p_con[BLE_CONNECTION_MAX];
    /// Bit field of connection ready to transmit
    uint32_t          tx_con_bf;
    /// Bit field of connection which are on reception flow paused
    uint32_t          rx_flow_paused_bf;

    #if (BLE_HL_MSG_API)
    /// Destination task number of event - only valid for Message API usage
    uint16_t          dest_task_nbr;
    #endif // (BLE_HL_MSG_API)

    /// Lower Layer buffers maximum size
    uint16_t          ll_buf_size;
    /// Total number of available Lower Layer buffers
    uint16_t          ll_buf_nb_total;
    /// Number of available Lower Layer buffers
    uint16_t          ll_buf_nb_avail;
    //// Last Connection index used for transmission
    uint8_t           last_tx_con_idx;
    /// Number of buffer available for host flow control - Flow control mechanism
    uint8_t           hl_buf_nb_avail;
} l2cap_env_t;

/// Structure to manage received ACL data
typedef struct l2cap_acl_rx
{
    /// List header to put it in a queue
    co_list_hdr_t hdr;
    /// Buffer pointer
    uint32_t      buf_ptr;
    /// Connection index
    uint8_t       conidx;
    /// HCI PB FLAG
    uint8_t       pb_flag;
    /// Buffer length
    uint16_t      length;
    /// Cursor position into the buffer
    uint16_t      cursor;
} l2cap_acl_rx_t;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// L2CAP Environment
extern l2cap_env_t l2cap_env;

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Internal function used to register a dynamic L2CAP channel.
 *        This channel supports credit management, segmentation and reassembly mechanisms.
 *        To create a L2CAP Credit Based connection, @see l2cap_coc_create command should
 *        be used instead.
 *
 * @param[in]  conidx       Connection Index
 * @param[out] p_chan_lid   Pointer to L2CAP Channel local index
 * @param[out] pp_chan      Pointer to the allocated channel
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_dyn_register(uint8_t conidx, uint8_t* p_chan_lid, l2cap_chan_t** pp_chan);

/**
 ****************************************************************************************
 * @brief Internal function used to increment number of dynamic L2CAP channel transmission credits.
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  chan_lid     L2CAP Channel local index
 * @param[in]  credit       Number of credit to add for SDU transmission
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_tx_credit_add(uint8_t conidx, uint8_t chan_lid, uint16_t credit);


/**
 ****************************************************************************************
 * @brief Retrieve Channel information
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 *
 * @return Found channel information structure; NULL if not found
 ****************************************************************************************
 */
l2cap_chan_t* l2cap_chan_get(uint8_t conidx, uint8_t chan_lid);

/**
 ****************************************************************************************
 * @brief Find a L2CAP channel using TX or RX Channel Identifier
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  cid_type     Channel identifier typer (@see enum l2cc_chan_cid_type)
 * @param[in]  cid          L2CAP Channel identifier
 * @param[out] pp_chan      Pointer where channel environment will be set
 *
 * @return Corresponding local channel identifier
 ****************************************************************************************
 */
uint8_t l2cap_chan_find(uint8_t conidx, uint8_t cid_type, uint16_t cid, l2cap_chan_t** pp_chan);


/**
 ****************************************************************************************
 * @brief Enable or disable usage of the channel
 *        Disabling the channel force a buffer clean-up of transmission and reception
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] enable        True to enable the channel, False to disable it
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_enable_set(uint8_t conidx, uint8_t chan_lid, uint8_t enable);

/**
 ****************************************************************************************
 * @brief Control TX Flow: Accept transmission of new L2CAP Segments
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] token         Procedure token expecting to be informed when TX Flow is off.
 * @param[in] on            True: resume Segment transmission, False: Pause Segment transmission
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_chan_tx_flow_set(uint8_t conidx, uint8_t chan_lid, uint16_t token, uint8_t on);

/**
 ****************************************************************************************
 * @brief Increment number of reception credit.
 *        It ensure that number of credit added does not exceeds maximum number of credits
 *        allocated for connection
 *
 * @param[in]  conidx       Connection Index
 * @param[in]  chan_lid     L2CAP Channel local index
 * @param[in]  credit       Number of credit to add in reception
 ****************************************************************************************
 */
void l2cap_chan_rx_credit_add(uint8_t conidx, uint8_t chan_lid, uint16_t credit);

/**
 ****************************************************************************************
 * @brief Update number of ACL LL buffer released
 *
 * @param[in] conidx        Connection Index
 * @param[in] nb_buf        Number of buffer released
 ****************************************************************************************
 */
void l2cap_chan_ll_buf_release(uint8_t conidx, uint16_t nb_buf);

/**
 ****************************************************************************************
 * @brief handle reception of ACL packets for the connection in the reception queue
 *
 * @param[in] conidx        Connection Index
 ****************************************************************************************
 */
void l2cap_chan_ll_rx_acl(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Background function used to continue transmission of ACL data
 *
 * @param[in] p_env Unused
 ****************************************************************************************
 */
void l2cap_chan_tx_handler(void* p_env);

/**
 ****************************************************************************************
 * @brief Background function used to continue reception of ACL data
 *
 * @param[in] p_env Unused
 ****************************************************************************************
 */
void l2cap_chan_rx_handler(void* p_env);

/**
 ****************************************************************************************
 * @brief Connection cleanup fonction of active channels
 *
 * @param[in] conidx Connection index
 ****************************************************************************************
 */
void l2cap_chan_cleanup(uint8_t conidx);

/**
 ****************************************************************************************
 * @brief Retrieve Registered SPSM information
 *
 * @param[in] spsm     Simplified Protocol/Service Multiplexer
 *
 * @return Found SPSM information structure; NULL if not found
 ****************************************************************************************
 */
l2cap_spsm_t* l2cap_coc_spsm_get(uint16_t spsm);

/**
 ****************************************************************************************
 * @brief Increment number of channel for the L2CAP COC
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] credits       Number of credit to increment
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
uint16_t l2cap_coc_rx_credit_add(uint8_t conidx, uint8_t chan_lid, uint16_t credits);


/**
 ****************************************************************************************
 * @brief A L2CAP COC error has been detected, Disconnect the Link
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] status        Disconnection reason
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
void l2cap_coc_error_detected(uint8_t conidx, uint8_t chan_lid, uint16_t status);

/**
 ****************************************************************************************
 * @brief Inform that TX flow for the channel has been disabled
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] token         Token of the procedure that requests flow off
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
void l2cap_coc_tx_flow_off(uint8_t conidx, uint8_t chan_lid, uint16_t token);

#if (RW_DEBUG)
/**
 ****************************************************************************************
 * @brief Debug Command use to reconfigure a L2CAP connection oriented channel RX MTU and MPS
 *
 * @param[in] conidx        Connection Index
 * @param[in] dummy         Dummy parameter provided by upper layer for command execution
 * @param[in] local_rx_mtu  New Local reception Maximum Transmit Unit Size
 * @param[in] local_rx_mps  New Local reception Maximum Packet Size
 * @param[in] nb_chan       Number of L2CAP Channel local index in provided array
 * @param[in] p_chan_lid    Pointer to an array of L2CAP Channel local index
 *
 *
 * @return Status of the function execution (@see enum hl_err)
 *         Consider status only if an error occurs; else wait for execution completion
 ****************************************************************************************
 */
uint16_t l2cap_coc_dbg_reconfigure(uint8_t conidx, uint16_t dummy, uint16_t local_rx_mtu, uint16_t local_rx_mps,
                                   uint8_t nb_chan, uint8_t* p_chan_lid);

#endif // (RW_DEBUG)
#if (BLE_HL_MSG_API && RW_DEBUG)
/**
 ****************************************************************************************
 * @brief Inform Upper layer that number of credit for the L2CAP COC has been incremented
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] credits       Number of credit to increment
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
void l2cap_msg_coc_rx_credit_add(uint8_t conidx, uint8_t chan_lid, uint16_t credits);

/**
 ****************************************************************************************
 * @brief Inform Upper layer that L2CAP COC error has been detected
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 * @param[in] status        Disconnection reason
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
void l2cap_msg_coc_error_detected(uint8_t conidx, uint8_t chan_lid, uint16_t status);

/**
 ****************************************************************************************
 * @brief Inform Upper layer that L2CAP Channel TX Flow has been paused
 *
 * @param[in] conidx        Connection Index
 * @param[in] chan_lid      L2CAP Channel local index
 *
 * @return Status of the function execution (@see enum hl_err)
 ****************************************************************************************
 */
void l2cap_msg_coc_tx_flow_off(uint8_t conidx, uint8_t chan_lid);
#endif  // (BLE_HL_MSG_API && RW_DEBUG)

#endif // (BLE_L2CAP)

/// @} L2CAP

#endif // L2CAP_INT_H_
