/***************************************************************************
 *
 * Copyright 2019-2025 BES.
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
#ifndef _DATA_DUMP_H_
#define _DATA_DUMP_H_

#include <stdint.h>
#ifdef RTOS
#include "cmsis_os.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*data_dump_input_cb_t)(void *hdl, unsigned char *in, int in_len, int samplerate, int ch_num, unsigned char **p_out, int *p_out_len);
typedef void (*data_dump_cb_t)(unsigned char *data, unsigned int len, void *param);
typedef void (*data_dump_cb_register_t)(data_dump_cb_t cbf, void *param);
typedef void (*data_dump_cb_unregister_t)(data_dump_cb_t cbf, void *param);

enum _data_dump_state
{
    DATA_DUMP_OPEN,
    DATA_DUMP_START,
    DATA_DUMP_STOP,
    DATA_DUMP_CLOSE,
};

typedef struct
{
    int idx;
    void *handle;
#ifdef RTOS
    osMutexDef_t mutex_def;
    osMutexId mutex_id;
    uint32_t  os_mutex_cb[3];
#endif
    enum _data_dump_state state;
    int samplerate;
    uint8_t *_resample_buf;
    int timeout;
    int max_buf_size;
    void *user;
    void *param;
    int calc_input_data;
    int calc_output_data;
    int calc_lost_data;
    data_dump_input_cb_t input_cb;
    int ch_num;
} multi_data_dump_t;


void *data_dump_open(data_dump_cb_register_t register_cbf, unsigned int kfifo_len_KB, int samplerate, int auto_start);
void *data_dump_open_withfifo(data_dump_cb_register_t register_cbf, void *val_fifo, int samplerate, int auto_start);
void *data_dump_open_withcb(data_dump_cb_register_t register_cbf, data_dump_input_cb_t input_cb, unsigned int kfifo_len_KB, int samplerate, int ch_num, int auto_start);
int data_dump_set_timeout(void *handle, int timeout);
int data_dump_start(void *handle);
int data_dump_reset(void *handle);
unsigned int data_dump_get_size(void *handle);
int data_dump_write(void *handle, void *buf, unsigned int size);
int data_dump_read(void *handle, void *buf, unsigned int size);
int data_dump_stop(void *handle);
void data_dump_close(void *handle, data_dump_cb_unregister_t unregister_cbf);

#ifdef __cplusplus
}
#endif

#endif /* _DATA_DUMP_H_ */
