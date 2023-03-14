/**
 ****************************************************************************************
 *
 * @file gapc_msg.h
 *
 * @brief Generic Access Profile Controller  Message API.
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 ****************************************************************************************
 */
#ifndef _GAPC_MSG_H_
#define _GAPC_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup GAPC_MSG Generic Access Profile Controller Message API
 * @ingroup GAPC
 * @brief Handles ALL messages to/from GAP Controller block.
 *
 * It handles messages from lower and higher layers related to an ongoing connection.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_task.h" // Task definitions
#include "gap.h"
#include <stdbool.h>
#include "ke_msg.h"

/*
 * MACROS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */


/// GAP Controller Task messages
/*@TRACE*/
enum gapc_msg_id
{
    /* Default event */
    /// Command Complete event
    GAPC_CMP_EVT                                        = MSG_ID(GAPC, 0x00),
    /// Indication to the task that sends the unknown message
    GAPC_UNKNOWN_MSG_IND                                = MSG_ID(GAPC, 0x01),

    /* Connection state information */
    /// Indicate that a connection has been established
    GAPC_CONNECTION_REQ_IND                             = MSG_ID(GAPC, 0x02),
    /// Set specific link data configuration.
    GAPC_CONNECTION_CFM                                 = MSG_ID(GAPC, 0x03),
    /// Indicate that there is no more ATT bearer available
    GAPC_NO_MORE_ATT_BEARER_ERROR_IND                   = MSG_ID(GAPC, 0x04),

    /* Link management command */
    /// Request disconnection of current link command.
    GAPC_DISCONNECT_CMD                                 = MSG_ID(GAPC, 0x05),
    /// Indicate that a link has been disconnected
    GAPC_DISCONNECT_IND                                 = MSG_ID(GAPC, 0x06),

    /* ATTRIBUTE CLIENT */
    /// Indicate that the ATT MTU has been updated (negotiated)
    GAPC_MTU_CHANGED_IND                                = MSG_ID(GAPC, 0x07),


    /* Connection info */
    /// Retrieve information command
    GAPC_GET_INFO_CMD                                   = MSG_ID(GAPC, 0x10),
    /// Peer device attribute DB info such as Device Name, Appearance or Slave Preferred Parameters
    GAPC_PEER_ATT_INFO_IND                              = MSG_ID(GAPC, 0x11),
    /// Indication of peer version info
    GAPC_PEER_VERSION_IND                               = MSG_ID(GAPC, 0x12),
    /// Indication of peer features info
    GAPC_PEER_FEATURES_IND                              = MSG_ID(GAPC, 0x13),
    /// Indication of ongoing connection RSSI
    GAPC_CON_RSSI_IND                                   = MSG_ID(GAPC, 0x14),
    /// Indication of ongoing connection Channel Map
    GAPC_CON_CHANNEL_MAP_IND                            = MSG_ID(GAPC, 0x15),
    /// Peer device request local device info such as name, appearance or slave preferred parameters
    GAPC_GET_DEV_INFO_REQ_IND                           = MSG_ID(GAPC, 0x16),
    /// Send requested info to peer device
    GAPC_GET_DEV_INFO_CFM                               = MSG_ID(GAPC, 0x17),
    /// Peer device request to modify local device info such as name or appearance
    GAPC_SET_DEV_INFO_REQ_IND                           = MSG_ID(GAPC, 0x18),
    /// Local device accept or reject device info modification
    GAPC_SET_DEV_INFO_CFM                               = MSG_ID(GAPC, 0x19),
    /// Indication of currently used channel selection algorithm
    /// @see struct gapc_chan_sel_algo_ind
    GAPC_CHAN_SEL_ALGO_IND                              = MSG_ID(GAPC, 0x1A),

    /* Connection parameters update */
    /// Perform update of connection parameters command
    GAPC_PARAM_UPDATE_CMD                               = MSG_ID(GAPC, 0x40),
    /// Request of updating connection parameters indication
    GAPC_PARAM_UPDATE_REQ_IND                           = MSG_ID(GAPC, 0x41),
    /// Master confirm or not that parameters proposed by slave are accepted or not
    GAPC_PARAM_UPDATE_CFM                               = MSG_ID(GAPC, 0x42),
    /// Connection parameters updated indication
    GAPC_PARAM_UPDATED_IND                              = MSG_ID(GAPC, 0x43),
    /// LE Set Data Length Command
    GAPC_SET_LE_PKT_SIZE_CMD                            = MSG_ID(GAPC, 0x44),
    /// LE Set Data Length Indication
    GAPC_LE_PKT_SIZE_IND                                = MSG_ID(GAPC, 0x45),
    /// Update LE Ping timeout value
    GAPC_SET_LE_PING_TO_CMD                             = MSG_ID(GAPC, 0x46),
    /// LE Ping timeout indication
    GAPC_LE_PING_TO_VAL_IND                             = MSG_ID(GAPC, 0x47),
    /// LE Ping timeout expires indication
    GAPC_LE_PING_TO_IND                                 = MSG_ID(GAPC, 0x48),
    /// Set the PHY configuration for current active link
    GAPC_SET_PHY_CMD                                    = MSG_ID(GAPC, 0x49),
    /// Active link PHY configuration. Triggered when configuration is read or during an update.
    GAPC_LE_PHY_IND                                     = MSG_ID(GAPC, 0x4A),
    /// Set the preferred slave latency (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_LATENCY_CMD                     = MSG_ID(GAPC, 0x4B),
    /// Set the preferred slave event duration (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_EVT_DUR_CMD                     = MSG_ID(GAPC, 0x4C),
    /// Set the maximum reception and size and time using DLE negotiation
    GAPC_SET_MAX_RX_SIZE_AND_TIME_CMD                   = MSG_ID(GAPC, 0x4D),

