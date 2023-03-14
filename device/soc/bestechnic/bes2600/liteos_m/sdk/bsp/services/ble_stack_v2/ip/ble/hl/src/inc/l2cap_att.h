/**
 ****************************************************************************************
 *
 * @file l2cap_att.h
 *
 * @brief Header file - L2CAP Attribute Protocol Defines
 *
 * Copyright (C) RivieraWaves 2009-2019
 ****************************************************************************************
 */

#ifndef L2CAP_ATT_H_
#define L2CAP_ATT_H_

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
/// Attribute PDU header length (op_code)
#define L2CAP_ATT_HEADER_LEN        (1)
/// Each Len field length
#define L2CAP_ATT_EACHLEN_LEN       (1)
/// Value Length field length
#define L2CAP_ATT_VALLEN_LEN        (2)
/// F field length
#define L2CAP_ATT_FORMAT_LEN        (1)
/// Length of offset field
#define L2CAP_ATT_OFFSET_LEN        (2)
/// Length of Characteristic property length
#define L2CAP_ATT_PROP_LEN          (1)
/// length of signature counter
#define L2CAP_ATT_SIGN_COUNTER_LEN  (4)
/// length of signature MAC
#define L2CAP_ATT_SIGN_MAC_LEN      (8)
/// length of the signature
#define L2CAP_ATT_SIGN_LEN          (L2CAP_ATT_SIGN_COUNTER_LEN + L2CAP_ATT_SIGN_MAC_LEN)

/// Perform write execution
#define L2CAP_ATT_WRITE_EXECUTE     (0x01)
/// Cancel prepare write queue
#define L2CAP_ATT_WRITE_CANCEL      (0x00)

/// 16-bit UUID format - used in att_find_info_rsp
#define L2CAP_ATT_UUID_16_FORMAT    (0x01)
/// 128-bit UUID format - used in att_find_info_rsp
#define L2CAP_ATT_UUID_128_FORMAT   (0x02)


/// Operation coded bit field
///    7      6    5   4   3   2   1   0
/// +------+-----+---+---+---+---+---+---+
/// | SIGN | CMD |        METHOD         |
/// +------+-----+---+---+---+---+---+---+
enum l2cap_att_opcode
{
    /// Attribute operation code - Method (bit 5 - 0)
    L2CAP_ATT_OPCODE_METHOD_MASK                  = 0x3F,
    L2CAP_ATT_OPCODE_METHOD_LSB                   = 0,
    /// Attribute operation code - Command Flag (bit 6)
    L2CAP_ATT_OPCODE_CMD_FLAG_BIT                 = 0x40,
    L2CAP_ATT_OPCODE_CMD_FLAG_POS                 = 6,
    /// Attribute operation code - Authentication Signature Flag (bit 7)
    L2CAP_ATT_OPCODE_AUTH_SIGNATURE_FLAG_BIT      = 0x80,
    L2CAP_ATT_OPCODE_AUTH_SIGNATURE_FLAG_POS      = 7,
};

