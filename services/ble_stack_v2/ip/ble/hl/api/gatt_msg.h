/**
 ****************************************************************************************
 *
 * @file gatt_msg.h
 *
 * @brief Header file - GATT Message API.
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef GATT_MSG_H_
#define GATT_MSG_H_

/**
 ****************************************************************************************
 * @addtogroup GATT_MSG Message API
 * @ingroup GATT
 * @brief Handles ALL messages to/from GATT block.
 *
 * The GATT task is responsible for Attribute Client and service exchange over a BLE link.
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
/// Length of an handle
#define GATT_HANDLE_LEN      (2)
/// Length of 16-bit UUID in octets
#define GATT_UUID_16_LEN     (2)
/// Length of 32-bit UUID in octets
#define GATT_UUID_32_LEN     (4)
/// Length of 128-bit UUID in octets
#define GATT_UUID_128_LEN    (16)
/// Length of Database Hash in octets
#define GATT_DB_HASH_LEN     (16)

/*
 * MESSAGES
 ****************************************************************************************
 */

/// Message API of the GATT task
/*@TRACE*/
enum gatt_msg_id
{
    /// GATT command
    GATT_CMD                = MSG_ID(GATT, 0x00),
    /// GATT command complete event
    GATT_CMP_EVT            = MSG_ID(GATT, 0x01),
    /// GATT Indication
    GATT_IND                = MSG_ID(GATT, 0x02),
    /// GATT request indication
    GATT_REQ_IND            = MSG_ID(GATT, 0x03),
    /// GATT confirmation
    GATT_CFM                = MSG_ID(GATT, 0x04),
};


/// GATT_CMD command codes
/*@TRACE*/
enum gatt_cmd_code
{
    /* --- GATT User Management --- */
    /// Command used to register a GATT user. This must be done prior to any GATT procedure execution.
    GATT_USER_REGISTER                      = 0x0000,
    /// Command used to unregister a GATT user (client or server).
    GATT_USER_UNREGISTER                    = 0x0001,

    /* --- GATT Server Database Management --- */
    /// Command used to add a service into local attribute database.
    GATT_DB_SVC_ADD                         = 0x0100,
    /// Command used to remove a service from local attribute database.
    GATT_DB_SVC_REMOVE                      = 0x0101,
    /// Command used to control visibility and usage authorization of a local service.
    /// A hidden service is present in database but cannot be discovered or manipulated by a peer device.
    /// A disabled service can be discovered by a peer device but it is not authorized to use it.
    GATT_DB_SVC_CTRL                        = 0x0102,
    /// Command used to retrieve (or compute) the local database hash value.
    GATT_DB_HASH_GET                        = 0x0103,

    /* --- GATT Server initiated procedure --- */
    /// Command used by a GATT server user to send notifications or indications for some attributes values to peer device.
    /// Number of attributes must be set to one for GATT_INDICATE event type.
    /// This command is consider reliable because GATT user is aware of maximum packet  size that can be transmitted
    /// over the air.
    GATT_SRV_EVENT_RELIABLE_SEND            = 0x0200,
    /// Command used by a GATT server user to send notifications or indications for one attribute value to peer device.
    GATT_SRV_EVENT_SEND                     = 0x0201,
    /// Command used by a GATT server user to send notifications or indications for one attribute value to multiple peer devices
    GATT_SRV_EVENT_MTP_SEND                 = 0x0202,
    /// Command used by a GATT server to cancel an on-going multipoint event transmission
    GATT_SRV_EVENT_MTP_CANCEL               = 0x0203,

    /* --- GATT Client initiated procedure --- */
    /// Command used by a GATT client user to discover primary or secondary services, exposed by peer device in its
    /// attribute database. All services can be discovered or filtering services having a specific UUID.
    GATT_CLI_DISCOVER_SVC                   = 0x0300,
    /// Command used by a GATT client user to discover included services, exposed by peer device in its attribute database.
    GATT_CLI_DISCOVER_INC_SVC               = 0x0301,
    /// Command used by a GATT client user to discover all or according to a specific UUID characteristics exposed by
    /// peer device in its attribute database.
    GATT_CLI_DISCOVER_CHAR                  = 0x0302,
    /// Command used by a GATT client user to discover characteristic descriptor exposed by peer device in its
    /// attribute database.
    GATT_CLI_DISCOVER_DESC                  = 0x0303,
    /// Command used by a GATT client user to cancel an on-going discovery procedure. The dummy parameter in the
    /// request must be equals to dummy parameter used for service discovery command.
    /// The discovery is aborted as soon as on-going discovery attribute transaction is over.
    GATT_CLI_DISCOVER_CANCEL                = 0x0304,
    /// Command used by a GATT client user to read value of an attribute (identified by its handle) present in peer
    /// database.
    GATT_CLI_READ                           = 0x0305,
    /// Command used by a GATT client user to read value of an attribute with a given UUID in peer database.
    GATT_CLI_READ_BY_UUID                   = 0x0306,
    /// Command used by a GATT client user to read multiple attribute at the same time. If one of attribute length is
    /// unknown, the read multiple variable length procedure is used.
    GATT_CLI_READ_MULTIPLE                  = 0x0307,
    /// Command used by a GATT client user to request to write value of an attribute in peer database.
    /// This command is consider reliable because GATT user is aware of maximum packet size that can be transmitted
    /// over the air
    GATT_CLI_WRITE_RELIABLE                 = 0x0308,
    /// Command used by a GATT client user to request to write value of an attribute in peer database.
    /// Since user is not aware of MTU size of the bearer used for attribute transmission it cannot be considered reliable.
    /// For a GATT_WRITE_NO_RESP if attribute bearer max transmission size isn't sufficient, a GATT_WRITE (with
    /// response) procedure will be used.
    /// For a GATT_WRITE_SIGNED, if attribute bearer max transmission size isn't sufficient, the procedure is aborted
    /// with L2CAP_ERR_INVALID_MTU error code.
    GATT_CLI_WRITE                          = 0x0309,
    /// Command used by a GATT client user to request peer server to execute prepare write queue.
    GATT_CLI_WRITE_EXE                      = 0x030A,
    /// Command used by a GATT client user to register for reception of events (notification / indication) for a
    /// given handle range.
    GATT_CLI_EVENT_REGISTER                 = 0x030B,
    /// Command used by a GATT client user to stop reception of events (notification / indication) onto a specific
    /// handle range.
    GATT_CLI_EVENT_UNREGISTER               = 0x030C,
    /// Command used to update MTU to a specific value on legacy attribute bearer
    GATT_CLI_MTU_UPDATE                     = 0x030D,

