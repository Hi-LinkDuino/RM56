/**
 ****************************************************************************************
 *
 * @file app_bap.h
 *
 * @brief BLE Audio Basic Audio Profile
 *
 * Copyright 2015-2019 BES.
 *
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_BAP_H_
#define APP_BAP_H_
#if BLE_AUDIO_ENABLED
#include "app_gaf_define.h"
#include "app_bap_bc_scan_msg.h"

/// Activity Type
enum gaf_bap_activity_type
{
    GAF_BAP_ACT_TYPE_CIS_AUDIO = 0,
    GAF_BAP_ACT_TYPE_BIS_AUDIO,
    GAF_BAP_ACT_TYPE_BIS_SHARE,
    GAF_BAP_ACT_TYPE_MAX,
};

/// PHY Type
enum gaf_bap_phy_type
{
    /// LE 1M
    GAF_BAP_PHY_1MBPS      = (1<<0),
    /// LE 2M
    GAF_BAP_PHY_2MBPS      = (1<<1),
    /// LE Coded
    GAF_BAP_PHY_LE_CODED   = (1<<2),
    GAF_BAP_PHY_ALL        = (GAF_BAP_PHY_1MBPS | GAF_BAP_PHY_2MBPS | GAF_BAP_PHY_LE_CODED),
};

/// Packet Type
enum gaf_bap_pack_type
{
    /// Sequential
    GAF_BAP_PACK_TYPE_SEQ = 0,
    /// Interleaved
    GAF_BAP_PACK_TYPE_INTER,
    GAF_BAP_PACK_TYPE_UNKNOW,
};

/// Framing Type
enum gaf_bap_frame_type
{
    /// Unframed
    GAF_BAP_FRAME_TYPE_UNFRAMED = 0,
    /// Framed
    GAF_BAP_FRAME_TYPE_FRAMED,
    GAF_BAP_FRAME_TYPE_UNKNOW,
};

/// Sampling_Frequency
enum gaf_bap_sampling_freq
{
    GAF_BAP_SAMPLE_FREQ_8000    = 0x01,
    GAF_BAP_SAMPLE_FREQ_11025,
    GAF_BAP_SAMPLE_FREQ_16000,
    GAF_BAP_SAMPLE_FREQ_22050,
    GAF_BAP_SAMPLE_FREQ_24000,
    GAF_BAP_SAMPLE_FREQ_32000,
    GAF_BAP_SAMPLE_FREQ_44100,
    GAF_BAP_SAMPLE_FREQ_48000,
    GAF_BAP_SAMPLE_FREQ_88200,
    GAF_BAP_SAMPLE_FREQ_96000,
    GAF_BAP_SAMPLE_FREQ_176400,
    GAF_BAP_SAMPLE_FREQ_192000,
    GAF_BAP_SAMPLE_FREQ_384000,
};

/// Frame_Duration
enum gaf_bap_frame_duration
{
    GAF_BAP_FRAME_DURATION_7_5MS    = 0x00,
    GAF_BAP_FRAME_DURATION_10MS     = 0x01,
};

/// Supported Sampling_Frequency Bitfield
enum gaf_bap_supported_sampling_freqs_bf
{
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_8000    = 0x0001,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_11025   = 0x0002,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_16000   = 0x0004,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_22050   = 0x0008,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_24000   = 0x0010,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_32000   = 0x0020,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_44100   = 0x0040,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_48000   = 0x0080,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_88200   = 0x0100,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_96000   = 0x0200,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_176400  = 0x0400,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_192000  = 0x0800,
    GAF_BAP_SUPPORTED_SAMPLE_FREQ_384000  = 0x1000,
};

/// Supported Frame Duration Bitfield
/*
Examples:
0x01 = 0b00000001: The codec supports the 7.5 ms frame duration.
0x02 = 0b00000010: The codec supports the 10 ms frame duration.
0x03 = 0b00000011: The codec supports the 7.5 ms frame duration and the 10 ms frame duration.
0x15 = 0b00010011: The codec supports the 7.5 ms frame duration and the 10 ms frame duration and prefers the 7.5 ms frame duration.
*/
enum gaf_bap_supported_frame_durations_bf
{
    GAF_BAP_SUPPORTED_FRAME_DURATION_7_5MS  = 0x01,
    GAF_BAP_SUPPORTED_FRAME_DURATION_10MS   = 0x02,
    GAF_BAP_SUPPORTED_FRAME_DURATION_RFU_2  = 0x04,
    GAF_BAP_SUPPORTED_FRAME_DURATION_RFU_3  = 0x08,
    ///  7.5 ms preferred.valid only when 7.5 ms is supported and 10 ms is supported. Shall not be set to 0b1 if bit 5 is set to 0b1.
    GAF_BAP_PREFERRED_FRAME_DURATION_7_5MS  = 0x10,
    /// 10 ms preferred. Valid only when 7.5 ms is supported and 10 ms is supported. Shall not be set to 0b1 if bit 4 is set to 0b1.
    GAF_BAP_PREFERRED_FRAME_DURATION_10MS   = 0x20,
    GAF_BAP_SUPPORTED_FRAME_DURATION_RFU_6  = 0x40,
    GAF_BAP_SUPPORTED_FRAME_DURATION_RFU_7  = 0x80,
};

