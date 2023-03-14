/**
 ****************************************************************************************
 *
 * @file bap.h
 *
 * @brief Basic Audio Profile - Header file
 *
 * Copyright (C) RivieraWaves 2019-2020
 *
 ****************************************************************************************
 */

#ifndef BAP_H_
#define BAP_H_

/**
 ****************************************************************************************
 * @addtogroup BAP Basic Audio Profile
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gaf_cfg.h"         // GAF Configuration
#include "gaf.h"             // Code definitions

/*
 * ENUMERATIONS
 ****************************************************************************************
 */

/// Module type values
enum bap_module_type
{
    /// Common
    BAP_MODULE_COMMON = 0,
    /// Basic Audio Profile Capabilities Server
    BAP_MODULE_CAPA_SRV = 1,
    /// Basic Audio Profile Capabilities Client
    BAP_MODULE_CAPA_CLI = 2,
    /// Basic Audio Profile Unicast Server
    BAP_MODULE_UC_SRV = 3,
    /// Basic Audio Profile Unicast Client
    BAP_MODULE_UC_CLI = 4,
    /// Basic Audio Profile Broadcast Source
    BAP_MODULE_BC_SRC = 5,
    /// Basic Audio Profile Broadcast Sink
    BAP_MODULE_BC_SINK = 6,
    /// Basic Audio Profile Broadcast Scan
    BAP_MODULE_BC_SCAN = 7,
    /// Basic Audio Profile Broadcast Assistant
    BAP_MODULE_BC_ASSIST = 8,
    /// Basic Audio Profile Broadcast Delegator
    BAP_MODULE_BC_DELEG = 9,

    BAP_MODULE_MAX,
};

/// List of GAF_REQ request codes
enum bap_msg_req_codes
{
    /// Configure Basic Audio Profile
    BAP_CONFIGURE = GAF_CODE(BAP, COMMON, 0),
    /// Get supported roles
    BAP_GET_ROLES = GAF_CODE(BAP, COMMON, 1),
};

/// Position of fields in LTV structure
enum bap_ltv_pos
{
    /// Length
    BAP_LTV_LENGTH_POS = 0,
    /// Type
    BAP_LTV_TYPE_POS,
    /// Value
    BAP_LTV_VALUE_POS,

    /// Minimal length of LTV structure
    BAP_LTV_LENGTH_MIN = 1,
};

/// Type values for Metadata LTV structure
enum bap_ltv_type_metadata
{
    /// Preferred Audio Contexts
    BAP_LTV_TYPE_METADATA_PREF_CONTEXTS = 0,
    /// Streaming Audio Contexts
    BAP_LTV_TYPE_METADATA_STREAM_CONTEXTS,
};

/// (Minimal) value of length field for Metadata LTV structure
enum bap_ltv_length_metadata
{
    /// Preferred Audio Contexts
    BAP_LTV_LENGTH_METADATA_PREF_CONTEXTS = 3,
    /// Streaming Audio Contexts
    BAP_LTV_LENGTH_METADATA_STREAM_CONTEXTS = 3,
};

/// Type values for LC3 Specific Capabilities LTV structure
enum bap_ltv_type_lc3_capa
{
    /// Supported Sampling Frequencies
    BAP_LTV_TYPE_LC3_CAPA_SUPP_SAMP_FREQ = 0,
    /// Supported Frame Durations
    BAP_LTV_TYPE_LC3_CAPA_SUPP_FRAME_DUR,
    /// Audio Channel Counts
    BAP_LTV_TYPE_LC3_CAPA_CHNL_CNT,
    /// Supported Octets per Codec Frame
    BAP_LTV_TYPE_LC3_CAPA_SUPP_OCTETS_FRAME,
    /// Maximum Supported LC3 Frames per SDU
    BAP_LTV_TYPE_LC3_CAPA_SUPP_FRAMES_SDU,
};

/// (Minimal) value of length field for LC3 Specific Capabilities LTV structure
enum bap_ltv_length_lc3_capa
{
    /// Supported Sampling Frequencies
    BAP_LTV_LENGTH_LC3_CAPA_SUPP_SAMP_FREQ = 3,
    /// Supported Frame Durations
    BAP_LTV_LENGTH_LC3_CAPA_SUPP_FRAME_DUR = 2,
    /// Audio Channel Counts
    BAP_LTV_LENGTH_LC3_CAPA_CHNL_CNT = 2,
    /// Supported Octets per Codec Frame
    BAP_LTV_LENGTH_LC3_CAPA_SUPP_OCTETS_FRAME = 5,
    /// Maximum Supported LC3 Frames per SDU
    BAP_LTV_LENGTH_LC3_CAPA_SUPP_FRAMES_SDU = 2,
};

