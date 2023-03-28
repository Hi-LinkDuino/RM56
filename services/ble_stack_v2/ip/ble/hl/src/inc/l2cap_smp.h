/**
 ****************************************************************************************
 *
 * @file l2cap_smp.h
 *
 * @brief Header file - L2CAP Security Manager Protocol Defines
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_SMP_H_
#define L2CAP_SMP_H_

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
#include "gap.h"

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */
/// MTU Size for SMP L2CAP channel
#define L2CAP_SMP_MTU           (65)
/// SMP header length
#define L2CAP_SMP_HEADER_LEN    (1)


/// SMP Pairing Failed Reasons
enum l2cap_smp_err_codes
{
    /// The user input of passkey failed, for example, the user cancelled the operation.
    L2CAP_SMP_ERR_PASSKEY_ENTRY_FAILED                          = 0x01,
    /// The OOB Data is not available.
    L2CAP_SMP_ERR_OOB_NOT_AVAILABLE                             = 0x02,
    /// The pairing procedure cannot be performed as authentication requirements cannot be
    /// met due to IO capabilities of one or both devices.
    L2CAP_SMP_ERR_AUTH_REQ                                      = 0x03,
    /// The confirm value does not match the calculated confirm value.
    L2CAP_SMP_ERR_CONF_VAL_FAILED                               = 0x04,
    /// Pairing is not supported by the device.
    L2CAP_SMP_ERR_PAIRING_NOT_SUPP                              = 0x05,
    ///  The resultant encryption key size is insufficient for the security requirements of this device.
    L2CAP_SMP_ERR_ENC_KEY_SIZE                                  = 0x06,
    /// The SMP command received is not supported on this device.
    L2CAP_SMP_ERR_CMD_NOT_SUPPORTED                             = 0x07,
    /// Pairing failed due to an unspecified reason.
    L2CAP_SMP_ERR_UNSPECIFIED_REASON                            = 0x08,
    /// Pairing or Authentication procedure is disallowed because too little time has elapsed
    /// since last pairing request or security request.
    L2CAP_SMP_ERR_REPEATED_ATTEMPTS                             = 0x09,
    ///  The command length is invalid or a parameter is outside of the specified range.
    L2CAP_SMP_ERR_INVALID_PARAM                                 = 0x0A,
    /// Indicates to the remote device that the DHKey Check value received doesn't
    /// match the one calculated by the local device.
    L2CAP_SMP_ERR_DHKEY_CHECK_FAILED                            = 0x0B,
    /// Indicates that the confirm values in the numeric comparison protocol do not match.
    L2CAP_SMP_ERR_NUMERIC_COMPARISON_FAILED                     = 0x0C,
    /// Indicates that the pairing over the LE transport failed due to a Pairing Request sent
    /// over the BR/EDR transport in process.
    L2CAP_SMP_ERR_BREDR_PAIRING_IN_PROGRESS                     = 0x0D,
    /// Indicates that the BR/EDR Link Key generated on the BR/EDR transport cannot be
    /// used to derive and distribute keys for the LE transport.
    L2CAP_SMP_ERR_CROSS_TRANSPORT_KEY_GENERATION_NOT_ALLOWED    = 0x0E,
};

/// L2CAP security Op-Codes
/*@TRACE*/
enum l2cap_smp_code
{
    /// Reserved
    L2CAP_SMP_RESERVED_OPCODE                     = 0x00,
    /// Pairing Request
    L2CAP_SMP_PAIRING_REQ_OPCODE                  = 0x01,
    /// Pairing Response
    L2CAP_SMP_PAIRING_RSP_OPCODE                  = 0x02,
    /// Pairing Confirm
    L2CAP_SMP_PAIRING_CFM_OPCODE                  = 0x03,
    /// Pairing Random
    L2CAP_SMP_PAIRING_RANDOM_OPCODE               = 0x04,
    /// Pairing Failed
    L2CAP_SMP_PAIRING_FAILED_OPCODE               = 0x05,
    /// Encryption Information
    L2CAP_SMP_ENCRYPTION_INF_OPCODE               = 0x06,
    /// Master Identification
    L2CAP_SMP_MASTER_ID_OPCODE                    = 0x07,
    /// Identity Information
    L2CAP_SMP_IDENTITY_INF_OPCODE                 = 0x08,
    /// Identity Address Information
    L2CAP_SMP_ID_ADDR_INF_OPCODE                  = 0x09,
    /// Signing Information
    L2CAP_SMP_SIGNING_INF_OPCODE                  = 0x0A,
    /// Security Request
    L2CAP_SMP_SECURITY_REQ_OPCODE                 = 0x0B,
    /// Pairing Public Keys
    L2CAP_SMP_PUBLIC_KEY_OPCODE                   = 0x0C,
    /// Pairing DHKey Check
    L2CAP_SMP_DHKEY_CHECK_OPCODE                  = 0x0D,
    /// Pairing Key-press Notification
    L2CAP_SMP_KEYPRESS_NTF_OPCODE                 = 0x0E,
    /// max number of security codes
    L2CAP_SMP_OPCODE_MAX,
};

