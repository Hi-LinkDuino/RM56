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


#ifndef __L2CAP_I_H__
#define __L2CAP_I_H__

#include "co_ppbuff.h"
#include "btlib_type.h"
#include "btm_i.h"
#include "data_link.h"

typedef enum {
    L2CAP_PSM_TARGET_SELF,
    L2CAP_PSM_TARGET_AVDTP_SIGNAL = 1,
    L2CAP_PSM_TARGET_AVDTP_MEDIA,
    L2CAP_PSM_TARGET_OUTGOING_SDP,
    L2CAP_PSM_TARGET_INCOMING_SDP,
    L2CAP_DYNAMIC_PSM_TARGET_PBAP,
    L2CAP_DYNAMIC_PSM_TARGET_MAP_ACCESS,
    L2CAP_DYNAMIC_PSM_TARGET_MAP_NOTIFY,
} l2cap_psm_target_profile_t;

#define PSM_SDP                 0x0001
#define PSM_RFCOMM              0x0003
#define PSM_BNEP                0x000F
#define PSM_HID_CTRL            0x0011
#define PSM_HID_INTR            0x0013
#define PSM_UPNP                0x0015
#define PSM_AVCTP               0x0017
#define PSM_AVDTP               0x0019
#define PSM_AVCTP_BROWSING      0x001B
#define PSM_ATT                 0x001F
#define PSM_BESAUD              0x0033
#define PSM_AVDTP_SIGNAL        PSM_AVDTP
#define PSM_AVDTP_MEDIA_FAKE    0xffff
#define PSM_INCOMING_SDP_BASE   0xee00
#define PSM_DYNAMIC_PBAP        0x1011
#define PSM_DYNAMIC_MAP_ACCESS  0x1013
#define PSM_DYNAMIC_MAP_NOTIFY  0x1015

#define L2CAP_SCID_PREFIX_MASK              0xff00
#define L2CAP_SCID_INDEX_MASK               0x00ff

#define L2CAP_SCID_PREFIX_OUTGOING_SDP      0x1000
#define L2CAP_SCID_PREFIX_INCOMING_SDP      0x1100
#define L2CAP_SCID_PREFIX_RFCOMM            0x2000
#define L2CAP_SCID_PREFIX_BNEP              0x3000
#define L2CAP_SCID_PREFIX_HID_CTRL          0x4000
#define L2CAP_SCID_PREFIX_HID_INTR          0x5000
#define L2CAP_SCID_PREFIX_UPNP              0x6000
#define L2CAP_SCID_PREFIX_AVCTP             0x7000
#define L2CAP_SCID_PREFIX_AVCTP_BROWSING    0x8000
#define L2CAP_SCID_PREFIX_AVDTP_SINGAL      0x9000
#define L2CAP_SCID_PREFIX_AVDTP_MEDIA       0xa000
#define L2CAP_SCID_PREFIX_ATT               0xb000
#define L2CAP_SCID_PREFIX_PBAP              0xc000
#define L2CAP_SCID_PREFIX_MAP_ACCESS        0xd000
#define L2CAP_SCID_PREFIX_MAP_NOTIFY        0xe000

#define L2CAP_BESAUD_EXTRA_CHAN_ID 0xbbee

enum l2cap_event_enum {
    L2CAP_CHANNEL_CONN_REQ,
    L2CAP_CHANNEL_OPENED,
    L2CAP_CHANNEL_NEW_OPENED,
    L2CAP_CHANNEL_TX_HANDLED,
    L2CAP_CHANNEL_CLOSED,
};

struct l2cap_ctx_input {
    struct ctx_content ctx;
    struct bdaddr_t *remote;
    uint32 l2cap_handle;
    uint16 conn_handle;
    int (*l2cap_notify_callback)(uint8 device_id, enum l2cap_event_enum event, uint32 l2cap_handle, void *pdata, uint8 reason);
    void (*l2cap_datarecv_callback)(uint8 device_id, uint32 l2cap_handle, struct pp_buff *ppb);
};

struct l2cap_ctx_output {
    uint32 l2cap_handle;
};

