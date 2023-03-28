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
#ifndef __WIFIUSB_AUDIO_H__
#define __WIFIUSB_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "usb_audio.h"
// #define WIFIUSB_WITH_PLAY_CAPTURE
#ifdef __USB_SUPPORT_48K_24B__
#define WIFIUSB_BUFF_FRAME_NUM    6
#define WIFIUSB_BUFF_FRAM_SIZE    288  //288 = 48*3B*2ch
#else
#define WIFIUSB_BUFF_FRAME_NUM    10
#define WIFIUSB_BUFF_FRAM_SIZE    192  //192 = 48*2*2ch
#endif

#define WIFIUSB_CAPTURE_BUFF_LEN (WIFIUSB_BUFF_FRAM_SIZE*WIFIUSB_BUFF_FRAME_NUM)


enum WIFIUSB_AUDIO_CMD_T {
    WIFIUSB_AUDIO_CMD_START_PLAY = 0,
    WIFIUSB_AUDIO_CMD_STOP_PLAY,
    WIFIUSB_AUDIO_CMD_START_CAPTURE,
    WIFIUSB_AUDIO_CMD_STOP_CAPTURE,
    WIFIUSB_AUDIO_CMD_SET_VOLUME,
    WIFIUSB_AUDIO_CMD_SET_CAP_VOLUME,
    WIFIUSB_AUDIO_CMD_MUTE_CTRL,
    WIFIUSB_AUDIO_CMD_CAP_MUTE_CTRL,
    WIFIUSB_AUDIO_CMD_USB_RESET,
    WIFIUSB_AUDIO_CMD_USB_DISCONNECT,
    WIFIUSB_AUDIO_CMD_USB_CONFIG,
    WIFIUSB_AUDIO_CMD_USB_SLEEP,
    WIFIUSB_AUDIO_CMD_USB_WAKEUP,
    WIFIUSB_AUDIO_CMD_RECV_PAUSE,
    WIFIUSB_AUDIO_CMD_RECV_CONTINUE,
    WIFIUSB_AUDIO_CMD_SET_RECV_RATE,
    WIFIUSB_AUDIO_CMD_SET_SEND_RATE,
    WIFIUSB_AUDIO_CMD_RESET_CODEC,
    WIFIUSB_AUDIO_CMD_NOISE_GATING,
    WIFIUSB_AUDIO_CMD_NOISE_REDUCTION,
    WIFIUSB_AUDIO_CMD_TUNE_SAMPLE_RATE,
    WIFIUSB_AUDIO_CMD_QTY
};

enum USB_AUDIO_TYPE_T{
    USB_AUDIO_SUBMIT_CMD,
    USB_AUDIO_SUBMIT_DATA,
    USB_AUDIO_SUBMIT_TYPE_QTY,
};

typedef struct {
    enum WIFIUSB_AUDIO_CMD_T cmd;
    uint8_t seq;
    uint8_t arg;
}USB_AUDIO_CMD_T;

typedef struct {
    uint8_t *buff;
    uint32_t len;
}USB_AUDIO_DATA_T;

typedef struct {
    enum USB_AUDIO_TYPE_T type;
    union {
     USB_AUDIO_CMD_T cmd;
     USB_AUDIO_DATA_T data;
    }info;
}USB_AUDIO_INFO;


typedef void (*USB_AUDIO_CALLBACK)(USB_AUDIO_INFO *audio_info);
int32_t wifiusb_audio_app_init(uint32_t submit_size,uint32_t xtal_time,USB_AUDIO_CALLBACK call_back);
void wifiusb_audio_open(void);
void wifiusb_audio_data_recv_handler(const struct USB_AUDIO_XFER_INFO_T *info);
void wifiusb_audio_cmd_recv_handler(void *param);
void wifiusb_audio_set_vol(uint8_t vol);
void wifiusb_audio_set_mute(uint8_t vol);
void wifiusb_audio_send_cmd(uint32_t data);
uint32_t wifiusb_audio_get_capture_buffer_size(void);
int32_t wifiusb_audio_data_capture_handler(uint8_t *buf, uint32_t len);
#ifdef __cplusplus
}
#endif

#endif

