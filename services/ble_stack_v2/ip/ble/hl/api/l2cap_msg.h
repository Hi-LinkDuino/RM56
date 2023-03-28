/**
 ****************************************************************************************
 *
 * @file l2cap_msg.h
 *
 * @brief Header file - L2CAP Message API.
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_MSG_H_
#define L2CAP_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup L2CAP_MSG Message API
 * @ingroup L2CAP
 * @brief Handles ALL messages to/from L2CAP block.
 *
 * The L2CAP task is responsible for SDU and PDU exchange over a BLE link.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_task.h" // Task definitions
#include "compiler.h"

#include <stdbool.h>

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * MESSAGES
 ****************************************************************************************
 */

/// Message API of the L2CAP task
/*@TRACE*/
enum l2cap_msg_id
{
    /// L2CAP command
    L2CAP_CMD                = MSG_ID(L2CAP, 0x00),
    /// L2CAP command complete event
    L2CAP_CMP_EVT            = MSG_ID(L2CAP, 0x01),
    /// L2CAP Indication
    L2CAP_IND                = MSG_ID(L2CAP, 0x02),
    /// L2CAP request indication
    L2CAP_REQ_IND            = MSG_ID(L2CAP, 0x03),
    /// L2CAP confirmation
    L2CAP_CFM                = MSG_ID(L2CAP, 0x04),
};


/// L2CAP_CMD command codes
/*@TRACE*/
enum l2cap_cmd_code
{
    /* Connection oriented channel                      */
    /* ************************************************ */
    /// Command used to register a Simplified Protocol/Service Multiplexer in order to accept L2CAP connection oriented
    /// channel (COC) from a peer device. Security level parameter ensures that a certain level of security is
    /// met before requesting application to accept or reject channel establishment .
    L2CAP_COC_SPSM_ADD                  = 0x0100,
    /// Command used to unregister a Simplified Protocol/Service Multiplexer.
    L2CAP_COC_SPSM_REMOVE               = 0x0101,
    /// Command use to initiate creation of several L2CAP connection oriented channels.
    /// The operation completes when all requested L2CAP channels are created or when an error is detected.
    /// If all channels cannot be created, reason is provided in error code.
    L2CAP_COC_CREATE                    = 0x0102,
    /// Command used to change reception MTU size for given L2CAP channels. New MTU size must be greater or equals
    /// to the previous reception MTU size of any channels.
    /// The operation completes when all L2CAP channels reception MTU size is modified.
    /// If all channels cannot be reconfigured, reason is provided in error code.
    L2CAP_COC_RECONFIGURE               = 0x0103,
    /// Command use to initiate disconnection of a L2CAP connection oriented channel.
    L2CAP_COC_TERMINATE                 = 0x0104,

    /* Send PDU                                         */
    /* ************************************************ */
    /// Command used to send a SDU onto a specific L2CAP fixed or dynamic channel.
    /// @note Upper layer shall wait for L2CAP_CMP_EVT indication before sending other message to prevent
    ///       buffer overflow on the device.
    ///
    /// In debug mode, only L2CAP segment can be transmitted. This can be start segment or continuation segment.
    /// L2CAP length and channel ID (CID) fields must be present in SDU data.
    L2CAP_SDU_SEND                      = 0x0200,

