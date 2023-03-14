/**
 ****************************************************************************************
 *
 * @file app_gaf.h
 *
 * @brief BLE Audio Generic Audio Framework
 *
 * Copyright 2015-2021 BES.
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

#ifndef APP_GAF_DEFINE_H_
#define APP_GAF_DEFINE_H_

#if BLE_AUDIO_ENABLED
#include <stdbool.h>       // standard boolean definitions
#include <stddef.h>        // standard definitions
#include <stdint.h>        // standard integer definitions

#ifdef __cplusplus
extern "C"{
#endif

/// Length of Codec ID value
#define APP_GAF_CODEC_ID_LEN    (5)
/// Length of GAP Key
#define APP_GAP_KEY_LEN         (16)
/// Length of Device Address
#define APP_GAP_BD_ADDR_LEN     (6)
/// Default Preffered MTU
#define APP_GAF_DFT_PREF_MTU    (128)

/// Error codes
enum app_gaf_err
{
    /// No Error
    APP_GAF_ERR_NO_ERROR = 0,
    /// Params Invalid
    APP_GAF_ERR_INVALID_PARAM,
    /// Mermory Malloced Failed
    APP_GAF_ERR_MALLOC_ERROR,
};

/// ASE Direction
enum app_gaf_direction
{
    /// Sink direction
    APP_GAF_DIRECTION_SINK = 0,
    /// Source direction
    APP_GAF_DIRECTION_SRC,

    APP_GAF_DIRECTION_MAX,
};

/// Codec Type values
enum app_gaf_codec_type
{
    /// LC3 Codec
    APP_GAF_CODEC_TYPE_LC3    = 0x00,

    /// Maximum SIG Codec
    APP_GAF_CODEC_TYPE_SIG_MAX,

    /// Same codec as broadcast group codec use for broadcast subgroup
    APP_GAF_CODEC_TYPE_SAME   = 0xFE,
    /// Vendor Specific Codec
    APP_GAF_CODEC_TYPE_VENDOR = 0xFF,
};

/// Codec Identifier
typedef struct app_gaf_codec_id
{
    /// Codec ID value
    uint8_t codec_id[APP_GAF_CODEC_ID_LEN];
} app_gaf_codec_id_t;

/// Broadcast code used for stream encryption
typedef struct app_gaf_bcast_code_t
{
    /// Broadcast Code value
    uint8_t bcast_code[APP_GAP_KEY_LEN];
} app_gaf_bcast_code_t;

/// Periodic advertising address information
typedef struct
{
    /// BD Address of device
    uint8_t addr[APP_GAP_BD_ADDR_LEN];
    /// Address type of the device 0=public/1=private random
    uint8_t addr_type;
    /// Advertising SID
    uint8_t adv_sid;
} app_gaf_per_adv_bdaddr_t;

/// Extended advertising information
typedef struct
{
    // Device Address
    app_gaf_per_adv_bdaddr_t        addr;
    // Length of complete extend advertising data
    uint8_t                     length;
    // Complete extend advertising data containing the complete Broadcast Audio Announcement
    uint8_t                     data[251];
} app_gaf_extend_adv_report_t;

/// Data value in LTV format
typedef struct app_gaf_ltv
{
    /// Length of data value
    uint8_t len;
    /// Data value
    uint8_t data[0];
} app_gaf_ltv_t;

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_GAF_DEFINE_H_

/// @} APP_GAF
