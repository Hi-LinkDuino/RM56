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

#ifndef APP_BAP_STREAM_H_
#define APP_BAP_STREAM_H_

/*****************************header include********************************/

/******************************macro defination*****************************/

/******************************type defination******************************/
typedef uint8_t (*iso_send_data_func)(uint8_t **payload, uint16_t payload_len, uint32_t ref_time);
typedef bool (*iso_rev_data_func)(uint8_t channel, uint8_t *buf, uint32_t *len, void *iso_buffer);

/// HCI ISO_Data_Load - Packet Status Flag
typedef enum
{
    /// Valid data. The complete ISO_SDU was received correctly
    BAP_ISO_PKT_STATUS_VALID   = (0),
    /// Possibly invalid data. The contents of the ISO_SDU may contain errors or part of the ISO_SDU may
    /// be missing. This is reported as "data with possible errors".
    BAP_ISO_PKT_STATUS_INVALID = (1),
    /// Part(s) of the ISO_SDU were not received correctly. This is reported as "lost data".
    BAP_ISO_PKT_STATUS_LOST    = (2),
} BAP_ISO_PKT_STATUS_E;

/// BAP STREAM TYPE
typedef enum
{
    BAP_STREAM_CIS = (0),
    BAP_STREAM_BIS = (1),
} BAP_STREAM_TYPE_E;

/// BAP AUDIO TYPE
typedef enum
{
    BAP_AUDIO_BIS_SRC = (0),
    BAP_AUDIO_BIS_SINK = (1),
} BAP_AUDIO_TYPE_E;

typedef enum
{
    BIS_SRC_PCM_INPUT_LINEIN = 0,
    BIS_SRC_PCM_INPUT_WIFI   = 1,
    BIS_SRC_PCM_INPUT_BT     = 2,
    BIS_SRC_PCM_INPUT_NONE   = 3,

    BIS_SRC_PCM_INPUT_NUM,
} BIS_SRC_PCM_INPUT_TYPE_E;

typedef struct
{
    uint8_t capture_inited  : 1;
    uint8_t playback_inited : 1;
    uint8_t mempool_inited  : 1;
    uint8_t rfu             : 5;
} bap_stream_env_t;

typedef struct
{
    int32_t        bitrates;
    uint32_t       maxSdu;
    uint32_t       sduInterval;
    uint32_t       sendFrameNum;
    uint32_t       audioPlayInterval;
    uint32_t       sinkTransDelay;
} bis_config_para_t;

#ifdef __cplusplus
extern "C" {
#endif

int app_bap_stream_lc3_decode_16(void *input_bytes, int32_t num_bytes, int16_t **output_samples);
int app_bap_stream_audio_init(iso_rev_data_func func, BAP_STREAM_TYPE_E type);
int app_bap_stream_audio_deinit(void);
int app_bap_stream_lc3_encoder_init(void);
void app_bap_stream_lc3_encoder_deinit(void);
int app_bap_source_stream_init(iso_send_data_func func, BAP_STREAM_TYPE_E type);
int app_bap_source_stream_deinit(BAP_STREAM_TYPE_E type);
void app_bis_audio_start(uint16_t stream_type, uint8_t device_id);
void app_bis_audio_stop(uint16_t stream_type, uint8_t device_id);
int app_bc_src_local_audio_init(void);
int app_bc_src_local_audio_deinit(void);
uint32_t app_bap_write_pcm_queue_data(uint8_t *buf, uint32_t len);
int app_bc_sink_af_audio_init(void);
int app_bc_sink_af_audio_deinit(void);
void app_bap_bc_src_stop(uint8_t big_idx);
void app_bis_capture_init(iso_send_data_func func);
void app_bis_capture_deinit(void);
int app_bc_sink_local_audio_init(iso_rev_data_func func);
void app_bc_sink_local_audio_deinit(void);
void app_bis_stream_receive_data(uint8_t channel, BAP_ISO_PKT_STATUS_E pkt_status);
void app_bis_wifi_init(iso_send_data_func func);
void app_bis_wifi_deinit(void);
void app_bis_bt_init(iso_send_data_func func);
void app_bis_bt_deinit(void);
void app_bis_sink_set_lc3_para(uint32_t maxsdu,uint32_t interval,uint32_t delay);
void app_bis_set_src_pcm_input_type(uint8_t type);
void app_bap_audio_stop_control_by_user(BAP_AUDIO_TYPE_E type);
bool app_bis_is_run(void);

#ifdef AIRPLAY_SUPPORT
void app_bis_airplay_init(void);
void app_bis_airplay_write(uint8_t *buf, uint32_t len);
#endif
#ifdef __cplusplus
}
#endif

#endif // APP_BAP_STREAM_H_

/// @} APP_BAP