    /* Debug only - Channel management                  */
    /* ************************************************ */
    /// Debug command used to register a fixed L2CAP channel such as ATT, Signaling or SMP. This channel does not support
    /// segmentation and reassembly mechanism.
    L2CAP_DBG_CHAN_FIX_REGISTER         = 0x0A01,
    /// Debug command use to register a dynamic L2CAP channel. This channel supports credit management, segmentation
    /// and reassembly mechanisms. To create a L2CAP Credit Based connection, @see L2CAP_COC_CREATE command should
    /// be used instead.
    L2CAP_DBG_CHAN_DYN_REGISTER         = 0x0A02,
    /// Debug command used to unregister a dynamic or a fixed channel. After sending this command, it�s no more
    /// possible to receive or send SDUs onto the channel
    L2CAP_DBG_CHAN_UNREGISTER           = 0x0A03,
    /// Debug command used to increment number of dynamic L2CAP channel transmission credits.
    L2CAP_DBG_CHAN_TX_CREDIT_ADD        = 0x0A04,
    /// Debug command used to configure or reconfigure a registered channel.
    L2CAP_DBG_CHAN_CONFIG_SET           = 0x0A05,
    /// Debug command used to retrieve information about a fixed or a dynamic channel.
    L2CAP_DBG_CHAN_INFO_GET             = 0x0A06,
    /// Command Used to control usage of Enhanced L2CAP COC negotiations
    L2CAP_DBG_COC_ENHANCED_NEGO_CTRL    = 0x0900,
    /// Command used to change reception MTU and MPS size for given L2CAP channels. New MTU size must be greater
    /// or equals to the previous reception MTU size of any channels.
    /// The operation completes when all L2CAP channels reception MTU size is modified.
    /// If all channels cannot be reconfigured, reason is provided in error code.
    L2CAP_DBG_COC_RECONFIGURE           = 0x0901,
    /// Debug command used to control reception of ACL packet at L2CAP level. When reception is off, received packet
    /// are put in a reception queue till reception is enabled again. When BLE connection is established without
    /// confirmation from application, RX is disabled by default.
    L2CAP_DBG_RX_CTRL                   = 0X0800,
};


/// L2CAP_IND indication codes
/*@TRACE*/
enum l2cap_ind_code
{
    /// Event triggered when an unknown message has been received by L2CAP layer  from an upper layer.
    L2CAP_UNKNOWN_MSG                   = 0x0000,
    /// Event triggered when a L2CAP connection oriented channel is created.
    L2CAP_COC_CREATED                   = 0x0100,
    /// Event triggered when a L2CAP connection oriented channel local or peer device reception MTU size is updated.
    L2CAP_COC_MTU_CHANGED               = 0x0101,
    /// Event triggered when a L2CAP connection oriented channel is terminated.
    L2CAP_COC_TERMINATED                = 0x0102,

    /* Debug only - Channel management                  */
    /* ************************************************ */
    /// Event triggered when an error is detected onto a L2CAP dynamic channel.
    L2CAP_DBG_CHAN_ERROR                = 0x0A00,
    /// Event triggered when new reception credits are available for a L2CAP dynamic channel.
    L2CAP_DBG_CHAN_RX_CREDIT_ADDED      = 0x0A01,
    /// Event triggered when transmission flow is paused onto a specific channel.
    L2CAP_DBG_CHAN_TX_FLOW_OFF          = 0x0A02,
};

/// L2CAP_REQ_IND request indication codes
/*@TRACE*/
enum l2cap_req_ind_code
{
    /* Connection oriented channel                      */
    /* ************************************************ */
    /// Inform Upper Layer that a L2CAP credit oriented connection is initiated by peer device.
    L2CAP_COC_CONNECT                   = 0x0100,

    /* SDU Reception                                    */
    /* ************************************************ */
    /// Inform Upper Layer about reception of SDU data over a L2CAP fixed or dynamically allocated channel.
    L2CAP_SDU_RX                        = 0x0200,
};

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// L2CAP security level
enum  l2cap_sec_lvl // TODO [FBE] reuse GAP Sec Level
{
    /// Channel can be created on an unencrypted link.
    L2CAP_SEC_NOT_ENC               = 0x00,
    /// Channel can be created on an encrypted link with unauthenticated pairing.
    L2CAP_SEC_UNAUTH                = 0x01,
    /// Channel can be created on an encrypted link with authenticated pairing.
    L2CAP_SEC_AUTH                  = 0x02,
    /// Channel can be created on an encrypted link with secure connection pairing.
    L2CAP_SEC_SECURE_CON            = 0x03,
};

/// L2CAP security level Bit Field
enum  l2cap_sec_lvl_bf
{
    /// With encrypted security requirement also requires a 128-bit encryption key size.
    L2CAP_COC_EKS_BIT               = 0x01,
    L2CAP_COC_EKS_POS               = 0,

    /// Channel minimum required security level (@see enum l2cap_sec_lvl)
    L2CAP_COC_AUTH_MASK             = 0x06,
    L2CAP_COC_AUTH_LSB              = 1,
};

