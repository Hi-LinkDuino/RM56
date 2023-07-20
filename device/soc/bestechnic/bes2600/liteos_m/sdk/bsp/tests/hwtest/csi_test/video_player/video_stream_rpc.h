/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifndef __VIDEO_STREAM_RPC_H__
#define __VIDEO_STREAM_RPC_H__

#ifdef __cpulsplus
extern "C" {
#endif

#include "video_player.h"

enum RPC_MSG_CMD_T {
    RPC_MSG_VP_OPEN,
    RPC_MSG_VP_CLOSE,
    RPC_MSG_VP_STREAM_OPEN,
    RPC_MSG_VP_STREAM_START,
    RPC_MSG_VP_STREAM_STOP,
    RPC_MSG_VP_STREAM_CLOSE,
    RPC_MSG_VP_STREAM_SET_CFG,
    RPC_MSG_VP_STREAM_GET_CFG,
    RPC_MSG_VP_STREAM_DATA_COME,
    RPC_MSG_VP_STREAM_DATA_DONE,
    RPC_MSG_VP_STREAM_MALLOC_BUF,
    RPC_MSG_VP_STREAM_FREE_BUF,
    RPC_MSG_VP_CAMERA_I2C_WR,
    RPC_MSG_VP_CAMERA_I2C_RD,
};

#define RPC_MSG_HDR_ID        0xDE
#define RPC_REQ_MSG_PARAM_NUM 5
#define RPC_RSP_MSG_PARAM_NUM 5

struct RPC_MSG_HDR_T {
    uint16_t id             : 8;
    uint16_t reply          : 1;
    uint16_t sync_flag      : 1;
    uint16_t wait_sync_flag : 1;
    uint16_t trans_cnt      : 5;
};

struct RPC_REQ_MSG_T {
    struct RPC_MSG_HDR_T hdr;
    uint16_t cmd;
    uint32_t param[RPC_REQ_MSG_PARAM_NUM];
};

struct RPC_RSP_MSG_T {
    struct RPC_MSG_HDR_T hdr;
    uint16_t cmd;
    uint32_t param[RPC_RSP_MSG_PARAM_NUM];
};

enum RPC_MSG_RX_HDLR_ID_T {
    RPC_MSG_RX_HDLR_ID_0,
    RPC_MSG_RX_HDLR_ID_1,
    RPC_MSG_RX_HDLR_ID_NUM
};

typedef void (*VP_RPC_MSG_DATA_HDLR_T)(char *data, uint32_t len);

/* vplayer rpc stream APIs */
int vplayer_rpc_open(void);

int vplayer_rpc_close(void);

int vplayer_rpc_stream_open(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, struct VP_STREAM_CONFIG_T *cfg);

int vplayer_rpc_stream_start(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_rpc_stream_stop(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_rpc_stream_close(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream);

int vplayer_rpc_stream_data_notify(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, uint32_t addr, uint32_t len, uint32_t seq);

int vplayer_rpc_stream_data_done(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, uint32_t addr, uint32_t len, uint32_t seq);

int vplayer_rpc_stream_get_cfg(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, struct VP_STREAM_CONFIG_T *cfg);

int vplayer_rpc_stream_set_cfg(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, struct VP_STREAM_CONFIG_T *cfg);

int vplayer_rpc_stream_get_buff(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, uint8_t **ptr, uint32_t len);

int vplayer_rpc_stream_free_buff(enum VP_STREAM_ID_T id, enum VP_STREAM_T stream, uint8_t *ptr, uint32_t len);

/* vplayer rpc request/reply message APIs */
void vplayer_rpc_msg_setup_rx_handler(enum RPC_MSG_RX_HDLR_ID_T id, VP_RPC_MSG_DATA_HDLR_T handler);

int vplayer_request_msg_send(enum RPC_MSG_CMD_T cmd, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);

int vplayer_fine_request_msg_send(struct RPC_REQ_MSG_T *msg);

int vplayer_fine_request_msg_send_no_ack(struct RPC_REQ_MSG_T *msg);

int vplayer_reply_msg_send(enum RPC_MSG_CMD_T cmd, uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3);

int vplayer_fine_reply_msg_send(struct RPC_RSP_MSG_T *msg);

/* vplayer rpc message rx/tx handler APIs */
int vplayer_rpc_msg_rx_handler(char *data, uint32_t size);

int vplayer_rpc_msg_tx_handler(char *data, uint32_t size);

/* vplayer rpc message queue APIs */
struct RPC_MSG_QUEUE_T {
    struct RPC_RSP_MSG_T *msg;
    uint32_t rpos;
    uint32_t wpos;
    uint32_t size;
};

int vplayer_rpc_msg_queue_init(struct RPC_MSG_QUEUE_T *q, struct RPC_RSP_MSG_T *msg, uint32_t num);

int vplayer_rpc_msg_enqueue(struct RPC_MSG_QUEUE_T *q, struct RPC_RSP_MSG_T *m);

int vplayer_rpc_msg_queue_is_not_empty(struct RPC_MSG_QUEUE_T *q);

int vplayer_rpc_msg_dequeue(struct RPC_MSG_QUEUE_T *q, struct RPC_RSP_MSG_T *m);

#ifdef __cpulsplus
}
#endif

#endif
