/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
 ****************************************************************************/
/**
 ****************************************************************************************
 * @addtogroup APP_BAP
 * @{
 ****************************************************************************************
 */

#ifndef APP_GAF_EVENT_MOBILE_H_
#define APP_GAF_EVENT_MOBILE_H_

#include <stdint.h>                 // Standard Integer Definitions
#include <stdbool.h>                // Standard Boolean definitions
#include "hal_trace.h"

// event only can use low 12 bits
typedef enum {
    APP_GAF_EVENT_MOBILE_FIRST              = 0x000,

    // ASCC Events
    APP_GAF_ASCC_CIS_ESTABLISHED_IND        = 0x001,
    APP_GAF_ASCC_CIS_DISCONNETED_IND        = 0x002,
    APP_GAF_ASCC_CIS_STREAM_STARTED_IND     = 0x003,
    APP_GAF_ASCC_CIS_STREAM_STOPPED_IND     = 0x004,

    APP_GAF_PACC_PAC_IND                    = 0x010,
    APP_GAF_PACC_LOCATION_IND               = 0x011,
    APP_GAF_PACC_CONTEXT_IND                = 0x012,

    APP_GAF_ASSIST_SCAN_TIMEOUT_IND         = 0x020,
    APP_GAF_ASSIST_SOLICITATION_IND         = 0x021,
    APP_GAF_ASSIST_SOURCE_STATE_IND         = 0x022,
    APP_GAF_ASSIST_BCAST_CODE_RI            = 0x023,

    APP_GAF_MCS_SET_OBJ_ID_RI               = 0x030,
    APP_GAF_MCS_CONTROL_RI                  = 0x031,
    APP_GAF_MCS_SEARCH_RI                   = 0x032,
    APP_GAF_MCS_GET_RI                      = 0x033,
    APP_GAF_MCS_GET_POSITION_RI             = 0x034,
    APP_GAF_MCS_SET_RI                      = 0x035,

    APP_GAF_TBS_REPORT_INTV_IND             = 0x040,
    APP_GAF_TBS_GET_RI                      = 0x041,
    APP_GAF_TBS_CALL_OUTGOING_RI            = 0x042,
    APP_GAF_TBS_CALL_ACTION_RI              = 0x043,
    APP_GAF_TBS_CALL_JOIN_RI                = 0x044,

    APP_GAF_AICC_GAIN_IND                   = 0x050,
    APP_GAF_AICC_GAIN_PROP_IND              = 0x051,
    APP_GAF_AICC_VALUE_IND                  = 0x052,

    APP_GAF_MICC_MUTE_IND                   = 0x060,

    APP_GAF_VCC_VOLUME_IND                  = 0x070,
    APP_GAF_VCC_FLAGS_IND                   = 0x071,

    APP_GAF_VOCC_VALUE_IND                  = 0x080,

    APP_GAF_CSISC_SIRK_IND                  = 0x090,
    APP_GAF_CSISC_INFO_IND                  = 0x091,

    APP_GAF_EVENT_MOBILE_LAST               = 0xFFF
} aob_mgr_gaf_evt_mobile_e;


/// Structure for BAP_UC_SRV_CIS_STATE indication message
typedef struct
{
    /// Indication code (@see enum bap_uc_cli_ind_code)
    uint16_t ind_code;
    /// ASE local index for Sink direction
    uint8_t ase_lid_sink;
    /// ASE local index for Source direction
    uint8_t ase_lid_src;
    /// Group local index
    uint8_t grp_lid;
    /// CIS ID
    uint8_t cis_id;
    /// Connection handle allocated for the CIS by Controller
    /// GAP_INVALID_CONHDL indicates that CIS has been lost
    uint16_t conhdl;
} app_gaf_uc_cli_cis_state_ind_t;

/// Structure for cis stream started indication message
typedef struct
{
    uint8_t ase_lid;
    uint16_t cis_hdl;
    uint8_t direction;
} app_gaf_ascc_cis_stream_started_t;

/// Structure for cis stream stopped indication message
typedef struct
{
    uint8_t ase_lid;
    uint16_t cis_hdl;
    uint8_t direction;
} app_gaf_ascc_cis_stream_stopped_t;

typedef union {
    //BAP ASCC Callback Param
    app_gaf_uc_cli_cis_state_ind_t  ascc_cis_established;
    app_gaf_uc_cli_cis_state_ind_t  ascc_cis_disconnected;
    app_gaf_ascc_cis_stream_started_t   ascc_cis_stream_started;
    app_gaf_ascc_cis_stream_stopped_t   ascc_cis_stream_stopped;
} ble_audio_mobile_event_t;

#ifdef __cplusplus
extern "C" {
#endif

void app_gaf_mobile_event_cb(aob_mgr_gaf_evt_mobile_e evt_type, void *evt, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // APP_GAF_EVENT_MOBILE_H_

/// @} APP_GAF_EVENT_MOBILE_H_