#if defined(__cplusplus)
extern "C" {
#endif

int8 l2cap_init ( void );


int8 l2cap_register  (uint16 psm,
                      int8 l2cap_conn_count_max,
                      int (*l2cap_notify_callback)(uint8 device_id, enum l2cap_event_enum event, uint32 l2cap_handle, void *pdata, uint8 reason),
                      void (*l2cap_datarecv_callback)(uint8 device_id, uint32 l2cap_handle, struct pp_buff *ppb)
                      );


uint32 l2cap_open (struct bdaddr_t *remote,
                    uint16 psm,
                    int (*l2cap_notify_callback)(uint8 device_id, enum l2cap_event_enum event, uint32 l2cap_handle, void *pdata, uint8 reason),
                    void (*l2cap_datarecv_callback)(uint8 device_id, uint32 l2cap_handle, struct pp_buff *ppb)
                    );

uint32 l2cap_open_with_target(struct bdaddr_t *remote,
                  uint16 psm, l2cap_psm_target_profile_t target_profile,
                  int (*l2cap_notify_callback)(uint8 device_id, enum l2cap_event_enum event, uint32 l2cap_handle, void *pdata, uint8 reason),
                  void (*l2cap_datarecv_callback)(uint8 device_id, uint32 l2cap_handle, struct pp_buff *ppb));

uint32 l2cap_enre_open(struct bdaddr_t *remote, uint16 psm, l2cap_psm_target_profile_t target,
                  int (*l2cap_notify_callback)(uint8 device_id, enum l2cap_event_enum event, uint32 l2cap_handle, void *pdata, uint8 reason),
                  void (*l2cap_datarecv_callback)(uint8 device_id, uint32 l2cap_handle, struct pp_buff *ppb));

void l2cap_create_besaud_extra_channel(void* remote, uint16_t channel_id,
    int (*notify_callback)(uint8 device_id, enum l2cap_event_enum event, uint32 l2cap_handle, void *pdata, uint8 reason),
    void (*datarecv_callback)(uint8 device_id, uint32 l2cap_handle, struct pp_buff *ppb));

uint16 l2cap_get_converted_psm_from_handle(uint32 l2cap_handle);

int8 l2cap_close(uint32 l2cap_handle);
int8 l2cap_close_v2(uint32 l2cap_handle,uint8 reason);
struct pp_buff *l2cap_data_ppb_alloc(uint32 l2cap_handle, uint32 datalen);

int8 l2cap_send_data_link(uint32 l2cap_handle, struct data_link *head, void *context);

int8 l2cap_send_data_ppb( uint32 l2cap_handle, struct pp_buff *ppb);

int8 l2cap_send_data_auto_fragment(uint32 l2cap_handle, const uint8* data, uint32 len, void *context);

int8 l2cap_send_data( uint32 l2cap_handle, uint8 *data, uint32 datalen, void *context);

int l2cap_enre_send_data(uint32 l2cap_handle, const uint8 *sdu_data, uint32 datalen, void* context);

int8 l2cap_unregister(uint16 psm);

int8 l2cap_close_v2(uint32 l2cap_handle, uint8 reason);

int8 l2cap_close_delay (uint32 l2cap_handle, int delay_sec);

int32 l2cap_get_tx_mtu(uint32 l2cap_handle);

bool l2cap_is_valid_dynamic_psm(uint16 psm);

const char *l2cap_event2str(enum l2cap_event_enum event);
const char *l2cap_psm2str(uint32 psm);

void l2cap_conn_reset_sigid(struct bdaddr_t *bdaddr);

void l2cap_clean_ibrt_slave_status(void *remote);

/* below is called by lower layer (btm) */
void l2cap_btm_notify_callback(uint8_t device_id, enum btm_l2cap_event_enum event, uint16 conn_handle, void *pdata, uint8 reason);
void l2cap_btm_datarecv_callback (uint8_t device_id, uint16 conn_handle, struct pp_buff *ppb);

int8 l2cap_send_frame(uint16 conn_handle, struct pp_buff *ppb);

struct bdaddr_t *l2cap_get_conn_remote_addr(uint32 l2cap_handle);
struct l2cap_channel *l2cap_channel_search_l2caphandle(uint32 l2cap_handle);

uint32 l2cap_save_ctx(uint32 l2cap_handle, uint8_t *buf, uint32_t buf_len);
uint32 l2cap_restore_ctx(struct l2cap_ctx_input *input, struct l2cap_ctx_output *output, void (*close_old_channel_cb)(uint8_t));

void l2cap_pts_send_disconnect_channel(void);
void l2cap_pts_send_l2cap_data(void);

#if defined(__cplusplus)
}
#endif

#endif /* __L2CAP_I_H__ */