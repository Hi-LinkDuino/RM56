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
#ifndef __HFP_API_H__
#define __HFP_API_H__

#include "bluetooth.h"
#include "conmgr_api.h"
#include "hci_api.h"

#define HF_CHANNEL_NUM (BT_DEVICE_NUM+BT_SOURCE_DEVICE_NUM)

typedef struct { /* empty */ } btif_hf_channel_t; /* used to hold the pointer to struct _hshf_channel */

/* start of btif_audio_state_t*/
typedef enum {
    BTIF_HF_AUDIO_DISCON = 0,
    BTIF_HF_AUDIO_CON = 1,
} btif_audio_state_t;

/* End of btif_audio_state_t*/

/*---------------------------------------------------------------------------
 * btif_hf_call_held_state type
 *
 */
typedef uint8_t btif_hf_call_held_state;

#define BTIF_HF_CALL_HELD_NONE      0

#define BTIF_HF_CALL_HELD_ACTIVE    1

#define BTIF_HF_CALL_HELD_NO_ACTIVE 2

/* End of btif_hf_call_held_state */

/* start the definition of btif_hf_call_setup_t*/
typedef uint8_t btif_hf_call_setup_t;

#define BTIF_HF_CALL_SETUP_NONE   0

#define BTIF_HF_CALL_SETUP_IN     1

#define BTIF_HF_CALL_SETUP_OUT    2

#define BTIF_HF_CALL_SETUP_ALERT  3
/* end btif_hf_call_setup_t*/

/* start the definition of btif_hf_call_active_t */
typedef uint8_t btif_hf_call_active_t;

#define BTIF_HF_CALL_NONE           0

#define BTIF_HF_CALL_ACTIVE         1
/* end of btif_hf_call_active_t */

typedef enum {
    BTIF_HF_SCO_CODEC_NONE = 0,
    BTIF_HF_SCO_CODEC_CVSD = 1,
    BTIF_HF_SCO_CODEC_MSBC = 2,
    BTIF_HF_SCO_CODEC_XXXX = 16,    // Cunstomer Codec
} hfp_sco_codec_t;

typedef enum {
    BTIF_HF_HOLD_RELEASE_HELD_CALLS = 0,
    BTIF_HF_HOLD_RELEASE_ACTIVE_CALLS = 1,
    BTIF_HF_HOLD_HOLD_ACTIVE_CALLS = 2,
    BTIF_HF_HOLD_ADD_HELD_CALL = 3,
    BTIF_HF_HOLD_CALL_TRANSFER = 4,
} btif_hf_hold_call_t;

typedef enum {
    BTIF_HF_STATE_CLOSED = 0,
    BTIF_HF_STATE_CONN_PENDING = 1,
    BTIF_HF_STATE_CONN_INCOMING = 2,
    BTIF_HF_STATE_NEGOTIATE = 3,
    BTIF_HF_STATE_OPEN = 4,
    BTIF_HF_STATE_DISC = 5,
} btif_hf_chan_state_t;

typedef uint16_t hf_gateway_version;

/* Unable to determine the Hands Free Profile version that is supported */
#define BTIF_HF_GW_VERSION_UNKNOWN 0x0000

/* Supports Version 0.96 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_0_96    0x0100

/* Supports Version 1.0 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_1_0     0x0101

/* Supports Version 1.5 of the Hands Free Profile */
#define BTIF_HF_GW_VERSION_1_5     0x0105

#define BTIF_HF_GW_VERSION_1_6     0x0106

#define BTIF_HF_GW_VERSION_1_7     0x0107

/* End of hf_gateway_version */