/// Supported Roles bit field meaning
enum bap_role_bf
{
    /// Capabilities Server supported
    BAP_ROLE_SUPP_CAPA_SRV_BIT = 0x0001,
    BAP_ROLE_SUPP_CAPA_SRV_POS = 0,
    /// Capabilities Client supported
    BAP_ROLE_SUPP_CAPA_CLI_BIT = 0x0002,
    BAP_ROLE_SUPP_CAPA_CLI_POS = 1,
    /// Unicast Server supported
    BAP_ROLE_SUPP_UC_SRV_BIT = 0x0004,
    BAP_ROLE_SUPP_UC_SRV_POS = 2,
    /// Unicast Client supported
    BAP_ROLE_SUPP_UC_CLI_BIT = 0x0008,
    BAP_ROLE_SUPP_UC_CLI_POS = 3,
    /// Broadcast Source supported
    BAP_ROLE_SUPP_BC_SRC_BIT = 0x0010,
    BAP_ROLE_SUPP_BC_SRC_POS = 4,
    /// Broadcast Sink supported
    BAP_ROLE_SUPP_BC_SINK_BIT = 0x0020,
    BAP_ROLE_SUPP_BC_SINK_POS = 5,
    /// Broadcast Scan supported
    BAP_ROLE_SUPP_BC_SCAN_BIT = 0x0040,
    BAP_ROLE_SUPP_BC_SCAN_POS = 6,
    /// Broadcast Scan Assistant supported
    BAP_ROLE_SUPP_BC_ASSIST_BIT = 0x0080,
    BAP_ROLE_SUPP_BC_ASSIST_POS = 7,
    /// Broadcast Scan Delegator supported
    BAP_ROLE_SUPP_BC_DELEG_BIT  = 0x0100,
    BAP_ROLE_SUPP_BC_DELEG_POS  = 8,
};

/*
 * TYPE DEFINITION
 ****************************************************************************************
 */

typedef struct bap_capa_srv_cfg
{
    /// Number of PAC Groups for Sink direction
    uint8_t nb_pacs_sink;
    /// Number of PAC Groups for Source direction
    uint8_t nb_pacs_src;
    /// Configuration bit field (@see TODO [LT])
    uint8_t cfg_bf;
    /// Preferred MTU
    /// Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
    /// Required start handle
    /// If set to GATT_INVALID_HDL, the start handle will be automatically chosen
    uint16_t shdl;
    /// Supported Audio Locations bit field for Sink direction
    /// Meaningful only if nb_pac_sink is different than 0
    uint32_t location_bf_sink;
    /// Supported Audio Locations bit field for Source direction
    /// Meaningful only if nb_pac_src is different than 0
    uint32_t location_bf_src;
    /// Supported Audio Contexts bit field for Sink direction
    /// Meaningful only if nb_pac_sink is different than 0
    uint16_t supp_context_bf_sink;
    /// Supported Audio Contexts bit field for Source direction
    /// Meaningful only if nb_pac_src is different than 0
    uint16_t supp_context_bf_src;
} bap_capa_srv_cfg_t;

typedef struct bap_capa_cli_cfg
{
    /// Preferred MTU
    /// Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
} bap_capa_cli_cfg_t;

/// Configuration structure for BAP Unicast Server module
typedef struct bap_uc_srv_cfg
{
    /// Number of instances of the ASE characteristic
    /// Shall be in the range ]0, 15]
    uint8_t nb_ase_chars;
    /// Number of ASE configurations that can be maintained
    /// Shall be at least equal to nb_ase_chars
    /// Should be a multiple of nb_ase_chars
    /// Shall not be larger than nb_ase_chars * BLE_CONNECTION_MAX
    uint8_t nb_ases_cfg;
    /// Preferred MTU
    /// Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
    /// Required start handle
    /// If set to GATT_INVALID_HDL, the start handle will automatically chosen
    uint16_t shdl;
} bap_uc_srv_cfg_t;

/// Configuration structure for BAP Unicast Client module
typedef struct bap_uc_cli_cfg
{
    /// Configuration bit field
    uint8_t cfg_bf;
    /// Number of ASE configurations that can be maintained
    /// Shall be at least equal to nb_ase_chars
    /// Should be a multiple of nb_ase_chars
    /// Shall not be larger than nb_ase_chars * BLE_CONNECTION_MAX
    uint8_t nb_ases_cfg;
    /// Preferred MTU
    /// Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
} bap_uc_cli_cfg_t;