    /* --- Debug only --- */
    /// Debug command used to remove all services from local attribute database.
    GATT_DBG_DB_SVC_REMOVE_ALL              = 0x0904,
    /// Debug command used to get information about all services present in attribute database.
    GATT_DBG_DB_SVC_LIST_GET                = 0x0905,
    /// Debug command used to update service security level permission or to enable/disable a service.
    /// The UUID length information cannot be updated.
    GATT_DBG_DB_SVC_INFO_SET                = 0x0906,
    /// Debug command used to retrieve information of an attribute.
    GATT_DBG_DB_ATT_INFO_GET                = 0x0907,
    /// Debug command used to set information of an attribute.
    GATT_DBG_DB_ATT_INFO_SET                = 0x0908,
    /// Debug command used to get information about active bearer
    GATT_DBG_BEARER_INFO_GET                = 0x0909,
    /// Debug command used to close a specific bearer
    GATT_DBG_BEARER_CLOSE                   = 0x090A,
    /// Debug command used to force establishment of EATT bearers
    GATT_DBG_BEARER_EATT_ESTAB              = 0x090B,
};


/// GATT_IND indication codes
/*@TRACE*/
enum gatt_ind_code
{
    /// Event triggered when an unknown message has been received by GATT task.
    GATT_UNKNOWN_MSG                    = 0x0000,
    /* --- Client Message --- */
    /// Event triggered when service discovery procedure has information to provide about a service.
    /// To prevent memory overflow during discovery procedure, service information can be split into several events.
    /// When receiving start or complete service information, this means new service, first attribute can be either
    /// GATT_ATT_PRIMARY_SVC or GATT_ATT_SECONDARY_SVC
    /// If start receive discovery information is present in message, GATT user should wait for new GATT_CLI_SVC
    /// message with a continue service or end service discovery information.
    GATT_CLI_SVC                        = 0x0300,
    /// Event triggered when a service is found during service discovery procedure - Only Service information.
    GATT_CLI_SVC_INFO                   = 0x0301,
    /// Event triggered when an included service is found during discovery procedure.
    GATT_CLI_INC_SVC                    = 0x0302,
    /// Event triggered when a characteristic is found during discovery procedure.
    GATT_CLI_CHAR                       = 0x0303,
    /// Event triggered when a characteristic descriptor is found during discovery procedure.
    GATT_CLI_DESC                       = 0x0304,
    /// Event triggered when an attribute value has been read.
    GATT_CLI_ATT_VAL                    = 0x0305,
    /// Event triggered when a service change has been received or if an attribute transaction triggers an
    /// out of sync error.
    GATT_CLI_SVC_CHANGED                = 0x0306,
};

/// GATT_REQ_IND request indication codes
/*@TRACE*/
enum gatt_req_ind_code
{
    /* --- GATT Server User --- */
    /// Event triggered by GATT when peer device requests to read an attribute.
    GATT_SRV_ATT_READ_GET               = 0x0200,
    /// Event triggered by GATT when user requires to send either a notification or an indication.
    GATT_SRV_ATT_EVENT_GET              = 0x0201,
    /// Event triggered by GATT in order to ask GATT server user to provide information about a specific attribute value.
    GATT_SRV_ATT_INFO_GET               = 0x0202,
    /// Event triggered by GATT in order to inform GATT server user when an attribute value has been written by a
    /// peer device.
    GATT_SRV_ATT_VAL_SET                = 0x0203,
    /* --- GATT Client User --- */
    /// Event triggered by GATT in order to ask GATT client user to provide specific attribute value.
    /// This event can be trigger if GATT client user requests to update peer device attribute value using write
    /// procedure.
    GATT_CLI_ATT_VAL_GET                = 0x0300,
    /// Inform GATT client user about reception of either a notification or an indication from peer device.
    GATT_CLI_ATT_EVENT                  = 0x0301,
};

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// GATT User Role
enum gatt_user_role
{
    /// Client user role
    GATT_ROLE_CLIENT = 0x00,
    /// Server user role
    GATT_ROLE_SERVER = 0x01,
    /// Role not defined
    GATT_ROLE_NONE   = 0xFF,
};

/// GATT service discovery information
enum gatt_svc_disc_info
{
    /// Complete service present in structure
    GATT_SVC_CMPLT = 0x00,
    /// First service attribute present in structure
    GATT_SVC_START = 0x01,
    /// Last service attribute present in structure
    GATT_SVC_END   = 0x02,
    /// Following service attribute present in structure
    GATT_SVC_CONT  = 0x03,
};

///  GATT UUID Type
enum gatt_uuid_type
{
    /// 16-bit UUID value
    GATT_UUID_16      = 0x00,
    /// 32-bit UUID value
    GATT_UUID_32      = 0x01,
    /// 128-bit UUID value
    GATT_UUID_128     = 0x02,
    /// Invalid UUID Type
    GATT_UUID_INVALID = 0x03,
};

/// GATT attribute security level
enum gatt_sec_lvl
{
    /// Attribute value is accessible on an unencrypted link.
    GATT_SEC_NOT_ENC    = 0x00,
    /// Attribute value is accessible on an encrypted link or modified with using write signed procedure
    /// on unencrypted link if bonded using an unauthenticated pairing.
    GATT_SEC_NO_AUTH    = 0x01,
    /// Attribute value is accessible on an encrypted link or modified with using write signed procedure
    /// on unencrypted link if bonded using an authenticated pairing.
    GATT_SEC_AUTH       = 0x02,
    /// Attribute value is accessible on an encrypted link or modified with using write signed procedure
    /// on unencrypted link if bonded using a secure connection pairing.
    GATT_SEC_SECURE_CON = 0x03,
};