/// Supported Audio Channel Counts Bitfield
/*
Example:
0x01 = 0b00000001: One channel supported.
0x02 = 0b00000010: Two channels supported.
0x27 = 0b00100111: One channel supported, two channels supported, three channels supported, and six channels supported.
*/
enum gaf_bap_supported_audio_chan_cnts_bf
{
    GAF_BAP_SUPPORTED_CHAN_CNT_1    = 0x01,
    GAF_BAP_SUPPORTED_CHAN_CNT_2    = 0x02,
    GAF_BAP_SUPPORTED_CHAN_CNT_3    = 0x04,
    GAF_BAP_SUPPORTED_CHAN_CNT_4    = 0x08,
    GAF_BAP_SUPPORTED_CHAN_CNT_5    = 0x10,
    GAF_BAP_SUPPORTED_CHAN_CNT_6    = 0x20,
    GAF_BAP_SUPPORTED_CHAN_CNT_7    = 0x40,
    GAF_BAP_SUPPORTED_CHAN_CNT_8    = 0x80,
};


/// Supported Audio Location Bitfield
enum gaf_bap_supported_locations_bf
{
    GAF_BAP_AUDIO_LOCATION_FRONT_LEFT               = 0x00000001,
    GAF_BAP_AUDIO_LOCATION_FRONT_RIGHT              = 0x00000002,
    GAF_BAP_AUDIO_LOCATION_FRONT_CENTER             = 0x00000004,
    GAF_BAP_AUDIO_LOCATION_LOW_FREQ_EFFECTS_1       = 0x00000008,
    GAF_BAP_AUDIO_LOCATION_BACK_LEFT                = 0x00000010,
    GAF_BAP_AUDIO_LOCATION_BACK_RIGHT               = 0x00000020,
    GAF_BAP_AUDIO_LOCATION_FRONT_LEFT_OF_CENTER     = 0x00000040,
    GAF_BAP_AUDIO_LOCATION_FRONT_RIGHT_OF_CENTER    = 0x00000080,
    GAF_BAP_AUDIO_LOCATION_BACK_CENTER              = 0x00000100,
    GAF_BAP_AUDIO_LOCATION_LOW_FREQ_EFFECTS_2       = 0x00000200,
    GAF_BAP_AUDIO_LOCATION_SIDE_LEFT                = 0x00000400,
    GAF_BAP_AUDIO_LOCATION_SIDE_RIGHT               = 0x00000800,
    GAF_BAP_AUDIO_LOCATION_TOP_FRONT_LEFT           = 0x00001000,
    GAF_BAP_AUDIO_LOCATION_TOP_FRONT_RIGHT          = 0x00002000,
    GAF_BAP_AUDIO_LOCATION_TOP_FRONT_CENTER         = 0x00004000,
    GAF_BAP_AUDIO_LOCATION_TOP_CENTER               = 0x00008000,
    GAF_BAP_AUDIO_LOCATION_TOP_BACK_LEFT            = 0x00010000,
    GAF_BAP_AUDIO_LOCATION_TOP_BACK_RIGHT           = 0x00020000,
    GAF_BAP_AUDIO_LOCATION_TOP_SIDE_LEFT            = 0x00040000,
    GAF_BAP_AUDIO_LOCATION_TOP_SIDE_RIGHT           = 0x00080000,
    GAF_BAP_AUDIO_LOCATION_TOP_BACK_CENTER          = 0x00100000,
    GAF_BAP_AUDIO_LOCATION_BOTTOM_FRONT_CENTER      = 0x00200000,
    GAF_BAP_AUDIO_LOCATION_BOTTOM_FRONT_LEFT        = 0x00400000,
    GAF_BAP_AUDIO_LOCATION_BOTTOM_FRONT_RIGHT       = 0x00800000,
    GAF_BAP_AUDIO_LOCATION_FRONT_LEFT_WIDE          = 0x01000000,
    GAF_BAP_AUDIO_LOCATION_FRONT_RIGHT_WIDE         = 0x02000000,
    GAF_BAP_AUDIO_LOCATION_LEFT_SURROUND            = 0x04000000,
    GAF_BAP_AUDIO_LOCATION_RIGHT_SURROUND           = 0x08000000,