    /* Bonding procedure */
    /// Start Bonding command procedure
    GAPC_BOND_CMD                                       = MSG_ID(GAPC, 0x50),
    /// Bonding requested by peer device indication message.
    GAPC_BOND_REQ_IND                                   = MSG_ID(GAPC, 0x51),
    /// Confirm requested bond information.
    GAPC_BOND_CFM                                       = MSG_ID(GAPC, 0x52),
    /// Bonding information indication message
    GAPC_BOND_IND                                       = MSG_ID(GAPC, 0x53),
    /// Request to inform the remote device when keys have been entered or erased
    GAPC_KEY_PRESS_NOTIFICATION_CMD                     = MSG_ID(GAPC, 0x54),
    /// Indication that a KeyPress has been performed on the peer device.
    GAPC_KEY_PRESS_NOTIFICATION_IND                     = MSG_ID(GAPC, 0x55),
    /* Security request procedure */
    /// Start Security Request command procedure
    GAPC_SECURITY_CMD                                   = MSG_ID(GAPC, 0x56),
    /// Security requested by peer device indication message
    GAPC_SECURITY_IND                                   = MSG_ID(GAPC, 0x57),
    /* Encryption procedure */
    /// Start Encryption command procedure
    GAPC_ENCRYPT_CMD                                    = MSG_ID(GAPC, 0x58),
    /// Encryption requested by peer device indication message.
    GAPC_ENCRYPT_REQ_IND                                = MSG_ID(GAPC, 0x59),
    /// Confirm requested Encryption information.
    GAPC_ENCRYPT_CFM                                    = MSG_ID(GAPC, 0x5A),
    /// Encryption information indication message
    GAPC_ENCRYPT_IND                                    = MSG_ID(GAPC, 0x5B),
    /* Bond Data information  */
    /// Indicate update of bond data information
    GAPC_BOND_DATA_UPDATE_IND                           = MSG_ID(GAPC, 0x5C),

    /* Periodic Sync Transfer */
    /// Transfer periodic advertising sync information to peer device
    GAPC_PER_ADV_SYNC_TRANS_CMD                         = MSG_ID(GAPC, 0x60),
    /* Client Features */
    /// Enable usage of supported client features
    GAPC_CLI_FEAT_EN_CMD                                = MSG_ID(GAPC, 0x61),

    /* Constant Tone Extension */
    /// Constant Tone Extension Transmission configuration command
    GAPC_CTE_TX_CFG_CMD                                 = MSG_ID(GAPC, 0x70),
    /// Constant Tone Extension Reception configuration command
    GAPC_CTE_RX_CFG_CMD                                 = MSG_ID(GAPC, 0x71),
    /// Constant Tone Extension request control command (enable / disable)
    GAPC_CTE_REQ_CTRL_CMD                               = MSG_ID(GAPC, 0x72),
    /// Constant Tone Extension Response control command (enable / disable)
    GAPC_CTE_RSP_CTRL_CMD                               = MSG_ID(GAPC, 0x73),
    /// Indicate reception of a IQ Report event over a ble connection
    GAPC_CTE_IQ_REPORT_IND                              = MSG_ID(GAPC, 0x74),

    /* LE Power Control */
    /// Local TX power indication
    GAPC_LOC_TX_PWR_IND                                 = MSG_ID(GAPC, 0x80),
    /// Remote TX power indication
    GAPC_PEER_TX_PWR_IND                                = MSG_ID(GAPC, 0x81),
    /// Control TX Power Reports command
    GAPC_TX_PWR_REPORT_CTRL_CMD                         = MSG_ID(GAPC, 0x82),
    /// Local TX power change report indication
    GAPC_LOC_TX_PWR_REPORT_IND                          = MSG_ID(GAPC, 0x83),
    /// Remote TX power change report indication
    GAPC_PEER_TX_PWR_REPORT_IND                         = MSG_ID(GAPC, 0x84),
    /// Control Path loss configuration
    GAPC_PATH_LOSS_CTRL_CMD                             = MSG_ID(GAPC, 0x85),
    /// Path Loss Threshold Event Indication
    GAPC_PATH_LOSS_THRESHOLD_IND                        = MSG_ID(GAPC, 0x86),
};

/// request operation type - application interface
/*@TRACE*/
enum gapc_operation
{
    /*                 Operation Flags                  */
    /* No Operation (if nothing has been requested)     */
    /* ************************************************ */
    /// No operation
    GAPC_NO_OP                                    = 0x00,

    /* Connection management */
    /// Disconnect link
    GAPC_DISCONNECT                               = 0x01,

    /* Connection information */
    /// Retrieve name of peer device.
    GAPC_GET_PEER_NAME                            = 0x10,
    /// Retrieve peer device version info.
    GAPC_GET_PEER_VERSION                         = 0x11,
    /// Retrieve peer device features.
    GAPC_GET_PEER_FEATURES                        = 0x12,
    /// Get Peer device appearance
    GAPC_GET_PEER_APPEARANCE                      = 0x13,
    /// Get Peer device Slaved Preferred Parameters
    GAPC_GET_PEER_SLV_PREF_PARAMS                 = 0x14,
    /// Retrieve connection RSSI.
    GAPC_GET_CON_RSSI                             = 0x15,
    /// Retrieve Connection Channel MAP.
    GAPC_GET_CON_CHANNEL_MAP                      = 0x16,
    /// Retrieve Channel Selection Algorithm
    GAPC_GET_CHAN_SEL_ALGO                        = 0x17,
    /// Get if Central Address resolution supported
    GAPC_GET_ADDR_RESOL_SUPP                      = 0x18,
    /// Retrieve Peer database Hash value
    GAPC_GET_PEER_DB_HASH                         = 0x19,
    /// get timer timeout value
    GAPC_GET_LE_PING_TO                           = 0x1A,
    /// Retrieve PHY configuration of active link
    GAPC_GET_PHY                                  = 0x1B,
    /// Read the local current and maximum transmit power levels for 1M PHY
    GAPC_GET_LOC_TX_PWR_LEVEL_1M                  = 0x1C,
    /// Read the local current and maximum transmit power levels for 2M PHY
    GAPC_GET_LOC_TX_PWR_LEVEL_2M                  = 0x1D,
    /// Read the local current and maximum transmit power levels for LE CODED PHY with S=8 data coding
    GAPC_GET_LOC_TX_PWR_LEVEL_LE_CODED_S8         = 0x1E,
    /// Read the local current and maximum transmit power levels for LE CODED PHY with S=2 data coding
    GAPC_GET_LOC_TX_PWR_LEVEL_LE_CODED_S2         = 0x1F,

    /// Read the transmit power level used by the remote Controller for 1M PHY
    GAPC_GET_PEER_TX_PWR_LEVEL_1M                 = 0x20,
    /// Read the transmit power level used by the remote Controller for 2M PHY
    GAPC_GET_PEER_TX_PWR_LEVEL_2M                 = 0x21,
    /// Read the transmit power level used by the remote Controller for LE CODED PHY with S=8 data coding
    GAPC_GET_PEER_TX_PWR_LEVEL_LE_CODED_S8        = 0x22,
    /// Read the transmit power level used by the remote Controller for LE CODED PHY with S=2 data coding
    GAPC_GET_PEER_TX_PWR_LEVEL_LE_CODED_S2        = 0x23,
    /// Resolvable Private Address Only declaration
    GAPC_GET_RSLV_PRIV_ADDR_ONLY                  = 0x24,