///   7      6     5     4      3     2    1   0
/// +-----+-----+-----+------+-----+-----+---+---+
/// | RFU | UUID_TYPE | HIDE | DIS | EKS |  AUTH |
/// +-----+-----+-----+------+-----+-----+---+---+
/// GATT Service information Bit Field
enum gatt_svc_info_bf
{
    /// Service minimum required security level (see enum gatt_sec_lvl).
    GATT_SVC_AUTH_MASK       = 0x03,
    GATT_SVC_AUTH_LSB        = 0,
    /// If set, access to value with encrypted security requirement also requires a 128-bit encryption key size.
    GATT_SVC_EKS_BIT         = 0x04,
    GATT_SVC_EKS_POS         = 2,
    /// If set, service is visible but cannot be used by peer device
    GATT_SVC_DIS_BIT         = 0x08,
    GATT_SVC_DIS_POS         = 3,
    /// Hide the service
    GATT_SVC_HIDE_BIT        = 0x10,
    GATT_SVC_HIDE_POS        = 4,
    /// Type of service UUID (@see enum gatt_uuid_type)
    GATT_SVC_UUID_TYPE_MASK  = 0x60,
    GATT_SVC_UUID_TYPE_LSB   = 5,
};

///    15   14    13  12 11 10  9  8   7    6    5   4   3    2    1    0
/// +-----+-----+---+---+--+--+--+--+-----+----+---+---+----+----+----+---+
/// | UUID_TYPE |  NIP  |  WP |  RP | EXT | WS | I | N | WR | WC | RD | B |
/// +-----+-----+---+---+--+--+--+--+-----+----+---+---+----+----+----+---+
///                                  <--------------- PROP -------------->
/// GATT Attribute information Bit Field
enum gatt_att_info_bf
{
    /// Broadcast descriptor present
    GATT_ATT_B_BIT          = 0x0001,
    GATT_ATT_B_POS          = 0,
    /// Read Access Mask
    GATT_ATT_RD_BIT         = 0x0002,
    GATT_ATT_RD_POS         = 1,
    /// Write Command Enabled attribute Mask
    GATT_ATT_WC_BIT         = 0x0004,
    GATT_ATT_WC_POS         = 2,
    /// Write Request Enabled attribute Mask
    GATT_ATT_WR_BIT         = 0x0008,
    GATT_ATT_WR_POS         = 3,
    /// Notification Access Mask
    GATT_ATT_N_BIT          = 0x0010,
    GATT_ATT_N_POS          = 4,
    /// Indication Access Mask
    GATT_ATT_I_BIT          = 0x0020,
    GATT_ATT_I_POS          = 5,
    /// Write Signed Enabled attribute Mask
    GATT_ATT_WS_BIT         = 0x0040,
    GATT_ATT_WS_POS         = 6,
    /// Extended properties descriptor present
    GATT_ATT_EXT_BIT        = 0x0080,
    GATT_ATT_EXT_POS        = 7,
    /// Read security level permission (@see enum gatt_sec_lvl).
    GATT_ATT_RP_MASK        = 0x0300,
    GATT_ATT_RP_LSB         = 8,
    /// Write security level permission (@see enum gatt_sec_lvl).
    GATT_ATT_WP_MASK        = 0x0C00,
    GATT_ATT_WP_LSB         = 10,
    /// Notify and Indication security level permission (@see enum gatt_sec_lvl).
    GATT_ATT_NIP_MASK       = 0x3000,
    GATT_ATT_NIP_LSB        = 12,
    /// Type of attribute UUID (@see enum gatt_uuid_type)
    GATT_ATT_UUID_TYPE_MASK  = 0xC000,
    GATT_ATT_UUID_TYPE_LSB   = 14,

    /// Attribute value property
    GATT_ATT_PROP_MASK      = 0x00FF,
    GATT_ATT_PROP_LSB       = 0,
};


///       15     14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
/// +-----------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
/// | NO_OFFSET |               WRITE_MAX_SIZE               |
/// +-----------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
/// |                     INC_SVC_HANDLE                     |
/// +-----------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
/// |                     EXT_PROP_VALUE                     |
/// +-----------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
///
/// GATT Attribute extended information Bit Field
enum gatt_att_ext_info_bf
{
    /// Maximum value authorized for an attribute write.
    /// Automatically reduce to Maximum Attribute value (@see GATT_MAX_VALUE) if greater
    GATT_ATT_WRITE_MAX_SIZE_MASK  = 0x7FFF,
    GATT_ATT_WRITE_MAX_SIZE_LSB   = 0,
    /// 1: Do not authorize peer device to read or write an attribute with an offset != 0
    /// 0: Authorize offset usage
    GATT_ATT_NO_OFFSET_BIT        = 0x8000,
    GATT_ATT_NO_OFFSET_POS        = 15,
    /// Include Service handle value
    GATT_INC_SVC_HDL_BIT          = 0xFFFF,
    GATT_INC_SVC_HDL_POS          = 0,
    /// Characteristic Extended Properties value
    GATT_ATT_EXT_PROP_VALUE_MASK  = 0xFFFF,
    GATT_ATT_EXT_PROP_VALUE_LSB   = 0,
};


/// GATT Service Visibility
enum gatt_svc_visibility
{
    /// Service hidden
    GATT_SVC_HIDE       = 0x00,
    /// Service visible
    GATT_SVC_VISIBLE    = 0x01,
};

/// GATT Event Type
enum gatt_evt_type
{
    /// Server initiated notification
    GATT_NOTIFY     = 0x00,
    /// Server initiated indication
    GATT_INDICATE   = 0x01,
};

/// GATT Service Discovery Type
enum gatt_svc_discovery_type
{
    /// Discover all primary services
    GATT_DISCOVER_SVC_PRIMARY_ALL         = 0x00,
    /// Discover primary services using UUID value
    GATT_DISCOVER_SVC_PRIMARY_BY_UUID     = 0x01,
    /// Discover all secondary services
    GATT_DISCOVER_SVC_SECONDARY_ALL       = 0x02,
    /// Discover secondary services using UUID value
    GATT_DISCOVER_SVC_SECONDARY_BY_UUID   = 0x03,
};

/// GATT Characteristic Discovery Type
enum gatt_char_discovery_type
{
    /// Discover all characteristics
    GATT_DISCOVER_CHAR_ALL      = 0x00,
    /// Discover characteristics using UUID value
    GATT_DISCOVER_CHAR_BY_UUID  = 0x01,
};

