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
#include "string.h"
#include "hal_timer.h"
#include "hal_sleep.h"
#include "hal_trace.h"
#include "cmsis.h"
#include "safe_queue.h"
#include "wifiusb_audio.h"
#include "usb_audio_app.h"
#include "usbaudio_thread.h"

#define SAMPLE_RATE               48 //ms
#define SAMPLE_SIZE               2//16BIT
#ifndef __WIFI_USB_AUDIO_CH__
#define CAPTURE_CH                2
#else
#define CAPTURE_CH                __WIFI_USB_AUDIO_CH__
#endif

#define CAPTURE_TIME              20

#define WIFIUSB_AUDIO_RECV_BUFF_LEN   (SAMPLE_RATE*SAMPLE_SIZE*CAPTURE_CH*CAPTURE_TIME ) //3840=48K*2B*2ch*20ms


#define WIFIUSB_AUDIO_CPU_WAKE_USER         HAL_CPU_WAKE_LOCK_USER_3

#define EXTRACT_CMD(d)                  ((d) & 0xFF)
#define EXTRACT_SEQ(d)                  (((d) >> 8) & 0xFF)
#define EXTRACT_ARG(d)                  (((d) >> 16) & 0xFF)

extern void analog_tsf_xtal_tune(uint32_t tsf, uint32_t fasttick);
extern void wusb_audio_input(uint8_t *pData, uint32_t Len);

extern void anc_usb_open(void);

static USB_AUDIO_CALLBACK usb_audio_callback = NULL;
static uint32_t usb_audio_submit_len;
static uint32_t usb_audio_xtal_time;
static uint8_t usb_audio_recv_buff[WIFIUSB_AUDIO_RECV_BUFF_LEN];
static uint32_t usb_audio_recv_len = 0;
//static uint32_t usb_audio_recv_cnt;
//static uint32_t usb_audio_last_recv_time;
//static bool usb_audio_recv_fst = true;

int32_t wifiusb_audio_app_init(uint32_t submit_size,uint32_t xtal_time,USB_AUDIO_CALLBACK call_back)
{
    TRACE(0,"%s: %d",__func__,__LINE__);
    if(submit_size > WIFIUSB_AUDIO_RECV_BUFF_LEN)
    {
        TRACE(0,"%s:submit_size(%d) too large.",__func__,submit_size);
        return -1;
    }
    if(!call_back)
    {
        TRACE(0,"%s:callback is null.",__func__);
        return -1;
    }
    usb_audio_submit_len = submit_size;
    usb_audio_xtal_time = xtal_time;
    usb_audio_callback = call_back;
    return 0;
}

void wifiusb_audio_data_recv_handler(const struct USB_AUDIO_XFER_INFO_T *info)
{
    const uint8_t *data;
    uint32_t size;
    //uint32_t old_wpos, new_wpos, wpos_boundary, rpos;
    //uint32_t old_play_pos = 0;
    //uint32_t play_pos;
    //uint32_t lock;
    //int conflicted;
    //uint32_t recv_samp, play_samp;
    //uint32_t cur_time;
    uint32_t odd_size;
    uint8_t *buff;
    USB_AUDIO_INFO audio_info;

    //cur_time = hal_fast_sys_timer_get();
    data = info->data;
    size = info->size;
    //TRACE(0,"%s: %d",__func__,__LINE__);
#if 0
    if (usb_audio_recv_fst == true)
    {
        usb_audio_recv_fst = false;
        usb_audio_last_recv_time = cur_time;
        usb_audio_recv_cnt = 1;
    }
    else if ((usb_audio_recv_cnt + 1) % usb_audio_xtal_time == 0)
    {
        TRACE(0,"%s: usb_recv_cnt = %d,time = %d.",
            usb_audio_recv_cnt*1000,
            FAST_TICKS_TO_US(cur_time - usb_audio_last_recv_time));
        analog_tsf_xtal_tune(usb_audio_recv_cnt*1000,cur_time - usb_audio_last_recv_time);
        usb_audio_last_recv_time = cur_time;
        usb_audio_recv_cnt = 0;
    }
    else
    {
        usb_audio_recv_cnt ++;
    }
#endif
    buff = (uint8_t*)usb_audio_recv_buff + usb_audio_recv_len;
    if (usb_audio_recv_len + size >= usb_audio_submit_len)
    {
        odd_size = (size + usb_audio_recv_len)- usb_audio_submit_len;
        memcpy(buff,data,(usb_audio_submit_len - usb_audio_recv_len));
        if(usb_audio_callback)
        {
            audio_info.type = USB_AUDIO_SUBMIT_DATA;
            audio_info.info.data.len = usb_audio_submit_len ;
            audio_info.info.data.buff = (uint8_t*)usb_audio_recv_buff;
            usb_audio_callback(&audio_info);
        }
        else
        {
            ASSERT(0,"%s: callback function is null!",__func__);
        }

        usb_audio_recv_len = 0;

        if(odd_size > 0)
        {
            buff = (uint8_t*)usb_audio_recv_buff;
            memcpy(buff, data + (usb_audio_submit_len - usb_audio_recv_len), odd_size);
            usb_audio_recv_len = odd_size;
        }
    }
    else
    {
        memcpy(buff,data,size);
        usb_audio_recv_len += size;
    }
    return;
}

