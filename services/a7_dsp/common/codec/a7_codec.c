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
#include "stdlib.h"
#include "dsp_codec.h"
#include "speech_msg.h"

#define A7_CODEC_MAILBOX_MAX (10)
osMailQDef (a7_codec_mailbox, A7_CODEC_MAILBOX_MAX, DSP_CODEC_MSG_T);
static osMailQId a7_codec_mailbox = NULL;
static uint8_t a7_codec_mailbox_cnt = 0;
osMutexDef(a7_codec_mailbox_mutex);
static osMutexId a7_codec_mailbox_mutex_id = NULL;

static void a7_codec_mailbox_mutex_wait(void)
{
    if (a7_codec_mailbox_mutex_id) {
        osMutexWait(a7_codec_mailbox_mutex_id, osWaitForever);
    } else {
        a7_codec_mailbox_mutex_id =osMutexCreate(osMutex(a7_codec_mailbox_mutex));
        if (a7_codec_mailbox_mutex_id == NULL) {
            TRACE(0, "Failed to Create a7_codec_mailbox_mutex");
        }
    }
}
static void a7_codec_mailbox_mutex_release(void)
{
    if (a7_codec_mailbox_mutex_id) {
        osMutexRelease(a7_codec_mailbox_mutex_id);
    }
}
static int a7_codec_mailbox_init(void)
{
    if (a7_codec_mailbox == NULL){
        a7_codec_mailbox = osMailCreate(osMailQ(a7_codec_mailbox), NULL);
        if (a7_codec_mailbox == NULL)  {
            TRACE(0, "Failed to Create a7_codec_mailbox");
            return -1;
        }
        TRACE(0, "%s() create a7_codec_mailbox[%p] address success",__func__,a7_codec_mailbox);
        a7_codec_mailbox_cnt = 0;
    }
    return 0;
}

static int a7_codec_mailbox_free(DSP_CODEC_MSG_T* msg_p)
{
    osStatus status = os_status_reserved;
    if (a7_codec_mailbox)
    {
        if (msg_p == NULL)
            return (int)status;
        if (msg_p->data){
            free(msg_p->data);
            msg_p->data = NULL;
        }
        status = osMailFree(a7_codec_mailbox, msg_p);
        if (osOK != status){
            TRACE(0, "%s error status 0x%02x", __func__, status);
            return (int)status;
        }
        if (a7_codec_mailbox_cnt)
        {
            a7_codec_mailbox_mutex_wait();
            a7_codec_mailbox_cnt--;
            a7_codec_mailbox_mutex_release();
        }
    }
    return (int)status;
}

static int a7_codec_msg_put(DSP_CODEC_MSG_T *input)
{
    osStatus status = os_status_reserved;
    DSP_CODEC_MSG_T *msg_p = NULL;

    if (a7_codec_mailbox == NULL){
        return -1;
    }
    if (a7_codec_mailbox_cnt >= A7_CODEC_MAILBOX_MAX){
        TRACE(0, "%s warn tx full cnt is %d", __func__, a7_codec_mailbox_cnt);
        return osErrorValue;
    }
    if (input == NULL)
        return -1;

    msg_p = (DSP_CODEC_MSG_T *)osMailAlloc(a7_codec_mailbox, 0);
    ASSERT(msg_p, "osMailAlloc error");
    msg_p->cmd = input->cmd;
    msg_p->data = NULL;
    msg_p->len = 0;
    if (input->data && input->len){
        msg_p->data = malloc(input->len);
        if ( msg_p->data == NULL){
            TRACE(0, "%s malloc msg_p->data failed!", __func__);
            return os_status_reserved;
        }
        memcpy(msg_p->data,input->data, input->len);
        msg_p->len=input->len;
    }
    //TRACE(0, "%s now put cmd=%d,len=%d",__func__,msg_p->cmd,msg_p->len);
    status = osMailPut(a7_codec_mailbox, msg_p);
    if (osOK != status){
        TRACE(0, "%s error status 0x%02x", __func__, status);
        return (int)status;
    }
    a7_codec_mailbox_mutex_wait();
    a7_codec_mailbox_cnt++;
    a7_codec_mailbox_mutex_release();
    return (int)status;
}

static int a7_codec_msg_get(DSP_CODEC_MSG_T** msg_p)
{
    osEvent evt;
    if (a7_codec_mailbox == NULL){
        TRACE(0, "%s bes_vad_mailbox is NULL \n", __func__);
        return os_status_reserved;
    }
    evt = osMailGet(a7_codec_mailbox, osWaitForever);
    if (evt.status == osEventMail) {
        *msg_p = (DSP_CODEC_MSG_T *)evt.value.p;
        return 0;
    }else{
        TRACE(0, "%s fail! evt.status 0x%02x!!", __func__, evt.status);
        return -1;
    }
}

static void a7_codec_msg_handler(void *msg, int size)
{
    static int idx = -1;
    uint8_t *ptr = msg;
    DSP_CODEC_MSG_T codec_msg;

    if (!ptr || size < 1)
        return;

    codec_msg.cmd = ptr[0];
    codec_msg.len = size - 1;
    codec_msg.data = codec_msg.len ? &ptr[1] : NULL;
    switch (codec_msg.cmd) {
        case DSP_CODEC_CMD_LC3_CFG:
        case DSP_CODEC_CMD_LC3_ENC:
        case DSP_CODEC_CMD_LC3_DEC:
        case DSP_CODEC_CMD_OPUS_CFG:
        case DSP_CODEC_CMD_OPUS_ENC:
        case DSP_CODEC_CMD_OPUS_ENC1:
        case DSP_CODEC_CMD_OPUS_ENC2:
        case DSP_CODEC_CMD_OPUS_ENC3:
        case DSP_CODEC_CMD_OPUS_DEC:
            a7_codec_msg_put(&codec_msg);
            break;
        default:
            break;
    }
    return;
}

static void a7_codec_main(void *argument)
{
    int ret = 0;
    uint8_t *outbuf = NULL;
    uint32_t outlen = 0;
    DSP_CODEC_MSG_T* msg_p = NULL;

    dsp_codec_init();
    while(1)
    {
        if (!a7_codec_msg_get(&msg_p))
        {
            ret = dsp_codec_process(msg_p, &outbuf, &outlen);
            a7_codec_mailbox_free(msg_p);
            if (!ret)
                speech_msg_send_ext(TRANSQ_MSG_TYPE_CODEC, DSP_CODEC_RSP_OK, outbuf, outlen);
            else
                speech_msg_send_ext(TRANSQ_MSG_TYPE_CODEC, DSP_CODEC_RSP_FAIL, NULL, 0);
        }
    }
    osThreadExit();
}

osThreadDef(a7_codec_main, osPriorityHigh, 1, (32*1024), "a7_codec_main");
void a7_codec_open(void)
{
    TRACE(0, "%s\n", __FUNCTION__);
    speech_msg_subscribe_ext(TRANSQ_MSG_TYPE_CODEC, a7_codec_msg_handler);
    a7_codec_mailbox_init();
    osThreadCreate(osThread(a7_codec_main), NULL);
}