/// Write execution mode
enum gatt_writ_mode
{
    /// Perform automatic write execution
    GATT_WRITE_MODE_AUTO_EXECUTE    = 0x00,
    /// Force to use prepare write queue. Can be used to write multiple attributes
    GATT_WRITE_MODE_QUEUE    = 0x01,
};

/// Service Discovery Attribute type
enum gatt_att_type
{
    /// No Attribute Information
    GATT_ATT_NONE           = 0x00,
    /// Primary service attribute
    GATT_ATT_PRIMARY_SVC    = 0x01,
    /// Secondary service attribute
    GATT_ATT_SECONDARY_SVC  = 0x02,
    /// Included service attribute
    GATT_ATT_INCL_SVC       = 0x03,
    /// Characteristic declaration
    GATT_ATT_CHAR           = 0x04,
    /// Attribute value
    GATT_ATT_VAL            = 0x05,
    /// Attribute descriptor
    GATT_ATT_DESC           = 0x06,
};

/// GATT Write Type
enum gatt_write_type
{
    /// Write attribute
    GATT_WRITE              = 0x00,
    /// Write attribute without response
    GATT_WRITE_NO_RESP      = 0x01,
    /// Write attribute signed
    GATT_WRITE_SIGNED       = 0x02,
};

/*
 * STRUCTURES
 ****************************************************************************************
 */

/// Attribute Description structure
/*@TRACE*/
typedef struct gatt_att_desc
{
    /// Attribute UUID (LSB First)
    uint8_t  uuid[GATT_UUID_128_LEN];
    /// Attribute information bit field (@see enum gatt_att_info_bf)
    uint16_t info;
    /// Attribute extended information bit field (@see enum gatt_att_ext_info_bf)
    /// Note:
    ///   - For Included Services and Characteristic Declarations, this field contains targeted handle.
    ///   - For Characteristic Extended Properties, this field contains 2 byte value
    ///   - For Client Characteristic Configuration and Server Characteristic Configuration, this field is not used.
    uint16_t ext_info;
} gatt_att_desc_t;

/// Service Description structure
/*@TRACE*/
typedef struct gatt_svc_desc
{
    /// GATT User Local identifier
    uint8_t  user_lid;
    /// Service Information bit field (@see enum gatt_svc_info_bf)
    uint8_t  info;
    /// Service UUID (LSB first)
    uint8_t  uuid[GATT_UUID_128_LEN];
    ///  Attribute Start Handle
    uint16_t start_hdl;
    /// Attribute End Handle
    uint16_t end_hdl;
} gatt_svc_desc_t;

/// Attribute structure
/*@TRACE*/
typedef struct gatt_att
{
    /// Attribute handle
    uint16_t hdl;
    /// Value length
    uint16_t length;
} gatt_att_t;

/// Attribute info structure for Service Discovery
typedef struct gatt_svc_att
{
    /// Attribute Type (@see enum gatt_att_type)
    uint8_t  att_type;
    /// UUID type (@see enum gatt_uuid_type)
    uint8_t  uuid_type;
    /// UUID - LSB first (0 for GATT_ATT_NONE)
    uint8_t  uuid[GATT_UUID_128_LEN];

    /// Information about Service attribute
    union gatt_info
    {
        /// Service info (GATT_ATT_PRIMARY_SVC, GATT_ATT_SECONDARY_SVC, GATT_ATT_INCL_SVC)
        struct gatt_svc_info
        {
            /// Service start handle
            uint16_t start_hdl;
            /// Service end handle
            uint16_t end_hdl;
        } svc;

        /// Characteristic info (GATT_ATT_CHAR)
        struct gatt_char_info
        {
            /// Value handle
            uint16_t val_hdl;
            /// Characteristic properties (@see enum gatt_att_info_bf - bits [0-7])
            uint8_t  prop;
        } charac;
    } info;
} gatt_svc_att_t;

/*
 * DEFAULT MESSAGE CONTENT
 ****************************************************************************************
 */

/// Default GATT command structure
/*@TRACE*/
typedef struct gatt_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
} gatt_cmd_t;

/// Default GATT command Complete structure
/*@TRACE*/
typedef struct gatt_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier
    uint8_t         user_lid;
} gatt_cmp_evt_t;

/// Default command complete for client or server initiated procedures
/*@TRACE*/
typedef struct gatt_proc_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
} gatt_proc_cmp_evt_t;

/// Default GATT indication structure
/*@TRACE*/
typedef struct gatt_ind
{
    /// Indication code (@see enum gatt_ind_code)
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
} gatt_ind_t;


/// Default GATT request indication structure
/*@TRACE*/
typedef struct gatt_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
} gatt_req_ind_t;

/// Default GATT confirm structure
/*@TRACE*/
typedef struct gatt_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user  (@see enum hl_err)
    uint16_t        status;
} gatt_cfm_t;


/*
 * MESSAGE CONTENT
 ****************************************************************************************
 */

/// GATT_USER_REGISTER Command structure definition
/*@TRACE*/
typedef struct gatt_user_register_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_USER_REGISTER
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// Preferred MTU for attribute exchange.
    uint16_t        pref_mtu;
    /// User attribute priority level
    uint8_t         prio_level;
    /// Client or Server role (@see enum gatt_user_role)
    uint8_t         role;
} gatt_user_register_cmd_t;

/// GATT_USER_REGISTER uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_user_register_cmp_evt_t;

/// GATT_USER_UNREGISTER Command structure definition
/*@TRACE*/
typedef struct gatt_user_unregister_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_USER_UNREGISTER
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT User Local identifier
    uint8_t         user_lid;
} gatt_user_unregister_cmd_t;

/// GATT_USER_UNREGISTER uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_user_unregister_cmp_evt_t;

/// GATT_DB_SVC_ADD Command structure definition
/*@TRACE*/
typedef struct gatt_db_svc_add_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DB_SVC_ADD
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Service Information bit field (see Table 18)
    uint8_t         info;
    /// Service UUID (LSB first)
    uint8_t         uuid[GATT_UUID_128_LEN];
    /// Attribute Start Handle (0 = chosen by GATT module)
    uint16_t        start_hdl;
    /// Number of attribute(s) reserved for the service (shall be equals or greater nb_att)
    /// Prevent any services to be inserted between start_hdl and (start_hdl + nb_att_rsvd -1)
    uint8_t         nb_att_rsvd;
    /// Number of attribute(s) in service
    uint8_t         nb_att;
    /// List of attribute description present in service.
    /// @note First attribute in attribute array must be a Primary or a Secondary service
    gatt_att_desc_t atts[__ARRAY_EMPTY];
} gatt_db_svc_add_cmd_t;