/// L2CAP channel configuration Bit Field
enum  l2cap_chan_cfg_bf
{
    /// Channel enable; else reject any PDU transmission or reception
    L2CAP_CHAN_EN_BIT               = 0x01,
    L2CAP_CHAN_EN_POS               = 0,

    /// Pause PDU transmission; else resume
    L2CAP_CHAN_PDU_TX_PAUSED_BIT    = 0x02,
    L2CAP_CHAN_PDU_TX_PAUSED_POS    = 1,

    /// Enable credit flow control (Note: setting ignored for a fixed channel)
    L2CAP_CHAN_CREDIT_FLOW_EN_BIT   = 0x04,
    L2CAP_CHAN_CREDIT_FLOW_EN_POS   = 2,
};

/// L2CAP Channel Type
enum l2cap_chan_type
{
    /// Channel with a dynamically allocated L2CAP channel identifier
     L2CAP_CHAN_DYNAMIC             = 0,
     /// Channel with a fixed L2cap channel identifier
     L2CAP_CHAN_FIX                 = 1,
};

/// L2CAP channel parameter update Bit Field
enum  l2cap_chan_upd_bf
{
    /// Update Channel enable state
    L2CAP_CHAN_UP_EN_STATE_BIT             = 0x01,
    L2CAP_CHAN_UP_EN_STATE_POS             = 0,

    /// Update TX Paused state
    L2CAP_CHAN_UP_TX_PAUSED_STATE_BIT      = 0x02,
    L2CAP_CHAN_UP_TX_PAUSED_STATE_POS      = 1,

    /// Update Credit Flow control state
    L2CAP_CHAN_UP_CREDIT_FLOW_EN_STATE_BIT = 0x04,
    L2CAP_CHAN_UP_CREDIT_FLOW_EN_STATE_POS = 2,

    /// Update reception MTU
    L2CAP_CHAN_UP_RX_MTU_BIT               = 0x10,
    L2CAP_CHAN_UP_RX_MTU_POS               = 4,

    /// Update transmission MTU
    L2CAP_CHAN_UP_TX_MTU_BIT               = 0x20,
    L2CAP_CHAN_UP_TX_MTU_POS               = 5,

    /// Update reception MPS
    L2CAP_CHAN_UP_RX_MPS_BIT               = 0x40,
    L2CAP_CHAN_UP_RX_MPS_POS               = 6,

    /// Update transmission MPS
    L2CAP_CHAN_UP_TX_MPS_BIT               = 0x80,
    L2CAP_CHAN_UP_TX_MPS_POS               = 7,
};


/// L2CAP debug segment Bit Field
enum  l2cap_dbg_bf
{
    /// Start segment with L2CAP header present
    /// Can be used for invalid behaviors or to change the L2CAP destination channel identifier
    L2CAP_DBG_SEG_HEADER_PRESENT_BIT    = 0x01,
    L2CAP_DBG_SEG_HEADER_PRESENT_POS    = 0,

    /// Continuation of segment transmission
    L2CAP_DBG_SEG_CONTINUE_BIT          = 0x02,
    L2CAP_DBG_SEG_CONTINUE_POS          = 1,
};


/*
 * STRUCTURES
 ****************************************************************************************
 */


/*
 * DEFAULT MESSAGE CONTENT
 ****************************************************************************************
 */

/// Default L2CAP command structure
/*@TRACE*/
typedef struct l2cap_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
} l2cap_cmd_t;

/// Default L2CAP command Complete structure
/*@TRACE*/
typedef struct l2cap_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t status;
} l2cap_cmp_evt_t;

/// Default Channel L2CAP command complete structure
/*@TRACE*/
typedef struct l2cap_chan_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t status;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index.
    uint8_t  chan_lid;
} l2cap_chan_cmp_evt_t;

/// Default L2CAP indication structure
/*@TRACE*/
typedef struct l2cap_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    uint16_t ind_code;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t dummy;
} l2cap_ind_t;


/// Default L2CAP request indication structure
/*@TRACE*/
typedef struct l2cap_req_ind
{
    /// Request Indication code (@see enum l2cap_req_ind_code)
    uint16_t req_ind_code;
    /// Token provided by L2CAP module that must be used into the L2CAP_CFM message
    uint16_t token;
} l2cap_req_ind_t;