/// L2CAP security message length
enum l2cap_smp_msg_len
{
    /// Reserved
    L2CAP_SMP_RESERVED_LEN                        = 0,
    /// Pairing Request
    L2CAP_SMP_PAIRING_REQ_LEN                     = 6,
    /// Pairing Response
    L2CAP_SMP_PAIRING_RSP_LEN                     = 6,
    /// Pairing Confirm
    L2CAP_SMP_PAIRING_CFM_LEN                     = 16,
    /// Pairing Random
    L2CAP_SMP_PAIRING_RANDOM_LEN                  = 16,
    /// Pairing Failed
    L2CAP_SMP_PAIRING_FAILED_LEN                  = 1,
    /// Encryption Information
    L2CAP_SMP_ENCRYPTION_INF_LEN                  = 16,
    /// Master Identification
    L2CAP_SMP_MASTER_ID_LEN                       = 10,
    /// Identity Information
    L2CAP_SMP_IDENTITY_INF_LEN                    = 16,
    /// Identity Address Information
    L2CAP_SMP_ID_ADDR_INF_LEN                     = 7,
    /// Signing Information
    L2CAP_SMP_SIGNING_INF_LEN                     = 16,
    /// Security Request
    L2CAP_SMP_SECURITY_REQ_LEN                    = 1,
    /// Pairing Public Keys
    L2CAP_SMP_PUBLIC_KEY_LEN                      = 64,
    /// Pairing DHKey Check
    L2CAP_SMP_DHKEY_CHECK_LEN                     = 16,
    /// Pairing Key-press Notification
    L2CAP_SMP_KEYPRESS_NTF_LEN                    = 1,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// Pairing Request
/*@TRACE*/
typedef struct l2cap_smp_pairing_req
{
    /// security code - 0x01
    uint8_t     code;
    /// IO Capability
    uint8_t     iocap;
    /// OOB data flag
    uint8_t     oob;
    /// AuthReq
    uint8_t     auth;
    /// Maximum Encryption Key Size
    uint8_t     key_size;
    /// Initiator Key Distribution
    uint8_t     ikey_dist;
    /// Responder Key Distribution
    uint8_t     rkey_dist;
} l2cap_smp_pairing_req_t;

/// Pairing Response
/*@TRACE*/
typedef struct l2cap_smp_pairing_rsp
{
    /// security code - 0x02
    uint8_t     code;
    /// IO Capability
    uint8_t     iocap;
    /// OOB data flag
    uint8_t     oob;
    /// AuthReq
    uint8_t     auth;
    /// Maximum Encryption Key Size
    uint8_t     key_size;
    /// Initiator Key Distribution
    uint8_t     ikey_dist;
    /// Responder Key Distribution
    uint8_t     rkey_dist;
} l2cap_smp_pairing_rsp_t;

/// Pairing Confirm
/*@TRACE*/
typedef struct l2cap_smp_pairing_cfm
{
    /// security code - 0x03
    uint8_t     code;
    ///Confirm value
    uint8_t     cfm_val[GAP_KEY_LEN];
} l2cap_smp_pairing_cfm_t;

/// Pairing Random
/*@TRACE*/
typedef struct l2cap_smp_pairing_random
{
    /// security code - 0x04
    uint8_t     code;
    ///Random value
    uint8_t     random[GAP_KEY_LEN];
} l2cap_smp_pairing_random_t;

/// Pairing Failed
/*@TRACE*/
typedef struct l2cap_smp_pairing_failed
{
    /// security code - 0x05
    uint8_t     code;
    /// The Reason field indicates why the pairing failed
    uint8_t     reason;
} l2cap_smp_pairing_failed_t;

/// Encryption Information
/*@TRACE*/
typedef struct l2cap_smp_encryption_inf
{
    /// security code - 0x06
    uint8_t     code;
    ///16-byte array for LTK value
    uint8_t     ltk[GAP_KEY_LEN];
} l2cap_smp_encryption_inf_t;

/// Master Identification
/*@TRACE*/
typedef struct l2cap_smp_master_id
{
    /// security code - 0x07
    uint8_t     code;
    /// key diversifier
    uint16_t    ediv;
    ///8-byte array for random number
    uint8_t     nb[GAP_RAND_NB_LEN];
} l2cap_smp_master_id_t;

/// Identity Information
/*@TRACE*/
typedef struct l2cap_smp_identity_inf
{
    /// security code - 0x08
    uint8_t     code;
    ///16-byte array for IRK value
    uint8_t     irk[GAP_KEY_LEN];
} l2cap_smp_identity_inf_t;

/// Identity Address Information
/*@TRACE*/
typedef struct l2cap_smp_id_addr_inf
{
    /// security code - 0x09
    uint8_t     code;
    /// BD Address Type
    uint8_t     addr_type;
    /// BD Address
    gap_addr_t  addr;
} l2cap_smp_id_addr_inf_t;

/// Signing Information
/*@TRACE*/
typedef struct l2cap_smp_signing_inf
{
    /// security code - 0x0A
    uint8_t     code;
    ///16-byte array for CSRK value
    uint8_t     csrk[GAP_KEY_LEN];
} l2cap_smp_signing_inf_t;

/// Security Request
/*@TRACE*/
typedef struct l2cap_smp_security_req
{
    /// security code - 0x0B
    uint8_t     code;
    /// AuthReq
    uint8_t     auth;
} l2cap_smp_security_req_t;

/// Public Key (x,y)
/*@TRACE*/
typedef struct l2cap_smp_public_key
{
    /// security code - 0x0C
    uint8_t     code;
    /// X coordinates of the Public Key
    uint8_t     x[GAP_P256_KEY_LEN];
    /// Y coordinates of the Public Key
    uint8_t     y[GAP_P256_KEY_LEN];
} l2cap_smp_public_key_t;

/// DH Key Check
/*@TRACE*/
typedef struct l2cap_smp_dhkey_check
{
    /// security code - 0x0D
    uint8_t  code;
    /// Check hash
    uint8_t  check[GAP_KEY_LEN];
} l2cap_smp_dhkey_check_t;

/// Keypress Notification - PassKey method only
/*@TRACE*/
typedef struct l2cap_smp_keypress_ntf
{
    /// security code - 0x0E
    uint8_t  code;
    /// Notificaition code
    uint8_t  ntf_type;
} l2cap_smp_keypress_ntf_t;



/*@TRACE
 @trc_ref l2cap_cid, l2cap_smp_code
 */
union l2cap_smp_pdu
{
    /// L2Cap packet code.
    uint8_t code;