/// Broadcast Scanner configuration structure
typedef struct bap_bc_scan_cfg
{
    /// Number of Broadcast Source information that may be stored in the cache
    uint8_t cache_size;
    /// Number of Periodic Synchronizations that may be established in parallel
    uint8_t nb_sync;
} bap_bc_scan_cfg_t;

/// Broadcast Assistant configuration structure
typedef struct bap_bc_assist_cfg
{
    /// Preferred MTU - Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
} bap_bc_assist_cfg_t;

/// Broadcast Delegator configuration structure
typedef struct bap_bc_deleg_cfg
{
    /// Number of supported Broadcast Sources Up to 15
    uint8_t  nb_srcs;
    /// Required start handle - If set to GATT_INVALID_HDL, the start handle will be automatically chosen
    uint16_t shdl;
    /// Preferred MTU - Values from 0 to 63 are equivalent to 64
    uint16_t pref_mtu;
} bap_bc_deleg_cfg_t;

/// Broadcast Group Parameters structure
typedef struct bap_bc_grp_param
{
    /// SDU interval in microseconds
    uint32_t sdu_intv_us;
    /// Maximum size of an SDU
    uint16_t max_sdu;
    /// Maximum time (in milliseconds) between the first transmission of an SDU to the end of the last transmission
    /// of the same SDU
    uint16_t max_tlatency;
    /// Sequential or Interleaved scheduling
    uint8_t packing;
    /// Unframed or framed mode
    uint8_t framing;
    /// Bitfield indicating PHYs that can be used by the controller for transmission of SDUs
    uint8_t phy_bf;
    /// Number of times every PDU should be transmitted
    uint8_t rtn;
}bap_bc_grp_param_t;

/// Advertising Parameters structure
typedef struct bap_bc_adv_param
{
    /// Minimum advertising interval in multiple of 0.625ms. Must be higher than 20ms.
    uint32_t adv_intv_min_slot;
    /// Maximum advertising interval in multiple of 0.625ms. Must be higher than 20ms.
    uint32_t adv_intv_max_slot;
    /// Channel Map
    uint8_t chnl_map;
    /// PHY for primary advertising. Only LE 1M and LE Codec PHYs are allowed
    uint8_t phy_prim;
    /// PHY for secondary advertising
    uint8_t phy_second;
    /// Advertising SID
    uint8_t adv_sid;
    #if BLE_AUDIO_ENABLED
    uint8_t local_addr[GAP_BD_ADDR_LEN];
    #endif
}bap_bc_adv_param_t;

/// Periodic Advertising Parameters structure
typedef struct bap_bc_per_adv_param
{
    /// Minimum periodic advertising interval in multiple of 1.25ms. Must be higher than 7.5ms
    uint32_t adv_intv_min_frame;
    /// Maximum periodic advertising interval in multiple of 1.25ms. Must be higher than 7.5ms
    uint32_t adv_intv_max_frame;
} bap_bc_per_adv_param_t;

/// LC3 Capabilities structure (with pointer)
typedef struct bap_lc3_capa_ptr
{
    /// Supported sampling frequencies bit field (@see TODO [LT])
    uint16_t sampling_freq_bf;
    /// Minimum number of octets supported per codec frame
    uint16_t frame_octet_min;
    /// Maximum number of octets supported per codec frame
    uint16_t frame_octet_max;
    /// Preferred Audio Context bit field
    uint16_t context_bf;
    /// Supported frame durations bit field (@see TODO [LT])
    uint8_t frame_dur_bf;
    /// Channel count supported (@see TODO [LT])
    uint8_t chan_cnt;
    /// Length of full Codec Capabilities value
    uint8_t capa_len;
    /// Length of full Metadata value
    uint8_t metadata_len;
    /// Maximum supported LC3 frames per SDU
    /// Shall be either equal to 0 or >= to chan_cnt
    /// Part of Codec Specific Capabilities only if different than 0
    uint8_t max_lc3_frame_sdu;
    /// Pointer to full Codec Capabilities value (in LTV format)
    const uint8_t* p_capa;
    /// Pointer to full Metadata value (in LTV format)
    const uint8_t* p_metadata;
} bap_lc3_capa_ptr_t;

/// Codec Capabilities structure (with pointer)
typedef struct bap_vendor_codec_capa_ptr
{
    /// Preferred Audio Context bit field
    uint16_t context_bf;
    /// Length of full Codec Capabilities value
    uint8_t capa_len;
    /// Length of full Metadata value
    uint8_t metadata_len;
    /// Pointer to full Codec Capabilities value (in LTV format)
    const uint8_t* p_capa;
    /// Pointer to full Metadata value (in LTV format)
    const uint8_t* p_metadata;
} bap_vendor_codec_capa_ptr_t;