/// Default L2CAP confirm structure
/*@TRACE*/
typedef struct l2cap_cfm
{
    /// Request Indication code (@see enum l2cap_req_ind_code)
    uint16_t req_ind_code;
    /// Token provided by L2CAP module into the L2CAP_REQ_IND message
    uint16_t token;
} l2cap_cfm_t;


/*
 * MESSAGE CONTENT
 ****************************************************************************************
 */


/// L2CAP_COC_SPSM_ADD Command structure definition
/*@TRACE*/
typedef struct l2cap_coc_spsm_add_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_COC_SPSM_ADD
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Simplified Protocol/Service Multiplexer
    uint16_t spsm;
    /// Security level bit field (@see enum l2cap_sec_lvl_bf)
    uint8_t  sec_lvl_bf;
} l2cap_coc_spsm_add_cmd_t;

/// L2CAP_COC_SPSM_ADD uses default complete event structure
/*@TRACE*/
typedef l2cap_cmp_evt_t l2cap_coc_spsm_add_cmp_evt_t;


/// L2CAP_COC_SPSM_REMOVE Command structure definition
/*@TRACE*/
typedef struct l2cap_coc_spsm_remove_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_COC_SPSM_REMOVE
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Simplified Protocol/Service Multiplexer
    uint16_t spsm;
} l2cap_coc_spsm_remove_cmd_t;

/// L2CAP_COC_SPSM_REMOVE uses default complete event structure
/*@TRACE*/
typedef l2cap_cmp_evt_t l2cap_coc_spsm_remove_cmp_evt_t;


/// L2CAP_COC_CREATE Command structure definition
/*@TRACE*/
typedef struct l2cap_coc_create_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_COC_CREATE
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;

    /// Connection Index
    uint8_t   conidx;
    /// Number of L2CAP channel to create in parallel
    uint8_t   nb_chan;
    /// Simplified Protocol/Service Multiplexer
    uint16_t  spsm;
    /// Local reception Maximum Transmit Unit Size
    uint16_t  local_rx_mtu;
} l2cap_coc_create_cmd_t;

/// L2CAP_COC_CREATE Complete event structure definition
/*@TRACE*/
typedef struct l2cap_coc_create_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_COC_CREATE
    uint16_t cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t status;
    /// Connection Index
    uint8_t  conidx;
    /// Number of L2CAP channel created.
    uint8_t  nb_chan;
} l2cap_coc_create_cmp_evt_t;


/// L2CAP_COC_RECONFIGURE Command structure definition
/*@TRACE*/
typedef struct l2cap_coc_reconfigure_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_COC_RECONFIGURE
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    ///  New Maximum Transmit Size used for reception
    uint16_t local_rx_mtu;
    /// Number of L2CAP channel to reconfigure
    uint8_t  nb_chan;
    /// Array that contains list of L2CAP channel identifier to reconfigure
    uint8_t  chan_lid[__ARRAY_EMPTY];
} l2cap_coc_reconfigure_cmd_t;

/// L2CAP_COC_RECONFIGURE uses command complete event structure
/*@TRACE*/
typedef struct l2cap_coc_reconfigure_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t status;
    /// Connection Index
    uint8_t  conidx;
} l2cap_coc_reconfigure_cmp_evt_t;

/// L2CAP_COC_TERMINATE Command structure definition
/*@TRACE*/
typedef struct l2cap_coc_terminate_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_COC_TERMINATE
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
} l2cap_coc_terminate_cmd_t;

/// L2CAP_COC_TERMINATE uses default channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t l2cap_coc_terminate_cmp_evt_t;

/// L2CAP_SDU_SEND Command structure definition
/*@TRACE*/
typedef struct l2cap_sdu_send_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_SDU_SEND
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
    /// Debug bit field (@see enum l2cap_dbg_bf)
    uint8_t  dbg_bf;
    /// SDU Length
    uint16_t length;
    /// SDU Data
    uint8_t  data[__ARRAY_EMPTY];
} l2cap_sdu_send_cmd_t;

/// L2CAP_SDU_SENDuses default channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t l2cap_sdu_send_cmp_evt_t;