/// L2CAP attribute Op-Codes
/*@TRACE*/
enum l2cap_att_code
{
    /// Reserved
    L2CAP_ATT_RESERVED0_OPCODE          = 0x00,
    /// Error response
    L2CAP_ATT_ERR_RSP_OPCODE            = 0x01,
    /// Exchange MTU Request
    L2CAP_ATT_MTU_REQ_OPCODE            = 0x02,
    /// Exchange MTU Response
    L2CAP_ATT_MTU_RSP_OPCODE            = 0x03,
    /// Find Information Request
    L2CAP_ATT_FIND_INFO_REQ_OPCODE      = 0x04,
    /// Find Information Response
    L2CAP_ATT_FIND_INFO_RSP_OPCODE      = 0x05,
    /// Find By Type Value Request
    L2CAP_ATT_FIND_BY_TYPE_REQ_OPCODE   = 0x06,
    /// Find By Type Value Response
    L2CAP_ATT_FIND_BY_TYPE_RSP_OPCODE   = 0x07,
    /// Read By Type Request
    L2CAP_ATT_RD_BY_TYPE_REQ_OPCODE     = 0x08,
    /// Read By Type Response
    L2CAP_ATT_RD_BY_TYPE_RSP_OPCODE     = 0x09,
    /// Read Request
    L2CAP_ATT_RD_REQ_OPCODE             = 0x0A,
    /// Read Response
    L2CAP_ATT_RD_RSP_OPCODE             = 0x0B,
    /// Read Blob Request
    L2CAP_ATT_RD_BLOB_REQ_OPCODE        = 0x0C,
    /// Read Blob Response
    L2CAP_ATT_RD_BLOB_RSP_OPCODE        = 0x0D,
    /// Read Multiple Request
    L2CAP_ATT_RD_MULT_REQ_OPCODE        = 0x0E,
    /// Read Multiple Response
    L2CAP_ATT_RD_MULT_RSP_OPCODE        = 0x0F,
    /// Read by Group Type Request
    L2CAP_ATT_RD_BY_GRP_TYPE_REQ_OPCODE = 0x10,
    /// Read By Group Type Response
    L2CAP_ATT_RD_BY_GRP_TYPE_RSP_OPCODE = 0x11,
    /// Write Request / Command / Signed
    L2CAP_ATT_WR_REQ_OPCODE             = 0x12,
    /// Write Response
    L2CAP_ATT_WR_RSP_OPCODE             = 0x13,
    /// Reserved
    L2CAP_ATT_RESERVED1_OPCODE          = 0x14,
    /// Reserved
    L2CAP_ATT_RESERVED2_OPCODE          = 0x15,
    /// Prepare Write Request
    L2CAP_ATT_PREP_WR_REQ_OPCODE        = 0x16,
    /// Prepare Write Response
    L2CAP_ATT_PREP_WR_RSP_OPCODE        = 0x17,
    /// Execute Write Request
    L2CAP_ATT_EXE_WR_REQ_OPCODE         = 0x18,
    /// Execute Write Response
    L2CAP_ATT_EXE_WR_RSP_OPCODE         = 0x19,
    /// Reserved
    L2CAP_ATT_RESERVED3_OPCODE          = 0x1A,
    /// Handle Value Notification
    L2CAP_ATT_HDL_VAL_NTF_OPCODE        = 0x1B,
    /// Reserved
    L2CAP_ATT_RESERVED4_OPCODE          = 0x1C,
    /// Handle Value Indication
    L2CAP_ATT_HDL_VAL_IND_OPCODE        = 0x1D,
    /// Handle Value Confirmation
    L2CAP_ATT_HDL_VAL_CFM_OPCODE        = 0x1E,
    /// Reserved
    L2CAP_ATT_RESERVED5_OPCODE          = 0x1F,
    /// Read two or more values of a set of attributes that have a variable or unknown value length request
    L2CAP_ATT_RD_MULT_VAR_REQ_OPCODE    = 0x20,
    /// Read two or more values of a set of attributes that have a variable or unknown value length response
    L2CAP_ATT_RD_MULT_VAR_RSP_OPCODE    = 0x21,
    /// Reserved
    L2CAP_ATT_RESERVED6_OPCODE          = 0x22,
     /// Handle Multiple Value Notification
    L2CAP_ATT_MULT_HDL_VAL_NTF_OPCODE   = 0x23,

    /// max number of security codes
    L2CAP_ATT_OPCODE_MAX,

    /// Write Command
    L2CAP_ATT_WR_CMD_OPCODE             = 0x52,
    /// Write Signed Command
    L2CAP_ATT_WR_SIGNED_OPCODE          = 0xD2,
};

