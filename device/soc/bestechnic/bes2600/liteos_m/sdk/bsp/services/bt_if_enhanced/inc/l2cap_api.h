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
#ifndef _L2CAP_API_H
#define _L2CAP_API_H
#include "bluetooth.h"
#include "me_api.h"
#include "stdbool.h"

#define BTIF_NULL_IDENTIFIER_CID      0x0000
#define BTIF_SIGNALING_CHNL_CID       0x0001
#define BTIF_CONNLESS_CHNL_CID        0x0002
#define BTIF_BASE_DYNAMIC_CID         0x0040
#define BTIF_LAST_DYNAMIC_CID         (BTIF_BASE_DYNAMIC_CID + L2CAP_NUM_CHANNELS - 1)
#define BTIF_CID_TO_INDEX(_CID)       ((_CID) - BTIF_BASE_DYNAMIC_CID)
#define BTIF_BASE_GROUP_CID           ((BTIF_LAST_DYNAMIC_CID+16) & 0xFFF0)
#define BTIF_LAST_GROUP_CID           (BTIF_BASE_GROUP_CID + L2CAP_NUM_GROUPS - 1)
#define BTIF_GROUP_CID_TO_INDEX(_CID) ((_CID) - BTIF_BASE_GROUP_CID)

#define BTIF_LLC_COMMAND_REJ     0x01
#define BTIF_LLC_CONN_REQ        0x02
#define BTIF_LLC_CONN_RSP        0x03
#define BTIF_LLC_CONFIG_REQ      0x04
#define BTIF_LLC_CONFIG_RSP      0x05
#define BTIF_LLC_DISC_REQ        0x06
#define BTIF_LLC_DISC_RSP        0x07
#define BTIF_LLC_ECHO_REQ        0x08
#define BTIF_LLC_ECHO_RSP        0x09
#define BTIF_LLC_INFO_REQ        0x0A
#define BTIF_LLC_INFO_RSP        0x0B
#define  BTIF_LLC_TWS_DATA_XFER	0xFE

typedef U16 btif_l2cap_psm_value_t;

#define BTIF_BT_CLIENT_ONLY_PSM      0x0000

/* Value for a Service Discovery Protocol server */
#define BTIF_BT_PSM_SDP              0x0001

/* Value for an RFCOMM server */
#define BTIF_BT_PSM_RFCOMM           0x0003

/* Value for a TCS Binary server */
#define BTIF_BT_PSM_TCS              0x0005

/* Value for a TCS Binary group */
#define BTIF_BT_PSM_TCS_CORDLESS     0x0007

/* Value for the BNEP service */
#define BTIF_BT_PSM_BNEP             0x000F

/* Value for the HID Control Channel */
#define BTIF_BT_PSM_HID_CTRL         0x0011

/* Value for the HID Interrupt Channel */
#define BTIF_BT_PSM_HID_INTR         0x0013

/* Value for the UPnP/ESDP service */
#define BTIF_BT_PSM_UPNP             0x0015

/* Value for the A/V Control Transport Protocol signal channel */
#define BTIF_BT_PSM_AVCTP            0x0017

/* Value for the A/V Distribution Transport protocol */
#define BTIF_BT_PSM_AVDTP            0x0019

/* Value for the A/V Control Transport Protocol browsing channel*/
#define BTIF_BT_PSM_AVCTP_BROWSING   0x001B

/* Value for Unrestricted Digital Information Control Plane protocol */
#define BTIF_BT_PSM_UDI_C            0x001D

#define BTIF_BT_DYNAMIC_PSM          0x1101

typedef void btif_l2cap_channel_t;

#ifdef __cplusplus
extern "C" {
#endif

#if defined(IBRT)
    bool btif_l2cap_is_profile_channel_connected(btif_remote_device_t *p_dev, uint8_t psm_context_mask);
#endif
    void btif_l2cap_register_sdp_disconnect_callback(void (*cb)(const void* addr));
    void btif_btm_register_get_ibrt_role_callback(uint8_t (*cb)(const void* addr));
    void btif_btm_register_get_tss_state_callback(uint8_t (*cb)(const void* addr));
    btif_l2cap_channel_t *btif_l2cap_alloc_channel_instance(uint16_t index);
    btif_l2cap_channel_t *btif_l2cap_get_l2cap_channel_instance(uint16_t index);
    uint16_t btif_l2cap_cid_to_index(uint16_t index);
    uint32_t btif_l2cap_get_channel_size(void);
    void btif_l2cap_set_channel_local_cid(btif_l2cap_channel_t * channel, uint16_t cid);
    void btif_l2cap_set_channel_remote_cid(btif_l2cap_channel_t * channel, uint16_t cid);
    uint16_t btif_l2cap_get_channel_local_cid(btif_l2cap_channel_t * channel);
    uint16_t btif_l2cap_get_channel_remote_cid(btif_l2cap_channel_t * channel);

#if IS_USE_INTERNAL_ACL_DATA_PATH

#else
    bt_status_t btif_l2cap_send_data_to_peer_tws(U16 connHandle, U8 dataLen, U8 * data);

#endif

    typedef void (*btif_l2cap_process_echo_req_callback_func)(uint8_t device_id, uint16_t conhdl, uint8_t id, uint16_t len, uint8_t *data);
    typedef void (*btif_l2cap_process_echo_res_callback_func)(uint8_t device_id, uint16_t conhdl, uint8_t *rxdata, uint16_t rxlen);
    typedef void (*btif_l2cap_fill_in_echo_req_data_callback_func)(uint8_t device_id, struct l2cap_conn *conn, uint8_t *data, uint16_t data_len);

    void btif_l2cap_process_echo_req_rewrite_rsp_data(uint8_t device_id, uint16_t conhdl, uint8_t id, uint16_t len, uint8_t *data);
    void btif_l2cap_process_echo_res_analyze_data(uint8_t device_id, uint16_t conhdl, uint8_t *rxdata, uint16_t rxlen);
    void btif_l2cap_fill_in_echo_req_data(uint8_t device_id, struct l2cap_conn *conn, uint8_t *data, uint16_t data_len);

    void btif_l2cap_echo_init(btif_l2cap_process_echo_req_callback_func req_func,btif_l2cap_process_echo_res_callback_func res_func,btif_l2cap_fill_in_echo_req_data_callback_func data_func);

#ifdef __cplusplus
}
#endif
#endif