    /* Connection parameters update */
    /// Perform update of connection parameters.
    GAPC_UPDATE_PARAMS                            = 0x40,
    /// set timer timeout value
    GAPC_SET_LE_PING_TO                           = 0x41,
    /// LE Set Data Length
    GAPC_SET_LE_PKT_SIZE                          = 0x42,
    /// Set the PHY configuration for current active link
    GAPC_SET_PHY                                  = 0x43,
    /// Set the preferred slave latency (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_LATENCY                   = 0x44,
    /// Set the preferred slave event duration (for slave only, with RW controller)
    GAPC_SET_PREF_SLAVE_EVT_DUR                   = 0x45,
    /// Set maximum RX size and time using DLE negotiation
    GAPC_SET_MAX_RX_SIZE_AND_TIME                 = 0x46,

    /* Security procedures */
    /// Start bonding procedure.
    GAPC_BOND                                     = 0x50,
    /// Start encryption procedure.
    GAPC_ENCRYPT                                  = 0x51,
    /// Start security request procedure
    GAPC_SECURITY_REQ                             = 0x52,
    /// Request to inform the remote device when keys have been entered or erased
    GAPC_KEY_PRESS_NOTIFICATION                   = 0x53,

    /* Periodic Sync Transfer */
    /// Transfer periodic advertising sync information to peer device
    GAPC_PER_ADV_SYNC_TRANS                       = 0x60,

    /* Client Features */
    /// Enable usage of supported client features
    GAPC_CLI_FEAT_EN                              = 0x61,

    /* Constant Tone Extension */
    /// Constant Tone Extension Transmission configuration
    GAPC_CTE_TX_CFG                               = 0x70,
    /// Constant Tone Extension Reception configuration
    GAPC_CTE_RX_CFG                               = 0x71,
    /// Constant Tone Extension request control (enable / disable)
    GAPC_CTE_REQ_CTRL                             = 0x72,
    /// Constant Tone Extension Response control (enable / disable)
    GAPC_CTE_RSP_CTRL                             = 0x73,

    /* LE Power Control */
    /// Enable or disable the reporting to the local Host of transmit power level
    /// changes in the local and remote Controllers for the ACL connection
    GAPC_TX_PWR_REPORT_CTRL                       = 0x80,
    /// Command is used to enable/disable path loss reporting for the connection
    GAPC_PATH_LOSS_REPORT_CTRL                    = 0x81,
};

/// Bond event type.
/*@TRACE*/
enum gapc_bond
{
    /// Bond Pairing request
    GAPC_PAIRING_REQ,
    /// Respond to Pairing request
    GAPC_PAIRING_RSP,

    /// Pairing Finished information
    GAPC_PAIRING_SUCCEED,
    /// Pairing Failed information
    GAPC_PAIRING_FAILED,

    /// Used to retrieve pairing Temporary Key
    GAPC_TK_EXCH,
    /// Used for Identity Resolving Key exchange
    GAPC_IRK_EXCH,
    /// Used for Connection Signature Resolving Key exchange
    GAPC_CSRK_EXCH,
    /// Used for Long Term Key exchange
    GAPC_LTK_EXCH,

    /// Bond Pairing request issue, Repeated attempt
    GAPC_REPEATED_ATTEMPT,

    /// Out of Band - exchange of confirm and rand.
    GAPC_OOB_EXCH,

    /// Numeric Comparison - Exchange of Numeric Value -
    GAPC_NC_EXCH
};

/// List of device info that should be provided by application
/*@TRACE*/
enum gapc_dev_info
{
    /// Device Name
    GAPC_DEV_NAME,
    /// Device Appearance Icon
    GAPC_DEV_APPEARANCE,
    /// Device Slave preferred parameters
    GAPC_DEV_SLV_PREF_PARAMS,
    /// Device Central address resolution
    GAPC_DEV_CTL_ADDR_RESOL,
    /// Device database hash value
    GAPC_DEV_DB_HASH,
    /// Resolvable Private address only after bond
    GAPC_DEV_RSLV_PRIV_ADDR_ONLY,
    /// maximum device info parameter
    GAPC_DEV_INFO_MAX,
};

/// Option for PHY configuration
enum gapc_phy_option
{
    /// No preference for rate used when transmitting on the LE Coded PHY
    GAPC_PHY_OPT_LE_CODED_ALL_RATES     = (1 << 0),
    /// 500kbps rate preferred when transmitting on the LE Coded PHY
    GAPC_PHY_OPT_LE_CODED_500K_RATE     = (1 << 1),
    /// 125kbps  when transmitting on the LE Coded PHY
    GAPC_PHY_OPT_LE_CODED_125K_RATE     = (1 << 2),
};

/// Keypress Notification types
enum gapc_key_ntf_type
{
    /// Passkey entry started
    GAPC_KEY_NTF_ENTRY_STARTED = 0x00,
    /// Passkey digit entered
    GAPC_KEY_NTF_DIGIT_ENTERED,
    /// Passkey digit erased
    GAPC_KEY_NTF_DIGIT_ERASED,
    /// Passkey cleared
    GAPC_KEY_NTF_CLEARED,
    /// Passkey entry completed
    GAPC_KEY_NTF_ENTRY_COMPLETED
};

/// Client bond information
enum gapc_cli_info
{
    /// Service changed indication enabled
    GAPC_CLI_SVC_CHANGED_IND_EN_BIT = (1 << 0),
    GAPC_CLI_SVC_CHANGED_IND_EN_POS = 0,
    /// Database updated since last connection
    GAPC_CLI_DB_UPDATED_BIT         = (1 << 1),
    GAPC_CLI_DB_UPDATED_POS         = 1,
};

/// Client supported features
enum gapc_cli_feat
{
    /// Robust Cache feature enabled
    GAPC_CLI_ROBUST_CACHE_EN_BIT    = (1 << 0),
    GAPC_CLI_ROBUST_CACHE_EN_POS    = 0,
    /// The client supports Enhanced ATT bearer
    GAPC_CLI_EATT_SUPPORTED_BIT     = (1 << 1),
    GAPC_CLI_EATT_SUPPORTED_POS     = 1,
    /// The client supports Multiple Handle Value Notifications
    GAPC_CLI_MULT_NTF_SUPPORTED_BIT = (1 << 2),
    GAPC_CLI_MULT_NTF_SUPPORTED_POS = 2,
};

/// Server supported features
enum gapc_srv_feat
{
    /// Enhanced ATT bearer supported
    GAPC_SRV_EATT_SUPPORTED_BIT     = (1 << 0),
    GAPC_SRV_EATT_SUPPORTED_POS     = 0,
};


/// Power Control Bit Field parameters.
enum gapc_pwr_ctrl_flags
{
    /// bit[0] - Sender is at the minimum supported power level
    GAPC_PWR_CTRL_MIN_BIT    = 0x01,
    GAPC_PWR_CTRL_MIN_POS    = 0,