/// L2CAP attribute base message length
enum l2cap_att_msg_len
{
    /// Reserved
    L2CAP_ATT_RESERVED0_LEN             = 0,
    /// Error response
    L2CAP_ATT_ERR_RSP_LEN               = 4,
    /// Exchange MTU Request
    L2CAP_ATT_MTU_REQ_LEN               = 2,
    /// Exchange MTU Response
    L2CAP_ATT_MTU_RSP_LEN               = 2,
    /// Find Information Request
    L2CAP_ATT_FIND_INFO_REQ_LEN         = 4,
    /// Find Information Response
    L2CAP_ATT_FIND_INFO_RSP_LEN         = 1,
    /// Find By Type Value Request
    L2CAP_ATT_FIND_BY_TYPE_REQ_LEN      = 4,
    /// Find By Type Value Response
    L2CAP_ATT_FIND_BY_TYPE_RSP_LEN      = 0,
    /// Read By Type Request
    L2CAP_ATT_RD_BY_TYPE_REQ_LEN        = 4,
    /// Read By Type Response
    L2CAP_ATT_RD_BY_TYPE_RSP_LEN        = 1,
    /// Read Request
    L2CAP_ATT_RD_REQ_LEN                = 2,
    /// Read Response
    L2CAP_ATT_RD_RSP_LEN                = 0,
    /// Read Blob Request
    L2CAP_ATT_RD_BLOB_REQ_LEN           = 4,
    /// Read Blob Response
    L2CAP_ATT_RD_BLOB_RSP_LEN           = 0,
    /// Read Multiple Request
    L2CAP_ATT_RD_MULT_REQ_LEN           = 0,
    /// Read Multiple Response
    L2CAP_ATT_RD_MULT_RSP_LEN           = 0,
    /// Read by Group Type Request
    L2CAP_ATT_RD_BY_GRP_TYPE_REQ_LEN    = 4,
    /// Read By Group Type Response
    L2CAP_ATT_RD_BY_GRP_TYPE_RSP_LEN    = 1,
    /// Write Request / Command / Signed
    L2CAP_ATT_WR_REQ_LEN                = 2,
    /// Write Response
    L2CAP_ATT_WR_RSP_LEN                = 0,
    /// Reserved
    L2CAP_ATT_RESERVED1_LEN             = 0,
    /// Reserved
    L2CAP_ATT_RESERVED2_LEN             = 0,
    /// Prepare Write Request
    L2CAP_ATT_PREP_WR_REQ_LEN           = 4,
    /// Prepare Write Response
    L2CAP_ATT_PREP_WR_RSP_LEN           = 4,
    /// Execute Write Request
    L2CAP_ATT_EXE_WR_REQ_LEN            = 1,
    /// Execute Write Response
    L2CAP_ATT_EXE_WR_RSP_LEN            = 0,
    /// Reserved
    L2CAP_ATT_RESERVED3_LEN             = 0,
    /// Handle Value Notification
    L2CAP_ATT_HDL_VAL_NTF_LEN           = 2,
    /// Reserved
    L2CAP_ATT_RESERVED4_LEN             = 0,
    /// Handle Value Indication
    L2CAP_ATT_HDL_VAL_IND_LEN           = 2,
    /// Handle Value Confirmation
    L2CAP_ATT_HDL_VAL_CFM_LEN           = 0,
    /// Reserved
    L2CAP_ATT_RESERVED5_LEN             = 0,
    /// Read two or more values of a set of attributes that have a variable or unknown value length request
    L2CAP_ATT_RD_MULT_VAR_REQ_LEN       = 0,
    /// Read two or more values of a set of attributes that have a variable or unknown value length response
    L2CAP_ATT_RD_MULT_VAR_RSP_LEN       = 0,
    /// Reserved
    L2CAP_ATT_RESERVED6_LEN             = 0,
     /// Handle Multiple Value Notification
    L2CAP_ATT_MULT_HDL_VAL_NTF_LEN      = 0,