/// GATT_DB_SVC_ADD Command Complete Event structure definition
/*@TRACE*/
typedef struct gatt_db_svc_add_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Service start handle associated to created service.
    uint16_t        start_hdl;
} gatt_db_svc_add_cmp_evt_t;

/// GATT_DB_SVC_REMOVE Command structure definition
/*@TRACE*/
typedef struct gatt_db_svc_remove_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DB_SVC_REMOVE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Attribute Start Handle of the service to remove
    uint16_t        start_hdl;
} gatt_db_svc_remove_cmd_t;

/// GATT_DB_SVC_REMOVE uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_db_svc_remove_cmp_evt_t;


/// GATT_DB_SVC_CTRL_SET Command structure definition
/*@TRACE*/
typedef struct gatt_db_svc_ctrl_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DB_SVC_REMOVE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// 0: Authorize usage of the service
    /// 1: Reject usage of the service
    uint8_t         enable;
    /// Service visibility (@see enum gatt_svc_visibility)
    uint8_t         visible;
    /// Attribute Start Handle of the service to hide/show
    uint16_t        start_hdl;
} gatt_db_svc_ctrl_cmd_t;

/// GATT_DB_SVC_VISIBILITY_SET uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_db_svc_visibility_set_cmp_evt_t;

/// GATT_DB_HASH_GET Command structure definition
/*@TRACE*/
typedef struct gatt_db_hash_get_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DB_HASH_GET
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
} gatt_db_hash_get_cmd_t;

/// GATT_DB_HASH_GET Command Complete Event structure definition
/*@TRACE*/
typedef struct gatt_db_hash_get_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Database Hash.
    uint8_t         hash[GATT_DB_HASH_LEN];
} gatt_db_hash_get_cmp_evt_t;


/// GATT_SRV_EVENT_RELIABLE_SEND Command structure definition
/*@TRACE*/
typedef struct gatt_srv_event_reliable_send_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_SRV_EVENT_RELIABLE_SEND
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Event type to trigger (@see enum gatt_evt_type)
    uint8_t         evt_type;
    /// Number of attribute
    uint8_t         nb_att;
    /// List of attribute
    gatt_att_t      atts[__ARRAY_EMPTY];
} gatt_srv_event_reliable_send_cmd_t;

/// GATT_SRV_EVENT_RELIABLE_SEND uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_srv_event_reliable_send_cmp_evt_t;



/// GATT_SRV_EVENT_SEND Command structure definition
/*@TRACE*/
typedef struct gatt_srv_event_send_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_SRV_EVENT_SEND
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Event type to trigger (@see enum gatt_evt_type)
    uint8_t         evt_type;
    /// Attribute handle
    uint16_t        hdl;
    /// Value length
    uint16_t        value_length;
    /// Value to transmit
    uint8_t         value[__ARRAY_EMPTY];
} gatt_srv_event_send_cmd_t;

/// GATT_SRV_EVENT_SEND uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_srv_event_send_cmp_evt_t;

/// GATT_SRV_EVENT_MTP_SEND Command structure definition
/*@TRACE*/
typedef struct gatt_srv_event_mtp_send_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_SRV_EVENT_MTP_SEND
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index bit field
    uint32_t        conidx_bf;
    /// Event type to trigger (@see enum gatt_evt_type)
    uint8_t         evt_type;
    /// Attribute handle
    uint16_t        hdl;
    /// Value length
    uint16_t        value_length;
    /// Value to transmit
    uint8_t         value[__ARRAY_EMPTY];
} gatt_srv_event_mtp_send_cmd_t;

/// GATT_SRV_EVENT_MTP_SEND uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_srv_event_mtp_send_cmp_evt_t;

/// GATT_SRV_EVENT_MTP_CANCEL Command structure definition
/*@TRACE*/
typedef struct gatt_srv_event_mtp_cancel_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_SRV_EVENT_MTP_CANCEL
    uint16_t        cmd_code;
    /// Dummy parameter provided in GATT_SRV_EVENT_MTP_SEND command
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
} gatt_srv_event_mtp_cancel_cmd_t;

/// GATT_SRV_EVENT_MTP_CANCEL uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_srv_event_mtp_cancel_cmp_evt_t;


/// GATT_CLI_DISCOVER_SVC Command structure definition
/*@TRACE*/
typedef struct gatt_cli_discover_svc_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_DISCOVER_SVC
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// GATT Service discovery type (@see enum gatt_svc_discovery_type)
    uint8_t         disc_type;
    /// Perform discovery of all information present in the service  (1: enable, 0: disable)
    uint8_t         full;
    /// Search start handle
    uint16_t        start_hdl;
    /// Search end handle
    uint16_t        end_hdl;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Searched Service UUID (meaningful only for discovery by UUID)
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_discover_svc_cmd_t;

/// GATT_CLI_DISCOVER_SVC uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_discover_svc_cmp_evt_t;


/// GATT_CLI_DISCOVER_INC_SVC Command structure definition
/*@TRACE*/
typedef struct gatt_cli_discover_inc_svc_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_DISCOVER_INC_SVC
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Search start handle
    uint16_t        start_hdl;
    /// Search end handle
    uint16_t        end_hdl;
} gatt_cli_discover_inc_svc_cmd_t;

/// GATT_CLI_DISCOVER_INC_SVC uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_discover_inc_svc_cmp_evt_t;

/// GATT_CLI_DISCOVER_CHAR Command structure definition
/*@TRACE*/
typedef struct gatt_cli_discover_char_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_DISCOVER_CHAR
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// GATT characteristic discovery type (@see enum gatt_char_discovery_type)
    uint8_t         disc_type;
    /// Search start handle
    uint16_t        start_hdl;
    /// Search end handle
    uint16_t        end_hdl;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Searched UUID (meaningful only for discovery by UUID)
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_discover_char_cmd_t;

/// GATT_CLI_DISCOVER_CHAR uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_discover_char_cmp_evt_t;