/// L2CAP_DBG_CHAN_FIX_REGISTER Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_fix_register_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_CHAN_FIX_REGISTER
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP Channel Identifier for both reception and transmission
    uint16_t cid;
    /// Maximum Transmit Unit Size for both reception and transmission
    uint16_t mtu;
} l2cap_dbg_chan_fix_register_cmd_t;

/// L2CAP_DBG_CHAN_FIX_REGISTERuses default channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t  l2cap_dbg_chan_fix_register_cmp_evt_t;

/// L2CAP_DBG_CHAN_DYN_REGISTER Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_dyn_register_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_CHAN_DYN_REGISTER
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// Reception L2CAP Channel Identifier
    uint16_t rx_cid;
    /// Transmission L2CAP Channel Identifier
    uint16_t tx_cid;
    /// Reception Maximum Transmit Unit Size
    uint16_t rx_mtu;
    /// Transmission Maximum Transmit Unit Size
    uint16_t tx_mtu;
    /// Reception Maximum Packet Size
    uint16_t rx_mps;
    /// Transmission Maximum Packet Size
    uint16_t tx_mps;
    /// Reception credit number
    uint16_t rx_credit;
    /// Transmission credit number
    uint16_t tx_credit;
} l2cap_dbg_chan_dyn_register_cmd_t;

/// L2CAP_DBG_CHAN_DYN_REGISTER uses default channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t l2cap_dbg_chan_dyn_register_cmp_evt_t;

/// L2CAP_DBG_CHAN_UNREGISTER Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_unregister_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_CHAN_UNREGISTER
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index.
    uint8_t  chan_lid;
} l2cap_dbg_chan_unregister_cmd_t;

/// L2CAP_DBG_CHAN_UNREGISTER uses default channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t l2cap_dbg_chan_unregister_cmp_evt_t;

/// L2CAP_DBG_CHAN_TX_CREDIT_ADD Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_tx_credit_add_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_CHAN_TX_CREDIT_ADD
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index.
    uint8_t  chan_lid;
    /// Number of credit to add for SDU transmission
    uint16_t credit;
} l2cap_dbg_chan_tx_credit_add_cmd_t;

/// L2CAP_DBG_CHAN_TX_CREDIT_ADD uses default channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t l2cap_dbg_chan_tx_credit_add_cmp_evt_t;

/// L2CAP_DBG_CHAN_CONFIG_SET Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_config_set_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_CHAN_CONFIG_SET
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index.
    uint8_t  chan_lid;
    /// Updated parameters Bit field  (@see enum l2cap_chan_upd_bf)
    uint8_t  update_bf;
    /// Configuration bit field (@see enum l2cap_chan_cfg_bf)
    uint8_t  config_bf;
    /// New reception MTU
    uint16_t rx_mtu;
    /// New transmission MTU
    /// @note setting ignored for a fixed channel.
    uint16_t tx_mtu;
    /// New reception MPS
    /// @note setting ignored for a fixed channel.
    uint16_t rx_mps;
    /// New transmission MPS
    /// @note setting ignored for a fixed channel.
    uint16_t tx_mps;
} l2cap_dbg_chan_config_set_cmd_t;

/// L2CAP_DBG_CHAN_CONFIG_SET uses channel command complete event structure
/*@TRACE*/
typedef l2cap_chan_cmp_evt_t l2cap_dbg_chan_config_set_cmp_evt_t;

/// L2CAP_DBG_CHAN_INFO_GET Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_info_get_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_CHAN_INFO_GET
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index.
    uint8_t  chan_lid;
} l2cap_dbg_chan_info_get_cmd_t;


/// L2CAP_DBG_CHAN_INFO_GET Complete event structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_info_get_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t          cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t          dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t          status;
    /// Connection Index
    uint8_t           conidx;
    /// L2CAP channel local index.
    uint8_t           chan_lid;
    /// Channel type (@see enum l2cap_chan_type)
    uint8_t           chan_type;
    /// Configuration bit field (@see enum l2cap_chan_cfg_bf)
    uint8_t           config_bf;
    /// Reception L2CAP Channel Identifier
    uint16_t          rx_cid;
    /// Transmission L2CAP Channel Identifier
    uint16_t          tx_cid;
    ///Reception Maximum Transmit Unit Size
    uint16_t          rx_mtu;
    /// Transmission Maximum Transmit Unit Size
    uint16_t          tx_mtu;
    /// Reception Maximum Packet Size.        (@note 0 for a fixed L2CAP channel)
    uint16_t          rx_mps;
    /// Transmission Maximum Packet Size.     (@note 0 for a fixed L2CAP channel)
    uint16_t          tx_mps;
    /// Reception credit number.              (@note 0 for a fixed L2CAP channel)
    uint16_t          rx_credit;
    /// Transmission reception credit number. (@note 0 for a fixed L2CAP channel)
    uint16_t          tx_credit;
} l2cap_dbg_chan_info_get_cmp_evt_t;