    L2CAP_ATT_MAX_LEN                   = 6,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// Error response
/*@TRACE*/
typedef struct l2cap_att_err_rsp
{
    /// Error Response - 0x01
    uint8_t     code;
    /// The request that generated this error response
    uint8_t     op_code;
    /// The attribute handle that generated this error response
    uint16_t    handle;
    ///The reason why the request has generated an error response
    uint8_t     reason;
} l2cap_att_err_rsp_t;

/// Exchange MTU Request
/*@TRACE*/
typedef struct l2cap_att_mtu_req
{
    /// Exchange MTU Request - 0x02
    uint8_t     code;
    /// Client Rx MTU size
    uint16_t    mtu_size;
} l2cap_att_mtu_req_t;

/// Exchange MTU Response
/*@TRACE*/
typedef struct l2cap_att_mtu_rsp
{
    /// Exchange MTU Response - 0x03
    uint8_t     code;
    /// Server Rx MTU size
    uint16_t    mtu_size;
} l2cap_att_mtu_rsp_t;

/// Find Information Request
/*@TRACE*/
typedef struct l2cap_att_find_info_req
{
    /// Find Information Request - 0x04
    uint8_t     code;
    /// First requested handle number
    uint16_t    shdl;
    /// Last requested handle number
    uint16_t    ehdl;
} l2cap_att_find_info_req_t;

/// Find Information Response
/*@TRACE*/
typedef struct l2cap_att_find_info_rsp
{
    /// Find Information Response - 0x05
    uint8_t     code;
    /// The format of the information data.
    uint8_t     format;
    // Then List of handle + 16-bit or 128-bit UUID
} l2cap_att_find_info_rsp_t;

/// Find By Type Value Request
/*@TRACE*/
typedef struct l2cap_att_find_by_type_req
{
    /// Find By Type Value Request - 0x06
    uint8_t     code;
    /// First requested handle number
    uint16_t    shdl;
    /// Last requested handle number
    uint16_t    ehdl;
    // 2 octet UUID to find
    // Then Value of searched handle
} l2cap_att_find_by_type_req_t;

/// Find By Type Value Response
/*@TRACE*/
typedef struct l2cap_att_find_by_type_rsp
{
    /// Find By Type Value Response - 0x07
    uint8_t     code;
    // Then List of Found attribute handle + Group End Handle
} l2cap_att_find_by_type_rsp_t;

/// Read By Type Request
/*@TRACE*/
typedef struct l2cap_att_rd_by_type_req
{
    /// Read By Type Request - 0x08
    uint8_t     code;
    /// Starting Handle
    uint16_t    shdl;
    /// Ending Handle
    uint16_t    ehdl;
    // Then Attribute UUID : 16-bit or 128-bit
} l2cap_att_rd_by_type_req_t;

/// Read By Type Response
/*@TRACE*/
typedef struct l2cap_att_rd_by_type_rsp
{
    /// Read By Type Response - 0x09
    uint8_t     code;
    /// size of each attribute handle listed
    uint8_t     each_len;
    // Then list of Attribute Handle - Attribute Value (size: each_len - 2)
} l2cap_att_rd_by_type_rsp_t;

/// Read Request
/*@TRACE*/
typedef struct l2cap_att_rd_req
{
    /// Read Request - 0x0A
    uint8_t     code;
    /// Attribute Handle
    uint16_t    handle;
} l2cap_att_rd_req_t;

/// Read Response
/*@TRACE*/
typedef struct l2cap_att_rd_rsp
{
    /// Read Response - 0x0B
    uint8_t    code;
    // Then Attribute value read
} l2cap_att_rd_rsp_t;

/// Read Blob Request
/*@TRACE*/
typedef struct l2cap_att_rd_blob_req
{
    /// Read Blob Request - 0x0C
    uint8_t     code;
    /// Attribute Handle
    uint16_t    handle;
    /// The offset of the first octet to be read
    uint16_t    offset;
} l2cap_att_rd_blob_req_t;

/// Read Blob Response
/*@TRACE*/
typedef struct l2cap_att_rd_blob_rsp
{
    /// Read Blob Response - 0x0D
    uint8_t    code;
    /// value length
    uint16_t   value_len;
    // Then Attribute value read
} l2cap_att_rd_blob_rsp_t;

/// Read Multiple Request
/*@TRACE*/
typedef struct l2cap_att_rd_mult_req
{
    /// Read Multiple Request - 0x0E
    uint8_t     code;
    // Then List of Handle
} l2cap_att_rd_mult_req_t;

/// Read Multiple Response
/*@TRACE*/
typedef struct l2cap_att_rd_mult_rsp
{
    /// Read Multiple Response - 0x0F
    uint8_t    code;
    // Then Data with multiple attribute value
} l2cap_att_rd_mult_rsp_t;

/// Read by Group Type Request
/*@TRACE*/
typedef struct l2cap_att_rd_by_grp_type_req
{
    /// Read by Group Type Request - 0x10
    uint8_t     code;
    /// First requested handle number
    uint16_t    shdl;
    /// Last requested handle number
    uint16_t    ehdl;
    //  Then Attribute Group UUID : 16-bit or 128-bit
} l2cap_att_rd_by_grp_type_req_t;

/// Read By Group Type Response
/*@TRACE*/
typedef struct l2cap_att_rd_by_grp_type_rsp
{
    /// Read By Group Type Response - 0x11
    uint8_t     code;
    /// size of each attribute handle listed
    uint8_t     each_len;
    // Then list of Attribute Handle - End Group Handle - Attribute Value (size: each_len - 4)
} l2cap_att_rd_by_grp_type_rsp_t;

/// Write Request
/*@TRACE*/
typedef struct l2cap_att_wr_req
{
    /// Write Request - 0x12 / Also work for write command and write signed
    uint8_t     code;
    /// The handle of the attribute to be written
    uint16_t    handle;
    // Then Value
} l2cap_att_wr_req_t;

/// Write Response
/*@TRACE*/
typedef struct l2cap_att_wr_rsp
{
    /// Write Response - 0x13
    uint8_t     code;
} l2cap_att_wr_rsp_t;

/// Prepare Write Request
/*@TRACE*/
typedef struct l2cap_att_prep_wr_req
{
    /// Prepare Write Request - 0x16
    uint8_t     code;
    /// The handle of the attribute to be written
    uint16_t    handle;
    /// The offset of the first octet to be written
    uint16_t    offset;
    // Then Value
} l2cap_att_prep_wr_req_t;

/// Prepare Write Response
/*@TRACE*/
typedef struct l2cap_att_prep_wr_rsp
{
    /// Prepare Write Response - 0x17
    uint8_t     code;
    /// The handle of the attribute to be written
    uint16_t    handle;
    /// The offset of the first octet to be written
    uint16_t    offset;
    // Then Value
} l2cap_att_prep_wr_rsp_t;

/// Execute Write Request
/*@TRACE*/
typedef struct l2cap_att_exe_wr_req
{
    /// Execute Write Request - 0x18
    uint8_t     code;
    /// 0x00 - Cancel all prepared writes
    /// 0x01 - Immediately write all pending prepared values
    uint8_t     flags;
} l2cap_att_exe_wr_req_t;

/// Execute Write Response
/*@TRACE*/
typedef struct l2cap_att_exe_wr_rsp
{
    /// Prepare Write Response - 0x19
    uint8_t     code;
} l2cap_att_exe_wr_rsp_t;

/// Handle Value Notification
/*@TRACE*/
typedef struct l2cap_att_hdl_val_ntf
{
    /// Handle Value Notification - 0x1B
    uint8_t     code;
    /// The handle of the attribute to be written
    uint16_t    handle;
    // Then Value
} l2cap_att_hdl_val_ntf_t;

/// Handle Value Indication
/*@TRACE*/
typedef struct l2cap_att_hdl_val_ind
{
    /// Handle Value Indication - 0x1D
    uint8_t     code;
    /// The handle of the attribute to be written
    uint16_t    handle;
    // Then Value
} l2cap_att_hdl_val_ind_t;

/// Handle Value Confirmation
/*@TRACE*/
typedef struct l2cap_att_hdl_val_cfm
{
    /// Handle Value Confirmation - 0x1E
    uint8_t     code;
} l2cap_att_hdl_val_cfm_t;

/// Handle read multiple variable length value request
/*@TRACE*/
typedef struct l2cap_att_rd_mult_var_req
{
    /// Handle read multiple variable length value request - 0x20
    uint8_t     code;
    // then list of handles
} l2cap_att_rd_mult_var_req_t;

/// Handle read multiple variable length value response
/*@TRACE*/
typedef struct l2cap_att_rd_mult_var_rsp
{
    /// Handle read multiple variable length value response - 0x21
    uint8_t     code;
    // then list of (length + value)
} l2cap_att_rd_mult_var_rsp_t;

/// Handle Multiple Value Notification
/*@TRACE*/
typedef struct l2cap_att_mult_hdl_val_ntf
{
    /// Handle Multiple Value Notification - 0x23
    uint8_t     code;
    // then list of handles
} l2cap_att_mult_hdl_val_ntf_t;




/*@TRACE
 @trc_ref l2cap_cid, l2cap_att_code
 */
union l2cap_att_pdu
{
    /// L2Cap packet code.
    uint8_t code;