/// GATT_CLI_DISCOVER_DESC Command structure definition
/*@TRACE*/
typedef struct gatt_cli_discover_desc_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_DISCOVER_DESC
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Search start handle
    uint16_t        start_hdl;
    /// Search end handle
    uint16_t        end_hdl;
} gatt_cli_discover_desc_cmd_t;

/// GATT_CLI_DISCOVER_DESC uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_discover_desc_cmp_evt_t;


/// GATT_CLI_DISCOVER_CANCEL Command structure definition
/*@TRACE*/
typedef struct gatt_cli_discover_cancel_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_DISCOVER_CANCEL
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
} gatt_cli_discover_cancel_cmd_t;

/// GATT_CLI_DISCOVER_CANCEL uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_discover_cancel_cmp_evt_t;



/// GATT_CLI_READ Command structure definition
/*@TRACE*/
typedef struct gatt_cli_read_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_READ
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute handle
    uint16_t        hdl;
    /// Value offset
    uint16_t        offset;
    /// Value length to read (0 = read complete value)
    uint16_t        length;
} gatt_cli_read_cmd_t;

/// GATT_CLI_READ uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_read_cmp_evt_t;


/// GATT_CLI_READ_BY_UUID Command structure definition
/*@TRACE*/
typedef struct gatt_cli_read_by_uuid_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_READ_BY_UUID
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Search start handle
    uint16_t        start_hdl;
    /// Search end handle
    uint16_t        end_hdl;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Searched UUID
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_read_by_uuid_cmd_t;

/// GATT_CLI_READ_BY_UUID uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_read_by_uuid_cmp_evt_t;


/// GATT_CLI_READ_MULTIPLE Command structure definition
/*@TRACE*/
typedef struct gatt_cli_read_multiple_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_READ_MULTIPLE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Number of attribute
    uint8_t         nb_att;
    /// List of attribute
    /// If Attribute length is zero (consider length unknown):
    ///     - Attribute protocol read multiple variable length procedure used
    gatt_att_t      atts[__ARRAY_EMPTY];
} gatt_cli_read_multiple_cmd_t;

/// GATT_CLI_READ_MULTIPLE uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_read_multiple_cmp_evt_t;


/// GATT_CLI_WRITE_RELIABLE Command structure definition
/*@TRACE*/
typedef struct gatt_cli_write_reliable_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_WRITE_RELIABLE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// GATT write type (@see enum gatt_write_type)
    uint8_t         write_type;
    /// Write execution mode (@see enum gatt_write_mode). Valid only for GATT_WRITE.
    uint8_t         write_mode;
    /// Attribute handle
    uint16_t        hdl;
    /// Value offset, valid only for GATT_WRITE
    uint16_t        offset;
    /// Value length to write
    uint16_t        length;
} gatt_cli_write_reliable_cmd_t;

/// GATT_CLI_WRITE_RELIABLE uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_write_reliable_cmp_evt_t;

/// GATT_CLI_WRITE Command structure definition
/*@TRACE*/
typedef struct gatt_cli_write_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_WRITE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// GATT write type (@see enum gatt_write_type)
    uint8_t         write_type;
    /// Attribute handle
    uint16_t        hdl;
    /// Value offset, valid only for GATT_WRITE
    uint16_t        offset;
    /// Value length to write
    uint16_t        value_length;
    /// Attribute value
    uint8_t         value[__ARRAY_EMPTY];
} gatt_cli_write_cmd_t;

/// GATT_CLI_WRITE uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_write_cmp_evt_t;


/// GATT_CLI_WRITE_EXE Command structure definition
/*@TRACE*/
typedef struct gatt_cli_write_exe_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_WRITE_EXE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// 1: Perform pending write operations
    /// 0: Cancel pending write operations
    uint8_t         execute;
} gatt_cli_write_exe_cmd_t;

/// GATT_CLI_WRITE_EXE uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_write_exe_cmp_evt_t;


/// GATT_CLI_EVENT_REGISTER Command structure definition
/*@TRACE*/
typedef struct gatt_cli_event_register_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_EVENT_REGISTER
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute start handle
    uint16_t        start_hdl;
    /// Attribute end handle
    uint16_t        end_hdl;
} gatt_cli_event_register_cmd_t;

/// GATT_CLI_EVENT_REGISTER uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_event_register_cmp_evt_t;

/// GATT_CLI_EVENT_UNREGISTER Command structure definition
/*@TRACE*/
typedef struct gatt_cli_event_unregister_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_EVENT_UNREGISTER
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute start handle
    uint16_t        start_hdl;
    /// Attribute end handle
    uint16_t        end_hdl;
} gatt_cli_event_unregister_cmd_t;

/// GATT_CLI_EVENT_UNREGISTER uses default command procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_event_unregister_cmp_evt_t;
/// GATT_CLI_MTU_UPDATE Command structure definition
/*@TRACE*/
typedef struct gatt_cli_mtu_update_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_CLI_MTU_UPDATE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
} gatt_cli_mtu_update_cmd_t;

/// GATT_CLI_MTU_UPDATE uses default procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_cli_mtu_update_cmp_evt_t;


/// GATT_DBG_DB_SVC_REMOVE_ALL Command structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_svc_remove_all_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_DB_SVC_REMOVE_ALL
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
} gatt_dbg_db_svc_remove_all_cmd_t;


/// GATT_DBG_DB_SVC_REMOVE_ALL uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_dbg_db_svc_remove_all_cmp_evt_t;


/// GATT_DBG_DB_SVC_LIST_GET Command structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_svc_list_get_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_DB_SVC_LIST_GET
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
} gatt_dbg_db_svc_list_get_cmd_t;


/// GATT_DBG_DB_SVC_LIST_GET Command Complete Event structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_svc_list_get_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Number of services
    uint8_t         nb_svc;
    /// List of service description
    gatt_svc_desc_t svcs[__ARRAY_EMPTY];
} gatt_dbg_db_svc_list_get_cmp_evt_t;


/// GATT_DBG_DB_SVC_INFO_SET Command structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_svc_info_set_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_DB_SVC_INFO_SET
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// Start handle of service to update
    uint16_t        hdl;
    /// Service Information bit field (@see enum gatt_svc_info_bf)
    uint8_t         info;
} gatt_dbg_db_svc_info_set_cmd_t;