    GAF_BAP_AUDIO_LOCATION_RFU                      = 0xF0000000,
};

/// Context type bit field meaning
enum gaf_bap_context_type_bf
{
    APP_BAP_CONTEXT_TYPE_UNSPECIFIED            = 0x0001,
    /// Conversation between humans as, for example, in telephony or video calls
    APP_BAP_CONTEXT_TYPE_CONVERSATIONAL         = 0x0002,
    /// Media as, for example, in music, public radio, podcast or video soundtrack.
    APP_BAP_CONTEXT_TYPE_MEDIA                  = 0x0004,
    /// Instructional audio as, for example, in navigation, traffic announcements or user guidance
    APP_BAP_CONTEXT_TYPE_INSTRUCTIONAL          = 0x0008,
    /// Attention seeking audio as, for example, in beeps signalling arrival of a message or keyboard clicks
    APP_BAP_CONTEXT_TYPE_ATTENTION_SEEKING      = 0x0010,
    /// Immediate alerts as, for example, in a low battery alarm, timer expiry or alarm clock.
    APP_BAP_CONTEXT_TYPE_IMMEDIATE_ALERT        = 0x0020,
    /// Man machine communication as, for example, with voice recognition or virtual assistant
    APP_BAP_CONTEXT_TYPE_MAN_MACHINE            = 0x0040,
    /// Emergency alerts as, for example, with fire alarms or other urgent alerts
    APP_BAP_CONTEXT_TYPE_EMERGENCY_ALERT        = 0x0080,
    /// Ringtone as in a call alert
    APP_BAP_CONTEXT_TYPE_RINGTOME               = 0x0100,
    /// Audio associated with a television program and/or with metadata conforming to the Bluetooth Broadcast TV
    /// profile
    APP_BAP_CONTEXT_TYPE_TV                     = 0x0200,
    /// Live audio as from a microphone where audio is perceived both through a direct acoustic path and through
    /// an LE Audio Stream
    APP_BAP_CONTEXT_TYPE_LIVE                   = 0x0400,
    /// Audio associated with video gaming, for example gaming media, gaming effects, music and in-game voice chat
    /// between participants; or a mix of all the above
    APP_BAP_CONTEXT_TYPE_GAME                   = 0x0800,
};

/// Data path configuration structure
typedef struct app_bap_uc_dp_cfg
{
    /// Datapath ID
    uint8_t dp_id;
    /// Controller Delay in microseconds
    uint32_t ctl_delay_us;
}app_bap_dp_cfg_t;

/// LC3 Capabilities structure
typedef struct app_bap_lc3_capa
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
    uint8_t val[0];
} app_bap_lc3_capa_t;

