/***************************************************************************
 *
 * Copyright 2015-2020 BES.
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
#ifndef __APP_BT_SOURCE_H__
#define __APP_BT_SOURCE_H__
#include "a2dp_api.h"
#include "avrcp_api.h"
#include "app_a2dp_source.h"
#include "app_avrcp_target.h"
#include "app_hfp_ag.h"
#include "btapp.h"
#include "hal_aud.h"
#include "app_bt.h"
#include "bt_co_list.h"

enum BT_SOURCE_DEVICE_ID_T {
    BT_SOURCE_DEVICE_ID_1 = BT_SOURCE_DEVICE_ID_BASE,
    BT_SOURCE_DEVICE_ID_2 = BT_SOURCE_DEVICE_ID_BASE + 1,
    BT_SOURCE_DEVICE_ID_N = BT_SOURCE_DEVICE_ID_BASE + BT_SOURCE_DEVICE_NUM,
    BT_SOURCE_DEVICE_INVALID_ID = 0xff,
};

#ifdef __cplusplus
extern "C" {
#endif

typedef enum bt_source_event {
    BT_SOURCE_EVENT_SEARCH_RESULT = 0, // one result with information
    BT_SOURCE_EVENT_SEARCH_COMPLETE, // complete with status
    BT_SOURCE_EVENT_A2DP_SOURCE_CONNECT_FAIL,
    BT_SOURCE_EVENT_A2DP_SOURCE_STREAM_OPEN,
    BT_SOURCE_EVENT_A2DP_SOURCE_STREAM_CLOSE,
} bt_source_event_t;

typedef struct bt_source_event_param_t {
    union {
        struct {
            app_bt_search_result_t *result;
        } search_result;
        struct {
            char status;
        } search_complete;
        struct {
            bt_bdaddr_t *addr;
        } a2dp_source_connect_fail;
        struct {
            bt_bdaddr_t *addr;
            int device_id;
        } a2dp_source_stream_open;
        struct {
            bt_bdaddr_t *addr;
            int device_id;
        } a2dp_source_stream_close;
    } p;
} bt_source_event_param_t;

typedef int (*bt_source_callback_t)(bt_source_event_t event, bt_source_event_param_t *param);

struct BT_SOURCE_DEVICE_T {
    struct BT_DEVICE_T *base_device;
    bool play_status_notify_registered;
    enum AUD_SAMPRATE_T aud_sample_rate;
    uint32_t aud_bit_rate;
    bool prev_packet_is_pending;
    bool a2dp_paused_by_sink;
    bool a2dp_paused_by_ag_sco;
};

struct bt_source_config {
    bool read_snk_or_src_from_nv;
    bool bt_source_enable;
    bool enable_sink_along_with_bt_source;
    bool ag_standalone;
    bool ag_enable;
    bool av_enable;
};

struct BT_SOURCE_MANAGER_T {
    struct bt_source_config config;
    struct BT_SOURCE_DEVICE_T devices[BT_SOURCE_DEVICE_NUM];
    bool a2dp_source_input_on;
    uint8_t curr_source_a2dp_id;
    uint8_t curr_source_playing_a2dp;
    uint8_t curr_source_hfp_id;
    uint8_t curr_source_playing_sco;
    struct list_node codec_packet_list;
};

extern struct BT_SOURCE_MANAGER_T bt_source_manager;

extern struct BT_SOURCE_DEVICE_T *app_bt_source_get_device(int i);

extern struct BT_SOURCE_DEVICE_T *app_bt_source_find_device(bt_bdaddr_t *remote);

uint8_t app_bt_source_count_connected_device(void);

bool app_bt_source_has_streaming_a2dp(void);

bool app_bt_source_has_streaming_sco(void);

uint8_t app_bt_source_count_streaming_sco(void);

uint8_t app_bt_source_count_streaming_a2dp(void);

uint8_t app_bt_source_count_streaming_aac(void);

uint8_t app_bt_source_count_streaming_lhdc(void);

uint8_t app_bt_source_count_streaming_sbc(void);

uint8_t app_bt_source_get_streaming_a2dp(void);

uint8_t app_bt_source_get_current_a2dp(void);

struct BT_SOURCE_DEVICE_T *app_bt_source_get_current_a2dp_device(void);

uint8_t app_bt_source_get_streaming_sco(void);

uint8_t app_bt_source_get_current_hfp(void);

struct BT_SOURCE_DEVICE_T *app_bt_source_get_current_hfp_device(void);

void app_bt_source_init(void);

bool app_bt_source_nv_snk_or_src_enabled(void);

void app_bt_source_set_connectable_state(bool enable);

void app_bt_source_search_device(void);

void app_bt_report_source_link_connected(uint8_t errcode, btif_remote_device_t *rem_dev);

void app_bt_source_disconnect_all_connections(bool power_off);

void bt_source_register_callback(bt_source_callback_t cb);

void bt_source_reconnect_hfp_profile(const bt_bdaddr_t *remote);

void bt_source_reconnect_a2dp_profile(const bt_bdaddr_t *remote);

void bt_source_perform_profile_reconnect(const bt_bdaddr_t *remote);

void bt_source_create_audio_link(const bt_bdaddr_t *remote);

void bt_source_disc_audio_link(const bt_bdaddr_t *remote);

#if defined(A2DP_SOURCE_TEST) || defined(HFP_AG_TEST)
void app_bt_source_set_source_addr(uint8_t *addr);
#endif

enum app_bt_source_audio_event_t {
    APP_BT_SOURCE_AUDIO_EVENT_HF_SCO_CONNECTED = 0x01,
    APP_BT_SOURCE_AUDIO_EVENT_HF_SCO_DISCONNECTED,
    APP_BT_SOURCE_AUDIO_EVENT_SNK_STREAM_START,
    APP_BT_SOURCE_AUDIO_EVENT_SNK_STREAM_SUSPEND,
    APP_BT_SOURCE_AUDIO_EVENT_AG_SERVICE_CONNECTED = 0x10,
    APP_BT_SOURCE_AUDIO_EVENT_AG_SERVICE_DISCONNECTED,
    APP_BT_SOURCE_AUDIO_EVENT_AG_SCO_CONNECTED,
    APP_BT_SOURCE_AUDIO_EVENT_AG_SCO_DISCONNECTED,
    APP_BT_SOURCE_AUDIO_EVENT_SRC_CONNECT_FAIL,
    APP_BT_SOURCE_AUDIO_EVENT_SRC_STREAM_OPEN,
    APP_BT_SOURCE_AUDIO_EVENT_SRC_STREAM_CLOSE,
    APP_BT_SOURCE_AUDIO_EVENT_SRC_STREAM_START,
    APP_BT_SOURCE_AUDIO_EVENT_SRC_STREAM_SUSPEND,
};

struct app_bt_source_audio_event_param_t {
    union {
        struct {
            bt_bdaddr_t *addr;
            int reason;
        } a2dp_source_connect_fail;
    } p;
};

void app_bt_source_audio_event_handler(uint8_t device_id, enum app_bt_source_audio_event_t event, struct app_bt_source_audio_event_param_t *param);
void app_bt_source_set_hfp_ag_pts_enable(bool enable);
void app_bt_source_set_hfp_ag_pts_esc_01_enable(bool enable);
void app_bt_source_set_hfp_ag_pts_esc_02_enable(bool enable);
void app_bt_source_set_hfp_ag_pts_ecc_enable(bool enable);
void app_bt_source_set_source_pts_get_all_cap_flag(bool enable);
void app_bt_source_set_source_pts_suspend_err_flag(bool enable);
void app_bt_source_set_source_pts_unknown_cmd_flag(bool enable);

#ifdef __cplusplus
}
#endif

#endif /* __APP_BT_SOURCE_H__ */

