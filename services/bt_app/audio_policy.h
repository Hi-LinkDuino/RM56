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
#ifndef __APP_BT_AUDIO_POLICY_H__
#define __APP_BT_AUDIO_POLICY_H__

#include "bluetooth.h"

#ifdef HIECHO_ENABLE

typedef enum
{
    SPEACKER,
    BLUETOOTH,
}AudioDevice;

typedef struct
{
    bt_bdaddr_t bdaddr;
    bool support_hiecho;
    bool start_recheck_timer;
    bool force_route_audio_to_speaker;
} __attribute__((packed)) ibrt_audio_policy_info_t;

#endif


#ifdef __cplusplus
extern "C" {
#endif

enum app_bt_audio_event_t {
    APP_BT_AUDIO_EVENT_AVRCP_PLAY_STATUS_CHANGED = 0x01,
    APP_BT_AUDIO_EVENT_AVRCP_PLAY_STATUS_NOTIFY_RSP,
    APP_BT_AUDIO_EVENT_AVRCP_PLAY_STATUS,
    APP_BT_AUDIO_EVENT_AVRCP_PLAY_STATUS_MOCK,
    APP_BT_AUDIO_EVENT_AVRCP_IS_REALLY_PAUSED,
    APP_BT_AUDIO_EVENT_AVRCP_MEDIA_PLAYING,
    APP_BT_AUDIO_EVENT_AVRCP_MEDIA_PAUSED,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_OPEN,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_CLOSE,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_START,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_MOCK_START,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_SUSPEND,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_SWITCH,
    APP_BT_AUDIO_EVENT_A2DP_STREAM_RESELECT,
    APP_BT_AUDIO_EVENT_HFP_SERVICE_CONNECTED,
    APP_BT_AUDIO_EVENT_HFP_SERVICE_DISCONNECTED,
    APP_BT_AUDIO_EVENT_HFP_SCO_CONNECT_REQ,
    APP_BT_AUDIO_EVENT_HFP_SCO_CONNECTED,
    APP_BT_AUDIO_EVENT_HFP_SCO_DISCONNECTED,
    APP_BT_AUDIO_EVENT_HFP_CCWA_IND,
    APP_BT_AUDIO_EVENT_HFP_RING_IND,
    APP_BT_AUDIO_EVENT_HFP_CLCC_IND,
    APP_BT_AUDIO_EVENT_HFP_CALL_IND,  //call hang up or active indication
    APP_BT_AUDIO_EVENT_HFP_CALLSETUP_IND,  //call setup
    APP_BT_AUDIO_EVENT_HFP_CALLHELD_IND,
    APP_BT_AUDIO_EVENT_PROFILE_EXCHANGED,
    APP_BT_AUDIO_EVENT_REPLAY_WHEN_PAUSE_COME_AFTER_SCO_DISC,
    APP_BT_AUDIO_EVENT_REPLAY_WHEN_SUSPEND_COME_AFTER_SCO_DISC,
};

struct app_bt_config;

void app_bt_init_config_postphase(struct app_bt_config *config);

bool app_bt_audio_curr_a2dp_data_need_receive(uint8_t device_id);

int app_bt_audio_event_handler(uint8_t device_id, enum app_bt_audio_event_t event, uint32_t data);

uint32_t app_bt_audio_create_new_prio(void);

uint8_t app_bt_audio_get_curr_a2dp_device(void);

uint8_t app_bt_audio_get_curr_hfp_device(void);

uint8_t app_bt_audio_get_curr_sco_device(void);

uint8_t app_bt_audio_get_hfp_device_for_user_action(void);

uint8_t app_bt_audio_get_device_for_user_action(void);

uint8_t app_bt_audio_get_another_hfp_device_for_user_action(uint8_t curr_device_id);

uint8_t app_bt_audio_get_curr_playing_a2dp(void);

uint8_t app_bt_audio_get_curr_playing_sco(void);

uint8_t app_bt_audio_select_connected_a2dp(void);

uint8_t app_bt_audio_select_another_streaming_a2dp(uint8_t curr_device_id);

uint8_t app_bt_audio_select_another_device_to_create_sco(uint8_t curr_device_id);

uint8_t app_bt_audio_select_another_call_active_hfp(uint8_t curr_device_id);

uint8_t app_bt_audio_select_call_active_hfp(void);

uint8_t app_bt_audio_count_streaming_a2dp(void);

uint8_t app_bt_audio_select_connected_device(void);

uint8_t app_bt_audio_count_connected_a2dp(void);

uint8_t app_bt_audio_count_connected_sco(void);

bool app_bt_get_if_sco_audio_connected(uint8_t curr_device_id);

uint8_t app_bt_audio_count_straming_mobile_links(void);

uint8_t app_bt_audio_count_connected_hfp(void);

uint8_t app_bt_audio_select_streaming_sco(void);

uint8_t app_bt_audio_select_bg_a2dp_to_resume(void);

void app_bt_audio_set_bg_a2dp_device(bool is_paused_bg_device, bt_bdaddr_t *bdaddr);

void app_bt_audio_enable_active_link(bool enable, uint8_t active_id);

void app_bt_audio_switch_streaming_sco(void);

void app_bt_audio_a2dp_resume_this_profile(uint8_t device_id);

bool app_bt_audio_switch_streaming_a2dp(void);

void app_bt_audio_recheck_a2dp_streaming(void);

void app_bt_audio_rechceck_kill_another_connected_a2dp(uint8_t device_id);

bool app_bt_audio_a2dp_disconnect_self_check(uint8_t device_id);

uint32_t app_bt_audio_trigger_switch_streaming_a2dp(uint32_t btclk);

void app_bt_audio_new_sco_is_rejected_by_controller(uint8_t device_id);

void app_bt_audio_receive_peer_a2dp_playing_device(bool is_response, uint8_t device_id);

void app_bt_audio_peer_sco_codec_received(uint8_t device_id);

#ifdef HIECHO_ENABLE
void app_bt_audio_policy_enhance_init();
void app_bt_audio_set_force_use(bt_bdaddr_t *addr,AudioDevice out_device);
void app_bt_audio_set_hi_echo_capability(bt_bdaddr_t *addr,bool support);
bool app_bt_audio_support_hi_echo(uint8_t device_id);
void app_bt_audio_reset_device_echo_info(uint8_t device_id);
void app_bt_audio_recv_sync_audio_policy_info(uint8_t *p_buff, uint16_t length);
void app_bt_audio_reset_force_speaker_output_flag(const bt_bdaddr_t *addr);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __APP_BT_AUDIO_POLICY_H__ */