    /// bit[1] - Sender is at the maximum supported power level
    GAPC_PWR_CTRL_MAX_BIT    = 0x02,
    GAPC_PWR_CTRL_MAX_POS    = 1,
};

/// Specify which PHY the Controller is specifying transmit power.
enum gapc_phy_pwr_value
{
    /// 1 Mbits PHY
    GAPC_PHY_PWR_1MBPS_VALUE    = 1,
    /// 2 Mbits PHY
    GAPC_PHY_PWR_2MBPS_VALUE    = 2,
    /// LE Coded PHY with S=8 data coding
    GAPC_PHY_PWR_S8_CODED_VALUE = 3,
    /// LE Coded PHY with S=2 data coding
    GAPC_PHY_PWR_S2_CODED_VALUE = 4,
};

/// Path Loss zones.
enum gapc_path_loss_zone
{
    /// Entered Low zone
    GAPC_PATH_LOSS_LOW           = 0,
    /// Entered Middle zone
    GAPC_PATH_LOSS_MID           = 1,
    /// Entered High zone
    GAPC_PATH_LOSS_HIGH          = 2,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Operation command structure in order to keep requested operation.
struct gapc_operation_cmd
{
    /// GAP request type
    uint8_t operation;
};


/// Command complete event data structure
/*@TRACE*/
struct gapc_cmp_evt
{
    /// GAP request type
    uint8_t operation;
    /// Status of the request
    uint8_t status;
};

/// Indicate that an unknown message has been received
/*@TRACE*/
struct gapc_unknown_msg_ind
{
    /// Unknown message id
    uint16_t unknown_msg_id;
};

/// Indicate that a connection has been established
/*@TRACE*/
struct gapc_connection_req_ind
{
    /// Connection handle
    uint16_t   conhdl;
    /// Connection interval
    uint16_t   con_interval;
    /// Connection latency
    uint16_t   con_latency;
    /// Link supervision timeout
    uint16_t   sup_to;
    /// Clock accuracy
    uint8_t    clk_accuracy;
    /// Peer address type
    uint8_t    peer_addr_type;
    /// Peer BT address
    gap_addr_t peer_addr;
    /// Role of device in connection (0 = Master / 1 = Slave)
    uint8_t    role;
};


/// Set specific link data configuration.
/*@TRACE*/
struct gapc_connection_cfm
{
    /// Local CSRK value
    gap_sec_key_t lcsrk;
    /// Local signature counter value
    uint32_t           lsign_counter;
    /// Remote CSRK value
    gap_sec_key_t rcsrk;
    /// Remote signature counter value
    uint32_t           rsign_counter;
    /// Pairing level (@see enum gap_pairing_lvl)
    uint8_t            pairing_lvl;
    /// Client bond data information (@see enum gapc_cli_info)
    uint8_t            cli_info;
    /// LTK exchanged during pairing.
    bool               ltk_present;
    /// Client supported features    (@see enum gapc_cli_feat)
    uint8_t            cli_feat;
    /// Peer GATT Service Start handle
    uint16_t           gatt_start_hdl;
    /// Peer GATT Service End Handle
    uint16_t           gatt_end_hdl;
    /// Peer Service Change value handle
    uint16_t           svc_chg_hdl;
    /// Server supported features    (@see enum gapc_srv_feat)
    uint8_t            srv_feat;
};

/// Request disconnection of current link command.
/*@TRACE*/
struct gapc_disconnect_cmd
{
    /// GAP request type:
    /// - GAPC_DISCONNECT: Disconnect link.
    uint8_t operation;

    /// Reason of disconnection
    uint8_t reason;
};


/// Indicate that a link has been disconnected
/*@TRACE*/
struct gapc_disconnect_ind
{
    /// Connection handle
    uint16_t conhdl;
    /// Reason of disconnection
    uint8_t reason;
};

/// Indicate that the ATT MTU has been updated (negotiated)
struct gapc_mtu_changed_ind
{
    /// Exchanged MTU value
    uint16_t mtu;
    /// operation sequence number
    uint16_t seq_num;
};

/// Retrieve information command
/*@TRACE*/
struct gapc_get_info_cmd
{
    /// GAP request type:
    /// - GAPC_GET_PEER_NAME: Retrieve name of peer device.
    /// - GAPC_GET_PEER_VERSION: Retrieve peer device version info.
    /// - GAPC_GET_PEER_FEATURES: Retrieve peer device features.
    /// - GAPC_GET_PEER_APPEARANCE: Get Peer device appearance
    /// - GAPC_GET_PEER_SLV_PREF_PARAMS: Get Peer device Slaved Preferred Parameters
    /// - GAPC_GET_CON_RSSI: Retrieve connection RSSI.
    /// - GAPC_GET_CON_CHANNEL_MAP: Retrieve Connection Channel MAP.
    /// - GAPC_GET_CHAN_SEL_ALGO: Retrieve Channel Selection Algorithm
    /// - GAPC_GET_ADDR_RESOL_SUPP: Address Resolution Supported
    /// - GAPC_GET_PEER_DB_HASH: Retrieve Peer database Hash value
    /// - GAPC_GET_LE_PING_TO: Retrieve LE Ping Timeout Value
    /// - GAPC_GET_PHY: Retrieve PHY configuration of active link
    /// - GAPC_GET_LOC_TX_PWR_LEVEL_1M: Read the current and maximum transmit power levels for 1M PHY
    /// - GAPC_GET_LOC_TX_PWR_LEVEL_2M: Read the current and maximum transmit power levels for 2M PHY
    /// - GAPC_GET_LOC_TX_PWR_LEVEL_LE_CODED_S8: Read the current and maximum transmit power levels for LE CODED PHY with S=8 data coding
    /// - GAPC_GET_LOC_TX_PWR_LEVEL_LE_CODED_S2: Read the current and maximum transmit power levels for LE CODED PHY with S=2 data coding
    /// - GAPC_GET_PEER_TX_PWR_LEVEL_1M: Read the transmit power level used by the remote Controller for 1M PHY
    /// - GAPC_GET_PEER_TX_PWR_LEVEL_2M: Read the transmit power level used by the remote Controller for 2M PHY
    /// - GAPC_GET_PEER_TX_PWR_LEVEL_LE_CODED_S8: Read the transmit power level used by the remote Controller for LE CODED PHY with S=8 data coding
    /// - GAPC_GET_PEER_TX_PWR_LEVEL_LE_CODED_S2: Read the transmit power level used by the remote Controller for LE CODED PHY with S=2 data coding
    /// - GAPC_GET_RSLV_PRIV_ADDR_ONLY: Resolvable Private Address Only declaration
    uint8_t operation;
};

/// device information data
/*@TRACE
 @trc_ref gapc_dev_info*/
union gapc_dev_info_val
{
    /// Device name
    //@trc_union parent.req == GAPC_DEV_NAME
    struct gap_dev_name name;
    /// Appearance Icon
    //@trc_union parent.req == GAPC_DEV_APPEARANCE
    uint16_t            appearance;
    /// Slave preferred parameters
    //@trc_union parent.req == GAPC_DEV_SLV_PREF_PARAMS
    gap_slv_pref_t      slv_pref_params;
    /// Central address resolution
    //@trc_union parent.req == GAPC_DEV_CTL_ADDR_RESOL
    uint8_t             ctl_addr_resol;
    /// Database Hash value
    //@trc_union parent.req == GAPC_DEV_DB_HASH
    uint8_t             hash[16];
    /// Resolvable Private address only
    //@trc_union parent.req == GAPC_DEV_RSLV_PRIV_ADDR_ONLY
    uint8_t             rslv_priv_addr_only;
};

/// Peer device attribute DB info such as Device Name, Appearance or Slave Preferred Parameters
/*@TRACE*/
struct gapc_peer_att_info_ind
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    /// - GAPC_DEV_SLV_PREF_PARAMS: Device Slave preferred parameters
    /// - GAPC_DEV_CTL_ADDR_RESOL: Address resolution supported
    /// - GAPC_DEV_DB_HASH: Device Database Hash value
    /// - GAPC_GET_RSLV_PRIV_ADDR_ONLY: Resolvable Private Address Only declaration
    uint8_t  req;
    /// Attribute handle
    uint16_t handle;

