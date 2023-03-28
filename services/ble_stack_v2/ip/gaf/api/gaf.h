/**
 ****************************************************************************************
 *
 * @file gaf.h
 *
 * @brief Generic Audio Framework - Header file
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef GAF_H_
#define GAF_H_

/**
 ****************************************************************************************
 * @addtogroup GAF Generic Audio Framework
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf_cfg.h"                // Generic Audio Framework Configuration
#include "gap.h"                    // GAP defines
#include "prf_types.h"              // Profile Definitions
#include "prf_utils.h"              // Profile Utilitaries

/*
 * DEFINES
 ****************************************************************************************
 */

/// Maximum number of ASE (must not be higher than 7 due to uint8_t size, see ASCS block)
#define GAF_ASE_NB_MAX                  (7)
/// Length of Codec ID value
#define GAF_CODEC_ID_LEN                (5)
/// Length of Vendor Specific Part of Codec ID
#define GAF_CODEC_ID_VENDOR_SPEC_LEN    (GAF_CODEC_ID_LEN - 1)
/// Invalid local index
#define GAF_INVALID_LID                 (0xFF)

/// Length of Advertising service data length
#define GAF_ADV_SVC_DATA_HEAD_LENGTH         (3)

/// Length of Broadcast Audio Scan Service advertising data
#define GAF_ADV_AUDIO_SCAN_SVC_DATA_LENGTH   (7)

// Minimum length for basic audio announcement
#define GAF_ADV_GRP_MIN_LEN                  (11)
#define GAF_ADV_SUBGRP_MIN_LEN               (4)
#define GAF_ADV_STREAM_MIN_LEN               (3)

/*
 * MACROS
 ****************************************************************************************
 */

/// Mask for GAF error code
#define GAF_ERR_CODE(idx)                    (0x0100 | idx)
/// Generate a command/request/request indication/indication code
#define GAF_CODE(layer, module, idx)         ((GAF_LAYER_ ## layer << 12)  | (layer ##_MODULE_ ## module << 8) | idx)
/// Retrieve layer from message code
#define GAF_LAYER(code)                      ((code & 0xF000) >> 12)
/// Retrieve module from message code
#define GAF_MODULE(code)                     ((code & 0x0F00) >> 8)
/// Retrieve code index from message code
#define GAF_CODE_IDX(code)                   (code & 0x00FF)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
/// GAF Layers
enum gaf_layers
{
    /// Generic Audio Framework
    GAF_LAYER_GAF       = 0,
    /// Isochronous Access Profile
    GAF_LAYER_IAP       = 1,
    /// Basic Audio Profile
    GAF_LAYER_BAP       = 2,
    /// Common Audio Profile
    GAF_LAYER_CAP       = 3,
    /// Audio Content Control
    GAF_LAYER_ACC       = 4,
    /// Audio Rendering Control
    GAF_LAYER_ARC       = 5,
    /// Audio Topology Control
    GAF_LAYER_ATC       = 6,

    GAF_LAYER_MAX,
};

/// Module type values
enum gaf_module_type
{
    /// Common
    GAF_MODULE_COMMON = 0,
    GAF_MODULE_MAX,
};

/// Error codes
enum gaf_err
{
    /// No error
    GAF_ERR_NO_ERROR = 0,
    /// Invalid parameters
    GAF_ERR_INVALID_PARAM           = GAF_ERR_CODE(0x01),
    /// Command disallowed
    GAF_ERR_COMMAND_DISALLOWED      = GAF_ERR_CODE(0x02),
    /// Unknown command
    GAF_ERR_UNKNOWN_COMMAND         = GAF_ERR_CODE(0x03),
    /// Unknown request
    GAF_ERR_UNKNOWN_REQUEST         = GAF_ERR_CODE(0x04),
    /// Insufficient resources
    GAF_ERR_INSUFFICIENT_RESOURCES  = GAF_ERR_CODE(0x05),
    /// Invalid connection
    GAF_ERR_INVALID_CONNECTION      = GAF_ERR_CODE(0x06),
    /// Busy
    GAF_ERR_BUSY                    = GAF_ERR_CODE(0x07),
    /// Failed
    GAF_ERR_FAILED                  = GAF_ERR_CODE(0x08),
    /// Read procedure successful but read value was not correct
    GAF_ERR_READ_ERROR              = GAF_ERR_CODE(0x09),
    /// Indicated direction is valid but not supported
    GAF_ERR_DIRECTION_NOT_SUPPORTED = GAF_ERR_CODE(0x0A),
    /// Invalid Output
    GAF_ERR_INVALID_OUTPUT          = GAF_ERR_CODE(0x0B),
    /// Invalid Input
    GAF_ERR_INVALID_INPUT           = GAF_ERR_CODE(0x0C),
    /// Invalid PAC
    GAF_ERR_INVALID_PAC             = GAF_ERR_CODE(0x0D),
    /// Optional feature not supported by peer device
    GAF_ERR_PEER_NOT_SUPPORT        = GAF_ERR_CODE(0x0E),
    /// Invalid Source
    GAF_ERR_INVALID_SRC             = GAF_ERR_CODE(0x0F),
    /// Invalid ASE
    GAF_ERR_INVALID_ASE             = GAF_ERR_CODE(0x10),