/// LC3 Capabilities structure
typedef struct bap_lc3_capa
{
    /// Supported sampling frequencies bit field
    uint16_t sampling_freq_bf;
    /// Minimum number of octets supported per codec frame
    uint16_t frame_octet_min;
    /// Maximum number of octets supported per codec frame
    uint16_t frame_octet_max;
    /// Preferred Audio Context bit field
    uint16_t context_bf;
    /// Supported frame durations bit field
    uint8_t frame_dur_bf;
    /// Channel count supported
    uint8_t chan_cnt;
    /// Maximum supported LC3 frames per SDU
    /// Shall be either equal to 0 or >= to chan_cnt
    /// Part of Codec Specific Capabilities only if different than 0
    uint8_t max_lc3_frame_sdu;
    /// Length of additional Codec Capabilities value
    uint8_t add_capa_len;
    /// Length of additional Metadata value
    uint8_t add_metadata_len;
    /// Additional Codec Capabilities value (in LTV format) followed by additional Metadata value
    /// (in LTV format)
    uint8_t val[__ARRAY_EMPTY];
} bap_lc3_capa_t;

/// Codec Capabilities structure
typedef struct bap_vendor_codec_capa
{
    /// Preferred Audio Context bit field
    /// Part of Metadata only if different than 0
    uint16_t context_bf;
    /// Length of additional Codec Capabilities value
    uint8_t add_capa_len;
    /// Length of additional Metadata value
    uint8_t add_metadata_len;
    /// Additional Codec Capabilities value (in LTV format) followed by additional Metadata value (in LTV format)
    uint8_t val[__ARRAY_EMPTY];
} bap_vendor_codec_capa_t;

/// LC3 Parameters structure
typedef struct bap_lc3_param
{
    /// Audio Locations of audio channels being configured for the codec
    uint32_t location_bf;
    /// Number of octets for a codec frame
    uint16_t frame_octet;
    /// Sampling frequency
    uint8_t sampling_freq;
    /// Frame duration
    uint8_t frame_dur;
    /// Number of blocks of LC3 codec frames per SDU
    uint8_t nb_lc3_frames;
} bap_lc3_param_t;

/// LC3 Configuration structure
typedef struct bap_lc3_cfg
{
    /// LC3 parameters
    bap_lc3_param_t params;
    /// Length of Codec Configuration value
    uint8_t cfg_len;
    /// Codec Configuration value (in LTV format)
    uint8_t cfg[__ARRAY_EMPTY];
} bap_lc3_cfg_t;

/// LC3 Configuration structure (Codec Parameters and Codec Configuration provided as pointer)
typedef struct bap_lc3_cfg_ptr
{
    /// Length of Codec Configuration value
    uint8_t cfg_len;
    /// LC3 parameters
    bap_lc3_param_t* p_params;
    /// Pointer to Codec Configuration value (in LTV format)
    uint8_t* p_cfg;
} bap_lc3_cfg_ptr_t;

/// Vendor Codec Configuration structure
typedef struct bap_vendor_codec_cfg
{
    /// Length of Codec Configuration value
    uint8_t cfg_len;
    /// Codec Configuration value (in LTV format)
    uint8_t cfg[__ARRAY_EMPTY];
} bap_vendor_codec_cfg_t;

/// Vendor Codec Configuration structure (Codec Configuration provided as pointer)
typedef struct bap_vendor_codec_cfg_ptr
{
    /// Length of Codec Configuration value
    uint8_t cfg_len;
    /// Pointer to Codec Configuration value (in LTV format)
    uint8_t* p_cfg;
} bap_vendor_codec_cfg_ptr_t;

/// Metadata for Codec Configuration structure
typedef struct bap_metadata_cfg
{
    /// Streaming audio context bit field
    uint16_t context_bf;
    /// Length of Metadata value
    uint8_t metadata_len;
    /// Metadata value
    uint8_t metadata[__ARRAY_EMPTY];
} bap_metadata_cfg_t;

/// Metadata for Codec Configuration structure (Metadata provided as pointer)
typedef struct bap_metadata_cfg_ptr
{
    /// Streaming audio context bit field
    uint16_t context_bf;
    /// Length of Metadata value
    uint8_t metadata_len;
    /// Pointer to Metadata value
    uint8_t* p_metadata;
} bap_metadata_cfg_ptr_t;

