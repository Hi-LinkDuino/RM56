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

#ifndef __TRANSQ_MSG_H__
#define __TRANSQ_MSG_H__

#include "hal_transq.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TRANSQ_MSG_HANDLE_CB_T)(void *param);

typedef enum {
    TRANSQ_MSG_TYPE_TRACE,
    TRANSQ_MSG_TYPE_AF_OPEN,
    TRANSQ_MSG_TYPE_AF_CLOSE,
    TRANSQ_MSG_TYPE_AF_CAPTURE_START,
    TRANSQ_MSG_TYPE_AF_CAPTURE_STOP,
    TRANSQ_MSG_TYPE_AF_PLAYBACK_START,
    TRANSQ_MSG_TYPE_AF_PLAYBACK_STOP,
    TRANSQ_MSG_TYPE_AF_CAPTURE_IRQ,
    TRANSQ_MSG_TYPE_AF_PLAYBACK_IRQ,
    TRANSQ_MSG_TYPE_RECORD_DATA,
    TRANSQ_MSG_TYPE_USERDATA,
    TRANSQ_MSG_TYPE_VOIP,
    TRANSQ_MSG_TYPE_KEY,
    TRANSQ_MSG_TYPE_CMD,
    TRANSQ_MSG_TYPE_AUDIO_DUMP,
    TRANSQ_MSG_TYPE_HEARTBEAT,
    TRANSQ_MSG_TYPE_CODEC,
    TRANSQ_MSG_TYPE_MIC_EQ_CFG,
    TRANSQ_MSG_TYPE_ALGORITHM_DATA,
#ifdef DSP_MATH_SUPPORT
    TRANSQ_MSG_TYPE_MATH,
#endif
    TRANSQ_MSG_TYPE_NUM,
}TRANSQ_MSG_TYPE_T;

struct TRANSQ_MSG_TRACE {
    unsigned int addr;
    unsigned int len;
};

struct TRANSQ_MSG_AF_CONFIG_T {
    unsigned int bits;
    unsigned int sample_rate;
    unsigned int channel_num;
    unsigned int channel_map;
    unsigned int device;
    unsigned int io_path;
    bool chan_sep_buf;
    bool i2s_master_clk_wait;
    unsigned char i2s_sample_cycles;
    unsigned char *data_ptr;
    unsigned int data_size;

    //should define type
    unsigned char vol;
};

struct TRANSQ_MSG_AF_BUF {
    unsigned char *buf;
    unsigned int len;
};

union TRANSQ_MSG {
    struct TRANSQ_MSG_TRACE trace;
    struct TRANSQ_MSG_AF_CONFIG_T stream_cfg;
    struct TRANSQ_MSG_AF_BUF stream_buf;
};

typedef struct {
    TRANSQ_MSG_TYPE_T type;
    enum HAL_TRANSQ_PRI_T pri;
    unsigned int id;
    union TRANSQ_MSG msg;
    void *user_data;
    unsigned int user_data_len;
    unsigned char sync;
} TRANSQ_MSG_T;

#ifndef RTOS
#define transq_msg_tx_wait_done(p) \
do{ \
    transq_tx_done = 0; \
    if(transq_msg_tx(p) == 1) { \
        while (!transq_tx_done) { \
            hal_sys_timer_delay(MS_TO_TICKS(1)); \
        } \
        hal_sys_timer_delay(MS_TO_TICKS(1)); \
    } \
}while(0)
#else
#define transq_msg_tx_wait_done(p) \
do{ \
    if (transq_tx_sem != NULL) \
        transq_msg_register((p)->type, set_transq_tx_done, true); \
    if(transq_msg_tx(p) == 1) { \
        if (transq_tx_sem != NULL) { \
            osSemaphoreWait(transq_tx_sem, osWaitForever); \
            transq_msg_register((p)->type, NULL, true); \
        } \
    } \
}while(0)
#endif

int transq_msg_init();
int transq_msg_reinit();
int transq_msg_flush();
void transq_msg_onoff(int onoff);
void transq_msg_flush_data(bool tx, void *addr, uint32_t len);
int transq_msg_tx(TRANSQ_MSG_T *msg);
void transq_msg_register(TRANSQ_MSG_TYPE_T type, TRANSQ_MSG_HANDLE_CB_T func, bool tx);

#ifdef __cplusplus
}
#endif
#endif
