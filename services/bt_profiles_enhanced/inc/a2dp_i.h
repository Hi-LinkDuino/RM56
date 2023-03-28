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

#ifndef __A2DP_I_H__
#define __A2DP_I_H__

#include "btlib_type.h"
#include "btm_i.h"
#include "avdtp.h"
#include "avdtp_i.h"
#include "sdp.h"

#define A2DP_RECVDATA_BUF_SIZE 48

/* notify upper layer */
enum a2dp_event_t
{
    /* user command event*/
    AV_CONNECT_REQ = 1,
    AV_DISCONNECT_REQ,
    AV_TRANS_REQ,
    AV_SUSPEND_REQ,
    AV_START_REQ,
    AV_ABORT_REQ,
    AV_SET_CONFIG_REQ,
    AV_RECONF_REQ,

    /* internal event */
    //  AV_EVNT_BEGIN,
    AV_CONN_OPENED,
    AV_CONN_CLOSED,
    AV_CONN_CLOSED_DUE_TO_SDP_FAIL,

    AV_MEDIA_GET_CAP_IND,
    AV_MEDIA_SET_CFG_IND,
    AV_MEDIA_GET_CFG_CFM,
    AV_MEDIA_STREAM_CLOSE,
    AV_MEDIA_STREAM_ABORT,
    AV_MEDIA_RECONF_IND,
    AV_MEDIA_DISCOVERY_COMPLETE,

    AV_MEDIA_OPENED,
    AV_MEDIA_STREAM_START,
    AV_MEDIA_STREAM_DATA_IND,
    AV_MEDIA_STREAM_SUSPEND,
    AV_MEDIA_FREQ_CHANGED,
    AV_CONN_REQ_FAIL,
    AV_MEDIA_SECURITY_CONTROL_CMD,
    AV_MEDIA_SECURITY_CONTROL_CFM,
    AV_MEDIA_TX_HANDLED,

    AV_REMOTE_NOT_SUPPORT,
};

enum av_conn_state_enum
{
    AV_STOP,
    AV_STANDBY = 1, //ready
    AV_QUERING, //sdp quering
    AV_CONNECTING,  //initializing
    AV_OPEN,        //AV_SIG_ACTIVE,
    AV_CONNECTED,
    AV_STREAMING
};


struct a2dp_control_t
{
    struct bdaddr_t remote;    
    bool is_source;
    bool cp_enable;
    uint8 disc_reason;
    uint32 l2capSignalHandle;
    uint32 l2capMediaHandle;
    uint32 freq;
    uint16 avdtp_local_version;
    uint16 avdtp_remote_version;
    uint16 a2dp_remote_features;
    uint8 channel_mode;
    bool reconfig_codec_pending;
    bool reconfig_restore_streaming;
    uint8_t reconfig_codec_id;

    enum av_conn_state_enum state;
    struct avdtp_control_t avdtp;

    struct avdtp_media_codec_capability *force_use_codec;
    struct avdtp_local_sep *lsep_list;
    struct avdtp_local_sep *selected_lsep;
    uint8 curr_lsep_prio;

    void *avdtp_codec_req; //finally setconf/reconf-ed codec capabilities
    void *avdtp_cp_req; //finally setconf/reconf-ed cp capabilities
    struct avdtp_media_content_protect_capability *security_cp_cap;

    void (*indicate)(uint8 device_id, struct a2dp_control_t *stream, uint8 event, void *pData);
    void (*data_cb)(uint8 device_id, struct a2dp_control_t *stream,struct pp_buff *ppb);
    
    avdtp_media_header_t media_header;
    struct sdp_request sdp_request;
};

struct a2dp_ctx_input {
    struct ctx_content ctx;
    struct bdaddr_t *remote;
    struct a2dp_control_t *a2dp_ctl;
    struct avdtp_control_t *avdtp_ctl;
    struct avdtp_local_sep *local_sep;
    uint32 l2capSignalHandle;
    uint32 l2capMediaHandle;
    void (*indicate_callback)(uint8 device_id, struct a2dp_control_t *stream, uint8 event, void *pData);
    void (*data_callback)(uint8 device_id, struct a2dp_control_t *stream, struct pp_buff *ppb);
};

struct a2dp_ctx_output {
};

extern struct a2dp_control_t *a2dp_ctl_list;

#if defined(__cplusplus)
extern "C" {
#endif

void a2dp_stack_init(struct avdtp_control_t* (*search)(uint8_t device_id, bool is_channel_req_context));

void a2dp_stream_init(struct a2dp_control_t *a2dp_ctl,
            void (*indicate_callback)(uint8 device_id, struct a2dp_control_t *stream, uint8 event, void *pData),
            void (*data_callback)(uint8 device_id, struct a2dp_control_t *stream, struct pp_buff *ppb));

int a2dp_register_sep(
            struct a2dp_control_t *stream,
            uint8 sep_type,
            struct avdtp_media_codec_capability *sep_code,
            struct avdtp_media_content_protect_capability *sep_cp,
            uint8 sep_prioprity);

int a2dp_unregister_sep(struct a2dp_control_t *ctl, uint8 codec_type);

enum av_conn_state_enum av_getState(struct  a2dp_control_t*stream) ;
void av_setState( struct  a2dp_control_t *stream, enum av_conn_state_enum state);
int8 av_close(struct a2dp_control_t * stream);

int8 av_connectReq(struct a2dp_control_t *stream, struct bdaddr_t *peer);
int8 av_disconnectReq(struct  a2dp_control_t *stream);

const char *a2dp_state2str(enum av_conn_state_enum state);

int8 av_suspendReq(struct a2dp_control_t *stream);
int8 av_startReq(  struct  a2dp_control_t  *stream);
int8 av_abortReq( struct  a2dp_control_t  *stream);
int8 av_setConfReq( struct  a2dp_control_t  *stream);

int8 av_reconfig_codec(struct a2dp_control_t *stream, uint8_t codec_id);

int8 av_setSinkDelay(struct a2dp_control_t *stream, uint16 delay_ms);
int8 av_security_control_req(struct a2dp_control_t *stream, uint8_t *data, uint16_t len);
int8 av_security_control_resp(struct a2dp_control_t *stream, uint8_t *data, uint16_t len, uint8 error);

struct avdtp_control_t *a2dp_get_avdtp_control(struct a2dp_control_t *stream);
uint32 a2dp_save_ctx(struct a2dp_control_t *a2dp_ctl, uint8_t *buf, uint32_t buf_len);
uint32 a2dp_restore_ctx(struct a2dp_ctx_input *input);

#if defined(__cplusplus)
}
#endif

#endif /* __A2DP_I_H__ */