    /// Link Group already exists
    GAF_ERR_LINK_GROUP_EXISTS       = GAF_ERR_CODE(0x11),
    /// Link already exists
    GAF_ERR_LINK_EXISTS             = GAF_ERR_CODE(0x12),
    /// Invalid Link
    GAF_ERR_INVALID_LINK            = GAF_ERR_CODE(0x13),

    /// Not configured
    GAF_ERR_NOT_CONFIGURED          = GAF_ERR_CODE(0x14),

    /// Internal error
    GAF_ERR_INTERNAL_ERROR          = GAF_ERR_CODE(0x15),

    /// No ASE available
    GAF_ERR_NO_AVA_ASE              = GAF_ERR_CODE(0x16),
    /// Invalid Bearer
    GAF_ERR_INVALID_BEARER          = GAF_ERR_CODE(0x17),
    /// Invalid Call
    GAF_ERR_INVALID_CALL            = GAF_ERR_CODE(0x18),
    /// Invalid Media
    GAF_ERR_INVALID_MEDIA           = GAF_ERR_CODE(0x19),
    /// Optional feature not supported locally
    GAF_ERR_LOCAL_NOT_SUPPORT       = GAF_ERR_CODE(0x1A),
    /// Discovery error
    GAF_ERR_DISC_ERROR              = GAF_ERR_CODE(0x1B),
    /// Invalid Record
    GAF_ERR_INVALID_RECORD          = GAF_ERR_CODE(0x1C),
    /// Invalid ASE ID
    GAF_ERR_INVALID_ASE_ID          = GAF_ERR_CODE(0x1D),
    /// Disconnection has occurred on the ACL link during the procedure
    GAF_ERR_DISCONNECTED            = GAF_ERR_CODE(0x1E),
};

/// ASE Direction
enum gaf_direction
{
    /// Sink direction
    GAF_DIRECTION_SINK = 0,
    /// Source direction
    GAF_DIRECTION_SRC,

    GAF_DIRECTION_MAX,
};

/// Link Direction
enum gaf_link_direction
{
    /// Monodirectional link as sink (from server point of view)
    GAF_LINK_DIRECTION_MONODIR_SINK = 0x01,
    /// Monodirectional link as source (from server point of view)
    GAF_LINK_DIRECTION_MONODIR_SRC = 0x02,
    /// Bidirectional link
    GAF_LINK_DIRECTION_BIDIR = GAF_LINK_DIRECTION_MONODIR_SINK | GAF_LINK_DIRECTION_MONODIR_SRC,
};

/// Direction requirements bit field
enum gaf_direction_bf
{
    /// Required for sink direction
    GAF_DIRECTION_BF_SINK_POS = 0,
    GAF_DIRECTION_BF_SINK_BIT = 0x01,

    /// Required for source direction
    GAF_DIRECTION_BF_SRC_POS = 1,
    GAF_DIRECTION_BF_SRC_BIT = 0x02,

    /// Required for both directions
    GAF_DIRECTION_BF_BOTH = GAF_DIRECTION_BF_SRC_BIT + GAF_DIRECTION_BF_SINK_BIT,
};

/// Codec Type values
enum gaf_codec_type
{
    /// LC3 Codec
    GAF_CODEC_TYPE_LC3    = 0x00,

    /// Maximum SIG Codec
    GAF_CODEC_TYPE_SIG_MAX,