/// L2CAP_DBG_COC_RECONFIGURE Command structure definition
/*@TRACE*/
typedef struct l2cap_dbg_coc_reconfigure_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_COC_RECONFIGURE
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    ///  New Maximum Transmit Size used for reception
    uint16_t local_rx_mtu;
    ///  New Maximum Packet Size used for reception
    uint16_t local_rx_mps;
    /// Number of L2CAP channel to reconfigure
    uint8_t  nb_chan;
    /// Array that contains list of L2CAP channel identifier to reconfigure
    uint8_t  chan_lid[__ARRAY_EMPTY];
} l2cap_dbg_coc_reconfigure_cmd_t;

/// L2CAP_COC_RECONFIGURE uses L2CAP_COC_RECONFIGURE command complete event structure
/*@TRACE*/
typedef l2cap_coc_reconfigure_cmp_evt_t l2cap_dbg_coc_reconfigure_cmp_evt_t;

/// L2CAP_DBG_COC_ENHANCED_NEGO_CTRL command structure
/*@TRACE*/
typedef struct l2cap_dbg_coc_enhanced_nego_ctrl_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_COC_ENHANCED_NEGO_CTRL
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// 0: Disable enhanced COC negotiation
    /// 1: Enable enhanced COC negotiation
    uint8_t  enable;
} l2cap_dbg_coc_enhanced_nego_ctrl_cmd_t;

/// L2CAP_DBG_COC_ENHANCED_NEGO_CTRL complete event structure
/*@TRACE*/
typedef struct l2cap_dbg_coc_enhanced_nego_ctrl_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t status;
    /// Connection Index
    uint8_t  conidx;
} l2cap_dbg_coc_enhanced_nego_ctrl_cmp_evt_t;

/// L2CAP_DBG_RX_CTRL command structure
/*@TRACE*/
typedef struct l2cap_dbg_rx_ctrl_cmd
{
    /// Command code (@see enum l2cap_cmd_code)
    ///  - L2CAP_DBG_RX_CTRL
    uint16_t cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// 0: Reception disabled, received ACL packets are put in a queue
    /// 1: Reception enabled
    uint8_t  enable;
} l2cap_dbg_rx_ctrl_cmd_t;

/// L2CAP_DBG_RX_CTRL complete event structure
/*@TRACE*/
typedef struct l2cap_dbg_rx_ctrl_cmp_evt
{
    /// Command code (@see enum l2cap_cmd_code)
    uint16_t cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t status;
    /// Connection Index
    uint8_t  conidx;
} l2cap_dbg_rx_ctrl_cmp_evt_t;

/// L2CAP_UNKNOWN_MSG Indication structure definition
/*@TRACE*/
typedef struct l2cap_unknown_msg_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_UNKNOWN_MSG
    uint16_t ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t dummy;
    /// Message identifier
    uint16_t msg_id;
} l2cap_unknown_msg_ind_t;

/// L2CAP_COC_CREATED Indication structure definition
/*@TRACE*/
typedef struct l2cap_coc_created_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_COC_CREATED
    uint16_t ind_code;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// Created L2CAP channel local index
    uint8_t  chan_lid;
    /// Local device reception Maximum Transmit Unit size
    uint16_t local_rx_mtu;
    /// Peer device reception Maximum Transmit Unit size
    uint16_t peer_rx_mtu;
} l2cap_coc_created_ind_t;

/// L2CAP_COC_MTU_CHANGED  Indication structure definition
/*@TRACE*/
typedef struct l2cap_coc_mtu_changed_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_COC_MTU_CHANGED
    uint16_t ind_code;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
    /// Local device reception Maximum Transmit Unit size
    uint16_t local_rx_mtu;
    /// Peer device reception Maximum Transmit Unit size
    uint16_t peer_rx_mtu;
} l2cap_coc_mtu_changed_ind_t;