/// GATT_DBG_DB_SVC_INFO_SET uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_dbg_db_svc_info_set_cmp_evt_t;


/// GATT_DBG_DB_ATT_INFO_GET Command structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_att_info_get_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_DB_ATT_INFO_GET
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// Attribute Handle
    uint16_t        hdl;
} gatt_dbg_db_att_info_get_cmd_t;

/// GATT_DBG_DB_ATT_INFO_GET Command Complete Event structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_att_info_get_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Attribute Handle
    uint16_t        hdl;
    /// Attribute description
    gatt_att_desc_t att;
} gatt_dbg_db_att_info_get_cmp_evt_t;

/// GATT_DBG_DB_ATT_INFO_SET Command structure definition
/*@TRACE*/
typedef struct gatt_dbg_db_att_info_set_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_DB_ATT_INFO_SET
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// Attribute Handle
    uint16_t        hdl;
    /// Attribute information bit field (@see enum gatt_att_info_bf)
    uint16_t        info;
} gatt_dbg_db_att_info_set_cmd_t;


/// GATT_DBG_DB_ATT_INFO_SET uses default complete event structure
/*@TRACE*/
typedef gatt_cmp_evt_t gatt_dbg_db_att_info_set_cmp_evt_t;

/// GATT_DBG_BEARER_INFO_GET Command structure definition
typedef struct gatt_dbg_bearer_info_get_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_BEARER_INFO_GET
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
} gatt_dbg_bearer_info_get_cmd_t;

/// Structure that contains bearer information
/*@TRACE*/
typedef struct gatt_dbg_bearer_info
{
    /// Bearer Local Identifier
    uint8_t     bearer_lid;
    /// L2CAP Channel Local identifier
    uint8_t     chan_lid;
    /// 1 if Enhanced ATT bearer 0 if legacy ATT bearer
    uint8_t     eatt;
    /// Bearer MTU size
    uint16_t    mtu;
} gatt_dbg_bearer_info_t;

/// GATT_DBG_BEARER_INFO_GET Command Complete Event structure definition
/*@TRACE*/
typedef struct gatt_dbg_bearer_info_get_cmp_evt
{
    /// Command code (@see enum gatt_cmd_code)
    uint16_t        cmd_code;
    /// Dummy parameter provided by upper layer for command execution.
    uint16_t        dummy;
    /// Status of the operation (@see enum hl_err)
    uint16_t        status;
    /// GATT user local identifier - not used
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Number of active bearers
    uint8_t         nb_bearer;
    /// list of available bearers
    gatt_dbg_bearer_info_t bearers[__ARRAY_EMPTY];
} gatt_dbg_bearer_info_get_cmp_evt_t;

/// GATT_DBG_BEARER_CLOSE Command structure definition
typedef struct gatt_dbg_bearer_close_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_BEARER_CLOSE
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Bearer Local identifier
    uint8_t         bearer_lid;
} gatt_dbg_bearer_close_cmd_t;

/// GATT_DBG_BEARER_CLOSE uses default procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_dbg_bearer_close_cmp_evt_t;

/// GATT_DBG_BEARER_EATT_ESTAB Command structure definition
/*@TRACE*/
typedef struct gatt_dbg_bearer_eatt_estab_cmd
{
    /// Command code (@see enum gatt_cmd_code)
    ///  - GATT_DBG_BEARER_EATT_ESTAB
    uint16_t        cmd_code;
    /// Dummy parameter whose meaning is upper layer dependent and which is returned in command complete event and
    /// indications sent during command handling. It can be used as a sequence number for instance.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
} gatt_dbg_bearer_eatt_estab_cmd_t;

/// GATT_DBG_BEARER_EATT_ESTAB uses default procedure complete event structure
/*@TRACE*/
typedef gatt_proc_cmp_evt_t gatt_dbg_bearer_eatt_estab_cmp_evt_t;

/// GATT_UNKNOWN_MSG Indication structure definition
/*@TRACE*/
typedef struct gatt_unknown_msg_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_UNKNOWN_MSG
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Message identifier
    uint16_t        msg_id;
} gatt_unknown_msg_ind_t;


/// GATT_CLI_SVC Indication structure definition
typedef struct gatt_cli_svc_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_SVC
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// First handle value of following list
    uint16_t        hdl;
    /// Discovery information (@see enum gatt_svc_disc_info)
    uint8_t         disc_info;
    /// Number of attributes
    uint8_t         nb_att;
    /// Attribute information present in a service
    gatt_svc_att_t  atts[__ARRAY_EMPTY];
} gatt_cli_svc_ind_t;

/// GATT_CLI_SVC_INFO Indication structure definition
/*@TRACE*/
typedef struct gatt_cli_svc_info_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_SVC_INFO
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Service start handle
    uint16_t        start_hdl;
    /// Service end handle
    uint16_t        end_hdl;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Service UUID (LSB first)
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_svc_info_ind_t;

/// GATT_CLI_INC_SVC Indication structure definition
/*@TRACE*/
typedef struct gatt_cli_inc_svc_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_INC_SVC
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Include service attribute handle
    uint16_t        inc_svc_hdl;
    /// Service start handle
    uint16_t        start_hdl;
    /// Service end handle
    uint16_t        end_hdl;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Service UUID (LSB first)
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_inc_svc_ind_t;

/// GATT_CLI_CHAR Indication structure definition
/*@TRACE*/
typedef struct gatt_cli_char_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_CHAR
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Characteristic attribute handle
    uint16_t        char_hdl;
    /// Value handle
    uint16_t        val_hdl;
    /// Characteristic properties (@see enum gatt_att_info_bf - bits [0-7])
    uint8_t         prop;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Characteristic value UUID - LSB first
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_char_ind_t;

/// GATT_CLI_DESC Indication structure definition
/*@TRACE*/
typedef struct gatt_cli_desc_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_DESC
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Characteristic descriptor attribute handle
    uint16_t        desc_hdl;
    /// UUID Type (@see enum gatt_uuid_type)
    uint8_t         uuid_type;
    /// Attribute UUID - LSB first
    uint8_t         uuid[GATT_UUID_128_LEN];
} gatt_cli_desc_ind_t;