    // Attribute protocol PDUs
    /// Error response
    //@trc_union code == L2CAP_ATT_ERR_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_err_rsp_t             err_rsp;
    /// Exchange MTU Request
    //@trc_union code == L2CAP_ATT_MTU_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_mtu_req_t             mtu_req;
    /// Exchange MTU Response
    //@trc_union code == L2CAP_ATT_MTU_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_mtu_rsp_t             mtu_rsp;
    /// Find Information Request
    //@trc_union code == L2CAP_ATT_FIND_INFO_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_find_info_req_t       find_info_req;
    /// Find Information Response
    //@trc_union code == L2CAP_ATT_FIND_INFO_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_find_info_rsp_t       find_info_rsp;
    /// Find By Type Value Request
    //@trc_union code == L2CAP_ATT_FIND_BY_TYPE_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_find_by_type_req_t    find_by_type_req;
    /// Find By Type Value Response
    //@trc_union code == L2CAP_ATT_FIND_BY_TYPE_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_find_by_type_rsp_t    find_by_type_rsp;
    /// Read By Type Request
    //@trc_union code == L2CAP_ATT_RD_BY_TYPE_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_by_type_req_t      rd_by_type_req;
    /// Read By Type Response
    //@trc_union code == L2CAP_ATT_RD_BY_TYPE_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_by_type_rsp_t      rd_by_type_rsp;
    /// Read Request
    //@trc_union code == L2CAP_ATT_RD_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_req_t              rd_req;
    /// Read Response
    //@trc_union code == L2CAP_ATT_RD_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_rsp_t              rd_rsp;
    /// Read Blob Request
    //@trc_union code == L2CAP_ATT_RD_BLOB_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_blob_req_t         rd_blob_req;
    /// Read Blob Response
    //@trc_union code == L2CAP_ATT_RD_BLOB_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_blob_rsp_t         rd_blob_rsp;
    /// Read Multiple Request
    //@trc_union code == L2CAP_ATT_RD_MULT_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_mult_req_t         rd_mult_req;
    /// Read Multiple Response
    //@trc_union code == L2CAP_ATT_RD_MULT_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_mult_rsp_t         rd_mult_rsp;
    /// Read by Group Type Request
    //@trc_union code == L2CAP_ATT_RD_BY_GRP_TYPE_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_by_grp_type_req_t  rd_by_grp_type_req;
    /// Read By Group Type Response
    //@trc_union code == L2CAP_ATT_RD_BY_GRP_TYPE_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_by_grp_type_rsp_t  rd_by_grp_type_rsp;
    /// Write Request
    //@trc_union code == L2CAP_ATT_WR_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_wr_req_t              wr_req;
    /// Write Response
    //@trc_union code == L2CAP_ATT_WR_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_wr_rsp_t              wr_rsp;
    /// Prepare Write Request
    //@trc_union code == L2CAP_ATT_PREP_WR_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_prep_wr_req_t         prep_wr_req;
    /// Prepare Write Response
    //@trc_union code == L2CAP_ATT_PREP_WR_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_prep_wr_rsp_t         prep_wr_rsp;
    /// Execute Write Request
    //@trc_union code == L2CAP_ATT_EXE_WR_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_exe_wr_req_t          exe_wr_req;
    /// Execute Write Response
    //@trc_union code == L2CAP_ATT_EXE_WR_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_exe_wr_rsp_t          exe_wr_rsp;
    /// Handle Value Notification
    //@trc_union code == L2CAP_ATT_HDL_VAL_NTF_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_hdl_val_ntf_t         hdl_val_ntf;
    /// Handle Value Indication
    //@trc_union code == L2CAP_ATT_HDL_VAL_IND_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_hdl_val_ind_t         hdl_val_ind;
    /// Handle Value Confirmation
    //@trc_union code == L2CAP_ATT_HDL_VAL_CFM_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_hdl_val_cfm_t         hdl_val_cfm;
    /// Handle read multiple variable length value request
    //@trc_union code == L2CAP_ATT_RD_MULT_VAR_REQ_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_mult_var_req_t     rd_mult_var_req;
    /// Handle read multiple variable length value response
    //@trc_union code == L2CAP_ATT_RD_MULT_VAR_RSP_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_rd_mult_var_rsp_t     rd_mult_var_rsp;
    /// Handle Multiple Value Notification
    //@trc_union code == L2CAP_ATT_MULT_HDL_VAL_NTF_OPCODE and parent.chan_id == L2CAP_CID_ATTRIBUTE
    l2cap_att_mult_hdl_val_ntf_t    hdl_mult_val_ntf;
};
typedef union l2cap_att_pdu l2cap_att_pdu_t;

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