static uint8_t wifiusb_audio_playback_vol;
void wifiusb_audio_set_vol(uint8_t vol)
{
    wifiusb_audio_playback_vol = vol;
}

static uint8_t wifiusb_audio_mute_state;
void wifiusb_audio_set_mute(uint8_t state)
{
    wifiusb_audio_mute_state = state;
}

void wifiusb_audio_cmd_recv_handler(void *param)
{
    uint32_t data;
    enum WIFIUSB_AUDIO_CMD_T cmd;
    uint8_t seq;
    uint8_t POSSIBLY_UNUSED arg;
    USB_AUDIO_INFO audio_info;
    struct SAFE_QUEUE_T *cmd_queue;

    TRACE(0,"%s: %d",__func__,__LINE__);
    hal_cpu_wake_unlock(WIFIUSB_AUDIO_CPU_WAKE_USER);
    cmd_queue = (struct SAFE_QUEUE_T*)param;
    while (safe_queue_get(cmd_queue, &data) == 0) {
        cmd = EXTRACT_CMD(data);
        seq = EXTRACT_SEQ(data);
        arg = EXTRACT_ARG(data);
        if (cmd == WIFIUSB_AUDIO_CMD_SET_VOLUME)
            arg = wifiusb_audio_playback_vol;
        if (cmd == WIFIUSB_AUDIO_CMD_MUTE_CTRL)
            arg = wifiusb_audio_mute_state;
        TRACE(0,"%s: cmd=%d seq=%d arg=%d", __func__, cmd, seq, arg);
        switch (cmd) {
            case WIFIUSB_AUDIO_CMD_START_PLAY:
            case WIFIUSB_AUDIO_CMD_STOP_PLAY:
            case WIFIUSB_AUDIO_CMD_START_CAPTURE:
            case WIFIUSB_AUDIO_CMD_STOP_CAPTURE:
            case WIFIUSB_AUDIO_CMD_SET_CAP_VOLUME:
            case WIFIUSB_AUDIO_CMD_CAP_MUTE_CTRL:
            case WIFIUSB_AUDIO_CMD_SET_VOLUME:
            case WIFIUSB_AUDIO_CMD_MUTE_CTRL:
            case WIFIUSB_AUDIO_CMD_USB_RESET:
            case WIFIUSB_AUDIO_CMD_USB_DISCONNECT:
            case WIFIUSB_AUDIO_CMD_USB_CONFIG:
            case WIFIUSB_AUDIO_CMD_USB_SLEEP:
            case WIFIUSB_AUDIO_CMD_USB_WAKEUP:
            case WIFIUSB_AUDIO_CMD_RECV_PAUSE:
            case WIFIUSB_AUDIO_CMD_RECV_CONTINUE:
            case WIFIUSB_AUDIO_CMD_SET_RECV_RATE:
            case WIFIUSB_AUDIO_CMD_SET_SEND_RATE:
            case WIFIUSB_AUDIO_CMD_RESET_CODEC:
            case WIFIUSB_AUDIO_CMD_NOISE_GATING:
            case WIFIUSB_AUDIO_CMD_NOISE_REDUCTION:
            case WIFIUSB_AUDIO_CMD_TUNE_SAMPLE_RATE:
                if(usb_audio_callback)
                {
                    audio_info.type = USB_AUDIO_SUBMIT_CMD;
                    audio_info.info.cmd.cmd = cmd;
                    audio_info.info.cmd.seq = seq;
                    audio_info.info.cmd.arg = arg;
                    usb_audio_callback(&audio_info);
                }
                else
                {
                    ASSERT(0,"%s: callback function is null!",__func__);
                }
                break;
            default:
                //ASSERT(false, "%s: Invalid cmd %d", __func__, cmd);
                break;
        }
    }
}