/// LC3 Parameters structure
typedef struct app_bap_lc3_param
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
} app_bap_lc3_param_t;

/// LC3 Configuration structure
typedef struct app_bap_lc3_cfg
{
    /// LC3 parameters
    app_bap_lc3_param_t params;
    /// Length of Codec Configuration value
    uint8_t cfg_len;
    /// Codec Configuration value (in LTV format)
    uint8_t cfg[0];
} app_bap_lc3_cfg_t;

/// Codec Capabilities structure
typedef struct app_bap_vendor_codec_capa
{
    /// Preferred Audio Context bit field
    /// Part of Metadata only if different than 0
    uint16_t context_bf;
    /// Length of additional Codec Capabilities value
    uint8_t add_capa_len;
    /// Length of additional Metadata value
    uint8_t add_metadata_len;
    /// Additional Codec Capabilities value (in LTV format) followed by additional Metadata value (in LTV format)
    uint8_t val[0];
} app_bap_vendor_codec_capa_t;

/// Vendor Codec Configuration structure
typedef struct app_bap_vendor_codec_cfg
{
    /// Length of Codec Configuration value
    uint8_t cfg_len;
    /// Codec Configuration value (in LTV format)
    uint8_t cfg[0];
} app_bap_vendor_codec_cfg_t;

/// Metadata for Codec Configuration structure
typedef struct app_bap_metadata_cfg
{
    /// Streaming audio context bit field
    uint16_t context_bf;
    /// Length of Metadata value
    uint8_t metadata_len;
    /// Metadata value
    uint8_t metadata[0];
} app_bap_metadata_cfg_t;

/// QoS Requirement structure
typedef struct app_bap_qos_req
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
} app_bap_qos_req_t;

/// QoS Configuration structure
typedef struct app_bap_qos_cfg
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
} app_bap_qos_cfg_t;

/// Broadcast Group Parameters structure
typedef struct app_bap_bc_grp_param
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
}app_bap_bc_grp_param_t;

/// Advertising Parameters structure
typedef struct app_bap_bc_adv_data
{
    /// Advertising data length
    uint8_t adv_data_len;
    /// Advertising data adv_type
    uint8_t adv_type;
    /// Advertising data
    uint8_t adv_data[0];
}app_bap_bc_adv_data_t;

/// Advertising Parameters structure
typedef struct app_bap_bc_adv_param
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
    /// Local Device Address
    uint8_t local_addr[APP_GAP_BD_ADDR_LEN];
}app_bap_bc_adv_param_t;

/// Periodic Advertising Parameters structure
typedef struct app_bap_bc_per_adv_param
{
    /// Minimum periodic advertising interval in multiple of 1.25ms. Must be higher than 7.5ms
    uint32_t adv_intv_min_frame;
    /// Maximum periodic advertising interval in multiple of 1.25ms. Must be higher than 7.5ms
    uint32_t adv_intv_max_frame;
} app_bap_bc_per_adv_param_t;

bool app_bap_codec_is_lc3(app_gaf_codec_id_t* p_codec_id);
void app_bap_lc3_capa_print(app_bap_lc3_capa_t* p_lc3_capa);
void app_bap_lc3_cfg_print(app_bap_lc3_cfg_t* p_lc3_cfg);
void app_bap_vendor_capa_print(app_bap_vendor_codec_capa_t* p_vendor_capa);
void app_bap_vendor_cfg_print(app_bap_vendor_codec_cfg_t* p_vendor_cfg);
void app_bap_add_data_set(uint8_t *data, uint8_t data_len);

#ifdef __cplusplus
extern "C" {
#endif

uint32_t app_bap_cmp_evt_handler(void const *param);
uint32_t app_bap_rsp_handler(void const *param);
uint32_t app_bap_ind_handler(void const *param);
uint32_t app_bap_req_ind_handler(void const *param);

void app_bap_client_init(void);

void app_bap_start(uint8_t con_lid);

void app_bap_bis_src_init(void);
void app_bap_bis_sink_init(void);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_BAP_H_

/// @} APP_BAP
