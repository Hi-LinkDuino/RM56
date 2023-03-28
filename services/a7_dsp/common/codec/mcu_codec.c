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
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "plat_addr_map.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "string.h"
#include "stdio.h"
#include "dsp_codec.h"
#include "speech_msg.h"

osMutexDef(dsp_codec_req_mutex);
static osMutexId codec_req_mutex_id = 0;

osSemaphoreDef(dsp_codec_done_sem);
static osSemaphoreId codec_done_sem_id = NULL;
static uint8_t * dsp_out_buf = NULL;
static uint32_t * dsp_out_len = NULL;

static void mcu_codec_msg_handler(void *msg, int size)
{
    uint8_t *ptr = msg;

    if (ptr && size > 0) {
        if (ptr[0] == DSP_CODEC_RSP_OK) {
            /* get codec proecss result */
            if (dsp_out_buf && dsp_out_len && size > 1) {
                memcpy(dsp_out_buf, &ptr[1], size-1);
                *dsp_out_len = size-1;
            }
            osSemaphoreRelease(codec_done_sem_id);
        } else if (ptr[0] == DSP_CODEC_RSP_FAIL) {
            if (dsp_out_len)
                *dsp_out_len = 0;
            osSemaphoreRelease(codec_done_sem_id);
        }
    }
    return;
}

void mcu_codec_open(void)
{
    printf("%s\n", __FUNCTION__);
    codec_req_mutex_id =osMutexCreate(osMutex(dsp_codec_req_mutex));
    codec_done_sem_id = osSemaphoreCreate(osSemaphore(dsp_codec_done_sem), 0);
    speech_msg_subscribe_ext(TRANSQ_MSG_TYPE_CODEC, mcu_codec_msg_handler);
    transq_msg_onoff(1);
}

int mcu_codec_request(DSP_CODEC_CMD_T cmd, uint8_t * in, uint32_t len, uint8_t *out, uint32_t *out_len)
{
    int ret = 0;

    if (!(in && len))
        return -1;

    osMutexWait(codec_req_mutex_id, osWaitForever);
    dsp_out_buf = out;
    dsp_out_len = out_len;
    speech_msg_send_ext(TRANSQ_MSG_TYPE_CODEC, cmd, in, len);
    ret = osSemaphoreWait(codec_done_sem_id, 100);
    if (ret <= 0) {
        TRACE(0, "%s failed ret %d", __FUNCTION__, ret);
        ret = -1;
    } else
        ret = 0;
    dsp_out_buf = NULL;
    dsp_out_len = NULL;
    osMutexRelease(codec_req_mutex_id);
    return ret;
}