    // Security manager PDUs
    /// Pairing Request
    //@trc_union code == L2CAP_SMP_PAIRING_REQ_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_pairing_req_t    pairing_req;
    /// Pairing Response
    //@trc_union code == L2CAP_SMP_PAIRING_RSP_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_pairing_rsp_t    pairing_rsp;
    /// Pairing Confirm
    //@trc_union code == L2CAP_SMP_PAIRING_CFM_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_pairing_cfm_t    pairing_cfm;
    /// Pairing Random
    //@trc_union code == L2CAP_SMP_PAIRING_RANDOM_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_pairing_random_t pairing_random;
    /// Pairing Failed
    //@trc_union code == L2CAP_SMP_PAIRING_FAILED_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_pairing_failed_t pairing_failed;
    /// Encryption Information
    //@trc_union code == L2CAP_SMP_ENCRYPTION_INF_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_encryption_inf_t encryption_inf;
    /// Master Identification
    //@trc_union code == L2CAP_SMP_MASTER_ID_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_master_id_t      master_id;
    /// Identity Information
    //@trc_union code == L2CAP_SMP_IDENTITY_INF_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_identity_inf_t   identity_inf;
    /// Identity Address Information
    //@trc_union code == L2CAP_SMP_ID_ADDR_INF_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_id_addr_inf_t    id_addr_inf;
    /// Signing Information
    //@trc_union code == L2CAP_SMP_SIGNING_INF_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_signing_inf_t    signing_inf;
    /// Security Request
    //@trc_union code == L2CAP_SMP_SECURITY_REQ_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
   l2cap_smp_security_req_t    security_req;
    /// Public Keys
   //@trc_union code == L2CAP_SMP_PUBLIC_KEY_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_public_key_t     public_key;
    /// Key Press Notification
    //@trc_union code == L2CAP_SMP_KEYPRESS_NTF_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_keypress_ntf_t   keypress_ntf;
    /// DH Key Check
    //@trc_union code == L2CAP_SMP_DHKEY_CHECK_OPCODE and parent.chan_id == L2CAP_CID_SECURITY
    l2cap_smp_dhkey_check_t    dhkey_check;
};
typedef union l2cap_smp_pdu l2cap_smp_pdu_t;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */


/// @} L2CAP
///
#endif // (BLE_L2CAP)
#endif // L2CAP_SMP_H_