typedef enum {
    BTIF_HF_EVENT_SERVICE_CONNECT_REQ = 1,
    BTIF_HF_EVENT_SERVICE_CONNECTED = 2,
    BTIF_HF_EVENT_SERVICE_DISCONNECTED = 3,
    BTIF_HF_EVENT_AUDIO_CONNECTED = 4,
    BTIF_HF_EVENT_AUDIO_DISCONNECTED = 5,
    BTIF_HF_EVENT_AUDIO_DATA = 6,
    BTIF_HF_EVENT_AUDIO_DATA_SENT = 7,
    BTIF_HF_EVENT_GATEWAY_FEATURES = 8,
    BTIF_HF_EVENT_GW_HOLD_FEATURES = 9,
    BTIF_HF_EVENT_CALL_STATE = 10,
    BTIF_HF_EVENT_CALLER_ID = 11,
    BTIF_HF_EVENT_CALL_LISTING_ENABLED = 12,
    BTIF_HF_EVENT_RESPONSE_HOLD_APPL = 13,
    BTIF_HF_EVENT_CALL_IND = 14,
    BTIF_HF_EVENT_CALLSETUP_IND = 15,
    BTIF_HF_EVENT_CALLHELD_IND = 16,
    BTIF_HF_EVENT_RING_IND = 17,
    BTIF_HF_EVENT_WAIT_NOTIFY = 18,
    BTIF_HF_EVENT_CALLER_ID_NOTIFY = 19,
    BTIF_HF_EVENT_CURRENT_CALL_STATE = 20,
    BTIF_HF_EVENT_RESPONSE_HOLD = 21,
    BTIF_HF_EVENT_SERVICE_IND = 22,
    BTIF_HF_EVENT_BATTERY_IND = 23,
    BTIF_HF_EVENT_SIGNAL_IND = 24,
    BTIF_HF_EVENT_ROAM_IND = 25,
    BTIF_HF_EVENT_SMS_IND = 26,
    BTIF_HF_EVENT_VOICE_REC_STATE = 27,
    BTIF_HF_EVENT_VOICE_TAG_NUMBER = 28,
    BTIF_HF_EVENT_SPEAKER_VOLUME = 29,
    BTIF_HF_EVENT_MIC_VOLUME = 30,
    BTIF_HF_EVENT_IN_BAND_RING = 31,
    BTIF_HF_EVENT_NETWORK_OPERATOR = 32,
    BTIF_HF_EVENT_SUBSCRIBER_NUMBER = 33,
    BTIF_HF_EVENT_NO_CARRIER = 34,
    BTIF_HF_EVENT_BUSY = 35,
    BTIF_HF_EVENT_NO_ANSWER = 36,
    BTIF_HF_EVENT_DELAYED = 37,
    BTIF_HF_EVENT_BLACKLISTED = 38,
    BTIF_HF_EVENT_PHONEBOOK_STORAGE = 39,
    BTIF_HF_EVENT_PHONEBOOK_INFO = 40,
    BTIF_HF_EVENT_PHONEBOOK_SIZE = 41,
    BTIF_HF_EVENT_PHONEBOOK_ENTRY = 42,
    BTIF_HF_EVENT_AT_RESULT_DATA = 43,
    BTIF_HF_EVENT_COMMAND_COMPLETE = 44,
    BTIF_HF_EVENT_SIRI_STATUS = 45,
    BTIF_HF_EVENT_READ_AG_INDICATORS_STATUS = 46,
    BTIF_HF_EVENT_BES_TEST = 47,
    BTIF_HF_EVENT_SELECT_CHANNEL = 48,
    BTIF_HF_EVENT_RFCOMM_CONNECTED = 49,
    BTIF_HF_EVENT_REMOTE_NOT_SUPPORT = 50,
    BTIF_HF_EVENT_AUDIO_MOCK_CONNECTED = 51,
    BTIF_HF_EVENT_AUDIO_MOCK_DISCONNECTED = 52,
    BTIF_HF_EVENT_SERVICE_MOCK_CONNECTED = 53,
} hf_event_t;

/* End of hf_event_t*/

struct hfp_context {
    uint16_t remote_dev_hcihandle;
    /*TODO:should remove the status */
    bt_status_t status;         /* Status of the callback event             */
    btif_hf_chan_state_t state;
    bt_bdaddr_t remote_dev_bdaddr;
    const char *call_number;
    btif_hf_call_active_t call;
    btif_hf_call_setup_t call_setup;
    btif_hf_call_held_state  call_held;
    hf_event_t event;
    uint8_t *audio_data;
    uint16_t audio_data_len;
    uint8_t error_code;
    uint8_t disc_reason;
    uint8_t disc_reason_saved;
    uint8_t speaker_volume;
    uint8_t mic_volume;
    uint8_t bsir_enable;
    uint8_t voice_rec_state;
    uint8_t ciev_status;
    uint32_t **chan_sel_channel;
    btif_remote_device_t *chan_sel_remDev;
    char *ptr;
    uint8_t sco_codec;
};