/// L2CAP_COC_TERMINATED Indication structure definition
/*@TRACE*/
typedef struct l2cap_coc_terminated_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_COC_TERMINATED
    uint16_t ind_code;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
    /// Termination Reason (@see enum hl_err)
    uint16_t reason;
} l2cap_coc_terminated_ind_t;

/// L2CAP_DBG_CHAN_ERROR Indication structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_error_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_DBG_CHAN_ERROR
    uint16_t ind_code;
    /// 0x0000.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
    /// Termination Reason (@see enum hl_err)
    uint16_t reason;
} l2cap_dbg_chan_error_ind_t;


/// L2CAP_DBG_CHAN_TX_FLOW_OFF Indication structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_tx_flow_off_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_DBG_CHAN_TX_FLOW_OFF
    uint16_t ind_code;
    /// 0x0000.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
} l2cap_dbg_chan_tx_flow_off_ind_t;

/// L2CAP_DBG_CHAN_RX_CREDIT_ADDED Indication structure definition
/*@TRACE*/
typedef struct l2cap_dbg_chan_rx_credit_added_ind
{
    /// Indication code (@see enum l2cap_ind_code)
    ///  - L2CAP_DBG_CHAN_RX_CREDIT_ADDED
    uint16_t ind_code;
    /// 0x0000.
    uint16_t dummy;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
    /// Number of credit added for PDU reception
    uint16_t credits;
} l2cap_dbg_chan_rx_credit_added_ind_t;

/// L2CAP_COC_CONNECT request indication structure
/*@TRACE*/
typedef struct l2cap_coc_connect_req_ind
{
    /// Request Indication code (@see enum l2cap_req_ind_code)
    uint16_t req_ind_code;
    /// Token provided by L2CAP module that must be used into the L2CAP_CFM message
    uint16_t token;
    /// Connection Index
    uint8_t  conidx;
    /// Number of L2CAP channel requested to be created in parallel
    uint8_t  nb_chan;
    /// Simplified Protocol/Service Multiplexer
    uint16_t spsm;
    /// Peer device Maximum Transmit Unit reception size
    uint16_t peer_rx_mtu;
} l2cap_coc_connect_req_ind_t;


/// L2CAP_COC_CONNECT confirm structure
/*@TRACE*/
typedef struct l2cap_coc_connect_cfm
{
    /// Request Indication code (@see enum l2cap_req_ind_code)
    uint16_t req_ind_code;
    /// Token provided by L2CAP module into the L2CAP_REQ_IND message
    uint16_t token;
    /// Connection Index
    uint8_t  conidx;
    /// Number of L2CAP channel local device accepts to create in parallel
    /// 0: reject all channel creation
    uint8_t  nb_chan;
    /// Local reception Maximum Transmit Unit Size
    uint16_t local_rx_mtu;
} l2cap_coc_connect_cfm_t;

/// L2CAP_SDU_RX request indication structure
/*@TRACE*/
typedef struct l2cap_sdu_rx_req_ind
{
    /// Request Indication code (@see enum l2cap_req_ind_code)
    uint16_t req_ind_code;
    /// Token req_ind_code by L2CAP module that must be used into the L2CAP_CFM message
    uint16_t token;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
    /// Reception status (@see enum hl_err)
    uint16_t status;
    /// SDU Length
    uint16_t length;
    /// SDU Data
    uint8_t  data[__ARRAY_EMPTY];
} l2cap_sdu_rx_req_ind_t;

/// L2CAP_SDU_RX confirm structure
/*@TRACE*/
typedef struct l2cap_sdu_rx_cfm
{
    /// Request Indication code (@see enum l2cap_req_ind_code)
    uint16_t req_ind_code;
    /// Token provided by L2CAP module into the L2CAP_REQ_IND message
    uint16_t token;
    /// Connection Index
    uint8_t  conidx;
    /// L2CAP channel local index
    uint8_t  chan_lid;
} l2cap_sdu_rx_cfm_t;

/// @} L2CAP_MSG

#endif // L2CAP_MSG_H_