    /// device information data
    union gapc_dev_info_val info;
};

/// Indication of peer version info
/*@TRACE*/
struct gapc_peer_version_ind
{
    /// Manufacturer name
    uint16_t compid;
    /// LMP subversion
    uint16_t lmp_subvers;
    /// LMP version
    uint8_t  lmp_vers;
};

/// Indication of peer features info
/*@TRACE*/
struct gapc_peer_features_ind
{
    /// 8-byte array for LE features
    uint8_t features[GAP_LE_FEATS_LEN];
};

/// Indication of ongoing connection RSSI
/*@TRACE*/
struct gapc_con_rssi_ind
{
    /// RSSI value
    int8_t rssi;
};

/// Indication of ongoing connection Channel Map
/*@TRACE*/
struct gapc_con_channel_map_ind
{
    /// channel map value
    le_chnl_map_t ch_map;
};

/// Sign counter value changed due to packet signing or signature verification.
struct gapc_sign_counter_updated_ind
{
    /// New Local signature counter value
    uint32_t lsign_counter;
    /// New Remote signature counter value
    uint32_t rsign_counter;
};

/// Indication of LE Ping
/*@TRACE*/
struct gapc_le_ping_to_val_ind
{
    ///Authenticated payload timeout
    uint16_t timeout;
};

/// Peer device request local device info such as name, appearance or slave preferred parameters
/*@TRACE*/
struct gapc_get_dev_info_req_ind
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    /// - GAPC_DEV_SLV_PREF_PARAMS: Device Slave preferred parameters
    uint8_t                 req;
    /// Token value that must be returned in confirmation
    uint16_t                token;
    /// Device Name data offset
    uint16_t                name_offset;
    /// Maximum name length (starting from offset)
    uint16_t                max_name_length;
};

/// Send requested info to peer device
/*@TRACE*/
struct gapc_get_dev_info_cfm
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    /// - GAPC_DEV_SLV_PREF_PARAMS: Device Slave preferred parameters
    uint8_t                 req;
    /// Status code used to know if requested has been accepted or not
    uint8_t                 status;
    /// Token value provided in request indication
    uint16_t                token;
    /// Complete value length including offset
    uint16_t                complete_length;
    /// Peer device information data
    union gapc_dev_info_val info;
};

/// device information data
/*@TRACE
 @trc_ref gapc_dev_info*/
union gapc_set_dev_info
{
    /// Device name
    //@trc_union parent.req == GAPC_DEV_NAME
    struct gap_dev_name     name;
    /// Appearance Icon
    //@trc_union parent.req == GAPC_DEV_APPEARANCE
    uint16_t                appearance;
};

/// Peer device request to modify local device info such as name or appearance
/*@TRACE*/
struct gapc_set_dev_info_req_ind
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    uint8_t                 req;
    /// Token value that must be returned in confirmation
    uint16_t                token;
    /// device information data
    union gapc_set_dev_info info;
};

/// Local device accept or reject device info modification
/*@TRACE*/
struct gapc_set_dev_info_cfm
{
    /// Requested information
    /// - GAPC_DEV_NAME: Device Name
    /// - GAPC_DEV_APPEARANCE: Device Appearance Icon
    uint8_t                 req;
    /// Status code used to know if requested has been accepted or not
    uint8_t                 status;
    /// Token value provided in request indication
    uint16_t                token;
};

/// Connection Parameter used to update connection parameters
struct gapc_conn_param
{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
};

/// Perform update of connection parameters command
/*@TRACE*/
struct gapc_param_update_cmd
{
    /// GAP request type:
    /// - GAPC_UPDATE_PARAMS: Perform update of connection parameters.
    uint8_t operation;
    /// Internal parameter used to manage internally l2cap packet identifier for signaling
    uint8_t pkt_id;
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
    /// Minimum Connection Event Duration
    uint16_t ce_len_min;
    /// Maximum Connection Event Duration
    uint16_t ce_len_max;
};

/// Request of updating connection parameters indication
/*@TRACE*/
struct gapc_param_update_req_ind
{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
};

/// Connection parameters updated indication
/*@TRACE*/
struct gapc_param_updated_ind
{
    ///Connection interval value
    uint16_t            con_interval;
    ///Connection latency value
    uint16_t            con_latency;
    ///Supervision timeout
    uint16_t            sup_to;
};

/// Master confirm or not that parameters proposed by slave are accepted or not
/*@TRACE*/
struct gapc_param_update_cfm
{
    /// True to accept slave connection parameters, False else.
    bool accept;
    /// Minimum Connection Event Duration
    uint16_t ce_len_min;
    /// Maximum Connection Event Duration
    uint16_t ce_len_max;
};

/// Parameters of the @ref GAPC_SET_PREF_SLAVE_LATENCY_CMD message
/*@TRACE*/
struct gapc_set_pref_slave_latency_cmd
{
    /// GAP request type:
    /// - GAPC_SET_PREF_SLAVE_LATENCY_CMD : Set preferred slave latency
    uint8_t operation;
    /// Preferred latency that the controller should use on a connection (in number of connection events)
    uint16_t latency;
};