struct hfp_vendor_info {
    uint16_t vendor_id;
    uint16_t product_id;
    uint16_t version_id;
    uint16_t feature_id;  //default is 3, if you do not known, set it 0
};

typedef void (*hf_event_cb_t) (uint8_t device_id, btif_hf_channel_t* chan, struct hfp_context * ctx);

#ifdef __cplusplus
extern "C" {
#endif

    struct btif_hf_cind_value {
        uint8_t index;
        uint8_t min;
        uint8_t max;
        uint8_t value;
        bool initial_support;
    };
    
#ifdef USE_BT_ADAPTER
    void btif_hfp_register_user_event_callback(hf_event_cb_t cb);
    hf_event_cb_t btif_hfp_get_user_event_callback(void);
#endif

    struct btif_hf_cind_value btif_hf_get_cind_service_value(btif_hf_channel_t* chan_h);

    struct btif_hf_cind_value btif_hf_get_cind_call_value(btif_hf_channel_t* chan_h);

    struct btif_hf_cind_value btif_hf_get_cind_callsetup_value(btif_hf_channel_t* chan_h);

    struct btif_hf_cind_value btif_hf_get_cind_callheld_value(btif_hf_channel_t* chan_h);

    struct btif_hf_cind_value btif_hf_get_cind_signal_value(btif_hf_channel_t* chan_h);

    struct btif_hf_cind_value btif_hf_get_cind_roam_value(btif_hf_channel_t* chan_h);

    struct btif_hf_cind_value btif_hf_get_cind_battchg_value(btif_hf_channel_t* chan_h);

    uint32_t btif_hf_get_ag_features(btif_hf_channel_t *chan_h);

    /*
     *  AT+XAPL=[vendorID]-[productID]-[version],[features]
     *  if you set features as 0,this api will ignore your features setting and use default value
     */
    void btif_hf_init_vendor_info(struct hfp_vendor_info *info_ptr);

    int btif_hfp_initialize(void);

    int btif_hf_register_callback(hf_event_cb_t callback);

    int btif_ag_register_callback(hf_event_cb_t callback);

    hf_gateway_version btif_hf_get_version(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_update_indicators_batt_level(btif_hf_channel_t* chan_h, uint32_t level);

    bt_status_t btif_hf_batt_report(btif_hf_channel_t* chan_h, uint8_t level);

    bt_status_t btif_hf_report_enhanced_safety(btif_hf_channel_t* chan_h, uint8_t value);

    bt_status_t btif_hf_enable_voice_recognition(btif_hf_channel_t* chan_h, bool en);

    bt_status_t btif_hf_batt_report(btif_hf_channel_t* chan_h, uint8_t level);

    bool btif_hf_is_voice_rec_active(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_disable_nrec(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_report_speaker_volume(btif_hf_channel_t* chan_h, uint8_t gain);

    bt_status_t btif_hf_send_at_cmd(btif_hf_channel_t* chan_h, const char *at_str);

    bt_status_t btif_hf_list_current_calls(btif_hf_channel_t* chan_h);

    bool btif_hf_is_hf_indicators_support(btif_hf_channel_t* chan_h);

    bool btif_hf_is_batt_report_support(btif_hf_channel_t* chan_h);

    bool btif_hf_is_sco_wait_codec_sync(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_force_disconnect_sco(btif_hf_channel_t* chan_h);

    void btif_hf_set_negotiated_codec(btif_hf_channel_t* chan_h, hfp_sco_codec_t codec);

    hfp_sco_codec_t btif_hf_get_negotiated_codec(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_answer_call(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_hang_up_call(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_redial_call(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_dial_number(btif_hf_channel_t* chan_h, uint8_t *number, uint16_t len);

    bt_status_t btif_hf_disc_audio_link(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_create_audio_link(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_create_sco_directly(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_call_hold(btif_hf_channel_t* chan_h, btif_hf_hold_call_t action,
                                  uint8_t index);
    bt_status_t btif_hf_switch_calls(btif_hf_channel_t* hangup_chan_h,
                                     btif_hf_channel_t* answer_chan_h);

    btif_hf_channel_t* btif_get_hf_chan_by_address(bt_bdaddr_t *bdaddr);

    uint8_t btif_get_hf_chan_audio_up_flag(btif_hf_channel_t* chan_h);

    btif_hf_chan_state_t btif_get_hf_chan_state(btif_hf_channel_t* chan_h);

    bool btif_hf_check_AudioConnect_status(btif_hf_channel_t* chan_h);

    btif_hf_channel_t* btif_hf_alloc_channel(void);

    btif_hf_channel_t* btif_ag_alloc_channel(void);

    int btif_hf_init_channel(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_disconnect_service_link(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_create_service_link(bt_bdaddr_t * bt_addr);

    btif_cmgr_handler_t *btif_hf_get_chan_manager_handler(btif_hf_channel_t* chan_h);

    bt_status_t btif_hfp_switch_sco(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_set_master_role(btif_hf_channel_t* chan_h, bool role);

    bt_status_t btif_hf_enable_sniff_mode(btif_hf_channel_t* chan_h, bool enable);

    bool btif_hf_get_remote_bdaddr(btif_hf_channel_t* chan_h, bt_bdaddr_t *bdaddr_p);

    void btif_hfp_register_peer_sco_codec_receive_handler(void (*cb)(uint8_t device_id,void * chan,uint8_t codec));

    void btif_hf_receive_peer_sco_codec_info(const void* remote, uint8_t codec);

    void btif_hf_register_sco_codec_info_sync_handler(void (*cb)(const void* remote, uint8_t codec));

    uint16_t btif_hf_get_sco_hcihandle(btif_hf_channel_t* chan_h);

    btif_hci_handle_t btif_hf_get_remote_hci_handle(btif_hf_channel_t* chan_h);

    bool btif_hf_is_acl_connected(btif_hf_channel_t* chan_h);

    btif_remote_device_t *btif_hf_cmgr_get_remote_device(btif_hf_channel_t* chan_h);

    bool btif_hf_check_rfcomm_l2cap_channel_is_creating(bt_bdaddr_t *bdaddr);

    btif_remote_device_t *btif_hf_cmgr_get_remote_device(btif_hf_channel_t* chan_h);

    bt_status_t btif_hf_send_audio_data(btif_hf_channel_t* chan_h, btif_bt_packet_t *packet);

    bt_status_t btif_hf_is_inbandring_enabled(btif_hf_channel_t* chan_h);
    bool btif_hfp_is_profile_initiator(bt_bdaddr_t* remote);

#if defined(IBRT)
    uint32_t btif_hfp_profile_save_ctx(btif_remote_device_t *rem_dev, uint8_t *buf, uint32_t buf_len);
    uint32_t btif_hfp_profile_restore_ctx(bt_bdaddr_t *bdaddr_p, uint8_t *buf, uint32_t buf_len);
    bt_status_t btif_hf_sync_conn_audio_connected(hfp_sco_codec_t codec,uint16_t conhdl);
    bt_status_t btif_hf_sync_conn_audio_disconnected(uint16_t conhdl);
    bool btif_hfp_profile_connecting(bt_bdaddr_t *bdaddr_p);
    bt_status_t btif_hfp_force_disconnect_hfp_profile(uint8_t device_id,uint8_t reason);
#endif

    bt_status_t btif_hf_indicators_1(btif_hf_channel_t* chan_h);
    bt_status_t btif_hf_indicators_2(btif_hf_channel_t* chan_h);
    bt_status_t btif_hf_indicators_3(btif_hf_channel_t* chan_h);

    struct btif_ag_call_info
    {
        uint8_t direction; // 0 outgoing, 1 incoming
        uint8_t state; // 0 active, 1 held, 2 outgoing dialing, 3 outgoing alerting, 4 incoming, 5 waiting, 6 held by Response and Hold
        uint8_t mode; // 0 voice, 1 data, 2 fax
        uint8_t multiparty; // 0 is not one of multiparty call parties, 1 is one of.
        const char* number; // calling number, optional
    };

    typedef int (*btif_ag_handler)(btif_hf_channel_t *chan);

    typedef int (*btif_ag_handler_int)(btif_hf_channel_t *chan, int n);

    typedef int (*btif_ag_handler_str)(btif_hf_channel_t *chan, const char* s);

    typedef int (*btif_ag_iterate_call_handler)(btif_hf_channel_t *chan, struct btif_ag_call_info* out);

    typedef const char* (*btif_ag_query_operator_handler)(btif_hf_channel_t *chan);

    struct btif_ag_module_handler
    {
        btif_ag_handler answer_call;
        btif_ag_handler hungup_call;
        btif_ag_handler dialing_last_number;
        btif_ag_handler release_held_calls;
        btif_ag_handler release_active_and_accept_calls;
        btif_ag_handler hold_active_and_accept_calls;
        btif_ag_handler add_held_call_to_conversation;
        btif_ag_handler connect_remote_two_calls;
        btif_ag_handler disable_mobile_nrec;
        btif_ag_handler_int release_specified_active_call;
        btif_ag_handler_int hold_all_calls_except_specified_one;
        btif_ag_handler_int hf_battery_change; /* battery level 0 ~ 100 */
        btif_ag_handler_int hf_spk_gain_change; /* speaker gain 0 ~ 15 */
        btif_ag_handler_int hf_mic_gain_change; /* mic gain 0 ~ 15 */
        btif_ag_handler_int transmit_dtmf_code;
        btif_ag_handler_int memory_dialing_call;
        btif_ag_handler_str dialing_call;
        btif_ag_handler_str handle_at_command;
        btif_ag_query_operator_handler query_current_operator;
        btif_ag_iterate_call_handler iterate_current_call;
    };

    bt_status_t btif_ag_create_service_link(btif_hf_channel_t* chan_h, bt_bdaddr_t * bt_addr);

    bt_status_t btif_ag_disconnect_service_link(btif_hf_channel_t* chan_h);

    bt_status_t btif_ag_create_audio_link(btif_hf_channel_t* chan_h);

    bt_status_t btif_ag_disc_audio_link(btif_hf_channel_t* chan_h);

    bt_status_t btif_ag_send_service_status(btif_hf_channel_t* chan_h, bool enabled);

    bt_status_t btif_ag_send_call_active_status(btif_hf_channel_t* chan_h, bool active);

    bt_status_t btif_ag_send_callsetup_status(btif_hf_channel_t* chan_h, uint8_t status);

    bt_status_t btif_ag_send_callheld_status(btif_hf_channel_t* chan_h, uint8_t status);

    bt_status_t btif_ag_send_mobile_signal_level(btif_hf_channel_t* chan_h, uint8_t level);

    bt_status_t btif_ag_send_mobile_roam_status(btif_hf_channel_t* chan_h, bool enabled);

    bt_status_t btif_ag_send_mobile_battery_level(btif_hf_channel_t* chan_h, uint8_t level);

    bt_status_t btif_ag_send_calling_ring(btif_hf_channel_t* chan_h, const char* number);

    bt_status_t btif_ag_set_speaker_gain(btif_hf_channel_t* chan_h, uint8_t volume);

    bt_status_t btif_ag_set_inband_ring_tone(btif_hf_channel_t* chan_h, bool enabled);

    bt_status_t btif_ag_set_last_dial_number(btif_hf_channel_t* chan_h, bool enabled);

    bt_status_t btif_ag_set_microphone_gain(btif_hf_channel_t* chan_h, uint8_t volume);

    bt_status_t btif_ag_send_call_waiting_notification(btif_hf_channel_t* chan_h, const char* number);

    bt_status_t btif_ag_send_result_code(btif_hf_channel_t* chan_h, const char *data, int len);

    bt_status_t btif_ag_register_module_handler(btif_hf_channel_t* chan_h, struct btif_ag_module_handler* handler);

#ifdef __cplusplus
}
#endif                          /*  */
#endif /*__HFP_API_H__*/