/// QoS Requirement structure
typedef struct bap_qos_req
{
    /// Preferred SDU interval minimum in microseconds
    /// From 255us (0xFF) to 16777215us (0xFFFFFF)
    uint32_t sdu_intv_min_us;
    /// Preferred SDU interval maximum in microseconds
    /// From 255us (0xFF) to 16777215us (0xFFFFFF)
    uint32_t sdu_intv_max_us;
    /// Presentation delay minimum microseconds
    uint32_t pres_delay_min_us;
    /// Presentation delay maximum in microseconds
    uint32_t pres_delay_max_us;
    /// Preferred Transport latency maximum in milliseconds
    /// From 5ms (0x5) to 4000ms (0xFA0)
    uint16_t trans_latency_max_ms;
    /// Preferred maximum SDU size
    /// From 0 to 4095 bytes (0xFFF)
    uint16_t max_sdu_size;
    /// Preferred PDU framing arrangement
    uint8_t framing;
    /// Preferred PHY bit field
    uint8_t phy_bf;
    /// Preferred maximum number of retransmissions for each CIS Data PDU
    /// From 0 to 15
    uint8_t retx_nb;
} bap_qos_req_t;

/// QoS Configuration structure
typedef struct bap_qos_cfg
{
    /// PDU framing arrangement
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
} bap_qos_cfg_t;

/*
 * CALLBACK FUNCTIONS DEFINITION
 ****************************************************************************************
 */

typedef struct bap_capa_srv_cb bap_capa_srv_cb_t;
typedef struct bap_capa_cli_cb bap_capa_cli_cb_t;
typedef struct bap_uc_srv_cb bap_uc_srv_cb_t;
typedef struct bap_uc_cli_cb  bap_uc_cli_cb_t;
typedef struct bap_bc_src_cb bap_bc_src_cb_t;
typedef struct bap_bc_sink_cb   bap_bc_sink_cb_t;
typedef struct bap_bc_scan_cb bap_bc_scan_cb_t;
typedef struct bap_bc_assist_cb bap_bc_assist_cb_t;
typedef struct bap_bc_deleg_cb  bap_bc_deleg_cb_t;

/// Basic Audio Profile callback set for each roles
typedef struct bap_cb
{
    /// Capabilities Server callback functions
    const bap_capa_srv_cb_t*  p_capa_srv_cb;
    /// Capabilities Client callback functions
    const bap_capa_cli_cb_t*  p_capa_cli_cb;
    /// Unicast Server callback functions
    const bap_uc_srv_cb_t*    p_uc_srv_cb;
    /// Unicast Client callbacks
    const bap_uc_cli_cb_t*    p_uc_cli_cb;
    /// Broadcast Source callback functions
    const bap_bc_src_cb_t*    p_bc_src_cb;
    /// Broadcast Sink callback functions
    const bap_bc_sink_cb_t*   p_bc_sink_cb;
    /// Broadcast Scan callback functions
    const bap_bc_scan_cb_t*   p_bc_scan_cb;
    /// Broadcast Scan Assistant callbacks
    const bap_bc_assist_cb_t* p_bc_assist_cb;
    /// Broadcast Scan Delegator callbacks
    const bap_bc_deleg_cb_t*  p_bc_deleg_cb;
} bap_cb_t;

/*
 * API FUNCTION DEFINITION
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Configure Basic Audio profile
 *
 * @param[in] role_bf           Supported role bit field (@see enum gaf_role_bf)
 * @param[in] p_capa_srv_cfg    Pointer to Capabilities Server configuration
 * @param[in] p_capa_cli_cfg    Pointer to Capabilities Client configuration
 * @param[in] p_uc_srv_cfg      Pointer to Unicast Server configuration
 * @param[in] p_uc_cli_cfg      Pointer to Unicast Client configuration
 * @param[in] p_bc_scan_cfg     Pointer to Broadcast Scan configuration
 * @param[in] p_bc_deleg_cfg    Pointer to Broadcast Delegator configuration
 * @param[in] p_cb              Pointer to callback structure
 *
 * @return status of function execution (@see gaf_err)
 ****************************************************************************************
 */
uint16_t bap_configure(uint32_t role_bf, bap_capa_srv_cfg_t* p_capa_srv_cfg, bap_capa_cli_cfg_t* p_capa_cli_cfg,
                       bap_uc_srv_cfg_t* p_uc_srv_cfg, bap_uc_cli_cfg_t* p_uc_cli_cfg,
                       bap_bc_scan_cfg_t* p_bc_scan_cfg, bap_bc_deleg_cfg_t* p_bc_deleg_cfg,
                       bap_bc_assist_cfg_t* p_bc_assist_cfg, const bap_cb_t* p_cb);

/// @}

#endif // BAP_H_