/// Parameters of the @ref GAPC_SET_PREF_SLAVE_EVT_DUR_CMD message
/*@TRACE*/
struct gapc_set_pref_slave_evt_dur_cmd
{
    /// GAP request type:
    /// - GAPC_SET_PREF_SLAVE_EVT_DUR_CMD : Set preferred slave event duration
    uint8_t operation;
    /// Preferred event duration that the controller should use on a connection (N * 0.625 ms)
    uint16_t duration;
    /// Slave transmits a single packet per connection event (False/True)
    uint8_t single_tx;
};

/// Parameters of the @ref GAPC_SET_MAX_RX_SIZE_AND_TIME_CMD message
/*@TRACE*/
struct gapc_set_max_rx_size_and_time_cmd
{
    /// GAP request type:
    /// - GAPC_SET_MAX_RX_SIZE_AND_TIME: Set maximum RX size and time using DLE negotiation
    uint8_t  operation;
    /// Maximum RX size (in Bytes)
    uint16_t rx_octets;
    /// Maximum RX time (in us)
    uint16_t rx_time;
};


/// Parameters of the @ref GAPC_PER_ADV_SYNC_TRANS_CMD message
/*@TRACE*/
struct gapc_per_adv_sync_trans_cmd
{
    /// GAP request type:
    /// - GAPC_PER_ADV_SYNC_TRANS : Periodic Advertising Sync Transfer
    uint8_t  operation;
    /// Periodic Advertising or Periodic Sync activity index
    uint8_t  actv_idx;
    /// A value provided by application
    uint16_t service_data;
};

/// Parameters of the @ref GAPC_CTE_TX_CFG_CMD message
/*@TRACE*/
struct gapc_cte_tx_cfg_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_TX_CFG: Constant Tone Extension Transmission configuration
    uint8_t  operation;
    /// CTE types (bit0: AOA | bit1: AOD-1us | bit2: AOD-2us) (@see enum gap_cte_type)
    uint8_t  cte_types;
    /// Length of switching pattern (number of antenna IDs in the pattern)
    uint8_t  switching_pattern_len;
    /// Antenna IDs
    uint8_t  antenna_id[__ARRAY_EMPTY];
};

/// Parameters of the @ref GAPC_CTE_RX_CFG_CMD message
/*@TRACE*/
struct gapc_cte_rx_cfg_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_RX_CFG: Constant Tone Extension Reception configuration
    uint8_t  operation;
    /// Sampling enable
    bool     sample_en;
    /// Slot durations (1: 1us | 2: 2us)
    uint8_t  slot_dur;
    /// Length of switching pattern (number of antenna IDs in the pattern)
    uint8_t  switching_pattern_len;
    /// Antenna IDs
    uint8_t  antenna_id[__ARRAY_EMPTY];
};

/// Parameters of the @ref GAPC_CTE_REQ_CTRL_CMD message
/*@TRACE*/
struct gapc_cte_req_ctrl_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_REQ_CTRL: Constant Tone Extension request control (enable / disable)
    uint8_t  operation;
    /// True to enable TX or RX Constant Tone Extension, False to disable
    bool     enable;
    /// CTE request interval (in number of connection events)
    uint16_t interval;
    /// Requested CTE length (in 8us unit)
    uint8_t  cte_len;
    /// Requested CTE type (0: AOA | 1: AOD-1us | 2: AOD-2us)  (@see enum gap_cte_type)
    uint8_t  cte_type;
};

/// Parameters of the @ref GAPC_CTE_RSP_CTRL_CMD message
/*@TRACE*/
struct gapc_cte_rsp_ctrl_cmd
{
    /// GAP request type:
    /// - GAPC_CTE_RSP_CTRL: Constant Tone Extension response control (enable / disable)
    uint8_t  operation;
    /// True to enable TX or RX Constant Tone Extension, False to disable
    bool     enable;
};

/// Indicate reception of a IQ Report event over a BLE connection
/*@TRACE*/
struct gapc_cte_iq_report_ind
{
    /// Rx PHY  (@see enum gap_phy_val)
    uint8_t  rx_phy;
    /// Data channel index
    uint8_t  data_channel_idx;
    /// RSSI (in 0.1 dBm)
    int16_t  rssi;
    /// RSSI antenna ID
    uint8_t  rssi_antenna_id;
    /// CTE type (0: AOA | 1: AOD-1us | 2: AOD-2us) (@see enum gap_cte_type)
    uint8_t  cte_type;
    /// Slot durations (1: 1us | 2: 2us)
    uint8_t  slot_dur;
    /// Packet status
    uint8_t  pkt_status;
    /// Connection event counter
    uint16_t con_evt_cnt;
    /// Number of samples
    uint8_t  nb_samples;
    /// I/Q sample
    struct gap_iq_sample sample[__ARRAY_EMPTY];
};

/// Pairing parameters
/*@TRACE*/
struct gapc_pairing
{
    /// IO capabilities (@see gap_io_cap)
    uint8_t iocap;
    /// OOB information (@see gap_oob)
    uint8_t oob;
    /// Authentication (@see gap_auth)
    /// Note in BT 4.1 the Auth Field is extended to include 'Key Notification' and
    /// and 'Secure Connections'.
    uint8_t auth;
    /// Encryption key size (7 to 16)
    uint8_t key_size;
    ///Initiator key distribution (@see gap_kdist)
    uint8_t ikey_dist;
    ///Responder key distribution (@see gap_kdist)
    uint8_t rkey_dist;

    /// Device security requirements (minimum security level). (@see gap_sec_req)
    uint8_t sec_req;
};

/// Long Term Key information
/*@TRACE*/
struct gapc_ltk
{
    /// Long Term Key
    gap_sec_key_t ltk;
    /// Encryption Diversifier
    uint16_t ediv;
    /// Random Number
    rand_nb_t randnb;
    /// Encryption key size (7 to 16)
    uint8_t key_size;
};
/// Out of Band Information
/*@TRACE*/
struct gapc_oob
{
    /// Confirm Value
    uint8_t conf[GAP_KEY_LEN];
    /// Random Number
    uint8_t rand[GAP_KEY_LEN];
};

/*@TRACE*/
struct gapc_nc
{
    uint8_t value[4];
};

/// Identity Resolving Key information
/*@TRACE*/
struct gapc_irk
{
    /// Identity Resolving Key
    gap_sec_key_t irk;
    /// Device BD Identity Address
    gap_bdaddr_t addr;
};


/// Start Bonding command procedure
/*@TRACE*/
struct gapc_bond_cmd
{
    /// GAP request type:
    /// - GAPC_BOND:  Start bonding procedure.
    uint8_t operation;
    /// Pairing information
    struct gapc_pairing pairing;
};

/// Bond procedure requested information data
/*@TRACE
 @trc_ref gapc_bond*/
