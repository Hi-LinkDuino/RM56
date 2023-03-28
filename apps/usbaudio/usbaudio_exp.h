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
#ifndef __USBAUDIO_EXP_H__
#define __USBAUDIO_EXP_H__

enum UsbAudioClassCmd {
    USB_AUDIO_START_PLAY = 0,
    USB_AUDIO_STOP_PLAY,
    USB_AUDIO_START_CAPTURE,
    USB_AUDIO_STOP_CAPTURE,
    USB_AUDIO_SET_VOL,
    USB_AUDIO_SET_CAP_VOL,
    USB_AUDIO_MUTE_CTRL,
    USB_AUDIO_CAP_MUTE_CTRL,
    USB_AUDIO_USB_RESET,
    USB_AUDIO_USB_DISCONNECT,
    USB_AUDIO_USB_CONFIG,
    USB_AUDIO_USB_SLEEP,
    USB_AUDIO_USB_WAKEUP,
    USB_AUDIO_RECV_PAUSE,
    USB_AUDIO_RECV_CONTINUE,
    USB_AUDIO_SET_RECV_RATE,
    USB_AUDIO_SET_SEND_RATE,
    USB_AUDIO_RESET_CODEC,
    USB_AUDIO_NOISE_GATING,
    USB_AUDIO_NOISE_REDUCTION,
    USB_AUDIO_TUNE_SAMPLE_RATE,
    USB_AUDIO_QTY
};

enum UsbAudioNotifyType {
    USB_AUDIO_NOTIFY_CMD,
    USB_AUDIO_NOTIFY_DATA,
    USB_AUDIO_NOTIFY_TYPE_QTY,
};

typedef struct {
    enum UsbAudioClassCmd cmd;
    unsigned char seq;
    unsigned char arg;
} UsbAudioExpCmd;

typedef struct {
    unsigned char *buff;
    unsigned int len;
} UsbAudioExpData;

typedef struct {
    enum UsbAudioNotifyType type;
    union {
        UsbAudioExpCmd cmd;
        UsbAudioExpData data;
    } info;
} UsbAudioExpInfo;

typedef void (*UsbAudioExpCallback)(UsbAudioExpInfo *audio_info);

typedef struct {
    UsbAudioExpCallback notifier_call;
} UsbAudioExpNotify;

int UsbAudioRegisterClient(UsbAudioExpNotify *callback);
void UsbAudioDevInit(unsigned int recvInterval);
void UsbAudioHidPause(void);
unsigned int CaptureDataToUsbNormalMode(unsigned char *pData, unsigned int len);
unsigned int CaptureDataToUsbTestMode(unsigned char *pData, unsigned int len);
#endif