    /// Same codec as broadcast group codec use for broadcast subgroup
    GAF_CODEC_TYPE_SAME   = 0xFE,
    /// Vendor Specific Codec
    GAF_CODEC_TYPE_VENDOR = 0xFF,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */


/// Codec Identifier
typedef struct gaf_codec_id
{
    /// Codec ID value
    uint8_t codec_id[GAF_CODEC_ID_LEN];
} gaf_codec_id_t;


/// Data value in LTV format
typedef struct gaf_ltv
{
    /// Length of data value
    uint8_t len;
    /// Data value
    uint8_t data[__ARRAY_EMPTY];
} gaf_ltv_t;

/// Codec Information structure
typedef struct gaf_codec_info
{
    /// Codec ID
    uint8_t codec_id[GAF_CODEC_ID_LEN];
    /// Codec Specific Capabilities value length
    uint8_t codec_capa_len;
    /// Codec Specific Capabilities value
    uint8_t data[__ARRAY_EMPTY];
} gaf_codec_info_t;

/// Codec Information structure (pointer version)
typedef struct gaf_codec_info_ptr
{
    /// Codec ID
    uint8_t codec_id[GAF_CODEC_ID_LEN];
    /// Codec Specific Capabilities value length
    uint8_t codec_capa_len;
    /// Pointer to Codec Specific Capabilities value
    uint8_t* p_codec_capa;
} gaf_codec_info_ptr_t;

/// Codec Configuration structure
typedef struct gaf_codec_cfg
{
    /// Codec ID
    uint8_t codec_id[GAF_CODEC_ID_LEN];
    /// Length of Codec Specific Configuration
    uint8_t codec_cfg_len;
    /// Codec Specific Configuration
    uint8_t codec_cfg[__ARRAY_EMPTY];
} gaf_codec_cfg_t;

/// Codec Configuration structure (pointer version)
typedef struct gaf_codec_cfg_ptr
{
    /// Codec ID
    uint8_t codec_id[GAF_CODEC_ID_LEN];
    /// Length of Codec Specific Configuration
    uint8_t codec_cfg_len;
    /// Pointer to Codec Specific Configuration
    uint8_t* p_codec_cfg;
} gaf_codec_cfg_ptr_t;

/// QoS Requirements structure
typedef struct gaf_qos_req
{
    /// Minimum SDU interval in microseconds
    /// From 255us (0xFF) to 16777215us (0xFFFFFF)
    uint32_t sdu_intv_min_us;
    /// Maximum SDU interval in microseconds
    /// From 255us (0xFF) to 16777215us (0xFFFFFF)
    uint32_t sdu_intv_max_us;
    /// Maximum SDU size
    /// From 0 to 4095 bytes (0xFFF)
    uint16_t max_sdu_size;
    /// Minimum transport latency in milliseconds
    /// From 5ms (0x5) to 4000ms (0xFA0)
    uint16_t trans_latency_min_ms;
    /// Maximum transport latency in milliseconds
    /// From 5ms (0x5) to 4000ms (0xFA0)
    uint16_t trans_latency_max_ms;
    /// Indicate if Unframed ISOAL PDUs or Framed ISOAL PDUs are preferred
    uint8_t pref_framing;
    /// Indicate preferred PHY
    uint8_t pref_phy;
    /// Maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
    /// Minimum presentation delay in microseconds
    uint32_t pres_delay_min_us;
    /// Maximum presentation delay in microseconds
    uint32_t pres_delay_max_us;
} gaf_qos_req_t;

/// Full QoS Requirement structure
typedef struct gaf_qos_req_full
{
    /// QoS Requirements structure
    gaf_qos_req_t qos_req;
    /// Codec Configuration structure
    gaf_codec_cfg_t codec_cfg;
} gaf_qos_req_full_t;

/// Full QoS Requirement structure (pointer version)
typedef struct gaf_qos_req_full_ptr
{
    /// QoS Requirements structure
    gaf_qos_req_t qos_req;
    /// Codec Configuration structure
    gaf_codec_cfg_ptr_t codec_cfg;
} gaf_qos_req_full_ptr_t;

/// QoS Configuration structure
typedef struct gaf_qos_cfg
{
    /// ISOAL PDU framing arrangement
    uint8_t framing;
    /// PHY
    uint8_t phy;
    /// Maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
    /// Maximum SDU size
    /// From 0 to 4095 bytes (0xFFF)
    uint16_t max_sdu_size;
    /// Transport latency in milliseconds
    /// From 5ms (0x5) to 4000ms (0xFA0)
    uint16_t trans_latency_ms;
    /// Presentation delay in microseconds
    uint32_t pres_delay_us;
    /// SDU interval in microseconds
    /// From 255us (0xFF) to 16777215us (0xFFFFFF)
    uint32_t sdu_intv_us;
} gaf_qos_cfg_t;

/// ASE Configuration structure
typedef struct gaf_ase_cfg
{
    /// QoS Configuration structure
    gaf_qos_cfg_t qos_cfg;
    /// CIG ID
    uint8_t cig_id;
    /// CIS ID
    uint8_t cis_id;
} gaf_ase_cfg_t;

/// Content Configuration structure
typedef struct gaf_content_cfg
{
    /// Content type intended to be used on the ASE
    uint16_t active_content_bf;
    /// Number of Content Control IDs (CCIDs) in use for the ASE
    uint8_t nb_ccids;
    /// List of CCIDs
    uint8_t ccids[__ARRAY_EMPTY];
} gaf_content_cfg_t;

/// Content Configuation structure (pointer version)
typedef struct gaf_content_cfg_ptr
{
    /// Content type intended to be used on the ASE
    uint16_t active_content_bf;
    /// Number of Content Control IDs (CCIDs) in use for the ASE
    uint8_t nb_ccids;
    /// Pointer to list of CCIDs
    uint8_t* p_ccids;
} gaf_content_cfg_ptr_t;

/// Broadcast code used for stream encryption
typedef struct gaf_bcast_code_t
{
    /// Broadcast Code value
    uint8_t bcast_code[GAP_KEY_LEN];
} gaf_bcast_code_t;

/*
 * CALLBACK FUNCTIONS DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Callback function called when a command has been completed.
 *
 * @param[in] cmd_code          Command code
 * @param[in] status            Status
 ****************************************************************************************
 */
typedef void (*gaf_cb_cmp_evt)(uint16_t cmd_code, uint16_t status);

/// @}

#endif // GAF_H_