union gapc_bond_req_data
{
    /// Authentication level (@see gap_auth) (if request = GAPC_PAIRING_REQ)
    //@trc_union parent.request == GAPC_PAIRING_REQ
    uint8_t auth_req;
    /// LTK Key Size (if request = GAPC_LTK_EXCH)
    //@trc_union parent.request == GAPC_LTK_EXCH
    uint8_t key_size;
    /// Device IO used to get TK: (if request = GAPC_TK_EXCH)
    ///  - GAP_TK_OOB:       TK get from out of band method
    ///  - GAP_TK_DISPLAY:   TK generated and shall be displayed by local device
    ///  - GAP_TK_KEY_ENTRY: TK shall be entered by user using device keyboard
    //@trc_union parent.request == GAPC_TK_EXCH
    uint8_t tk_type;

    /// Addition OOB Data for the OOB Conf and Rand values
    //@trc_union parent.request == GAPC_OOB_EXCH
    struct gapc_oob  oob_data;
    /// Numeric Comparison Data
    //@trc_union parent.request == GAPC_NC_EXCH
    struct gapc_nc   nc_data;
};

/// Bonding requested by peer device indication message.
/*@TRACE*/
struct gapc_bond_req_ind
{
    /// Bond request type (@see gapc_bond)
    uint8_t request;

    /// Bond procedure requested information data
    union gapc_bond_req_data data;
};

/*@TRACE
 @trc_ref gapc_bond
*/
union gapc_bond_cfm_data
{
    /// Pairing Features (request = GAPC_PAIRING_RSP)
    //@trc_union parent.request == GAPC_PAIRING_RSP
    struct gapc_pairing pairing_feat;
    /// LTK (request = GAPC_LTK_EXCH)
    //@trc_union parent.request == GAPC_LTK_EXCH
    struct gapc_ltk ltk;
    /// CSRK (request = GAPC_CSRK_EXCH)
    //@trc_union parent.request == GAPC_CSRK_EXCH
    gap_sec_key_t csrk;
    /// TK (request = GAPC_TK_EXCH)
    //@trc_union parent.request == GAPC_TK_EXCH
    gap_sec_key_t tk;
    /// IRK (request = GAPC_IRK_EXCH)
    //@trc_union parent.request == GAPC_IRK_EXCH
    struct gapc_irk irk;
    /// OOB Confirm and Random from the peer (request = GAPC_OOB_EXCH)
    //@trc_union parent.request == GAPC_OOB_EXCH
    struct gapc_oob oob;
};

/// Confirm requested bond information.
/*@TRACE*/
struct gapc_bond_cfm
{
    /// Bond request type (@see gapc_bond)
    uint8_t request;
    /// Request accepted
    uint8_t accept;

    /// Bond procedure information data
    union gapc_bond_cfm_data data;
};

/**
 *  Pairing information
 */
/*@TRACE*/
struct gapc_pairing_info
{
    /// Pairing level information (@see gap_pairing_lvl)
    uint8_t level;
    /// LTK exchanged during pairing.
    bool    ltk_present;
};

/// Bond procedure information data
/*@TRACE
 @trc_ref gapc_bond*/
union gapc_bond_data
{
    /// Pairing information
    /// (if info = GAPC_PAIRING_SUCCEED)
    //@trc_union parent.info == GAPC_PAIRING_SUCCEED
    struct gapc_pairing_info pairing;
    /// Pairing failed reason  (if info = GAPC_PAIRING_FAILED)
    //@trc_union parent.info == GAPC_PAIRING_FAILED
    uint8_t reason;
    /// Long Term Key information (if info = GAPC_LTK_EXCH)
    //@trc_union parent.info == GAPC_LTK_EXCH
    struct gapc_ltk ltk;
    /// Identity Resolving Key information (if info = GAPC_IRK_EXCH)
    //@trc_union parent.info == GAPC_IRK_EXCH
    struct gapc_irk irk;
    /// Connection Signature Resolving Key information (if info = GAPC_CSRK_EXCH)
    //@trc_union parent.info == GAPC_CSRK_EXCH
    gap_sec_key_t csrk;
};

/// Bonding information indication message
/*@TRACE*/
struct gapc_bond_ind
{
    /// Bond information type (@see gapc_bond)
    uint8_t info;

    /// Bond procedure information data
    union gapc_bond_data data;
};

/// Start Encryption command procedure
/*@TRACE*/
struct gapc_encrypt_cmd
{
    /// GAP request type:
    /// - GAPC_ENCRYPT:  Start encryption procedure.
    uint8_t operation;
    /// Long Term Key information
    struct gapc_ltk ltk;
};

/// Encryption requested by peer device indication message.
/*@TRACE*/
struct gapc_encrypt_req_ind
{
    /// Encryption Diversifier
    uint16_t ediv;
    /// Random Number
    rand_nb_t rand_nb;
};

/// Confirm requested Encryption information.
/*@TRACE*/
struct gapc_encrypt_cfm
{
    /// Indicate if a LTK has been found for the peer device
    uint8_t found;
    /// Long Term Key
    gap_sec_key_t ltk;
    /// LTK Key Size
    uint8_t key_size;
};

/// Encryption information indication message
/*@TRACE*/
struct gapc_encrypt_ind
{
    /// Pairing level (@see gap_pairing_lvl)
    uint8_t pairing_lvl;
};

/// Start Security Request command procedure
/*@TRACE*/
struct gapc_security_cmd
{
    /// GAP request type:
    /// - GAPC_SECURITY_REQ: Start security request procedure
    uint8_t operation;
    /// Authentication level (@see gap_auth)
    uint8_t auth;
};
/// Security requested by peer device indication message
/*@TRACE*/
struct gapc_security_ind
{
    /// Authentication level (@see gap_auth)
    uint8_t auth;
};

/// Parameters of the @ref GAPC_BOND_DATA_UPDATE_IND message
/*@TRACE*/
struct gapc_bond_data_update_ind
{
    /// Local SignCounter value
    uint32_t local_sign_counter;
    /// Peer SignCounter value
    uint32_t peer_sign_counter;
    /// Peer GATT Service Start handle
    uint16_t gatt_start_hdl;
    /// Peer GATT Service End Handle
    uint16_t gatt_end_hdl;
    /// Peer Service Change value handle
    uint16_t svc_chg_hdl;
    /// Client bond data information (@see enum gapc_cli_info)
    uint8_t  cli_info;
    /// Client supported features    (@see enum gapc_cli_feat)
    uint8_t  cli_feat;
    /// Server supported features    (@see enum gapc_srv_feat)
    uint8_t  srv_feat;
};