void wifiusb_audio_send_cmd(uint32_t data)
{
    enum WIFIUSB_AUDIO_CMD_T cmd;
    uint8_t seq;
    uint8_t POSSIBLY_UNUSED arg;
    USB_AUDIO_INFO audio_info;

    TRACE(0,"%s: %d",__func__,__LINE__);
    cmd = EXTRACT_CMD(data);
    seq = EXTRACT_SEQ(data);
    arg = EXTRACT_ARG(data);
    switch (cmd) {
            case WIFIUSB_AUDIO_CMD_START_PLAY:
            case WIFIUSB_AUDIO_CMD_STOP_PLAY:
            case WIFIUSB_AUDIO_CMD_START_CAPTURE:
            case WIFIUSB_AUDIO_CMD_STOP_CAPTURE:
            case WIFIUSB_AUDIO_CMD_SET_VOLUME:
            case WIFIUSB_AUDIO_CMD_SET_CAP_VOLUME:
            case WIFIUSB_AUDIO_CMD_CAP_MUTE_CTRL:
            case WIFIUSB_AUDIO_CMD_MUTE_CTRL:
            case WIFIUSB_AUDIO_CMD_USB_RESET:
            case WIFIUSB_AUDIO_CMD_USB_DISCONNECT:
            case WIFIUSB_AUDIO_CMD_USB_CONFIG:
            case WIFIUSB_AUDIO_CMD_USB_SLEEP:
            case WIFIUSB_AUDIO_CMD_USB_WAKEUP:
            case WIFIUSB_AUDIO_CMD_RECV_PAUSE:
            case WIFIUSB_AUDIO_CMD_RECV_CONTINUE:
            case WIFIUSB_AUDIO_CMD_SET_RECV_RATE:
            case WIFIUSB_AUDIO_CMD_SET_SEND_RATE:
            case WIFIUSB_AUDIO_CMD_RESET_CODEC:
            case WIFIUSB_AUDIO_CMD_NOISE_GATING:
            case WIFIUSB_AUDIO_CMD_NOISE_REDUCTION:
            case WIFIUSB_AUDIO_CMD_TUNE_SAMPLE_RATE:
                if(usb_audio_callback)
                {
                    audio_info.type = USB_AUDIO_SUBMIT_CMD;
                    audio_info.info.cmd.cmd = cmd;
                    audio_info.info.cmd.seq = seq;
                    audio_info.info.cmd.arg = arg;
                    usb_audio_callback(&audio_info);
                }
                else
                {
                    ASSERT(0,"%s: callback function is null!",__func__);
                }
                break;
            default:
                ASSERT(false, "%s: Invalid cmd %d", __func__, cmd);
                break;
        }
}
void wifiusb_audio_open(void)
{
    TRACE(0,"%s: %d",__func__,__LINE__);
    usb_os_init();
    anc_usb_open();
}

uint32_t wifiusb_audio_get_capture_buffer_size(void)
{
    TRACE(0,"%s: %d",__func__,__LINE__);
    return WIFIUSB_CAPTURE_BUFF_LEN/2;
}

uint8_t wifiusb_capture_buff[WIFIUSB_CAPTURE_BUFF_LEN];
uint32_t wifiusb_wpos = 0;
int32_t wifiusb_audio_data_capture_handler(uint8_t *buf, uint32_t len)
{
    int32_t ret;
    uint8_t *pbuff;
//    uint32_t rpos;
//    uint32_t i;

    //TRACE(0,"%s: %d,len = %d",__func__,__LINE__,len);
    if(len > WIFIUSB_CAPTURE_BUFF_LEN/2)
    {
        TRACE(0,"%s: %d",__func__,__LINE__);
        return -1;
    }
    if(wifiusb_wpos == WIFIUSB_CAPTURE_BUFF_LEN)
    {
        //TRACE(0,"%s: wifiusb_wpos = %d, line=%d",__func__,wifiusb_wpos, __LINE__);
        ret = usb_audio_data_capture_handler(buf,len);
        if(ret)
        {
            TRACE(0,"%s: usb_audio_data_capture_handler fail1.ret = %d.",__func__,ret);
            return ret;
        }
    }
    else if(wifiusb_wpos + len < WIFIUSB_CAPTURE_BUFF_LEN)
    {
        //TRACE(0,"%s: wifiusb_wpos = %d, line=%d",__func__,wifiusb_wpos, __LINE__);
        pbuff = wifiusb_capture_buff + wifiusb_wpos;
        memcpy(pbuff,buf,len);
        wifiusb_wpos += len;
        ret = 0;
    }
    else if(wifiusb_wpos + len == WIFIUSB_CAPTURE_BUFF_LEN)
    {
        //TRACE(0,"%s: wifiusb_wpos = %d, line=%d",__func__,wifiusb_wpos, __LINE__);
        pbuff = wifiusb_capture_buff + wifiusb_wpos;
        memcpy(pbuff,buf,len);
        wifiusb_wpos += len;
        ret = 0;
        ret = usb_audio_data_capture_handler(wifiusb_capture_buff,wifiusb_wpos);
        if(ret)
        {
            TRACE(0,"%s: usb_audio_data_capture_handler fail1.ret = %d.",__func__,ret);
            return ret;
        }
    }
    else
    {
        TRACE(0,"%s: wifiusb_wpos = %d,len = %d",__func__,wifiusb_wpos,len);
        return -2;
    }

    return ret;

}