/// GATT_CLI_ATT_VAL Indication structure definition
/*@TRACE*/
typedef struct gatt_cli_att_val_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_ATT_VAL
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute handle
    uint16_t        hdl;
    /// Data offset
    uint16_t        offset;
    /// Value length
    uint16_t        value_length;
    /// Attribute value starting from offset
    uint8_t         value[__ARRAY_EMPTY];
} gatt_cli_att_val_ind_t;


/// GATT_CLI_SVC_CHANGED Indication structure definition
/*@TRACE*/
typedef struct gatt_cli_svc_changed_ind
{
    /// Indication code (@see enum gatt_ind_code)
    ///  - GATT_CLI_SVC_CHANGED
    uint16_t        ind_code;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// True if an out of sync error has been received
    uint8_t         out_of_sync;
    /// Service start handle
    uint16_t        start_hdl;
    /// Service end handle
    uint16_t        end_hdl;
} gatt_cli_svc_changed_ind_t;

/// GATT_SRV_ATT_READ_GET request indication structure
/*@TRACE*/
typedef struct gatt_srv_att_read_get_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_READ_GET
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute handle
    uint16_t        hdl;
    /// Value offset
    uint16_t        offset;
    /// Maximum value length to return
    uint16_t        max_length;
} gatt_srv_att_read_get_req_ind_t;

/// GATT_SRV_ATT_READ_GET confirm structure
/*@TRACE*/
typedef struct gatt_srv_att_read_get_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_READ_GET
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user (@see enum hl_err)
    uint16_t        status;
    /// Complete Length of the attribute value
    uint16_t        att_length;
    /// Value length
    uint16_t        value_length;
    /// Attribute value (starting from data offset)
    uint8_t         value[__ARRAY_EMPTY];
} gatt_srv_att_read_get_cfm_t;

/// GATT_SRV_ATT_EVENT_GET request indication structure
/*@TRACE*/
typedef struct gatt_srv_att_event_get_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_EVENT_GET
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Dummy parameter provided by upper layer for command execution
    uint16_t        dummy;
    /// Attribute handle
    uint16_t        hdl;
    /// Maximum value length to return
    uint16_t        max_length;
} gatt_srv_att_event_get_req_ind_t;

/// GATT_SRV_ATT_EVENT_GET confirm structure
/*@TRACE*/
typedef struct gatt_srv_att_event_get_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_EVENT_GET
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user (@see enum hl_err)
    uint16_t        status;
    /// Complete Length of the attribute value
    uint16_t        att_length;
    /// Value length
    uint16_t        value_length;
    /// Attribute value (starting from data offset)
    uint8_t         value[__ARRAY_EMPTY];
} gatt_srv_att_event_get_cfm_t;


/// GATT_SRV_ATT_INFO_GET request indication structure
/*@TRACE*/
typedef struct gatt_srv_att_info_get_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_INFO_GET
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute handle
    uint16_t        hdl;
} gatt_srv_att_info_get_req_ind_t;

/// GATT_SRV_ATT_INFO_GET confirm structure
/*@TRACE*/
typedef struct gatt_srv_att_info_get_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_INFO_GET
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user (@see enum hl_err)
    uint16_t        status;
    /// Attribute Value length
    uint16_t        att_length;
} gatt_srv_att_info_get_cfm_t;

/// GATT_SRV_ATT_VAL_SET request indication structure
/*@TRACE*/
typedef struct gatt_srv_att_val_set_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_VAL_SET
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Attribute handle
    uint16_t        hdl;
    /// Value offset
    uint16_t        offset;
    /// Value length to write
    uint16_t        value_length;
    /// Attribute value to update (starting from offset)
    uint8_t         value[__ARRAY_EMPTY];
} gatt_srv_att_val_set_req_ind_t;

/// GATT_SRV_ATT_VAL_SET confirm structure
/*@TRACE*/
typedef struct gatt_srv_att_val_set_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_SRV_ATT_VAL_SET
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user (@see enum hl_err)
    uint16_t        status;
} gatt_srv_att_val_set_cfm_t;

/// GATT_CLI_ATT_VAL_GET request indication structure
/*@TRACE*/
typedef struct gatt_cli_att_val_get_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_CLI_ATT_VAL_GET
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Dummy parameter provided by upper layer for command execution
    /// 0x0000 else.
    uint16_t        dummy;
    /// Attribute handle
    uint16_t        hdl;
    /// Data offset
    uint16_t        offset;
    /// Maximum value length to return
    uint16_t        max_length;
} gatt_cli_att_val_get_req_ind_t;

/// GATT_CLI_ATT_VAL_GET confirm structure
/*@TRACE*/
typedef struct gatt_cli_att_val_get_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_CLI_ATT_VAL_GET
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user (@see enum hl_err)
    uint16_t        status;
    /// Value length
    uint16_t        value_length;
    /// Attribute value (starting from data offset)
    uint8_t         value[__ARRAY_EMPTY];
} gatt_cli_att_val_get_cfm_t;

/// GATT_CLI_ATT_EVENT request indication structure
/*@TRACE*/
typedef struct gatt_cli_att_event_req_ind
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_CLI_ATT_EVENT
    uint16_t        req_ind_code;
    /// Token provided by GATT module that must be used into the GAT_CFM message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Event type triggered (@see enum gatt_evt_type)
    uint8_t         evt_type;
    /// 1: if event value if complete value has been received
    /// 0: if data received is equals to maximum attribute protocol value. In such case GATT Client User should
    ///    perform a read procedure.
    uint8_t         complete;
    /// Attribute handle
    uint16_t        hdl;
    /// Value length
    uint16_t        value_length;
    /// Attribute value
    uint8_t         value[__ARRAY_EMPTY];
} gatt_cli_att_event_req_ind_t;

/// GATT_CLI_ATT_EVENT confirm structure
/*@TRACE*/
typedef struct gatt_cli_att_event_cfm
{
    /// Request Indication code (@see enum gatt_req_ind_code)
    /// - GATT_CLI_ATT_EVENT
    uint16_t        req_ind_code;
    /// Token provided by GAT module into the GATT_REQ_IND message
    uint16_t        token;
    /// GATT user local identifier
    uint8_t         user_lid;
    /// Connection index
    uint8_t         conidx;
    /// Status of the request by GATT user (@see enum hl_err) - Ignored
    uint16_t        status;
} gatt_cli_att_event_cfm_t;

/// @} GATT_MSG

#endif // GATT_MSG_H_