/// Parameters of the @ref GAPC_SET_LE_PING_TO_CMD message
/*@TRACE*/
struct gapc_set_le_ping_to_cmd
{
    /// GAP request type:
    /// - GAPC_SET_LE_PING_TO : Set the LE Ping timeout value
    uint8_t operation;
    /// Authenticated payload timeout
    uint16_t timeout;
};

/// Parameters of the @ref GAPC_SET_LE_PKT_SIZE_CMD message
/*@TRACE*/
struct gapc_set_le_pkt_size_cmd
{
    /// GAP request type:
    /// - GAPC_SET_LE_PKT_SIZE : Set the LE Data length value
    uint8_t operation;
    ///Preferred maximum number of payload octets that the local Controller should include
    ///in a single Link Layer Data Channel PDU.
    uint16_t tx_octets;
    ///Preferred maximum number of microseconds that the local Controller should use to transmit
    ///a single Link Layer Data Channel PDU
    uint16_t tx_time;
};

/// Parameters of the @ref GAPC_LE_PKT_SIZE_IND message
/*@TRACE*/
struct gapc_le_pkt_size_ind
{
    ///The maximum number of payload octets in TX
    uint16_t max_tx_octets;
    ///The maximum time that the local Controller will take to TX
    uint16_t max_tx_time;
    ///The maximum number of payload octets in RX
    uint16_t max_rx_octets;
    ///The maximum time that the local Controller will take to RX
    uint16_t max_rx_time;
};

/// Parameters of the @ref GAPC_KEY_PRESS_NOTIFICATION_CMD message
/*@TRACE*/
struct gapc_key_press_notif_cmd
{
    /// GAP request type:
    /// - GAPC_KEY_PRESS_NOTIFICATION_CMD : Inform the remote device when keys have been entered or erased
    uint8_t operation;
    /// notification type
    uint8_t notification_type;
};

/// Parameters of the @ref GAPC_KEY_PRESS_NOTIFICATION_IND message
/*@TRACE*/
struct gapc_key_press_notif_ind
{
    /// notification type
    uint8_t notification_type;
};

/// Set the PHY configuration for current active link
/*@TRACE*/
struct gapc_set_phy_cmd
{
    /// GAP request type:
    /// - GAPC_SET_PHY : Set the PHY configuration for current active link
    uint8_t operation;
    /// Supported LE PHY for data transmission (@see enum gap_phy)
    uint8_t tx_phy;
    /// Supported LE PHY for data reception (@see enum gap_phy)
    uint8_t rx_phy;
    /// PHY options (@see enum gapc_phy_option)
    uint8_t phy_opt;
};

/// Active link PHY configuration. Triggered when configuration is read or during an update.
/*@TRACE*/
struct gapc_le_phy_ind
{
    /// LE PHY for data transmission (@see enum gap_phy)
    uint8_t tx_phy;
    /// LE PHY for data reception (@see enum gap_phy)
    uint8_t rx_phy;
};

/// Parameters of the @ref GAPC_CHAN_SEL_ALGO_IND
/*@TRACE*/
struct gapc_chan_sel_algo_ind
{
    /// Used channel selection algorithm
    uint8_t chan_sel_algo;
};

/// Local TX power indication
/*@TRACE*/
typedef struct gapc_loc_tx_pwr_ind
{
    /// PHY (@see enum gapc_phy_pwr_value)
    uint8_t        phy;
    /// Current transmit power level (dBm)
    int8_t         tx_pwr;
    /// Max transmit power level (dBm)
    int8_t         max_tx_pwr;
} gapc_loc_tx_pwr_ind_t;

/// Remote TX power indication
/*@TRACE*/
typedef struct gapc_peer_tx_pwr_ind
{
    /// PHY (@see enum gapc_phy_pwr_value)
    uint8_t        phy;
    /// Transmit Power level (dBm)
    int8_t         tx_pwr;
    /// Transmit Power level flags (@see enum gapc_pwr_ctrl_flags)
    uint8_t        flags;
} gapc_peer_tx_pwr_ind_t;

/// Control TX Power Reports command
/*@TRACE*/
typedef struct gapc_tx_pwr_report_ctrl_cmd
{
    /// GAP request type:
    /// - GAPC_TX_PWR_REPORTING_CTRL: Enable or disable the reporting to the local Host of transmit power level
    ///                               changes in the local and remote Controllers for the ACL connection
    uint8_t         operation;
    /// 1 To Enable local power changes reporting, 0 to disable.
    uint8_t         local_en;
    /// 1 To Enable remote power changes reporting, 0 to disable.
    uint8_t         remote_en;

} gapc_tx_pwr_report_ctrl_cmd_t;

/// Local TX power change report indication
/*@TRACE*/
typedef struct gapc_loc_tx_pwr_report_ind
{
    /// PHY (@see enum gapc_phy_pwr_value)
    uint8_t        phy;
    /// Transmit Power level (dBm)
    int8_t         tx_pwr;
    /// Transmit Power level flags (@see enum gapc_pwr_ctrl_flags)
    uint8_t        flags;
    /// Delta (dB)
    int8_t         delta;
} gapc_loc_tx_pwr_report_ind_t;

/// Remote TX power change report indication
/*@TRACE*/
typedef struct gapc_peer_tx_pwr_report_ind
{
    /// PHY (@see enum gapc_phy_pwr_value)
    uint8_t        phy;
    /// Transmit Power level (dBm)
    int8_t         tx_pwr;
    /// Transmit Power level flags (@see enum gapc_pwr_ctrl_flags)
    uint8_t        flags;
    /// Delta (dB)
    int8_t         delta;
} gapc_peer_tx_pwr_report_ind_t;


/// Control Path loss configuration
/*@TRACE*/
typedef struct gapc_path_loss_ctrl_cmd
{
    /// GAP request type:
    /// - GAPC_PATH_LOSS_REPORT_CTRL: Command is used to enable/disable path loss reporting for the connection
    uint8_t         operation;
    /// 1 To Enable reporting, 0 to disable.
    uint8_t         enable;
    /// High threshold (dB)
    uint8_t         high_threshold;
    /// High hysteresis (dB)
    uint8_t         high_hysteresis;
    /// Low threshold (dB)
    uint8_t         low_threshold;
    /// Low hysteresis (dB)
    uint8_t         low_hysteresis;
    /// Min time spent (conn events)
    uint16_t        min_time;

} gapc_path_loss_ctrl_cmd_t;

/// Path Loss Threshold Indication
/*@TRACE*/
typedef struct gapc_path_loss_threshold_ind
{
    /// Current path loss (dB)
    uint8_t        curr_path_loss;
    /// Zone entered (@see enum gapc_path_loss_zone)
    uint8_t        zone_entered;
} gapc_path_loss_threshold_ind_t;

/// @} GAPC_MSG

#endif /* _GAPC_MSG_H_ */
